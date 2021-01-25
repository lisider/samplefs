How to build a Linux Filesystem in 21 days

- doc

```c
http://svn.samba.org/samba/ftp/cifs-cvs/ols2006-fs-tutorial-smf.odp
```
- code
```c
https://github.com/kvaneesh/samplefs
https://github.com/Andiry/Sample-FS
http://svn.samba.org/samba/ftp/cifs-cvs/samplefs.tar.gz
```
- Day status
```c
day1
	可插入内核
day2
	samplefs 创建 super_block
day3
	samplefs 加入 proc 支持,用作调试
day4
	samplefs 创建 inode
	新增支持挂载
day5
	samplefs 创建 dir  文件 的 inode_operations , 并初始化成员 lookup 和 创建 dentry_operations
	新增无
day6 	------ 从此开始支持文件操作
	samplefs 创建 dir  文件 的 inode_operations , 并初始化成员 create unlink mkdir rmdir mknod rename
	新增 创建文件 创建目录 mknod
day7
	samplefs 创建 file 文件 的 inode_operations , 并初始化成员 getattr
day8
	无
day9
	samplefs 创建 file 文件 的 file_operations  , 并初始化成员 read_iter write_iter mmap fsync llseek
	新增 普通文件 的读写
dayA
	无
dayB
	samplefs 创建 link 文件 的 inode_operations , 并初始化成员 link symlink // (该成员在 dir  文件 的 inode_operations)
	新增 对普通文件 创建链接

```

