#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"
#include "tools.h"

extern struct DEVICE device;
extern struct ATABLE *active_table;//defined in egtable.h //for test

int kv_write(){
	printf("i am write\n");	
	printf("device.mmap_begin=%p\n",device.mmap_begin);
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
	FILE *kvfile=fopen("../udev","r");
	uint64_t i;
	for(i=0;i<100;i++){
		char *key=malloc(20);;
		char *value=malloc(40);
		fscanf(kvfile,"%5s",key);
		fscanf(kvfile,"%10s",value);
		put(key,value);
	}
	
	struct KNODE *knode;
	knode=active_table->key_head;
	for(;knode!=NULL;knode=knode->next){
		printf("key:%s value:%s\n",knode->key,knode->value);
	}
}