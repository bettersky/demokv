#ifndef FLASH
#define FLASH
//flash.h
#include "std.h"
#define PAGE_BYTES 1024  //1KB
#define BLOCK_PAGES 4096 //4K pages a block
#define BLOCK_BYTES 4194304 //4MB
#define SEGMENT_BLOCKS 1 //1 block
#define VERSION_BYTES 4

#define test_seg_bytes 2048  //1M //actually seg_bytes should be the same with segment_bytes

struct DEVICE{
	unsigned char * mmap_begin;
	uint64_t segment_bytes;
	uint64_t segment_total;
	
};
int flash_open(); //if the device is fit to flashkv, then call this function to obtain internal information and 
					//store them in some structures

int flash_formate();//this function use the device_info the formate the  device

int flash_init(void *);//this function obtain disk's external information and store it in a structure named device_info 

char * read_seg(uint64_t);
int discard_seg(uint64_t serial);

uint64_t *allocate_serial(int lev, int num);

int write_seg(char *seg_buffer, uint64_t serial_num);

uint64_t ftl_addr_map(uint64_t serial_num, char *arg);

int set_bit_map(int lev, uint64_t serial_num);


#endif