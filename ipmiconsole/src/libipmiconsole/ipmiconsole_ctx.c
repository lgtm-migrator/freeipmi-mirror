/*****************************************************************************\
 *  $Id: ipmiconsole_ctx.c,v 1.21 2007-09-27 20:27:36 chu11 Exp $
 *****************************************************************************
 *  Copyright (C) 2006 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Albert Chu <chu11@llnl.gov>
 *  UCRL-CODE-221226
 *  
 *  This file is part of Ipmiconsole, a set of IPMI 2.0 SOL libraries
 *  and utilities.  For details, see http://www.llnl.gov/linux/.
 *  
 *  Ipmiconsole is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by the 
 *  Free Software Foundation; either version 2 of the License, or (at your 
 *  option) any later version.
 *  
 *  Ipmiconsole is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 *  for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with Ipmiconsole; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
\*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#if HAVE_PTHREAD_H
#include <pthread.h>
#endif /* HAVE_PTHREAD_H */
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else  /* !TIME_WITH_SYS_TIME */
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else /* !HAVE_SYS_TIME_H */
#include <time.h>
#endif  /* !HAVE_SYS_TIME_H */
#endif /* !TIME_WITH_SYS_TIME */
#include <assert.h>
#include <errno.h>

#include "ipmiconsole.h"
#include "ipmiconsole_defs.h"

#include "cbuf.h"
#include "list.h"
#include "secure.h"
#include "timeval.h"
#include "ipmiconsole_ctx.h"
#include "ipmiconsole_debug.h"
#include "ipmiconsole_fiid_wrappers.h"
#include "ipmiconsole_util.h"

#define GETHOSTBYNAME_AUX_BUFLEN 1024

extern List console_engine_ctxs_to_destroy;
extern pthread_mutex_t console_engine_ctxs_to_destroy_mutex;

int
ipmiconsole_ctx_setup(ipmiconsole_ctx_t c)
{
  int perr;

  assert(c);

  /* magic may not be set yet, no assert */

  memset(c, '\0', sizeof(struct ipmiconsole_ctx));
  c->magic = IPMICONSOLE_CTX_MAGIC;
  c->api_magic = IPMICONSOLE_CTX_API_MAGIC;

  if ((perr = pthread_mutex_init(&(c->errnum_mutex), NULL)) != 0)
    {
      errno = perr;
      return -1;
    }

  ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SUCCESS);

  return 0;
}

void
ipmiconsole_ctx_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  /* don't call ctx_set_errnum after the mutex_destroy */
  ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_CTX_INVALID);
  pthread_mutex_destroy(&(c->errnum_mutex));
  c->magic = ~IPMICONSOLE_CTX_MAGIC;
  c->api_magic = ~IPMICONSOLE_CTX_API_MAGIC;
  if (c->config.engine_flags & IPMICONSOLE_ENGINE_LOCK_MEMORY)
    secure_free(c, sizeof(struct ipmiconsole_ctx));
  else
    free(c);
}

/* Wrapper for list callback */
void 
ipmiconsole_ctx_list_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  ipmiconsole_ctx_config_cleanup(c);
  ipmiconsole_ctx_debug_cleanup(c);
  ipmiconsole_ctx_signal_cleanup(c);
  ipmiconsole_ctx_blocking_cleanup(c);
  ipmiconsole_ctx_cleanup(c);
}

int 
ipmiconsole_ctx_config_setup(ipmiconsole_ctx_t c,
                             char *hostname,
                             struct ipmiconsole_ipmi_config *ipmi_config,
                             struct ipmiconsole_protocol_config *protocol_config,
                             struct ipmiconsole_engine_config *engine_config)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  strcpy(c->config.hostname, hostname);

  if (ipmi_config->username)
    strcpy(c->config.username, ipmi_config->username);

  if (ipmi_config->password)
    strcpy(c->config.password, ipmi_config->password);

  /* k_g may contain nulls */
  if (ipmi_config->k_g && ipmi_config->k_g_len)
    {
      memcpy(c->config.k_g, ipmi_config->k_g, ipmi_config->k_g_len);
      c->config.k_g_len = ipmi_config->k_g_len;
    }

  if (ipmi_config->privilege_level >= 0)
    {
      if (ipmi_config->privilege_level == IPMICONSOLE_PRIVILEGE_USER)
        c->config.privilege_level = IPMI_PRIVILEGE_LEVEL_USER;
      else if (ipmi_config->privilege_level == IPMICONSOLE_PRIVILEGE_OPERATOR)
        c->config.privilege_level = IPMI_PRIVILEGE_LEVEL_OPERATOR;
      else
        c->config.privilege_level = IPMI_PRIVILEGE_LEVEL_ADMIN;
    }
  else
    c->config.privilege_level = IPMI_PRIVILEGE_LEVEL_DEFAULT;

  if (ipmi_config->cipher_suite_id >= IPMI_CIPHER_SUITE_ID_MIN)
    c->config.cipher_suite_id = ipmi_config->cipher_suite_id;
  else
    c->config.cipher_suite_id = IPMI_CIPHER_SUITE_ID_DEFAULT;

  c->config.workaround_flags = ipmi_config->workaround_flags;

  if (protocol_config->session_timeout_len > 0)
    c->config.session_timeout_len = protocol_config->session_timeout_len;
  else
    c->config.session_timeout_len = IPMICONSOLE_SESSION_TIMEOUT_LENGTH_DEFAULT;

  if (protocol_config->retransmission_timeout_len > 0)
    c->config.retransmission_timeout_len = protocol_config->retransmission_timeout_len;
  else
    c->config.retransmission_timeout_len = IPMICONSOLE_RETRANSMISSION_TIMEOUT_LENGTH_DEFAULT;

  if (protocol_config->retransmission_backoff_count > 0)
    c->config.retransmission_backoff_count = protocol_config->retransmission_backoff_count;
  else
    c->config.retransmission_backoff_count = IPMICONSOLE_RETRANSMISSION_BACKOFF_COUNT_DEFAULT;

  if (protocol_config->keepalive_timeout_len > 0)
    c->config.keepalive_timeout_len = protocol_config->keepalive_timeout_len;
  else
    c->config.keepalive_timeout_len = IPMICONSOLE_KEEPALIVE_TIMEOUT_LENGTH_DEFAULT;

  if (protocol_config->retransmission_keepalive_timeout_len > 0)
    c->config.retransmission_keepalive_timeout_len = protocol_config->retransmission_keepalive_timeout_len;
  else
    c->config.retransmission_keepalive_timeout_len = IPMICONSOLE_RETRANSMISSION_KEEPALIVE_TIMEOUT_LENGTH_DEFAULT;

  if (protocol_config->acceptable_packet_errors_count > 0)
    c->config.acceptable_packet_errors_count = protocol_config->acceptable_packet_errors_count;
  else
    c->config.acceptable_packet_errors_count = IPMICONSOLE_ACCEPTABLE_PACKET_ERRORS_COUNT_DEFAULT;

  if (protocol_config->maximum_retransmission_count > 0)
    c->config.maximum_retransmission_count = protocol_config->maximum_retransmission_count;
  else
    c->config.maximum_retransmission_count = IPMICONSOLE_MAXIMUM_RETRANSMISSION_COUNT_DEFAULT;

  c->config.engine_flags = engine_config->engine_flags;

  c->config.behavior_flags = engine_config->behavior_flags;

  c->config.debug_flags = engine_config->debug_flags;

  /* Data based on Configuration Parameters */

  if (ipmi_cipher_suite_id_to_algorithms(c->config.cipher_suite_id,
                                         &(c->config.authentication_algorithm),
                                         &(c->config.integrity_algorithm),
                                         &(c->config.confidentiality_algorithm)) < 0)
    {
      IPMICONSOLE_DEBUG(("ipmi_cipher_suite_id_to_algorithms: %s", strerror(errno)));
      return -1;
    }

  /* Retransmission timeout cannot be larger than the session timeout */
  if (c->config.retransmission_timeout_len > c->config.session_timeout_len)
    {
      errno = EINVAL;
      return -1;
    }

  /* Keepalive timeout cannot be larger than the session timeout */
  if (c->config.keepalive_timeout_len > c->config.session_timeout_len)
    {
      errno = EINVAL;
      return -1;
    }

  /* Retransmission timeout cannot be larger than the keepalive timeout */
  if (c->config.retransmission_timeout_len > c->config.keepalive_timeout_len)
    {
      errno = EINVAL;
      return -1;
    }

  /* Retransmission keepalive timeout cannot be larger than the keepalive timeout */
  if (c->config.retransmission_keepalive_timeout_len > c->config.keepalive_timeout_len)
    {
      errno = EINVAL;
      return -1;
    }

  return 0;
}

void
ipmiconsole_ctx_config_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);
  
  /* nothing now, reserve for later */
}

int
ipmiconsole_ctx_debug_setup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  if (c->config.debug_flags & IPMICONSOLE_DEBUG_FILE)
    {
      char filename[MAXPATHLEN];
      snprintf(filename,
               MAXPATHLEN,
               "%s/%s.%s",
               IPMICONSOLE_DEBUG_DIRECTORY,
               IPMICONSOLE_DEBUG_FILENAME,
               c->config.hostname);

      if ((c->debug.debug_fd = open(filename,
                                    O_CREAT | O_APPEND | O_WRONLY,
                                    0600)) < 0)
        {
          c->config.debug_flags &= ~IPMICONSOLE_DEBUG_FILE;
          IPMICONSOLE_CTX_DEBUG(c, ("open: %s", strerror(errno)));
          ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
          c->config.debug_flags = 0;
          return -1;
        }
    }

  return 0;
}

void
ipmiconsole_ctx_debug_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  if (c->config.debug_flags & IPMICONSOLE_DEBUG_FILE && c->debug.debug_fd >= 0)
    {
      close(c->debug.debug_fd);
      c->debug.debug_fd = -1;
    }
  c->config.debug_flags = 0;
}

int
ipmiconsole_ctx_signal_setup(ipmiconsole_ctx_t c)
{
  int perr;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  if ((perr = pthread_mutex_init(&c->signal.status_mutex, NULL)) != 0)
    {
      errno = perr;
      return -1;
    }
  c->signal.status = IPMICONSOLE_CTX_STATUS_NOT_SUBMITTED;

  if ((perr = pthread_mutex_init(&c->signal.destroyed_mutex, NULL)) != 0)
    {
      errno = perr;
      return -1;
    }
  c->signal.user_has_destroyed = 0;
  c->signal.moved_to_destroyed = 0;

  return 0;
}

void
ipmiconsole_ctx_signal_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  pthread_mutex_destroy(&(c->signal.status_mutex));
  pthread_mutex_destroy(&(c->signal.destroyed_mutex));
}

int
ipmiconsole_ctx_non_blocking_setup(ipmiconsole_ctx_t c,
                                   Ipmiconsole_callback callback,
                                   void *callback_arg)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  c->non_blocking.callback = callback;
  c->non_blocking.callback_arg = callback_arg;

  return 0;
}

int
ipmiconsole_ctx_blocking_setup(ipmiconsole_ctx_t c)
{
  int perr;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  if ((perr = pthread_mutex_init(&c->blocking.blocking_mutex, NULL)) != 0)
    {
      errno = perr;
      return -1;
    }
  c->blocking.blocking_submit_requested = 0;
  c->blocking.blocking_notification[0] = -1;
  c->blocking.blocking_notification[1] = -1;
  c->blocking.sol_session_established = 0;

  return 0;
}

void
ipmiconsole_ctx_blocking_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  pthread_mutex_destroy(&(c->blocking.blocking_mutex));
}

static void
_ipmiconsole_ctx_connection_init(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  memset(&(c->connection), '\0', sizeof(struct ipmiconsole_ctx_connection));
  c->connection.user_fd = -1;
  c->connection.ipmiconsole_fd = -1;
  c->connection.ipmi_fd = -1;
  c->connection.asynccomm[0] = -1;
  c->connection.asynccomm[1] = -1;
}

int 
ipmiconsole_ctx_connection_setup(ipmiconsole_ctx_t c)
{
  struct sockaddr_in srcaddr;
  int sv[2];
  int secure_malloc_flag;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);
  assert(!(c->session_submitted));

  _ipmiconsole_ctx_connection_init(c);

  /* File Descriptor User Interface */

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    {
      IPMICONSOLE_DEBUG(("socketpair: %s", strerror(errno)));
      if (errno == EMFILE)
        ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_TOO_MANY_OPEN_FILES);
      else
        ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
      goto cleanup;
    }
  c->connection.user_fd = sv[0];
  c->connection.ipmiconsole_fd = sv[1];

  if (ipmiconsole_set_closeonexec(c, c->connection.user_fd) < 0)
    {
      IPMICONSOLE_DEBUG(("closeonexec error"));
      goto cleanup;
    }
  if (ipmiconsole_set_closeonexec(c, c->connection.ipmiconsole_fd) < 0)
    {
      IPMICONSOLE_DEBUG(("closeonexec error"));
      goto cleanup;
    }

  /* Copy for API level */
  c->fds.user_fd = c->connection.user_fd;

  secure_malloc_flag = (c->config.engine_flags & IPMICONSOLE_ENGINE_LOCK_MEMORY) ? 1 : 0;

  if (!(c->connection.console_remote_console_to_bmc = cbuf_create(CONSOLE_REMOTE_CONSOLE_TO_BMC_BUF_MIN, CONSOLE_REMOTE_CONSOLE_TO_BMC_BUF_MAX, secure_malloc_flag)))
    {
      IPMICONSOLE_DEBUG(("cbuf_create: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_OUT_OF_MEMORY);
      goto cleanup;
    }

  if (!(c->connection.console_bmc_to_remote_console = cbuf_create(CONSOLE_BMC_TO_REMOTE_CONSOLE_BUF_MIN, CONSOLE_BMC_TO_REMOTE_CONSOLE_BUF_MAX, secure_malloc_flag)))
    {
      IPMICONSOLE_DEBUG(("cbuf_create: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_OUT_OF_MEMORY);
      goto cleanup;
    }

  /* Connection Data */

  if ((c->connection.ipmi_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      IPMICONSOLE_DEBUG(("socket: %s", strerror(errno)));
      if (errno == EMFILE)
        ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_TOO_MANY_OPEN_FILES);
      else
        ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
      goto cleanup;
    }

  if (ipmiconsole_set_closeonexec(c, c->connection.ipmi_fd) < 0)
    {
      IPMICONSOLE_DEBUG(("closeonexec error"));
      goto cleanup;
    }

  memset(&srcaddr, '\0', sizeof(struct sockaddr_in));
  srcaddr.sin_family = AF_INET;
  srcaddr.sin_port = htons(0);
  srcaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(c->connection.ipmi_fd, (struct sockaddr *)&srcaddr, sizeof(struct sockaddr_in)) < 0)
    {
      IPMICONSOLE_DEBUG(("bind: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
      goto cleanup;
    }

  if (!(c->connection.ipmi_from_bmc = cbuf_create(IPMI_FROM_BMC_BUF_MIN, IPMI_FROM_BMC_BUF_MAX, secure_malloc_flag)))
    {
      IPMICONSOLE_DEBUG(("cbuf_create: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_OUT_OF_MEMORY);
      goto cleanup;
    }

  if (!(c->connection.ipmi_to_bmc = cbuf_create(IPMI_TO_BMC_BUF_MIN, IPMI_TO_BMC_BUF_MAX, secure_malloc_flag)))
    {
      IPMICONSOLE_DEBUG(("cbuf_create: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_OUT_OF_MEMORY);
      goto cleanup;
    }

  /* Pipe for non-fd communication */
  if (pipe(c->connection.asynccomm) < 0)
    {
      IPMICONSOLE_DEBUG(("pipe: %s", strerror(errno)));
      if (errno == EMFILE)
        ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_TOO_MANY_OPEN_FILES);
      else
        ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
      goto cleanup;
    }

  if (ipmiconsole_set_closeonexec(c, c->connection.asynccomm[0]) < 0)
    {
      IPMICONSOLE_DEBUG(("closeonexec error"));
      goto cleanup;
    }

  if (ipmiconsole_set_closeonexec(c, c->connection.asynccomm[1]) < 0)
    {
      IPMICONSOLE_DEBUG(("closeonexec error"));
      goto cleanup;
    }

  /* Copy for API level */
  c->fds.asynccomm[0] = c->connection.asynccomm[0];
  c->fds.asynccomm[1] = c->connection.asynccomm[1];

  /* Fiid Objects */

  if (!(c->connection.obj_rmcp_hdr_rq = Fiid_obj_create(c, tmpl_rmcp_hdr)))
    goto cleanup;
  if (!(c->connection.obj_rmcp_hdr_rs = Fiid_obj_create(c, tmpl_rmcp_hdr)))
    goto cleanup;
  if (!(c->connection.obj_lan_session_hdr_rq = Fiid_obj_create(c, tmpl_lan_session_hdr)))
    goto cleanup;
  if (!(c->connection.obj_lan_session_hdr_rs = Fiid_obj_create(c, tmpl_lan_session_hdr)))
    goto cleanup;
  if (!(c->connection.obj_lan_msg_hdr_rq = Fiid_obj_create(c, tmpl_lan_msg_hdr_rq)))
    goto cleanup;
  if (!(c->connection.obj_lan_msg_hdr_rs = Fiid_obj_create(c, tmpl_lan_msg_hdr_rs)))
    goto cleanup;
  if (!(c->connection.obj_lan_msg_trlr_rs = Fiid_obj_create(c, tmpl_lan_msg_trlr)))
    goto cleanup;
  if (!(c->connection.obj_rmcpplus_session_hdr_rq = Fiid_obj_create(c, tmpl_rmcpplus_session_hdr)))
    goto cleanup;
  if (!(c->connection.obj_rmcpplus_session_hdr_rs = Fiid_obj_create(c, tmpl_rmcpplus_session_hdr)))
    goto cleanup;
  if (!(c->connection.obj_rmcpplus_payload_rs = Fiid_obj_create(c, tmpl_rmcpplus_payload)))
    goto cleanup;
  if (!(c->connection.obj_rmcpplus_session_trlr_rq = Fiid_obj_create(c, tmpl_rmcpplus_session_trlr)))
    goto cleanup;
  if (!(c->connection.obj_rmcpplus_session_trlr_rs = Fiid_obj_create(c, tmpl_rmcpplus_session_trlr)))
    goto cleanup;
  if (!(c->connection.obj_authentication_capabilities_v20_rq = Fiid_obj_create(c, tmpl_cmd_get_channel_authentication_capabilities_v20_rq)))
    goto cleanup;
  if (!(c->connection.obj_authentication_capabilities_v20_rs = Fiid_obj_create(c, tmpl_cmd_get_channel_authentication_capabilities_v20_rs)))
    goto cleanup;
  if (!(c->connection.obj_open_session_request = Fiid_obj_create(c, tmpl_rmcpplus_open_session_request)))
    goto cleanup;
  if (!(c->connection.obj_open_session_response = Fiid_obj_create(c, tmpl_rmcpplus_open_session_response)))
    goto cleanup;
  if (!(c->connection.obj_rakp_message_1 = Fiid_obj_create(c, tmpl_rmcpplus_rakp_message_1)))
    goto cleanup;
  if (!(c->connection.obj_rakp_message_2 = Fiid_obj_create(c, tmpl_rmcpplus_rakp_message_2)))
    goto cleanup;
  if (!(c->connection.obj_rakp_message_3 = Fiid_obj_create(c, tmpl_rmcpplus_rakp_message_3)))
    goto cleanup;
  if (!(c->connection.obj_rakp_message_4 = Fiid_obj_create(c, tmpl_rmcpplus_rakp_message_4)))
    goto cleanup;
  if (!(c->connection.obj_set_session_privilege_level_rq = Fiid_obj_create(c, tmpl_cmd_set_session_privilege_level_rq)))
    goto cleanup;
  if (!(c->connection.obj_set_session_privilege_level_rs = Fiid_obj_create(c, tmpl_cmd_set_session_privilege_level_rs)))
    goto cleanup;
  if (!(c->connection.obj_get_channel_payload_support_rq = Fiid_obj_create(c, tmpl_cmd_get_channel_payload_support_rq)))
    goto cleanup;
  if (!(c->connection.obj_get_channel_payload_support_rs = Fiid_obj_create(c, tmpl_cmd_get_channel_payload_support_rs)))
    goto cleanup;
  if (!(c->connection.obj_get_payload_activation_status_rq = Fiid_obj_create(c, tmpl_cmd_get_payload_activation_status_rq)))
    goto cleanup;
  if (!(c->connection.obj_get_payload_activation_status_rs = Fiid_obj_create(c, tmpl_cmd_get_payload_activation_status_rs)))
    goto cleanup;
  if (!(c->connection.obj_activate_payload_rq = Fiid_obj_create(c, tmpl_cmd_activate_payload_sol_rq)))
    goto cleanup;
  if (!(c->connection.obj_activate_payload_rs = Fiid_obj_create(c, tmpl_cmd_activate_payload_sol_rs)))
    goto cleanup;
  if (!(c->connection.obj_sol_payload_data_rq = Fiid_obj_create(c, tmpl_sol_payload_data_remote_console_to_bmc)))
    goto cleanup;
  if (!(c->connection.obj_sol_payload_data_rs = Fiid_obj_create(c, tmpl_sol_payload_data_bmc_to_remote_console)))
    goto cleanup;
  if (!(c->connection.obj_get_channel_payload_version_rq = Fiid_obj_create(c, tmpl_cmd_get_channel_payload_version_rq)))
    goto cleanup;
  if (!(c->connection.obj_get_channel_payload_version_rs = Fiid_obj_create(c, tmpl_cmd_get_channel_payload_version_rs)))
    goto cleanup;
  if (!(c->connection.obj_deactivate_payload_rq = Fiid_obj_create(c, tmpl_cmd_deactivate_payload_rq)))
    goto cleanup;
  if (!(c->connection.obj_deactivate_payload_rs = Fiid_obj_create(c, tmpl_cmd_deactivate_payload_rs)))
    goto cleanup;
  if (!(c->connection.obj_close_session_rq = Fiid_obj_create(c, tmpl_cmd_close_session_rq)))
    goto cleanup;
  if (!(c->connection.obj_close_session_rs = Fiid_obj_create(c, tmpl_cmd_close_session_rs)))
    goto cleanup;

  return 0;

 cleanup:
  ipmiconsole_ctx_connection_cleanup(c);
  /* Previously called here, but this is now supposed to be handled in API land */
  /* _ipmiconsole_ctx_fds_cleanup(c); */
  /* _ipmiconsole_ctx_fds_setup(c); */
  return -1;
}

void
ipmiconsole_ctx_connection_cleanup(ipmiconsole_ctx_t c)
{
  int blocking_requested = 0;
  int status_initial = 0;
  int secure_malloc_flag;
  int perr;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);
  
  secure_malloc_flag = (c->config.engine_flags & IPMICONSOLE_ENGINE_LOCK_MEMORY) ? 1 : 0;

  /* We have to cleanup, so in general continue on even if locking fails */

  if ((perr = pthread_mutex_lock(&(c->signal.status_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_lock: %s", strerror(perr)));
  
  /* Don't change status if it's already been set before */
  if (c->signal.status != IPMICONSOLE_CTX_STATUS_SOL_ESTABLISHED)
    {
      c->signal.status = IPMICONSOLE_CTX_STATUS_SOL_ERROR;
      status_initial++;
    }
  
  if ((perr = pthread_mutex_unlock(&(c->signal.status_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_unlock: %s", strerror(perr)));

  if ((perr = pthread_mutex_lock(&(c->blocking.blocking_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_lock: %s", strerror(perr)));

  if (c->blocking.blocking_submit_requested
      && !c->blocking.sol_session_established)
    {
      uint8_t val;

      blocking_requested++;

      if (c->config.behavior_flags & IPMICONSOLE_BEHAVIOR_DEACTIVATE_ONLY
          && c->session.deactivate_only_succeeded_flag)
        val = IPMICONSOLE_BLOCKING_NOTIFICATION_SOL_SESSION_DEACTIVATED;
      else
        val = IPMICONSOLE_BLOCKING_NOTIFICATION_SOL_SESSION_ERROR;

      if (write(c->blocking.blocking_notification[1], &val, 1) < 0)
        IPMICONSOLE_CTX_DEBUG(c, ("write: %s", strerror(errno)));
    }

  if ((perr = pthread_mutex_unlock(&(c->blocking.blocking_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_unlock: %s", strerror(perr)));

  /* only call the callback if it's an initial SOL error and blocking
   * was not requested 
   */
  if (status_initial && !blocking_requested && c->non_blocking.callback)
    (*(c->non_blocking.callback))(c->non_blocking.callback_arg);

  /* Under default circumstances, close only the ipmiconsole_fd so
   * that an error will be detected by the user via a EOF on a read()
   * or EPIPE on a write() when reading/writing on their file
   * descriptor.
   *
   * On error situations (i.e. ipmiconsole_engine_submit() doesn't
   * return to the user w/ success), it is the responsibility of other
   * code to call _ipmiconsole_ctx_api_managed_session_data_cleanup().
   *
   * The exception to this is when the user specifies the
   * IPMICONSOLE_ENGINE_CLOSE_FD flag.  Then we close it here
   */
  if (c->config.engine_flags & IPMICONSOLE_ENGINE_CLOSE_FD)
    {
      if (c->connection.user_fd >= 0)
        close(c->connection.user_fd);
    }
  if (c->connection.ipmiconsole_fd >= 0)
    close(c->connection.ipmiconsole_fd);
  if (c->connection.console_remote_console_to_bmc)
    cbuf_destroy(c->connection.console_remote_console_to_bmc, secure_malloc_flag);
  if (c->connection.console_bmc_to_remote_console)
    cbuf_destroy(c->connection.console_bmc_to_remote_console, secure_malloc_flag);
  if (c->connection.ipmi_fd >= 0)
    close(c->connection.ipmi_fd);
  if (c->connection.ipmi_from_bmc)
    cbuf_destroy(c->connection.ipmi_from_bmc, secure_malloc_flag);
  if (c->connection.ipmi_to_bmc)
    cbuf_destroy(c->connection.ipmi_to_bmc, secure_malloc_flag);
  /* Similarly to the user_fd above, it is the responsibility of other
   * code to close asynccomm[0] and asynccomm[1], which is replicated
   * in the context.
   */
  if (c->connection.obj_rmcp_hdr_rq)
    Fiid_obj_destroy(c, c->connection.obj_rmcp_hdr_rq);
  if (c->connection.obj_rmcp_hdr_rs)
    Fiid_obj_destroy(c, c->connection.obj_rmcp_hdr_rs);
  if (c->connection.obj_lan_session_hdr_rq)
    Fiid_obj_destroy(c, c->connection.obj_lan_session_hdr_rq);
  if (c->connection.obj_lan_session_hdr_rs)
    Fiid_obj_destroy(c, c->connection.obj_lan_session_hdr_rs);
  if (c->connection.obj_lan_msg_hdr_rq)
    Fiid_obj_destroy(c, c->connection.obj_lan_msg_hdr_rq);
  if (c->connection.obj_lan_msg_hdr_rs)
    Fiid_obj_destroy(c, c->connection.obj_lan_msg_hdr_rs);
  if (c->connection.obj_lan_msg_trlr_rs)
    Fiid_obj_destroy(c, c->connection.obj_lan_msg_trlr_rs);
  if (c->connection.obj_rmcpplus_session_hdr_rq)
    Fiid_obj_destroy(c, c->connection.obj_rmcpplus_session_hdr_rq);
  if (c->connection.obj_rmcpplus_session_hdr_rs)
    Fiid_obj_destroy(c, c->connection.obj_rmcpplus_session_hdr_rs);
  if (c->connection.obj_rmcpplus_payload_rs)
    Fiid_obj_destroy(c, c->connection.obj_rmcpplus_payload_rs);
  if (c->connection.obj_rmcpplus_session_trlr_rq)
    Fiid_obj_destroy(c, c->connection.obj_rmcpplus_session_trlr_rq);
  if (c->connection.obj_rmcpplus_session_trlr_rs)
    Fiid_obj_destroy(c, c->connection.obj_rmcpplus_session_trlr_rs);
  if (c->connection.obj_authentication_capabilities_v20_rq)
    Fiid_obj_destroy(c, c->connection.obj_authentication_capabilities_v20_rq);
  if (c->connection.obj_authentication_capabilities_v20_rs)
    Fiid_obj_destroy(c, c->connection.obj_authentication_capabilities_v20_rs);
  if (c->connection.obj_open_session_request)
    Fiid_obj_destroy(c, c->connection.obj_open_session_request);
  if (c->connection.obj_open_session_response)
    Fiid_obj_destroy(c, c->connection.obj_open_session_response);
  if (c->connection.obj_rakp_message_1)
    Fiid_obj_destroy(c, c->connection.obj_rakp_message_1);
  if (c->connection.obj_rakp_message_2)
    Fiid_obj_destroy(c, c->connection.obj_rakp_message_2);
  if (c->connection.obj_rakp_message_3)
    Fiid_obj_destroy(c, c->connection.obj_rakp_message_3);
  if (c->connection.obj_rakp_message_4)
    Fiid_obj_destroy(c, c->connection.obj_rakp_message_4);
  if (c->connection.obj_set_session_privilege_level_rq)
    Fiid_obj_destroy(c, c->connection.obj_set_session_privilege_level_rq);
  if (c->connection.obj_set_session_privilege_level_rs)
    Fiid_obj_destroy(c, c->connection.obj_set_session_privilege_level_rs);
  if (c->connection.obj_get_channel_payload_support_rq)
    Fiid_obj_destroy(c, c->connection.obj_get_channel_payload_support_rq);
  if (c->connection.obj_get_channel_payload_support_rs)
    Fiid_obj_destroy(c, c->connection.obj_get_channel_payload_support_rs);
  if (c->connection.obj_get_payload_activation_status_rq)
    Fiid_obj_destroy(c, c->connection.obj_get_payload_activation_status_rq);
  if (c->connection.obj_get_payload_activation_status_rs)
    Fiid_obj_destroy(c, c->connection.obj_get_payload_activation_status_rs);
  if (c->connection.obj_activate_payload_rq)
    Fiid_obj_destroy(c, c->connection.obj_activate_payload_rq);
  if (c->connection.obj_activate_payload_rs)
    Fiid_obj_destroy(c, c->connection.obj_activate_payload_rs);
  if (c->connection.obj_sol_payload_data_rq)
    Fiid_obj_destroy(c, c->connection.obj_sol_payload_data_rq);
  if (c->connection.obj_sol_payload_data_rs)
    Fiid_obj_destroy(c, c->connection.obj_sol_payload_data_rs);
  if (c->connection.obj_get_channel_payload_version_rq)
    Fiid_obj_destroy(c, c->connection.obj_get_channel_payload_version_rq);
  if (c->connection.obj_get_channel_payload_version_rs)
    Fiid_obj_destroy(c, c->connection.obj_get_channel_payload_version_rs);
  if (c->connection.obj_deactivate_payload_rq)
    Fiid_obj_destroy(c, c->connection.obj_deactivate_payload_rq);
  if (c->connection.obj_deactivate_payload_rs)
    Fiid_obj_destroy(c, c->connection.obj_deactivate_payload_rs);
  if (c->connection.obj_close_session_rq)
    Fiid_obj_destroy(c, c->connection.obj_close_session_rq);
  if (c->connection.obj_close_session_rs)
    Fiid_obj_destroy(c, c->connection.obj_close_session_rs);
  
  _ipmiconsole_ctx_connection_init(c);

  /* Be careful, if the user requested to destroy the context, we can
   * destroy it here.  But if we destroy it, there is no mutex to
   * unlock.
   */

  /* Note: the code in _ipmiconsole_ctx_connection_cleanup() and
   * ipmiconsole_garbage_collector() may look like it may race and
   * could deadlock.  (ABBA and BAAB deadlock situation).  However,
   * the context mutexes c->destroyed_mutex are accessed from two
   * different lists.  So the c->destroyed_mutex context can never be
   * raced against in these two functions.
   */
  if ((perr = pthread_mutex_lock(&(c->signal.destroyed_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_lock: %s", strerror(perr)));

  if (c->signal.user_has_destroyed)
    {
      ipmiconsole_ctx_debug_cleanup(c);
      ipmiconsole_ctx_signal_cleanup(c);
      ipmiconsole_ctx_blocking_cleanup(c);
      ipmiconsole_ctx_cleanup(c);
    }
  else
    {
      if (!c->signal.moved_to_destroyed)
        {
          void *ptr;

          c->signal.moved_to_destroyed++;

          /* I suppose if we fail here, we mem-leak?? Log for now ... */
          
          if ((perr = pthread_mutex_lock(&(console_engine_ctxs_to_destroy_mutex))) != 0)
            IPMICONSOLE_DEBUG(("pthread_mutex_lock: %s", strerror(perr)));

          if (!(ptr = list_append(console_engine_ctxs_to_destroy, c)))
            IPMICONSOLE_DEBUG(("list_append: %s", strerror(errno)));
          
          if (ptr != (void *)c)
            IPMICONSOLE_DEBUG(("list_append: invalid pointer: ptr=%p; c=%p", ptr, c));

          if ((perr = pthread_mutex_unlock(&(console_engine_ctxs_to_destroy_mutex))) != 0)
            IPMICONSOLE_DEBUG(("pthread_mutex_unlock: %s", strerror(perr)));
        }
      
      if ((perr = pthread_mutex_unlock(&(c->signal.destroyed_mutex))) != 0)
        IPMICONSOLE_DEBUG(("pthread_mutex_unlock: %s", strerror(perr)));
    }
}

int
ipmiconsole_ctx_session_setup(ipmiconsole_ctx_t c)
{
#ifdef HAVE_FUNC_GETHOSTBYNAME_R_6
  struct hostent hent;
  int h_errnop;
  char buf[GETHOSTBYNAME_AUX_BUFLEN];
#endif /* HAVE_FUNC_GETHOSTBYNAME_R_6 */
  struct hostent *hptr;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  /* Note: May be modified later based on results from Activate
   * Payload packet received
   */
  c->session.console_port = RMCP_PRIMARY_RMCP_PORT;

  memset(&(c->session.addr), '\0', sizeof(struct sockaddr_in));
  c->session.addr.sin_family = AF_INET;
  c->session.addr.sin_port = htons(c->session.console_port);

  timeval_clear(&(c->session.last_ipmi_packet_sent));
  /* Note:
   * Initial last_ipmi_packet_received to current time, so session
   * timeout can be calculated in the beginning if necessary.
   */
  if (gettimeofday(&(c->session.last_ipmi_packet_received), NULL) < 0)
    {
      IPMICONSOLE_DEBUG(("gettimeofday: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
      return -1;
    }  

#ifdef HAVE_FUNC_GETHOSTBYNAME_R_6
  memset(&hent, '\0', sizeof(struct hostent));
  if (gethostbyname_r(c->config.hostname,
                      &hent,
                      buf,
                      GETHOSTBYNAME_AUX_BUFLEN,
                      &hptr,
                      &h_errnop) != 0)
    {
      if (h_errnop == HOST_NOT_FOUND
          || h_errnop == NO_ADDRESS
          || h_errnop == NO_DATA)
        {
          ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_HOSTNAME_INVALID);
          return -1;
        }
      IPMICONSOLE_DEBUG(("gethostbyname_r: %s", hstrerror(h_errnop)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_SYSTEM_ERROR);
      return -1;
    }

  if (!hptr)
    {
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_HOSTNAME_INVALID);
      return -1;
    }
#else /* !HAVE_FUNC_GETHOSTBYNAME_R */
#error Additional threadsafe gethostbyname support needed
#endif /* !HAVE_FUNC_GETHOSTBYNAME_R */

  c->session.addr.sin_addr = *((struct in_addr *)hptr->h_addr);

  c->session.protocol_state = IPMICONSOLE_PROTOCOL_STATE_START;
  c->session.close_session_flag = 0;
  c->session.try_new_port_flag = 0;
  c->session.deactivate_payload_instances_and_try_again_flag = 0;
  c->session.close_timeout_flag = 0;
  c->session.deactivate_only_succeeded_flag = 0;

  c->session.retransmission_count = 0;
  c->session.errors_count = 0;
  c->session.session_sequence_number_errors_count = 0;
  c->session.deactivate_active_payloads_count = 0;
  c->session.highest_received_sequence_number = 0; /* so first packet received will be > 0 */
  c->session.previously_received_list = IPMI_SESSION_SEQUENCE_NUMBER_PREVIOUSLY_RECEIVED_LIST_INIT;

  if (ipmi_get_random(&(c->session.message_tag),
                      sizeof(c->session.message_tag)) < 0)
    {
      IPMICONSOLE_DEBUG(("ipmi_get_random: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_INTERNAL_ERROR);
      return -1;
    }
  if (ipmi_get_random(&(c->session.requester_sequence_number),
                      sizeof(c->session.requester_sequence_number)) < 0)
    {
      IPMICONSOLE_DEBUG(("ipmi_get_random: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_INTERNAL_ERROR);
      return -1;
    }
  c->session.requester_sequence_number %= (IPMI_LAN_REQUESTER_SEQUENCE_NUMBER_MAX + 1);

  c->session.session_sequence_number = 0; /* 0, so initial increment puts it at 1 */
  c->session.name_only_lookup = IPMI_NAME_ONLY_LOOKUP;

  /* In IPMI 2.0, session_ids of 0 are special */
  do
    {
      if (ipmi_get_random((uint8_t *)&(c->session.remote_console_session_id),
                          sizeof(c->session.remote_console_session_id)) < 0)
        {
          IPMICONSOLE_DEBUG(("ipmi_get_random: %s", strerror(errno)));
          ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_INTERNAL_ERROR);
          return -1;
        }
    } while (!c->session.remote_console_session_id);

  if (ipmi_get_random(c->session.remote_console_random_number,
                      IPMI_REMOTE_CONSOLE_RANDOM_NUMBER_LENGTH) < 0)
    {
      IPMICONSOLE_DEBUG(("ipmi_get_random: %s", strerror(errno)));
      ipmiconsole_ctx_set_errnum(c, IPMICONSOLE_ERR_INTERNAL_ERROR);
      return -1;
    }

  /* Keys and ptrs will be calculated during session setup.  We just
   * memet/clear here.
   */
  memset(c->session.sik_key, '\0', IPMI_MAX_SIK_KEY_LENGTH);
  c->session.sik_key_ptr = c->session.sik_key;
  c->session.sik_key_len = IPMI_MAX_SIK_KEY_LENGTH;
  memset(c->session.integrity_key, '\0', IPMI_MAX_INTEGRITY_KEY_LENGTH);
  c->session.integrity_key_ptr = c->session.integrity_key;
  c->session.integrity_key_len = IPMI_MAX_INTEGRITY_KEY_LENGTH;
  memset(c->session.confidentiality_key, '\0', IPMI_MAX_CONFIDENTIALITY_KEY_LENGTH);
  c->session.confidentiality_key_ptr = c->session.confidentiality_key;
  c->session.confidentiality_key_len = IPMI_MAX_CONFIDENTIALITY_KEY_LENGTH;

  c->session.sol_payload_instance = IPMI_PAYLOAD_INSTANCE_DEFAULT;
  
  /* Following 3 will be calculated during session setup.  We only
   * memset/clear it here
   */
  c->session.sol_instance_capacity = 0;
  memset(c->session.sol_instances_activated, '\0', IPMI_INSTANCES_ACTIVATED_LENGTH);
  c->session.sol_instances_activated_count = 0;
  /* this is used just to index the number of instances deactivated */
  c->session.sol_instances_deactivated_count = 0;

  /* Calculated during the session setup. */
  c->session.max_sol_character_send_size = 0;
  
  /* SOL Session Maintenance */

  timeval_clear(&(c->session.last_keepalive_packet_sent));

  /* Serial Break Maintenance */
  c->session.break_requested = 0;
  c->session.console_remote_console_to_bmc_bytes_before_break = 0;

  /* SOL Input (remote console to BMC) */
  c->session.sol_input_waiting_for_ack = 0;
  c->session.sol_input_waiting_for_break_ack = 0;
  timeval_clear(&(c->session.last_sol_input_packet_sent));
  c->session.sol_input_packet_sequence_number = 0; /* 0, so initial increment puts it at 1 */
  memset(c->session.sol_input_character_data, '\0', IPMICONSOLE_MAX_CHARACTER_DATA+1);
  c->session.sol_input_character_data_len = 0;

  /* SOL Output (BMC to remote console) */
  c->session.last_sol_output_packet_sequence_number = 0;
  c->session.last_sol_output_accepted_character_count = 0;

  return 0;
}

void
ipmiconsole_ctx_fds_setup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  /* init to -1 b/c -1 isn't a legit fd */
  c->fds.user_fd = -1;
  c->fds.asynccomm[0] = -1;
  c->fds.asynccomm[1] = -1;
}

void
ipmiconsole_ctx_fds_cleanup(ipmiconsole_ctx_t c)
{
  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  /* Note: Close asynccomm[0] first, so an EBADFD error occurs in the
   * engine.  Closing asynccomm[1] first could result in a EPIPE
   * instead.
   */
  close(c->fds.user_fd);
  close(c->fds.asynccomm[0]);
  close(c->fds.asynccomm[1]);
  c->fds.user_fd = -1;
  c->fds.asynccomm[0] = -1;
  c->fds.asynccomm[1] = -1;
}

int
ipmiconsole_ctx_get_errnum(ipmiconsole_ctx_t c)
{
  int perr;
  int errnum;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  /* What do we do if a mutex lock/unlock fails here?  Ignore for
   * now. 
   */

  if ((perr = pthread_mutex_lock(&(c->errnum_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_lock: %s", strerror(perr)));

  errnum = c->errnum;
  c->errnum_retrieved++;
  
  if ((perr = pthread_mutex_unlock(&(c->errnum_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_unlock: %s", strerror(perr)));

  return errnum;
}

void 
ipmiconsole_ctx_set_errnum(ipmiconsole_ctx_t c, int errnum)
{
  int perr;

  assert(c);
  assert(c->magic == IPMICONSOLE_CTX_MAGIC);

  /* What do we do if a mutex lock/unlock fails here?  Ignore for
   * now. 
   */

  if ((perr = pthread_mutex_lock(&(c->errnum_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_lock: %s", strerror(perr)));

  /* If the errnum is ERR_SUCCESS, it is not required for the user
   * to retrieve it 
   */
  if (c->errnum_retrieved
      || c->errnum == IPMICONSOLE_ERR_SUCCESS)
    {
      c->errnum = errnum;
      if (errnum == IPMICONSOLE_ERR_SUCCESS)
        c->errnum_retrieved = 1;
      else
        c->errnum_retrieved = 0;
    }
  else
    IPMICONSOLE_DEBUG(("could not set errnum: current = %d, desired = %d", c->errnum, errnum));
  
  if ((perr = pthread_mutex_unlock(&(c->errnum_mutex))) != 0)
    IPMICONSOLE_DEBUG(("pthread_mutex_unlock: %s", strerror(perr)));
}
