/*
  Copyright (C) 2003-2009 FreeIPMI Core Team

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.
  
  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else /* !TIME_WITH_SYS_TIME */
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else /* !HAVE_SYS_TIME_H */
#include <time.h>
#endif /* !HAVE_SYS_TIME_H */
#endif /* !TIME_WITH_SYS_TIME */
#include <assert.h>

#include "freeipmi/api/ipmi-sel-cmds-api.h"
#include "freeipmi/cmds/ipmi-sel-cmds.h"

#include "ipmi-sel.h"
#include "ipmi-sel-argp.h"

#include "freeipmi-portability.h"
#include "pstdout.h"
#include "tool-common.h"
#include "tool-fiid-util.h"
#include "tool-cmdline-common.h"
#include "tool-hostrange-common.h"
#include "tool-sdr-cache-common.h"

#define IPMI_SEL_RECORD_SIZE   16
#define IPMI_SEL_OUTPUT_BUFLEN 1024

static int 
_display_sel_info (ipmi_sel_state_data_t *state_data)
{
  fiid_obj_t obj_cmd_rs = NULL;
  uint64_t val, val1, val2;
  char str[512];
  int rv = -1;
  time_t t;
  struct tm tm;
  
  assert(state_data);

  TOOL_FIID_OBJ_CREATE (obj_cmd_rs, tmpl_cmd_get_sel_info_rs);

  if (ipmi_cmd_get_sel_info (state_data->ipmi_ctx, obj_cmd_rs) < 0)
    {
      pstdout_fprintf(state_data->pstate,
                      stderr,
                      "ipmi_cmd_get_sel_info: %s\n",
                      ipmi_ctx_errormsg(state_data->ipmi_ctx));
      goto cleanup;
    }

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "sel_version_major", &val1);
  
  TOOL_FIID_OBJ_GET (obj_cmd_rs, "sel_version_minor", &val2);

  /* achu: ipmi version is BCD encoded, but major/minor are only 4 bits */
  pstdout_printf (state_data->pstate, 
                  "SEL version:                                      %d.%d\n", 
                  val1,
                  val2);

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "entries", &val);
  
  pstdout_printf (state_data->pstate, 
                  "Number of log entries:                            %d\n", 
                  val);

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "free_space", &val);

  pstdout_printf (state_data->pstate, 
                  "Free space remaining:                             %d bytes\n", 
                  val);
  
  TOOL_FIID_OBJ_GET (obj_cmd_rs, "most_recent_addition_timestamp", &val);
  
  t = val;
  localtime_r (&t, &tm);
  strftime (str, sizeof (str), "%m/%d/%Y - %H:%M:%S", &tm);
  pstdout_printf (state_data->pstate, 
                  "Recent addition timestamp:                        %s\n", 
                  str);
  
  TOOL_FIID_OBJ_GET (obj_cmd_rs, "most_recent_erase_timestamp", &val);

  t = val;
  localtime_r (&t, &tm);
  strftime (str, sizeof (str), "%m/%d/%Y - %H:%M:%S", &tm);
  pstdout_printf (state_data->pstate, 
                  "Recent erase timestamp:                           %s\n", 
                  str);
  
  TOOL_FIID_OBJ_GET (obj_cmd_rs, "get_sel_allocation_info_command_supported", &val);
  
  pstdout_printf (state_data->pstate, 
                  "Get SEL Allocation Information Command supported: %s\n", 
                  (val ? "Yes" : "No"));

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "reserve_sel_command_supported", &val);
  
  pstdout_printf (state_data->pstate, 
                  "Reserve SEL Command supported:                    %s\n", 
                  (val ? "Yes" : "No"));

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "partial_add_sel_entry_command_supported", &val);
  
  pstdout_printf (state_data->pstate, 
                  "Partial Add SEL Entry Command supported:          %s\n", 
                  (val ? "Yes" : "No"));

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "delete_sel_command_supported", &val);
  
  pstdout_printf (state_data->pstate, 
                  "Delete SEL Command supported:                     %s\n", 
                  (val ? "Yes" : "No"));

  TOOL_FIID_OBJ_GET (obj_cmd_rs, "overflow_flag", &val);

  pstdout_printf (state_data->pstate, 
                  "Events drop due to lack of space in SEL:          %s\n", 
                  (val ? "Yes" : "No"));
  
  rv = 0;
 cleanup:
  TOOL_FIID_OBJ_DESTROY(obj_cmd_rs);
  return (rv);
}

static int
_flush_cache (ipmi_sel_state_data_t *state_data)
{
  assert(state_data);

  if (sdr_cache_flush_cache(state_data->sdr_cache_ctx,
                            state_data->pstate,
                            state_data->prog_data->args->sdr.quiet_cache,
                            state_data->hostname,
                            state_data->prog_data->args->sdr.sdr_cache_directory) < 0)
    return -1;

  return 0;
}

static int
_clear_entries (ipmi_sel_state_data_t *state_data)
{
  int rv = -1;

  assert(state_data);

  if (ipmi_sel_parse_clear_sel (state_data->sel_parse_ctx) < 0)
    {
      pstdout_fprintf(state_data->pstate,
                      stderr,
                      "ipmi_sel_parse_clear_sel: %s\n",
                      ipmi_sel_parse_ctx_errormsg(state_data->sel_parse_ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  return rv;
}

static int
_delete_entry (ipmi_sel_state_data_t *state_data, 
               uint16_t record_id,
               int ignore_missing_sel_entries)
{
  int rv = -1;

  assert(state_data);
  assert(record_id);

  if (ipmi_sel_parse_delete_sel_entry (state_data->sel_parse_ctx,
                                       record_id) < 0)
    {
      if (!(ignore_missing_sel_entries
            && (ipmi_sel_parse_ctx_errnum(state_data->sel_parse_ctx) == IPMI_SEL_PARSE_ERR_NOT_FOUND)))
        {
          pstdout_fprintf(state_data->pstate,
                          stderr,
                          "ipmi_sel_parse_delete_sel_entry: %s\n",
                          ipmi_sel_parse_ctx_errormsg(state_data->sel_parse_ctx));
          goto cleanup;
        }
    }
  
  rv = 0;
 cleanup:
  return rv;
}

static int
_delete_records (ipmi_sel_state_data_t *state_data)
{
  struct ipmi_sel_arguments *args;
  int i;
  
  assert(state_data);
  
  args = state_data->prog_data->args;
  
  for (i = 0; i < args->delete_record_list_length; i++)
    {
      if (_delete_entry (state_data, 
                         args->delete_record_list[i],
                         0) < 0)
        return -1;
    }

  return 0;
}

static int
_delete_range (ipmi_sel_state_data_t *state_data)
{
  struct ipmi_sel_arguments *args;
  int i;
  
  assert(state_data);
  
  args = state_data->prog_data->args;
 
  for (i = args->delete_range1; i <= args->delete_range2; i++)
    {
      if (_delete_entry (state_data, i, 1) < 0)
        return -1;
    }
  
  return 0;
}

/* return -1, real error */
static int
_sel_parse_err_handle(ipmi_sel_state_data_t *state_data, char *func)
{
  assert(state_data);
  assert(func);

  if (ipmi_sel_parse_ctx_errnum(state_data->sel_parse_ctx) == IPMI_SEL_PARSE_ERR_INVALID_SEL_ENTRY)
    {
      /* maybe a bad SEL entry returned from remote system, don't error out */
      if (state_data->prog_data->args->common.debug)
        pstdout_fprintf(state_data->pstate,
                        stderr,
                        "Invalid SEL entry read\n");
      return 0;
    }

  pstdout_fprintf(state_data->pstate,
                  stderr,
                  "%s: %s\n",
                  func,
                  ipmi_sel_parse_ctx_errormsg(state_data->sel_parse_ctx));
  return -1;
}

static int
_hex_output(ipmi_sel_state_data_t *state_data)
{
  uint8_t record_data[IPMI_SEL_RECORD_SIZE];
  int record_data_len;
  int rv = -1;

  assert(state_data);

  if ((record_data_len = ipmi_sel_parse_read_record(state_data->sel_parse_ctx,
                                                    record_data,
                                                    IPMI_SEL_RECORD_SIZE)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record") < 0)
        goto cleanup;
      goto out;
    }

  if (state_data->prog_data->args->common.debug
      && record_data_len < IPMI_SEL_RECORD_SIZE)
    {
      pstdout_fprintf(state_data->pstate,
                      stderr,
                      "Invalid length SEL entry read: %d\n",
                      record_data_len);
      goto out;
    }
  
  pstdout_printf (state_data->pstate, 
                  "RID:[%02X][%02X] " 
                  "RT:[%02X] " 
                  "TS:[%02X][%02X][%02X][%02X] " 
                  "GID:[%02X][%02X] " 
                  "ER:[%02X] " 
                  "ST:[%02X] " 
                  "SN:[%02X] " 
                  "EDIR:[%02X] "
                  "ED1: [%02X] "
                  "ED2: [%02X] "
                  "ED3: [%02X]\n",
                  record_data[0], record_data[1], 
                  record_data[2], 
                  record_data[3], record_data[4], record_data[5], record_data[6], 
                  record_data[7], record_data[8], 
                  record_data[9], 
                  record_data[10], 
                  record_data[11], 
                  record_data[12], 
                  record_data[13], 
                  record_data[14], 
                  record_data[15]);

 out:
  rv = 0;
 cleanup:
  return rv;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_get_system_event_record_info(ipmi_sel_state_data_t *state_data,
                              uint8_t *event_type_code,
                              uint8_t *event_data2_flag,
                              uint8_t *event_data3_flag,
                              uint8_t *event_data2,
                              uint8_t *event_data3)
{
  assert(state_data);
  assert(event_type_code);
  assert(event_data2_flag);
  assert(event_data3_flag);
  assert(event_data2);
  assert(event_data3);

  if (ipmi_sel_parse_read_event_type_code(state_data->sel_parse_ctx, 
                                          event_type_code) < 0)
    {
      if (_sel_parse_err_handle(state_data, 
                                "ipmi_sel_parse_read_event_type_code") < 0)
        return -1;
      return 0;
    }
  
  if (ipmi_sel_parse_read_event_data1_event_data2_flag(state_data->sel_parse_ctx, 
                                                       event_data2_flag) < 0)
    {
      if (_sel_parse_err_handle(state_data, 
                                "ipmi_sel_parse_read_event_data1_event_data2_flag") < 0)
        return -1;
      return 0;
    }
  
  if (ipmi_sel_parse_read_event_data1_event_data3_flag(state_data->sel_parse_ctx, 
                                                       event_data3_flag) < 0)
    {
      if (_sel_parse_err_handle(state_data, 
                                "ipmi_sel_parse_read_event_data1_event_data3_flag") < 0)
        return -1;
      return 0;
    }
  
  if (ipmi_sel_parse_read_event_data2(state_data->sel_parse_ctx, 
                                      event_data2) < 0)
    {
      if (_sel_parse_err_handle(state_data, 
                                "ipmi_sel_parse_read_event_data2") < 0)
        return -1;
      return 0;
    }
  
  if (ipmi_sel_parse_read_event_data3(state_data->sel_parse_ctx, 
                                      event_data3) < 0)
    {
      if (_sel_parse_err_handle(state_data, 
                                "ipmi_sel_parse_read_event_data3") < 0)
        return -1;
      return 0;
    }
  
  return 1;
}

static int
_legacy_normal_output(ipmi_sel_state_data_t *state_data, uint8_t record_type)
{
  char fmtbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  char *fmt;
  int outbuf_len;
  unsigned int flags;
  int record_type_class;
  int rv = -1;
  int ret;

  assert(state_data);
  assert(state_data->prog_data->args->legacy_output);

  flags = IPMI_SEL_PARSE_STRING_FLAGS_IGNORE_UNAVAILABLE_FIELD;
  flags |= IPMI_SEL_PARSE_STRING_FLAGS_OUTPUT_NOT_AVAILABLE;
  flags |= IPMI_SEL_PARSE_STRING_FLAGS_DATE_MONTH_STRING;
  flags |= IPMI_SEL_PARSE_STRING_FLAGS_LEGACY;

  record_type_class = ipmi_sel_record_type_class(record_type);
  if (record_type_class == IPMI_SEL_RECORD_TYPE_CLASS_SYSTEM_EVENT_RECORD)
    {
      uint8_t event_type_code;
      uint8_t event_data2_flag;
      uint8_t event_data3_flag;
      uint8_t event_data2;
      uint8_t event_data3;

      if ((ret = _get_system_event_record_info(state_data,
                                               &event_type_code,
                                               &event_data2_flag,
                                               &event_data3_flag,
                                               &event_data2,
                                               &event_data3)) < 0)
        goto cleanup;

      if (!ret)
        goto out;

      strcpy(fmtbuf, "%i:%d %t:%g %s:%e");
      
      /* achu: special case, legacy output didn't support
         previous/severity output and would not output 0xFF for
         discrete events.
      */
      if (!(((ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_GENERIC_DISCRETE
              || ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_SENSOR_SPECIFIC_DISCRETE)
             && event_data2_flag == IPMI_SEL_EVENT_DATA_PREVIOUS_STATE_OR_SEVERITY)
            || ((ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_THRESHOLD
                 || ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_GENERIC_DISCRETE
                 || ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_SENSOR_SPECIFIC_DISCRETE)
                && event_data2_flag == IPMI_SEL_EVENT_DATA_SENSOR_SPECIFIC_EVENT_EXTENSION_CODE
                && event_data2 == IPMI_SEL_RECORD_UNSPECIFIED_EVENT)))
        {
          if (event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE)
            strcat(fmtbuf, ":%f");
        }
          
      if (!((ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_THRESHOLD
             || ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_GENERIC_DISCRETE
             || ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_SENSOR_SPECIFIC_DISCRETE)
            && event_data3_flag == IPMI_SEL_EVENT_DATA_SENSOR_SPECIFIC_EVENT_EXTENSION_CODE
            && event_data3 == IPMI_SEL_RECORD_UNSPECIFIED_EVENT))
        {
          if (event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE)
            strcat(fmtbuf, ":%h");
        }

      fmt = fmtbuf;
    }
  else if (record_type_class == IPMI_SEL_RECORD_TYPE_CLASS_TIMESTAMPED_OEM_RECORD)
    fmt = "%i:%d %t:%m:%o";
  else if (record_type_class == IPMI_SEL_RECORD_TYPE_CLASS_NON_TIMESTAMPED_OEM_RECORD)
    fmt = "%i:o";
  else
    {
      pstdout_fprintf(state_data->pstate,
                      stderr,
                      "Unknown SEL Record Type: %X\n",
                      record_type);
      goto out;
    }
  
  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      fmt,
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        goto cleanup;
      goto out;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, "%s\n", outbuf);

 out:
  rv = 0;
 cleanup:
  return rv;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_record_id(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%i",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, "%-9s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_date_and_time(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%d",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-11s", outbuf);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%t",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-8s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_sensor_name_and_group(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%s",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-16s", outbuf);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%g",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-24s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_event_direction(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);
  assert(state_data->prog_data->args->verbose_count >= 2);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%k",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-15s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_event(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%e",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-33s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_event_detail(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char fmtbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;
  char *fmt;
  uint8_t event_type_code;
  uint8_t event_data2_flag;
  uint8_t event_data3_flag;
  uint8_t event_data2;
  uint8_t event_data3;
  int ret;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);
  assert(state_data->prog_data->args->verbose_count);
      
  if ((ret = _get_system_event_record_info(state_data,
                                           &event_type_code,
                                           &event_data2_flag,
                                           &event_data3_flag,
                                           &event_data2,
                                           &event_data3)) < 0)
    return -1;
  
  if (!ret)
    return 0;

  /* note: previously set sel parse library separator to " ; "
   * so some places where there could be two outputs
   * would be separated by a semi-colon
   */

  if (ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_THRESHOLD
      && event_data2_flag == IPMI_SEL_EVENT_DATA_TRIGGER_READING
      && event_data3_flag == IPMI_SEL_EVENT_DATA_TRIGGER_THRESHOLD_VALUE)
    strcat(fmtbuf, "%c");
  else if ((ipmi_event_reading_type_code_class(event_type_code) != IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM 
            && event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE
            && event_data2_flag != IPMI_SEL_EVENT_DATA_OEM_CODE
            && ipmi_event_reading_type_code_class(event_type_code) != IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM
            && event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE
            && event_data3_flag != IPMI_SEL_EVENT_DATA_OEM_CODE)
           || (ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM
               && event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE
               && event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE)
           || (state_data->prog_data->args->verbose_count >= 2
               && event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE
               && event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE))
    strcat(fmtbuf, "%f ; %h");
  else if ((ipmi_event_reading_type_code_class(event_type_code) != IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM
            && event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE
            && event_data2_flag != IPMI_SEL_EVENT_DATA_OEM_CODE)
           || (ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM
               && event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE)
           || (state_data->prog_data->args->verbose_count >= 2
               && event_data2_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE))
    strcat(fmtbuf, "%f");
  else if ((ipmi_event_reading_type_code_class(event_type_code) != IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM
            && event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE
            && event_data3_flag != IPMI_SEL_EVENT_DATA_OEM_CODE)
           || (ipmi_event_reading_type_code_class(event_type_code) == IPMI_EVENT_READING_TYPE_CODE_CLASS_OEM
               && event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE)
           || (state_data->prog_data->args->verbose_count >= 2
               && event_data3_flag != IPMI_SEL_EVENT_DATA_UNSPECIFIED_BYTE))
    strcat(fmtbuf, "%h");
          
  fmt = fmtbuf;

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      fmt,
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_manufacturer_id(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%m",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %-16s", outbuf);

  return 1;
}

/* return 1 on success
 * return 0 on non-success, but don't fail
 * return -1 on error 
 */
static int
_normal_output_oem_data(ipmi_sel_state_data_t *state_data, unsigned int flags)
{
  char outbuf[IPMI_SEL_OUTPUT_BUFLEN+1];
  int outbuf_len;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  memset(outbuf, '\0', IPMI_SEL_OUTPUT_BUFLEN+1);
  if ((outbuf_len = ipmi_sel_parse_read_record_string(state_data->sel_parse_ctx,
                                                      "%o",
                                                      outbuf,
                                                      IPMI_SEL_OUTPUT_BUFLEN,
                                                      flags)) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_string") < 0)
        return -1;
      return 0;
    }
  
  if (outbuf_len)
    pstdout_printf (state_data->pstate, " | %s", outbuf);

  return 1;
}

static int
_normal_output(ipmi_sel_state_data_t *state_data, uint8_t record_type)
{
  unsigned int flags;
  int record_type_class;
  int rv = -1;
  int ret;

  assert(state_data);
  assert(!state_data->prog_data->args->legacy_output);

  if (!state_data->output_headers)
    {
      pstdout_printf(state_data->pstate,
                     "Record ID | Date        | Time     | Sensor Name      | Sensor Group            ");
      if (state_data->prog_data->args->verbose_count >= 2)
        {
          pstdout_printf(state_data->pstate,
                         " | Event Direction");
        }
      pstdout_printf(state_data->pstate,
                     " | Event                            ");
      if (state_data->prog_data->args->verbose_count)
        {
          pstdout_printf(state_data->pstate,
                         " | Event Detail");
        }
      pstdout_printf(state_data->pstate, 
                     "\n");
      state_data->output_headers++;
    }

  flags = IPMI_SEL_PARSE_STRING_FLAGS_IGNORE_UNAVAILABLE_FIELD;
  flags |= IPMI_SEL_PARSE_STRING_FLAGS_OUTPUT_NOT_AVAILABLE;
  flags |= IPMI_SEL_PARSE_STRING_FLAGS_DATE_MONTH_STRING;
  if (state_data->prog_data->args->verbose_count >= 2)
    flags|= IPMI_SEL_PARSE_STRING_FLAGS_VERBOSE;
  
  record_type_class = ipmi_sel_record_type_class(record_type);
  if (record_type_class == IPMI_SEL_RECORD_TYPE_CLASS_SYSTEM_EVENT_RECORD)
    {
      if ((ret = _normal_output_record_id(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto out;

      if ((ret = _normal_output_date_and_time(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto newline_out;

      if ((ret = _normal_output_sensor_name_and_group(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto newline_out;

      if (state_data->prog_data->args->verbose_count >= 2)
        {
          if ((ret = _normal_output_event_direction(state_data, flags)) < 0)
            goto cleanup;
          
          if (!ret)
            goto newline_out;
        }

      if ((ret = _normal_output_event(state_data, flags)) < 0)
        goto cleanup;
      
      if (!ret)
        goto newline_out;

      if (state_data->prog_data->args->verbose_count)
        {
          if ((ret = _normal_output_event_detail(state_data, flags)) < 0)
            goto cleanup;
          
          if (!ret)
            goto newline_out;
        }
    }
  else if (record_type_class == IPMI_SEL_RECORD_TYPE_CLASS_TIMESTAMPED_OEM_RECORD)
    {
      if ((ret = _normal_output_record_id(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto out;

      if ((ret = _normal_output_date_and_time(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto newline_out;

      if ((ret = _normal_output_manufacturer_id(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto newline_out;

      if ((ret = _normal_output_oem_data(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto newline_out;
    }
  else if (record_type_class == IPMI_SEL_RECORD_TYPE_CLASS_NON_TIMESTAMPED_OEM_RECORD)
    {
      if ((ret = _normal_output_record_id(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto out;
      
      if ((ret = _normal_output_oem_data(state_data, flags)) < 0)
        goto cleanup;

      if (!ret)
        goto newline_out;
    }
  else
    {
      pstdout_fprintf(state_data->pstate,
                      stderr,
                      "Unknown SEL Record Type: %X\n",
                      record_type);
      goto out;
    }
  
 newline_out:
  pstdout_printf (state_data->pstate, "\n");
 out:
  rv = 0;
 cleanup:
  return rv;
}

static int
_sel_parse_callback(ipmi_sel_parse_ctx_t ctx, void *callback_data)
{
  ipmi_sel_state_data_t *state_data;
  uint8_t record_type;
  int record_type_class;
  int rv = -1;

  assert(ctx);
  assert(callback_data);

  state_data = (ipmi_sel_state_data_t *)callback_data;

  if (state_data->prog_data->args->display || state_data->prog_data->args->display_range)
    {
      uint16_t record_id;
      int found = 0;

      if (ipmi_sel_parse_read_record_id(state_data->sel_parse_ctx,
                                        &record_id) < 0)
        {
          if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_id") < 0)
            goto cleanup;
          goto out;
        }

      if (state_data->prog_data->args->display)
        {
          int i;

          /* achu: I know it's slow, shouldn't be that big of a deal in the grand scheme */
          for (i = 0; i < state_data->prog_data->args->display_record_list_length; i++)
            {
              if (state_data->prog_data->args->display_record_list[i] == record_id)
                {
                  found++;
                  break;
                }
            }
        }
      else
        {
          if (record_id >= state_data->prog_data->args->display_range1
              && record_id <= state_data->prog_data->args->display_range2)
            found++;
        }
      
      if (!found)
        goto out;
    }

  if (ipmi_sel_parse_read_record_type(state_data->sel_parse_ctx,
                                      &record_type) < 0)
    {
      if (_sel_parse_err_handle(state_data, "ipmi_sel_parse_read_record_type") < 0)
        goto cleanup;
      goto out;
    }

  record_type_class = ipmi_sel_record_type_class(record_type);

  if (state_data->prog_data->args->system_event_only
      && record_type_class != IPMI_SEL_RECORD_TYPE_CLASS_SYSTEM_EVENT_RECORD)
    goto out;
  
  if (state_data->prog_data->args->oem_event_only
      && record_type_class != IPMI_SEL_RECORD_TYPE_CLASS_TIMESTAMPED_OEM_RECORD
      && record_type_class != IPMI_SEL_RECORD_TYPE_CLASS_NON_TIMESTAMPED_OEM_RECORD)
    goto out;

  if (state_data->prog_data->args->hex_dump)
    {
      if (_hex_output(state_data) < 0)
        goto cleanup;
    }
  else
    {
      if (state_data->prog_data->args->legacy_output)
        {
          if (_legacy_normal_output(state_data, record_type) < 0)
            goto cleanup;
        }
      else
        {
          if (_normal_output(state_data, record_type) < 0)
            goto cleanup;
        }
    }

 out:
  rv = 0;
 cleanup:
  return rv;
}

static int 
_display_sel_records (ipmi_sel_state_data_t *state_data)
{
  struct ipmi_sel_arguments *args;
  
  assert(state_data);

  args = state_data->prog_data->args;

  if (!args->sdr.ignore_sdr_cache)
    {
      if (sdr_cache_create_and_load (state_data->sdr_cache_ctx,
                                     state_data->pstate,
                                     state_data->ipmi_ctx,
                                     args->sdr.quiet_cache,
                                     args->sdr.sdr_cache_recreate,
                                     state_data->hostname,
                                     args->sdr.sdr_cache_directory) < 0)
        return -1;
    }

  if (!args->legacy_output)
    {
      if (ipmi_sel_parse_ctx_set_separator (state_data->sel_parse_ctx, " ; ") < 0)
        {
          pstdout_fprintf(state_data->pstate,
                          stderr,
                          "ipmi_sel_parse: %s\n",
                          ipmi_sel_parse_ctx_errormsg(state_data->sel_parse_ctx));
          return -1;
        }
    }

  if (ipmi_sel_parse(state_data->sel_parse_ctx,
                     _sel_parse_callback,
                     state_data) < 0)
    {
      pstdout_fprintf(state_data->pstate,
                      stderr,
                      "ipmi_sel_parse: %s\n",
                      ipmi_sel_parse_ctx_errormsg(state_data->sel_parse_ctx));
      return -1;
    }

  return 0;
}

static int 
run_cmd_args (ipmi_sel_state_data_t *state_data)
{
  struct ipmi_sel_arguments *args;

  assert(state_data);

  args = state_data->prog_data->args;
  
  if (args->info)
    return _display_sel_info (state_data);

  if (args->sdr.flush_cache)
    return _flush_cache (state_data);
    
  if (args->delete_all)
    return _clear_entries (state_data);

  if (args->delete)
    return _delete_records (state_data);  
 
  if (args->delete_range)
    return _delete_range (state_data);

  /* else default to displaying records */

  if (_display_sel_records (state_data) < 0)
    return -1;

  return 0;
}

static int
_ipmi_sel (pstdout_state_t pstate,
           const char *hostname,
           void *arg)
{
  ipmi_sel_state_data_t state_data;
  ipmi_sel_prog_data_t *prog_data;
  char errmsg[IPMI_OPEN_ERRMSGLEN];
  int exit_code = -1;

  prog_data = (ipmi_sel_prog_data_t *)arg;
  memset(&state_data, '\0', sizeof(ipmi_sel_state_data_t));

  state_data.prog_data = prog_data;
  state_data.pstate = pstate;
  state_data.hostname = (char *)hostname;

  /* Special case, just flush, don't do an IPMI connection */
  if (!prog_data->args->sdr.flush_cache)
    {
      if (!(state_data.ipmi_ctx = ipmi_open(prog_data->progname,
                                            hostname,
                                            &(prog_data->args->common),
                                            errmsg,
                                            IPMI_OPEN_ERRMSGLEN)))
        {
          pstdout_fprintf(pstate,
                          stderr,
                          "%s\n",
                          errmsg);
          exit_code = EXIT_FAILURE;
          goto cleanup;
        }
    }

  if (!(state_data.sdr_cache_ctx = ipmi_sdr_cache_ctx_create()))
    {
      pstdout_perror (pstate, "ipmi_sdr_cache_ctx_create()");
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

  if (state_data.prog_data->args->common.debug)
    {
      /* Don't error out, if this fails we can still continue */
      if (ipmi_sdr_cache_ctx_set_flags(state_data.sdr_cache_ctx,
                                       IPMI_SDR_CACHE_FLAGS_DEBUG_DUMP) < 0)
        pstdout_fprintf (pstate,
                         stderr,
                         "ipmi_sdr_cache_ctx_set_flags: %s\n",
                         ipmi_sdr_cache_ctx_errormsg(state_data.sdr_cache_ctx));

      if (hostname)
        {
          if (ipmi_sdr_cache_ctx_set_debug_prefix(state_data.sdr_cache_ctx,
                                                  hostname) < 0)
            pstdout_fprintf (pstate,
                             stderr,
                             "ipmi_sdr_cache_ctx_set_debug_prefix: %s\n",
                             ipmi_sdr_cache_ctx_errormsg(state_data.sdr_cache_ctx));
        }
    }

  /* Special case, just flush, don't do SEL stuff */
  if (!prog_data->args->sdr.flush_cache)
    {
      if (!(state_data.sel_parse_ctx = ipmi_sel_parse_ctx_create(state_data.ipmi_ctx,
                                                                 prog_data->args->sdr.ignore_sdr_cache ? NULL : state_data.sdr_cache_ctx)))
        {
          pstdout_perror (pstate, "ipmi_sel_parse_ctx_create()");
          goto cleanup;
        }
      
      if (state_data.prog_data->args->common.debug)
        {
          /* Don't error out, if this fails we can still continue */
          if (ipmi_sel_parse_ctx_set_flags(state_data.sel_parse_ctx,
                                           IPMI_SEL_PARSE_FLAGS_DEBUG_DUMP) < 0)
            pstdout_fprintf (pstate,
                             stderr,
                             "ipmi_sel_parse_ctx_set_flags: %s\n",
                             ipmi_sel_parse_ctx_errormsg(state_data.sel_parse_ctx));
          
          if (hostname)
            {
              if (ipmi_sel_parse_ctx_set_debug_prefix(state_data.sel_parse_ctx,
                                                      hostname) < 0)
                pstdout_fprintf (pstate,
                                 stderr,
                                 "ipmi_sel_parse_ctx_set_debug_prefix: %s\n",
                                 ipmi_sel_parse_ctx_errormsg(state_data.sel_parse_ctx));
            }
        }
    }

  if (run_cmd_args (&state_data) < 0)
    {
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

  exit_code = 0;
 cleanup:
  if (state_data.sdr_cache_ctx)
    ipmi_sdr_cache_ctx_destroy(state_data.sdr_cache_ctx);
  if (state_data.sel_parse_ctx)
    ipmi_sel_parse_ctx_destroy(state_data.sel_parse_ctx);
  if (state_data.ipmi_ctx)
    {
      ipmi_ctx_close (state_data.ipmi_ctx);
      ipmi_ctx_destroy (state_data.ipmi_ctx);
    }
  return exit_code;
}

int 
main (int argc, char **argv)
{
  ipmi_sel_prog_data_t prog_data;
  struct ipmi_sel_arguments cmd_args;
  int exit_code;
  int hosts_count;
  int rv;

  ipmi_disable_coredump();

  memset(&prog_data, '\0', sizeof(ipmi_sel_prog_data_t));
  prog_data.progname = argv[0];
  ipmi_sel_argp_parse (argc, argv, &cmd_args);
  prog_data.args = &cmd_args;

  if ((hosts_count = pstdout_setup(&(prog_data.args->common.hostname),
                                   prog_data.args->hostrange.buffer_output,
                                   prog_data.args->hostrange.consolidate_output,
                                   prog_data.args->hostrange.fanout,
                                   prog_data.args->hostrange.eliminate,
                                   prog_data.args->hostrange.always_prefix)) < 0)
    {
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

  /* We don't want caching info to output when are doing ranged output */
  if (hosts_count > 1)
    prog_data.args->sdr.quiet_cache = 1;

  if ((rv = pstdout_launch(prog_data.args->common.hostname,
                           _ipmi_sel,
                           &prog_data)) < 0)
    {
      fprintf(stderr,
              "pstdout_launch: %s\n",
              pstdout_strerror(pstdout_errnum));
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

  exit_code = rv;
 cleanup:
  return (exit_code);
}
