/* This file tests the kernel heap. */

#include "types.h"
#include "test_kernel_heap.h"
#include "graphics/screen.h"
#include "mem.h"
#include "kernel_heap_alloc_nopaging.h"


/* Tests the kernel heap. Please see kernel_heap_alloc_nopaging
 * for more things about the kernel heap. 
 */
void test_kernel_heap_func()
{

        /* Creates the structure. */
        struct tkh3* test_list_head = (struct tkh3*)kmem_allocate(sizeof(struct tkh3));
        print("\n Size of struct tkh3: ");
        printi((uint32_t)sizeof(struct tkh3));
        /* Creates two pointers to the structure. */
        struct tkh3* temp = test_list_head;
        struct tkh3* temp_back;
        test_list_head->back = (struct tkh3*)0;


        /* Creates structures in a list of three items and puts values into the array
         * in the structures. 
        */
        for(int32_t a = 0; a < 3; a++) {
                temp_back = test_list_head;
                for(int32_t b = 0; b < 510; b++) {
                        test_list_head->tkhar[b] = b;
                }
                test_list_head->next = (struct tkh3*)kmem_allocate(sizeof(struct tkh3));
                /*Commented code: if((a == 8) || (a == 9))
                        kfree(test_list_head); */
                test_list_head = test_list_head->next;
                test_list_head->back = temp_back;
                
        }

        /* Populates the last node. */
        for(int32_t b = 0; b < 510; b++) {
                test_list_head->tkhar[b] = b;
        }
        test_list_head->next = (struct tkh3*)0;

        /* Sets the list back to the first node. */
        test_list_head = temp;
        /* Prints the addresses of the nodes and the values in the array. */
        for(int32_t a = 0; a < 4; a++) {
                print(" list node ");
                printi((uint32_t)(a+1));
                print("'s address: ");
                print_hex((uint32_t)test_list_head);
                /* Commented code: 
                print(" Values in the array: ");
                for(int32_t b = 0; b < 30; b++) {
                        printi(test_list_head->tkhar[b]);
                        print(", ");
                } */ 

                test_list_head = test_list_head->next;
        }

        /* Sets the list back to the first node. */
        test_list_head = temp;

        /* Frees the list. */


        struct tkh* test_list_head2 = (struct tkh*)kmem_allocate(sizeof(struct tkh));
        struct tkh* temp2 = test_list_head2;
        struct tkh* temp_back2;


        test_list_head2->back = (struct tkh*)0;
        for(int32_t a = 0; a < 15; a++) {
                temp_back2 = test_list_head2;
                test_list_head2->a = 1;
                test_list_head2->b = 1;
                test_list_head2->next = (struct tkh*)kmem_allocate(sizeof(struct tkh));
                /* Commented code: if((a == 8) || (a == 9))
                        kfree(test_list_head2); */
                test_list_head2 = test_list_head2->next;
                test_list_head2->back = temp_back2;
                
        } 

        /* Populates the last node. */
        test_list_head2->a = 2;
        test_list_head2->b = 2;
        test_list_head2->next = (struct tkh*)0;



        test_list_head2 = temp2;
        
        for(int32_t a = 0; a < 15; a++) {

            print("\nlist node ");
            printi((uint32_t)(a+1));
            print(": ");
            print_hex((uint32_t)test_list_head2);
            test_list_head2 = test_list_head2->next;
 
        }
        test_list_head2 = temp2;
        for(int32_t a = 0; a < 15; a++) {
                test_list_head2 = test_list_head2->next;
                kfree(test_list_head2->back);
                
        }
        kfree(test_list_head2); 

        /* for(int32_t a = 0; a < 3; a++) {
                test_list_head = test_list_head->next;
                kfree(test_list_head->back);
                
        }
        kfree(test_list_head); */
        /* Commented code: 
        struct tkh2* test_two = (struct tkh2*)kmem_allocate(sizeof(struct tkh2));
        temp2 = test_two;
        test_two->c = 3;
        test_two->d = 3;
        test_two->next = (struct tkh2*)kmem_allocate(sizeof(struct tkh2));
        test_two->back = (struct tkh2*)0;
        temp_back2 = test_two;
        test_two = test_two->next;
        test_two->c = 3;
        test_two->d = 3;
        test_two->next = (struct tkh2*)0;
        test_two->back = temp_back2;

        test_two = temp2; */
        
        /* Prints the memory addresses. */
        /* Commented code: for(int32_t a = 0; a < 2; a++) {

            print("\nlist node (the other struct) ");
            printi((uint32_t)(a+1));
            print(": ");
            print_hex((uint32_t)test_two);
            test_two = test_two->next;

        } */

}
