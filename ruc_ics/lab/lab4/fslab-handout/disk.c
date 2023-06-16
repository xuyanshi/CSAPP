/*
Filesystem Lab disigned and implemented by Liang Junkai,RUC
*/

#include "disk.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

char disk_prefix[256];

int disk_init()
{
	FILE*fp=fopen("fuse~","r");
	if(fp==NULL)return 1;
	fscanf(fp,"%s",disk_prefix);
	fclose(fp);
	strcpy(disk_prefix+strlen(disk_prefix)-8,"vdisk/block");
	char name[256];
	char buffer[BLOCK_SIZE];
	memset(buffer,0,sizeof(buffer));
	for(int i=0;i<BLOCK_NUM;++i)
		{
		strcpy(name,disk_prefix);
		sprintf(name+strlen(name),"%d",i);
		FILE*disk=fopen(name,"w");
		if(disk==NULL)return 1;
		fwrite(&buffer,BLOCK_SIZE,1,disk);
		fclose(disk);
		}
	return 0;
}

int disk_read(int block_id,void*buffer)
{
	if(block_id>=BLOCK_NUM||block_id<0)return 1;
	char name[256];
	strcpy(name,disk_prefix);
	sprintf(name+strlen(name),"%d",block_id);
	FILE*disk=fopen(name,"r");
	fread(buffer,BLOCK_SIZE,1,disk);
	fclose(disk);
	return 0;
}

int disk_write(int block_id,void*buffer)
{
	if(block_id>=BLOCK_NUM||block_id<0)return 1;
	char name[256];
	strcpy(name,disk_prefix);
	sprintf(name+strlen(name),"%d",block_id);
	FILE*disk=fopen(name,"w");
	fwrite(buffer,BLOCK_SIZE,1,disk);
	fclose(disk);
	return 0;
}