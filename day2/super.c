// headers {{{1
#include <linux/slab.h>
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

	if (!options)
		return;

	while ((data = strsep(&options, ",")) != NULL) {
		if (!*data)
			continue;
		if ((value = strchr(data, '=')) != NULL)
			*value++ = '\0';

		if (strncmp(data, "rsize", 5) == 0) {
			if (value && *value) {
				size = simple_strtoul(value, &value, 0);
				if (size > 0)
					sfs_sb->rsize = size;
			}
		} else if (strncmp(data, "wsize", 5) == 0) {
			if (value && *value) {
				size = simple_strtoul(value, &value, 0);
				if (size > 0)
					sfs_sb->wsize = size;
			}
		} /* else unknown mount option */
	}
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

	/* Eventually replace iget with:
	inode = samplefs_get_inode(sb, S_IFDIR | 0755, 0); */

	inode = iget_locked(sb, SAMPLEFS_ROOT_I);

	if (!inode)
		return -ENOMEM;

	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		iput(inode);
		kfree(sfs_sb);
		return -ENOMEM;
	}

	// 文件系统私有数据

	sb->s_fs_info = kzalloc(sizeof(struct samplefs_sb_info), GFP_KERNEL);
	sfs_sb = SFS_SB(sb);
	if (!sfs_sb) {
		iput(inode);
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
