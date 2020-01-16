/*
 * Copyright (C) 2003-2015 FreeIPMI Core Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef IPMI_SENSOR_AND_EVENT_CODE_TABLES_OEM_INTEL_S5500WB_SPEC_H
#define IPMI_SENSOR_AND_EVENT_CODE_TABLES_OEM_INTEL_S5500WB_SPEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <freeipmi/spec/oem/ipmi-sensor-and-event-code-tables-oem-intel-common-spec.h>

/*
 * Intel S5500WB/Penguin Computing Relion 700
 */

/*
 * Event Reading Type Code = IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_PCIE_FATAL_SENSOR
 * Sensor Type = IPMI_SENSOR_TYPE_CRITICAL_INTERRUPT
 */
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_DATA_LINK_LAYER_PROTOCOL_ERROR 0x00
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_SURPRISE_LINK_DOWN             0x01
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_UNEXPECTED_COMPLETION_A        0x02
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_UNSUPPORTED_REQUEST            0x03
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_POISONED_TLP                   0x04
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_FLOW_CONTROL_PROTOCOL          0x05
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_COMPLETION_TIMEOUT             0x06
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_COMPLETER_ABORT                0x07
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_RECEIVER_BUFFER_OVERFLOW       0x08
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_ACS_VIOLATION                  0x09
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_MALFORMED_TLP                  0x0A
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_RECEIVED_FATAL_MESSAGE         0x0B
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_UNEXPECTED_COMPLETION_B        0x0C
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_FATAL_SENSOR_RECEIVED_NON_FATAL_MESSAGE     0x0D

/*
 * Event Reading Type Code = IPMI_EVENT_READING_TYPE_CODE_OEM_INTEL_PCIE_CORRECTABLE_SENSOR
 * Sensor Type = IPMI_SENSOR_TYPE_CRITICAL_INTERRUPT
 */
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_RECEIVER_ERROR       0x00
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_BAD_DLLP             0x01
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_BAD_TLLP             0x02
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_REPLAY_NUM_ROLLOVER  0x03
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_REPLAY_TIMER_TIMEOUT 0x04
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_ADVISORY_NON_FATAL   0x05
#define IPMI_OEM_INTEL_S5500WB_SPECIFIC_PCIE_CORRECTABLE_SENSOR_LINK_BW_CHANGED      0x06

/*
 * String arrays for above
 */

extern const char * const ipmi_oem_intel_s5500wb_specific_pci_fatal_sensor[];
extern unsigned int ipmi_oem_intel_s5500wb_specific_pci_fatal_sensor_max_index;

extern const char * const ipmi_oem_intel_s5500wb_specific_pci_correctable_sensor[];
extern unsigned int ipmi_oem_intel_s5500wb_specific_pci_correctable_sensor_max_index;

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_CHANNEL_INFORMATION_VALIDITY_BITMASK 0x10
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_CHANNEL_INFORMATION_VALIDITY_SHIFT   4

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_CHANNEL_INFORMATION_VALID   0x1
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_CHANNEL_INFORMATION_INVALID 0x0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_DIMM_INFORMATION_VALIDITY_BITMASK 0x08
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_DIMM_INFORMATION_VALIDITY_SHIFT   3

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_DIMM_INFORMATION_VALID   0x1
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_DIMM_INFORMATION_INVALID 0x0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_ERROR_TYPE_BITMASK 0x07
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_ERROR_TYPE_SHIFT   0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_ERROR_TYPE_UNKNOWN_ERROR        0x0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_ERROR_TYPE_DATA_PARITY_ERROR    0x1
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_ERROR_TYPE_ADDRESS_PARITY_ERROR 0x2

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOGICAL_RANK_BITMASK 0x03
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOGICAL_RANK_SHIFT   0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_MIRRORING_DOMAIN_LOCAL_SUBINSTANCE_BITMASK 0x0F
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_MIRRORING_DOMAIN_LOCAL_SUBINSTANCE_SHIFT   0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_MIRRORING_DOMAIN_LOCAL_SUBINSTANCE_CHANNEL_0_1 0x1
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_MIRRORING_DOMAIN_LOCAL_SUBINSTANCE_CHANNEL_0_2 0x2
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_MIRRORING_DOMAIN_LOCAL_SUBINSTANCE_CHANNEL_1_2 0x3

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_GLOBAL_FIRST_SOCKET_ID_BITMASK 0x0F
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_GLOBAL_FIRST_SOCKET_ID_SHIFT   0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_SOCKET_ID_BITMASK 0xF0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_SOCKET_ID_SHIFT   4

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_LOCAL_SOCKET_ID_APPLIES_TO_ALL_SOCKETS 0xE

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_GLOBAL_SECOND_SOCKET_ID_BITMASK 0xF0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_GLOBAL_SECOND_SOCKET_ID_SHIFT   4

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA2_OEM_INTEL_S5500WB_UNUSED_FIELD 0xF

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_PROCESSOR_SOCKET_BITMASK 0xE0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_PROCESSOR_SOCKET_SHIFT   5

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_PROCESSOR_SOCKET_1 0x0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_PROCESSOR_SOCKET_2 0x1

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_CHANNEL_NUMBER_BITMASK 0x18
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_CHANNEL_NUMBER_SHIFT   3

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_CHANNEL_A 0x0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_CHANNEL_B 0x1
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_CHANNEL_C 0x2

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DIMM_SLOT_ID_BITMASK 0x07
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DIMM_SLOT_ID_SHIFT   0

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DIMM_SOCKET_1 0x0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DIMM_SOCKET_2 0x1

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DOMAIN_INSTANCE_TYPE_BITMASK 0x80
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DOMAIN_INSTANCE_TYPE_SHIFT   7

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DOMAIN_INSTANCE_TYPE_LOCAL  0x0
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_DOMAIN_INSTANCE_TYPE_GLOBAL 0x1

#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_INSTANCE_ID_BITMASK 0x0F
#define IPMI_SENSOR_TYPE_MEMORY_EVENT_DATA3_OEM_INTEL_S5500WB_INSTANCE_ID_SHIFT   0

/*
 *
 * Intel Server Board S5500WB Technical Product Specification
 *
 * http://download.intel.com/support/motherboards/server/s5500wb/sb/s5500wb_tps_r1_8.pdf
 */

#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_CMOS_DATE_TIME_NOT_SET                                                             0x0012
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PASSWORD_CHECK_FAILED                                                              0x0048
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_KEYBOARD_COMPONENT_ENCOUNTERED_A_LOCKED_ERROR                                      0x0108
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_KEYBOARD_COMPONENT_ENCOUNTERED_A_STUCK_KEY_ERROR                                   0x0109
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_FIXED_MEDIA_THE_SAS_RAID_FIRMWARE_CAN_NOT_RUN_PROPERLY                             0x0113
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_COMPONENT_ENCOUNTERED_A_PERR_ERROR                                             0x0140
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_RESOURCE_CONFLICT                                                              0x0141
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_OUT_OF_RESOURCES_ERROR                                                         0x0146
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_CACHE_SIZE_MISMATCH_DETECTED                                          0x0192
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_STEPPING_MISMATCH                                                     0x0193
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_FAMILY_MISMATCH_DETECTED                                              0x0194
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_INTEL_QPI_SPEED_MISMATCH                                              0x0195
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_MODEL_MISMATCH                                                        0x0196
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_SPEEDS_MISMATCHED                                                     0x0197
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_FAMILY_IS_NOT_SUPPORTED                                               0x0198
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_AND_CHIPSET_STEPPING_CONFIGURATION_IS_UNSUPPORTED                        0x019F
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_CMOS_NVRAM_CONFIGURATION_CLEARED                                                   0x5220
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PASSWORDS_CLEARED_BY_JUMPER                                                        0x5221
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PASSWORD_CLEAR_JUMPER_IS_SET                                                       0x5224
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_01_UNABLE_TO_APPLY_MICROCODE_UPDATE                                      0x8160
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_02_UNABLE_TO_APPLY_MICROCODE_UPDATE                                      0x8161
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_0X_MICROCODE_UPDATE_NOT_FOUND                                            0x8180
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_WATCHDOG_TIMER_FAILED_ON_LAST_BOOT                                                 0x8190
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_OS_BOOT_WATCHDOG_TIMER_FAILURE                                                     0x8198
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_BASEBOARD_MANAGEMENT_CONTROLLER_FAILED_SELF_TEST                                   0x8300
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_BASEBOARD_MANAGEMENT_CONTROLLER_FAILED_TO_RESPOND                                  0x84F2
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_BASEBOARD_MANAGEMENT_CONTROLLER_IN_UPDATE_MODE                                     0x84F3
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SENSOR_DATA_RECORD_EMPTY                                                           0x84F4
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SYSTEM_EVENT_LOG_FULL                                                              0x84FF
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_MEMORY_COMPONENT_COULD_NOT_BE_CONFIGURED_IN_THE_SELECTED_RAS_MODE                  0x8500
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_POPULATION_ERROR                                                              0x8501
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_CLTT_CONFIGURATION_FAILURE_ERROR                                                   0x8502
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A1_FAILED_SELF_TEST_BIST                                                      0x8520
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A2_FAILED_SELF_TEST_BIST                                                      0x8521
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B1_FAILED_SELF_TEST_BIST                                                      0x8522
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B2_FAILED_SELF_TEST_BIST                                                      0x8523
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C1_FAILED_SELF_TEST_BIST                                                      0x8524
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C2_FAILED_SELF_TEST_BIST                                                      0x8525
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D1_FAILED_SELF_TEST_BIST                                                      0x8526
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D2_FAILED_SELF_TEST_BIST                                                      0x8527
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E1_FAILED_SELF_TEST_BIST                                                      0x8528
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E2_FAILED_SELF_TEST_BIST                                                      0x8529
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F1_FAILED_SELF_TEST_BIST                                                      0x852A
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F2_FAILED_SELF_TEST_BIST                                                      0x852B
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A1_DISABLED                                                                   0x8540
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A2_DISABLED                                                                   0x8541
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B1_DISABLED                                                                   0x8542
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B2_DISABLED                                                                   0x8543
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C1_DISABLED                                                                   0x8544
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C2_DISABLED                                                                   0x8545
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D1_DISABLED                                                                   0x8546
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D2_DISABLED                                                                   0x8547
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E1_DISABLED                                                                   0x8548
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E2_DISABLED                                                                   0x8549
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F1_DISABLED                                                                   0x854A
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F2_DISABLED                                                                   0x854B
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A1_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8560
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A2_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8561
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B1_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8562
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B2_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8563
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C1_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8564
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C2_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8565
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D1_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8566
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D2_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8567
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E1_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8568
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E2_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x8569
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F1_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x856A
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F2_COMPONENT_ENCOUNTERED_A_SERIAL_PRESENCE_DETECTION_FAIL_ERROR               0x856B
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A1_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A0
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_A2_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A1
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B1_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A2
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_B2_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A3
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C1_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A4
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_C2_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A5
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D1_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A6
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_D2_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A7
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E1_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A8
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_E2_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85A9
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F1_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85AA
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DIMM_F2_UNCORRECTABLE_ECC_ERROR_ENCOUNTERED                                        0x85AB
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_CHIPSET_RECLAIM_OF_NON_CRITICAL_VARIABLES_COMPLETE                                 0x8604
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_UNSPECIFIED_PROCESSOR_COMPONENT_HAS_ENCOUNTERED_A_NON_SPECIFIC_ERROR               0x9000
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_KEYBOARD_COMPONENT_WAS_NOT_DETECTED                                                0x9223
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_KEYBOARD_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                                  0x9226
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_MOUSE_COMPONENT_WAS_NOT_DETECTED                                                   0x9243
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_MOUSE_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                                     0x9246
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_LOCAL_CONSOLE_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                             0x9266
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_LOCAL_CONSOLE_COMPONENT_ENCOUNTERED_AN_OUTPUT_ERROR                                0x9268
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_LOCAL_CONSOLE_COMPONENT_ENCOUNTERED_A_RESOURCE_CONFLICT_ERROR                      0x9269
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_REMOTE_CONSOLE_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                            0x9286
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_REMOTE_CONSOLE_COMPONENT_ENCOUNTERED_AN_INPUT_ERROR                                0x9287
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_REMOTE_CONSOLE_COMPONENT_ENCOUNTERED_AN_OUTPUT_ERROR                               0x9288
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SERIAL_PORT_COMPONENT_WAS_NOT_DETECTED                                             0x92A3
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SERIAL_PORT_COMPONENT_ENCOUNTERED_A_RESOURCE_CONFLICT_ERROR                        0x92A9
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SERIAL_PORT_CONTROLLER_ERROR                                                       0x92C6
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SERIAL_PORT_COMPONENT_ENCOUNTERED_AN_INPUT_ERROR                                   0x92C7
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SERIAL_PORT_COMPONENT_ENCOUNTERED_AN_OUTPUT_ERROR                                  0x92C8
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_LPC_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                                       0x94C6
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_LPC_COMPONENT_ENCOUNTERED_A_RESOURCE_CONFLICT_ERROR                                0x94C9
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_ATA_ATPI_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                                  0x9506
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_COMPONENT_ENCOUNTERED_A_CONTROLLER_ERROR                                       0x95A6
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_COMPONENT_ENCOUNTERED_A_READ_ERROR                                             0x95A7
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_COMPONENT_ENCOUNTERED_A_WRITE_ERROR                                            0x95A8
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_UNSPECIFIED_SOFTWARE_COMPONENT_ENCOUNTERED_A_START_ERROR                           0x9609
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PEI_CORE_COMPONENT_ENCOUNTERED_A_LOAD_ERROR                                        0x9641
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PEI_MODULE_COMPONENT_ENCOUNTERED_A_ILLEGAL_SOFTWARE_STATE_ERROR                    0x9667
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DXE_CORE_COMPONENT_ENCOUNTERED_A_ILLEGAL_SOFTWARE_STATE_ERROR                      0x9687
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DXE_BOOT_SERVICES_DRIVER_COMPONENT_ENCOUNTERED_A_ILLEGAL_SOFTWARE_STATE_ERROR      0x96A7
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DXE_BOOT_SERVICES_DRIVER_COMPONENT_ENCOUNTERED_INVALID_CONFIGURATION               0x96AB
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_SMM_DRIVER_COMPONENT_ENCOUNTERED_A_ILLEGAL_SOFTWARE_STATE_ERROR                    0x96E7
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_TPM_DEVICE_NOT_DETECTED                                                            0xA000
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_TPM_DEVICE_MISSING_OR_NOT_RESPONDING                                               0xA001
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_TPM_DEVICE_FAILURE                                                                 0xA002
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_TPM_DEVICE_FAILED_SELF_TEST                                                        0xA003
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_COMPONENT_ENCOUNTERED_A_MISMATCH_ERROR                                   0xA022
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_COMPONENT_ENCOUNTERED_A_LOW_VOLTAGE_ERROR                                0xA027
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PROCESSOR_COMPONENT_ENCOUNTERED_A_HIGH_VOLTAGE_ERROR                               0xA028
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_COMPONENT_ENCOUNTERED_A_SERR_ERROR                                             0xA421
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_ATA_ATPI_ATA_BUS_SMART_NOT_SUPPORTED                                               0xA500
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_ATA_ATPI_ATA_SMART_IS_DISABLED                                                     0xA501
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_EXPRESS_COMPONENT_ENCOUNTERED_A_PERR_ERROR                                     0xA5A0
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_EXPRESS_COMPONENT_ENCOUNTERED_A_SERR_ERROR                                     0xA5A1
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_PCI_EXPRESS_IBIST_ERROR                                                            0xA5A4
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DXE_BOOT_SERVICES_DRIVER_NOT_ENOUGH_MEMORY_AVAILABLE_TO_SHADOW_A_LEGACY_OPTION_ROM 0xA6A0
#define IPMI_SENSOR_TYPE_SYSTEM_FIRMWARE_PROGRESS_OEM_INTEL_S5500WB_POST_ERROR_CODE_DXE_BOOT_SERVICES_DRIVER_UNRECOGNIZED                                              0xB6A3

#ifdef __cplusplus
}
#endif

#endif /* IPMI_SENSOR_AND_EVENT_CODE_TABLES_OEM_INTEL_S5500WB_SPEC_H */
