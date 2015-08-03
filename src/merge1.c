#include "segtable.h"
#include "flash.h" //for device
#include <stdio.h>
#include <math.h>  //pow

extern struct ATABLE *active_table;
extern struct LEV0 * lev0_head;
extern char *table_finder_0;
extern char *lev0_tables[];
extern char *levels_summary;

int counter=0;

void print_table(char *str, char *table);
int fill_sorted_active_table(char *sorted_active_table,int *lev0_info);
int set_base_entry(int *);
int set_crossed_num(int *lev0_info);
int fill_big_table(char *big_table, char *sorted_active_table, int *lev0_info );
int split_big_table(char *big_table,int *);

int merge1(){
	counter++;
	if(counter>2) exit(1);
	int i;
	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I am merge1, counter=%d \n",counter);
	
	
	char *sorted_active_table=malloc(test_seg_bytes);
	memset(sorted_active_table,0, test_seg_bytes);;
	int lev0_info[LEV0_NUM+1]={0};//the last is total crossed number 
	
	
	fill_sorted_active_table(sorted_active_table,lev0_info);
	exit(1);
	print_table("sorted_active_table",sorted_active_table);
	
	for(i=0;i<LEV0_NUM+1;i++){
		printf("lev0_info[%d]=%d\n",i,lev0_info[i]);
	}
	
	int base_entry=set_base_entry(lev0_info);
	int crossed_num=lev0_info[LEV0_NUM];
	printf("base_entry=%d\n", base_entry);
	
	char *big_table=(char*)malloc( (crossed_num+1)* test_seg_bytes );
	memset(big_table, 0 , (crossed_num+1)* test_seg_bytes);
	fill_big_table(big_table, sorted_active_table, lev0_info );
	
	
	split_big_table(big_table, lev0_info);
	
	printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<I am merge1, end ,counter=%d \n",counter);
	//exit(1);

}














void print_table(char* str,char *table){
	char *test=table;
	int i=0;
	int size=0;
	printf("\n---%s------------------------------------------------------\n",str);
	while(1){//print test			
			if(*test==0) break;
				printf("%d: key=%s, ", ++i, test);
				size+=strlen(test)+1;
				test=test+strlen(test)+1;
				
				printf("value=%s,                        ", test);
				size+=strlen(test)+1;
				test=test+strlen(test)+1;	

				printf("size=%d\n",size);
	}
	printf("------------------------------------------------------%s---\n\n",str);
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
	/*every cycle make the smallest remain knode  to the current position
	compare one by one with the following kcodes, if this is bigger, then exchange their KV data
	*/
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
	exit(1);
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
	
	for(curr_node=active_table_old->key_head;curr_node->next!=NULL;){
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
	char *big_crossed_table=malloc( (crossed_num)* test_seg_bytes +1 );//+1 is for the case crossed_num=0;
	memset(big_crossed_table, 0, (crossed_num)* test_seg_bytes +1);
	char *crossed_tables[lev0_info[LEV0_NUM]];
	int kv_len;
	char *cross_advancer=big_crossed_table;
	
	int j=0;
	for(i=0;i<LEV0_NUM;i++){
			if(lev0_info[i]==1){//1 indicates a crossed table 
				crossed_tables[j]=lev0_tables[i];
				j++;
			}
	}
		
	printf("crossed_num=%d, j=%d\n",crossed_num,j);
	for(i=0;i<j;i++){
			//uint32_t copy_size=*(uint32_t *) (crossed_tables[i] + test_seg_bytes - TABLE_END_RESERVED + TABLE_END_ZERO);
			char *advancer=crossed_tables[i];
			uint32_t copy_size;
			while(*advancer!=0){//caculate size
				int kv_len=strlen(advancer)+1 + strlen(advancer+ strlen(advancer)+1 ) +1;
				copy_size+=kv_len;
				advancer+=kv_len;
				
			}
			memcpy(cross_advancer,crossed_tables[i], copy_size );
			cross_advancer+= copy_size;
	}
	print_table("big_crossed_table",big_crossed_table);
	
	char *sorted_table_advancer=sorted_active_table;
	cross_advancer=big_crossed_table;
	char *big_table_advancer=big_table;
	
	i=0;
	while(1){
	
			if(*sorted_table_advancer==0){
			//printf("*sorted_table_advancer==0\n");
				while(*cross_advancer!=0){
					kv_len=strlen(cross_advancer)+1 + strlen(cross_advancer+ strlen(cross_advancer)+1 ) +1;
					memcpy(big_table_advancer, cross_advancer,kv_len );
					cross_advancer += kv_len;
					big_table_advancer += kv_len;
				}
				break;
			}
			if(*cross_advancer==0){
				//printf("*cross_advancer==0\n");
				while(*sorted_table_advancer!=0){
					kv_len=strlen(sorted_table_advancer)+1 + strlen(sorted_table_advancer+ strlen(sorted_table_advancer)+1 ) +1;
					memcpy(big_table_advancer, sorted_table_advancer,kv_len );
					sorted_table_advancer += kv_len;
					big_table_advancer += kv_len;
				}
				break;
			}
			
			int res=strcmp(sorted_table_advancer,cross_advancer);
			if(res<=0){
				
				kv_len=strlen(sorted_table_advancer)+1 + strlen(sorted_table_advancer+ strlen(sorted_table_advancer)+1 ) +1;
				//printf("res<=0, res=%d, kv_len=%d\n",res, kv_len);
				memcpy(big_table_advancer, sorted_table_advancer,kv_len );
				sorted_table_advancer += kv_len;
				big_table_advancer += kv_len;
				if(res==0){
					//printf("res=0, res=%d, kv_len=%d\n",res, kv_len);
					cross_advancer += strlen(cross_advancer) +1;//key
					cross_advancer += strlen(cross_advancer) +1;//value
				}
			}
			else{
				i++;
				//if(i>22) break;
				kv_len=strlen(cross_advancer)+1 + strlen(cross_advancer+ strlen(cross_advancer)+1 ) +1;
				//printf("res>0, res=%d, kv_len=%d, cross_advancer=%s\n",res, kv_len,cross_advancer);
				memcpy(big_table_advancer, cross_advancer,kv_len );
				cross_advancer += kv_len;
				big_table_advancer += kv_len;
			}
			
			
	}
	
	
	free(big_crossed_table);

	return 1;

}












int split_big_table(char *big_table, int*lev0_info){
int i;
	char *big_table_advancer=big_table;
	int splitting_counter=0;
	int splitted_tables=0;
	
	char *manua_splitted_first_key[LEV0_NUM];
	manua_splitted_first_key[0]=big_table_advancer;
	char *manua_splitted_last_key[LEV0_NUM];
	char *start_pointer=big_table_advancer;//records the start point when splitting table
	char *p0,*p1;//for recording two 
printf("!!!!!split_big_table\n");	
	int base_entry=set_base_entry(lev0_info);
	while(1){
		if(*big_table_advancer==0){
				break;
		}
		p0=p1;//if would split, p0 points to the last key
		p1=big_table_advancer;//and p1 points to the first key of the next table
		
		int len_key= strlen(big_table_advancer)+1;
		big_table_advancer += len_key;//advances the key for the first time
		int len_value =strlen(big_table_advancer)+1;
		big_table_advancer += len_value; //advances the value 		
		
		if( splitting_counter + len_key + len_value + TABLE_END_RESERVED > test_seg_bytes){//splitting
			
			lev0_tables[base_entry+splitted_tables]=malloc(0);//for freeing
			free(lev0_tables[base_entry+splitted_tables]);
			lev0_tables[base_entry+splitted_tables]=(char *)malloc(test_seg_bytes);
			memset(lev0_tables[base_entry+splitted_tables],0 , test_seg_bytes);
		
			memcpy( lev0_tables[base_entry+splitted_tables], start_pointer, p1-start_pointer);
		
			manua_splitted_last_key[splitted_tables]=p0;
			
			splitted_tables++;
			splitting_counter=0;				
			manua_splitted_first_key[splitted_tables]=p1;
			start_pointer=p1;//resetg the start pointer
				
		}
		splitting_counter += len_key+len_value;
	}
	
	
printf("splitted_tables-1 =%d, lev0_info[LEV0_NUM]=%d, base_entry=%d\n",splitted_tables,lev0_info[LEV0_NUM], base_entry);
	if(base_entry+splitted_tables<5){
		if(splitted_tables+1> lev0_info[LEV0_NUM]){
				for(i=LEV0_NUM-1;i>base_entry+splitted_tables;i--){
					lev0_tables[i]=lev0_tables[i-1];
				}
		
		}
		
		lev0_tables[base_entry+splitted_tables]=(char *)malloc(test_seg_bytes);
	}
	
	manua_splitted_last_key[splitted_tables]=p1;
	memset(lev0_tables[base_entry+splitted_tables],0 , test_seg_bytes);
	memcpy( lev0_tables[base_entry+splitted_tables], start_pointer, big_table_advancer-start_pointer);
	splitted_tables++;
	
	print_table("big_table",big_table);
	
	//update levels_summary --begin
		if(splitted_tables> lev0_info[LEV0_NUM]){
			( *(int*) levels_summary)++;
			for(i=base_entry+1; i<LEV0_NUM; i++){
				memmove(table_finder_0+ FINDER_ENTRY_LENGTH*(i+1), table_finder_0+ FINDER_ENTRY_LENGTH*i, FINDER_ENTRY_LENGTH*(LEV0_NUM-i-1));
				
			}
			
		}
	//update levels_summary --end
	
	//update the entries --begin
		for(i=0;i<splitted_tables;i++){
			memcpy(table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+i), manua_splitted_first_key[i], strlen(manua_splitted_first_key[i]) );
			memcpy(table_finder_0+ FINDER_ENTRY_LENGTH*(base_entry+i)+FINDER_KEY_LENGTH, manua_splitted_last_key[i], strlen(manua_splitted_last_key[i]) );
		}
	//update the entries --end

		free(big_table);

}