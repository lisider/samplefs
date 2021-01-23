#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *proc_fs_samplefs;
static struct proc_dir_entry *debug_data;

static int proc_show_msg(struct seq_file *file, void *v)
{
	int cnt = 0;
	/*函数seq_printf是最常用的输出函数，它用于把给定参数按照给定的格式输出到seq_file文件*/
	seq_printf(file, "Display Debugging Information\n");
	seq_printf(file, "-----------------------------\n");
	seq_printf(file, "Day: %d\n",3);

	return cnt;
}

static int proc_key_open(struct inode *inode, struct file *file)
{
	/*将proc_show_msg 和 seq_file文件关联起来*/
	single_open(file, proc_show_msg, NULL);
	return 0;
}

static struct file_operations samplefs_debug_fops = {
	.owner   = THIS_MODULE,
	.open     = proc_key_open,
	.read     = seq_read,     /*定义在linux/seq_file.c*/
	.release = single_release,  /*定义在linux/seq_file.c*/
};

void
samplefs_proc_init(void)
{
	proc_fs_samplefs = proc_mkdir("fs/samplefs", NULL);
	if (proc_fs_samplefs == NULL)
		return;

	debug_data = proc_create("debug_data", 0x0644, proc_fs_samplefs, &samplefs_debug_fops);
	if (debug_data == NULL)
		printk("proc_fs create file fail %s,%s,%d\n",__FILE__,__func__,__LINE__);
}

void
samplefs_proc_clean(void)
{
	if (proc_fs_samplefs == NULL)
		return;

	proc_remove(debug_data);
	proc_remove(proc_fs_samplefs);

}
#endif /* CONFIG_PROC_FS */
