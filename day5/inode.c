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
	if (dentry->d_name.len > NAME_MAX)
		return ERR_PTR(-ENAMETOOLONG);
	dentry->d_op = &sfs_ci_dentry_ops;
	d_add(dentry, NULL);
	return NULL;
}


struct inode_operations sfs_ci_inode_ops = {
	.lookup		= sfs_lookup,
};
