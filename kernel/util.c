/* This file contains some utility functions. */

#include "types.h"
#include "util.h"


/* This is this kernel's power of function.
 * arg1 could be 10, arg2 could be 2: 10^2 
 */
uint32_t power_of(uint32_t arg1, uint32_t arg2) 
{
	    uint32_t result = 1;

	    for (uint32_t counter = 0; counter < arg2; counter++)
		        result *= arg1; 


	    return result;
}

/* This sorts an array. Sort size corresponds to the amount of numbers
 * to be sorted.
 */
uint16_t* sort_array2(uint32_t* input_array, int sort_size)
{
      
        uint16_t* temp_array_ptr = test_array;
        int shift_var = 0;
        int sort_flag = 0;
        for(int i = 0; i < sort_size;i++) {
                *(temp_array_ptr + i) = 0;
        }
        *temp_array_ptr = input_array[0];


        for(int i = 1; i < sort_size;i++) {

                if(*(temp_array_ptr + i - 1) < input_array[i]) {
                        *(temp_array_ptr + i) = input_array[i]; 

                }

                else {
 
                        for(int j = (i-2); (j >= 0) && (sort_flag == 0);j--) {
 
                                if(*(temp_array_ptr + j) < input_array[i]) {
                                            sort_flag = 1;


                                           for(int k = i; k >= (j+2); k--) 
                                                *(temp_array_ptr + k) = *(temp_array_ptr + k - 1); 
                                }
                                shift_var++;
                        } 

                        if(sort_flag == 0) {

                                for(int l = 0; l < i;l++) {
                                        *(temp_array_ptr + i - l) = *(temp_array_ptr + i -l - 1); 
                                }
                                *temp_array_ptr = input_array[i];
                        }

                        else {

                                if((i - shift_var) >= 0) {
                                    *(temp_array_ptr + i - shift_var) = input_array[i]; 

                                }

                        }
                }
                
                shift_var = 0;
                sort_flag = 0;
        }

        return temp_array_ptr;
}


/* Sorts an array */
uint16_t* sort_array3(uint16_t* input_array, int sort_size)
{
      
        uint16_t* temp_array_ptr = test_array; 
        int shift_var = 0; 
        int sort_flag = 0;
        for(int i = 0; i < (sort_size*2);i++) {
                *(temp_array_ptr + i) = 0;
        }
        *temp_array_ptr = input_array[0]; 
        *(temp_array_ptr + 1) = input_array[1];

        for(int i = 1; i < sort_size;i++) {

                if(*(temp_array_ptr + (i*2) - 2) < input_array[i*2]) {
                        *(temp_array_ptr + (i*2)) = input_array[(i*2)]; 
                        *(temp_array_ptr + (i*2) + 1) = input_array[(i*2) + 1];
                }

                else {

                        for(int j = (i-2); (j >= 0) && (sort_flag == 0);j--) {
 
                                if(*(temp_array_ptr + (j*2)) < input_array[i*2]) {
                                            sort_flag = 1;

                                           for(int k = i; k >= (j+2); k--) {
                                                    *(temp_array_ptr + (k*2)) = *(temp_array_ptr + (k*2) - 2); 
                                                    *(temp_array_ptr + (k*2) + 1) = *(temp_array_ptr + (k*2) - 1); 
                                            }
                                }
                                shift_var++;
                        } 

                        if(sort_flag == 0) {

                                for(int l = 0; l < i;l++) {
                                        *(temp_array_ptr + (i*2) - (l*2)) = *(temp_array_ptr + (i*2) -(l*2) - 2); 
                                        *(temp_array_ptr + (i*2) - (l*2) + 1) = *(temp_array_ptr + (i*2) -(l*2) - 1); 
                                }
                                *temp_array_ptr = input_array[(i*2)];
                                *(temp_array_ptr+1) = input_array[(i*2)+1];
                        }

                        else {

                                if((i - shift_var) >= 0) {
                                    *(temp_array_ptr + (i*2) - (shift_var*2)) = input_array[i*2]; 
                                    *(temp_array_ptr + (i*2) - (shift_var*2) + 1) = input_array[(i*2) + 1]; 
                                }

                        }
                }
                
                        shift_var = 0;
                        sort_flag = 0;
        }

        return temp_array_ptr;
}


/* This is a function that is currently not in use in the kernel.
 * It shifts bits to the right. Num is the number of bits to shift
 * from the base. For instance, the base can be 5 bits from the start.
 */
uint32_t** shift_bitmap_right(uint32_t* input_array, uint32_t base, uint32_t position, uint32_t num)
{

        uint32_t temp = 4095;
        uint32_t temp2;
        bm_temp_holder = 0;
        bm_input_array2 = &bm_temp_holder;
        bm_array_ptr[0] = input_array;
        bm_array_ptr[1] = bm_input_array2;

        uint32_t** bm_array_ptr2 = bm_array_ptr;
        uint32_t of_flag = 0;
        uint32_t temp4;
        if((position + num) > 4095) {
                of_flag = position + num - 4096;
                
        }

        if(of_flag > 0) {
                temp4 = of_flag;
                base += 4096;

                for(uint32_t i = 0; i < temp4; i++) {
                        temp2 = (((*(input_array + ((uint32_t)(base + i)/32))) >> ((base + i) % 32)) & 0x00000001);
                        if(temp2) {
                                (*(bm_input_array2 + ((uint32_t)(i)/32))) |= (1 << ((i) % 32));
                        }

                        else {
                                (*(bm_input_array2 + ((uint32_t)(i)/32))) &= (~(1 << ((i) % 32)));


                        }
                }
                base -= 4096;
                bm_temp_holder |= 0x80000000;
        }

        while(temp >= position) {

                temp2 = (((*(input_array + ((uint32_t)(base + temp - num)/32))) >> ((base + temp - num) % 32)) & 0x00000001);

                if(temp2) {
                        (*(input_array + ((uint32_t)(base + temp)/32))) |= (1 << ((base + temp) % 32));
                }

                else {
                        (*(input_array + ((uint32_t)(base + temp)/32))) &= (~(1 << ((base + temp) % 32)));

                }

                temp--;

        }

        return bm_array_ptr2;

}

/* Shifts bits to the right. Num is the number of bits to shift
 * from the base. For instance, the base can be 5 bits from the start.
 */
uint32_t** shift_bitmap_left(uint32_t* input_array, uint32_t base, uint32_t position, uint32_t num)
{

        uint32_t temp = position;
        uint32_t temp2;
        bm_temp_holder = 0;

        bm_input_array2 = &bm_temp_holder;
        bm_array_ptr[0] = input_array;
        bm_array_ptr[1] = bm_input_array2;

        uint32_t** bm_array_ptr2 = bm_array_ptr;
        uint32_t of_flag = 0;
        uint32_t temp4 = 0;
        
        if(position + num > 4095) {
                of_flag = position + num - 4096;
                
        }

        if(of_flag > 0) {
                temp4 = of_flag;
                base += 4096;

                for(uint32_t i = 0; i < temp4; i++) {
                        temp2 = (((*(input_array + ((uint32_t)(base + i)/32))) >> ((base + i) % 32)) & 0x00000001);
                        if(temp2) {
                                (*(bm_input_array2 + ((uint32_t)(i)/32))) |= (1 << ((i) % 32));
                        }
                        else {
                                (*(bm_input_array2 + ((uint32_t)(i)/32))) &= (~(1 << ((i) % 32)));

                        }
                }
                base -= 4096;
                bm_temp_holder |= 0x80000000;
        }

        while(temp < 4096) {

                temp2 = (((*(input_array + ((uint32_t)(base + temp + num)/32))) >> ((base + temp + num) % 32)) & 0x00000001);

                if(temp2) {
                        (*(input_array + ((uint32_t)(base + temp)/32))) |= (1 << ((base + temp) % 32));
                }
                else {
                        (*(input_array + ((uint32_t)(base + temp)/32))) &= (~(1 << ((base + temp) % 32)));

                }


                temp++;

        }

        return bm_array_ptr2;

}


/* This function inserts some bits into a bitmap */
void insert_bitmap(uint32_t* input_array, uint32_t base, uint32_t position, uint32_t size, uint32_t free)
{
        uint32_t temp = position;
        uint32_t temp2;

        if(!free) {

                if(size == 4) {

                        (*(input_array + (uint32_t)((base + temp) / 32))) &= (~(1 << ((base + temp) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 1) / 32))) |= (1 << ((base + temp + 1) % 32));
                        (*(input_array + (uint32_t)((base + temp + 2) / 32))) &= (~(1 << ((base + temp + 2) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 3) / 32))) |= (1 << ((base + temp + 3) % 32)); 

                }

                else if (size > 4) {

                        (*(input_array + (uint32_t)((base + temp) / 32))) &= (~(1 << ((base + temp) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 1) / 32))) |= (1 << ((base + temp + 1) % 32));
                        (*(input_array + (uint32_t)((base + temp + 2) / 32))) |= (1 << ((base + temp + 2) % 32));
                        (*(input_array + (uint32_t)((base + temp + 3) / 32))) &= (~(1 << ((base + temp + 3) % 32))); 


                        temp +=4;

                        temp2 = (uint32_t)(size / 4);

                        while(temp2 > 0) {
                               (*(input_array + (uint32_t)((base + temp) / 32))) |= (1 << ((base + temp) % 32)); 

                                temp++;
                                temp2--;


                        }

                }

        }


        else {

                if(size == 4) {

                        (*(input_array + (uint32_t)((base + temp) / 32))) &= (~(1 << ((base + temp) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 1) / 32))) &= (~(1 << ((base + temp + 1) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 2) / 32))) |= (1 << ((base + temp + 2) % 32));
                        (*(input_array + (uint32_t)((base + temp + 3) / 32))) |= (1 << ((base + temp + 3) % 32)); 


                }

                else if (size > 4) {

                        (*(input_array + (uint32_t)((base + temp) / 32))) &= (~(1 << ((base + temp) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 1) / 32))) &= (~(1 << ((base + temp + 1) % 32)));
                        (*(input_array + (uint32_t)((base + temp + 2) / 32))) |= (1 << ((base + temp + 2) % 32));
                        (*(input_array + (uint32_t)((base + temp + 3) / 32))) &= (~(1 << ((base + temp + 3) % 32))); 


                        temp +=4;

                        temp2 = (uint32_t)(size / 4);

                        while(temp2 > 0) {

                               (*(input_array + (uint32_t)((base + temp) / 32))) |= (1 << ((base + temp) % 32)); 
                                temp++;
                                temp2--;


                        }
                }


        }

}

