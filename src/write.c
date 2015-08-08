#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"
#include "tools.h"
extern char *levels_summary;

extern struct DEVICE device;
extern struct ATABLE *active_table;//defined in egtable.h //for test
extern char *lev0_tables[];

int kv_write(){
	printf("i am write\n");	
	//printf("device.mmap_begin=%p\n",device.mmap_begin);
	/*
	srand(time(NULL));
	int r = rand();
	printf("r=%d\n",r);
	int i;
	char *key=malloc(21);
	for(i=0;i<20;i++){
		int remain=r%48;
		*(key+i)=remain+48;
		r=r-remain;
	
	}
	*(key+i)='\0';
	printf("key=%s\n",key);
	*/
	FILE *kvfile=fopen("../fire","r");
	uint64_t i;
	int end=0;
	for(i=0;;i++){
		char *key=malloc(10);;
		char *value=malloc(20);
		int res=fscanf(kvfile,"%5s",key);
		
		if(feof(kvfile)){
			end=1;
			break;
		}
		res=fscanf(kvfile,"%10s",value);
		if(feof(kvfile)){
			
			end=2;
			break;
		}
		//printf("write, i=%d\n",i);
		put(key,value);
	}
	
	//int lev0_nums=0;
	
	if(end==1) printf(" KV file read to end when get key\n"); 
	if(end==2) printf(" KV file read to end when get value\n"); 
	//printf("write end, lev0_nums=%d\n",lev0_nums);
	//printf("levels_summary=%d\n",*(int*)levels_summary);
}