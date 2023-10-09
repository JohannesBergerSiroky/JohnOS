#ifndef USB_EHCI_LL_H
#define USB_EHCI_LL_H


struct queue_head* Qh1;
struct queue_head* Qh_firstnode; 
struct queue_head* Qh_middle_node; 
struct queue_head* Qh_temp; 
struct queue_head* Qh_temp2; 


struct queue_transfer_descriptor* qt_temp;
struct queue_transfer_descriptor* qt_temp2;
struct queue_transfer_descriptor* qt_temp3;


volatile uint32_t current_queue_head_pointers;
volatile uint32_t current_qh_index;

void init_qhs(int amount); 
void init_queue_transfers(volatile uint32_t active_qhs, volatile uint32_t amount); 
void set_qh(volatile uint32_t qh_index, volatile uint32_t qh_c, volatile uint32_t aoq, volatile uint32_t cq_index, volatile uint32_t cqhp_temp);  

void set_qt(volatile uint32_t qh_index, volatile uint32_t cq_index, volatile uint32_t qt_index, volatile uint32_t qt_a, volatile uint32_t qt_b, volatile uint32_t cqhp_temp); 
void unlink_qh(volatile uint32_t qh_index, volatile uint32_t cq_index, volatile uint32_t cqhp_temp); 
void unlink_qt(volatile uint32_t qh_index, volatile uint32_t qt_index, volatile uint32_t cq_index, volatile uint32_t cqhp_temp); 
void add_qh(volatile uint32_t qh_index, volatile uint32_t cqhp_temp, volatile uint32_t cq_index);

void add_qt(volatile uint32_t qh_index, volatile uint32_t qt_index, volatile uint32_t cqhp_temp, volatile uint32_t cqh_index); 
void add_qhs(volatile uint32_t q_index, int q_amount);
void remove_qhs(volatile uint32_t q_index, volatile uint32_t q_amount);
void add_qts(volatile uint32_t q_index, volatile uint32_t qd_index, int qd_amount);

void remove_qts(volatile uint32_t q_index, volatile uint32_t qd_index, int qd_amount);
void free_all_qhs(int cqhp_temp); 
void print_all_qhsqts(volatile uint32_t cqhp_temp);

void print_qh_backpointers(volatile uint32_t cqhp_temp);
void print_qt_backpointers(volatile uint32_t cqhp_temp);


#endif
