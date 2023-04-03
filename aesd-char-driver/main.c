/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include "linux/slab.h"
#include <linux/mutex.h>
#include "linux/string.h"
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Nihal T"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    struct aesd_dev* device;

    PDEBUG("open");
    /**
     * TODO: handle open
     */

    // Getting the pointer of the sturcure that containes the cdev
    device = container_of(inode->i_cdev, struct aesd_dev, cdev);

    // Storing the device pointer in the private data for other modules to use it
    filp->private_data = device;

    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    struct aesd_dev *device = filp->private_data;
    struct aesd_buffer_entry *temp_kernel_buff_ptr;
    size_t offset_byte_loc;
    int temp_count = 0;

    PDEBUG("Reads %zu bytes with offset %lld", count, *f_pos);
    /**
     * TODO: handle read
     */

    // Obtaining the lock 
    mutex_lock(&aesd_device.lock);

    // Getting the buffer and the position of the offset
    temp_kernel_buff_ptr = aesd_circular_buffer_find_entry_offset_for_fpos(&device->aesd_circular_buffer,
                                                                           *f_pos, 
                                                                           &offset_byte_loc);
    if(temp_kernel_buff_ptr == NULL)
    {
        *f_pos = 0;
        goto out;
    }

    // 
    if ((temp_kernel_buff_ptr->size - offset_byte_loc) < count) 
    {
        *f_pos += (temp_kernel_buff_ptr->size - offset_byte_loc);
        temp_count = temp_kernel_buff_ptr->size - offset_byte_loc;
    } 
    else 
    {
        *f_pos += count;
        temp_count = count;
    }

    if (copy_to_user(buf, temp_kernel_buff_ptr->buffptr+offset_byte_loc, temp_count)) 
    {
        retval = -EFAULT;
        goto out;
    }

    retval = temp_count;

    // Releasing the lock 
    out: mutex_unlock(&aesd_device.lock);

    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = count;
    char* temp_kernel_buff_ptr = NULL;
    bool newline_recieved = false;
    int status;
    int i;
    char* ret_buff_ptr = NULL;
    struct aesd_buffer_entry aesd_buffer_write_entry;
    char* temp = NULL;

    
    // Obtaining the device pointer ans storing in a local variable for our use
    struct aesd_dev* device = filp->private_data;
    /**
     * TODO: handle write
     */

    // Debug prints
    PDEBUG("Writing %zu bytes with offset %lld", count, *f_pos);
    
    // Obtaining the lock
    mutex_lock(&aesd_device.lock);

    // Mallocing user space buffer into a temp kernel space buffer
    temp_kernel_buff_ptr = (char*)kmalloc(count, GFP_KERNEL);
    if(temp_kernel_buff_ptr == NULL)
    {
        PDEBUG("Error: kmalloc %zu bytes", count);
        retval = -ENOMEM;
        goto unlock;
    }

    // Copying user space buffer into a temp kernel space buffer
    status = copy_from_user(temp_kernel_buff_ptr, buf, count);
    if(status != 0)
    {
        PDEBUG("Error: copy_from_user");
        retval = -ENOMEM;
        goto free;
    }

    // Checking if '\n' is recieved
    for (i=0; i< count; i++)
    {
        if(*(temp_kernel_buff_ptr+i) == '\n')
        {
            newline_recieved = true;
            break;
        }
    }
    if(!newline_recieved)
    {
        i--;
    }

    // If this is a new packet being recieved
    if(device->size_global_buff_ptr == 0)
    {
        device->size_global_buff_ptr = i+1;
        device->global_buff_ptr = (char*)kmalloc(i+1, GFP_KERNEL);
        if(device->global_buff_ptr == NULL)
        {
            device->size_global_buff_ptr = 0;
            PDEBUG("Error: kmalloc %u bytes", i+1);
            goto free;
        }
        memcpy(device->global_buff_ptr, temp_kernel_buff_ptr, device->size_global_buff_ptr);
    }
    // If this is a packet being continued after the previous
    else
    {
        temp = (char*)krealloc(device->global_buff_ptr, device->size_global_buff_ptr + (i+1), GFP_KERNEL);
        if(temp == NULL)
        {
            PDEBUG("Error: krealloc %u bytes", i+1);
            retval = -ENOMEM;
            goto free;
        }
        device->global_buff_ptr = temp;
        memcpy((device->global_buff_ptr + device->size_global_buff_ptr), temp_kernel_buff_ptr, i+1);
        device->size_global_buff_ptr += i+1;
    }
    
    
    if(newline_recieved)
    {
        aesd_buffer_write_entry.buffptr = device->global_buff_ptr;
        aesd_buffer_write_entry.size = device->size_global_buff_ptr;

        ret_buff_ptr = aesd_circular_buffer_add_entry(&device->aesd_circular_buffer, &aesd_buffer_write_entry);
    
        // If the buffer is full then return the replaced buffer pointer
        if (ret_buff_ptr != NULL)
            kfree(ret_buff_ptr);
        
        device->size_global_buff_ptr = 0;
    }


    // If error then the code will jump to this location else it will normally execute this line
    free: kfree(temp_kernel_buff_ptr);

    // Releasing the lock
    unlock: mutex_unlock(&aesd_device.lock);

    return retval;
}

struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int status;
    int devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    status = cdev_add (&dev->cdev, devno, 1);
    if (status) 
    {
        printk(KERN_ERR "Error: adding aesd cdev. Error code: %d", status);
    }
    return status;
}


int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device, 0, sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */

    // Initializing the mutex lock before using in cdev
    mutex_init(&aesd_device.lock);
    printk(" init hey hello\n");
    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    uint8_t index;
    struct aesd_buffer_entry *entry;

    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */

    // Freeing any buffer on the circular buffer
    AESD_CIRCULAR_BUFFER_FOREACH(entry, &aesd_device.aesd_circular_buffer, index) 
    {
      kfree(entry->buffptr);
    }

    // Deleting the mutex lock
    mutex_destroy(&aesd_device.lock);

    unregister_chrdev_region(devno, 1);
}

// ToDo A9:
// - fized_size_llseek uses size of the file and so the CB will have to caclulate the size of the CB
// - Option 2 is implement own llseek functionbut use the fized_size_llseek
//      > the read and write function should update the f_pos value that is passed as pointer for the kernel call for lseek to work
// - IOCTL based on the .h file 
// - The user will fill the structure and you will have to seek to that command and the offset within the command
// - Use fill no to convert the file stream into a fd (of int type)
// - Will have to use copy from user to copy this to the kernel space and then work on it
// - Call the aesd_adjust_file_offset() from the ioctl switch case
//      > check for valid write_cmd and write_cmd_offset
//      > calculate the start offset to write_cmd
//      > add write_cmd_offset
//      > save as flip->f_pos



module_init(aesd_init_module);
module_exit(aesd_cleanup_module);