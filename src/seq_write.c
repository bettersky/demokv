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

#include <time.h>
#define s_to_ns 1000000000	

int seq_write(){
	printf("i am seq_write\n");	
	
	
	uint64_t i;

	uint64_t size=0;
	size=100000;
	printf("Input size:\n");
	scanf("%llu",&size);
	printf("%llu\n",size);
	srand( (unsigned)time( NULL ) );
	char key[MAX_KEY_LENGTH]={0};
	char value[MAX_VALUE_LENGTH]={0};// =malloc(20);
		//=malloc(10);
double duration=0;
struct timespec begin, end; 
clock_gettime(CLOCK_MONOTONIC,&begin); //begin insert , so begin counting 
	
	for(put_counter=0;put_counter<size;put_counter++){
	
		memset(key,0,MAX_KEY_LENGTH);		
		memset(key,0,MAX_VALUE_LENGTH);
		sprintf(key,"%019llu",put_counter);
		
		generate_string(value, MAX_VALUE_LENGTH);
		
		//printf("key=%s, value=%s\n",key,value);
		put(key,value);
	}

clock_gettime(CLOCK_MONOTONIC,&end); //begin insert , so begin counting
duration=( (int)end.tv_sec+((double)end.tv_nsec)/s_to_ns ) - ( (int)begin.tv_sec+((double)begin.tv_nsec)/s_to_ns );
printf("write duration=%f s\n",duration);		
	//int lev0_nums=0;
	
	
	
	//printf("lev1 tip entry %d: serial=%d, first_key=%s | last_key=%s\n",1, tip_tables_entry[1]->serial_num , tip_tables_entry[1]->first_key,  tip_tables_entry[1]->last_key);	
	
}