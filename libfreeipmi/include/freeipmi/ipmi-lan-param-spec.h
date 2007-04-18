/* 
   ipmi-lan-param-spec.h - IPMI LAN Parameter specification

   Copyright (C) 2003, 2004, 2005 FreeIPMI Core Team

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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  
*/

#ifndef _IPMI_LAN_PARAM_SPEC_H
#define _IPMI_LAN_PARAM_SPEC_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define IPMI_LAN_PARAM_SET_IN_PROGRESS                                  0
#define IPMI_LAN_PARAM_AUTHENTICATION_TYPE_SUPPORT                      1
#define IPMI_LAN_PARAM_AUTHENTICATION_TYPE_ENABLES                      2 
#define IPMI_LAN_PARAM_IP_ADDRESS                                       3 
#define IPMI_LAN_PARAM_IP_ADDRESS_SOURCE                                4 
#define IPMI_LAN_PARAM_MAC_ADDRESS                                      5 
#define IPMI_LAN_PARAM_SUBNET_MASK                                      6 
#define IPMI_LAN_PARAM_IPV4_HEADER_PARAMETERS                           7 
#define IPMI_LAN_PARAM_PRIMARY_RMCP_PORT_NO                             8 
#define IPMI_LAN_PARAM_SECONDARY_RMCP_PORT_NO                           9 
#define IPMI_LAN_PARAM_BMC_GENERATED_ARP_CONTROL                        10 
#define IPMI_LAN_PARAM_GRATUITOUS_ARP_INTERVAL                          11
#define IPMI_LAN_PARAM_DEFAULT_GATEWAY_ADDRESS                          12
#define IPMI_LAN_PARAM_DEFAULT_GATEWAY_MAC_ADDRESS                      13
#define IPMI_LAN_PARAM_BACKUP_GATEWAY_ADDRESS                           14
#define IPMI_LAN_PARAM_BACKUP_GATEWAY_MAC_ADDRESS                       15
#define IPMI_LAN_PARAM_COMMUNITY_STRING                                 16
#define IPMI_LAN_PARAM_NUMBER_OF_DESTINATIONS                           17
#define IPMI_LAN_PARAM_DESTINATION_TYPE                                 18
#define IPMI_LAN_PARAM_DESTINATION_ADDRESSES                            19
#define IPMI_LAN_PARAM_VLAN_ID                                          20
#define IPMI_LAN_PARAM_VLAN_PRIORITY                                    21
#define IPMI_LAN_PARAM_RMCPPLUS_MESSAGING_CIPHER_SUITE_ENTRY_SUPPORT    22
#define IPMI_LAN_PARAM_RMCPPLUS_MESSAGING_CIPHER_SUITE_ENTRIES          23
#define IPMI_LAN_PARAM_RMCPPLUS_MESSAGING_CIPHER_SUITE_PRIVILEGE_LEVELS 24

/* To avoid gcc warnings, added +1 and -1 in comparison */
#define IPMI_LAN_PARAM_VALID(__lan_param) \
        (((__lan_param+1) >= IPMI_LAN_PARAM_AUTHENTICATION_TYPE_SUPPORT \
          && (__lan_param-1) <= IPMI_LAN_PARAM_RMCPPLUS_MESSAGING_CIPHER_SUITE_PRIVILEGE_LEVELS) ? 1 : 0)

#ifdef __cplusplus
}
#endif

#endif /* _IPMI_LAN_PARAM_SPEC_H */
