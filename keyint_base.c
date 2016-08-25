/*
 * Interrupt Example: request_irq
*/
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/irq.h>

//#include <asm/arch/regs-gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>


#define DRV_NAME "keyint"


struct mds2450_key_detection {
    int             irq;
    int             pin;
    int             pin_setting;
    char            *name;
    int             last_state;
};

static struct mds2450_key_detection mds2450_gd = {
    IRQ_EINT0, 
	S3C2410_GPF(0), 
	S3C2410_GPF0_EINT0,
	IRQ_EINT1,
	S3C2410_GPF(1),
	S3C2410_GPF1_EINT1,
	"key-detect", 
	0
};

// TODO: ISR(Interrupt Service Routine) implementation
static irqreturn_t mds2450_keyevent(int irq, void *dev_id, struct pt_regs *regs)
{
	// TODO here
	unsigned long flags;

	local_irq_save(flags);
	local_irq_disable();

	printk("intterupt occur\n");
	local_irq_restore(flags);
	return IRQ_HANDLED;
}

static int __init mds2450_keyint_init(void)
{
	int ret;
	// POINT: initialize the EINT0 
	s3c2410_gpio_cfgpin(S3C2410_GPF(0), S3C2410_GPF0_EINT0);
	writel(readl(S3C2410_EXTINT0) & (~(0xf << 0)), S3C2410_EXTINT0);	
	writel(readl(S3C2410_EXTINT0) | (0x2 << 0), S3C2410_EXTINT0);
	// falling Edge interrupr
	
	s3c2410_gpio_cfgpin(S3C2410_GPF(1), S3C2410_GPF1_EINT1);
	writel(readl(S3C2410_EXTINT1) & (~(0xf << 0)), S3C2410_EXTINT1);	
	writel(readl(S3C2410_EXTINT1) | (0x2 << 0), S3C2410_EXTINT1);
	// TODO: register the key interrupt (IRQ_EINT0)
	if(request_irq(IRQ_EINT0,(void *)mds2450_keyevent,IRQF_DISABLED|IRQF_TRIGGER_FALLING, DRV_NAME, NULL))
	{
       printk("failed to request external interrupt.\n");
       ret = -ENOENT;
       return ret;
    }
	printk(KERN_INFO "%s successfully loaded\n", DRV_NAME);

	if(request_irq(IRQ_EINT1,(void *)mds2450_keyevent,IRQF_DISABLED|IRQF_TRIGGER_FALLING, DRV_NAME, NULL))
	{
       printk("failed to request external interrupt.\n");
       ret = -ENOENT;
       return ret;
    }
    return 0;
}

static void __exit mds2450_keyint_exit(void)
{
    // TODO: free the irq
	free_irq(IRQ_EINT0,NULL);
	free_irq(IRQ_EINT1,NULL);
    printk(KERN_INFO "%s successfully removed\n", DRV_NAME);
}

module_init(mds2450_keyint_init);
module_exit(mds2450_keyint_exit);

MODULE_LICENSE("GPL");

