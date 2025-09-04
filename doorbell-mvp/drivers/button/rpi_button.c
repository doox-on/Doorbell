#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h> 
#include <linux/jiffies.h>
#include <linux/poll.h>
#include <linux/wait.h>


static unsigned long last_jf;  
static unsigned int debounce_ms = 100;
// static char buf[64];

unsigned int irq_number;
static int irq_ready=0;
static wait_queue_head_t waitqueue;
#define IO_BUTTON 20

static irqreturn_t gpio_irq_handler(int irq, void *dev_id) {
    unsigned long now = jiffies;

    if (time_before(now, last_jf + msecs_to_jiffies(debounce_ms))){
        pr_notice("--Debounced--\n");
        return IRQ_HANDLED;
    }

    last_jf = now;
    
    pr_notice("Interruped is triggered!\n");
    irq_ready = 1;
    wake_up_interruptible(&waitqueue);
    return IRQ_HANDLED;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *ppos)
{
    char c = '1';

    wait_event_interruptible(waitqueue, irq_ready);

    irq_ready = 0;

    if (copy_to_user(buf, &c, 1))
        return -EFAULT;

    return 1;
}


static __poll_t my_poll(struct file *file, poll_table *wait) {

    __poll_t mask = 0;
    poll_wait(file,&waitqueue,wait);
    if(irq_ready == 1) {
        mask |= POLLIN | POLLRDNORM;
    }
    return mask;

}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    // .write = my_write,
    .poll = my_poll
};

static struct miscdevice my_device = {
    .name = "doorBell",
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &fops
};

// static struct file_operations fops ={
//     .read = my_read
// };

int __init my_init(void) {

    int ret;
    //cdev registration
    misc_register(&my_device);
    // major = register_chrdev(0,"hello_cdev", &fops);
    printk("hello - Hello, Kernel! \n");

    //GPIO
    gpio_request(IO_BUTTON, "rpi-gpio-17");
    gpio_direction_input(IO_BUTTON);
    irq_number = gpio_to_irq(IO_BUTTON);
    ret = request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL);
    if(ret) {
        pr_err("request_irq failed: %d (irq=%u)\n", ret, irq_number);
    }
    printk("IRQ DONE!\n");

    //waitQueue
    init_waitqueue_head(&waitqueue);

   
    printk("GPIO %d is mapped to IRQ #%d!\n",IO_BUTTON,irq_number);

    return 0;
}


void __exit my_exit (void) {
    printk("hello - Goodbye, Kernel!\n");
    misc_deregister(&my_device);
    free_irq(irq_number,NULL);
    gpio_free(IO_BUTTON);
}

module_init(my_init);
module_exit(my_exit);


MODULE_AUTHOR("Jungwoo Moon");
MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION("Doorbell Driver");
