#include <linux/module.h>	/* Specifically, a module */
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/gpio.h>

/* GPIO Register Set */
#define BSP_GPIO_BASE   (0xB8003500)
#define BSP_PABCD_CNR   (0x000 + BSP_GPIO_BASE) /* Port ABCD control */
#define BSP_PABCD_PTYPE (0x004 + BSP_GPIO_BASE) /* Port ABCD type */
#define BSP_PABCD_DIR   (0x008 + BSP_GPIO_BASE) /* Port ABCD direction */
#define BSP_PABCD_DAT   (0x00C + BSP_GPIO_BASE) /* Port ABCD data */
#define BSP_PABCD_ISR   (0x010 + BSP_GPIO_BASE) /* Port ABCD interrupt status */
#define BSP_PAB_IMR     (0x014 + BSP_GPIO_BASE) /* Port AB interrupt mask */
#define BSP_PCD_IMR     (0x018 + BSP_GPIO_BASE) /* Port CD interrupt mask */
#define BSP_PEFGH_CNR   (0x01C + BSP_GPIO_BASE) /* Port ABCD control */
#define BSP_PEFGHP_TYPE (0x020 + BSP_GPIO_BASE) /* Port ABCD type */
#define BSP_PEFGH_DIR   (0x024 + BSP_GPIO_BASE) /* Port ABCD direction */
#define BSP_PEFGH_DAT   (0x028 + BSP_GPIO_BASE) /* Port ABCD data */
#define BSP_PEFGH_ISR   (0x02C + BSP_GPIO_BASE) /* Port ABCD interrupt status */
#define BSP_PEF_IMR     (0x030 + BSP_GPIO_BASE) /* Port AB interrupt mask */
#define BSP_PGH_IMR     (0x034 + BSP_GPIO_BASE) /* Port CD interrupt mask */

/*
 * PIN MUX
 */
#define BSP_PIN_MUX_SEL1        0xB8000040
#define BSP_PIN_MUX_SEL2        0xB8000044

/*
 * GPIO PIN
 */
enum BSP_GPIO_PIN
{
        BSP_GPIO_PIN_A0 = 0,
        BSP_GPIO_PIN_A1,
        BSP_GPIO_PIN_A2,
        BSP_GPIO_PIN_A3,
        BSP_GPIO_PIN_A4,
        BSP_GPIO_PIN_A5,
        BSP_GPIO_PIN_A6,
        BSP_GPIO_PIN_A7,

        BSP_GPIO_PIN_B0,
        BSP_GPIO_PIN_B1,
        BSP_GPIO_PIN_B2,
        BSP_GPIO_PIN_B3,
        BSP_GPIO_PIN_B4,
        BSP_GPIO_PIN_B5,
        BSP_GPIO_PIN_B6,
        BSP_GPIO_PIN_B7,

        BSP_GPIO_PIN_C0,
        BSP_GPIO_PIN_C1,
        BSP_GPIO_PIN_C2,
        BSP_GPIO_PIN_C3,
        BSP_GPIO_PIN_C4,
        BSP_GPIO_PIN_C5,
        BSP_GPIO_PIN_C6,
        BSP_GPIO_PIN_C7,

        BSP_GPIO_PIN_D0,
        BSP_GPIO_PIN_D1,
        BSP_GPIO_PIN_D2,
        BSP_GPIO_PIN_D3,
        BSP_GPIO_PIN_D4,
        BSP_GPIO_PIN_D5,
        BSP_GPIO_PIN_D6,
        BSP_GPIO_PIN_D7,

        BSP_GPIO_2ND_REG,
        BSP_GPIO_PIN_MAX = BSP_GPIO_2ND_REG
};

#define BSP_GPIO_BIT(pin)               (pin & ~(BSP_GPIO_2ND_REG))
#define BSP_GPIO_CNR_REG(pin)   ((pin & BSP_GPIO_2ND_REG) ? BSP_PEFGH_CNR : BSP_PABCD_CNR)
#define BSP_GPIO_DIR_REG(pin)   ((pin & BSP_GPIO_2ND_REG) ? BSP_PEFGH_DIR : BSP_PABCD_DIR)
#define BSP_GPIO_DAT_REG(pin)   ((pin & BSP_GPIO_2ND_REG) ? BSP_PEFGH_DAT : BSP_PABCD_DAT)

//-----export functions
void rtl819x_gpio_pin_enable(u32 pin);
int rtl819x_gpio_direction (unsigned pin,int dir);
void rtl819x_gpio_pin_set_val(u32 pin, int val);
int rtl819x_gpio_pin_get_val(u32 pin);

//----------------

static int rtl819x_gpio_mux(u32 pin, u32 *reg, u32 *mask, u32 *val);

static DEFINE_SPINLOCK(rtl819x_gpio_lock);

static int __rtl819x_gpio_get_value(unsigned gpio)
{
        unsigned int data;

        data = (__raw_readl((void __iomem*)BSP_GPIO_DAT_REG(gpio)) >> BSP_GPIO_BIT(gpio) ) & 1;

        return data;
}
static int __rtl819x_gpio_set_value(unsigned pin, int value)
{
        unsigned int data;

        data = __raw_readl((void __iomem*)BSP_GPIO_DAT_REG(pin));

        if (value == 0)
                data &= ~(1 << BSP_GPIO_BIT(pin));
        else
                data |= (1 << BSP_GPIO_BIT(pin));

        __raw_writel(data, (void __iomem*)BSP_GPIO_DAT_REG(pin));

        return 0;
}

static int rtl819x_gpio_direction_input(unsigned pin)
{
        unsigned long flags;

        if (pin >= BSP_GPIO_PIN_MAX)
                return -1;

        spin_lock_irqsave(&rtl819x_gpio_lock, flags);

        /* 0 : input */
        __raw_writel(__raw_readl((void __iomem*)BSP_GPIO_DIR_REG(pin)) & ~(1 << BSP_GPIO_BIT(pin)),
                                        (void __iomem*)BSP_GPIO_DIR_REG(pin));

        spin_unlock_irqrestore(&rtl819x_gpio_lock, flags);

        return 0;
}

static int rtl819x_gpio_direction_output (unsigned pin)
{
        unsigned long flags;

        if (pin >= BSP_GPIO_PIN_MAX)
                return -1;

        spin_lock_irqsave(&rtl819x_gpio_lock, flags);

        __raw_writel(__raw_readl((void __iomem*)BSP_GPIO_DIR_REG(pin)) | (1 << BSP_GPIO_BIT(pin)),
                                        (void __iomem*)BSP_GPIO_DIR_REG(pin) );

        spin_unlock_irqrestore(&rtl819x_gpio_lock, flags);

        return 0;
}

//dir = 0 input , dir = 1 uptput
int rtl819x_gpio_direction (unsigned pin,int dir)
{
	if(dir)
		rtl819x_gpio_direction_output(pin);
	else
		rtl819x_gpio_direction_input(pin);

}

void rtl819x_gpio_pin_set_val(u32 pin, int val)
{
        __rtl819x_gpio_set_value(pin, val);
}

int rtl819x_gpio_pin_get_val(u32 pin)
{
        return __rtl819x_gpio_get_value(pin);
}

int write_bsp_gpio_proc(struct file *file, const char *buffer,unsigned long count, void *data)
{     
	
	  char tmp[32], command[8], action[4];
	   unsigned int num, gpio_num, value;

      if (buffer && !copy_from_user(tmp, buffer, 32)) {
                num = sscanf(tmp, "%s %d %s", command, &gpio_num, action);

                if (num != 3) {
                        panic_printk("Invalid gpio parameter! Failed!\n");
                        return num;
                }
        }

	   if (!memcmp(command, "config", 6)) {
                if (!memcmp(action, "r", 1))
                	{
             		rtl819x_gpio_pin_enable(gpio_num);
                        rtl819x_gpio_direction(gpio_num,0);
                	}						
                else if (!memcmp(action, "w", 1))
                	{
                	rtl819x_gpio_pin_enable(gpio_num);
                        rtl819x_gpio_direction(gpio_num,1);
                	}			
                else {
                        panic_printk("Action not supported!\n");
                        
                }

		return count;
        }
        else if (!memcmp(command, "set", 3)) {
                if (!memcmp(action, "0", 1))
                         rtl819x_gpio_pin_set_val(gpio_num,0);
                else if (!memcmp(action, "1", 1))
                        rtl819x_gpio_pin_set_val(gpio_num,1);
                else {
			panic_printk("Action not supported!\n");

                }

                        return count;
		}
	 else if (!memcmp(command, "get", 3)) {
                if (!memcmp(action, "dir", 3))
                {
                      //FIXME , now always show input direction ,cz output gpio status is not real 
                      panic_printk("input");
                }				 
                else if (!memcmp(action, "val", 3))
                {
			panic_printk("%d",__rtl819x_gpio_get_value(gpio_num));
                }				
                else {
			panic_printk("Action not supported!\n");

                }

                        return count;
		}	

	return count;	
}
//########Below are platform dependent part !!

#if defined(CONFIG_RTL_8196E)
void rtl8196e_gpio_pin_enable(u32 pin)
{
   unsigned long flags;
        u32 mask = 0;
        u32 mux = 0;
        u32 reg = 0;
        u32 val = 0;

        switch(pin) {
                /*
                 * PIN SEL 1
                 */
                case BSP_GPIO_PIN_A2:
                case BSP_GPIO_PIN_A4:
                case BSP_GPIO_PIN_A5:
                case BSP_GPIO_PIN_A6:
                /* 0xB800-0040 bit 2:0 */
                mask = 0x00000007;
                val  = 0x00000006;      //3'b110
                reg  = BSP_PIN_MUX_SEL1;
                break;

                /* 0xB800-0040 bit 4:3 reserved*/

                case BSP_GPIO_PIN_A7:
                case BSP_GPIO_PIN_B0:
                /* 0xB800-0040 bit 5 */
                mask = 0x00000020;
                val  = 0x00000020; //1'b1
                reg  = BSP_PIN_MUX_SEL1;
                break;

                case BSP_GPIO_PIN_B1:
                /* 0xB800-0040 bit 6 */
                mask = 0x00000040;
                val  = 0x00000040; //1'b1
                reg  = BSP_PIN_MUX_SEL1;
                break;

                /* 0xB800-0040 bit 19:7 reserved */

                case BSP_GPIO_PIN_C1:
                /* 0xB800-0040 bit 21:20 */
                mask = 0x00300000;
                val  = 0x00300000;
                reg  = BSP_PIN_MUX_SEL1;
                break;

                case BSP_GPIO_PIN_C2:
                /* 0xB800-0040 bit 23:22 */
                mask = 0x00C00000;
                val  = 0x00C00000;
                reg  = BSP_PIN_MUX_SEL1;
                break;

                case BSP_GPIO_PIN_C3:
                /* 0xB800-0040 bit 25:24 */
                mask = 0x00C00000;
                val  = 0x00C00000;
                reg  = BSP_PIN_MUX_SEL1;
                break;

                /* 0xB800-0040 bit 31:26 reserved */

                /*
                 * PIN SEL 2
                 */
                case BSP_GPIO_PIN_B2:
                /* 0xB800-0040 bit 1:0 */
                mask = 0x00000003;
                val  = 0x00000003;
                reg  = BSP_PIN_MUX_SEL2;
                break;

                /* 0xB800-0044 bit 2 reserved */

                case BSP_GPIO_PIN_B3:
                /* 0xB800-0044 bit 4:3 */
                mask = 0x0000000C;
                val  = 0x0000000C;
                reg  = BSP_PIN_MUX_SEL2;
                break;

                /* 0xB800-0044 bit 5 reserved */

                case BSP_GPIO_PIN_B4:
                /* 0xB800-0044 bit 7:6 */
                mask = 0x000000C0;
                val  = 0x000000C0;
                reg  = BSP_PIN_MUX_SEL2;
                break;

                /* 0xB800-0044 bit 8 reserved */

                case BSP_GPIO_PIN_B5:
                /* 0xB800-0044 bit 10:9 */
                mask = 0x00000600;
                val  = 0x00000600;
                reg  = BSP_PIN_MUX_SEL2;
                break;

                /* 0xB800-0044 bit 11 reserved */

                case BSP_GPIO_PIN_B6:
                /* 0xB800-0044 bit 13:12 */
                mask = 0x00003000;
                val  = 0x00003000;
                reg  = BSP_PIN_MUX_SEL2;
                break;

                /* 0xB800-0044 bit 14:12 reserved */
		  case BSP_GPIO_PIN_B7:
                /* 0xB800-0044 bit 17:15 */
                mask = 0x00038000;
                val  = 0x00020000; //3 
                reg  = BSP_PIN_MUX_SEL2;
                break;

                /* 0xB800-0044 bit 31:18 reserved */

                default:
                /* other gpio pin are not supported */
                printk("%s doesn't support pin %d\n",__FUNCTION__,pin);
                return;
        }

        spin_lock_irqsave(&rtl819x_gpio_lock, flags);

        mux  = __raw_readl((void __iomem*) reg);
        /* set masked bit to zero */
        mux &= ~mask;

        /* set value to masked bit */
        mux |= val;

        //if ((mux & mask) == 0) {
                __raw_writel( mux, (void __iomem*) reg);

                /* 0 as BSP_GPIO pin */
                __raw_writel(__raw_readl((void __iomem*)BSP_GPIO_CNR_REG(pin))& ~(1<<BSP_GPIO_BIT(pin)),
                                                (void __iomem*)BSP_GPIO_CNR_REG(pin));
        //} //mark_fix

        spin_unlock_irqrestore(&rtl819x_gpio_lock, flags);
}
#endif
void rtl819x_gpio_pin_enable(u32 pin)
{
#if defined(CONFIG_RTL_8196E)
	rtl8196e_gpio_pin_enable(pin);
#else
	panic_printk("Need define your platform bsp gpio!!!\n");
#endif

}



