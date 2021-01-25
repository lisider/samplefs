#include <linux/module.h>
#include <linux/fs.h>

long test_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("enter %s\n", __func__);
	switch (cmd) {
	case FS_IOC_GETFLAGS:
		printk("ioctl: FS_IOC_GETFLAGS %d\n", *(int *)arg);
		break;
	default:
		break;
	}
	return 0;
}

struct address_space_operations sfs_aops = {
	.readpage       = simple_readpage,
	.write_begin	= simple_write_begin,
	.write_end      = simple_write_end,
};

struct file_operations sfs_file_operations = {
	//.open 			= generic_file_open,
	//.read           = do_sync_read,
	//.read           = new_sync_read,
	.read_iter      = generic_file_read_iter,
	//.aio_read	= generic_file_aio_read,
	//.write          = do_sync_write,
	//.write          = new_sync_write,
	.write_iter     = generic_file_write_iter,
	//.aio_write	= generic_file_aio_write,
	.mmap           = generic_file_mmap,
	.fsync          = noop_fsync,
	.llseek         = generic_file_llseek,
	.unlocked_ioctl = test_ioctl,
};
