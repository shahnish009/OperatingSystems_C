#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/rtc.h>

MODULE_LICENSE("GPL");

static struct miscdevice linepipe;

static int N;						//Number of lines in pipe

static struct semaphore full;                           //semaphore for pipe
static struct semaphore empty;

static struct semaphore read_mutex;                  //semaphore for read/write
static struct semaphore write_mutex;


static int char_count = 100;				//length of each string

static int read_index = 0, write_index = 0;

static int empty_lines;					//number of empty lines in pipe

module_param(N, int, 0000);				//number of lines from command line

char ** pipe;

							//fops functions declaration
static int my_open(struct inode*, struct file*);
static ssize_t my_read(struct file*, char*, size_t, loff_t*);
static ssize_t my_write(struct file*, const char*, size_t, loff_t*);
static int my_release(struct inode*, struct file*);

static struct file_operations linepipe_fops =
{							//fops struct
	.open = &my_open,
	.read = &my_read,
	.write = &my_write,
	.release = &my_release
};

int init_module()
{							//module initialization

	linepipe.name = "linepipe";
	linepipe.minor = MISC_DYNAMIC_MINOR;
	linepipe.fops = &linepipe_fops;
	int reg;					//registering device
	if((reg = misc_register(&linepipe)) < 0)
	{
		printk(KERN_ERR "linepipe did not register successfully\n");
		return reg;
	}
	printk(KERN_INFO "linepipe egistered!\n");
	printk(KERN_INFO "number of lines in pipe (N): %d\n", N);

	int mem = 0;					//allocating memory to pipe
	pipe = (char**)kmalloc(N*sizeof(char*), GFP_KERNEL);
	while(mem < N)
	{
		pipe[mem] = (char*)kmalloc((char_count+1)*sizeof(char), GFP_KERNEL);
		pipe[char_count] = '\0';
		++mem;
	}

							//initializing semaphore and mutex
	sema_init(&full, 0);
	sema_init(&empty, N);
	sema_init(&read_mutex, 1);
	sema_init(&write_mutex, 1);

	empty_lines = N;
	return 0;
}

static int my_open(struct inode* _inode, struct file* _file)
{							//linepipe open
	printk(KERN_INFO "linepipe opened\n");
	return 0;
}

static ssize_t my_read(struct file* _file, char* user_buffer, size_t len, loff_t* offset)
{
	int index = 0;					//reading from linepipe

	//acquiring lock on read mutex and performing down operation and up operation on reading so as to satisfy locking adequately
	down_interruptible(&read_mutex);
	down_interruptible(&full);
	read_index %= N;
	while(index < len)
	{
		if(empty_lines >= N)
		{
			break;
		}
		copy_to_user(&user_buffer[index], &pipe[read_index][index], 1);
		index++;
	}
	++read_index;
	++empty_lines;
	up(&empty);
	up(&read_mutex);
	return index;
}

static ssize_t my_write(struct file* _file, const char* user_buffer, size_t len, loff_t* offset)
{							//writing to linepipe
	int index = 0;

	//acquiring lock on write mutex and performing down operation and up operation on reading so as to satisfy locking adequately
	down_interruptible(&write_mutex);
	down_interruptible(&empty);
	write_index %= N;
	while(index < len)
	{
		if(empty_lines <= 0)
		{
			break;
		}
		copy_from_user(&pipe[write_index][index], &user_buffer[index], 1);
		index++;
	}
	++write_index;
	--empty_lines;
	up(&full);
	up(&write_mutex);
	return index;
}

static int my_release(struct inode* _inode, struct file* _file)
{							//closing linepipe
	printk(KERN_INFO "linepipe closed\n");
	return 0;
}

void cleanup_module()					//free memory and deregister linepipe
{
	/*freeing memory*/
	int i = 0;
	while (i < N)
	{
		kfree(pipe[i]);
		i++;
	}
	misc_deregister(&linepipe);
	printk(KERN_INFO "linepipe unregistered!\n");
}
