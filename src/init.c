#include <fcntl.h>    //provides O_RDONLY etc.
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"
#include <linux/fs.h>   //provides BLKGETSIZE
#include <sys/ioctl.h>  //provides ioctl()
 #include <sys/mman.h> //mmap

extern struct DEVICE device;
extern struct ATABLE *active_table;//defined in egtable.h
extern char *table_finder_0;


int read_disk(void *);
int init_ftl();
int init_memory();

int flash_init(void * args){
	read_disk(args);
	init_ftl();
	init_memory();
	//if is formatted, call flash_open
	//else first call flash_formate, then call flash_open
	
}
int read_disk(void * args){
	printf("I am flash_init,\n");
	char *device_name=(char *) args;  
	int fd=-1;
	fd=open(device_name,O_RDWR);
	printf("fd=%d\n",fd);
	
	uint64_t blklb,blkpb,blk64, blk;
		blklb=blkpb=blk64=blk=0;
		ioctl(fd, BLKSSZGET, &blkpb);// is for physical sector size
		ioctl(fd, BLKBSZGET, &blklb);// is for logical sector size
		ioctl(fd, BLKGETSIZE64, &blk64);//reture the size in bytes . This result is real
		ioctl(fd, BLKGETSIZE, &blk);//just /512 of the BLKGETSIZE64
		
	printf("physical block size=%llu, logical block size=%llu,block number in 64 bits=%llu,/512=%llu\n",blkpb,blklb,blk64,blk);
		printf("page=%d B, block=%d MB, area=%d block\n",PAGE_BYTES,BLOCK_BYTES/1024/1024,SEGMENT_BLOCKS);
		uint64_t pages_total=blk64/PAGE_BYTES;
		
	uint64_t bytes_offset=0;
	unsigned char*	dev=(unsigned char*)mmap(NULL,blk64,PROT_WRITE,MAP_SHARED,fd,bytes_offset);
	
	printf("dev=%p\n",dev);
	*dev='g';
	device.mmap_begin=dev;
	device.segment_bytes=test_seg_bytes;//SEGMENT_BLOCKS*BLOCK_BYTES;
	printf("device.mmap_begin=%p\n",device.mmap_begin);


}

int init_ftl(){
	printf("i am init_ftl\n");
	FILE *map_table=fopen("../ftl/map_table","r+");
	FILE *log_pointer=fopen("../ftl/log_pointer","r+");
	int fd_table_finder_0=open("../ftl/table_finder_0",O_RDWR);
	int res=lseek(fd_table_finder_0,PAGE_BYTES-1,SEEK_SET);
	write(fd_table_finder_0,"\0",1);
	table_finder_0=(char *)mmap(NULL, PAGE_BYTES ,PROT_WRITE,MAP_SHARED,fd_table_finder_0,0);
	memset(table_finder_0, 0, PAGE_BYTES);
	printf("map_table=%p, log_pointer=%p, table_finder_0=%p\n",map_table,log_pointer,table_finder_0);
	uint64_t i;
	for(i=0;i<100;i++){
		//fprintf(map_table,"%019d\n",i);
	}
	printf("ftell returns:%d\n",ftell(map_table));
	i=2;
	fseek(map_table,i*20,SEEK_SET);
	printf("ftell returns:%d\n",ftell(map_table));
	uint64_t phy;
	fscanf(map_table,"%llu",&phy);
	printf("phy:%d\n",phy);
	
	 /*
	for(i=0;i<LEV0_NUM+1;i++){
		memcpy(table_finder_0+ (i+1)*(FINDER_KEY_LENGTH*2 +1) -1 ,"\n",1);
		//fseek(table_finder_0, (FINDER_KEY_LENGTH*2 +1 ) * i, SEEK_SET);		
	}
	*/

}

int init_memory(){
	active_table=malloc(sizeof(struct ATABLE));
	

}