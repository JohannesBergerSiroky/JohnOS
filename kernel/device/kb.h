#ifndef KB_H
#define KB_H

volatile uint8_t kb_i;
volatile uint8_t reading;


uint32_t fs_a;
uint32_t fs_b;
uint32_t fs_c;
int8_t* kb_temp_dir;


uint32_t kb_temp1;
uint32_t kb_temp2;
uint8_t fs_string_match;
uint8_t check_root;

uint32_t mem_addr_result;
uint8_t input2;
uint8_t input3;
uint32_t mem_addr_offset;
uint32_t result;
uint32_t * result_addr;

uint8_t inputsec;
uint8_t temp100;
uint8_t temphead;

uint8_t temptrack;
uint8_t tempsector;
uint8_t counter;


uint8_t errorcheck;
void init_floppy_check_buffer();
void keyboard_handler();


void keyboard_handler2();
void keyboard_handler3();
void keyboard_install();

void set_kbvariables_zero();


uint32_t input;

int8_t* buffstr;
int8_t* buffstr2;
int8_t buffstr3[11];


int8_t buffstr4[127];
int8_t buffstr5[11];


#endif
