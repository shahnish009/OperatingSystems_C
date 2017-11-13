#include <linux/init.h>								//used for __init __exit
#include <linux/module.h>         						//for loading module in kernel
#include <linux/device.h>         						//to support Device Driver
#include <linux/kernel.h>         						//basics of kernel
#include <linux/fs.h>             						//support linux file system
#include <asm/uaccess.h>          						//for copy_to_user function
#include <linux/sched.h>	 						//for getting process information

#define  DEVICE_NAME "process_list"
#define  CLASS_NAME  "process_class"

MODULE_LICENSE("GPL");
static int major_no;
static char information[10000] = {0};
static char state[150] = {0};
static short information_size;
static struct class * processlistClass  = NULL;
static struct device * processlistDevice = NULL;

static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);

static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.release = dev_release,
};

static struct task_struct *task;						//for using in for_each_process and getting the information of processes

static int __init processlist_init(void)
{
	printk(KERN_INFO "process_list module initializing\n");

	major_no = register_chrdev(0, DEVICE_NAME, &fops);			//assigning major number automatically
	if (major_no < 0)
	{
		printk(KERN_ALERT "process_list major number not registered\n");
		return major_no;
	}

	printk(KERN_INFO "process_list module registered correctly with major number %d\n", major_no);

	processlistClass = class_create(THIS_MODULE, CLASS_NAME);		//registering device class
	if (IS_ERR(processlistClass))						//if error in registering device class, unregister it
	{
		unregister_chrdev(major_no, DEVICE_NAME);
		printk(KERN_ALERT "process_list device class registration failed\n");
		return PTR_ERR(processlistClass);
	}
	printk(KERN_INFO "process_list device class registered correctly\n");

	processlistDevice = device_create(processlistClass, NULL, MKDEV(major_no, 0), NULL, DEVICE_NAME);	//registering device driver
	if (IS_ERR(processlistDevice))						//if error in registering device driver, unregister and destroy  device and class
	{
		class_destroy(processlistClass);
		unregister_chrdev(major_no, DEVICE_NAME);
		printk(KERN_ALERT "device creation failed\n");
		return PTR_ERR(processlistDevice);
	}
	printk(KERN_INFO "process_list device class created correctly\n");
	return 0;
}

static void __exit processlist_exit(void)
{
	device_destroy(processlistClass, MKDEV(major_no, 0));     		//remove device
	class_unregister(processlistClass);                          		//unregister device class
	class_destroy(processlistClass);                             		//remove device class
	unregister_chrdev(major_no, DEVICE_NAME);	             		//unregister major number
	printk(KERN_INFO "process_list module exited\n");
}

static int dev_open(struct inode *inodep, struct file *filep)			//for opening module
{
	printk(KERN_INFO "process_list device opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)		//for reading
{
	int error_count = 0;

	for_each_process(task)							//gets us the information for each process
	{
		if (task->state == 0)
		{
			sprintf(state, "TASK_RUNNING");
		}
                else if (task->state == 1)
                {
                        sprintf(state, "TASK_INTERRUPTIBLE");
                }
                else if (task->state == 2)
                {
                        sprintf(state, "TASK_UNINTERRUPTIBLE");
                }
                else if (task->state == 4)
                {
                        sprintf(state, "__TASK_STOPPED");
                }
                else if (task->state == 8)
                {
                        sprintf(state, "__TASK_TRACED");
                }
                else if (task->state == 16)
                {
                        sprintf(state, "EXIT_DEAD");
                }
                else if (task->state == 32)
                {
                        sprintf(state, "EXIT_ZOMBIE");
                }
                else if (task->state == (32 | 16))
                {
                        sprintf(state, "EXIT_ZOMBIE, EXIT_DEAD");
                }
                else if (task->state == 64)
                {
                        sprintf(state, "TASK_DEAD");
                }
                else if (task->state == 128)
                {
                        sprintf(state, "TASK_WAKEHILL");
                }
                else if (task->state == 256)
                {
                        sprintf(state, "TASK_WAKING");
                }
                else if (task->state == 512)
                {
                        sprintf(state, "TASK_PARKED");
                }
                else if (task->state == 1024)
                {
                        sprintf(state, "TASK_NOLOAD");
                }
                else if (task->state == 2048)
                {
                        sprintf(state, "TASK_NEW");
                }
                else if (task->state == 4096)
                {
                        sprintf(state, "TASK_STATE_MAX");
                }
                else if (task->state == (128 | 2))
                {
                        sprintf(state, "TASK_WAKEHILL, TASK_UNINTERRUPTIBLE");
                }
                else if (task->state == (128 | 4))
                {
                        sprintf(state, "TASK_WAKEHILL, __TASK_STOPPED");
                }
                else if (task->state == (128 | 8))
                {
                        sprintf(state, "TASK_WAKEHILL, __TASK_TRACED");
                }
                else if (task->state == (2 | 1024))
                {
                        sprintf(state, "TASK_UNINTERRUPTIBLE, TASK_NOLOAD");
                }
                else if (task->state == (1 | 2))
                {
                        sprintf(state, "TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE");
                }
                else if (task->state == ((1 | 2) | 4 | 8))
                {
                        sprintf(state, "TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE, __TASK_STOPPED, __TASK_TRACED");
                }
                else if (task->state == (0 | 1 | 2 | 4 | 8 | 32 | 16))
                {
                        sprintf(state, "TASK_RUNNING, TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE, __TASK_STOPPED, __TASK_TRACED, EXIT_ZOMBIE, EXIT_DEAD");
                }

		sprintf(information + strlen(information), "\t\tPID=%d PPID=%d CPU=%d STATE=%s\n", task->pid, task->real_parent->pid, task_cpu(task), state);
		information_size = strlen(information);
	}
		error_count = copy_to_user(buffer, information, information_size);

		if (error_count == 0)
		{
			printk(KERN_INFO "process_list sending to user successful\n");
			return (information_size = 0);
		}
		else
		{
			printk(KERN_INFO "process_list sending to user failed\n");
			return -EFAULT;
		}
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "process_list device closed\n");
	return 0;
}
module_init(processlist_init);
module_exit(processlist_exit);
