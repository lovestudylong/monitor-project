#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "pwmdrv.h"
 
typedef struct {
    unsigned int CON;
    unsigned int DAT;
    unsigned int PUD;
    unsigned int DRV;
    unsigned int CONPDN;
    unsigned int PUDPDN;
}gpd0;
#define GPD0 (* (volatile gpd0 *)0x114000A0)

typedef struct {
    unsigned int	TCFG0;
    unsigned int	TCFG1;
    unsigned int	TCON;
    unsigned int	TCNTB0;
    unsigned int	TCMPB0;
    unsigned int	TCNTO0;
    unsigned int	TCNTB1;
    unsigned int	TCMPB1;
    unsigned int	TCNTO1;
    unsigned int	TCNTB2;
    unsigned int	TCMPB2;
    unsigned int	TCNTO2;
    unsigned int	TCNTB3;
    unsigned int	TCMPB3;
    unsigned int	TCNTO3;
    unsigned int	TCNTB4;
    unsigned int	TCNTO4;
    unsigned int	TINT_CSTAT;
}pwm;
#define PWM (* (volatile pwm *)0x139D0000)
 
int major = 13;
int minor = 0;
int mypwm_num = 1;
 
struct mypwm_dev
{
    struct cdev mydev;
 
    volatile unsigned long *gpd0_con;
    volatile unsigned long *pwm_tcfg0;
    volatile unsigned long *pwm_tcfg1;
    volatile unsigned long *pwm_tcon;
    volatile unsigned long *pwm_tcntb0;
    volatile unsigned long *pwm_tcmpb0;

	struct class *pcls;
    struct device *pdev;
};
 
struct mypwm_dev *pgmydev = NULL;
 
int mypwm_open(struct inode *pnode, struct file *pfile)
{
    pfile->private_data =(void*)(container_of(pnode->i_cdev, struct mypwm_dev, mydev));
    return 0;
}
 
int mypwm_close(struct inode *pnode, struct file *pfile)
{
    return 0;
}
 
long mypwm_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
    struct mypwm_dev *pmydev = (struct mypwm_dev*)pfile->private_data;
 
    switch (cmd) {
        case MY_PWM_ON:
            writel(readl(pmydev->pwm_tcon) | (0x1 << 0), pmydev->pwm_tcon);
            break;
        case MY_PWM_OFF:
            writel(readl(pmydev->pwm_tcon) & (~(0x1) << 0), pmydev->pwm_tcon);
            break;
        default:
            return -1;
    }
 
    return 0;
}
 
struct file_operations myops = {
    .owner = THIS_MODULE,
    .open = mypwm_open,
    .release = mypwm_close,
    .unlocked_ioctl = mypwm_ioctl,
};
 
void ioremap_ledreg(struct mypwm_dev *pmydev)
{
    pmydev->gpd0_con = ioremap((unsigned long)&GPD0.CON, 4);
    pmydev->pwm_tcfg0 = ioremap((unsigned long)&PWM.TCFG0, 4);
    pmydev->pwm_tcfg1 = ioremap((unsigned long)&PWM.TCFG1, 4);
	pmydev->pwm_tcon = ioremap((unsigned long)&PWM.TCON, 4);
	pmydev->pwm_tcntb0 = ioremap((unsigned long)&PWM.TCNTB0, 4);
	pmydev->pwm_tcmpb0 = ioremap((unsigned long)&PWM.TCMPB0, 4);
}
 
void iounmap_ledreg(struct mypwm_dev *pmydev)
{
    iounmap(pmydev->gpd0_con);
    iounmap(pmydev->pwm_tcfg0);
    iounmap(pmydev->pwm_tcfg1);
    iounmap(pmydev->pwm_tcon);
    iounmap(pmydev->pwm_tcntb0);
    iounmap(pmydev->pwm_tcmpb0);
    pmydev->gpd0_con = NULL;
    pmydev->pwm_tcfg0 = NULL;
    pmydev->pwm_tcfg1 = NULL;
    pmydev->pwm_tcon = NULL;
    pmydev->pwm_tcntb0 = NULL;
    pmydev->pwm_tcmpb0 = NULL;
}
 
void init_pwm(struct mypwm_dev *pmydev)
{
    writel((readl(pmydev->gpd0_con) & (~(0xF))) | (0x2), pmydev->gpd0_con);
	writel((readl(pmydev->pwm_tcfg0) & (~(0xFF))) | 99, pmydev->pwm_tcfg0);
    writel(readl(pmydev->pwm_tcfg1) & (~(0xF)), pmydev->pwm_tcfg1);
    writel(readl(pmydev->pwm_tcon) | (0x1 << 3), pmydev->pwm_tcon);
    writel(2000, pmydev->pwm_tcntb0);
    writel(1000, pmydev->pwm_tcmpb0);
    writel(readl(pmydev->pwm_tcon) | (0x1 << 1), pmydev->pwm_tcon);
    writel(readl(pmydev->pwm_tcon) & (~(0x1) << 1), pmydev->pwm_tcon);
    writel(readl(pmydev->pwm_tcon) & (~(0x1) << 0), pmydev->pwm_tcon);
}
 
int __init mypwm_init(void)
{
	int ret = 0;
	dev_t devno = MKDEV(major, minor);
 
	/* 申请设备号 */
	ret = register_chrdev_region(devno, mypwm_num, "mypwm");
	if (ret) {
		ret = alloc_chrdev_region(&devno, minor, mypwm_num, "mypwm");
		if (ret) {
			printk("get devno faile\n");
			return -1;
		}
		major = MAJOR(devno);
	}
 
    pgmydev = (struct mypwm_dev*)kmalloc(sizeof(struct mypwm_dev), GFP_KERNEL);
    if (NULL == pgmydev) {
        unregister_chrdev_region(devno, mypwm_num);
        printk("kmalloc failed\n");
        return -1;
    }
    memset(pgmydev, 0, sizeof(struct mypwm_dev));
 
	/* 给struct cdev对象指定操作函数集 */
	cdev_init(&pgmydev->mydev, &myops);
 
	/* 给struct cdev对象添加到内核对应的数据结构里 */
	pgmydev->mydev.owner = THIS_MODULE;
	cdev_add(&pgmydev->mydev, devno, mypwm_num);
 
	/* ioremap */
	ioremap_ledreg(pgmydev);
 
    init_pwm(pgmydev);
 
	pgmydev->pcls = class_create(THIS_MODULE, "pwm_class");
    if (IS_ERR(pgmydev->pcls)) {
        printk("class_create failed\n");
        cdev_del(&pgmydev->mydev);
	    unregister_chrdev_region(devno, mypwm_num);
        return -1;
    }
 
	pgmydev->pdev = device_create(pgmydev->pcls, NULL, devno, NULL, "pwmdev");
    if (NULL == pgmydev->pdev) {
        printk("device_create failed\n");
        class_destroy(pgmydev->pcls);
        cdev_del(&pgmydev->mydev);
	    unregister_chrdev_region(devno, mypwm_num);
        return -1;
    }
 
	return 0;
}
 
void __exit mypwm_exit(void)
{
	dev_t devno = MKDEV(major, minor);
 
	/* iounmap */
    iounmap_ledreg(pgmydev);
 
	device_destroy(pgmydev->pcls, devno);
    class_destroy(pgmydev->pcls);
	cdev_del(&pgmydev->mydev);
	unregister_chrdev_region(devno, mypwm_num);
 
    kfree(pgmydev);
    pgmydev = NULL;
}
 
 
MODULE_LICENSE("GPL");
 
module_init(mypwm_init);
module_exit(mypwm_exit);