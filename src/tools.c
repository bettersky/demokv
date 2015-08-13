#include "std.h"
#include "tools.h"




void print_table(char* str,char *table){
	char *test=table;
	int i=0;
	int size=0;
	printf("\n---%s------------------------------------------------------\n",str);
	while(1){//print test	
			if(test==NULL){
				printf("NULL\n");
				break;
			}
			if(*test==0) break;
				printf("%03d: key=%s, ", ++i, test);
				size+=strlen(test)+1;
				test=test+strlen(test)+1;
				
				printf("value=%s,                        ", test);
				size+=strlen(test)+1;
				test=test+strlen(test)+1;	

				printf("size=%d\n",size);
	}
	printf("------------------------------------------------------%s---\n\n",str);
}

void print_atable(char *str, struct ATABLE *table){
	printf("\n---%s------------------------------------------------------\n",str);
	struct KNODE *pk=table->key_head;
	int i;
	for(i=0;pk!=NULL;pk=pk->next){//print test					s
				printf("%03d: key=%s, ", ++i, pk->key);								
				printf("value=%s,                        ", pk->value);	
				printf("\n");
	}
	printf("------------------------------------------------------%s---\n\n",str);

}




uint64_t simple_pow(int base , int exp){
	int i;
	
	uint64_t res=1;
	for(i=0;i<exp;i++){
		res*=base;
		
	}
	
	return res;

}



void print_bit_map(char *str, char *bit_map, int size){
	printf("\n---%s------------------------------------------------------\n",str);
	
	int i;
	for(i=0;i<size;i++){//print test					s
				printf("%d ",bit_map[i]);
				
	}
	printf("\n");
	printf("------------------------------------------------------%s---\n\n",str);
	
}