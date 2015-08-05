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
	FILE *kvfile=fopen("../web.spc","r");
	uint64_t i;
	for(i=0;;i++){
		char *key=malloc(10);;
		char *value=malloc(20);
		int res=fscanf(kvfile,"%5s",key);
		if(res==EOF){
			printf("test KV file read to end when get key\n");
			exit(1);
		}
		res=fscanf(kvfile,"%10s",value);
		if(res==EOF){
			printf("test KV file read to end when get value\n");
			exit(1);
		}
		//printf("write, i=%d\n",i);
		put(key,value);
	}
	
	printf("write end\n");
	//printf("levels_summary=%d\n",*(int*)levels_summary);
}