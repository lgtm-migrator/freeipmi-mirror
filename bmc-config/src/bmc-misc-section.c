#include "bmc-config.h"
#include "bmc-common.h"
#include "bmc-config-api.h"
#include "bmc-diff.h"
#include "bmc-map.h"
#include "bmc-sections.h"

static int
power_restore_policy_checkout (const struct bmc_config_arguments *args,
			       const struct section *sect,
			       struct keyvalue *kv)
{
  uint8_t policy;
  int ret;

  ret = get_bmc_power_restore_policy (args->dev,
				      &policy);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (!(kv->value = strdup (power_restore_policy_string (policy))))
    {
      perror("strdup");
      return -1;
    }
  return 0;
}

static int
power_restore_policy_commit (const struct bmc_config_arguments *args,
			     const struct section *sect,
			     const struct keyvalue *kv)
{
  return set_bmc_power_restore_policy (args->dev,
				       power_restore_policy_number (kv->value));
}

static int
power_restore_policy_diff (const struct bmc_config_arguments *args,
			   const struct section *sect,
			   const struct keyvalue *kv)
{
  int ret;
  uint8_t got_value;
  uint8_t passed_value;
  
  ret = get_bmc_power_restore_policy (args->dev,
				      &got_value);
  
  if (ret != 0)
    return -1;
  
  passed_value = power_restore_policy_number (kv->value);

  if (passed_value == got_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section_name,
                   kv->key,
                   kv->value,
                   power_restore_policy_string (got_value));
    }
  return ret;
}

static int
power_restore_policy_validate (const struct bmc_config_arguments *args,
			       const struct section *sect,
			       const char *value)
{
  return (power_restore_policy_number (value) == -1 ? 1 : 0);
}

struct section *
bmc_misc_section_get (struct bmc_config_arguments *args)
{
  struct section *misc_section = NULL;

  if (!(misc_section = bmc_section_create ("Misc")))
    goto cleanup;

  add_keyvalue (misc_section,
		"Power_Restore_Policy",
		"Possible values: Off_State_AC_Apply/Restore_State_AC_Apply/On_State_AC_Apply",
		BMC_CHECKOUT_KEY_COMMENTED_OUT_IF_VALUE_EMPTY,
		power_restore_policy_checkout,
		power_restore_policy_commit,
		power_restore_policy_diff,
		power_restore_policy_validate);

  return misc_section;

 cleanup:
  if (misc_section)
    bmc_section_destroy(misc_section);
  return NULL;
}
