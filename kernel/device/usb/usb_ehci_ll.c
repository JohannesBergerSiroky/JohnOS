/* This file uses dynamically allocated linked lists for EHCI */

#include "../../types.h"
#include "usb.h"
#include "usb_ehci.h"
#include "usb_ehci_ll.h"
#include "../../mem.h"
#include "../../graphics/screen.h"


/* Initializes queue heads and writes a zero to most
 * values there.
 */
void init_qhs(int amount) 
{ 

	    Qh1 = (struct queue_head*)kmem_4k_allocate();
        zero_usbms_mem_4((volatile uint8_t*)Qh1);
	    Qh_firstnode = Qh1;
	    Qh_temp2 = Qh1;
	    Qh1->p_back = 0;

        Qh1->Horizontalpointer = Qh1;
	    Qh1->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
        zero_usbms_mem_6((uint32_t*)(Qh1->qhnext_qtd_ptr));
	    Qh1->qhnext_qtd_ptr->transfer_info = 0;
	    Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
        Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
        Qh1->qhnext_qtd_ptr->buffer_ptr0 = 0;
        Qh1->qhnext_qtd_ptr->buffer_ptr1 = 0;
        Qh1->qhnext_qtd_ptr->buffer_ptr2 = 0;
        Qh1->qhnext_qtd_ptr->buffer_ptr3 = 0;
        Qh1->qhnext_qtd_ptr->buffer_ptr4 = 0;
        Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = 0;
        Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = 0;
        Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = 0;
        Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = 0;
        Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = 0;
        Qh1->qh_extended_buffers[0] = 0;
        Qh1->qh_extended_buffers[1] = 0;
        Qh1->qh_extended_buffers[2] = 0;
        Qh1->qh_extended_buffers[3] = 0;
        Qh1->qh_extended_buffers[4] = 0;
	    Qh1->qhnext_qtd_ptr->zero = 0;
	    Qh1->qhnext_qtd_ptr->qtd_back = 0;
        Qh1->qhalternate_qtd_ptr = 0;
        Qh1->qhbuffer_ptr0 = 0;
        Qh1->qhbuffer_ptr1 = 0;
        Qh1->qhbuffer_ptr2 = 0;
        Qh1->qhbuffer_ptr3 = 0;
        Qh1->qhbuffer_ptr4 = 0;
        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
	    qt_temp2 = Qh1->qhnext_qtd_ptr;
	    qt_temp = Qh1->qhnext_qtd_ptr;
	    qt_temp3 = Qh1->qhnext_qtd_ptr;

        /* first qh node */
	    for(int i = 1; i <= amount;i++) {
			    
			    Qh_temp = Qh1;
			    if(i != amount) {
					    Qh1->Horizontalpointer = (struct queue_head*)kmem_4k_allocate();
                        zero_usbms_mem_6((uint32_t*)Qh1->Horizontalpointer);
			    }

			    else
				    Qh1->Horizontalpointer = 0;
			    if(i == 1) {

					    Qh1->endpointch1 = 0;
                        Qh1->endpointch2 = 0;
                        Qh1->qhtransfer_info = 0;
                        Qh1->amount_of_qts = 0;
			    }
			    else {
				        Qh1->endpointch1 = 0;
                        Qh1->endpointch2 = 0;
                        Qh1->qhtransfer_info = 0;
                }

			    Qh1->amount_of_qts = 1;
			    if((amount > 20) && (i == (int)(amount/2)))
				        Qh_middle_node = Qh1; 

			    if(i != amount) {
					    Qh1 = Qh1->Horizontalpointer; 
					    Qh1->p_back = Qh_temp;
					    Qh1->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                        Qh1->qhalternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                        zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr);
					    if(i == 2)
						        Qh1->qhnext_qtd_ptr->transfer_info = 0;
					    else
						        Qh1->qhnext_qtd_ptr->transfer_info = 0;
					    Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                        Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
                        Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
					    Qh1->qhnext_qtd_ptr->zero = 0;
					    Qh1->qhnext_qtd_ptr->qtd_back = 0;
			    }
			    else {
					    Qh_temp = Qh_firstnode;
			    } 

	    }

	    current_queue_head_pointers = amount;

        /* set Qh to either first node or middle node */
	    if((amount > 20)) {
			    current_qh_index = (int)(current_queue_head_pointers/2);
			    Qh1 = Qh_middle_node; 
	    }
	    else {
			    Qh1 = Qh_firstnode;
			    current_qh_index = 1;
	    }

}

/* Initializes queue transfer descriptors for queue heads.
 * Amount stands for amount of queue transfer 
 * descriptors, I guess.
 */
void init_queue_transfer_descriptors(volatile uint32_t active_qhs, volatile uint32_t amount) 
{

	    if(active_qhs == current_queue_head_pointers) {

	    Qh1 = Qh_firstnode;


	    for(uint32_t i = 1; i <= active_qhs;i++) {
			    qt_temp2 = Qh1->qhnext_qtd_ptr;
			    for(uint32_t a = 1; a <= amount; a++) {
					    qt_temp = Qh1->qhnext_qtd_ptr;
					    if((i == 2) && (a == 2))
						        Qh1->qhnext_qtd_ptr->transfer_info = 1; 
					    else {
						        Qh1->qhnext_qtd_ptr->transfer_info = 0;

					    }
                        Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                        Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
					    Qh1->qhnext_qtd_ptr->zero = 0;

					    if((a != amount)) {

		                        if((amount >= 800) && (a == (uint32_t)(amount/2))) {
                                        Qh1->qtd_middle = Qh1->qhnext_qtd_ptr;
                                }
			                             

					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->next_qtd_ptr);
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->alternate_qtd_ptr);
					            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr; 
					            Qh1->qhnext_qtd_ptr->qtd_back = qt_temp; 

					    }
					    else {
							    Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
							    Qh1->qhnext_qtd_ptr = qt_temp2;
							    qt_temp = qt_temp2;
					    }

			    }
			    Qh1->amount_of_qts = amount;
			    if(i != active_qhs) {

					    Qh1 = Qh1->Horizontalpointer;
			    }

	    }
                /* set Qh to either first node or middle node */
			    if((current_queue_head_pointers > 20))  {
					    Qh1 = Qh_middle_node;
					    qt_temp2 = Qh1->qhnext_qtd_ptr;
			    }
			    else {
					    Qh1 = Qh_firstnode;
					    qt_temp2 = Qh1->qhnext_qtd_ptr;
					    qt_temp = qt_temp2;
					    qt_temp3 = qt_temp;
					    current_qh_index = 1;
			    }
	    }
		    else
			        print("\nError: Function does not allow an amount that is not\nequal to the current amount of Queue Heads.");
		    print("\nend of set qts");

} 


/* This function makes a chosen amount of qt pointers in a chosen qh. cq_index could be current_qh_index or 1
 * aoq means amount of queue transfer descriptors to be put into the qh. cq_index is the current qh node.
 * qh index is the qh index to be chosen, and qh_c is the value to be put into the endpointch1 of the
 * chosen qh. cqhp_temp is the current amount of qhs in the linked list.
 */
void set_qh(volatile uint32_t qh_index, volatile uint32_t qh_c, volatile uint32_t aoq, volatile uint32_t cq_index, volatile uint32_t cqhp_temp)
{

	    volatile uint32_t d;
        volatile uint32_t e;
	    if((qh_index <= current_queue_head_pointers) && (qh_index >= 1)) {
	            if(cqhp_temp > 20) {
                        /* qh_index = chosen index. cq = where it is now. */
	                    if(qh_index >= cq_index) {
			                    Qh1 = Qh_middle_node;
                                /* if an index is set to the middle of the list then maybe it should stay there if the value of current_pointes is high */
			                    for(uint32_t i = cq_index; i <= qh_index;i++)  {
					                    if(i != qh_index)
						                        Qh1 = Qh1->Horizontalpointer;
			                    }
	                    }
	                    else if(qh_index <= cq_index) {
			                    Qh1 = Qh_middle_node;
			                    for(uint32_t i = cq_index; i >= qh_index;i--) {
					                    if(i != qh_index)
						                        Qh1 = Qh1->p_back;
			                    }

	                    }
	            }
	            else {
			            Qh1 = Qh_firstnode;
			            for(uint32_t i = 1; i <= qh_index;i++)
				            {
					            if(i != qh_index)
						            Qh1 = Qh1->Horizontalpointer;
				            }


	            }
		        qt_temp2 = Qh1->qhnext_qtd_ptr;
	            /* now set qts accordingly */
	            if (Qh1->amount_of_qts > aoq) { 
			            d = Qh1->amount_of_qts;
                        if(Qh1->amount_of_qts >= 800) {
                                Qh1->qhnext_qtd_ptr = Qh1->qtd_middle;
                                d = (volatile uint32_t)d/2;
                        }
			            for (uint32_t i = 0; i < d;i++) {
                                /* goes to the last qt element in selected qh */
				                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;

			            }
			            qt_temp = Qh1->qhnext_qtd_ptr;
			            d = Qh1->amount_of_qts - aoq;
			            for(uint32_t a = 1; a <= d ;a++) {

					            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back;
					            free_mem_uint((uint32_t)qt_temp);
					            qt_temp = Qh1->qhnext_qtd_ptr;

			            }
                        if((aoq >= 800)) {
                               Qh1->qhnext_qtd_ptr = qt_temp2;
                               e = (volatile uint32_t)aoq/2;
                               for (uint32_t i = 0; i < e;i++) {
                                       Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
                                    
                               }
	                           Qh1->qtd_middle = Qh1->qhnext_qtd_ptr; 


                       }
		               Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                       Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
                }
	            /* Qh1->amount_of_qts = aoq */
	            else if (Qh1->amount_of_qts < aoq) {
			            d = Qh1->amount_of_qts;
                        if(Qh1->amount_of_qts >= 800) {
                                Qh1->qhnext_qtd_ptr = Qh1->qtd_middle;
                                d = (volatile uint32_t)d/2;
                        }
			            for (uint32_t i = 0; i < d;i++) {
                                /* again, goes to the last qt element in selected qh */
				                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
			            }
			            qt_temp = Qh1->qhnext_qtd_ptr;
			            d = aoq - Qh1->amount_of_qts;
			            for(uint32_t a = 1; a <= d ;a++) {

					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->next_qtd_ptr);
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->alternate_qtd_ptr);
					            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
					            Qh1->qhnext_qtd_ptr->qtd_back = qt_temp;
					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
					            Qh1->qhnext_qtd_ptr->transfer_info = 0;
                                Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
					            Qh1->qhnext_qtd_ptr->zero = 0;
					            qt_temp = Qh1->qhnext_qtd_ptr;


			            }
                         if((aoq >= 800)) {
                                Qh1->qhnext_qtd_ptr = qt_temp2;
                                e = (volatile uint32_t)aoq/2;
                                for (uint32_t i = 0; i < e;i++) {
                                        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
                                        
                                }
		                        Qh1->qtd_middle = Qh1->qhnext_qtd_ptr; 


                        }
                    
	            }
                /* back to root */
	            qt_temp = qt_temp2; 
	            Qh1->qhnext_qtd_ptr = qt_temp2;
	            Qh1->amount_of_qts = aoq;

	            Qh1->endpointch1 = qh_c;
                Qh1->endpointch2 = 0;
                Qh1->qhtransfer_info = 0;
                Qh1->qhalternate_qtd_ptr = 0;
                if(Qh1 != Qh_firstnode) {
                        Qh1->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                        Qh1->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                }
                else {
                        Qh1->qhbuffer_ptr0 = 0;
                        Qh1->qhbuffer_ptr1 = 0;
                        Qh1->qhbuffer_ptr2 = 0;
                        Qh1->qhbuffer_ptr3 = 0;
                        Qh1->qhbuffer_ptr4 = 0;
                }

                /* set Qh to either first node or middle node */
	            if((current_queue_head_pointers > 20)) {
			            Qh1 = Qh_middle_node;
	            }
	            else {
			            Qh1 = Qh_firstnode;
			            current_qh_index = 1;
	            }
	    }

	    else
		    print("\nError: Invalid Queue Head Index.");

}

/* This function picks a qt in a chosen qh and traverses to the selected qt 
 * there, and sets some values in that qt. 
 * The kernel should use the set_qhs and set qts first.
 * cq_index could be current_qh_index or 1. qh index is the qh index to be chosen.
 * aoq means amount of queue transfer descriptors to be put into the qh.
 * qt_a and is the value to be put into the transfer info of the
 * chosen qt. qt_b is put into the extended buffers[0] of the qt we want to access.
 * cqhp_temp is the current amount of qhs in the linked list.
 * qt_index is the qt we'd like to access.
 */
void set_qt(volatile uint32_t qh_index, volatile uint32_t cq_index,  volatile uint32_t qt_index, volatile uint32_t qt_a, volatile uint32_t qt_b, volatile uint32_t cqhp_temp)
{

        volatile uint32_t d;
        volatile uint32_t f = 0;
        volatile uint32_t e;
        /* will use the qtd_middle here */
	    if((qh_index <= current_queue_head_pointers) && (qh_index >= 1)) {
	            if(cqhp_temp > 20) {
                /* qh_index = chosen index. cq = where it is now. */
	                    if(qh_index >= cq_index) {
			                    Qh1 = Qh_middle_node;
                                /* if an index is set to the middle of the list then maybe it should stay there if the value of current_pointes is high */
			                    for(uint32_t i = cq_index; i <= qh_index;i++) {
					                    if(i != qh_index)
						                        Qh1 = Qh1->Horizontalpointer;
			                    }
	                    }
	                    else if(qh_index <= cq_index) {
			                    Qh1 = Qh_middle_node;
			                    for(uint32_t i = cq_index; i >= qh_index;i--) {
					                    if(i != qh_index)
						                        Qh1 = Qh1->p_back;
			                    }

	                    }

                }
	            else {
			            Qh1 = Qh_firstnode;
			            for(uint32_t i = 1; i <= qh_index;i++) {
					            if(i != qh_index)
						                Qh1 = Qh1->Horizontalpointer;
			            }

		        }
	            qt_temp = Qh1->qhnext_qtd_ptr;
	            /* now we are at the selected Qh */
	            /* check if the amount of qts in the current Qh is equal or more than qt_index */
                if(Qh1->amount_of_qts >= 800) {
                        Qh1->qhnext_qtd_ptr = Qh1->qtd_middle;
                        /* middle set */
                        d = (volatile uint32_t)(Qh1->amount_of_qts / 2);
                        f = 1;
                }
                else {
                        d = qt_index;
                }

	            if((Qh1->amount_of_qts >= qt_index) && (qt_index >= 1)) {
			            print("\namount of qts in qh: ");
                        printi((uint32_t)Qh1->amount_of_qts);
                        if (f == 0) {
			                    for(uint32_t a = 0; a < d; a++) {
					                    if(a != qt_index) {
							                    Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
					                    }

			                    }

                        }
                        else if (f == 1) {
                                if(d > qt_index) {
                                        e = d - qt_index;
                                        for(uint32_t a = 0; a < e; a++) {
                                                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back;
                                        }


                                }
                                else if (d < qt_index) {
                                        e = qt_index - d;
                                        for(uint32_t a = 0; a < e; a++) {
                                                Qh1->qhnext_qtd_ptr = Qh1->qhcurrent_qtd_ptr->next_qtd_ptr;

                                        }

                                }

                        }
	                    Qh1->qhnext_qtd_ptr->transfer_info = qt_a;
	                    Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = qt_b;
	                    Qh1->qhnext_qtd_ptr = qt_temp;
	            }
					    
	            else
		            print("\nError: Invalid qt index.");
        }

	    else
		    print("Error, invalid queue head index.");
        /* set Qh to either first node or middle node */
	    if((current_queue_head_pointers > 20))  {
			    Qh1 = Qh_middle_node;
	    }
	    else {
			    print("\n going to the first node");
			    Qh1 = Qh_firstnode;
			    qt_temp = Qh1->qhnext_qtd_ptr;
			    current_qh_index = 1;
	    }
	    
}

/* Unlinks a selected Qh. First checks if it's the first Qh, somewhere in the middle, or last Qh. 
 * Also check the Qh_middle_node and change it. cq_index is the current qh index, and qh_index
 * is the desired qh we would like to unlink. cqhp_temp is the amount of qhs in the linked list.
 */
void unlink_qh(volatile uint32_t qh_index, volatile uint32_t cq_index, volatile uint32_t cqhp_temp) 
{ 

	    if((qh_index <= current_queue_head_pointers) && (qh_index >= 1)) {
                /* here can just let things be. I think. 
                * Because the first qh can be sort of empty
                */
	            if(qh_index == 1) {
                        /* temp = back */
			            Qh_temp = Qh_firstnode; 
                        /* qh firstnode at front */
			            Qh_firstnode = Qh_firstnode->Horizontalpointer; 
			            Qh_firstnode->p_back = 0;
			            free_mem_uint((uint32_t)Qh_temp);
			            Qh1 = Qh_firstnode;
	            }

	            if(qh_index > 1) {
			            if(cqhp_temp > 20) {
                            /* qh_index = chosen index. cq = where it is now. */
				            if(qh_index >= cq_index) {
                                    /* if an index is set to the middle of the list
                                     * then maybe it should stay there if the value
                                     * of current_pointes is high
                                     */
						            Qh1 = Qh_middle_node;
						            for(uint32_t i = cq_index; i <= qh_index;i++) {
								            if(i != qh_index)
									                Qh1 = Qh1->Horizontalpointer;
						            }
				            }
				            else if(qh_index <= cq_index) {
						            Qh1 = Qh_middle_node;
						            for(uint32_t i = cq_index; i >= qh_index;i--) {
								            if(i != qh_index)
									                Qh1 = Qh1->p_back;
						            }

				            }
			            }
			            else {
					            Qh1 = Qh_firstnode;
					            for(uint32_t i = 1; i <= qh_index;i++) {
							            if(i != qh_index)
								                Qh1 = Qh1->Horizontalpointer;
					            }


				            }
                        /* now we are at the selected qh */
			            if(qh_index == cqhp_temp) {
                                qt_temp = Qh1->qhnext_qtd_ptr;
                                for(uint32_t a = 0; a < Qh1->amount_of_qts; a++) {
                                        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
                                        free_mem_uint((uint32_t)qt_temp);

                                }
                                free_mem_uint((uint32_t)Qh1->qhnext_qtd_ptr);
					            Qh_temp = Qh1; 

					            Qh1 = Qh1->p_back; 
					            Qh1->Horizontalpointer = 0;
					            free_mem_uint((uint32_t)Qh_temp);
					            Qh_temp = Qh_firstnode;
					            qh_index--;
			            }
		                /* a qh somewhere between */
		                else if(qh_index < cqhp_temp) {
				                
                                qt_temp = Qh1->qhnext_qtd_ptr;
                                for(uint32_t a = 0; a < Qh1->amount_of_qts; a++) {
                                        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
                                        free_mem_uint((uint32_t)qt_temp);

                                }

                                free_mem_uint((uint32_t)Qh1->qhnext_qtd_ptr);
				                Qh_temp = Qh1;
				                Qh_temp2 = Qh1;
				                Qh_temp = Qh_temp->Horizontalpointer; 
				                Qh1 = Qh1->p_back; 
				                Qh_temp->p_back = Qh1;
				                Qh1->Horizontalpointer = Qh_temp;
				                free_mem_uint((uint32_t)Qh_temp2);
				                Qh1 = Qh1->Horizontalpointer;
				                Qh_temp2 = Qh_firstnode;
				                Qh_temp = Qh_firstnode;
		                }
			    }
			    current_queue_head_pointers--;
                /* set Qh to either first node or middle node */
			    if((current_queue_head_pointers > 20)) {
					    current_qh_index = (int)(current_queue_head_pointers/2);
					    cq_index = current_qh_index;

					    if(qh_index >= cq_index) {

							    for(uint32_t i = qh_index; i >= cq_index;i--) {
									    if(i != cq_index)
										        Qh1 = Qh1->p_back;
							    }
					    }
					    else if(qh_index <= cq_index) {

							    for(uint32_t i = qh_index; i <= cq_index;i++) {
									    if(i != cq_index)
										        Qh1 = Qh1->Horizontalpointer;
							    }

					    }
					    Qh_middle_node = Qh1;
			    }
			    else {
					    Qh1 = Qh_firstnode;
					    Qh_temp = Qh_firstnode;
					    current_qh_index = 1;
			    }
	    }
	    else
		    print("\nInvalid queue head index.");
	
} 

/* Unlinks a qt. Initialize and set qts first before 
 * using this function.
 * cq_index is the current qh index, and qh_index
 * is the desired qh we would like to unlink. 
 * cqhp_temp is the amount of qhs in the linked list.
 */
void unlink_qt(volatile uint32_t qh_index, volatile uint32_t qt_index, volatile uint32_t cq_index, volatile uint32_t cqhp_temp) 
{

	    if((qh_index > cqhp_temp) || (qh_index < 1))
		        print("\n\nError: Invalid queue head index"); 
	    else {

		        if(cqhp_temp > 20) {

                        /* qh_index = chosen index. cq = where it is now. */
		                if(qh_index >= cq_index)  {
				                Qh1 = Qh_middle_node;
                                /* if an index is set to the middle of the list then maybe it should stay there if the value of current_pointes is high */
				                for(uint32_t i = cq_index; i <= qh_index;i++)  {
						                if(i != qh_index)
							                    Qh1 = Qh1->Horizontalpointer;
				                }
		                }
		                else if(qh_index <= cq_index) {
				                Qh1 = Qh_middle_node;
				                for(uint32_t i = cq_index; i >= qh_index;i--) {
						                if(i != qh_index)
							                    Qh1 = Qh1->p_back;
				                }

		                }
		        }
		        else {
			        Qh1 = Qh_firstnode;
			        for(uint32_t i = 1; i <= qh_index;i++) {
					        if(i != qh_index)
						            Qh1 = Qh1->Horizontalpointer;
			        }


		        } 

                /* now we are at the selected Qh
	               check if the amount of qts in the current Qh is equal or more than qt_index */
	            qt_temp3 = Qh1->qhnext_qtd_ptr;
	            qt_temp2 = Qh1->qhnext_qtd_ptr;
	            qt_temp = Qh1->qhnext_qtd_ptr;
                volatile uint32_t d;
                volatile uint32_t e;
                volatile uint32_t f;


		        /* now unlink a qt */
		        if((qt_index > 0) && (qt_index < (Qh1->amount_of_qts + 1))) {

		                if((qt_index == 1) && (Qh1->amount_of_qts > 1)) {
                                /* written before: temp = furthest back */
				                qt_temp = Qh1->qhnext_qtd_ptr; 
				                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
				                Qh1->qhnext_qtd_ptr->qtd_back = 0;
				                free_mem_uint((uint32_t)qt_temp);
				                qt_temp3 = Qh1->qhnext_qtd_ptr;
		                }
                        else if ((qt_index <= Qh1->amount_of_qts) && (qt_index > 1)) {
                   
                                if(Qh1->amount_of_qts >= 800) {

                                        Qh1->qhnext_qtd_ptr = Qh1->qtd_middle;
                                        d = (volatile uint32_t)(Qh1->amount_of_qts / 2);
                                        f = 1;
                                } 
                                else
                                        d = qt_index;

                                if (f == 0) {

			                            for(uint32_t a = 0; a < d; a++) {

					                            if(a != qt_index) {
                                                        /* use middle here */
							                            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr; 
					                            }

			                            }

                                }

                                else if (f == 1) {

                                        if(d > qt_index) {
                                                e = d - qt_index;
                                                for(uint32_t a = 0; a < e; a++) {
                                                        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back;
                                                }


                                        }

                                        else if (d < qt_index) {
                                                e = qt_index - d;
                                                for(uint32_t a = 0; a < e; a++) {
                                                        Qh1->qhnext_qtd_ptr = Qh1->qhcurrent_qtd_ptr->next_qtd_ptr;

                                                }

                                        }


                                }

                        }

                        /* if qt = max amount of qts in that Qh */
		                if((qt_index == Qh1->amount_of_qts) && (Qh1->amount_of_qts > 1)) {
                     
                                /* written before: temp = front */
		                        qt_temp = Qh1->qhnext_qtd_ptr;
		                        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back;
		                        Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
		                        free_mem_uint((uint32_t)qt_temp);
                        }

                        /* if qt is in the middle */
		                else if((qt_index < Qh1->amount_of_qts) && (qt_index != 1)) {


				                qt_temp2 = Qh1->qhnext_qtd_ptr;
				                qt_temp = Qh1->qhnext_qtd_ptr->next_qtd_ptr;

				                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back; 

				                qt_temp->qtd_back = Qh1->qhnext_qtd_ptr; 


				                Qh1->qhnext_qtd_ptr->next_qtd_ptr = qt_temp;


				                print("\nimhere im here im here again.\n");
				                free_mem_uint((uint32_t)qt_temp2);
				                print("\nimhere im here im here again again.\n");

		                }

                Qh1->amount_of_qts--;
                Qh1->qhnext_qtd_ptr = qt_temp3;
                qt_temp = Qh1->qhnext_qtd_ptr;
                qt_temp2 = Qh1->qhnext_qtd_ptr;


                }

		        else {
			            print("\nError: Invalid qt index");
                        return;
                }

                if(Qh1->amount_of_qts >= 800) {

                        d = (volatile uint32_t)Qh1->amount_of_qts / 2;
                        for(uint32_t i = 0; i < d; i++) {
                                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;

                        }
                        Qh1->qtd_middle = Qh1->qhnext_qtd_ptr;

                }
                

		        print("\n at the end of the function");
		        if((current_queue_head_pointers > 20)) {
				        Qh1 = Qh_middle_node;
		        }
		        else {
				        Qh1 = Qh_firstnode;
				        current_qh_index = 1;
		        }
	    }
        /* written before: ok except set Qh1->qhnext_qtd_ptr to qt_temp at the end. */

} 

/* Adds a queue head to the list. 
 * cq_index is the current qh index, and qh_index
 * is the desired qh we would like to unlink. 
 * cqhp_temp is the amount of qhs in the linked list.
 */ 
void add_qh(volatile uint32_t qh_index, volatile uint32_t cqhp_temp, volatile uint32_t cq_index) 
{

	    if((qh_index <= current_queue_head_pointers) && (qh_index >= 1)) {

	            if(qh_index == 1) {	
			            Qh_temp = Qh_firstnode;
			            Qh_firstnode = (struct queue_head*)kmem_4k_allocate();
                        zero_usbms_mem_4((volatile uint8_t*)Qh_firstnode);
                        /* add things here for Qh_temp which is the second node now (it was the first node before) */
			            Qh_firstnode->Horizontalpointer = Qh_temp; 
			            Qh_firstnode->p_back = 0;
			            Qh_temp->p_back = Qh_firstnode;
			            Qh_firstnode->endpointch1 = 0;
                        Qh_firstnode->endpointch2 = 0;
                        Qh_firstnode->qhtransfer_info = 0;
                        Qh_firstnode->qhalternate_qtd_ptr = 0;
                        Qh_firstnode->qhbuffer_ptr0 = 0;
                        Qh_firstnode->qhbuffer_ptr1 = 0;
                        Qh_firstnode->qhbuffer_ptr2 = 0;
                        Qh_firstnode->qhbuffer_ptr3 = 0;
                        Qh_firstnode->qhbuffer_ptr4 = 0;
			            Qh_firstnode->amount_of_qts = 0;
                    
                        /* set the qt to default values */
			            Qh_firstnode->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate(); 
                        zero_usbms_mem_6((uint32_t*)Qh_firstnode->qhnext_qtd_ptr);
			            Qh_firstnode->qhnext_qtd_ptr->transfer_info = 0;
			            Qh_firstnode->qhnext_qtd_ptr->next_qtd_ptr = 0;
                        Qh_firstnode->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
                        Qh_firstnode->qhnext_qtd_ptr->buffer_ptr0 = 0;
                        Qh_firstnode->qhnext_qtd_ptr->buffer_ptr1 = 0;
                        Qh_firstnode->qhnext_qtd_ptr->buffer_ptr2 = 0;
                        Qh_firstnode->qhnext_qtd_ptr->buffer_ptr3 = 0;
                        Qh_firstnode->qhnext_qtd_ptr->buffer_ptr4 = 0;
                        Qh_firstnode->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)0;
                        Qh_firstnode->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)0;
                        Qh_firstnode->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)0;
                        Qh_firstnode->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)0;
                        Qh_firstnode->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)0;
			            Qh_firstnode->qhnext_qtd_ptr->zero = 0;
			            Qh_firstnode->qhnext_qtd_ptr->qtd_back = 0;
			            Qh_temp = Qh_firstnode;
			            Qh1 = Qh_firstnode;
	            } 
	            if (qh_index > 1) {

	                    if(cqhp_temp > 20) {
                                /* qh_index = chosen index. cq = where it is now. */
			                    if(qh_index >= cq_index) {

					                    Qh1 = Qh_middle_node;
                                        /* if an index is set to the middle of the list then maybe it should stay there if the value of current_pointes is high */
					                    for(uint32_t i = cq_index; i <= qh_index;i++) {
							                    if(i != qh_index)
								                        Qh1 = Qh1->Horizontalpointer;
					                    }

			                    }

			                    else if(qh_index <= cq_index) {
					                    Qh1 = Qh_middle_node;

					                    for(uint32_t i = cq_index; i >= qh_index;i--) {
							                    if(i != qh_index)
								                        Qh1 = Qh1->p_back;
					                    }

			                    }
	                    }

                        else {

	                            Qh1 = Qh_firstnode;
	                            for(uint32_t i = 1; i <= qh_index;i++) {
			                            if(i != qh_index)
				                            Qh1 = Qh1->Horizontalpointer;
	                            }


                        }

	                    if(qh_index == cqhp_temp) {


			                    Qh_temp = Qh1;
			                    Qh1->Horizontalpointer = (struct queue_head*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->Horizontalpointer);
			                    Qh1 = Qh1->Horizontalpointer;
			                    Qh1->Horizontalpointer = 0;
			                    Qh1->p_back = Qh_temp;
			                    Qh1->endpointch1 = 0;
                                Qh1->endpointch2 = 0;
                                Qh1->qhtransfer_info = 0;
			                    Qh1->amount_of_qts = 1;
                                /* and afterwards set the new qt */
			                    Qh1->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate(); 
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr);
                                Qh1->qhalternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
			                    Qh1->qhnext_qtd_ptr->transfer_info = 0;
			                    Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
			                    Qh1->qhnext_qtd_ptr->zero = 0;
			                    Qh1->qhnext_qtd_ptr->qtd_back = 0;
			                    Qh1 = Qh1->p_back;
			                    Qh_temp = Qh_firstnode;
	                    }

	                    if((qh_index > 1) && (qh_index < cqhp_temp)) {

			                    Qh_temp = Qh1;
			                    Qh_temp = Qh_temp->Horizontalpointer;
                                /* qh1 = middle */
			                    Qh1->Horizontalpointer = (struct queue_head*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->Horizontalpointer);
			                    Qh_temp2 = Qh1; 
                                /* before Qh_temp: */
			                    Qh1 = Qh1->Horizontalpointer; 
			                    Qh_temp->p_back = Qh1;
			                    Qh1->endpointch1 = 0;
                                Qh1->endpointch2 = 0;
                                Qh1->qhtransfer_info = 0;
			                    Qh1->amount_of_qts = 1;
                                /* set the Qh pointers */
			                    Qh1->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate(); 
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr);
                                Qh1->qhalternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhbuffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
			                    Qh1->Horizontalpointer = Qh_temp;
			                    Qh1->p_back = Qh_temp2;

			                    Qh1->qhnext_qtd_ptr->transfer_info = 0;
			                    Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();

                                /* set all next and back pointers correctly */
			                    Qh1->qhnext_qtd_ptr->zero = 0; 
			                    Qh1->qhnext_qtd_ptr->qtd_back = 0;
			                    Qh1 = Qh1->p_back;
			                    Qh_temp = Qh_firstnode;
			                    Qh_temp2 = Qh_firstnode;


	                    }
	            }	

                /* set Qh to either first node or middle node */
			    current_queue_head_pointers++;
			    if((current_queue_head_pointers > 20)) {

					    current_qh_index = (int)(current_queue_head_pointers/2);
					    cq_index = current_qh_index;

                        /* qh_index = chosen index. cq = where it is now. */
					    if(qh_index >= cq_index) {

                                /* if an index is set to the middle of the list then maybe it should stay there if the value of current_pointes is high */
							    for(uint32_t i = qh_index; i >= cq_index;i--) {
									    if(i != cq_index)
										        Qh1 = Qh1->p_back;
							    }
					    }

					    else if(qh_index <= cq_index) {

							    for(uint32_t i = qh_index; i <= cq_index;i++) {
									    if(i != cq_index)
										        Qh1 = Qh1->Horizontalpointer;
							    }

					    }
					    Qh_middle_node = Qh1;
			    }

			    else {
					    Qh1 = Qh_firstnode;
					    current_qh_index = 1;
			    }
	    }

	    else
		    print("Error, invalid queue head index.");

}

/* Adds a queue transfer descriptor in a selected qh at a 
 * selected qt index.
 * cq_index is the current qh index, and qh_index
 * is the desired qh we would like to unlink. 
 * cqhp_temp is the amount of qhs in the linked list.
 */ 
void add_qt(volatile uint32_t qh_index, volatile uint32_t qt_index, volatile uint32_t cqhp_temp, volatile uint32_t cqh_index)
{
	    if((qh_index <= current_queue_head_pointers) && (qh_index >= 1)) {

        volatile uint32_t d;
        volatile uint32_t e;
        volatile uint32_t f;

		        if(cqhp_temp > 20) {

                        /* qh_index = chosen index. cq = where it is now. */
			            if(qh_index >= cqh_index) {
					            Qh1 = Qh_middle_node;

                                /* if an index is set to the middle of the list then maybe it should stay there if the value of current_pointes is high */
					            for(uint32_t i = cqh_index; i <= qh_index;i++) {
							            if(i != qh_index)
								            Qh1 = Qh1->Horizontalpointer;
					            }
			            }

			            else if(qh_index <= cqh_index) {
					            Qh1 = Qh_middle_node;
					            for(uint32_t i = cqh_index; i >= qh_index;i--) {
							            if(i != qh_index)
								                Qh1 = Qh1->p_back;
					            }
                            /* Qh is now at an index */
			            } 
		        }

		        else {

			        Qh1 = Qh_firstnode;
			        for(uint32_t i = 1; i <= qh_index;i++) {
					        if(i != qh_index)
						            Qh1 = Qh1->Horizontalpointer;
			        }


		        }

                /* Qh selected */
		        if((qt_index > 0) && (qt_index < (Qh1->amount_of_qts + 1))) {
			            qt_temp3 = Qh1->qhnext_qtd_ptr;


                        /* first qt */
			            if(qt_index == 1) {

					            qt_temp = Qh1->qhnext_qtd_ptr;		
					            Qh1->qhnext_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr);
					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = qt_temp;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->alternate_qtd_ptr);
					            qt_temp->qtd_back = Qh1->qhnext_qtd_ptr;
					            Qh1->qhnext_qtd_ptr->qtd_back = 0;
					            Qh1->qhnext_qtd_ptr->transfer_info = 0;
                                Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
					            Qh1->qhnext_qtd_ptr->zero = 0;
					            qt_temp3 = Qh1->qhnext_qtd_ptr;

			            }

                        else if ((qt_index <= Qh1->amount_of_qts) && (qt_index > 1)) {

                                if(Qh1->amount_of_qts >= 800) {

                                        Qh1->qhnext_qtd_ptr = Qh1->qtd_middle;
                                        d = (volatile uint32_t)(Qh1->amount_of_qts / 2);
                                        f = 1;
                                        /* middle set */
                                } 
                                else
                                        d = qt_index;

                                if (f == 0) {

			                            for(uint32_t a = 0; a < d; a++) {

					                            if(a != qt_index) {

                                                        /* written before: use middle here */
							                            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr; 
					                            }
			                            }
                                }

                                else if (f == 1) {

                                        if(d > qt_index) {

                                                e = d - qt_index;

                                                for(uint32_t a = 0; a < e; a++) {
                                                        Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back;
                                                }


                                        }

                                        else if (d < qt_index) {

                                                e = qt_index - d;
                                                for(uint32_t a = 0; a < e; a++) {

                                                        Qh1->qhnext_qtd_ptr = Qh1->qhcurrent_qtd_ptr->next_qtd_ptr;

                                                }
                                        }

                                }

                        }

                        /* the end qt */
			            if((qt_index == Qh1->amount_of_qts) && (Qh1->amount_of_qts != 1)) {
					            qt_temp = Qh1->qhnext_qtd_ptr;
					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->next_qtd_ptr);
					            Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate();
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->alternate_qtd_ptr);
					            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
					            Qh1->qhnext_qtd_ptr->transfer_info = 0;
                                Qh1->qhnext_qtd_ptr->next_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = 0;
                                Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
					            Qh1->qhnext_qtd_ptr->zero = 0;
					            Qh1->qhnext_qtd_ptr->qtd_back = qt_temp;
					            Qh1->qhnext_qtd_ptr = qt_temp3;
					            qt_temp = Qh1->qhnext_qtd_ptr;
			            }

                        /* at somewhere in middle qt */
			            if((qt_index > 1) && (qt_index < Qh1->amount_of_qts)) {
                                /* temp = back */
					            qt_temp = Qh1->qhnext_qtd_ptr; 
                                /* Qh1->qhnext_qtd_ptr = middle */
					            qt_temp2 = Qh1->qhnext_qtd_ptr; 
                                /* temp2 = front */
					            qt_temp2 = Qh1->qhnext_qtd_ptr->next_qtd_ptr; 
					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate(); 
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->next_qtd_ptr);
					            Qh1->qhnext_qtd_ptr->alternate_qtd_ptr = (struct queue_transfer_descriptor*)kmem_4k_allocate(); 
                                zero_usbms_mem_6((uint32_t*)Qh1->qhnext_qtd_ptr->alternate_qtd_ptr);
					            Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
					            Qh1->qhnext_qtd_ptr->transfer_info = 0;
                                Qh1->qhnext_qtd_ptr->buffer_ptr0 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr1 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr2 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr3 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->buffer_ptr4 = (volatile uint8_t*)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[0] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[1] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[2] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[3] = (volatile uint32_t)kmem_4k_allocate();
                                Qh1->qhnext_qtd_ptr->qt_extended_buffers[4] = (volatile uint32_t)kmem_4k_allocate();
					            Qh1->qhnext_qtd_ptr->zero = 0;
					            Qh1->qhnext_qtd_ptr->qtd_back = qt_temp;
					            qt_temp->next_qtd_ptr = Qh1->qhnext_qtd_ptr;
					            Qh1->qhnext_qtd_ptr->next_qtd_ptr = qt_temp2; 
					            qt_temp2->qtd_back = Qh1->qhnext_qtd_ptr;

                                /* written before: back to root */
					            Qh1->qhnext_qtd_ptr = qt_temp3; 
					            qt_temp = Qh1->qhnext_qtd_ptr;
					            qt_temp2 = Qh1->qhnext_qtd_ptr;
			            }
			            Qh1->amount_of_qts++;
		        }

		        else {
			            print("\nError: Invalid qt index.");
                        return;
                }


                if(Qh1->amount_of_qts >= 800) {

                        d = (volatile uint32_t)Qh1->amount_of_qts / 2;
                        for(uint32_t i = 0; i < d; i++) {
                                Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;

                            }
                        Qh1->qtd_middle = Qh1->qhnext_qtd_ptr;

                }
                /* set Qh to either first node or middle node */
		        if((current_queue_head_pointers > 20)) {
				        Qh1 = Qh_middle_node;
		        }

		        else {
				        Qh1 = Qh_firstnode;
				        current_qh_index = 1;
		        }
        }

        else
		        print("Error, invalid queue head index.");

}

/* Adds queue heads to the list. q_index should mean the desired
 * spot where we add queue heads. q_amount is the amount of 
 * queue_heads we'd like to add.
 */
void add_qhs(volatile uint32_t q_index, int q_amount)
{
        if(q_amount <= 0) {
		        print("\nError. Invalid amount of Qhs to add");
		        return;
        }

        for(int b = 1; b <= q_amount; b++) {
		        add_qh(q_index, current_queue_head_pointers, current_qh_index);

        }
}

/* Removes queue heads from the list. q_index should mean the desired
 * spot where we add queue heads. q_amount is the amount of 
 * queue_heads we'd like to add.
 */
void remove_qhs(volatile uint32_t q_index, volatile uint32_t q_amount)
{
        if((q_amount >= current_queue_head_pointers) || (q_amount <= 0)) {
		        print("\nError. Invalid amount of Qhs to remove");
		        return;
        }

        for(uint32_t b = 1; b <= q_amount; b++) {
		        unlink_qh(q_index, current_qh_index, current_queue_head_pointers);

        }
}

/* Adds some queue transfer descriptors to the list. 
 * q_index stands for the desired qh we'd like to add
 * qts to. qd_index is the qt index where we start to
 * add qts. qd_amount is the amount of qts to be added.
*/
void add_qts(volatile uint32_t q_index, volatile uint32_t qd_index, int qd_amount)
{
	    for(int b = 1; b <= qd_amount; b++)
		    add_qt(q_index, qd_index, current_queue_head_pointers, current_qh_index);
}

/* Removes some queue transfer descriptors to the list. 
 * q_index stands for the desired qh we'd like to remove
 * qts from. qd_index is the qt index where we start to
 * remove qts. qd_amount is the amount of qts to be 
 * removed.
 */
void remove_qts(volatile uint32_t q_index, volatile uint32_t qd_index, int qd_amount)
{
	    for(int b = 1; b <= qd_amount; b++)
		    unlink_qt(q_index, qd_index, current_qh_index, current_queue_head_pointers);
}

/* Frees all queue heads and queue transfer descriptors
 * from the heap. cqhp_temp stands for the amount of qhs
 * in the list.
 */
void free_mem_uint_all_qhs(int cqhp_temp)
{

	    Qh1 = Qh_firstnode;

	    for(int i = 1; i <= cqhp_temp; i++) {

			    for(uint32_t b = 1; b <= Qh1->amount_of_qts; b++) {

					    qt_temp = Qh1->qhnext_qtd_ptr;
					    if (b != Qh1->amount_of_qts) {
							    Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
							    free_mem_uint((uint32_t)qt_temp);
					    }

					    else {

							    Qh1->qhnext_qtd_ptr = 0;
							    free_mem_uint((uint32_t)qt_temp);

							    qt_temp = 0;
					    }
			    }

						    
			    if(i != cqhp_temp) {
					    Qh1 = Qh1->Horizontalpointer; 

			    }

			    else {

					    free_mem_uint((uint32_t)Qh1);
					    Qh_firstnode = 0;
					    Qh_middle_node = 0;
					    Qh_temp = 0;
					    Qh_temp2 = 0;
					    qt_temp = 0;
					    qt_temp2 = 0;
					    qt_temp3 = 0;
					    free_mem_uint((uint32_t)Qh_temp);
					    Qh1 = 0;
			    }

	    }

}

/* Prints all queue heads and queue transfer descriptors
 * in the list. cqhp_temp stands for the amount of qhs
 * in the list.
 */
void print_all_qhsqts(volatile uint32_t cqhp_temp) 
{
	    Qh1 = Qh_firstnode;

	    for(uint32_t i = 1; i <= cqhp_temp;i++) {


	            print("\nQh1 endpointch1: ");
                printi((uint32_t)Qh1->endpointch1);
	            print("\nQh1 amount of qts: ");
                printi((uint32_t)Qh1->amount_of_qts);

		        qt_temp = Qh1->qhnext_qtd_ptr;
			    for(uint32_t a = 1; a <= Qh1->amount_of_qts; a++) {

				        print("\nQh1 qt ");
                        printi(a);
                        print(" transfer info: ");
                        printi((uint32_t)Qh1->qhnext_qtd_ptr->transfer_info);
				        print("\nQh1 current qt extended buffers 0: ");
                        printi((uint32_t)Qh1->qhnext_qtd_ptr->qt_extended_buffers[0]);
					    if(a != Qh1->amount_of_qts)
						    Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
					    else
						    Qh1->qhnext_qtd_ptr = qt_temp;
			    }
			    if(i != cqhp_temp)
				    Qh1 = Qh1->Horizontalpointer;
			    else
				    Qh1 = Qh_firstnode;


	    }
}

/* Prints all of the backpointers from all queue heads
 * in the list. cqhp_temp stands for the amount of qhs 
 * in the list.
 */
void print_qh_backpointers(volatile uint32_t cqhp_temp)
{
	    Qh1 = Qh_firstnode;
	    uint32_t ptr;

	    for(uint32_t i = 1; i <= cqhp_temp; i++) {
			    if(i != cqhp_temp)
				        Qh1 = Qh1->Horizontalpointer;
	    }

	    print("\nStarting to print Qh backpointers...");
	    for(uint32_t a = cqhp_temp; a >=1; a--) {
			    ptr = (uint32_t)(Qh1->p_back);
			    print("\nCurrent qh back pointer: ");
                printi(ptr);

			    if(a != 1)
				        Qh1 = Qh1->p_back;
	     }
}

/* Prints all of the backpointers from all queue transfer
 * descriptors in the list. cqhp_temp stands for the 
 * amount of qhs in the list.
 */
void print_qt_backpointers(volatile uint32_t cqhp_temp)
{
	    Qh1 = Qh_firstnode;
	    qt_temp2 = Qh1->qhnext_qtd_ptr;
	    uint32_t ptr;
	    for(uint32_t i = 1; i <= cqhp_temp; i++) {
			    if(i != cqhp_temp)
				    Qh1 = Qh1->Horizontalpointer;
	    }


	    for(uint32_t a = cqhp_temp; a >=1; a--) {
			    
			    for(uint32_t b = 1; b <= Qh1->amount_of_qts; b++) {

					    if(b != Qh1->amount_of_qts)
						    Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->next_qtd_ptr;
			    }
			    
			    for(uint32_t d = Qh1->amount_of_qts; d >= 1; d--) {
					    ptr = (uint32_t)(Qh1->qhnext_qtd_ptr->qtd_back);
					    print("\nCurrent qt back pointer: ");
                        printi(ptr);

					    if(d != 1)
						    Qh1->qhnext_qtd_ptr = Qh1->qhnext_qtd_ptr->qtd_back;
			    }

			    if(a != 1) {
					    
					    Qh1 = Qh1->p_back;
			    }

	     }
	    
	    Qh1 = Qh_firstnode;
	    qt_temp = qt_temp2;
	    Qh1->qhnext_qtd_ptr = qt_temp2;
	    qt_temp3 = qt_temp2;
}



