/* 
   Copyright (C) 2003-2009 FreeIPMI Core Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.  
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#ifdef STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else /* !TIME_WITH_SYS_TIME */
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else /* !HAVE_SYS_TIME_H */
#include <time.h>
#endif /* !HAVE_SYS_TIME_H */
#endif	/* !TIME_WITH_SYS_TIME */

#include "freeipmi/api/ipmi-messaging-support-cmds-api.h"
#include "freeipmi/api/ipmi-device-global-cmds-api.h"
#include "freeipmi/cmds/ipmi-device-global-cmds.h"
#include "freeipmi/cmds/ipmi-messaging-support-cmds.h"
#include "freeipmi/spec/ipmi-authentication-type-spec.h"
#include "freeipmi/spec/ipmi-channel-spec.h"
#include "freeipmi/spec/ipmi-comp-code-spec.h"
#include "freeipmi/spec/ipmi-ipmb-lun-spec.h"
#include "freeipmi/spec/ipmi-netfn-spec.h"
#include "freeipmi/spec/ipmi-privilege-level-spec.h"

#include "ipmi-ctx.h"
#include "ipmi-trace-wrappers-api.h"

#include "freeipmi-portability.h"

int8_t 
ipmi_cmd_clear_message_flags (ipmi_ctx_t ctx,
                              uint8_t receive_message_queue,
                              uint8_t event_message_buffer,
                              uint8_t watchdog_pre_timeout_interrupt_flag,
                              uint8_t oem_0,
                              uint8_t oem_1,
                              uint8_t oem_2,
                              fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (ctx->type == IPMI_DEVICE_LAN
      || ctx->type == IPMI_DEVICE_LAN_2_0)
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_COMMAND_INVALID_FOR_SELECTED_INTERFACE);
      return (-1);
    }
  
  if (!IPMI_MESSAGE_FLAGS_VALID(receive_message_queue)
      || !IPMI_MESSAGE_FLAGS_VALID(event_message_buffer)
      || !IPMI_MESSAGE_FLAGS_VALID(watchdog_pre_timeout_interrupt_flag)
      || !IPMI_MESSAGE_FLAGS_VALID(oem_0)
      || !IPMI_MESSAGE_FLAGS_VALID(oem_1)
      || !IPMI_MESSAGE_FLAGS_VALID(oem_2)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs, 
                                    tmpl_cmd_clear_message_flags_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_clear_message_flags_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_clear_message_flags (receive_message_queue,
                                    event_message_buffer,
                                    watchdog_pre_timeout_interrupt_flag,
                                    oem_0,
                                    oem_1,
                                    oem_2,
                                    obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_message_flags (ipmi_ctx_t ctx,
                            fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (ctx->type == IPMI_DEVICE_LAN
      || ctx->type == IPMI_DEVICE_LAN_2_0)
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_COMMAND_INVALID_FOR_SELECTED_INTERFACE);
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_message_flags_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_message_flags_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_message_flags (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_enable_message_channel_receive (ipmi_ctx_t ctx,
                                         uint8_t channel_number,
                                         uint8_t channel_operation,
                                         fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (ctx->type == IPMI_DEVICE_LAN
      || ctx->type == IPMI_DEVICE_LAN_2_0)
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_COMMAND_INVALID_FOR_SELECTED_INTERFACE);
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_CHANNEL_OPERATION_VALID (channel_operation)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_enable_message_channel_receive_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_enable_message_channel_receive_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_enable_message_channel_receive (channel_number,
                                               channel_operation,
                                               obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_message (ipmi_ctx_t ctx,
                      fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (ctx->type == IPMI_DEVICE_LAN
      || ctx->type == IPMI_DEVICE_LAN_2_0)
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_COMMAND_INVALID_FOR_SELECTED_INTERFACE);
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_message_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_message_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_message (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_send_message (ipmi_ctx_t ctx,
                       uint8_t channel_number,
                       uint8_t message_authentication,
                       uint8_t message_encryption,
                       uint8_t tracking_operation,
                       uint8_t *message_data,
                       uint32_t message_data_len,
                       fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_SEND_MESSAGE_AUTHENTICATION_VALID(message_authentication)
      || !IPMI_SEND_MESSAGE_ENCRYPTION_VALID(message_encryption)
      || !IPMI_SEND_MESSAGE_TRACKING_VALID(tracking_operation)
      || !message_data
      || !message_data_len
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_send_message_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_send_message_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_send_message (channel_number,
                             message_authentication,
                             message_encryption,
                             tracking_operation,
                             message_data,
                             message_data_len,
                             obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_read_event_message_buffer (ipmi_ctx_t ctx,
                                    fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (ctx->type == IPMI_DEVICE_LAN
      || ctx->type == IPMI_DEVICE_LAN_2_0)
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_COMMAND_INVALID_FOR_SELECTED_INTERFACE);
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs, 
                                    tmpl_cmd_read_event_message_buffer_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_read_event_message_buffer_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_read_event_message_buffer (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t
ipmi_cmd_get_system_interface_capabilities (ipmi_ctx_t ctx,
                                            uint8_t system_interface,
                                            fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_SYSTEM_INTERFACE_VALID(system_interface)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_system_interface_capabilities_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_system_interface_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_interface_capabilities (system_interface,
                                                  obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t
ipmi_cmd_get_system_interface_capabilities_ssif (ipmi_ctx_t ctx,
                                                 fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_system_interface_capabilities_ssif_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_system_interface_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_interface_capabilities (IPMI_SYSTEM_INTERFACE_SSIF,
                                                  obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t
ipmi_cmd_get_system_interface_capabilities_kcs (ipmi_ctx_t ctx,
                                                fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx, 
                                    obj_cmd_rs,
                                    tmpl_cmd_get_system_interface_capabilities_kcs_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_system_interface_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_interface_capabilities (IPMI_SYSTEM_INTERFACE_KCS,
                                                  obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t
ipmi_cmd_get_bt_interface_capabilities (ipmi_ctx_t ctx,
                                        fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs, 
                                    tmpl_cmd_get_bt_interface_capabilities_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_bt_interface_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_bt_interface_capabilities (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}


int8_t 
ipmi_cmd_get_channel_authentication_capabilities (ipmi_ctx_t ctx, 
                                                  uint8_t channel_number,
                                                  uint8_t maximum_privilege_level,
						  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_PRIVILEGE_LEVEL_VALID(maximum_privilege_level)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx, 
                                    obj_cmd_rs, 
                                    tmpl_cmd_get_channel_authentication_capabilities_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_channel_authentication_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_channel_authentication_capabilities (channel_number,
                                                        maximum_privilege_level,
                                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_channel_authentication_capabilities_v20 (ipmi_ctx_t ctx, 
                                                      uint8_t channel_number,
                                                      uint8_t maximum_privilege_level,
                                                      uint8_t get_ipmi_v20_extended_data,
                                                      fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_PRIVILEGE_LEVEL_VALID(maximum_privilege_level)
      || !IPMI_GET_IPMI_DATA_VALID(get_ipmi_v20_extended_data)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_channel_authentication_capabilities_v20_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_channel_authentication_capabilities_v20_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_channel_authentication_capabilities_v20 (channel_number,
                                                            maximum_privilege_level,
                                                            get_ipmi_v20_extended_data,
                                                            obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
                       
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_session_challenge (ipmi_ctx_t ctx, 
                                uint8_t authentication_type,
                                char *user_name,
                                uint32_t user_name_len,
				fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_AUTHENTICATION_TYPE_VALID(authentication_type)
      || (user_name && user_name_len > IPMI_MAX_USER_NAME_LENGTH)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs, 
                                    tmpl_cmd_get_session_challenge_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_session_challenge_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_session_challenge (authentication_type, 
                                      user_name,
                                      user_name_len,
                                      obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }
  
  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_activate_session (ipmi_ctx_t ctx, 
                           uint8_t authentication_type,
                           uint8_t maximum_privilege_level,
                           uint8_t *challenge_string,
                           uint32_t challenge_string_len,
                           uint32_t initial_outbound_sequence_number,
			   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_AUTHENTICATION_TYPE_VALID(authentication_type)
      || !IPMI_PRIVILEGE_LEVEL_VALID(maximum_privilege_level)
      || !challenge_string
      || (challenge_string_len > IPMI_CHALLENGE_STRING_LENGTH)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_activate_session_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_activate_session_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_activate_session (authentication_type,
                                 maximum_privilege_level,
                                 challenge_string,
                                 challenge_string_len,
                                 initial_outbound_sequence_number,
                                 obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }
  
  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_set_session_privilege_level (ipmi_ctx_t ctx, 
                                      uint8_t privilege_level,
				      fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_PRIVILEGE_LEVEL_VALID(privilege_level)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_session_privilege_level_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_session_privilege_level_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_session_privilege_level (privilege_level, 
                                            obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0) 
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
  
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_close_session (ipmi_ctx_t ctx, 
                        uint32_t close_session_id,
			fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_close_session_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_close_session_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_close_session (close_session_id, obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_APP_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
  
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_set_channel_access (ipmi_ctx_t ctx, 
			     uint8_t channel_number, 
			     uint8_t ipmi_messaging_access_mode, 
			     uint8_t user_level_authentication, 
			     uint8_t per_message_authentication, 
			     uint8_t pef_alerting, 
			     uint8_t channel_access_set, 
			     uint8_t channel_privilege_level_limit, 
			     uint8_t channel_privilege_level_limit_set, 
			     fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_MESSAGING_ACCESS_MODE_VALID(ipmi_messaging_access_mode)
      || !IPMI_USER_LEVEL_AUTHENTICATION_VALID(user_level_authentication)
      || !IPMI_PER_MESSAGE_AUTHENTICATION_VALID(per_message_authentication)
      || !IPMI_PEF_ALERTING_VALID(pef_alerting)
      || !IPMI_CHANNEL_ACCESS_VALID(channel_access_set)
      || !IPMI_PRIVILEGE_LEVEL_LIMIT_VALID(channel_privilege_level_limit)
      || !IPMI_PRIVILEGE_LEVEL_LIMIT_SET_VALID(channel_privilege_level_limit_set)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_channel_access_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_channel_access_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_channel_access (channel_number, 
                                   ipmi_messaging_access_mode, 
                                   user_level_authentication, 
                                   per_message_authentication, 
                                   pef_alerting, 
                                   channel_access_set, 
                                   channel_privilege_level_limit, 
                                   channel_privilege_level_limit_set,
                                   obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_channel_access (ipmi_ctx_t ctx, 
			     uint8_t channel_number,
			     uint8_t channel_access_get,
			     fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_CHANNEL_ACCESS_GET_VALID(channel_access_get)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_channel_access_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_channel_access_rq)))

    if (fill_cmd_get_channel_access (channel_number, 
                                     channel_access_get,
                                     obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_channel_info (ipmi_ctx_t ctx, 
			   uint8_t channel_number,
			   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_channel_info_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_channel_info_rq)))

    if (fill_cmd_get_channel_info (channel_number, obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_APP_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t
ipmi_cmd_set_channel_security_keys (ipmi_ctx_t ctx,
                                    uint8_t channel_number,
                                    uint8_t operation,
                                    uint8_t key_id,
                                    uint8_t *key_value,
                                    uint32_t key_value_len,
                                    fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_CHANNEL_SECURITY_KEYS_OPERATION_VALID(operation)
      || !IPMI_CHANNEL_SECURITY_KEYS_KEY_ID_VALID(key_id)
      || ((key_id == IPMI_CHANNEL_SECURITY_KEYS_KEY_ID_K_R
           && key_value)
          && key_value_len > IPMI_MAX_K_R_LENGTH)
      || ((key_id == IPMI_CHANNEL_SECURITY_KEYS_KEY_ID_K_G
           && key_value)
          && key_value_len > IPMI_MAX_K_G_LENGTH)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_channel_security_keys_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_channel_security_keys_rq)))

    if (fill_cmd_set_channel_security_keys (channel_number,
                                            operation,
                                            key_id,
                                            key_value,
                                            key_value_len,
                                            obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }
  
  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_set_user_access (ipmi_ctx_t ctx, 
			  uint8_t channel_number,
			  uint8_t user_ipmi_messaging,
			  uint8_t user_link_authentication,
			  uint8_t user_restricted_to_callback,
			  uint8_t user_id,
			  uint8_t user_privilege_level_limit,
			  uint8_t user_session_number_limit, 
			  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !IPMI_USER_IPMI_MESSAGING_VALID(user_ipmi_messaging)
      || !IPMI_USER_LINK_AUTHENTICATION_VALID(user_link_authentication)
      || !IPMI_USER_RESTRICTED_TO_CALLBACK_VALID(user_restricted_to_callback)
      || !IPMI_PRIVILEGE_LEVEL_LIMIT_VALID(user_privilege_level_limit)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_user_access_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_user_access_rq)))

    if (fill_cmd_set_user_access (channel_number,
                                  user_ipmi_messaging,
                                  user_link_authentication,
                                  user_restricted_to_callback,
                                  user_id,
                                  user_privilege_level_limit,
                                  user_session_number_limit,
                                  obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_user_access (ipmi_ctx_t ctx, 
			  uint8_t channel_number,
			  uint8_t user_id,
			  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_CHANNEL_NUMBER_VALID(channel_number)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_user_access_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_user_access_rq)))

    if (fill_cmd_get_user_access (channel_number, user_id, obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_set_user_name (ipmi_ctx_t ctx, 
			uint8_t user_id, 
			char *user_name, 
			unsigned int user_name_len,
			fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if ((user_name && user_name_len > IPMI_MAX_USER_NAME_LENGTH)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_user_name_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_user_name_rq)))

    if (fill_cmd_set_user_name (user_id, 
                                user_name, 
                                user_name_len,
                                obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_get_user_name (ipmi_ctx_t ctx, 
			uint8_t user_id, 
			fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_get_user_name_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_user_name_rq)))

    if (fill_cmd_get_user_name (user_id, obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_set_user_password (ipmi_ctx_t ctx, 
			    uint8_t user_id, 
			    uint8_t operation, 
			    char *password,
			    unsigned int password_len,
			    fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_PASSWORD_OPERATION_VALID(operation)
      || (password && password_len > IPMI_1_5_MAX_PASSWORD_LENGTH)
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_user_password_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_user_password_rq)))

    if (fill_cmd_set_user_password (user_id, 
                                    operation, 
                                    password, 
                                    password_len,
                                    obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
  
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t 
ipmi_cmd_set_user_password_v20 (ipmi_ctx_t ctx, 
                                uint8_t user_id, 
                                uint8_t password_size,
                                uint8_t operation, 
                                char *password,
                                unsigned int password_len,
                                fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_PASSWORD_OPERATION_VALID(operation)
      && !IPMI_PASSWORD_SIZE_VALID(password_size)
      && (password
          && password_size == IPMI_PASSWORD_SIZE_16_BYTES
          && password_len > IPMI_1_5_MAX_PASSWORD_LENGTH)
      && (password
          && password_size == IPMI_PASSWORD_SIZE_20_BYTES
          && password_len > IPMI_2_0_MAX_PASSWORD_LENGTH)
      && !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_user_password_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_user_password_v20_rq)))
  
    if (fill_cmd_set_user_password_v20 (user_id, 
                                        password_size,
                                        operation, 
                                        password, 
                                        password_len,
                                        obj_cmd_rq) < 0)
      {
        API_ERRNO_TO_API_ERRNUM(ctx, errno);
        goto cleanup;
      }

  if (api_ipmi_cmd (ctx, 
                    IPMI_BMC_IPMB_LUN_BMC, 
                    IPMI_NET_FN_APP_RQ, 
                    obj_cmd_rq, 
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      goto cleanup;
    }
  
  rv = 0;
 cleanup:
  FIID_OBJ_DESTROY(obj_cmd_rq);
  return (rv);
}

int8_t
ipmi_get_channel_number (ipmi_ctx_t ctx, uint8_t channel_medium_type)
{
  fiid_obj_t obj_data_rs = NULL;
  uint64_t manufacturer_id, product_id;
  int8_t rv = -1;
  uint64_t val;
  int i;
    
  /* XXX channel medium type check? - OEM channels 0-0xFF possible, so skip */
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (channel_medium_type == IPMI_CHANNEL_MEDIUM_TYPE_LAN_802_3)
    {
      if (!(obj_data_rs = fiid_obj_create(tmpl_cmd_get_device_id_rs)))
        {
          API_ERRNO_TO_API_ERRNUM(ctx, errno);
          goto cleanup;
        }
      
      if (ipmi_cmd_get_device_id (ctx, obj_data_rs) < 0)
	goto cleanup;
      
      if (api_fiid_obj_get(ctx,
                           obj_data_rs,
                           "manufacturer_id.id",
                           &manufacturer_id) < 0)
        {
          ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
          goto cleanup;
        }

      if (api_fiid_obj_get(ctx,
                           obj_data_rs,
                           "product_id",
                           &product_id) < 0)
        {
          ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
          goto cleanup;
        }
      
      switch (manufacturer_id)
	{
	case IPMI_MANUFACTURER_ID_INTEL:
	case 0xB000157: /* Intel */
	  switch (product_id)
	    {
	    case IPMI_PRODUCT_ID_SE7501WV2:
	      rv = 7;
	      goto cleanup;
	    }
	}

      FIID_OBJ_DESTROY(obj_data_rs);
    }
  
  if (!(obj_data_rs = fiid_obj_create(tmpl_cmd_get_channel_info_rs)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }
  
  /* Channel numbers range from 0 - 7 */
  for (i = 0; i < 8; i++)
    {
      if (ipmi_cmd_get_channel_info (ctx, i, obj_data_rs) != 0)
	continue;
	
      if (api_fiid_obj_get(ctx,
                           obj_data_rs,
                           "channel_medium_type",
                           &val) < 0)
        {
          ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
          goto cleanup;
        }
      
      if ((uint8_t) val == channel_medium_type)
	{
	  if (api_fiid_obj_get(ctx,
                               obj_data_rs,
                               "actual_channel_number",
                               &val) < 0)
            {
              ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
              goto cleanup;
            }
	  
	  rv = (int8_t) val;
	  break;
	}
    }

  if (rv < 0)
    API_SET_ERRNUM(ctx, IPMI_ERR_NOT_FOUND);
 cleanup:
  FIID_OBJ_DESTROY(obj_data_rs);
  return (rv);
}

