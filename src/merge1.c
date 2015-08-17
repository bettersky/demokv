#include "segtable.h"
#include "flash.h" //for device
#include "tools.h"

#include <stdio.h>
#include <math.h>  //pow

#include "merge.h"

#include <time.h>
#define s_to_ns 1000000000


int merge1_num=0;

int merge_counter[2]={0};

int merge1(int full_lev){
//printf("\nI am merge1 \n");
	if(full_lev==MAX_LEV-1){
		printf("!!! all levels are full, exit\n");
		exit(9);
	}
	int i;
	merge1_num++;
	//if(merge1_num==3) exit(4);
	int counter_flag;
	if(full_lev==-1){ 
		counter_flag=0;		
	}
	else{
		//exit(1);
		counter_flag=1;
		//if(merge_counter[counter_flag]==5) exit(3);
	}
	merge_counter[counter_flag]++;
printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I am merge1, full_lev=%d, counter=%d \n", full_lev,merge_counter[counter_flag]);
double duration=0;
struct timespec begin, end; 
clock_gettime(CLOCK_MONOTONIC,&begin); //begin insert , so begin counting 
	
	char *tip_table=NULL;
	char *tip_first_key=NULL;
	char *tip_last_key=NULL;
	if(full_lev==-1){

		tip_table=malloc(test_seg_bytes);//should be freed
		memset(tip_table,0, test_seg_bytes);	
		//printf("..............\n");	

		fill_sorted_active_table(tip_table,&tip_first_key,&tip_last_key);
//printf("3333333333333\n");	

		//tip_table=sorted_active_table;

	}
	else{
		give_tip_table(&tip_table, full_lev,&tip_first_key, &tip_last_key );

	}
//printf("xxxxxxxxxxx\n");	
	struct FINDER_ENTRY * crossed_entry_chain=(struct FINDER_ENTRY*)malloc(sizeof(struct FINDER_ENTRY));//for create big_table
								//all links in the chain should be freed
				//this chain is a part in the total entry link list. now this is freed in split_big_table()
	memset(crossed_entry_chain,0,sizeof(struct FINDER_ENTRY));
	struct FINDER_ENTRY *insert_point=NULL;//for split big_table and inserting new tables
	int crossed_num;//crossed_entry_chain will be decided by the first crossed entry and the crossed_num. because crossd tables aways
						//link in sequence
//printf("3333333333333\n");
	crossed_num=give_crossed_chain(full_lev+1, tip_first_key, tip_last_key, crossed_entry_chain, &insert_point);
//printf("666666666666\n");	
	//if(crossed_num>0){
		//print_table("tip_table",tip_table);
		//print_table("crossed_entry_chain",crossed_entry_chain->next->table);
		//exit(1);
	//}	
	if(crossed_entry_chain->next){
		//print_table("**********", crossed_entry_chain->next->table);
		//exit(1);
	}	
	int big_table_size=(crossed_num+2)* test_seg_bytes;//crossed_num may be -1 //in chain crossed_num cann't be -1
	char *big_table=(char*)malloc(big_table_size);
	//this should be freed after split, because split function mallocs new space to store data
	memset(big_table,0, big_table_size );
	fill_big_table(big_table, tip_table, crossed_entry_chain, crossed_num);

	if(counter_flag==0){
		//print_table("tip_table",tip_table);
		//print_table("big_table",big_table);
	}
	
//printf("yyyyyyyyyyyy\n");	
	split_big_table(big_table, crossed_num,  insert_point, full_lev+1);
	free(big_table);
	if(full_lev==-1){
		free(tip_table);		
	}
	else{
		chop_tip_entry(full_lev);
	}
	
	free(crossed_entry_chain);
	//free
clock_gettime(CLOCK_MONOTONIC,&end); //begin insert , so begin counting
duration=( (int)end.tv_sec+((double)end.tv_nsec)/s_to_ns ) - ( (int)begin.tv_sec+((double)begin.tv_nsec)/s_to_ns );
printf("merge duration=%f s\n",duration);
printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I am merge1, end, full_lev=%d, counter=%d \n", full_lev,merge_counter[counter_flag]);

}




