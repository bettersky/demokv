#include "merge.h"
#include "flash.h"





int fill_sorted_active_table(char *sorted_active_table,char **tip_first_key, char **tip_last_key){
	printf("--------fill_sorted_active_table begin\n");

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
		*tip_first_key=active_first_key;
		*tip_last_key=active_last_key;
	for(curr_node=active_table_old->key_head;curr_node!=NULL;){
		free(curr_node->key);
		free(curr_node->value);
		struct KNODE *temp=curr_node;
		curr_node=curr_node->next;
		free(temp);
			//free big table
	}
	free(active_table_old);
	
	printf("--------fill_sorted_active_table end\n");
	
}




int give_crossed_chain(int lev, char *tip_first_key, char *tip_last_key, struct FINDER_ENTRY *crossed_entry_chain, struct FINDER_ENTRY **insert_point){
	printf("--------give_crossed_chain begin\n");

int i=0;
printf("give_crossed_chain begin, lev=%d\n",lev);
	
	struct FINDER_ENTRY *finder=first_tables_entry[lev]->next;//point to the header
	*insert_point=first_tables_entry[lev];//point to the header node
	//printf("give_crossed_serials, finder=%p,lev=%d\n",finder,lev);
	int total_crossed=0;
	int has_cross=0;
	printf("in give_crossed_chain, tip_first_key=%s, tip_last_key=%s\n", tip_first_key,tip_last_key);
	if(finder!=NULL )printf("in give_crossed_chain, finder->first_key=%s,  finder->last_key=%s\n",  finder->first_key, finder->last_key);
	while(finder!=NULL){
//printf("xxxxxxxxxxxx,tip_last_key=%s ,finder->first_key=%s\n",tip_last_key,finder->first_key);
		if(strcmp(tip_last_key, finder->first_key)<0){//no following tables
			if(total_crossed==0){
				*insert_point=finder->pre;//this node has pre
				return 0;
			}
			return total_crossed;
		}
		else if(strcmp(tip_first_key, finder->last_key)>0){//no cross, but followings may be crossing
			if(has_cross){
				printf("error in give_cross, exit\n");
				exit(1);
			}
			*insert_point=finder;
		}
//printf("zzzzzzzzzzz\n");
		else{
			//now that comes here, this table must be crossing
			if(total_crossed==0){
				*insert_point=finder->pre;//
				crossed_entry_chain->next=finder;
			}
			total_crossed++;
			has_cross=1;
			
		}
		finder=finder->next;
		i++;
	
	}

	printf("--------give_crossed_chain end\n");

	return total_crossed;
}



int fill_big_table(char *big_table, char *tip_table, struct FINDER_ENTRY *crossed_entry_chain, int crossed_num){
	printf("--------fill_big_table1 begin\n");

	int i;
	//char *sorted_active_table=tip_table;
	//printf("in fill_big_table1,crossed_num=%d, tip_table=%p, big_table=%p \n",crossed_num,tip_table,big_table);
	if(crossed_num<=0){//directly trans
	//printf("dddddddddddd\n");
		memcpy(big_table, tip_table, test_seg_bytes);
		return 1;
	}
	char *union_crossed_tables=malloc( (crossed_num)* test_seg_bytes +1 );//+1 is for the case crossed_num=0; but 
	memset(union_crossed_tables, 0, (crossed_num)* test_seg_bytes +1);
	
	char *union_advancer=union_crossed_tables;//anvances the cross union
	printf("in fill_big_table1,crossed_entry_chain=%p \n",crossed_entry_chain);
	
	for(i=0;i<crossed_num;i++){
	//for(;crossed_entry_chain!=NULL;crossed_entry_chain=crossed_entry_chain->next){//copy the crossed tables to union_crossed_tables
			//uint32_t copy_size=*(uint32_t *) (crossed_tables[i] + test_seg_bytes - TABLE_END_RESERVED + TABLE_END_ZERO);
			crossed_entry_chain=crossed_entry_chain->next;//first tiem skip the header, point to real entry
			char *advancer=crossed_entry_chain->table;//crossed_tables[i];//every cycle this points to a next crossed table
			uint32_t copy_size=0;
			//printf("debug1, i=%d, advancer=%p\n",i,advancer);
			
			while(*advancer!=0){//caculate the i-th table size
				int kv_len=strlen(advancer)+1 + strlen(advancer+ strlen(advancer)+1 ) +1;
				copy_size+=kv_len;
				advancer+=kv_len;
				
			}
			memcpy(union_advancer,crossed_entry_chain->table, copy_size );//copy the i-th crossed table
			union_advancer+= copy_size;
	}
	//print_table("union_crossed_tables in merging",union_crossed_tables);
	
	char *tip_table_advancer=tip_table;
	union_advancer=union_crossed_tables;
	char *big_table_advancer=big_table;
	
	i=0;
	while(1){
	
			if(*tip_table_advancer==0){
			//printf("*tip_table_advancer==0\n");
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
				while(*tip_table_advancer!=0){
					int kv_len=strlen(tip_table_advancer)+1 + strlen(tip_table_advancer+ strlen(tip_table_advancer)+1 ) +1;
					memcpy(big_table_advancer, tip_table_advancer,kv_len );
					tip_table_advancer += kv_len;
					big_table_advancer += kv_len;
				}
				break;
			}
			
			int res=strcmp(tip_table_advancer,union_advancer);
			if(res<=0){
				
				int kv_len=strlen(tip_table_advancer)+1 + strlen(tip_table_advancer+ strlen(tip_table_advancer)+1 ) +1;
				//printf("res<=0, res=%d, kv_len=%d\n",res, kv_len);
				memcpy(big_table_advancer, tip_table_advancer,kv_len );
				tip_table_advancer += kv_len;
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
	
	//print_table("in fill big table, big table", big_table);
	
	//if(merge2_num==2) exit(1);
	printf("--------fill_big_table1 end\n");

	return 1;

}





int split_big_table(char * big_table, int crossed_num, struct FINDER_ENTRY *insert_point, int lev){
	printf("--------split_big_table1 begin\n");

int i;

	//print_table("in splitting, big table", big_table);
	
	//if(merge2_num==2) exit(1);
printf("in split_big_table1, 1111111111 lev=%d\n",lev);	
//exit(1);
	char *big_table_advancer=big_table;
	int splitting_counter=0;
	char *splitted_tables_pointer[crossed_num + 2]; //crossed_num can be -1, but an array's size should be at lesast 1
	int splitted_tables_num=0;
	
	char *manua_splitted_first_key[crossed_num+2];
	manua_splitted_first_key[0]=big_table_advancer;
	char *manua_splitted_last_key[crossed_num+2];
	char *start_pointer=big_table_advancer;//records the start point when splitting table
	//printf("0000 start_pointer=%p \n",start_pointer);
	char *p0,*p1;//for recording two
	//printf("1111 start_pointer=%p \n",start_pointer);
	//print_table("in splitted, big table",big_table);
	int flag=0;
	while(1){//every cycle advance a KV
		if(*big_table_advancer==0){//advance to the end, finish
				break;
		}
		p0=p1;//if would split, p0 points to the last key
		p1=big_table_advancer;//if would split, p1 points to the first key of the next table
		//printf("in while, p1=%s\n",p1);
		int len_key= strlen(big_table_advancer)+1;
		big_table_advancer += len_key;//advances the key
		int len_value =strlen(big_table_advancer)+1;
		big_table_advancer += len_value; //advances the value 		
		if(flag==1){
			//printf("splitted_tables_num=%d\n",splitted_tables_num);
			//printf("in splitting,manua_splitted_last_key[0]=%s\n",manua_splitted_last_key[0]);
		}
		if( splitting_counter + len_key + len_value + TABLE_END_RESERVED > test_seg_bytes){//splitting
			
			//printf("in splitting,splitted_tables_num=%d , p0=%s\n",splitted_tables_num,p0);
			splitted_tables_pointer[splitted_tables_num]=(char *)malloc(test_seg_bytes);
			memset(splitted_tables_pointer[splitted_tables_num],0 , test_seg_bytes);			
			memcpy(splitted_tables_pointer[splitted_tables_num], start_pointer, p1-start_pointer);
			
			
			manua_splitted_last_key[splitted_tables_num]=p0;
			//printf("in splitting,splitted_tables_num=%d , p0=%s, manua_splitted_last_key[%d]=%s \n",splitted_tables_num,p0,splitted_tables_num, manua_splitted_last_key[splitted_tables_num]);
			splitted_tables_num++;
			splitting_counter=0;				
			manua_splitted_first_key[splitted_tables_num]=p1;
			start_pointer=p1;//resetg the start pointer
			flag=1;
				
		}
		splitting_counter += len_key+len_value;
	}
	
	for(i=0;i<splitted_tables_num;i++){
		//printf("splitted_table:%d, first_key=%s, last_key=%s\n",i,manua_splitted_first_key[i],manua_splitted_last_key[i]);
		//print_table("splitted_table",splitted_tables_pointer[i]);
	}
	//but the last splitted tables has not been stored. we do this int the next
	//printf("4444 start_pointer=%p \n",start_pointer);//this will couse it to wrong address
	//printf("start_pointer=%s, p1=%s,test_seg_bytes=%d\n",start_pointer,p1,test_seg_bytes);
	
	manua_splitted_last_key[splitted_tables_num]=p1;
	splitted_tables_pointer[splitted_tables_num]=(char *)malloc(test_seg_bytes);
	memset(splitted_tables_pointer[splitted_tables_num], 0 , test_seg_bytes);
	//printf("p1=%p \n",p1);
	//printf("splitted_tables_pointer[splitted_tables_num]=%p, start_pointer=%p, big_table=%p\n",splitted_tables_pointer[splitted_tables_num],start_pointer,big_table);
	memcpy(splitted_tables_pointer[splitted_tables_num], start_pointer, big_table_advancer-start_pointer);
	
	//print_table("splitted_tables_pointer[0]",splitted_tables_pointer[0]);
	splitted_tables_num++;//make it the ture num of splitted tables
	//splitting finished. splitted tables are store int splitted_tables_pointer[]. there are splitted_tables_num splitted tables
	
	
	printf("crossed_num=%d,splitted_tables_num=%d, big_table_advancer-start_pointer=%d\n",crossed_num,splitted_tables_num,big_table_advancer-start_pointer);
	for(i=0;i<splitted_tables_num;i++){
		//printf("splitted_table:%d, first_key=%s, last_key=%s\n",i,manua_splitted_first_key[i],manua_splitted_last_key[i]);
		//print_table("splitted_table",splitted_tables_pointer[i]);
	}
	
	
	//garbage reclaim --begin
	
	//printf("in split1, garbage reclaim begin, \n");
	if(insert_point!=NULL){//discard crossed tables in the entry links

		struct FINDER_ENTRY *cross_tables_advancer;
		cross_tables_advancer=insert_point->next;
		struct FINDER_ENTRY *crossed_mark=cross_tables_advancer;//mark for free
		for(i=0;i<crossed_num;i++){	
			cross_tables_advancer=cross_tables_advancer->next;//advance to end
		}
		insert_point->next=cross_tables_advancer;//discard the crossed entry node
		if(cross_tables_advancer!=NULL) cross_tables_advancer->pre=insert_point;
		
		for(i=0;i<crossed_num;i++){		//free crossed entries and the respond segments
				struct FINDER_ENTRY *temp=crossed_mark;
				discard_seg(temp->serial_num);
				clear_bit_map(lev, temp->serial_num);
				crossed_mark=crossed_mark->next;
				free(temp);
				//free(lev0_tables[base_entry+i]);
				//lev0_tables[base_entry+i]=NULL;
		}
	}
	printf("in split1, garbage reclaim end\n");
	//garbage reclaim --end
	//printf("crossed_num",crossed_num,);
	//allocate new serials --begin
	uint64_t *new_serials; //lev0 will not need this 
	new_serials=allocate_serial(lev,splitted_tables_num);
	for(i=0;i<splitted_tables_num;i++){
		printf("new_serials %d: %d\n",i+1, new_serials[i]);
	}
	printf("insert_point=%p, first_tables_entry[1]=%p,next=%p\n", insert_point,first_tables_entry[1],first_tables_entry[1]->next);
	//assert
	//allocate new serials --end
	
	//new tables written --begin
	
	//new tables written --end
//lev0 operations --begin
//give the splitted_tables_pointer to the *table in entry

//lev0 operations --end
	//update finder entry list --begin
		//case 1: no crossing and insert point is null. crossing is 0
	
		//case 2: no crossing and insert point is not null. crossing is -1
		
		//case 3: crossing is positive 

		struct FINDER_ENTRY *new_entry;
		for(i=0;i<splitted_tables_num;i++){
			new_entry=(struct FINDER_ENTRY *)malloc(sizeof(struct FINDER_ENTRY ) );
			memset(new_entry,0, sizeof(struct FINDER_ENTRY ));
				//printf("splitted %d: first:%s  last:%s\n",i, manua_splitted_first_key[i],manua_splitted_last_key[i] );
			memcpy(new_entry->first_key, manua_splitted_first_key[i], strlen(manua_splitted_first_key[i]) );// make sure manua_splitted_first_key
										//is zeroed when created
			memcpy(new_entry->last_key, manua_splitted_last_key[i], strlen(manua_splitted_last_key[i]) );
			new_entry->serial_num=new_serials[i];//not need in lev0
			if(lev==0){
				new_entry->table=splitted_tables_pointer[i];//special in lev0
			}
			else{
				//for(i=0;i<splitted_tables_num;i++){// lev0 will not need write
					//printf("xxxxx\n");
					write_seg(splitted_tables_pointer[i], new_serials[i] );	
					new_entry->table=read_seg(new_serials[i]);
					//printf("yyyyyyyyy\n");

				//}
			}
					//the space will be freed 
			new_entry->next=insert_point->next;
			insert_point->next=new_entry;
			new_entry->pre=insert_point;
			if(new_entry->next!=NULL){//insert to the tail
				new_entry->next->pre=new_entry;
			}
			else{
				tip_tables_entry[lev]=new_entry;
			}
			insert_point=new_entry;//advance the insert point
	
		}
		
//printf("22222222222222222222\n");
	//update finder entry list --end
	
	//other updates --begin
	int inc=0;
	if(crossed_num==0){
		inc=splitted_tables_num;
	}
	else{
		inc =splitted_tables_num-crossed_num;
	}
//printf("55555555555, levels_summary=%p, inc=%d, lev=%d \n",levels_summary,inc, lev);

	( *(int*) ( levels_summary+ (lev)*LEVELS_SUMMARY_ENTRY) )+= inc;
	
	printf("--------split_big_table1 end\n");


	//other updates --end
	
	//free
	free(new_serials);
	new_serials=NULL;
}



int give_tip_table(char **tip_table, int full_lev,char **tip_first_key, char **tip_last_key){
		printf("--------give_tip_table begin\n");

	
	//print_table("in give_tip_table", *tip_table);
	
	if(full_lev==0){
		//*tip_table=tip_tables_entry[0]->table;
		//*tip_first_key=tip_tables_entry[0]->first_key;//lev0_tables[LEV0_NUM-1];
		//*tip_last_key=tip_tables_entry[0]->last_key;//table_finder_0+ FINDER_ENTRY_LENGTH*(LEV0_NUM-1) + FINDER_KEY_LENGTH;
	}
	else{
	//now the lev full_lev must be full and the lev number is at least 1, we use tip_tables_entry[i] to get the table
	//printf("tip_tables_entry[full_lev]->serial_num=%d\n",tip_tables_entry[full_lev]->serial_num);
		//*tip_table=read_seg(tip_tables_entry[full_lev]->serial_num);
	}
	*tip_table=tip_tables_entry[full_lev]->table;
	*tip_first_key=tip_tables_entry[full_lev]->first_key;
	*tip_last_key=tip_tables_entry[full_lev]->last_key;
	//printf("in give_tip_table, *tip_table=%p\n",*tip_table);
	//print_table("in giv_tip_table, tip_table",*tip_table);
			printf("--------give_tip_table end\n");

	return 1;
}



int chop_tip_entry(int full_lev){
	printf("--------chop_lev begin\n");

	printf("I am chop_lev, full_lev=%d\n",full_lev);

		struct FINDER_ENTRY *temp=tip_tables_entry[full_lev];
		tip_tables_entry[full_lev]=tip_tables_entry[full_lev]->pre;
		
		discard_seg(temp->serial_num);//for low lev this will free the virtual segment and physical segment
		clear_bit_map(full_lev,temp->serial_num);
		//update the finder entry
		temp->pre->next=NULL;
		//free tip finder entry
		free(temp);
		temp=NULL;	

	( *(int*) ( levels_summary+ full_lev*LEVELS_SUMMARY_ENTRY) )--;
	
	printf("--------chop_lev end\n");

}


