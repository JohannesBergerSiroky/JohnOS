#ifndef TYPES_H
#define TYPES_H


typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned long uint32_t;
typedef long int32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;



#define low_16(adress) (uint16_t)((adress) & 0xffff)
#define high_16(adress) (uint16_t)(((adress) >> 16) & 0xffff)


#endif 
