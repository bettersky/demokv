#include <fcntl.h>    //provides O_RDONLY etc.
#include "std.h"
#include <math.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"
#include <linux/fs.h>   //provides BLKGETSIZE
#include <sys/ioctl.h>  //provides ioctl()
#include <sys/mman.h> //mmap

#include "tools.h"

extern struct DEVICE device;
extern struct ATABLE *active_table;//defined in egtable.h
extern char *table_finder_0;
extern char *levels_summary;

extern uint64_t serials_base[]; //all levels' base serial number. 0 is reserved
extern uint64_t serials_width[];// all levels' serial width. decided by the LEV)_NUM and LEV_PUFFER
extern char *seg_bit_maps[];

extern struct FINDER_ENTRY *tip_tables_entry[]; //pointers of the last tables of every level if the level is full. 0 is reserved to lev 0
													//NULL indicates the level is not full
extern struct FINDER_ENTRY *first_tables_entry[];

extern struct KNODE *active_chain_head;
extern flag_width[];

int read_disk(void *);
int init_ftl();
int init_memory();
int init_ftl_sub(char *ftl_path, uint64_t ftl_size, char **ftl_name);

int init_serial();
int init_seg_bit_maps();

int init_tables_entry();

int init_flag();
int flash_init(void * args){
	read_disk(args);
	init_ftl();
	printf("____________ in flash_init,levels_summary=%p\n",levels_summary);
	init_memory();//allocate active table. 
	
	init_serial();
	init_seg_bit_maps();
	init_tables_entry();
	
	init_flag();
	//exit(10);
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
		
	//printf("physical block size=%llu, logical block size=%llu,block number in 64 bits=%llu,/512=%llu\n",blkpb,blklb,blk64,blk);
		//printf("page=%d B, block=%d MB, area=%d block\n",PAGE_BYTES,BLOCK_BYTES/1024/1024,SEGMENT_BLOCKS);
		uint64_t pages_total=blk64/PAGE_BYTES;
		
	uint64_t bytes_offset=0;
	char*	dev=(char*)mmap(NULL,blk64,PROT_WRITE,MAP_SHARED,fd,bytes_offset);
	
	printf("dev=%p\n",dev);
	//*dev='g';
	device.mmap_begin=dev;
	device.segment_bytes=test_seg_bytes;//SEGMENT_BLOCKS*BLOCK_BYTES;
	device.segment_total=blk64/device.segment_bytes;
	//printf("device.mmap_begin=%p\n",device.mmap_begin);


}




int init_ftl(){
	//printf("i am init_ftl\n");
	uint64_t i;
	//init the table_finder_0 --begin
	init_ftl_sub("../ftl/table_finder_0", PAGE_BYTES,&table_finder_0);
	//init the table_finder_0 --end
	//printf("map_table=%p, log_pointer=%p, table_finder_0=%p\n",map_table,log_pointer,table_finder_0);
	
	//init the levels_summary --begin
	init_ftl_sub("../ftl/levels_summary", PAGE_BYTES*4,&levels_summary);
	
	//init the levels_summary --end
	
	for(i=0;i<100;i++){
		//fprintf(map_table,"%019d\n",i);
	}
	//printf("ftell returns:%d\n",ftell(map_table));
	
	//printf("phy:%d\n",phy);
	
	 /*
	for(i=0;i<LEV0_NUM+1;i++){
		memcpy(table_finder_0+ (i+1)*(FINDER_KEY_LENGTH*2 +1) -1 ,"\n",1);
		//fseek(table_finder_0, (FINDER_KEY_LENGTH*2 +1 ) * i, SEEK_SET);		
	}
	*/

}




int init_memory(){
	active_table=malloc(sizeof(struct ATABLE));
	memset(active_table, 0, sizeof(struct ATABLE));
	active_table->key_head=malloc(sizeof(struct KNODE));
	memset( active_table->key_head, 0,sizeof(struct KNODE));
	
	//active_chain_head=(struct KNODE*)malloc(sizeof(struct KNODE));
	//memset(active_chain_head, 0, sizeof(struct KNODE));
}




int init_ftl_sub(char *ftl_file_path, uint64_t ftl_size, char **ftl_name){
	int fd=open(ftl_file_path , O_RDWR);
	int res=lseek(fd,ftl_size-1,SEEK_SET);
	write(fd,"\0",1);
	*ftl_name=(char *)mmap(NULL, ftl_size ,PROT_WRITE,MAP_SHARED,fd,0);
	memset(*ftl_name, 0, ftl_size);
	close(fd);

}



int init_serial(){
	int i;
	serials_width[0]=LEV0_NUM;
	serials_base[0]=SERIAL_BASE;
	for(i=1;i<MAX_LEV;i++){
		serials_width[i]=simple_pow(LEV_PUFFER, i);
		serials_base[i]=serials_base[i-1]+serials_width[i-1];
	}
}


int init_seg_bit_maps(){//now we just set all bits/bytes to 0, but actually we should read them from ftl
	int i;
	
	
	for(i=0;i<MAX_LEV;i++){
		seg_bit_maps[i]=(char *)malloc(serials_width[i]);
		memset(seg_bit_maps[i], 0, serials_width[i]);	
	}
	
	//print_bit_map("init",seg_bit_maps[1], serials_width[1] );
	
}




int init_tables_entry(){
	int i;
	for(i=0;i<MAX_LEV;i++){
		struct FINDER_ENTRY *head_entry=(struct FINDER_ENTRY *)malloc(sizeof(struct FINDER_ENTRY) );
		head_entry->next=NULL;
		head_entry->pre=NULL;
		head_entry->serial_num=0;
		first_tables_entry[i]=head_entry;
	}


}

int init_flag(){
	int i;
	for(i=1;i<FLAG_LEVEL_MAX;i++){
		flag_width[i]=simple_pow(FLAG_LEVEL_PUFFER, i);
	}

}