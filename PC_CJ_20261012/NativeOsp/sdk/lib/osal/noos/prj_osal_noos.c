/*******************************************************************************
Copyright (c) 202-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: prj_osal_noos.c
 * \brief: Source of NO OS(bare metal) Abstraction Layer
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup osal
 *  @{
 */

/*================================= INCLUDES ================================*/
#include "prj_osal.h"
#include "adi_dbg.h"
#include "adi_pal.h"

/*! \cond PRIVATE */
/*================================== DEFINES ================================*/
#define MAX_THREADS           (10U)     /*! Maximum number of threads */

/*================================ DATA TYPES ===============================*/
/*! OSAL thread state types */
typedef enum THREAD_STATE
{
    THREAD_STATE_BLOCKED,
    THREAD_STATE_RUNNING,
} THREAD_STATE;

/*! @brief Data for creating threads */
typedef struct THREAD_DATA
{
    bool            bIsEnabled;
    uint64_t        nWaitUntilTime;
    THREAD_STATE    eState;
    PRJ_OSAL_THREAD_CFG *poThreadCfg;
} THREAD_DATA;

/*=================================== DATA ==================================*/
static THREAD_DATA gaoThreads[MAX_THREADS];
static uint8_t gnThreadCnt;
static bool gbThreadRun = false;
static uint8_t gnSemaphoreMem = 0;    // placeholder for semaphore memory allocation
static uint8_t gnMutexMem = 0;        // placeholder for mutex memory allocation
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
    /* Preconditions */
    ADI_DBG_REQUIRE(poThreadCfg != NULL, "Pointer to thread config is null");
    ADI_DBG_REQUIRE(gnThreadCnt < MAX_THREADS, "Creating more threads than allowed");

    (void)ADI_MEMSET(&gaoThreads[gnThreadCnt], 0, sizeof(THREAD_DATA));
    gaoThreads[gnThreadCnt].poThreadCfg = poThreadCfg;
    gaoThreads[gnThreadCnt].bIsEnabled = true;
    gaoThreads[gnThreadCnt].eState = THREAD_STATE_RUNNING;

    *phThread = &gaoThreads[gnThreadCnt];
    gnThreadCnt++;

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
    /* Precondition */
    ADI_DBG_REQUIRE(hThread != NULL, "Pointer to thread handle is null");

    THREAD_DATA *poThreadData = (THREAD_DATA *)hThread;
    poThreadData->bIsEnabled = false;

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
    /* Precondition */
    ADI_DBG_REQUIRE(nTimeInMs > 0u, "Time tick is invalid");

    THREAD_DATA *poThreadData = (THREAD_DATA *)hThread;

    uint64_t nTimeToWaitNs = (uint64_t)nTimeInMs * MS_TO_NS;
    uint64_t nCurrTime;
    adi_pal_getCurrTime(&nCurrTime);
    poThreadData->nWaitUntilTime = nCurrTime + nTimeToWaitNs;

    poThreadData->eState = THREAD_STATE_BLOCKED;

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
    ADI_DBG_REQUIRE(gnThreadCnt < MAX_THREADS, "Thread count is greater than max number of thread supported");

    uint64_t nCurrTime;

    gbThreadRun = true;

    while(gbThreadRun)
    {
        for(uint8_t i = 0U; i < gnThreadCnt; i++)
        {
            if(gaoThreads[i].bIsEnabled)
            {
                if(gaoThreads[i].eState == THREAD_STATE_RUNNING)
                {
                    gaoThreads[i].poThreadCfg->pfThread(gaoThreads[i].poThreadCfg->pArgument);

                    if(gaoThreads[i].poThreadCfg->bIsOneShot)
                    {
                        gaoThreads[i].bIsEnabled = false;
                    }
                }
                else
                {
                    adi_pal_getCurrTime(&nCurrTime);

                    if(nCurrTime >= gaoThreads[i].nWaitUntilTime)
                    {
                        gaoThreads[i].eState = THREAD_STATE_RUNNING;
                    }
                }
            }
        }
    }

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
    *phMutex = &gnMutexMem;
    return PRJ_OSAL_STATUS_SUCCESS;
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
    return PRJ_OSAL_STATUS_SUCCESS;
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
    return PRJ_OSAL_STATUS_SUCCESS;
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
    *phSemaphore = &gnSemaphoreMem;
    return PRJ_OSAL_STATUS_SUCCESS;
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
    return PRJ_OSAL_STATUS_SUCCESS;
}

/**
 * @brief Signal the semaphore from ISR context
 *
 * @param [in]  hSemaphore      Handle to the semaphore that has to be released
 * @return      Status
 *              - #PRJ_OSAL_STATUS_SUCCESS  Successfully released the semaphore
 *              - #PRJ_OSAL_STATUS_FAILURE  Failed to release the semaphore
 */
PRJ_OSAL_CODE_CRIT
PRJ_OSAL_STATUS prj_osal_signalSemaphoreFromISR(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore)
{
    return PRJ_OSAL_STATUS_SUCCESS;
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
    return PRJ_OSAL_STATUS_SUCCESS;
}

/** @} */

/**
 * EOF: www.analog.com
 */
