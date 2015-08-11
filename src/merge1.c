#include "segtable.h"
#include "flash.h" //for device
#include "tools.h"

#include <stdio.h>
#include <math.h>  //pow

#include "merge.h"
int merge1_num=0;

int merge_counter[2]={0};

int merge1(int full_lev){
if(full_lev==MAX_LEV-1){
	printf("!!! all levels are full, exit\n");
	exit(9);
}
int i;
	merge1_num++;
	int counter_flag;
	if(full_lev==-1){ 
		counter_flag=0;		
	}
	else{
		counter_flag=1;
		//if(merge_counter[counter_flag]==5) exit(3);
	}
	merge_counter[counter_flag]++;
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I am merge1, full_lev=%d, counter=%d \n", full_lev,merge_counter[counter_flag]);
	
	
	char *tip_table=NULL;
	char *tip_first_key=NULL;
	char *tip_last_key=NULL;
	if(full_lev==-1){
		char *sorted_active_table=malloc(test_seg_bytes);//should be freed
		memset(sorted_active_table,0, test_seg_bytes);	
		fill_sorted_active_table(sorted_active_table,&tip_first_key,&tip_last_key);
		tip_table=sorted_active_table;
	}
	else{
		give_tip_table(&tip_table, full_lev,&tip_first_key, &tip_last_key );

	}
	
	struct FINDER_ENTRY * crossed_entry_chain=(struct FINDER_ENTRY*)malloc(sizeof(struct FINDER_ENTRY));//for create big_table
								//all links in the chain should be freed
	memset(crossed_entry_chain,0,sizeof(struct FINDER_ENTRY));
	struct FINDER_ENTRY *insert_point=NULL;//for split big_table and inserting new tables
	int crossed_num;//crossed_entry_chain will be decided by the first crossed entry and the crossed_num. because crossd tables aways
						//link in sequence
	crossed_num=give_crossed_chain(full_lev+1, tip_first_key, tip_last_key, crossed_entry_chain, &insert_point);
	
	if(crossed_entry_chain->next){
		//print_table("**********", crossed_entry_chain->next->table);
		//exit(1);
	}	
	
	int big_table_size=(crossed_num+2)* test_seg_bytes;//crossed_num may be -1 //in chain crossed_num cann't be -1
	char *big_table=(char*)malloc(big_table_size);
	memset(big_table,0, big_table_size );
	fill_big_table(big_table, tip_table, crossed_entry_chain, crossed_num);
	if(counter_flag==1){
		//print_table("tip_table",tip_table);
		//print_table("big_table",big_table);
	}
	
	
	split_big_table(big_table, crossed_num,  insert_point, full_lev+1);
	 
	if(full_lev>=0){
		chop_tip_entry(full_lev);
	}
	else{
		free(tip_table);
	}
	
	//free
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I am merge1, end, full_lev=%d, counter=%d \n", full_lev,merge_counter[counter_flag]);

}




