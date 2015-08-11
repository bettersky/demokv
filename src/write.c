#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"
#include "tools.h"
extern char *levels_summary;

extern struct DEVICE device;
extern struct FINDER_ENTRY *first_tables_entry[MAX_LEV];
extern struct FINDER_ENTRY *tip_tables_entry[MAX_LEV];



int kv_write(){
	printf("i am write\n");	
	
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
	
	for(i=0;i<MAX_LEV;i++){
		int x=*(int*)(levels_summary+i*LEVELS_SUMMARY_ENTRY);
		if(x==0){
			printf("lev %d is zero\n",i);
			break;
		}
		printf("tables in lev %d: %d\n",i,x);
		
		struct FINDER_ENTRY *test=first_tables_entry[i]->next;
		int j=0;
		while(test!=NULL){
			j++;
			printf("entry %d: serial=%d, first_key=%s | last_key=%s\n",j, test->serial_num ,test->first_key, test->last_key);
			test=test->next;
		}
	}
	
	
	//printf("lev1 tip entry %d: serial=%d, first_key=%s | last_key=%s\n",1, tip_tables_entry[1]->serial_num , tip_tables_entry[1]->first_key,  tip_tables_entry[1]->last_key);	
	
}