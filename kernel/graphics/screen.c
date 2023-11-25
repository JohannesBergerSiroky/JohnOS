/* This file contains code which writes to video memory and accesses ports for the cursor */
#include "../types.h"
#include "screen.h"
#include "../system.h"
#include "../util.h"
#include "../string.h"

/* This is the string which is displayed before the cursor. In DOS we had C:\> for example.
 * At start, this string simply displays 'Root$' to indicate the current position in the
 * file system.
 */
 
int8_t Consolestring[128]  = {'~', 'R', 'o', 'o', 't', '$', ' ', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
         '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
         '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
         '\0', '\0', '\0', '\0'};

/* This function sets som variables. */

void init_screen_driver()
{
        sw = 80, sh = 25, sd = 2; 
        sc = 0x02; 
        poscheck = 0;


        pos = 0;
        hextemp2 = 0;
        hexbytetemp2 = 0;
        cursorX = 0, cursorY = 0;


        csptr = Consolestring;
}


/***********************************************************************************************************************************************
 *                                                                                                                                              *
 * sw stands for screen width which is 80 in this (ancient) video mode. It's a text mode. 80 times 25. sh = screen height (25 in this case).    *
 * sd is 2 because an address in the videomemory stands for the symbol, and the next address stands for the symbol's properties, like color for *
 * example. So the width of the screen is represented by 160, memory wise  (real width + properties, 80 plus 80). The value in 0xb8000 can      *
 * be 'A' for example, and in 0xb8001 it can be 0x02 (I think it stands for green color). These two addresses prints something to the upper     *
 * left corner of the screen. A green 'A' in this case.0xb8002 prints something to the right of the spot at the upper left corner of the screen.*
 * If we press enter, the cursor has been set to move down a line. To the address: 0xb8000+160d = 0xb80a0. And then the cursor moves            *
 * one cell to the right to make place for the symbol to eventually be printed at 0xb80a0.                                                      * 
 *                                                                                                                                              *
  **********************************************************************************************************************************************/


/* Zero the screenbuffers. */


void init_screenbuffers()
{
        for (int i = 0; i < 16000; i++) {
                screen_buffer[i] = 0;
                screen_buffer2[i] = 0;

        }
}

/* The three functions below will adjust some screen buffers. 
 * When the user scrolls, the buffer must be filled and/or removed 
 * and/or be printed out on the screen.
 */

void fill_Upscreen_buffer(uint32_t temppos)
{
        int8_t* vidmem = (int8_t*) 0xb8000;
        for (uint32_t a = 0; a < 160; a += 2)
                screen_buffer[160*(temppos - 24) + a]  = vidmem[a];
}

void adjust_Upscreen_buffer()
{
        int8_t* vidmem = (int8_t*) 0xb8000;
        
        for(uint32_t a = 1; a <= BUFLIMIT; a++) {
                for(uint32_t b = 0; b < 160; b +=2)
                        /* moves everything, in the buffer, up 1 step */
                        screen_buffer[160*(a - 1) + b]  = screen_buffer[160*a + b]; 
        }

        for(uint32_t b = 0; b < 160; b +=2)
                /* fill the empty space at the bottom, and done */
                screen_buffer[160*BUFLIMIT + b] = vidmem[b]; 
}



void fill_Dwnscreen_buffer()
{
        int8_t* vidmem = (int8_t*) 0xb8000;
        for (uint32_t a = 0; a < 160; a += 2)
                screen_buffer2[160*(pos - 24) + a]  = vidmem[22*160 +a];
}

/* this function clears a line */

void clearLine(uint8_t from, uint8_t to)
{
        /* row 2. 80*2 = 160 times 2 = 320.  to = 10 */ 
        uint16_t a = sw*from*sd;   
        int8_t* vidmem = (int8_t*) 0xb8000; 

        /* example: clear row 10: for a = 320, less than 1760. clears row 10 so that it gets to (to + 1). */
        for(;a<(sw*(to+1)*sd);a = a + 2) {
                vidmem[a] = 0x0;
                vidmem[a+1] = sc;
        }
}

/* Sets the cursor at another place. */

void updateCursor()
{
        uint16_t temp1;
        uint16_t temp2;
        uint8_t temp3;
        temp1 = cursorY*sw+cursorX;

        temp2 = temp1;
        temp3 = (uint8_t)temp2;


        /*outportb = write to port inportb = reads from a port into something (a variable for example)
         *as far as I know: 0x3D4 = access register. 0x3D5 = change or copy (inportb) from register 
         */ 

        /* access register number 15 = cursor location low (bits 0-7) register */
        outportb(0x3D4, 0x0F); 

        /* write the value of temp1 in there, for example: 1024 */
        outportb(0x3D5, temp3); 
        /* access register number 14: cursor location high (bits 8-15) register */
        outportb(0x3D4, 0x0E); 
        temp3 = (uint8_t)(temp1 >> 8);
        /* bits in temp1 shifted 8 bits to the right to get the remainder of bits to the high register (could be 00000000 if temp1 = 60 for example) */
        outportb(0x3D5, temp3); 
}

/* This function clears the whole screen from text. */

void clearScreen()
{
        clearLine(0, sh-1);
        cursorX = 0;
        cursorY = 0;
        poscheck = 0;
        pos = 0;
        updateCursor();
}

/*This function scrolls through text when the arrow up
 *key is pressed. 
 */

void scrollDown() 
{
        if (pos >= 24) {
                if (pos < ADJUSTEDBUFLIMIT)
                        fill_Dwnscreen_buffer();

                int8_t * vidmem = (int8_t*) 0xb8000;
                


                for(int i = 3838; i >= 0; i = i - 2) {
                        vidmem[i + 160] = vidmem[i];
                        vidmem[i+1] = sc;
                }
                	
                if (pos < ADJUSTEDBUFLIMIT) {
                        for (int i = 0; i < 160; i += 2) {
                                vidmem[i] = screen_buffer[160*(pos - 24) + i];
                                vidmem[i + 1] = sc;
                        }
                        for (int i = 0; i < 160; i++)
                                screen_buffer[160*(pos - 24) + i] = 0x0;
                }


                clearLine(23, 24);
                cursorX = 0;
                cursorY = 23;
                updateCursor();
                print(csptr);
                pos--;
        }

}
	
/*This function scrolls through text when the arrow down
 *key is pressed. 
 */

void scrollUp(uint8_t lineNumber) 
{
        if (screen_buffer2[0] != 0 || (pos + 1) >=24) {
                int8_t* vidmem = (int8_t*) 0xb8000;

                if (poscheck == 0 && pos < ADJUSTEDBUFLIMIT)
                        fill_Upscreen_buffer(pos + 1); 
                if (pos == ADJUSTEDBUFLIMIT)
                        adjust_Upscreen_buffer();

                /* moves up the symbols on the screen */
                uint16_t b = 0;
                for (;b < sw*(sh-1)*2;b++) {
                        vidmem[b] = vidmem[lineNumber*2*sw+b]; 
                }

                if((cursorY - lineNumber) < 0) {
                        cursorX = 0;
                        cursorY = 0;
                }

                if (poscheck == 0 && pos < ADJUSTEDBUFLIMIT) {
		    
                        int8_t* vidmem = (int8_t*) 0xb8000;


                        for (uint32_t a = 0; a < 160; a += 2) {
                                vidmem[22*160 +a] = screen_buffer2[160*(pos + 1 - 24) + a];
                                vidmem[22*160 +a + 1] = sc;
                        }

                        for (uint32_t a = 0; a < 160; a++)
                                screen_buffer2[160*(pos +1 - 24) + a] = 0x0;


                        clearLine(24, 24);
                        cursorX = 0;
                        cursorY = 23;
                        updateCursor();
                        print(csptr);
                        if (pos < ADJUSTEDBUFLIMIT)
                                pos++;
                }

                else if (poscheck == 0 && pos >= 24) {
                        clearLine(24, 24);
                        cursorX = 0;
                        cursorY = 23;
                        updateCursor();
                        print(csptr); 
	    
                }
    
                else if (poscheck == 1) {
                        clearLine(24, 24);
                        cursorX = 0;
                        cursorY = 23;
               }


        } 
        poscheck = 0;
}

/* check so that the text doesn't go out of screen */

void newLineCheck() 
{
        if(cursorY >= 24) {
                if ((pos + 1 - 24) < 77)
                        fill_Upscreen_buffer(pos + 1); 
                scrollUp(1); 
        }

        if (pos < ADJUSTEDBUFLIMIT)
        pos++;
}



/* This function prints a character. It is used in the print function to then print
 * several characters. 
 */

/* written before: change to uint8_t */

void printch(int8_t c, int32_t i) 
{

        int8_t* vidmem = (int8_t*) 0xb8000; 
        switch(c) {
                /* If the backspace key is pressed. */
                case (0x08): 
                        if((i + stringlength(csptr)) > stringlength(csptr)) {
                                cursorX--;
                                vidmem[(cursorY*sw+cursorX)*sd] = 0x0;

                        }
                break;
                /* If the tab character is pressed. */
                case (0x09): 
                        cursorX = (cursorX +8) & ~(8 - 1);
                break;
                /* If the enter key is pressed. */
                case ('\n'):
                        cursorX = 0;
                        cursorY++;


                        for(int temp = 0; temp < 160; temp +=2) {
                                vidmem[(cursorY)*160 + temp] = 0;
                                vidmem[(cursorY)*160 + temp + 1] = sc;
                        }

                        poscheck = 1;
                        newLineCheck();
                break;

                /* If not backspace, tab or the enter key is pressed this prints a 
                 *character and the properties of the character 
                 */
                default:
                vidmem[(cursorY*sw+cursorX)*sd] = c; 
                vidmem[(cursorY*sw+cursorX)*sd+1] = sc;


                cursorX++;
                break;
        }
        /* Now check of the cursor is out of bounds and if so, adjust it properly */
        if (cursorX >= sw) {
                cursorX = 0;
                cursorY++;
                poscheck = 1;
                newLineCheck();
        }

        else if (cursorX < 0 && (c != '\n')) {
                cursorX = sw - 1;
                cursorY--;
                pos--;
                vidmem[(cursorY*160 + cursorX*sd)] = 0x0;
        }

        else {
                vidmem[(cursorY*sw+cursorX)*sd+3] = sc;
		}
        updateCursor();

}

/* This function prints a whole string. This function is the one to use the most.
 * Written before and might not need to change: change to uint8_t
 */

void print (int8_t* ch) 
{
	    uint16_t d = 0;
	    
	    while (ch[d]) {
		        printch(ch[d], 0);
		        d++;	
	    }
}

/* This function will take an integer number and print that number on the screen.
 * First it must convert the number to the ASCII character value or values
 * that it should print.
 */

void printi (uint32_t value)
{
        int8_t chr[11] = "0123456789";
        int8_t chr2[10];
        uint8_t integ = 0;
        do {
                chr2[integ] = chr[value%10]; 
                integ++;
        } while ((value /= 10) > 0);
 
        for (;integ > 0; integ--)
                printch(chr2[integ - 1], 0);
}

/* The same concept as above. This function will take an 8 bit number and print 
 *that number on the screen. 
 */

void printbyteint8_t(int8_t value)
{
        int8_t chr[11] = "0123456789";
        int8_t chr2[3];
        uint8_t integ = 0;

        do {
                chr2[integ] = chr[value%10]; 
                integ++;
        } while ((value /= 10) > 0); 

        for (;integ > 0; integ--)
                printch(chr2[integ - 1], 0);
} 

/* The same concept as above. This function will take an unsigned 8 bit number and print 
 *that number on the screen. 
 */
void printbyte(uint8_t value)
{
        int8_t chr[11] = "0123456789";
        int8_t chr2[3];
        uint8_t integ = 0;

        do {
                chr2[integ] = chr[value%10]; 
                integ++;
        } while ((value /= 10) > 0); 

        for (;integ > 0; integ--)
                printch(chr2[integ - 1], 0);
}

/* The same concept as above. This function will take an unsigned 16 bit number and print 
 *that number on the screen. 
 */
void printword(uint16_t value)
{
        int8_t chr[11] = "0123456789";
        int8_t chr2[3];
        uint8_t integ = 0;

        do {
                chr2[integ] = chr[value%10]; 
                integ++;
        } while ((value /= 10) > 0);

        for (;integ > 0; integ--)
                printch(chr2[integ - 1], 0);
}

/* The same concept as above. This function will take an unsigned 32 bit number and print 
 *that number on the screen in hexadecimal notation. 
 */
void print_hex(uint32_t value)
{
        int8_t hexbuffer[17] = "0123456789ABCDEF\0";
        int8_t finished_buffer[9] = "00000000\0";

        print("0x");
        if (value == 0)
                print("00000000");

        uint32_t i_temp = 0;
        while (value >= 1) {
	            hextemp2 = value;
	            value /= 16;
	            finished_buffer[i_temp] = hexbuffer[(hextemp2 - (value*16))];
	            i_temp++;
        }

        for (;i_temp > 0; i_temp--)
        printch(finished_buffer[i_temp - 1], 0);
}

/* The same concept as above. This function will take an unsigned 8 bit number and print 
 *that number on the screen in hexadecimal notation. 
 */

void print_hex_byte(uint8_t value)
{
        /* the compiler always ignores everything after \0 */
		uint8_t hexbufferbyte[17] = "0123456789ABCDEF\0";  

        /* here is where our numbers are stored. A byte is ranged from 0x00-0xFF. 2 spots maximum. And the \0 at the end */
		uint8_t finished_buffer_byte[3] = "00\0"; 

        /* i_temp_byte is never bigger than 2. 2 is the number of spots in our int8_t array to store the numbers. */
		uint32_t i_temp_byte = 0; 

        /* stop when it gets too low */
		while (value >= 1) {
				hexbytetemp2 = value;
                /* value = value / 16. look up a decimal to hexadecimal formula where it divides by 16 per time */
				value /= 16; 

                /* original value minus the result of the division times 16 is actually the value */
				finished_buffer_byte[i_temp_byte] = (hexbufferbyte[(hexbytetemp2 - (value*16))]);
				i_temp_byte++;
		}
        /* have to print the stored values backwards. look at the formula. printch = print a int8_t. */
		for (i_temp_byte = 2;i_temp_byte > 0; i_temp_byte--)
                printch(finished_buffer_byte[i_temp_byte - 1], 0); 
}

/* This function will convert a hexadecimal number to a decimal number */

uint32_t hex_to_dec(uint32_t* value)
{
	    int c = 7;
	    uint32_t result = 0;
	    while(c >= 0) {
			    result +=(value[c]*power_of(16,c));
			    c--;
	    }

	    return (const uint32_t) result;
}

/* This function will convert a decimal number to a binary number
 * and print that binary number on the screen. 
 */

void dec_to_bin(uint32_t value)
{
		uint8_t temparray[34];
		temparray[33] = '0';
		uint32_t max = 2147483648;
		uint8_t a = 0;


        /* padding */
		while (value < max) {

	
				temparray[a] = '0'; 
				a++;
				max /= 2;
		};

		a = 31;


		/* formula: */	
		while (((uint32_t)value) > 0) {
				if ((value % 2) == 0)
					    temparray[a] = '0';

				else {
						temparray[a] = '1';
						value = value - 1;
				}
				value/= 2;
				a--;
		};
		temparray[32] = '\0';




		/* print it */
		a = 0;
		while(a < 32) {
				printch(temparray[a],0);
				a++;
				if ((a % 4) == 0)
					    print(" ");
		};
}
