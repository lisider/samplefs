#ifndef __SAMPLEFS_INTERNAL_H
#define __SAMPLEFS_INTERNAL_H


#define SAMPLEFS_ROOT_I 2

/* samplefs mount flags */
#define SFS_MNT_CASE 1


/* helpful if this is different than other fs */
#define SAMPLEFS_MAGIC     0x73616d70 /* "SAMP" */




/* This is an example of filesystem specific mount data that a file system might
   want to store.  FS per-superblock data varies widely and some fs do not
   require any information beyond the generic info which is already in
   struct super_block */
struct samplefs_sb_info {
	unsigned int rsize;
	unsigned int wsize;
	int flags;
	struct nls_table *local_nls;
};

static inline struct samplefs_sb_info *
SFS_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}




extern struct dentry *samplefs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data);

extern struct inode_operations sfs_ci_inode_ops;
#endif
