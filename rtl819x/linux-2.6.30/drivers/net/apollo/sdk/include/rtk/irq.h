/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date: 2011-04-19 $
 *
 * Purpose : Definition of IRQ API
 *
 * Feature : Provide the APIs to register/deregisger switch IRQ
 *
 */

#ifndef __RTK_IRQ_H__
#define __RTK_IRQ_H__


/*
 * Include Files
 */
#include <rtk/intr.h>


#define IRQ_ID_NO_CARE -1
/*structure define*/
typedef struct rtk_irq_data_s 
{	
	int32 (*isr)(void);
			
}rtk_irq_data_t;


/* Function Name:
 *      rtk_switch_irq_init
 * Description:
 *      IRQ register
 * Input:
 *      irq_id      - IRQ ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_switch_irq_init(uint32 irq_id);

/* Function Name:
 *      rtk_switch_irq_exist
 * Description:
 *      IRQ deregister
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_switch_irq_exit(void);


/* Function Name:
 *      rtk_switch_irq_register
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_irq_isr_register(rtk_intr_type_t intr, void (*fun)(void));


/* Function Name:
 *      rtk_irq_isr_unregister
 * Description:
 *     Unregister isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_irq_isr_unregister(rtk_intr_type_t intr);


#endif  /* __RTK_IRQ_H__ */
