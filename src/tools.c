#include "std.h"
#include "tools.h"




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

void print_atable(char *str, struct ATABLE *table){
	printf("\n---%s------------------------------------------------------\n",str);
	struct KNODE *pk=table->key_head;
	int i;
	for(i=0;pk!=NULL;pk=pk->next){//print test					s
				printf("%d: key=%s, ", ++i, pk->key);								
				printf("value=%s,                        ", pk->value);	
				printf("\n");
	}
	printf("------------------------------------------------------%s---\n\n",str);

}
