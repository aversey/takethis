#ifndef TAKETHIS_INCLUDED_MAP
#define TAKETHIS_INCLUDED_MAP


/* room is identified by single char
 * they are located in data/map/
 * roomfile has four neighbors at the first line
 * after this indexed tiles go
 * after them each line is speed of corresponding enemy in percents
 * and optionaly after = message of this enemy */


void tt_map_load();

void tt_map_free();


#endif
