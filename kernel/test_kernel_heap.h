#ifndef TEST_KERNEL_HEAP_H
#define TEST_KERNEL_HEAP_H

struct tkh
{
        uint32_t a;
        uint32_t b;
        struct tkh* next;
        struct tkh* back;
};

struct tkh2
{
        uint32_t c;
        uint32_t d;
        struct tkh2* next;
        struct tkh2* back;
};

struct tkh3
{
        uint32_t tkhar[510];
        struct tkh3* next;
        struct tkh3* back;
};

void test_kernel_heap_func();

#endif
