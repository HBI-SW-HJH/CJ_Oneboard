/*******************************************************************************
Copyright (c) 2023-2024 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: prj_osal.h
 * \brief: OS Abstraction Layer for the project
 * Developed by: Automotive Software and Systems team, Bangalore, India
*****************************************************************************/
/** \addtogroup osal
 *  @{
 */

#ifndef PRJ_OSAL_H
#define PRJ_OSAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================= INCLUDES ================================*/
#include <adi_common.h>       /*!< Common definitions */

/*! \cond PRIVATE */
/*================================= DEFINES =================================*/
/*! The size of the memory buffer to be allocated for the task control block.
    Note: This is set to a higher value to accomodate any type of OS. */
#define PRJ_OSAL_TCB_SIZE               (1000U)

/*! Memory size required for creating a mutex */
#define PRJ_OSAL_MUTEX_MEM_SIZE         (100U)

/*! Memory size required for creating a semaphore */
#define PRJ_OSAL_SEMAPHORE_MEM_SIZE     (100U)

#if defined(__EV_SC594_SOM__)
#define PRJ_OSAL_CODE_CRIT      __attribute__ ((section (".l2_cached_code")))
#define PRJ_OSAL_CODE_NOCRIT    __attribute__ ((section (".l3_code")))
#else
#define PRJ_OSAL_CODE_CRIT      /* __attribute__ ((section (".l2_cached_code"))) */
#define PRJ_OSAL_CODE_NOCRIT    /* __attribute__ ((section (".l3_code"))) */
#endif

/*! Maximum priority for a thread */
#if defined(__EV_SC594_SOM__)
#define PRJ_OSAL_THREAD_MAX_PRIORITY    (6U)
#else
#define PRJ_OSAL_THREAD_MAX_PRIORITY    (7U)
#endif

/*! Pointer size type */
#define PRJ_OSAL_POINTER_SIZE_TYPE      uint32_t

/*! Pointer alignment mask */
#define PRJ_OSAL_POINTER_ALIGN_MASK     (0x03U)

/*! \endcond */

/*=============================== DATA TYPES ================================*/

/**
 * @brief Status of the OSAL APIs types
 */
typedef enum PRJ_OSAL_STATUS
{
    PRJ_OSAL_STATUS_SUCCESS,    /*!< Success */
    PRJ_OSAL_STATUS_FAILURE,    /*!< Failure */
} PRJ_OSAL_STATUS;

/*! Handle type for the threads */
typedef void *PRJ_OSAL_THREAD_HANDLE;

/*! Handle type for the mutexes */
typedef void *PRJ_OSAL_MUTEX_HANDLE;

/*! Function signature of the threads */
typedef void (*PRJ_OSAL_THREAD_FUNC)(void *pArg);

/**
 * @brief Configuration for the threads
 */
typedef struct PRJ_OSAL_THREAD_CFG
{
    bool        bIsOneShot;             /*!< If this flag is set, the thread only gets executed once and exits */
    uint32_t    nPriority;              /*!< Priority of the thread (Lower the value, lower is the priority) */
    void        *pArgument;             /*!< Argument to be passed to the thread */
    uint32_t    nStackSize;             /*!< Stack size allocated (These many bytes should be allocated in pStackMem) */
    void        *pStackMem;             /*!< Stack memory (Should be 4-byte aligned) */
    uint32_t
    nTaskControlBlockSize;  /*!< Size of the TCB (These many bytes should be allocated in pTaskControlBlockMem) */
    void                 *pTaskControlBlockMem;   /*!< Task control block memory (Should be 4-byte aligned) */
    PRJ_OSAL_THREAD_FUNC  pfThread;               /*!< Pointer to the thread function */
} PRJ_OSAL_THREAD_CFG;

/**
 * @brief Configuration for mutexes
 */
typedef struct PRJ_OSAL_MUTEX_CFG
{
    uint32_t nMemSize;             /*!< Memory size for the mutex (These many bytes should be allocated in pMem) */
    void     *pMem;                /*!< Pointer to mutex memory (Should be 4-byte aligned) */
} PRJ_OSAL_MUTEX_CFG;

/**
 * @brief Configuration for the semaphores
 */
typedef struct PRJ_OSAL_SEMAPHORE_CFG
{
    uint32_t nMemSize;             /*!< Memory size for the semaphore (These many bytes should be allocated in pMem) */
    void     *pMem;                /*!< Pointer to semaphore memory (Should be 4-byte aligned) */
    bool     bCountingSemaphore;   /*!< If set true, considers the semaphore type as counting else considers it as binary */
    uint32_t nMaxCount;            /*!< Maximum count for the semaphore (incase of counting semaphores )*/
    uint32_t nInitialCount;        /*!< The count value assigned to the semaphore when it is created */
} PRJ_OSAL_SEMAPHORE_CFG;


/*! Handle type for the semaphores */
typedef void *PRJ_OSAL_SEMAPHORE_HANDLE;

/* Thread related APIs */
extern PRJ_OSAL_STATUS prj_osal_createThread(PRJ_OSAL_THREAD_CFG *poThreadCfg, PRJ_OSAL_THREAD_HANDLE *phThread);
extern PRJ_OSAL_STATUS prj_osal_destroyThread(PRJ_OSAL_THREAD_HANDLE hThread);
extern PRJ_OSAL_STATUS prj_osal_sleepThread(PRJ_OSAL_THREAD_HANDLE hThread, uint32_t nTimeInMs);

/* Scheduler APIs */
extern PRJ_OSAL_STATUS prj_osal_startScheduler(void);
extern PRJ_OSAL_STATUS prj_osal_endScheduler(void);

/* Mutex APIs */
extern PRJ_OSAL_STATUS prj_osal_createMutex(PRJ_OSAL_MUTEX_CFG *poMutexCfg, PRJ_OSAL_MUTEX_HANDLE *phMutex);
extern PRJ_OSAL_STATUS prj_osal_destroyMutex(PRJ_OSAL_MUTEX_HANDLE hMutex);
extern PRJ_OSAL_STATUS prj_osal_lockMutex(PRJ_OSAL_MUTEX_HANDLE hMutex);
extern PRJ_OSAL_STATUS prj_osal_unlockMutex(PRJ_OSAL_MUTEX_HANDLE hMutex);

/* Semaphore APIs */
extern PRJ_OSAL_STATUS prj_osal_createSemaphore(PRJ_OSAL_SEMAPHORE_CFG *poSemaphoreCfg,
        PRJ_OSAL_SEMAPHORE_HANDLE *phSemaphore);
extern PRJ_OSAL_STATUS prj_osal_destroySemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore);
extern PRJ_OSAL_STATUS prj_osal_signalSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore);
extern PRJ_OSAL_STATUS prj_osal_signalSemaphoreFromISR(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore);
extern PRJ_OSAL_STATUS prj_osal_waitSemaphore(PRJ_OSAL_SEMAPHORE_HANDLE hSemaphore);

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif /* PRJ_OSAL_H */

/** @} */

/**
 * EOF: www.analog.com
 */
