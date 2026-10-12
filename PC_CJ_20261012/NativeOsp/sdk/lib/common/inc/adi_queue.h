/*******************************************************************************
Copyright (c) 2022-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_queue.h
 * \brief: Queue library
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup common
 *  @{
 */

#ifndef ADI_QUEUE_H
#define ADI_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================= INCLUDES ================================*/
#include "adi_common.h"       /*!< Common definitions */
/*================================= DEFINES =================================*/

/* TODO: Update this based on platform! */
#define ADI_QUEUE_CODE_CRIT     /* __attribute__ ((section (".l2_cached_code"))) */
#define ADI_QUEUE_CODE_NOCRIT   /* __attribute__ ((section (".l3_code"))) */

/*=============================== DATA TYPES ================================*/

/*! Enumeration of the return status of queue APIs */
typedef enum ADI_QUEUE_STATUS
{
    ADI_QUEUE_STATUS_SUCCESS        = 0,    /*!< Queue operation was successful */
    ADI_QUEUE_STATUS_NO_SPACE       = 1,    /*!< Queue has no space */
    ADI_QUEUE_STATUS_QUEUE_EMPTY    = 2,    /*!< Queue is empty */
    ADI_QUEUE_STATUS_MUTEX_FAILURE  = 3,    /*!< Mutex operation failed */
} ADI_QUEUE_STATUS;

/*! Queue element structure (These elements will be linked together to create the queue) */
typedef struct ADI_QUEUE_ELEMENT
{
    void               *pData;          /*!< Pointer to element data */
    void               *pNext;          /*!< Pointer to next queue element */
} ADI_QUEUE_ELEMENT;

/*! Interface of mutex functions to be implemented by the application.
    NOTE: return value should be < 0 if error */
typedef struct ADI_QUEUE_MUTEX
{
    int (*queue_mutex_init)(void *poMutexCfg, void **phMutex);  /*!< Initialize a mutex lock and return the handle */
    int (*queue_mutex_destroy)(void *hMutex);                   /*!< Destroy the mutex lock */
    int (*queue_mutex_lock)(void *hMutex);                      /*!< Lock the mutex */
    int (*queue_mutex_unlock)(void *hMutex);                    /*!< Unlock the mutex */
} ADI_QUEUE_MUTEX;

/*! PRIVATE: Internal queue data structure */
typedef struct ADI_QUEUE_DATA
{
    bool                bInUse;         /*!< Is the queue created and in use? */
    void               *hMutex;         /*!< Mutex object */
    ADI_QUEUE_MUTEX    *poMutexFuncs;   /*!< Mutex functions */
    uint32_t            nQueueSize;     /*!< Size of the queue */
    ADI_QUEUE_ELEMENT  *poQueueMem;     /*!< Pointer to beginning of queue */
    size_t              nDataSize;      /*!< Size of each data element */
    ADI_QUEUE_ELEMENT  *poFreeElemHead; /*!< Pointer to the head of free elements */
    ADI_QUEUE_ELEMENT  *poUsedElemHead; /*!< Pointer to the head of used elements */
    ADI_QUEUE_ELEMENT  *poUsedElemTail; /*!< Pointer to the tail of used elements */
} ADI_QUEUE_DATA;

/*! Configuration for the queue */
typedef struct ADI_QUEUE_CONFIG
{
    uint32_t            nQueueSize;     /*!< Set the number of elements in the queue here */
    ADI_QUEUE_ELEMENT  *poQueueMem;     /*!< Pointer to beginning of queue */
    ADI_QUEUE_MUTEX    *poMutexFuncs;   /*!< Mutex functions */
    size_t              nDataSize;      /*!< Size of each data element */
    void               *poMutexCfg;     /*!< Pointer to the mutex configuration */

    /* ~~~~~~~ Internal ~~~~~~~ */
    ADI_QUEUE_DATA      oQueueData;     /*!< Internal queue data (Need not be filled by application) */
} ADI_QUEUE_CONFIG;

/*! Handle to the queue */
typedef void *ADI_QUEUE_HANDLE;

/*=================================== APIs ==================================*/

extern ADI_QUEUE_STATUS adi_queue_create(ADI_QUEUE_CONFIG *poQueueCfg, ADI_QUEUE_HANDLE *phQueue);
extern ADI_QUEUE_STATUS adi_queue_enqueue(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT *poQueueElem);
extern ADI_QUEUE_STATUS adi_queue_getFreeSlot(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT **poQueueElem);
extern ADI_QUEUE_STATUS adi_queue_markAsFinal(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT *poQueueElem);
extern ADI_QUEUE_STATUS adi_queue_peek(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT **poQueueElem);
extern ADI_QUEUE_STATUS adi_queue_dequeue(ADI_QUEUE_HANDLE hQueue);
extern bool adi_queue_isEmpty(ADI_QUEUE_HANDLE hQueue);
extern bool adi_queue_isFull(ADI_QUEUE_HANDLE hQueue);
extern ADI_QUEUE_STATUS adi_queue_destroy(ADI_QUEUE_HANDLE hQueue);

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif /* ADI_QUEUE_H */

/** @} */

/**
 * EOF: www.analog.com
 */
