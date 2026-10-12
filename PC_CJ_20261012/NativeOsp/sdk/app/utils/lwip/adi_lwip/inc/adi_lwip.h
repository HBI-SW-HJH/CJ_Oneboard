/*******************************************************************************
Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_lwip.c
 * \brief: lwip interface functions
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup app
 *  @{
 */

#ifndef __ADI_LWIP_H__
#define __ADI_LWIP_H__

/*================================= INCLUDES ================================*/
/* C runtime includes */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "adi_network.h"

/* lwIP core includes */
#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/api.h"

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/inet_chksum.h"

/* lwIP netif includes */
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#include "lwip/mem.h"
#include "lwip/memp.h"

#define ADI_LWIP_MAX_DEVS   ADI_NETWORK_MAX_DEVS

/*================================ DATA TYPES ===============================*/
/*
 * Struct housing the state of all devices in the stack
 */
typedef struct ADI_LWIP_DEV
{
    uint32_t    devNum;                  /*!< adi_network device number */
    uint8_t     macAddr[ETH_HWADDR_LEN]; /*!< MAC address */
    struct netif netif;                  /*!< LWIP netif instance */
} ADI_LWIP_DEV;

/*
 * Struct for cofiguration data when adding a new device
 */
typedef struct ADI_LWIP_CFG
{
    uint32_t    devNum;     /*!< adi_network device index/number */
    ip4_addr_t  ipAddr;     /*!< Device IP address */
    ip4_addr_t  gwAddr;     /*!< Gateway address */
    ip4_addr_t  netMask;    /*!< Netmask */
    bool        useDHCP;    /*!< Boolean to use DHCP for address */
    uint8_t     macAddr[ETH_HWADDR_LEN];    /*!< MAC Address */
} ADI_LWIP_CFG;

/*================================ PROTOTYPES ===============================*/
/*! Initialie the ADI LWIP Wrapper. Should be called before others */
void adi_lwip_init(void);

/*! Adds a network device to the LWIP stack */
struct netif* adi_lwip_add_dev(ADI_LWIP_CFG* initCfg);

/*! Gets the netif instance associated with an adi_network device number */
struct netif* adi_lwip_get_netif(uint32_t devNum);

/*! Runs the lwip tasking and calls adi_network_run */
void adi_lwip_run( void );

/*! Helper to check if the IPv4 address is valid (DHCP or static) */
bool adi_lwip_ipv4_addr_valid(struct netif* netif);

/*! Attaches a fallthrough callback for network events not handled by LWIP */
void adi_lwip_set_network_callback(ADI_NETWORK_CBK cbk);

/*! Sets a static IP for the device.  Will set the GW to .1 of the IP and mask to /24 */
void adi_lwip_set_static_ip(uint32_t devNum, ip4_addr_t* ipAddr);

/*! Sets a static IP for the device. */
void adi_lwip_set_static_ip_ext(uint32_t devNum, ip4_addr_t* ipAddr, ip4_addr_t* mask, ip4_addr_t* gw);

/*! Changes the MAC for the device */
void adi_lwip_set_mac(uint32_t devNum, uint8_t* mac);

/*! Attempts to configure the IP via DHCP */
void adi_lwip_do_dhcp(uint32_t devNum);

ADI_LWIP_DEV* adi_lwip_find_dev_by_devnum(uint32_t devNum);


#endif /*   __ADI_LWIP_H__ */