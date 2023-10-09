#ifndef UTIL_H
#define UTIL_H


uint32_t power_of(uint32_t arg1, uint32_t arg2);


uint16_t test_sort_array[32];
uint16_t test_array[32];
uint32_t test_array_two_pages[256];
uint32_t* bm_array_ptr[2];


uint16_t* sort_array2(uint32_t* input_array, int sort_size);
uint16_t* sort_array3(uint16_t* input_array, int sort_size);


uint32_t bm_temp_holder;
uint32_t* bm_input_array2;


uint32_t** shift_bitmap_right(uint32_t* input_array, uint32_t base,
                              uint32_t position, uint32_t num);
uint32_t** shift_bitmap_left(uint32_t* input_array, uint32_t base,
                             uint32_t position, uint32_t num);
void insert_bitmap(uint32_t* input_array, uint32_t base,
                   uint32_t position, uint32_t size, uint32_t free);


#endif
