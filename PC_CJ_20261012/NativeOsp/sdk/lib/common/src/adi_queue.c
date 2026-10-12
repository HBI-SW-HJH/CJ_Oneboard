/*******************************************************************************
Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: adi_queue.c
 * \brief: Queue library code
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup common
 *  @{
 */

/* TODO: Add pre-condition asserts in all APIs! */
/*================================= INCLUDES ================================*/
#include "adi_queue.h"
#include <assert.h>

/*================================== DEFINES ================================*/
#define ADI_QUEUE_DBG_REQUIRE(cond) assert(cond);

/*================================ DATA TYPES ===============================*/

/*=================================== DATA ==================================*/

/*=================================== CODE ==================================*/
/* --------------------- PRIVATE CODE --------------------- */

/*! Get a free slot in the queue */
ADI_QUEUE_CODE_CRIT
static ADI_QUEUE_STATUS GetFreeSlot(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT **poQueueElem)
{
    ADI_QUEUE_STATUS eStatus = ADI_QUEUE_STATUS_NO_SPACE;
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;
    *poQueueElem = NULL;

    /* If an element is available in free chain, return the head
       and move the head to the next element in the chain */
    if(poQueueData->poFreeElemHead != NULL)
    {
        /* Return the head */
        *poQueueElem = poQueueData->poFreeElemHead;

        /* Move the head to the next element in the chain */
        poQueueData->poFreeElemHead = (ADI_QUEUE_ELEMENT *)poQueueData->poFreeElemHead->pNext;

        /* Remove the link in the returned slot */
        (*poQueueElem)->pNext = NULL;

        eStatus = ADI_QUEUE_STATUS_SUCCESS;
    }

    return eStatus;
}

/*! Mark the queue element as finalized */
ADI_QUEUE_CODE_CRIT
static ADI_QUEUE_STATUS MarkAsFinal(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT *poQueueElem)
{
    ADI_QUEUE_STATUS eStatus = ADI_QUEUE_STATUS_SUCCESS;
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;

    poQueueElem->pNext = NULL;

    /* If there is no element that is finalized,
       make this element as the start of that chain */
    if(poQueueData->poUsedElemHead == NULL)
    {
        poQueueData->poUsedElemHead = poQueueElem;
        poQueueData->poUsedElemTail = poQueueElem;
    }
    else
    {
        /* Attach the element to the end of the used queue */
        poQueueData->poUsedElemTail->pNext = poQueueElem;
        poQueueData->poUsedElemTail = (ADI_QUEUE_ELEMENT *)poQueueData->poUsedElemTail->pNext;
    }

    return eStatus;
}

/* --------------------- PUBLIC CODE --------------------- */
/**
 * @brief       Create a queue
 *
 * @param [in]  poQueueCfg      Configuration for the queue
 * @param [out] phQueue         Queue handle
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS         Queue created successfully
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_create(ADI_QUEUE_CONFIG *poQueueCfg, ADI_QUEUE_HANDLE *phQueue)
{
    ADI_QUEUE_STATUS eQueueStatus = ADI_QUEUE_STATUS_SUCCESS;

    /* Initialize internal queue data */
    ADI_QUEUE_DATA *poQueueData = &poQueueCfg->oQueueData;
    (void)ADI_MEMSET(poQueueData, 0, sizeof(ADI_QUEUE_DATA));

    poQueueData->bInUse = true;
    poQueueData->poMutexFuncs = poQueueCfg->poMutexFuncs;
    poQueueData->nQueueSize = poQueueCfg->nQueueSize;
    poQueueData->poQueueMem = poQueueCfg->poQueueMem;
    poQueueData->nDataSize = poQueueCfg->nDataSize;

    /* Chain the queue elements together */
    for(uint32_t i = 0U; i < poQueueCfg->nQueueSize - 1U; i++)
    {
        poQueueData->poQueueMem[i].pNext = &poQueueData->poQueueMem[i + 1U];
    }

    poQueueData->poQueueMem[poQueueCfg->nQueueSize - 1U].pNext = NULL;
    poQueueData->poFreeElemHead = poQueueData->poQueueMem;
    poQueueData->poUsedElemHead = NULL;

    /* Initialize a mutex lock for use in other APIs */
    if(poQueueData->poMutexFuncs != NULL
            && poQueueData->poMutexFuncs->queue_mutex_init(poQueueCfg->poMutexCfg, &poQueueData->hMutex) < 0)
    {
        eQueueStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
    }

    /* Set the internal data structure as handle and return */
    *phQueue = poQueueData;

    return eQueueStatus;
}

/**
 * @brief       Insert data into the queue
 *
 * @param [in]  hQueue      Queue handle
 * @param [in]  poQueueElem Data to insert
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS     Data inserted into queue successfully
 *              - #ADI_QUEUE_STATUS_NO_SPACE    No space in queue
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_enqueue(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT *poQueueElem)
{
    ADI_QUEUE_ELEMENT *pFreeSlot = NULL;
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;
    ADI_QUEUE_STATUS eQueueStatus = ADI_QUEUE_STATUS_NO_SPACE;

    do
    {
        /* Enter critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_lock(poQueueData->hMutex) < 0)
        {
            eQueueStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
            break;
        }

        /* Get a free slot */
        eQueueStatus = GetFreeSlot(hQueue, &pFreeSlot);

        /* If free slot is available, insert the data into the queue */
        if(eQueueStatus == ADI_QUEUE_STATUS_SUCCESS)
        {
            /* Insert the data into the queue */
            (void)ADI_MEMCPY(pFreeSlot->pData, poQueueElem->pData, poQueueData->nDataSize);

            /* Mark the data as finalized */
            eQueueStatus = MarkAsFinal(hQueue, pFreeSlot);
        }

        /* Exit critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_unlock(poQueueData->hMutex) < 0)
        {
            eQueueStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
        }
    } while(false);

    return eQueueStatus;
}

/**
 * @brief       Get a free slot to fill data
 *
 * @param [in]  hQueue      Queue handle
 * @param [out] poQueueElem Free slot in which data can be filled
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS     Successfully got a free slot to add data
 *              - #ADI_QUEUE_STATUS_NO_SPACE    No space in queue
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 *
 * @note        After filling the data, set the bElemFinalized flag of the queue element
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_getFreeSlot(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT **poQueueElem)
{
    ADI_QUEUE_STATUS eStatus = ADI_QUEUE_STATUS_NO_SPACE;
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;

    do
    {
        /* Enter critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_lock(poQueueData->hMutex) < 0)
        {
            eStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
            break;
        }

        eStatus = GetFreeSlot(hQueue, poQueueElem);

        /* Exit critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_unlock(poQueueData->hMutex) < 0)
        {
            eStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
        }
    } while(false);

    return eStatus;
}

/**
 * @brief       Mark a queue element as finalized
 *
 * @param [in]  hQueue      Queue handle
 * @param [out] poQueueElem Queue element that is completely filled
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS     Successfully marked the element as final
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_markAsFinal(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT *poQueueElem)
{
    ADI_QUEUE_STATUS eStatus = ADI_QUEUE_STATUS_SUCCESS;
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;

    do
    {
        /* Enter critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_lock(poQueueData->hMutex) < 0)
        {
            eStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
            break;
        }

        eStatus = MarkAsFinal(hQueue, poQueueElem);

        /* Exit critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_unlock(poQueueData->hMutex) < 0)
        {
            eStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
        }
    } while(false);

    return eStatus;
}

/**
 * @brief       Peek queue for data
 *
 * @param [in]  hQueue      Queue handle
 * @param [out] poQueueElem Linked list to the queue data
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS         Successfully peeked the queue
 *              - #ADI_QUEUE_STATUS_QUEUE_EMPTY     Queue is empty
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_peek(ADI_QUEUE_HANDLE hQueue, ADI_QUEUE_ELEMENT **poQueueElem)
{
    ADI_QUEUE_STATUS eStatus = ADI_QUEUE_STATUS_QUEUE_EMPTY;
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;
    *poQueueElem = NULL;

    do
    {
        /* Enter critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_lock(poQueueData->hMutex) < 0)
        {
            eStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
            break;
        }

        /* If the used chain has any data, return the head */
        if(poQueueData->poUsedElemHead != NULL)
        {
            /* Return the head of used chain */
            *poQueueElem = poQueueData->poUsedElemHead;
            eStatus = ADI_QUEUE_STATUS_SUCCESS;
        }

        /* Exit critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_unlock(poQueueData->hMutex) < 0)
        {
            eStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
        }
    } while(false);

    return eStatus;
}

/**
 * @brief       Remove element from the queue
 *
 * @param [in]  hQueue      Queue handle
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS     Successfully dequeued element
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_dequeue(ADI_QUEUE_HANDLE hQueue)
{
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;
    ADI_QUEUE_STATUS eQueueStatus = ADI_QUEUE_STATUS_SUCCESS;

    do
    {
        /* Enter critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_lock(poQueueData->hMutex) < 0)
        {
            eQueueStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
            break;
        }

        /* Dequeue the element */
        ADI_QUEUE_ELEMENT *poNewUsedHead = (ADI_QUEUE_ELEMENT *)poQueueData->poUsedElemHead->pNext;
        poQueueData->poUsedElemHead->pNext = poQueueData->poFreeElemHead;
        poQueueData->poFreeElemHead = poQueueData->poUsedElemHead;
        poQueueData->poUsedElemHead = poNewUsedHead;

        /* Exit critical section */
        if(poQueueData->poMutexFuncs != NULL
                && poQueueData->poMutexFuncs->queue_mutex_unlock(poQueueData->hMutex) < 0)
        {
            eQueueStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
        }
    } while(false);

    return eQueueStatus;
}

/**
 * @brief       Is queue empty?
 *
 * @param [in]  hQueue      Queue handle
 * @return true     If queue is empty
 * @return false    If queue is not empty
 */
ADI_QUEUE_CODE_CRIT
bool adi_queue_isEmpty(ADI_QUEUE_HANDLE hQueue)
{
    /* Get the queue data from the handle */
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;

    /* If the pointer to the used element list is null, queue is empty */
    return (poQueueData->poUsedElemHead == NULL);
}

/**
 * @brief       Is queue full?
 *
 * @param [in]  hQueue      Queue handle
 * @return true     If queue is full
 * @return false    If queue is not full
 */
ADI_QUEUE_CODE_CRIT
bool adi_queue_isFull(ADI_QUEUE_HANDLE hQueue)
{
    /* Get the queue data from the handle */
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;

    /* If the pointer to the free element list is null, queue is empty */
    return (poQueueData->poFreeElemHead == NULL);
}

/**
 * @brief       Destroy the queue
 *
 * @param [in]  hQueue      Queue handle
 * @return      Status
 *              - #ADI_QUEUE_STATUS_SUCCESS     Queue successfully destroyed
 *              - #ADI_QUEUE_STATUS_MUTEX_FAILURE   Mutex API call failed
 */
ADI_QUEUE_CODE_CRIT
ADI_QUEUE_STATUS adi_queue_destroy(ADI_QUEUE_HANDLE hQueue)
{
    ADI_QUEUE_DATA *poQueueData = (ADI_QUEUE_DATA *)hQueue;
    ADI_QUEUE_STATUS eQueueStatus = ADI_QUEUE_STATUS_SUCCESS;

    /* Destroy the mutex lock */
    if(poQueueData->poMutexFuncs != NULL
            && poQueueData->poMutexFuncs->queue_mutex_destroy(poQueueData->hMutex) < 0)
    {
        eQueueStatus = ADI_QUEUE_STATUS_MUTEX_FAILURE;
    }
    else
    {
        /* Clear the internal data */
        eQueueStatus = ADI_QUEUE_STATUS_SUCCESS;
        (void)ADI_MEMSET(poQueueData, 0, sizeof(ADI_QUEUE_DATA));
    }

    return eQueueStatus;
}

/** @} */

/**
 * EOF: www.analog.com
 */
