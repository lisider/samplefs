// headers {{{1
#include <linux/module.h>
#include <linux/fs.h>
#include "samplefs.h"


static struct file_system_type samplefs_fs_type = {
	.owner = THIS_MODULE,
	.name = "samplefs",
	.mount = samplefs_mount,
	.kill_sb = kill_anon_super,
};

// module {{{1

static int __init init_samplefs_fs(void)
{
    return register_filesystem(&samplefs_fs_type);
}

static void __exit exit_samplefs_fs(void)
{
	unregister_filesystem(&samplefs_fs_type);
}

module_init(init_samplefs_fs)
module_exit(exit_samplefs_fs)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SUWS");
MODULE_DESCRIPTION("SAMPLEFS");


// file_system_type {{{1
	//struct file_system_type {
	//	const char *name;
	//	int fs_flags; 						// 文件系统类型标志的bitmap
	//#define FS_REQUIRES_DEV		1
	//#define FS_BINARY_MOUNTDATA	2
	//#define FS_HAS_SUBTYPE		4
	//#define FS_USERNS_MOUNT		8	/* Can be mounted by userns root */
	//#define FS_USERNS_DEV_MOUNT	16 /* A userns mount does not imply MNT_NODEV */
	//#define FS_RENAME_DOES_D_MOVE	32768	/* FS will handle d_move() during rename() internally. */
	//	struct dentry *(*mount) (struct file_system_type *, int,
	//		       const char *, void *);
	//	void (*kill_sb) (struct super_block *);
	//	struct module *owner;
	//	struct file_system_type * next; 	// 形成文件系统类型链表
	//	struct hlist_head fs_supers; 		//同一种文件类型的超级块形成一个链表，fs_supers是这个链表的头
	//
	//	struct lock_class_key s_lock_key;
	//	struct lock_class_key s_umount_key;
	//	struct lock_class_key s_vfs_rename_key;
	//	struct lock_class_key s_writers_key[SB_FREEZE_LEVELS];
	//
	//	struct lock_class_key i_lock_key;
	//	struct lock_class_key i_mutex_key;
	//	struct lock_class_key i_mutex_dir_key;
	//}
	//

	// 不同类型的文件系统通过next字段形成一个链表
	// 同一种文件系统类型的超级块通过s_instances字段连接在一起，并挂入fs_supers链表中。
	// 所有的vfsmount通过mnt_list字段形成一个链表
