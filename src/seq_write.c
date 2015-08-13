#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"
#include "tools.h"

#include <time.h>
extern char *levels_summary;

extern struct DEVICE device;
extern struct FINDER_ENTRY *first_tables_entry[MAX_LEV];
extern struct FINDER_ENTRY *tip_tables_entry[MAX_LEV];

extern int  merge_recur_num;
uint64_t put_counter=0;

void generate_string(char *value, int max){
	int i;
	
	i=rand();
	sprintf(value, "%d",i);
}

int seq_write(){
	printf("i am seq_write\n");	
	
	
	uint64_t i;
	int end=0;
	uint64_t size=0;
	size=100000;
	printf("Input size:");
	//scanf("%llu",&size);
	
	srand( (unsigned)time( NULL ) );
	for(put_counter=0;put_counter<size;put_counter++){
		char *key=malloc(10);
		memset(key,0,10);
		char *value=malloc(20);
		memset(key,0,10);
		
		sprintf(key,"%19llu",put_counter);
		generate_string(value, 20);
		
		//printf("key=%s, value=%s\n",key,value);
		put(key,value);
	}
	
	//int lev0_nums=0;
	
	
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
		//	printf("entry %d: serial=%d, first_key=%s | last_key=%s\n",j, test->serial_num ,test->first_key, test->last_key);
			test=test->next;
		}
	}
	
	printf("merge_recur_num=%d\n",merge_recur_num);
	//printf("lev1 tip entry %d: serial=%d, first_key=%s | last_key=%s\n",1, tip_tables_entry[1]->serial_num , tip_tables_entry[1]->first_key,  tip_tables_entry[1]->last_key);	
	
}