#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
 

#define TIMEOUT 1000    

static struct timer_list my_timer;
static unsigned int count = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;

static int __init my_driver_init(void);
static void __exit my_driver_exit(void);


static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t * off);



static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_read,
        .write          = my_write,
        .open           = my_open,
        .release        = my_release,
};
 
void timer_callback(struct timer_list * data)
{
 
    pr_info("Timer Callback function Called [%d]\n",count++);
   
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}


static int my_open(struct inode *inode, struct file *file)
{
    pr_info("Device File Opened...!!!\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("Device File Closed...!!!\n");
    return 0;
}

static ssize_t my_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
    pr_info("Read Function\n");
    return 0;
}



static ssize_t my_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
    pr_info("Write function\n");
    return len;
}


static int __init my_driver_init(void)
{
  
    if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) <0){
            pr_err("Cannot allocate major number\n");
            return -1;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
   
    cdev_init(&my_cdev,&fops);
 
  
    if((cdev_add(&my_cdev,dev,1)) < 0){
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }
 
  
    if((dev_class = class_create(THIS_MODULE,"my_class")) == NULL){
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
 

    if((device_create(dev_class,NULL,dev,NULL,"my_device")) == NULL){
        pr_err("Cannot create the Device 1\n");
        goto r_device;
    }
 
    timer_setup(&my_timer, timer_callback, 0);      
 
  
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMEOUT));
 
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;
r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    return -1;
}


static void __exit my_driver_exit(void)
{
  
    del_timer(&my_timer);
    pr_info("timer deleted\n");
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(my_driver_init);
module_exit(my_driver_exit);
 
MODULE_LICENSE("GPL");