#ifndef SCREEN_H
#define SCREEN_H


#define BUFLIMIT                        75
#define ADJUSTEDBUFLIMIT               100


int8_t screen_buffer[16000];
int8_t screen_buffer2[16000];



int8_t * csptr;

uint32_t poscheck;
uint32_t sc_temp_addr;
uint32_t pos;
int cursorX, cursorY;
uint8_t sw, sh, sd;


/* screencolor */
uint8_t sc; 
void init_screen_driver();
void init_screenbuffers();
void clearLine(uint8_t from, uint8_t to);

void updateCursor();
void clearScreen();


void scrollUp(uint8_t lineNumber);
void scrollDown();
void fill_Upscreen_buffer(uint32_t temppos);
void fill_Dwnscreen_buffer();


void adjust_Upscreen_buffer();
uint32_t hex_to_dec(uint32_t* value);
void dec_to_bin(uint32_t value);
void newLineCheck();


uint32_t hextemp2;
uint32_t hexbytetemp2;


void printch(int8_t c, int32_t i);
void printbytechar(int8_t value);
void printword(uint16_t value);
void print_hex(uint32_t value);


void print_hex_byte(uint8_t value);
void print (int8_t* ch);
void printi (uint32_t value);
void printbyte(uint8_t value);


#endif
