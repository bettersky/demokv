#include "segtable.h"
#include "flash.h" //for device
#include "tools.h"

#include <stdio.h>
#include <math.h>  //pow

extern struct ATABLE *active_table;
extern struct LEV0 * lev0_head;
extern char *table_finder_0;
extern char *lev0_tables[];
extern char *levels_summary;

int counter=0;


int fill_sorted_active_table(char *sorted_active_table,int *lev0_info);
int set_base_entry(int *);
int set_crossed_num(int *lev0_info);
int fill_big_table(char *big_table, char *sorted_active_table, int *lev0_info );
int split_big_table(char *big_table,int *);

int merge1(){
	counter++;
	//if(counter>3) exit(1);
	int i;
	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I am merge1, counter=%d \n",counter);

	//print_atable("active_table", active_table);
	char *sorted_active_table=malloc(test_seg_bytes);
	
	memset(sorted_active_table,0, test_seg_bytes);;
	int lev0_info[LEV0_NUM+1]={0};//the last is total crossed number 
	
	fill_sorted_active_table(sorted_active_table,lev0_info);//this will alse find crossed tables
	
	//print_table("sorted_active_table",sorted_active_table);
	
	for(i=0;i<LEV0_NUM+1;i++){
		printf("lev0_info[%d]=%d\n",i,lev0_info[i]);
	}
	
	int base_entry=set_base_entry(lev0_info);
	int crossed_num=lev0_info[LEV0_NUM];
	printf("base_entry=%d\n", base_entry);
	
	char *big_table=(char*)malloc( (crossed_num+1)* test_seg_bytes );
	memset(big_table, 0 , (crossed_num+1)* test_seg_bytes);
	
	fill_big_table(big_table, sorted_active_table, lev0_info );
	
	//print_table("big_table after merge",big_table);
		
	split_big_table(big_table, lev0_info);

	free(sorted_active_table);
	
	printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<I am merge1, end ,counter=%d \n",counter);
	//exit(1);

}




int fill_lev0_info(int *lev0_info,char *active_first_key,char *active_last_key);
int fill_sorted_active_table(char *sorted_active_table ,int *lev0_info){
	struct ATABLE *active_table_old=active_table;
	active_table=NULL;
	active_table = (struct ATABLE *)malloc(sizeof(struct ATABLE));//this should be able to serve new writes immediatly
	memset(active_table, 0 , sizeof(struct ATABLE));
	
	struct KNODE *curr_node=active_table_old->key_head;//need to free, a linked list
	int i;

	for(;curr_node->next!=NULL;){ //sort the link list
	//every cycle make the smallest remain knode  to the current position
	//compare one by one with the following knodes, if this is bigger, then exchange their KV data
	//
		struct KNODE *stamp=curr_node;
		//printf("xxxxxxxxxxxxxxxx\n");
		for(;stamp->next!=NULL;){
			struct KNODE *no_name= stamp->next;
			
			if(strcmp(curr_node->key, no_name->key) ==0){//delete the repeated key nodes			
				stamp->next=no_name->next;
				free(no_name->key);
				free(no_name->value);	
				free(no_name);				
				continue;
			}
			//printf("zzzzzzzzzzzzz\n");
			if(strcmp(curr_node->key, no_name->key) >0 ){//exchange the KV
				
				char *temp_key= no_name->key;
				char *temp_value= no_name->value;
				no_name->key= curr_node->key;
				no_name->value=curr_node->value;				
				curr_node->key=temp_key;
				curr_node->value=temp_value;
				
			}
			stamp=stamp->next;
		}
		
		//printf("end for\n");
		//printf("end for 1. curr_node=%p, curr_node->next=%p\n",curr_node,curr_node->next);
		if(curr_node->next==NULL) break; 
		curr_node=curr_node->next;//sort the next node
		//printf("end for 2. curr_node=%p\n",curr_node);
	}

	int copied_size=0;//indicates the total bytes of data have been copied to sorted_active_table
	for(curr_node=active_table_old->key_head;curr_node->next!=NULL;curr_node=curr_node->next){
		memcpy(sorted_active_table+copied_size, curr_node->key, strlen(curr_node->key)+1 );
		copied_size+=strlen(curr_node->key)+1;
		memcpy(sorted_active_table+copied_size, curr_node->value, strlen(curr_node->value)+1 );
		copied_size+=strlen(curr_node->value)+1;	
	}
	
	//leave the last for getting the last key without if clause --begin
	int last_key_offset=copied_size;
	memcpy(sorted_active_table+copied_size, curr_node->key, strlen(curr_node->key)+1 );
	copied_size+=strlen(curr_node->key)+1;
	memcpy(sorted_active_table+copied_size, curr_node->value, strlen(curr_node->value)+1 );
	copied_size+=strlen(curr_node->value)+1;
	char *active_first_key=sorted_active_table;//active_table_old->key_head->key;
	char *active_last_key=sorted_active_table+last_key_offset;//curr_node->key;
	//printf("first_key:%s last_key:%s\n",active_first_key,active_last_key);
	//leave the last for getting the last key without if clause --end
	//construct a sorted table --end
	//free struct ATABLE *active_table_old
		
	for(curr_node=active_table_old->key_head;curr_node!=NULL;){
		free(curr_node->key);
		free(curr_node->value);
		struct KNODE *temp=curr_node;
		curr_node=curr_node->next;
		free(temp);
			//free big table
	}
	free(active_table_old);

	fill_lev0_info(lev0_info, active_first_key, active_last_key );
	
}

int fill_lev0_info(int *lev0_info, char *active_first_key, char *active_last_key){
	char find_first_key[FINDER_KEY_LENGTH];//record the current finding table's first key 
	char find_last_key[FINDER_KEY_LENGTH];//record the current finding table's last key 	
	int i;
	
	for(i=0;i< LEV0_NUM;i++){//find crossed tables in lev0	
			//now there is a following table and its entry num has been set
			if(*( table_finder_0 + FINDER_ENTRY_LENGTH*(i) ) == 0){//blank entry
				break;
			}		
			memcpy(find_first_key, ( table_finder_0 + (FINDER_ENTRY_LENGTH)*(i) ), FINDER_KEY_LENGTH );
			memcpy(find_last_key, ( table_finder_0 + (FINDER_ENTRY_LENGTH)*(i) + FINDER_KEY_LENGTH ), FINDER_KEY_LENGTH );
			if(strcmp(active_last_key, find_first_key)<0 ){//no following crossings
				lev0_info[i]=3;
				break;
			}
			if( strcmp(active_first_key, find_last_key) >0){//doesn't cross	, but fowllings may be crossed
				lev0_info[i]=2;
				continue;
			}
			//now that comes here it must be crossed 
			
			lev0_info[LEV0_NUM]++;//crossed table plus
			lev0_info[i]=1;//mark the table position as crossed 
			
			
					
	}
	
}







int set_base_entry(int *lev0_info){
	
	int i;
	for(i=0;i<LEV0_NUM;i++){
		if( lev0_info[i]==1 || lev0_info[i]==0) return i;
		
	}

}







int fill_big_table(char *big_table, char *sorted_active_table, int *lev0_info ){
int i;
	int crossed_num=lev0_info[LEV0_NUM];
	char *union_crossed_tables=malloc( (crossed_num)* test_seg_bytes +1 );//+1 is for the case crossed_num=0;
	memset(union_crossed_tables, 0, (crossed_num)* test_seg_bytes +1);
	char *crossed_tables[lev0_info[LEV0_NUM]];//this will point to the crossed tables
	
	char *union_advancer=union_crossed_tables;//anvances the cross union
	
	int j=0;
	for(i=0;i<LEV0_NUM;i++){//make the crossed_tables points to crossed tables serially
			if(lev0_info[i]==1){//1 indicates a crossed table 
				crossed_tables[j]=lev0_tables[i];
				j++;
			}
	}
		
	printf("crossed_num=%d, j=%d\n",crossed_num,j);
	//j equals crossed_num
	for(i=0;i<j;i++){//copy the crossed tables to union_crossed_tables
			//uint32_t copy_size=*(uint32_t *) (crossed_tables[i] + test_seg_bytes - TABLE_END_RESERVED + TABLE_END_ZERO);
			
			char *advancer=crossed_tables[i];//every cycle this points to a next crossed table
			uint32_t copy_size=0;
			printf("debug1, i=%d, advancer=%p\n",i,advancer);
			
			while(*advancer!=0){//caculate the i-th table size
				int kv_len=strlen(advancer)+1 + strlen(advancer+ strlen(advancer)+1 ) +1;
				copy_size+=kv_len;
				advancer+=kv_len;
				
			}
			memcpy(union_advancer,crossed_tables[i], copy_size );//copy the i-th crossed table
			union_advancer+= copy_size;
	}
	//print_table("union_crossed_tables in merging",union_crossed_tables);
	
	char *sorted_table_advancer=sorted_active_table;
	union_advancer=union_crossed_tables;
	char *big_table_advancer=big_table;
	
	i=0;
	while(1){
	
			if(*sorted_table_advancer==0){
			//printf("*sorted_table_advancer==0\n");
				while(*union_advancer!=0){
					int kv_len=strlen(union_advancer)+1 + strlen(union_advancer+ strlen(union_advancer)+1 ) +1;
					memcpy(big_table_advancer, union_advancer,kv_len );
					union_advancer += kv_len;
					big_table_advancer += kv_len;
				}
				break;
			}
			if(*union_advancer==0){
				//printf("*union_advancer==0\n");
				while(*sorted_table_advancer!=0){
					int kv_len=strlen(sorted_table_advancer)+1 + strlen(sorted_table_advancer+ strlen(sorted_table_advancer)+1 ) +1;
					memcpy(big_table_advancer, sorted_table_advancer,kv_len );
					sorted_table_advancer += kv_len;
					big_table_advancer += kv_len;
				}
				break;
			}
			
			int res=strcmp(sorted_table_advancer,union_advancer);
			if(res<=0){
				
				int kv_len=strlen(sorted_table_advancer)+1 + strlen(sorted_table_advancer+ strlen(sorted_table_advancer)+1 ) +1;
				//printf("res<=0, res=%d, kv_len=%d\n",res, kv_len);
				memcpy(big_table_advancer, sorted_table_advancer,kv_len );
				sorted_table_advancer += kv_len;
				big_table_advancer += kv_len;
				if(res==0){
					//printf("res=0, res=%d, kv_len=%d\n",res, kv_len);
					union_advancer += strlen(union_advancer) +1;//key
					union_advancer += strlen(union_advancer) +1;//value
				}
			}
			else{
				i++;
				//if(i>22) break;
				int kv_len=strlen(union_advancer)+1 + strlen(union_advancer+ strlen(union_advancer)+1 ) +1;
				//printf("res>0, res=%d, kv_len=%d, union_advancer=%s\n",res, kv_len,union_advancer);
				memcpy(big_table_advancer, union_advancer,kv_len );
				union_advancer += kv_len;
				big_table_advancer += kv_len;
			}
			
			
	}
	
	
	free(union_crossed_tables);

	return 1;

}












int split_big_table(char *big_table, int*lev0_info){
int i;
int crossed_num=lev0_info[LEV0_NUM];
	char *big_table_advancer=big_table;
	int splitting_counter=0;
	char *splitted_tables_pointer[crossed_num];
	int splitted_tables_num=0;
	
	char *manua_splitted_first_key[LEV0_NUM];
	manua_splitted_first_key[0]=big_table_advancer;
	char *manua_splitted_last_key[LEV0_NUM];
	char *start_pointer=big_table_advancer;//records the start point when splitting table
	char *p0,*p1;//for recording two 
printf("!!!!!split_big_table\n");	
	int base_entry=set_base_entry(lev0_info);
	while(1){//every cycle advance a KV
		if(*big_table_advancer==0){//advance to the end, finish
				break;
		}
		p0=p1;//if would split, p0 points to the last key
		p1=big_table_advancer;//if would split, p1 points to the first key of the next table
		
		int len_key= strlen(big_table_advancer)+1;
		big_table_advancer += len_key;//advances the key
		int len_value =strlen(big_table_advancer)+1;
		big_table_advancer += len_value; //advances the value 		
		
		if( splitting_counter + len_key + len_value + TABLE_END_RESERVED > test_seg_bytes){//splitting
			
			//lev0_tables[base_entry+splitted_tables_num]=malloc(0);//for freeing
			//free(lev0_tables[base_entry+splitted_tables_num]);
			//lev0_tables[base_entry+splitted_tables_num]=(char *)malloc(test_seg_bytes);
			//memset(lev0_tables[base_entry+splitted_tables_num],0 , test_seg_bytes);
			//memcpy( lev0_tables[base_entry+splitted_tables_num], start_pointer, p1-start_pointer);
			splitted_tables_pointer[splitted_tables_num]=(char *)malloc(test_seg_bytes);
			memset(splitted_tables_pointer[splitted_tables_num],0 , test_seg_bytes);			
			memcpy(splitted_tables_pointer[splitted_tables_num], start_pointer, p1-start_pointer);
			manua_splitted_last_key[splitted_tables_num]=p0;
			
			splitted_tables_num++;
			splitting_counter=0;				
			manua_splitted_first_key[splitted_tables_num]=p1;
			start_pointer=p1;//resetg the start pointer
				
		}
		splitting_counter += len_key+len_value;
	}
	//splitting finished. splitted tables are store int splitted_tables_pointer[]. there are splitted_tables_num splitted+1 tables
	//but the last splitted tables has not been stored. we do this int the next
	manua_splitted_last_key[splitted_tables_num]=p1;
	splitted_tables_pointer[splitted_tables_num]=(char *)malloc(test_seg_bytes);
	memset(splitted_tables_pointer[splitted_tables_num],0 , test_seg_bytes);
	memcpy(splitted_tables_pointer[splitted_tables_num], start_pointer, big_table_advancer-start_pointer);
	splitted_tables_num++;//make it the ture num of splitted tables
	//now if crossed table number is 0, directly make the lev0_tables[] from base_entry point to the splitted tables
	//else if splitted table number is equal to the crossed table number, also can directly change lev0_tables pointer
	//else the splitted table number must be bigger by 1 than crossed number, so we first move the pointer of the lev0_tables after the 
		//crossed by one, then make the lev0_tables[] from basse_entry point tot the splitted tables
	//and notice: before change the lev0_tables[] pointer, first free it 
	//and notice: every time the lev0_tables[] pointer are changed, the table_finder_0 should be updated
	//update lev0_tables[] --begin
	for(i=0;i<crossed_num;i++){		
			free(lev0_tables[base_entry+i]);
	}
	if(crossed_num==0|| splitted_tables_num==crossed_num){//direnctly point
		for(i=0;i<splitted_tables_num;i++){		
			
			lev0_tables[base_entry+i]=splitted_tables_pointer[i];
		}
	}
	else{
		for(i=LEV0_NUM-1;i>=base_entry+crossed_num;i--){//move
			lev0_tables[i]=lev0_tables[i-1];
		}
		memmove(table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+crossed_num+1), table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+crossed_num), FINDER_ENTRY_LENGTH*(LEV0_NUM-(base_entry+crossed_num+1)));
		for(i=0;i<splitted_tables_num;i++){					
			lev0_tables[base_entry+i]=splitted_tables_pointer[i];
		}
	}
	//update lev0_tables[] --end

	//update levels_summary --begin
		if(splitted_tables_num> crossed_num){
			( *(int*) levels_summary)++;
		}
	//update levels_summary --end
	
	//update the entries --begin
		//the moving action has been done in updating the lev0_tables[]
		for(i=0;i<splitted_tables_num;i++){
			memset(table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+i), 0 , FINDER_KEY_LENGTH*2);//clear old data
			memcpy(table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+i), manua_splitted_first_key[i], strlen(manua_splitted_first_key[i]) );
			memcpy(table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+i)+FINDER_KEY_LENGTH, manua_splitted_last_key[i], strlen(manua_splitted_last_key[i]) );
		}
	//update the entries --end

		free(big_table);
		
		printf("split finished, splitted_tables_num=%d, crossed=%d\n",splitted_tables_num, crossed_num);

}