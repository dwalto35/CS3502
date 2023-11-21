#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAX_LEN 4096
static struct proc_dir_entry *proc_entry;
static char *proc_buffer;

ssize_t read_proc(struct file *f, char *user_buf, size_t count, loff_t *off)
{
    // Output the content of proc_buffer to the user's buffer
    ssize_t len = strlen(proc_buffer);
    if (*off >= len) {
        // Reached the end of the file
        return 0;
    }

    if (count > len - *off) {
        count = len - *off;
    }

    if (copy_to_user(user_buf, proc_buffer + *off, count) != 0) {
        return -EFAULT;
    }

    *off += count;

    printk(KERN_INFO "procfs_read: read %lu bytes\n", count);
    return count;
}

ssize_t write_proc(struct file *f, const char *user_buf, size_t count, loff_t *off)
{
    // Copy the written data from user space and save it in proc_buffer
    if (*off != 0 || count > MAX_LEN) {
        // Writing beyond the end of the buffer or exceeding MAX_LEN
        return -EFAULT;
    }

    if (copy_from_user(proc_buffer, user_buf, count) != 0) {
        return -EFAULT;
    }

    proc_buffer[count] = '\0'; // Null-terminate the string

    printk(KERN_INFO "procfs_write: write %lu bytes\n", count);
    return count;
}

struct file_operations proc_fops = {
    .read = read_proc,
    .write = write_proc,
};

int init_module(void)
{
    int ret = 0;

    // Allocate memory for the proc buffer
    proc_buffer = (char *)vmalloc(MAX_LEN);

    if (!proc_buffer) {
        ret = -ENOMEM;
        goto fail;
    }

    // Create the entry in /proc
    proc_entry = proc_create("myproc", 0666, NULL, &proc_fops);

    if (!proc_entry) {
        ret = -ENOMEM;
        goto fail;
    }

    printk(KERN_INFO "test_proc created.\n");

    return ret;

fail:
    if (proc_buffer) {
        vfree(proc_buffer);
    }
    return ret;
}

void cleanup_module(void)
{
    // Remove the proc entry and free the proc_buffer space
    remove_proc_entry("myproc", NULL);
    vfree(proc_buffer);

    printk(KERN_INFO "test_proc deleted.\n");
}
