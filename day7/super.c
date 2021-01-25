// headers {{{1
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/pagemap.h>
#include <linux/nls.h>
#include "samplefs.h"


// super_operations {{{1

static void
samplefs_put_super(struct super_block *sb)
{
	struct samplefs_sb_info *sfs_sb;
	//dump_stack();

	sfs_sb = SFS_SB(sb);
	if (sfs_sb == NULL) {
		/* Empty superblock info passed to unmount */
		return;
	}

	unload_nls(sfs_sb->local_nls);

	/* FS-FILLIN your fs specific umount logic here */

	kfree(sfs_sb);
	return;
}


struct super_operations samplefs_super_ops = {
	.statfs         = simple_statfs,
	.drop_inode     = generic_delete_inode, /* Not needed, is the default */
	.put_super      = samplefs_put_super,
};


// fill_super {{{1


static void
samplefs_parse_mount_options(char *options, struct samplefs_sb_info *sfs_sb)
{
	char *value;
	char *data;
	int size;

	if (!options) {
		printk(KERN_INFO "no options\n");
		return;
	}

	printk(KERN_INFO "samplefs: parsing mount options %s\n", options);

	while ((data = strsep(&options, ",")) != NULL) {
		if (!*data)
			continue;
		if ((value = strchr(data, '=')) != NULL)
			*value++ = '\0';

		if (strncmp(data, "rsize", 5) == 0) {
			if (value && *value) {
				size = simple_strtoul(value, &value, 0);
				if (size > 0) {
					sfs_sb->rsize = size;
					printk(KERN_INFO
						"samplefs: rsize %d\n", size);
				}

			}
		} else if (strncmp(data, "wsize", 5) == 0) {
			if (value && *value) {
				size = simple_strtoul(value, &value, 0);
				if (size > 0) {
					sfs_sb->wsize = size;
					printk(KERN_INFO
						"samplefs: wsize %d\n", size);
				}
			}
		} else if ((strncmp(data, "nocase", 6) == 0) ||
			   (strncmp(data, "ignorecase", 10)  == 0)) {
			sfs_sb->flags |= SFS_MNT_CASE;
			printk(KERN_INFO "samplefs: ignore case\n");

		} else {
			printk(KERN_WARNING "samplefs: bad mount option %s\n",
				data);
		}
	}
}

struct inode *samplefs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
        struct inode * inode = new_inode(sb);
	struct samplefs_sb_info * sfs_sb = SFS_SB(sb);

        if (inode) {
                inode->i_mode = mode;
                inode->i_uid = current_fsuid();
                inode->i_gid = current_fsgid();
                inode->i_blocks = 0;
                inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		printk(KERN_INFO "about to set inode ops\n");
		switch (mode & S_IFMT) {
		/* We are not ready to handle files yet */
		case S_IFREG:
			printk(KERN_INFO "file inode\n");
			inode->i_op = &sfs_file_inode_ops;
			break;
		case S_IFDIR:
			printk(KERN_INFO "directory inode sfs_sb: %p\n",sfs_sb);
			inode->i_op = &sfs_dir_inode_ops;
			inode->i_fop = &simple_dir_operations;

			/* link == 2 (for initial ".." and "." entries) */
			inc_nlink(inode);
			break;
		default:
			init_special_inode(inode, mode, dev);
			break;
		}
        }
        return inode;

}


static int samplefs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct samplefs_sb_info *sfs_sb;
	//dump_stack();

	sb->s_maxbytes = MAX_LFS_FILESIZE; /* NB: may be too large for mem */
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = SAMPLEFS_MAGIC;
	sb->s_op = &samplefs_super_ops;
	sb->s_time_gran = 1; /* 1 nanosecond time granularity */

	printk(KERN_INFO "samplefs: fill super\n");

	// 文件系统私有数据

#ifdef CONFIG_SAMPLEFS_DEBUG
	printk(KERN_INFO "samplefs: about to alloc s_fs_info\n");
#endif
	sb->s_fs_info = kzalloc(sizeof(struct samplefs_sb_info), GFP_KERNEL);

	sfs_sb = SFS_SB(sb);
	if (!sfs_sb) {
		printk("error aloc s_fs_info\n");
		return -ENOMEM;
	}



	inode = samplefs_get_inode(sb, S_IFDIR | 0755, 0);
	if (!inode) {
		kfree(sfs_sb);
		return -ENOMEM;
	}

	printk(KERN_INFO "samplefs: about to alloc root inode\n");

	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		iput(inode);
		kfree(sfs_sb);
		return -ENOMEM;
	}


	/* below not needed for many fs - but an example of per fs sb data */
	sfs_sb->local_nls = load_nls_default();

	samplefs_parse_mount_options(data, sfs_sb);

	/* FS-FILLIN your filesystem specific mount logic/checks here */

	return 0;
}

// mount {{{1

// mount
// struct dentry *(*mount) (struct file_system_type *, int, const char *, void *);


struct dentry *samplefs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, samplefs_fill_super);
}
