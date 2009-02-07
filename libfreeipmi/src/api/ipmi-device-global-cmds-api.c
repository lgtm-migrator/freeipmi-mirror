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

#include "freeipmi/api/ipmi-device-global-cmds-api.h"
#include "freeipmi/cmds/ipmi-device-global-cmds.h"
#include "freeipmi/spec/ipmi-ipmb-lun-spec.h"
#include "freeipmi/spec/ipmi-netfn-spec.h"

#include "ipmi-api-defs.h"
#include "ipmi-api-trace.h"
#include "ipmi-api-util.h"

#include "freeipmi-portability.h"

int8_t 
ipmi_cmd_get_device_id (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
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
                                    tmpl_cmd_get_device_id_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_device_id_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_device_id (obj_cmd_rq) < 0)
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
ipmi_cmd_cold_reset (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
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
                                    tmpl_cmd_cold_reset_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_cold_reset_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_cold_reset (obj_cmd_rq) < 0)
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
ipmi_cmd_warm_reset (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
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
                                    tmpl_cmd_warm_reset_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_warm_reset_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_warm_reset (obj_cmd_rq) < 0)
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
ipmi_cmd_set_acpi_power_state (ipmi_ctx_t ctx, 
                               uint8_t system_power_state_enumeration,
                               uint8_t set_system_power_state,
                               uint8_t device_power_state_enumeration,
                               uint8_t set_device_power_state,
                               fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int8_t rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!IPMI_ACPI_SET_SYSTEM_POWER_STATE_VALID(set_system_power_state)
      || (set_system_power_state == IPMI_ACPI_SET_SYSTEM_POWER_STATE_SET_SYSTEM_POWER_STATE
          && !IPMI_ACPI_SYSTEM_POWER_STATE_VALID(system_power_state_enumeration))
      || !IPMI_ACPI_SET_DEVICE_POWER_STATE_VALID(set_device_power_state)
      || (set_device_power_state == IPMI_ACPI_SET_DEVICE_POWER_STATE_SET_DEVICE_POWER_STATE
          && !IPMI_ACPI_DEVICE_POWER_STATE_VALID(device_power_state_enumeration))
      || !fiid_obj_valid(obj_cmd_rs))
    {
      API_SET_ERRNUM(ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }
  
  if (api_fiid_obj_template_compare(ctx,
                                    obj_cmd_rs,
                                    tmpl_cmd_set_acpi_power_state_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_set_acpi_power_state_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_acpi_power_state (system_power_state_enumeration,
                                     set_system_power_state,
                                     device_power_state_enumeration,
                                     set_device_power_state,
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
ipmi_cmd_get_acpi_power_state (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
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
                                    tmpl_cmd_get_acpi_power_state_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_acpi_power_state_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_acpi_power_state (obj_cmd_rq) < 0)
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
ipmi_cmd_get_self_test_results (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
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
                                    tmpl_cmd_get_self_test_results_rs) < 0)
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_self_test_results_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_self_test_results (obj_cmd_rq) < 0)
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
ipmi_cmd_get_device_guid (ipmi_ctx_t ctx, fiid_obj_t obj_cmd_rs)
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
  
  if ((api_fiid_obj_template_compare(ctx,
                                     obj_cmd_rs,
                                     tmpl_cmd_get_device_guid_rs) < 0)
      && (api_fiid_obj_template_compare(ctx,
                                        obj_cmd_rs,
                                        tmpl_cmd_get_device_guid_format_rs) < 0))
    {
      ERR_TRACE(ipmi_ctx_errormsg(ctx), ipmi_ctx_errnum(ctx));
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create(tmpl_cmd_get_device_guid_rq)))
    {
      API_ERRNO_TO_API_ERRNUM(ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_device_guid (obj_cmd_rq) < 0)
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

