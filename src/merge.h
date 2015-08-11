#ifndef MERGE
#define MERGE

#include "std.h"
#include "segtable.h"
extern struct ATABLE *active_table;

extern char *levels_summary;


extern struct FINDER_ENTRY *tip_tables_entry[MAX_LEV]; 
extern struct FINDER_ENTRY *first_tables_entry[MAX_LEV];

extern uint64_t serials_width[];
extern char *seg_bit_maps[];

int fill_sorted_active_table(char *sorted_active_table,char **tip_first_key, char **tip_last_key);

int fill_big_table(char *big_table, char *tip_table, struct FINDER_ENTRY *crossed_entry_chain, int crossed_num);
int split_big_table(char * big_table, int crossed_num, struct FINDER_ENTRY *insert_point, int lev);

int give_tip_table(char **tip_table, int full_lev, char **tip_first_key, char **tip_last_key);

int give_crossed_chain(int lev, char *tip_first_key, char *tip_last_key, struct FINDER_ENTRY *crossed_entry_chain, struct FINDER_ENTRY **insert_point);
int chop_tip_entry(int full_lev);

#endif
