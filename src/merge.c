#include "segtable.h"
#include "flash.h" //for device
#include <stdio.h>
#include <math.h>  //pow

extern struct ATABLE *active_table;
extern struct LEV0 * lev0_head;
extern char *table_finder_0;
extern char *lev0_tables[];


int merge1();
int merge2();
int merge(){
	printf("___I am merge \n");
	//step a: recursively find a not full level, if the result is lev0 go to step 1, else go to step b;
	int max_level=MAX_LEV;
	int i;
	FILE *levels_summary=fopen("../ftl/levels_summary", "r+");
	//printf("levels_summary=%p\n", levels_summary);
	for(i=0;i<max_level;i++){
		fseek(levels_summary, i*LEVELS_SUMMARY_ENTRY, SEEK_SET);
		int tables_num ;
		fscanf(levels_summary, "%d", &tables_num);
		//printf("tables_num=%d\n",tables_num);
		if(tables_num < LEV0_NUM* (i+1) ){//here i+1 should be replaced by pow(10, i)
			break;
		}
	}
	if(i==0){
		//go to step 1
		merge1();
	}
	else{
		//go to step b
		merge2();
	}
	//step b: select a table from the higher level, the hightest could be level 0, then merge it to this not full level
			 //the merging steps is like from step 3. When selecting, an intact table would be prior. 
	
	
	//step 1: make a new pointer point to the active table's space, then malloc a new space for the active table
	
	
	//step 2: construct an ordered table from the old active table's data
	
	//step 3: select acrossed tables from lev0
	
	//step 4: merge

}

int merge1(){
//step 1
	struct ATABLE *active_table_old=active_table;//need to free
	active_table=NULL;
	active_table=malloc(sizeof(struct ATABLE));//this should be able to serve new writes immediatly
//step 2	
	struct KNODE *curr_node=active_table_old->key_head;//need to free, a link
	printf("__merge1, active_table_old->kv_num=%d\n",active_table_old->kv_num);
	int i;
	//sort the link list --begin
	for(i=0;i<active_table_old->kv_num -1;i++){ 
		struct KNODE *no_name=curr_node->next;
		int j;
		for(j=i+1;j<active_table_old->kv_num;j++){
			if(strcmp(curr_node->key, no_name->key) >0 ){
				char *temp_key= no_name->key;
				char *temp_value= no_name->value;
				no_name->key= curr_node->key;
				no_name->value=curr_node->value;
				
				curr_node->key=temp_key;
				curr_node->value=temp_value;
			}
			no_name=no_name->next;
		}
		curr_node=curr_node->next;
	}
	//sort the link list --end
	for(curr_node=active_table_old->key_head;curr_node!=NULL;curr_node=curr_node->next){
		printf("after sort, key=%s, value=%s\n",curr_node->key, curr_node->value);
	}
	//construct a sorted table --begin
	char *sorted_active_table=malloc(test_seg_bytes);
	
	int copied_size=0;
	for(curr_node=active_table_old->key_head;curr_node->next!=NULL;curr_node=curr_node->next){
		memcpy(sorted_active_table+copied_size, curr_node->key, strlen(curr_node->key)+1 );
		copied_size+=strlen(curr_node->key)+1;
		memcpy(sorted_active_table+copied_size, curr_node->value, strlen(curr_node->value)+1 );
		copied_size+=strlen(curr_node->value)+1;	
	}
	//leave the last for getting the last key without if clause --begin
	memcpy(sorted_active_table+copied_size, curr_node->key, strlen(curr_node->key)+1 );
	copied_size+=strlen(curr_node->key)+1;
	memcpy(sorted_active_table+copied_size, curr_node->value, strlen(curr_node->value)+1 );
	copied_size+=strlen(curr_node->value)+1;
	char *active_first_key=active_table_old->key_head->key;
	char *active_last_key=curr_node->key;
	printf("first_key:%s last_key:%s\n",active_first_key,active_last_key);
	//leave the last for getting the last key without if clause --begin
	//construct a sorted table --end
	/*
	int read_size=0;
	while(1){
		if(read_size>=test_seg_bytes) break;
		printf("%s ", sorted_active_table+read_size);
		read_size+=strlen(sorted_active_table)+1;
	}
	printf("---------\n");
	exit(2);
	*/
//step 3
	//find crossed tables in lev0 --begin
	
	int lev0_info[LEV0_NUM+1]={0};//the first is total crossed number //if not initialized, the value is not 0
	char find_first_key[FINDER_KEY_LENGTH];
	char find_last_key[FINDER_KEY_LENGTH];
	printf("before while,lev0_info[0],lev0_info[5]=%d\n",lev0_info[0],lev0_info[5]);
	
	
	//char entry_str[FINDRE_NUMBER_LENGTH];//to get entry_num
	//int entry_num=0;
	int if_no_cross_point=0;
	int total_lev0=0;
		for(i=0;i< LEV0_NUM+1;i++){//find crossed table in lev0			
			//int res=fscanf(table_finder_0, "%s", find_first_key);
			//memcpy(entry_str, ( table_finder_0 +  (FINDER_KEY_LENGTH*2)*(entry_num) ), FINDRE_NUMBER_LENGTH );
			//entry_num=atoi)(entry_str);
			//if(entry_num==0){//no following table, end
				//break;
			//}
			//now there is a following table and its entry num has been set
			if(*( table_finder_0 + (FINDER_KEY_LENGTH*2)*(i) ) == 0){//blank entry
				break;
			}
			total_lev0++;
			memcpy(find_first_key, ( table_finder_0 + (FINDER_ENTRY_LENGTH)*(i) ), FINDER_KEY_LENGTH );
			memcpy(find_last_key, ( table_finder_0 + (FINDER_ENTRY_LENGTH)*(i) + FINDER_KEY_LENGTH ), FINDER_KEY_LENGTH );
			if(strcmp(active_last_key, find_first_key)<0 ){//no following crossings
				
				break;
			}
			if( strcmp(active_first_key, find_last_key) >0){//doesn't cross	, but fowllings may be crossed
				continue;
			}
			//now that comes here it must be crossed 
			lev0_info[LEV0_NUM]++;//crossed table plus
			lev0_info[i]=1;//mark the table position as crossed  
			
			
		}
	if_no_cross_point=i;
	//find crossed tables in lev0 --end
	for(i=0;i<LEV0_NUM+1;i++){
		printf("lev0_info[%d]=%d\n", i, lev0_info[i]);
	}
	
	// merge --begin
	if(lev0_info[LEV0_NUM]==0){//no crossed lev0 table. directly place		
		//find an empty entry and update table_finder_0 --begin
				memcpy( ( table_finder_0 + (FINDER_KEY_LENGTH*2)*(if_no_cross_point) ),active_first_key ,strlen(active_first_key) );
				memcpy( ( table_finder_0 + (FINDER_KEY_LENGTH*2)*(if_no_cross_point) + FINDER_KEY_LENGTH  ),active_last_key ,strlen(active_last_key) );
		//find an empty entry and update table_finder_0 --end
		//make the responsible lev0_tables[] pointer point to the table --begin		
		lev0_tables[i]=sorted_active_table;		
		//make the responsible lev0_tables[] pointer point to the table --end
	}
	else{//there are crossed tables,  need merge
		printf("I am, merge1, ***there are %d crossed tables in lev0!\n", lev0_info[LEV0_NUM]);
		//get the crossed tables' pointer --begin
		char *crossed_tables[lev0_info[LEV0_NUM]];//sorted_active_table points to the merging table
		int j=0;
		for(i=0;i<LEV0_NUM;i++){
			if(lev0_info[i]==1){//1 indicates a crossed table 
				crossed_tables[j++]=lev0_tables[i];
			}
		}
		//get the crossed tables' pointer --end
		
		printf("j=%d\n",j);
		//now let's really begin merging --begin
		//1 allocate a big space that can contain all merging tables 
		for(i=0;i<j;i++){//merge one table a time
		
		}
		//2 merge all the tables to this big space
		
		//3 caculate the the number of  big table splitted, if larger than the crossed tables by 1, then move all the following entries by 1
		
		//4 split the big table, and write the entries
		//now let's really begin merging --end
		exit(1);
		if( lev0_info[LEV0_NUM] >=3){
			exit(2);
		}
	
	}
	//exit(1);
	// merge --end
	
	//free pointers --begin
	
	for(curr_node=active_table_old->key_head;curr_node->next!=NULL;curr_node=curr_node->next){
		//free();
	}
	//free pointers --end
}
int merge2(){

}