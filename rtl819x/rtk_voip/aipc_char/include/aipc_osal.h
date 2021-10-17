#ifndef __AIPC_OSAL_H__
#define __AIPC_OSAL_H__

#ifdef __KERNEL__

// interrupt context lock
#define AIPC_OSAL_INTERRUPT_FLAGS     ul32_t

#ifndef CONFIG_VOIP_HOST_SMP_LOCK
#define AIPC_OSAL_INTERRUPT_DISABLE(x)   local_irq_save(x)
#define AIPC_OSAL_INTERRUPT_ENABLE(x)    local_irq_restore(x)
#else
#if 1
extern spinlock_t lock_aipc_osal;
#define AIPC_OSAL_INTERRUPT_DISABLE(x)   spin_lock_irqsave(&lock_aipc_osal, x)
#define AIPC_OSAL_INTERRUPT_ENABLE(x)    spin_unlock_irqrestore(&lock_aipc_osal, x)
#else //debug version
extern int lock_aipc_owner;
#define AIPC_OSAL_INTERRUPT_DISABLE(x)	\
	do { \
         if(lock_aipc_owner!=smp_processor_id()) \
             spin_lock_irqsave(&lock_aipc_osal, (x)); \
         else \
         	printk("\x1B[31m" "[%s %s %d] recursion detection\n" "\x1B[0m", __FILE__, __FUNCTION__,__LINE__); \
         lock_aipc_owner=smp_processor_id();\
     } while(0)
#define AIPC_OSAL_INTERRUPT_ENABLE(x)	\
	do { \
         lock_aipc_owner=-1; \
         spin_unlock_irqrestore(&lock_aipc_osal, (x)); \
     } while(0)
#endif
#endif // CONFIG_VOIP_HOST_SMP_LOCK

// thread context lock
#define AIPC_OSAL_MUTEX_LOCK          down
#define AIPC_OSAL_MUTEX_UNLOCK        up

// schedule
#define AIPC_OSAL_SCHEDULE            schedule
#define AIPC_OSAL_SCHEDULE_TIMEOUT_INTERRUPTIBLE    schedule_timeout_interruptible
#define AIPC_OSAL_MDELAY              mdelay

// isr 
#define AIPC_OSAL_ISR_RET_TYPE        irqreturn_t
#define AIPC_OSAL_ISR_HANDLE          IRQ_HANDLED
#define AIPC_OSAL_ISR_CALLDSR         

#elif defined(__ECOS)
// interrupt context lock
#define AIPC_OSAL_INTERRUPT_FLAGS     cyg_uint32 
#define AIPC_OSAL_INTERRUPT_DISABLE   HAL_DISABLE_INTERRUPTS
#define AIPC_OSAL_INTERRUPT_ENABLE    HAL_RESTORE_INTERRUPTS

// thread context lock
#define AIPC_OSAL_MUTEX_LOCK          cyg_mutex_lock
#define AIPC_OSAL_MUTEX_UNLOCK        cyg_mutex_unlock

// schedule
#define AIPC_OSAL_SCHEDULE            cyg_thread_yield
#define AIPC_OSAL_SCHEDULE_TIMEOUT_INTERRUPTIBLE    error     //no this in eCos
#define AIPC_OSAL_MDELAY              error                   //no this in eCos

// isr 
#define AIPC_OSAL_ISR_RET_TYPE        cyg_uint32
#define AIPC_OSAL_ISR_HANDLE          CYG_ISR_HANDLED
#define AIPC_OSAL_ISR_CALLDSR         CYG_ISR_CALL_DSR

#endif

#endif
