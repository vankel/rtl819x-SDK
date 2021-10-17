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
 *
 *
 * $Revision:  $
 * $Date: 2011-04-19 $
 *
 * Purpose : IRQ API
 *
 * Feature : Provide the APIs to register/deregister IRQ
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

#include <bspchip.h>
#if defined(OLD_FPGA_DEFINED)
#include <gpio.h>
#else
#ifndef CONFIG_RTL_8198B
#include <bspchip_8686.h>
#endif
#endif

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/print.h>
#include <dal/apollo/dal_apollo.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/irq.h>
#include <rtk/gpon.h>
#include <dal/apollo/gpon/gpon_platform.h>
#include <rtk/intr.h>

/*
 * Symbol Definition
 */

	
#if defined(OLD_FPGA_DEFINED)
#define APOLLO_IRQ 16 /* TBD */
#elif defined(FPGA_DEFINED)
#define APOLLO_IRQ BSP_GPIO1_IRQ
#else
#define APOLLO_IRQ BSP_SWITCH_IRQ
#endif

/*
 * Data Declaration
 */
static struct net_device switch_dev;
static struct tasklet_struct switch_tasklets;
static int32 irq_init = {INIT_NOT_COMPLETED};


static rtk_irq_data_t irq_isr[INTR_TYPE_END-1];

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      switch_interrupt_bh
 * Description:
 *      switch interrupt bottom half
 * Input:
 *      data            - user data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called in interrupt process or a polling thread
 */
void switch_interrupt_bh(uint32 data)
{

	uint32 imrValue;
	rtk_enable_t status;
	int32 i,ret;
	
	GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"switch_interrupt_bh 0x%x", data);

	/*loop for check all of interrupt type & isr, explict INTR_TYPE_ALL*/
	for(i=0; i < (INTR_TYPE_END - 1) ; i++)
	{
		/*get status of interrupt type*/
		if((ret=rtk_intr_ims_get(i,&status)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_INTR | MOD_DAL), "");			
			GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"rtk_intr_ims_get failed, ret=%d",data);
           		 return ;
		}
		/*if status enabled and isr is registered*/
		if((status== ENABLED) && (irq_isr[i].isr != NULL))
		{
			irq_isr[i].isr();
		}
	}

	/* restore all of interrupt */  
	if((ret=rtk_intr_imr_restore(data)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");		
		GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"imrMask failed data=%d",data);
	    return ;
	}
	GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"sw Int, restore IMR");

	return;
}


/* Function Name:
 *      switch_interrupt_th
 * Description:
 *      switch interrupt top half
 * Input:
 *      irq
 *      dev_instance
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
static irqreturn_t switch_interrupt_th(int irq, void *dev_instance)
{
    uint32 data;
	int32  ret ;

#if defined(OLD_FPGA_DEFINED)
    /* disable interrupt */
    data = 0x3 << (GPIO_B_2*2);
    REG32(GPIO_PAB_IMR) = REG32(GPIO_PAB_IMR)&(~data);

    REG32(0xB8003510) = 0x00000400;      /*  reg.PABCD_ISR : write 1 to clear PORT B pin 2 */

    tasklet_hi_schedule(&switch_tasklets);

    /* enable interrupt */
    data = 0x1 << (GPIO_B_2*2);
    REG32(GPIO_PAB_IMR) = REG32(GPIO_PAB_IMR)|(data);

#else

	/*get current interrupt register*/
    if((ret=rtk_intr_imr_get(INTR_TYPE_ALL,&data)) != RT_ERR_OK)
    {
    	RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
	return IRQ_RETVAL(IRQ_HANDLED);
    }
	
    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"switch_interrupt_th 0x%x", data);
    switch_tasklets.data = data;

	/* disable all of interrupt */  
	if((ret=rtk_intr_imr_set(INTR_TYPE_ALL,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
		return IRQ_RETVAL(IRQ_HANDLED);

	}
	
	GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"sw Int, turn off all IMR");
    tasklet_hi_schedule(&switch_tasklets);
#endif

    return IRQ_RETVAL(IRQ_HANDLED);
}

/* Function Name:
 *      rtk_switch_irq_init
 * Description:
 *      IRQ module init
 * Input:
 *      irq_id      - IRQ ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_switch_irq_init(uint32 irq_id)
{
    uint32 data;
	int32  ret;

	/*find irq_id is user define or from header file*/
	if(irq_id == IRQ_ID_NO_CARE){
		irq_id = APOLLO_IRQ;
	}
	osal_printf("%s: irq_id = %d\n",__FUNCTION__,irq_id);
    memset(&switch_tasklets, 0, sizeof(struct tasklet_struct));
    switch_tasklets.func=(void (*)(unsigned long))switch_interrupt_bh;
    switch_tasklets.data=(unsigned long)NULL;

    /* switch interrupt clear all mask */	
	if((ret=rtk_intr_imr_set(INTR_TYPE_ALL,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
	}

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"sw Int, turn off all IMR");

    /* clear switch interrupt state */
	if((ret=rtk_intr_ims_clear(INTR_TYPE_ALL)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
	}

#if defined(OLD_FPGA_DEFINED)
	/* switch interrupt polarity set to low */
	if((ret=rtk_intr_polarity_set(INTR_POLAR_LOW)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        return ret;
	}
#else
    /* switch interrupt polarity set to high */
	if((ret=rtk_intr_polarity_set(INTR_POLAR_HIGH)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
		return ret;
	}
#endif

    /* IRQ register */
    osal_memset(&switch_dev, 0x0, sizeof(switch_dev));
    switch_dev.irq = irq_id;
    osal_strcpy(switch_dev.name,"apl_sw");

    if(INIT_NOT_COMPLETED == irq_init)
    {
        if(request_irq(switch_dev.irq, (irq_handler_t)switch_interrupt_th, IRQF_DISABLED, switch_dev.name, &switch_dev))
        {
            osal_printf("request_irq apollo Failed!!\n\r");
            return RT_ERR_NOT_INIT;
        }
        else
        {
            irq_init = INIT_COMPLETED;
        }
    }

#if defined(OLD_FPGA_DEFINED)
    /* �Q�� GPIO_B_2 (I/O) ��interrupt (�HRTL8672 4P+WIFI ���ҡA�o��PIN�Ԩ�reset button) */
    /*  reg.PABCD_DIR     : set direction configuration of PORT B pin 2 to b'0
                            (b'0:input pin , b'1:output pin)   */
    gpioConfig(GPIO_B_2,GPIO_FUNC_INPUT);
    /*  reg.PAB_IMR       : set interrupt mode of PORT B pin 2 to  b'01
                            (b'00=Disable interrupt
                             b'01=Enable falling edge interrupt
                             b'11=Enable both falling or rising edge interrupt) */
    data = 0x1 << (GPIO_B_2*2);
    REG32(GPIO_PAB_IMR) = REG32(GPIO_PAB_IMR)|(data);
#endif

	/*initial isr*/
	if((ret=rtk_irq_isr_unregister(INTR_TYPE_ALL)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
		return ret;
	}
	
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_switch_irq_exist
 * Description:
 *      IRQ module exist
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_switch_irq_exit(void)
{	
	int32 ret;
    /* IRQ de-register */
    if( INIT_COMPLETED == irq_init)
    {
        free_irq(switch_dev.irq, &switch_dev);
        irq_init = INIT_NOT_COMPLETED;
		/*initial isr*/
		if((ret=rtk_irq_isr_unregister(INTR_TYPE_ALL)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
			return ret;
		}
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_irq_isr_register
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
int32 
rtk_irq_isr_register(rtk_intr_type_t intr, void (*fun)(void))
{
	int32   ret;	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

	/* parameter check */
	RT_PARAM_CHK((NULL == fun), RT_ERR_NULL_POINTER);	
	RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);	

	/* hook function point to isr_mapper*/
	irq_isr[intr].isr = fun;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_irq_isr_unregister
 * Description:
 *      Register isr handler
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
int32 
rtk_irq_isr_unregister(rtk_intr_type_t intr){

	int32   ret,i;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(irq_init);

	/* parameter check */
	RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);	

	/* un-hook function point to isr_mapper*/
	if(INTR_TYPE_ALL == intr)
	{	
		for(i = 0; i < INTR_TYPE_ALL; i++)
        {
			irq_isr[i].isr = NULL;
        }
	}
	else
	{
			irq_isr[intr].isr = NULL;
	}
	
    return RT_ERR_OK;

}


