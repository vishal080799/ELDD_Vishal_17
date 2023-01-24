#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 
#include <linux/uaccess.h>
#include<linux/err.h>             
#include<linux/semaphore.h>
#include<linux/kthread.h>
#include<linux/wait.h>


#define MEM_SIZE 50

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishal");

static int wait_function(void *unused);
static struct task_struct *wait_thread;


uint32_t count_read=0;
dev_t dev;
static struct cdev char_driver_cdev;
struct semaphore my_sema;
wait_queue_head_t wait_queue_wq;
int wait_queue_flag=0;
char k_buff[MEM_SIZE];


static int file_open(struct inode *inode, struct file *file);
static int file_release(struct inode *inode, struct file *file);
static ssize_t  file_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  file_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .open           = file_open,
        .release        = file_release,
        .read           = file_read,
        .write          = file_write,
};


static int wait_function(void *unused)
{
while(1){
    pr_info("Waiting for Event\n");
    wait_event_interruptible(wait_queue_wq,wait_queue_flag !=0);
    if(wait_queue_flag ==2){
        pr_info("Event come from EXIT...\n");
        return 0;

    }
    if(wait_queue_flag ==3){
         pr_info("Event come from Read...%d\n",++count_read);
         wait_queue_flag=0;
        
    }
}
    wait_queue_flag=0;
    
}


static int file_open(struct inode *inode, struct file *file)
{
    printk("Open function called......\n");
    return 0;

}

static int file_release(struct inode *inode, struct file *file)
{
     printk("Release function called......\n");
    return 0;

}

static ssize_t  file_read(struct file *filp, char __user *buf, size_t len,loff_t * off)
{
    printk("Read function called......\n");
    ssize_t retval;
    if(copy_to_user(buf,k_buff,MEM_SIZE))
    {
        pr_err("Data Read : Err!\n");
    }
    else{
        printk("Data Read : Done\n");
        wait_queue_flag=3;
        wake_up_interruptible(&wait_queue_wq);
        up(&my_sema);
        return MEM_SIZE;
    } 

   
    retval=MEM_SIZE;

    return retval;

}

static ssize_t  file_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
    printk("Write function called......\n");
    ssize_t retval;
    down(&my_sema);
    if(copy_from_user(k_buff,buf,len))
    {
          pr_err("Data write : Err!\n");
    }
    else{
        printk("Data write : Done\n");
        return len;
    }

    retval=len;
    return retval;

}

static int my_init(void)
{
    printk(KERN_ALERT"Hello From init function....\n");
    
    if((alloc_chrdev_region(&dev, 0, 2, "char_driver_Dev"))<0)
    {
        printk("Cannot allocate major number\n");
        return -1;
    }
    printk("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    cdev_init(&char_driver_cdev,&fops);

        if((cdev_add(&char_driver_cdev,dev,2)) < 0){
        printk("Cannot add the device to the system\n");
        unregister_chrdev_region(dev,2);
        return -1;   
        }

    sema_init(&my_sema,1);
    init_waitqueue_head(&wait_queue_wq);
    
    wait_thread = kthread_create(wait_function,NULL,"Wait_thread");
        if(wait_thread)
        {
            pr_info("Thread Created succesfully\n");
            wake_up_process(wait_thread);
        }
        else
            pr_info("Thread creation failed\n");
    printk("Charecter file added succesfully\n.........Device Driver inserted successfully\n");
   

       return 0;
 
}

static void my_exit(void)
{   
    wait_queue_flag=2;
    wake_up_interruptible(&wait_queue_wq);
    cdev_del(&char_driver_cdev);
    unregister_chrdev_region(dev,2);
    printk("Device Drivers removed successfully\n");


}

module_init(my_init);
module_exit(my_exit);
/*
we should do mknod -m 777 /dev/read_device c Maj Min 
    */
