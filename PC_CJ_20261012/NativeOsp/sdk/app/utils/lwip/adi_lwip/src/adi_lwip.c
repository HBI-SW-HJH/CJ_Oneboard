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
/*================================= INCLUDES ================================*/
/* C runtime includes */
#include "inc/adi_lwip.h"

/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/* Holds all the stack devices */
static ADI_LWIP_DEV devs[ADI_LWIP_MAX_DEVS];

/* Number of devices in the stack. Can only be added, not removed */
static uint8_t numDevs = 0;

/* Extended Callback Struct Definition */
static netif_ext_callback_t extCallbackInst;
/*================================ PROTOTYPES ===============================*/
static void adi_lwip_ext_status_cbk(struct netif* netif, netif_nsc_reason_t reason,
                                    const netif_ext_callback_args_t* args);
/*=================================== DATA ==================================*/

/*=================================== CODE ==================================*/
/**
 * @brief Returns the pointer to a free device in the device stack (if available)
 *
 * @return Pointer to next free device, NULL otherwise
 */
static ADI_LWIP_DEV* adi_lwip_get_free_dev( )
{
    ADI_LWIP_DEV* devPtr;
    if( numDevs == ADI_LWIP_MAX_DEVS )
    {
        devPtr = NULL;
    }
    else
    {
        devPtr = &devs[numDevs];
        numDevs++;
    }
    return devPtr;
}


/**
 * @brief Finds the device information based on the provided netif instance
 *
 * @param netif - Interface instance to search for
 * @returns Pointer to the device struct, or NULL
 */
static ADI_LWIP_DEV* adi_lwip_find_dev_by_netif(struct netif* netif)
{
    int i;
    for( i = 0; i < numDevs; i++ )
    {
        if(&devs[i].netif == netif)
        {
            return &devs[i];
        }
    }

    return NULL;
}

/**
 * @brief Finds the device information based on adi_network device number
 *
 * @param devNum - Device number to search for
 * @returns Pointer to the device struct, or NULL
 */
ADI_LWIP_DEV* adi_lwip_find_dev_by_devnum(uint32_t devNum)
{
    int i;
    for( i = 0; i < numDevs; i++ )
    {
        if(devs[i].devNum == devNum)
        {
            return &devs[i];
        }
    }

    return NULL;
}

/**
 * @brief Finds the lwip network interface instance by adi_network device number
 *
 * @param devNum - Device number to search for
 * @returns netif instance pointer, or NULL
 */
struct netif* adi_lwip_get_netif(uint32_t devNum)
{
    ADI_LWIP_DEV* dev = adi_lwip_find_dev_by_devnum(devNum);

    if( dev == NULL )
    {
        return NULL;
    }
    else
    {
        return &dev->netif;
    }
}

/**
 * @brief Helper function to determine if the network interface has a valid IPv4
 * address.  This should always return true for static IPs, and will transition
 * from false to true once DHCP acquires an address
 *
 * @param netif - Interface to check
 * @returns True or false
*/
bool adi_lwip_ipv4_addr_valid(struct netif* netif)
{
    uint32_t ipaddru32;
    ip4_addr_t ipaddr;
    if( netif == NULL )
    {
        return false;
    }

    ipaddru32 = ip4_addr_get_u32(netif_ip4_addr(netif));
    ip4_addr_set_u32(&ipaddr, ipaddru32);
    if( ip4_addr_isany_val(ipaddr) )
    {
        return false;
    }
    else
    {
        return true;
    }

}

/**
 * @brief Performs the physical transmission of lwip frame data to the MACPHY stack.
 *
 * @param netif - Interface requesting the transmission
 * @param p - Packet to send
 * @returns ERR_OK on success, error codes otherwise
 */
static err_t adi_lwip_netif_output(struct netif *netif, struct pbuf *p)
{
    ADI_LWIP_DEV* dev;
    uint8_t* bufPtr;
    struct eth_hdr *ethhdr;
    ADI_NETWORK_FRAME_CFG oNwFrameCfg;

    dev = adi_lwip_find_dev_by_netif(netif);
    if(dev == NULL)
    {
        return ERR_ARG;
    }

    ethhdr = (struct eth_hdr*)p->payload;

    (void)memset(&oNwFrameCfg, 0, sizeof(ADI_NETWORK_FRAME_CFG));
    oNwFrameCfg.bLenInclMacHdr = true;
    oNwFrameCfg.eTsReg = ADI_TS_EGRESS_NONE;
    oNwFrameCfg.nFrameType = ethhdr->type;
    oNwFrameCfg.panPhysAddr = ethhdr->dest.addr;
    oNwFrameCfg.nLenByte = p->tot_len;
    if(adi_network_getTxBuffer(dev->devNum, &bufPtr, &oNwFrameCfg) != ADI_NETWORK_OK)
    {
        return ERR_MEM;
    }

    LINK_STATS_INC(link.xmit);

    pbuf_copy_partial(p, bufPtr, p->tot_len, 0);
    adi_network_transmit(dev->devNum, &oNwFrameCfg);
    return ERR_OK;
}

/**
 * @brief Initialization callback for the lwip interface device
 *
 * @param netif - Interface to initialize
 * @returns ERR_OK on success, error code otherwise
*/
static err_t adi_lwip_netif_init(struct netif *netif)
{
    ADI_LWIP_DEV* devPtr = adi_lwip_find_dev_by_netif(netif);
    if(devPtr == NULL)
    {
        return ERR_ARG;
    }

    netif->mtu        = 1500;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
    SMEMCPY(netif->hwaddr, devPtr->macAddr, ETH_HWADDR_LEN);
    netif->hwaddr_len = ETH_HWADDR_LEN;
    
    netif->linkoutput = adi_lwip_netif_output;
    netif->output     = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif

    return ERR_OK;
}

static void adi_lwip_ext_status_cbk(struct netif* netif, netif_nsc_reason_t reason,
                                    const netif_ext_callback_args_t* args)
{
    if( reason == LWIP_NSC_IPV4_ADDRESS_CHANGED )
    {
        //Support DHCP callbacks here
    }
}

/**
 * @brief Initializes the lwip stack and wrapper functionaltiy
 */
void adi_lwip_init()
{
    int i;

    lwip_init();

    /* Empty device list */
    numDevs = 0;

    netif_add_ext_callback(&extCallbackInst, adi_lwip_ext_status_cbk);
}

/**
 * @brief Adds a new device into the lwip device list/stack
 *
 * @param initCfg - Initialization configuration
 * @returns New netif instance
 */
struct netif* adi_lwip_add_dev(ADI_LWIP_CFG* initCfg)
{
    ADI_LWIP_DEV* devPtr = adi_lwip_get_free_dev();
    if( devPtr == NULL )
    {
        return NULL;
    }

    memcpy(devPtr->macAddr, initCfg->macAddr, ETH_HWADDR_LEN);
    devPtr->devNum = initCfg->devNum;

    //Using DHCP. Zero out the IP Configuration
    if( initCfg->useDHCP )
    {
        initCfg->ipAddr.addr = IPADDR_ANY;
        initCfg->netMask.addr = IPADDR_ANY;
        initCfg->gwAddr.addr = IPADDR_ANY;
    }

    netif_add(&devPtr->netif, &initCfg->ipAddr, &initCfg->netMask,
              &initCfg->gwAddr, NULL, adi_lwip_netif_init, netif_input );
    devPtr->netif.name[0] = 'e';
    devPtr->netif.name[1] = '0' + (numDevs - 1); //Dumb for now.

    if( numDevs == 1 )
    {
        netif_set_default(&devPtr->netif);
    }

    netif_set_up(&devPtr->netif);

    if( initCfg->useDHCP )
    {
        if( dhcp_start(&devPtr->netif) != ERR_OK )
        {
            //Error handler for DHCP issues
        }
    }

    //For now always assume the link is up
    netif_set_link_up(&devPtr->netif);

    return &devPtr->netif;
}

/** @brief Sets a static IP for the device.
 *
 *         Will set the GW to .1 of the IP and mask to /24
 * @param devNum - ADI Network device number
 * @param ipAddr - IP Address to set
 */
void adi_lwip_set_static_ip(uint32_t devNum, ip4_addr_t* ipAddr)
{
    ip4_addr_t gw, mask;
    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&gw, ip4_addr_get_byte(ipAddr, 0), ip4_addr_get_byte(ipAddr, 1), ip4_addr_get_byte(ipAddr, 2), 1);
    adi_lwip_set_static_ip_ext(devNum, ipAddr, &mask, &gw);
}

/**
 * @brief Sets a static IP for the device.
 *
 * @param devNum - ADI Network device number
 * @param ipAddr - IP Address
 * @param mask   - Netmask
 * @param gw     - Gateway
 */
void adi_lwip_set_static_ip_ext(uint32_t devNum, ip4_addr_t* ipAddr, ip4_addr_t* mask, ip4_addr_t* gw)
{
    struct netif* nif = adi_lwip_get_netif(devNum);
    if( nif )
    {
        //For now always assume the link is up
        netif_set_link_down(nif);
        netif_set_addr(nif, ipAddr, mask, gw);
        netif_set_link_up(nif);
    }
}

/**
 * Attempts to get a new IP address via DHCP
 *
 * @param devNum - Network device number
 */
void adi_lwip_do_dhcp(uint32_t devNum)
{
    ip4_addr_t ip, gw, mask;
    ip.addr = IPADDR_ANY;
    gw.addr = IPADDR_ANY;
    mask.addr = IPADDR_ANY;

    struct netif* nif = adi_lwip_get_netif(devNum);
    if( nif )
    {
        //For now always assume the link is up
        netif_set_link_down(nif);
        netif_set_addr(nif, &ip, &mask, &gw);
        netif_set_link_up(nif);
        dhcp_start(nif);
    }
}
