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

#define ADCCON 0x126C0000
#define ADCDAT 0x126C000C
#define ADCMUX 0x126C001C
 
int major = 12;
int minor = 0;
int myadc_num = 1;
 
struct myadc_dev
{
    struct cdev mydev;
 
    volatile unsigned long *adc_con;
    volatile unsigned long *adc_dat;
    volatile unsigned long *adc_mux;

	struct class *pcls;
    struct device *pdev;
};
 
struct myadc_dev *pgmydev = NULL;
 
int myadc_open(struct inode *pnode, struct file *pfile)
{
    pfile->private_data =(void*)(container_of(pnode->i_cdev, struct myadc_dev, mydev));
    return 0;
}
 
int myadc_close(struct inode *pnode, struct file *pfile)
{
    return 0;
}

ssize_t myadc_read(struct file* pfile, char __user *puser, size_t count, loff_t* p_pos)
{
	struct myadc_dev *pmydev = (struct myadc_dev *)pfile->private_data;
	unsigned int adcValue;
	int ret = 0;

    writel(readl(pmydev->adc_con) | (0x1 << 0), pmydev->adc_con);
    while (!(readl(pmydev->adc_con) & (0x1 << 15)));
    adcValue = (readl(pmydev->adc_dat) & 0xFFF);

	ret = copy_to_user(puser, &adcValue, count);
	if (ret) {
		printk("copy_to_user failed\n");
		return -1;
	}

	return count;
}

struct file_operations myops = {
    .owner = THIS_MODULE,
    .open = myadc_open,
    .release = myadc_close,
    .read = myadc_read,
};
 
void ioremap_ledreg(struct myadc_dev *pmydev)
{
    pmydev->adc_con = ioremap(ADCCON, 4);
    pmydev->adc_dat = ioremap(ADCDAT, 4);
    pmydev->adc_mux = ioremap(ADCMUX, 4);
}
 
void iounmap_ledreg(struct myadc_dev *pmydev)
{
    iounmap(pmydev->adc_con);
    iounmap(pmydev->adc_dat);
    iounmap(pmydev->adc_mux);
    pmydev->adc_con = NULL;
    pmydev->adc_dat = NULL;
    pmydev->adc_mux = NULL;
}
 
void init_adc(struct myadc_dev *pmydev)
{
    writel(readl(pmydev->adc_con) | (0x1 << 16), pmydev->adc_con);
    writel(readl(pmydev->adc_con) | (0x1 << 14), pmydev->adc_con);
    writel((readl(pmydev->adc_con) & (~(0xFF) << 6)) | (19 << 6), pmydev->adc_con);
    writel(readl(pmydev->adc_con) & (~(0x1) << 2), pmydev->adc_con);
    writel(readl(pmydev->adc_con) & (~(0x1) << 1), pmydev->adc_con);
    writel(0x3, pmydev->adc_mux);
}
 
int __init myadc_init(void)
{
	int ret = 0;
	dev_t devno = MKDEV(major, minor);
 
	/* 申请设备号 */
	ret = register_chrdev_region(devno, myadc_num, "myadc");
	if (ret) {
		ret = alloc_chrdev_region(&devno, minor, myadc_num, "myadc");
		if (ret) {
			printk("get devno faile\n");
			return -1;
		}
		major = MAJOR(devno);
	}
 
    pgmydev = (struct myadc_dev*)kmalloc(sizeof(struct myadc_dev), GFP_KERNEL);
    if (NULL == pgmydev) {
        unregister_chrdev_region(devno, myadc_num);
        printk("kmalloc failed\n");
        return -1;
    }
    memset(pgmydev, 0, sizeof(struct myadc_dev));
 
	/* 给struct cdev对象指定操作函数集 */
	cdev_init(&pgmydev->mydev, &myops);
 
	/* 给struct cdev对象添加到内核对应的数据结构里 */
	pgmydev->mydev.owner = THIS_MODULE;
	cdev_add(&pgmydev->mydev, devno, myadc_num);
 
	/* ioremap */
	ioremap_ledreg(pgmydev);
 
	/* con-register set output */
    init_adc(pgmydev);
 
	pgmydev->pcls = class_create(THIS_MODULE, "adc_class");
    if (IS_ERR(pgmydev->pcls)) {
        printk("class_create failed\n");
        cdev_del(&pgmydev->mydev);
	    unregister_chrdev_region(devno, myadc_num);
        return -1;
    }
 
	pgmydev->pdev = device_create(pgmydev->pcls, NULL, devno, NULL, "adcdev");
    if (NULL == pgmydev->pdev) {
        printk("device_create failed\n");
        class_destroy(pgmydev->pcls);
        cdev_del(&pgmydev->mydev);
	    unregister_chrdev_region(devno, myadc_num);
        return -1;
    }
 
	return 0;
}
 
void __exit myadc_exit(void)
{
	dev_t devno = MKDEV(major, minor);
 
	/* iounmap */
    iounmap_ledreg(pgmydev);
 
	device_destroy(pgmydev->pcls, devno);
    class_destroy(pgmydev->pcls);
	cdev_del(&pgmydev->mydev);
	unregister_chrdev_region(devno, myadc_num);
 
    kfree(pgmydev);
    pgmydev = NULL;
}
 
 
MODULE_LICENSE("GPL");
 
module_init(myadc_init);
module_exit(myadc_exit);