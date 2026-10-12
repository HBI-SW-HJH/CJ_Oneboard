/*******************************************************************************
Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_eal_topDisc.h
 * \brief:The header file for Topology discovery
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup EAL
 *  @{
 */

#ifndef ADI_EAL_TOPDISC_H
#define ADI_EAL_TOPDISC_H

/*================================= INCLUDES ================================*/
#include "adi_eal.h"            /* The EAL header */

#ifdef ADI_E2B_IFACE_TOPDISC_ENABLED


/*================================= DEFINES =================================*/

/*================================ DATA TYPES ===============================*/
/*============================ PUBLIC PROTOTYPES ============================*/
extern ADI_EAL_STATUS adi_eal_topoDiscDistanceMeasure(uint32_t nInstNum, uint8_t nTimeDuration,
        double *panTopoDiscovery);
extern ADI_EAL_STATUS adi_eal_topoDiscAutoDistMeasure(uint32_t nInstNum, uint8_t nTimeDuration,
        double *panTopoDiscovery);


#endif /* ADI_E2B_IFACE_TOPDISC_ENABLED */
#endif /* ADI_EAL_TOPDISC_H */

/** @} */

/**
 * EOF: www.analog.com
 */

