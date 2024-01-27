/*
 *  drv/mpu6050drv.c
 *
 *  (C) 2024  Chen Zichao
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "mpu6050.h"

#define SMPLRT_DIV   	0x19
#define CONFIG       	0x1A
#define GYRO_CONFIG  	0x1B
#define ACCEL_CONFIG 	0x1C

#define ACCEL_XOUT_H 	0x3B
#define ACCEL_XOUT_L 	0x3C
#define ACCEL_YOUT_H 	0x3D
#define ACCEL_YOUT_L 	0x3E
#define ACCEL_ZOUT_H 	0x3F
#define ACCEL_ZOUT_L 	0x40
#define TEMP_OUT_H   	0x41
#define TEMP_OUT_L   	0x42
#define GYRO_XOUT_H  	0x43
#define GYRO_XOUT_L  	0x44
#define GYRO_YOUT_H  	0x45
#define GYRO_YOUT_L  	0x46
#define GYRO_ZOUT_H  	0x47
#define GYRO_ZOUT_L  	0x48

#define PWR_MGMT_1   	0x6B

int major = 11;
int minor = 0;
int mpu6050_num  = 1;

struct mpu6050_dev
{
	struct cdev mydev;
	struct i2c_client *pclt;

	struct class *pcls;
    struct device *pdev;
};

struct mpu6050_dev *pgmydev = NULL;

int mpu6050_read_byte(struct i2c_client *pclt, unsigned char reg)
{
	int ret = 0;
	char txbuf[1] = {reg};
	char rxbuf[1] = {0};

	struct i2c_msg msg[2] = {
		{pclt->addr, 0, 1, txbuf},
		{pclt->addr, I2C_M_RD, 1, rxbuf}
	};

	ret = i2c_transfer(pclt->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0)	{
		printk("ret = %d, in mpu6050_read_byte\n", ret);
		return ret;
	}

	return rxbuf[0];
}


int mpu6050_write_byte(struct i2c_client *pclt,unsigned char reg,unsigned char val)
{
	int ret = 0;
	char txbuf[2] = {reg,val};

	struct i2c_msg msg[1] = {
		{pclt->addr, 0, 2, txbuf},
	};

	ret = i2c_transfer(pclt->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0)	{
		printk("ret = %d, in mpu6050_write_byte\n", ret);
		return ret;
	}

	return 0;
}


int mpu6050_open(struct inode *pnode, struct file *pfile)
{
	pfile->private_data =(void *)(container_of(pnode->i_cdev, struct mpu6050_dev, mydev));
	return 0;
}

int mpu6050_close(struct inode *pnode, struct file *pfile)
{
	return 0;
}


long mpu6050_ioctl(struct file *pfile,unsigned int cmd,unsigned long arg)
{
	struct mpu6050_dev *pmydev = (struct mpu6050_dev *)pfile->private_data;
	union mpu6050_data data;

	switch(cmd)
	{
		case GET_ACCEL:
			data.accel.x = mpu6050_read_byte(pmydev->pclt, ACCEL_XOUT_L);
			data.accel.x |= mpu6050_read_byte(pmydev->pclt, ACCEL_XOUT_H) << 8;
			
			data.accel.y = mpu6050_read_byte(pmydev->pclt, ACCEL_YOUT_L);
			data.accel.y |= mpu6050_read_byte(pmydev->pclt, ACCEL_YOUT_H) << 8;

			data.accel.z = mpu6050_read_byte(pmydev->pclt, ACCEL_ZOUT_L);
			data.accel.z |= mpu6050_read_byte(pmydev->pclt, ACCEL_ZOUT_H) << 8;
			break;
		case GET_GYRO:
			data.gyro.x = mpu6050_read_byte(pmydev->pclt, GYRO_XOUT_L);
			data.gyro.x |= mpu6050_read_byte(pmydev->pclt, GYRO_XOUT_H) << 8;
			
			data.gyro.y = mpu6050_read_byte(pmydev->pclt, GYRO_YOUT_L);
			data.gyro.y |= mpu6050_read_byte(pmydev->pclt, GYRO_YOUT_H) << 8;

			data.gyro.z = mpu6050_read_byte(pmydev->pclt, GYRO_ZOUT_L);
			data.gyro.z |= mpu6050_read_byte(pmydev->pclt, GYRO_ZOUT_H) << 8;
			break;
		case GET_TEMP:
			data.temp = mpu6050_read_byte(pmydev->pclt, TEMP_OUT_L);
			data.temp |= mpu6050_read_byte(pmydev->pclt, TEMP_OUT_H) << 8;
			break;
		default:
			return -EINVAL;
	}

	if(copy_to_user((void *)arg, &data, sizeof(data))) {
		return -EFAULT;
	}

	return sizeof(data);
}

void init_mpu6050(struct i2c_client *pclt)
{
	mpu6050_write_byte(pclt, PWR_MGMT_1, 0x00);
	mpu6050_write_byte(pclt, SMPLRT_DIV, 0x07);
	mpu6050_write_byte(pclt, CONFIG, 0x06);
	mpu6050_write_byte(pclt, GYRO_CONFIG, 0x18); // 不自检，2000deg/s
	mpu6050_write_byte(pclt, ACCEL_CONFIG, 0x0); // 不自检，2G，5Hz
}

struct file_operations myops = {
	.owner = THIS_MODULE,
	.open = mpu6050_open,
	.release = mpu6050_close,
	.unlocked_ioctl = mpu6050_ioctl,
};

static int mpu6050_probe(struct i2c_client *pclt, const struct i2c_device_id *pid)
{
	int ret = 0;
	dev_t devno = MKDEV(major, minor);

	/*申请设备号*/
	ret = register_chrdev_region(devno, mpu6050_num, "mpu6050");
	if (ret)	{
		ret = alloc_chrdev_region(&devno, minor, mpu6050_num, "mpu6050");
		if (ret) {
			printk("get devno failed\n");
			return -1;
		}
		major = MAJOR(devno);
	}

	pgmydev = (struct mpu6050_dev *)kmalloc(sizeof(struct mpu6050_dev), GFP_KERNEL);
	if (NULL == pgmydev) {
		unregister_chrdev_region(devno, mpu6050_num);
		printk("kmalloc failed\n");
		return -1;
	}
	memset(pgmydev, 0, sizeof(struct mpu6050_dev));

	pgmydev->pclt = pclt;

	/*给struct cdev对象指定操作函数集*/	
	cdev_init(&pgmydev->mydev,&myops);

	/*将struct cdev对象添加到内核对应的数据结构里*/
	pgmydev->mydev.owner = THIS_MODULE;
	cdev_add(&pgmydev->mydev, devno, mpu6050_num);

	init_mpu6050(pgmydev->pclt);

	pgmydev->pcls = class_create(THIS_MODULE, "mpu6050_class");
    if (IS_ERR(pgmydev->pcls)) {
        printk("class_create failed\n");
        cdev_del(&pgmydev->mydev);
	    unregister_chrdev_region(devno, mpu6050_num);
        return -1;
    }

	pgmydev->pdev = device_create(pgmydev->pcls, NULL, devno, NULL, "mpu6050");
    if (NULL == pgmydev->pdev) {
        printk("device_create failed\n");
        class_destroy(pgmydev->pcls);
        cdev_del(&pgmydev->mydev);
	    unregister_chrdev_region(devno, mpu6050_num);
        return -1;
    }

	return 0;
}

static int mpu6050_remove(struct i2c_client *pclt)
{
	dev_t devno = MKDEV(major, minor);

	device_destroy(pgmydev->pcls, devno);
    class_destroy(pgmydev->pcls);
	cdev_del(&pgmydev->mydev);
	unregister_chrdev_region(devno, mpu6050_num);

	kfree(pgmydev);
	pgmydev = NULL;

	return 0;
}

struct of_device_id mpu6050_dt[] = {
	{.compatible = "invensense,mpu6050"}, // 匹配依据
	{}
};

struct i2c_device_id mpu6050_ids[] = {
	{"mpu6050", 0},
	{}
};

struct i2c_driver mpu6050_driver = 
{
	.driver = {
		.name = "mpu6050",
		.owner = THIS_MODULE,
		.of_match_table = mpu6050_dt,
	},
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_ids,
};

module_i2c_driver(mpu6050_driver);

MODULE_LICENSE("GPL");
