/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : semaphore relative API
 *
 */
/*
 * Include Files
 */
#include <fcntl.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <time.h>
#include <common/error.h>
#include <osal/sem.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef sem_t   osal_linux_sem_t;

/*
 * Macro Definition
 */
  #define osal_alloc malloc
  #define osal_free free
/*
 * Function Declaration
 */

/* Function Name:
 *      osal_sem_mutex_create
 * Description:
 *      Create one mutex.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Mutex id
 * Note:
 *      It is NOT a reentrant mutex.
 */
osal_mutex_t
osal_sem_mutex_create(void)
{
    return (osal_mutex_t)osal_sem_create(1);
} /* end of osal_sem_mutex_create */


/* Function Name:
 *      osal_sem_mutex_destroy
 * Description:
 *      Destroy the mutex.
 * Input:
 *      mutex - mutex id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
osal_sem_mutex_destroy(osal_mutex_t mutex)
{
    osal_sem_destroy(mutex);
} /* end of osal_sem_mutex_destroy */


/* Function Name:
 *      osal_sem_mutex_take
 * Description:
 *      Take the mutex.
 * Input:
 *      mutex - mutex id
 *      usec  - 1. retry timeout (microseconds)
 *              2. OSAL_SEM_WAIT_FOREVER to wait until release
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to take the mutex.
 *      RT_ERR_FAILED       - fail to take the mutex.
 *      RT_ERR_NULL_POINTER - input mutex is NULL.
 * Note:
 *      None
 */
int32
osal_sem_mutex_take(osal_mutex_t mutex, uint32 usec)
{
    return osal_sem_take((osal_sem_t) mutex, usec);
} /* end of osal_sem_mutex_take */


/* Function Name:
 *      osal_sem_mutex_give
 * Description:
 *      Give the mutex.
 * Input:
 *      mutex - mutex id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - succeed in giving the mutex.
 *      RT_ERR_NULL_POINTER - mutex is invalid (NULL);
 * Note:
 *      None
 */
int32
osal_sem_mutex_give(osal_mutex_t mutex)
{
    return osal_sem_give((osal_sem_t)mutex);
} /* end of osal_sem_mutex_give */

#ifdef CONFIG_SDK_OSAL_SEM_KERNEL

/* Function Name:
 *      osal_sem_create
 * Description:
 *      Create semaphore.
 * Input:
 *      init_num - semaphore counter.
 * Output:
 *      None
 * Return:
 *      NULL   - fail to create semaphore.
 *      others - semaphore id
 * Note:
 *      None
 */
osal_sem_t
osal_sem_create(int32 init_num)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return (osal_sem_t)RT_ERR_FAILED;

    dio.data[0] = init_num;
    ioctl(fd, RTCORE_SEM_CREATE, &dio);
    
    close(fd);

    return (osal_sem_t)dio.ret;
    
} /* end of osal_sem_create */


/* Function Name:
 *      osal_sem_destroy
 * Description:
 *      Destroy semaphore.
 * Input:
 *      sem - semaphore id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
osal_sem_destroy(osal_sem_t sem)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return;

    dio.data[0] = sem;
    ioctl(fd, RTCORE_SEM_DESTROY, &dio);
    
    close(fd);
    
} /* end of osal_sem_destroy */


/* Function Name:
 *      osal_sem_take
 * Description:
 *      Take the semaphore.
 * Input:
 *      sem  - semaphore id
 *      usec - 1. retry timeout (microseconds) 
 *             2. OSAL_SEM_WAIT_FOREVER to wait until release
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to take the semaphore.
 *      RT_ERR_FAILED       - fail to take the semaphore.
 *      RT_ERR_NULL_POINTER - input sem is NULL.
 * Note:
 *      None
 */
int32
osal_sem_take(osal_sem_t sem, uint32 usec)
{    
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = sem;
    dio.data[1] = usec;
    ioctl(fd, RTCORE_SEM_TAKE, &dio);

    close(fd);
       
    return dio.ret;

} /* end of osal_sem_take */


/* Function Name:
 *      osal_sem_give
 * Description:
 *      Give the semaphore.
 * Input:
 *      sem - semaphore id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to give the semaphore.
 *      RT_ERR_NULL_POINTER - sem is invalid (NULL)
 * Note:
 *      None
 */
int32
osal_sem_give(osal_sem_t sem)
{  
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = sem;
    ioctl(fd, RTCORE_SEM_GIVE, &dio);

    close(fd);
 
    return dio.ret;
    
} /* end of osal_sem_give */

#else

/* Function Name:
 *      osal_sem_create
 * Description:
 *      Create semaphore.
 * Input:
 *      init_num - semaphore counter.
 * Output:
 *      None
 * Return:
 *      NULL   - fail to create semaphore.
 *      others - semaphore id
 * Note:
 *      None
 */
osal_sem_t
osal_sem_create(int32 init_num)
{
    osal_linux_sem_t *pS;

    /* allocate memory space for the semaphore. */
    pS = (osal_linux_sem_t *)osal_alloc(sizeof(osal_linux_sem_t));

    if (NULL == pS) 
        return (osal_sem_t)NULL;

    if(sem_init(pS, 0, init_num) < 0)
    {
        osal_free(pS);
        return (osal_sem_t)NULL;
    }

    return (osal_sem_t)pS; 
    
} /* end of osal_sem_create */


/* Function Name:
 *      osal_sem_destroy
 * Description:
 *      Destroy semaphore.
 * Input:
 *      sem - semaphore id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
osal_sem_destroy(osal_sem_t sem)
{
    osal_linux_sem_t *pS;

    pS = (osal_linux_sem_t *)sem;
    
    sem_destroy(pS);    
} /* end of osal_sem_destroy */


/* Function Name:
 *      osal_sem_take
 * Description:
 *      Take the semaphore.
 * Input:
 *      sem  - semaphore id
 *      usec - 1. retry timeout (microseconds) 
 *             2. OSAL_SEM_WAIT_FOREVER to wait until release
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to take the semaphore.
 *      RT_ERR_FAILED       - fail to take the semaphore.
 *      RT_ERR_NULL_POINTER - input sem is NULL.
 * Note:
 *      None
 */
int32
osal_sem_take(osal_sem_t sem, uint32 usec)
{    
    uint32 ts_sec, ts_msec, ts_usec;
    int32 e_status = RT_ERR_FAILED;
    struct timespec ts;
    struct timeval tv;
    osal_linux_sem_t *pS;

    pS = (osal_linux_sem_t *)sem;
    
    RT_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);

    if (OSAL_SEM_WAIT_FOREVER == usec)
    {
        /* Wait until get the semaphore. */
        e_status = sem_wait(pS) ? RT_ERR_FAILED : RT_ERR_OK;
    }
    else
    {
        ts_sec = usec / (1000 * 1000);
        ts_msec = usec / 1000;
        ts_usec = usec % 1000;

        gettimeofday(&tv, NULL);
        
        ts_msec += (tv.tv_usec) / 1000;
        if (ts_msec >= 1000)
        {
            ts_sec++;
            ts_msec -= 1000;
        }

        ts.tv_sec = tv.tv_sec + ts_sec;
        ts.tv_nsec = ts_msec * 1000 * 1000;

        e_status = sem_timedwait(pS,&ts) ? RT_ERR_FAILED : RT_ERR_OK;     
    }
    
    return e_status;
} /* end of osal_sem_take */


/* Function Name:
 *      osal_sem_give
 * Description:
 *      Give the semaphore.
 * Input:
 *      sem - semaphore id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to give the semaphore.
 *      RT_ERR_NULL_POINTER - sem is invalid (NULL)
 * Note:
 *      None
 */
int32
osal_sem_give(osal_sem_t sem)
{  
    osal_linux_sem_t *pS;

    pS = (osal_linux_sem_t *)sem;    
    RT_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);
    
    sem_post(pS);    
} /* end of osal_sem_give */

#endif /* CONFIG_SDK_OSAL_SEM_KERNEL */

