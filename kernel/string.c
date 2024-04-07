/* This file includes some string manipulation functions. */ 

#include "types.h"
#include "string.h"
#include "graphics/screen.h"
#include "fs/fat16.h"

/* Calculates the lenght of a string. */
uint16_t strlength(int8_t* ch)
{
        uint16_t e = 1;
        while((ch[e++]) && (e < 128));  
        return --e;
}

/* Calculates the lenght of a string in other ways. */
int32_t stringlength(int8_t* teststring)
{
        int i = 0;

        while(teststring[i])
                i += 1;

        return i;
}

/* Calculates the lenght of a string, too. */
uint16_t strlength3(int8_t* ch)
{
        uint16_t e = 1;
        while((ch[e++] != '\0') && (e < 128));  
        return --e;
}


/* Checks if two strings are equal. Returns 1 or 0. */
uint8_t strEql(int8_t* ch1, int8_t* ch2)                 
{
        uint8_t result = 1;
        uint8_t size = strlength(ch1);
        uint8_t size2 = strlength(ch2);

        if(size != size2) {
                result = 0;
        }
        else {
                uint8_t f = 0;
                for(;f<=size;f++) {
                        if(ch1[f] != ch2[f]) {
                                result = 0;		
                                break;
                        }
                }
        }

        return result; 
}

/* This function takes a string value and truncates it 
 * so that it has a size of three bytes. Then it compares
 * it to another string.
 */
uint8_t strEql2(int8_t* ch1,int8_t* ch2)                   
{
        uint8_t result = 1;
        uint8_t size = 3;
        uint8_t f = 0;

        for(;f < size;f++) {
                if(ch1[f] != ch2[f]) 
                        result = 0;		
        }

        return result; 
}

/* Makes small letters to big letters and compares that
 * string to a file in the FAT16 file system.
 */
uint8_t strEql3(int8_t* ch1,int8_t* ch2)                 
{
        int8_t temp3[12] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
        uint32_t strcmp_count = 0;
        uint8_t result;
        result = 1;
        uint16_t size = strlength(ch1);
        uint16_t size2 = strlength(ch2);

        /* Makes sure the strings have the same length. */
        if(size != size2) {
                result = 0;
                return result;
		}

        uint8_t f = 0;
		print("\n");

   		for(;f<=size;f++) {
                int8_t temp1;
                int8_t temp2;

                if((*(ch1 + f)) != '\0') {
                        temp1 = (int8_t)((*(ch1 + f))- 0x20); 
                        temp3[f] = temp1;

                }
                else {
                        temp1 = (int8_t)(*(ch1 + f));
                        temp3[f] = '\0';
                }

                temp2 = (int8_t)(*(ch2 + f));

                if(temp1 != temp2) {

                        result = 0;	
                }	
        }

		if (result == 1) {
                if(size < 8) {
                        for(strcmp_count = 0; strcmp_count <= size; strcmp_count++)
                                get_file[strcmp_count] = temp3[strcmp_count];
                }

                get_file[strcmp_count] = '\0';

                if (size > 11) {
                        print("\nError during string comparison.");
                        return 0;
                }
                else {
                        if(size < 8) {
                                for(;size < 8; size++)
                                        get_file[size] = (int8_t)0x20;
                        }
                        
                        get_file[8] = 'T';
                        get_file[9] = 'X';
                        get_file[10] = 'T';
                        get_file[11] = '\0';                        
                }			
        } 

        return result;
}

