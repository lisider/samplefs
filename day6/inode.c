#include <linux/nls.h>
#include <linux/proc_fs.h>
#include "samplefs.h"

static int sfs_ci_hash(const struct dentry *dentry, struct qstr *q)
{
        struct nls_table *codepage = SFS_SB(dentry->d_inode->i_sb)->local_nls;
        unsigned long hash;
        int i;

        hash = init_name_hash();
        for (i = 0; i < q->len; i++)
                hash = partial_name_hash(nls_tolower(codepage, q->name[i]),
                                         hash);
        q->hash = end_name_hash(hash);

        return 0;
}

#if 0
static int sfs_ci_compare(struct dentry *dentry, struct qstr *a,
                           struct qstr *b)
{
        struct nls_table *codepage = SFS_SB(dentry->d_inode->i_sb)->local_nls;

        if ((a->len == b->len) &&
            (nls_strnicmp(codepage, a->name, b->name, a->len) == 0)) {
                /*
                 * To preserve case, don't let an existing negative dentry's
                 * case take precedence.  If a is not a negative dentry, this
                 * should have no side effects
                 */
                memcpy((unsigned char *)a->name, b->name, a->len);
                return 0;
        }
        return 1;
}
#endif
/* No sense hanging on to negative dentries as they are only
in memory - we are not saving anything as we would for network
or disk filesystem */

static int sfs_delete_dentry(const struct dentry *dentry)
{
        return 1;
}

struct dentry_operations sfs_dentry_ops = {
	.d_delete = sfs_delete_dentry,
};

static struct dentry_operations sfs_ci_dentry_ops = {
/*	.d_revalidate = xxxd_revalidate, Not needed for this type of fs */
	.d_hash = sfs_ci_hash,
//	.d_compare = sfs_ci_compare,
	.d_delete = sfs_delete_dentry,
};

/*
 * Lookup the data, if the dentry didn't already exist, it must be
 * negative.  Set d_op to delete negative dentries to save memory
 * (and since it does not help performance for in memory filesystem).
 */
static struct dentry *sfs_lookup(struct inode *dir, struct dentry *dentry, unsigned int nd)
{
	struct samplefs_sb_info * sfs_sb = SFS_SB(dir->i_sb);
	if (dentry->d_name.len > NAME_MAX)
		return ERR_PTR(-ENAMETOOLONG);
	if (sfs_sb->flags & SFS_MNT_CASE)
		dentry->d_op = &sfs_ci_dentry_ops;
	else
		dentry->d_op = &sfs_dentry_ops;

	d_add(dentry, NULL);
	return NULL;
}

static int
sfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode * inode = samplefs_get_inode(dir->i_sb, mode, dev);
	int error = -ENOSPC;

	printk(KERN_INFO "samplefs: mknod\n");
	if (inode) {
		if (dir->i_mode & S_ISGID) {
			inode->i_gid = dir->i_gid;
			if (S_ISDIR(mode))
				inode->i_mode |= S_ISGID;
		}
		d_instantiate(dentry, inode);
		dget(dentry);   /* Extra count - pin the dentry in core */
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;

		/* real filesystems would normally use i_size_write function */
		dir->i_size += 0x20;  /* bogus small size for each dir entry */
	}
	return error;
}


static int sfs_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
	int retval = 0;

	retval = sfs_mknod(dir, dentry, mode | S_IFDIR, 0);

	/* link count is two for dir, for dot and dot dot */
	if (!retval)
		inc_nlink(dir);
	return retval;
}

static int sfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
			bool nd)
{
	return sfs_mknod(dir, dentry, mode | S_IFREG, 0);
}


struct inode_operations sfs_dir_inode_ops = {
	.create         = sfs_create,
	.lookup         = sfs_lookup,
	.unlink         = simple_unlink,
	.mkdir          = sfs_mkdir,
	.rmdir          = simple_rmdir,
	.mknod          = sfs_mknod,
	.rename         = simple_rename,
};
