/*
Filesystem Lab disigned and implemented by Liang Junkai,RUC
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fuse.h>
#include <errno.h>
#include "disk.h"

#define DIRMODE S_IFDIR|0755
#define REGMODE S_IFREG|0644

//Format the virtual block device in the following function
int mkfs()
{
	return 0;
}

//Filesystem operations that you need to implement
int fs_getattr (const char *path, struct stat *attr)
{
	printf("Getattr is called:%s\n",path);
	return 0;
}

int fs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	printf("Readdir is called:%s\n", path);
	return 0;
}

int fs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
	printf("Read is called:%s\n",path);
	return 0;
}

int fs_mknod (const char *path, mode_t mode, dev_t dev)
{
	printf("Mknod is called:%s\n",path);
	return 0;
}

int fs_mkdir (const char *path, mode_t mode)
{
	printf("Mkdir is called:%s\n",path);
	return 0;
}

int fs_rmdir (const char *path)
{
	printf("Rmdir is called:%s\n",path);
	return 0;
}

int fs_unlink (const char *path)
{
	printf("Unlink is callded:%s\n",path);
	return 0;
}

int fs_rename (const char *oldpath, const char *newpath)
{
	printf("Rename is called:%s\n",newpath);
	return 0;
}

int fs_write (const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
	printf("Write is called:%s\n",path);
	return 0;
}

int fs_truncate (const char *path, off_t size)
{
	printf("Truncate is called:%s\n",path);
	return 0;
}

int fs_utime (const char *path, struct utimbuf *buffer)
{
	printf("Utime is called:%s\n",path);
	return 0;
}

int fs_statfs (const char *path, struct statvfs *stat)
{
	printf("Statfs is called:%s\n",path);
	return 0;
}

int fs_open (const char *path, struct fuse_file_info *fi)
{
	printf("Open is called:%s\n",path);
	return 0;
}

//Functions you don't actually need to modify
int fs_release (const char *path, struct fuse_file_info *fi)
{
	printf("Release is called:%s\n",path);
	return 0;
}

int fs_opendir (const char *path, struct fuse_file_info *fi)
{
	printf("Opendir is called:%s\n",path);
	return 0;
}

int fs_releasedir (const char * path, struct fuse_file_info *fi)
{
	printf("Releasedir is called:%s\n",path);
	return 0;
}

static struct fuse_operations fs_operations = {
	.getattr    = fs_getattr,
	.readdir    = fs_readdir,
	.read       = fs_read,
	.mkdir      = fs_mkdir,
	.rmdir      = fs_rmdir,
	.unlink     = fs_unlink,
	.rename     = fs_rename,
	.truncate   = fs_truncate,
	.utime      = fs_utime,
	.mknod      = fs_mknod,
	.write      = fs_write,
	.statfs     = fs_statfs,
	.open       = fs_open,
	.release    = fs_release,
	.opendir    = fs_opendir,
	.releasedir = fs_releasedir
};

int main(int argc, char *argv[])
{
	if(disk_init())
		{
		printf("Can't open virtual disk!\n");
		return -1;
		}
	if(mkfs())
		{
		printf("Mkfs failed!\n");
		return -2;
		}
    return fuse_main(argc, argv, &fs_operations, NULL);
}
