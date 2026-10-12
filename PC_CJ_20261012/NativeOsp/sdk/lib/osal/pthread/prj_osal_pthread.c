/*******************************************************************************
Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: prj_osal_pthread.c
 * \brief: Source of pThread OS Abstraction Layer
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup osal
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "prj_osal.h"
#include "adi_dbg.h"
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

/*! \cond PRIVATE */
/*================================== DEFINES ================================*/
#define MAX_THREADS           (10U)     /*!< Maximum number of threads */

/*================================ DATA TYPES ===============================*/

/*=================================== DATA ==================================*/
static pthread_t gahThread[MAX_THREADS];
static PRJ_OSAL_THREAD_CFG gaoThreadCfg[MAX_THREADS];
static uint32_t gnThreadIdx = 0U;
static bool gbKernelActive = false;
static bool gbThreadRun = false;

/*================================ PROTOTYPES ===============================*/
static void *PThreadFunc(void *pThreadStructObj);

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
    /* Precondition */
    ADI_DBG_REQUIRE(poThreadCfg != NULL, "Pointer to thread config is null");

    *phThread = &gahThread[gnThreadIdx];
    (void)ADI_MEMCPY(&gaoThreadCfg[gnThreadIdx], poThreadCfg, sizeof(PRJ_OSAL_THREAD_CFG));

    if(gbKernelActive)
    {
        pthread_attr_t attr;
        struct sched_param schedParam;

        pthread_attr_init(&attr);

        // Set scheduling policy (e.g., SCHED_RR or SCHED_FIFO)
        pthread_attr_setschedpolicy(&attr, SCHED_RR);

        // Set priority from thread config
        schedParam.sched_priority = gaoThreadCfg[gnThreadIdx].nPriority;
        pthread_attr_setschedparam(&attr, &schedParam);

        // Set attribute to explicitly use the scheduling parameters
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

        if(pthread_create(&gahThread[gnThreadIdx], &attr, PThreadFunc, &gaoThreadCfg[gnThreadIdx]) != 0u)
        {
            pthread_attr_destroy(&attr);
            return PRJ_OSAL_STATUS_FAILURE;
        }

        pthread_attr_destroy(&attr);
    }

    gnThreadIdx++;

    return PRJ_OSAL_STATUS_SUCCESS;
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
    pthread_exit(NULL);
    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief       Send the thread to sleep
 *
 * @param [in]  hThread     Handle to the thread
 * @param [in]  nTimeInMs   The number of milliSecond the thread should be in sleep
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  API successful
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to send the thread to sleep
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_sleepThread(PRJ_OSAL_THREAD_HANDLE hThread, uint32_t nTimeInMs)
{
    if(usleep(nTimeInMs * MS_TO_US) < 0)
    {
        return PRJ_OSAL_STATUS_FAILURE;
    }

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
    /* Precondition */
    ADI_DBG_REQUIRE(gnThreadIdx < MAX_THREADS, "Thread count is greater than max number of thread supported");

    gbKernelActive = true;
    gbThreadRun = true;

    for(uint32_t i = 0U; i < gnThreadIdx; i++)
    {
        pthread_attr_t attr;
        struct sched_param schedParam;

        pthread_attr_init(&attr);

        // Set scheduling policy (e.g., SCHED_RR)
        pthread_attr_setschedpolicy(&attr, SCHED_RR);

        // Set priority from thread config
        schedParam.sched_priority = gaoThreadCfg[i].nPriority;
        pthread_attr_setschedparam(&attr, &schedParam);

        // Set attribute to explicitly use the scheduling parameters
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

        if(pthread_create(&gahThread[i], &attr, PThreadFunc, &gaoThreadCfg[i]) != 0u)
        {
            pthread_attr_destroy(&attr);
            return PRJ_OSAL_STATUS_FAILURE;
        }

        pthread_attr_destroy(&attr);
    }

    while(gbThreadRun);

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
    gbThreadRun = false;
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
    /* Precondition */
    ADI_DBG_REQUIRE(poMutexCfg != NULL, "Pointer to Mutex config is null");
    ADI_DBG_REQUIRE(poMutexCfg->pMem != NULL, "Pointer to allocated memory is null");

    pthread_mutex_t *lock;
    lock = (pthread_mutex_t *)(poMutexCfg->pMem);

    if(pthread_mutex_init(lock, NULL) == 0)
    {
        *phMutex = lock;
        return PRJ_OSAL_STATUS_SUCCESS;
    }

    return PRJ_OSAL_STATUS_FAILURE;
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

    if(pthread_mutex_destroy(hMutex) < 0)
    {
        return PRJ_OSAL_STATUS_FAILURE;
    }

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

    if(pthread_mutex_lock(hMutex) == 0)
    {
        return PRJ_OSAL_STATUS_SUCCESS;
    }

    return PRJ_OSAL_STATUS_FAILURE;
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

    if(pthread_mutex_unlock(hMutex) == 0)
    {
        return PRJ_OSAL_STATUS_SUCCESS;
    }

    return PRJ_OSAL_STATUS_FAILURE;
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
    /* Precondition */
    ADI_DBG_REQUIRE(poSemaphoreCfg != NULL, "Pointer to semaphore config is null");
    ADI_DBG_REQUIRE(poSemaphoreCfg->pMem != NULL, "Pointer to allocated semaphore memory is null");
    ADI_DBG_REQUIRE(phSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Create a semaphore */
    sem_t *sem = (sem_t *)(poSemaphoreCfg->pMem);

    /* Incase of binary semaphore , set the initial value of semaphore to '1' */
    if(!poSemaphoreCfg->bCountingSemaphore)
    {
        if(sem_init(sem, 0, 1) >= 0)
        {
            *phSemaphore = sem;
            eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
        }
    }
    else
    {
        ADI_DBG_REQUIRE(poSemaphoreCfg->nInitialCount <= poSemaphoreCfg->nMaxCount,
                        "Initial count for semaphore is greater than the maximum count of the semaphore");
        ADI_DBG_REQUIRE(poSemaphoreCfg->nMaxCount != 0u, "Maximum count value is given as zero");

        /* Incase of counting semaphore , set the initial value of semaphore according the structure field */
        if(sem_init(sem, 0, poSemaphoreCfg->nInitialCount) >= 0)
        {
            *phSemaphore = sem;
            eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
        }
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

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    if(sem_destroy((sem_t *)hSemaphore) == 0)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
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

    /* Release the semaphore */
    if(sem_post((sem_t *)hSemaphore) == 0)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
}

/**
 * @brief Signal the semaphore (pthread has same implementation for both ISR and non-ISR context)
 *
 * @param [in]  hSemaphore      Handle to the semaphore that has to be released
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully released the semaphore
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to release the semaphore
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_signalSemaphoreFromISR(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Release the semaphore */
    if(sem_post((sem_t *)hSemaphore) == 0)
    {
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
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_waitSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    /* pre-condition */
    ADI_DBG_REQUIRE(hSemaphore != NULL, "Pointer to semaphore handle is null");

    /* Return status */
    PRJ_OSAL_STATUS eOsalStatus = PRJ_OSAL_STATUS_FAILURE;

    /* Obtain the semaphore */
    if(sem_wait((sem_t *)hSemaphore) == 0)
    {
        eOsalStatus = PRJ_OSAL_STATUS_SUCCESS;
    }

    return eOsalStatus;
}

/*! \cond PRIVATE */
/******************************* STATIC FUNCTIONS *************************************/
/*! pthread */
PRJ_OSAL_CODE_CRIT
static void *PThreadFunc(void *pThreadStructObj)
{
    PRJ_OSAL_THREAD_CFG *pObj = (PRJ_OSAL_THREAD_CFG *)pThreadStructObj;

    do
    {
        pObj->pfThread(pObj->pArgument);
    } while(!pObj->bIsOneShot);

    return NULL;
}

/*! \endcond */


/** @} */

/**
 * EOF: www.analog.com
 */
