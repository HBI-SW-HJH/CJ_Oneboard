/*******************************************************************************
Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: prj_osal_freeRTOS.c
 * \brief: Source of freeRTOS OS Abstraction Layer
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup osal
 *  @{
 */

/*================================= INCLUDES ================================*/
#include <stddef.h>
#include "adi_dbg.h"
#include "prj_osal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/*! \cond PRIVATE */
/*================================== DEFINES ================================*/

/*================================ DATA TYPES ===============================*/

/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/
static void FreeRTOSThread(void *pThreadStructObj);

/*=================================== CODE ==================================*/
/*! \endcond */
/**
 * @brief       Create a thread
 *
 * @param [in]  poThreadCfg     Configuration for the thread
 * @param [out] phThread        Handle to the thread
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully created the thread
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to create the thread
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_createThread(PRJ_OSAL_THREAD_CFG *poThreadCfg, PRJ_OSAL_THREAD_HANDLE *phThread)
{
    /*****************************
      Preconditions:
          1. Validate the pointers
          2. Validate the memory
          3. Validate the memory size
    ******************************/
    ADI_DBG_REQUIRE(poThreadCfg != NULL, "Pointer to thread config is null");
    ADI_DBG_REQUIRE(poThreadCfg->pStackMem != NULL, "Pointer to allocated stack memory is null");
    ADI_DBG_REQUIRE(((PRJ_OSAL_POINTER_SIZE_TYPE)poThreadCfg->pStackMem & PRJ_OSAL_POINTER_ALIGN_MASK) == 0U,
                    "Pointer to allocated stack memory is not aligned");
    ADI_DBG_REQUIRE(poThreadCfg->pTaskControlBlockMem != NULL, "Pointer to allocated TCB memory is null");
    ADI_DBG_REQUIRE(((PRJ_OSAL_POINTER_SIZE_TYPE)poThreadCfg->pTaskControlBlockMem & PRJ_OSAL_POINTER_ALIGN_MASK) == 0U,
                    "Pointer to allocated TCB memory is not aligned");
    ADI_DBG_REQUIRE(poThreadCfg->nStackSize >= configMINIMAL_STACK_SIZE, "Allocated memory less than required value");
    ADI_DBG_REQUIRE(poThreadCfg->nTaskControlBlockSize >= sizeof(StaticTask_t),
                    "Allocated memory less than required value");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus;

    /* Handle to the task */
    TaskHandle_t hTask;

    /* We always create the task with the FreeRTOSThread as the function.
       Based on whether it is a one-shot execution / periodic execution,
       the FreeRTOSThread will call the actual thread function passed
       in poThreadCfg->pfThread */
    hTask = xTaskCreateStatic(
                FreeRTOSThread,
                NULL,
                poThreadCfg->nStackSize / 4U,
                poThreadCfg,
                poThreadCfg->nPriority,
                poThreadCfg->pStackMem,
                poThreadCfg->pTaskControlBlockMem);

    if(hTask != NULL)
    {
        /* Task creation was successful */
        *phThread = hTask;
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }
    else
    {
        /* Task creation failed */
        *phThread = NULL;
        eOsalStatus = PRJ_OSAL_STATUS_FAILURE;
    }

    return eOsalStatus;
}

/**
 * @brief       Destroy the thread
 *
 * @param [in]  hThread     Handle to the thread
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_destroyThread(PRJ_OSAL_THREAD_HANDLE hThread)
{

    /* Precondition */
    ADI_DBG_REQUIRE(hThread != NULL, "Pointer to thread handle is null");

    /* Handle to the task */
    TaskHandle_t hTask = (TaskHandle_t)hThread;

    /* Delete the task */
    vTaskDelete(hTask);

    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief       Send the thread to sleep
 *
 * @param [in]  hThread     Handle to the thread
 * @param [in]  nTimeInMs   The number of milliSecond the thread should be in sleep
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to create the thread
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_sleepThread(PRJ_OSAL_THREAD_HANDLE hThread, uint32_t nTimeInMs)
{
    /* Delay the task for nTimeInMs */
    vTaskDelay(nTimeInMs / portTICK_PERIOD_MS);

    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief       Start the scheduler. This API will not return
 *
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_startScheduler(void)
{
    /* Start the scheduler */
    vTaskStartScheduler();

    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief       End the scheduler
 *
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 *              - #PRJ_OSAL_STATUS_FAILURE  API failure
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_endScheduler(void)
{
    /* End the scheduler */
    vTaskEndScheduler();

    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief       Create a mutex object
 *
 * @param [in]  poMutexCfg      Configuration for the mutex
 * @param [out] phMutex         Handle to the mutex
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully created the mutex
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to create the mutex
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_createMutex(PRJ_OSAL_MUTEX_CFG *poMutexCfg, PRJ_OSAL_MUTEX_HANDLE *phMutex)
{
    /*****************************
      Preconditions:
          1. Validate the pointers
          2. Validate the memory
          3. Validate the memory size
    ******************************/
    ADI_DBG_REQUIRE(poMutexCfg != NULL, "Pointer to Mutex config is null");
    ADI_DBG_REQUIRE(poMutexCfg->pMem != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(((PRJ_OSAL_POINTER_SIZE_TYPE)poMutexCfg->pMem & PRJ_OSAL_POINTER_ALIGN_MASK) == 0U,
                    "Pointer to allocated mutex memory is not aligned");
    ADI_DBG_REQUIRE(poMutexCfg->nMemSize >= sizeof(StaticSemaphore_t), "Allocated memory less than required value");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus;

    /* Handle to the mutex */
    SemaphoreHandle_t hSemaphore;

    /* Create the mutex buffer */
    StaticSemaphore_t *pMutexBuffer = (StaticSemaphore_t *)poMutexCfg->pMem;

    /* Create the mutex */
    hSemaphore = xSemaphoreCreateMutexStatic(pMutexBuffer);

    if(hSemaphore != NULL)
    {
        /* Mutex creation was successful */
        *phMutex = hSemaphore;
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }
    else
    {
        /* Mutex creation failed */
        *phMutex = NULL;
        eOsalStatus = PRJ_OSAL_STATUS_FAILURE;
    }

    return eOsalStatus;
}

/**
 * @brief       Destroy the mutex
 *
 * @param [in]  hMutex      Handle to the mutex
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_destroyMutex(PRJ_OSAL_MUTEX_HANDLE hMutex)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hMutex != NULL, "Pointer to Mutex handle is null");

    /* Handle to the mutex */
    SemaphoreHandle_t hSemaphore = (SemaphoreHandle_t)hMutex;

    /* Delete the mutex */
    vSemaphoreDelete(hSemaphore);

    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief       Lock the mutex
 *
 * @param [in]  hMutex      Handle to the mutex
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully locked the mutex
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to lock the mutex
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_lockMutex(PRJ_OSAL_MUTEX_HANDLE hMutex)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hMutex != NULL, "Pointer to Mutex handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus;

    /* Handle to the mutex */
    SemaphoreHandle_t hSemaphore = (SemaphoreHandle_t)hMutex;

    /* Lock the mutex */
    if(xSemaphoreTake(hSemaphore, (TickType_t)portMAX_DELAY) == pdTRUE)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }
    else
    {
        eOsalStatus = PRJ_OSAL_STATUS_FAILURE;
    }

    return eOsalStatus;
}

/**
 * @brief       Unlock the mutex
 *
 * @param [in]  hMutex      Handle to the mutex
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully unlocked the mutex
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to unlock the mutex
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_unlockMutex(PRJ_OSAL_MUTEX_HANDLE hMutex)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hMutex != NULL, "Pointer to Mutex handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus;

    /* Handle to the mutex */
    SemaphoreHandle_t hSemaphore = (SemaphoreHandle_t)hMutex;

    /* Unlock the mutex */
    if(xSemaphoreGive(hSemaphore) == pdTRUE)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }
    else
    {
        eOsalStatus = PRJ_OSAL_STATUS_FAILURE;
    }

    return eOsalStatus;
}


/**
 * @brief Create a semaphore object
 *
 * @param [in]    poSemaphoreCfg Configurations for the semaphore
 * @param [out]   phSemaphore    Pointer to the semaphore handle
 * @return        Status
 *                - #PRJ_OSAL_STATUS_SUCCESS  Successfully created the semaphore
 *                - #PRJ_OSAL_STATUS_FAILURE  Failed to create the semaphore
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_createSemaphore(PRJ_OSAL_SEMAPHORE_CFG *poSemaphoreCfg,
        PRJ_OSAL_SEMAPHORE_HANDLE *phSemaphore)
{
    /*****************************
      Preconditions:
          1. Validate the pointers
          2. Validate the memory
          3. Validate the memory size
          4. Validate the pointer to handle
    ******************************/
    ADI_DBG_REQUIRE(poSemaphoreCfg != NULL, "Pointer to semaphore config is null");
    ADI_DBG_REQUIRE(poSemaphoreCfg->pMem != NULL, "Pointer to allocated memory is null");
    ADI_DBG_REQUIRE(((PRJ_OSAL_POINTER_SIZE_TYPE)poSemaphoreCfg->pMem & PRJ_OSAL_POINTER_ALIGN_MASK) == 0U,
                    "Pointer to allocated semaphore memory is not aligned");
    ADI_DBG_REQUIRE(poSemaphoreCfg->nMemSize >= sizeof(StaticSemaphore_t), "Allocated memory less than required value");
    ADI_DBG_REQUIRE(phSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Handle to the semaphore */
    SemaphoreHandle_t hSemaphore;

    /* Typecast the memory given for semaphore to StaticSemaphore_t type */
    StaticSemaphore_t *pSemBuffer = (StaticSemaphore_t *)poSemaphoreCfg->pMem;

    if(!poSemaphoreCfg->bCountingSemaphore)
    {
        /* Create a binary semaphore */
        hSemaphore = xSemaphoreCreateBinaryStatic(pSemBuffer);
    }
    else
    {
        ADI_DBG_REQUIRE(poSemaphoreCfg->nInitialCount <= poSemaphoreCfg->nMaxCount,
                        "Initial count for semaphore is greater than the maximum count of the semaphore");
        ADI_DBG_REQUIRE(poSemaphoreCfg->nMaxCount != 0u, "Maximum count value is given as zero");
        /* Create a counting semaphore */
        hSemaphore = xSemaphoreCreateCountingStatic(poSemaphoreCfg->nMaxCount, poSemaphoreCfg->nInitialCount, pSemBuffer);
    }

    if(hSemaphore != NULL)
    {
        /* semaphore creation was successful */
        *phSemaphore = hSemaphore;
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
}

/**
 * @brief       Destroy the semaphore
 *
 * @param [in]  hSemaphore      Handle to the semaphore
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_destroySemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Handle to the semaphore */
    SemaphoreHandle_t hSem = (SemaphoreHandle_t)hSemaphore;

    /* Delete the semaphore */
    vSemaphoreDelete(hSem);

    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief Signal the semaphore
 *
 * @param [in]  hSemaphore      Handle to the semaphore that has to be released
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully released the semaphore
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to release the semaphore
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_signalSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Handle to the semaphore */
    SemaphoreHandle_t hSem = (SemaphoreHandle_t)hSemaphore;

    /* Release the semaphore */
    if(xSemaphoreGive(hSem) == pdTRUE)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
}

/**
 * @brief Signal the semaphore from ISR context
 *
 * @param [in]  hSemaphore      Handle to the semaphore that has to be signalled
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully signalled the semaphore
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to signal the semaphore
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_signalSemaphoreFromISR(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Handle to the semaphore */
    SemaphoreHandle_t hSem = (SemaphoreHandle_t)hSemaphore;

    /* Release the semaphore */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(xSemaphoreGiveFromISR(hSem, &xHigherPriorityTaskWoken) == pdTRUE)
    {
        /* Perform the context switch to task if its priority is greater than the current task immedaitely after the ISR context
         i.e if xHigherPriorityTaskWoken is set to true */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
}

/**
 * @brief Obtain the semaphore
 *
 * @param [in] hSemaphore Handle to the semaphore that has to be obtained
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully obtained the semaphore
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to obtain the semaphore
 */
PRJ_OSAL_STATUS prj_osal_waitSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Handle to the semaphore */
    SemaphoreHandle_t hSem = (SemaphoreHandle_t)hSemaphore;

    /* Obtain the semaphore */
    if(xSemaphoreTake(hSem, (TickType_t)portMAX_DELAY) == pdTRUE)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
}

/*! \cond PRIVATE */
/******************************* STATIC FUNCTIONS *************************************/
/*! FreeRTOS thread */
PRJ_OSAL_CODE_CRIT
static void FreeRTOSThread(void *pThreadStructObj)
{
    PRJ_OSAL_THREAD_CFG *pObj = (PRJ_OSAL_THREAD_CFG *)pThreadStructObj;

    do
    {
        pObj->pfThread(pObj->pArgument);
    } while(!pObj->bIsOneShot);

    vTaskDelete(NULL);
}

/*! \endcond */


/** @} */

/**
 * EOF: www.analog.com
*/
