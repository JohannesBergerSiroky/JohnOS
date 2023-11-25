#ifndef FAT32_H
#define FAT32_H

/* bios parameter block for fat32 */
struct bios_parameter_block_fat32 {	
        /* executable code */
        volatile uint16_t jump1; 
        // executable code 
        volatile uint8_t jump2; 
        /* oem identityfinder */  
        volatile uint32_t oem1;
        volatile uint32_t oem2; 

        /* little endian format */
        volatile uint16_t bytes_per_sector; 
        volatile uint8_t sectors_per_cluster; 
        /* boot record sectors included */
        volatile uint16_t reserved_sectors; 
        /* usually 2 */
        volatile uint8_t fat_copies; 
        volatile uint16_t root_directory_entries; 
        /* if value = 0 then ts larger than 65355 and trhe real value is stored at large_total_sectors */
        volatile uint16_t total_sectors; 
        volatile uint8_t media_descriptor_type; 

        /* fat12/16 only */
        volatile uint16_t sectors_per_fat; 
        volatile uint16_t sectors_per_track; 
        /* number of heads */
        volatile uint16_t head_count;   
        /* number of hidden sectors */
        volatile uint32_t hidden_sectors_count;
        /* here's where the sector number are stored if larger than 65355 sectors */ 
        volatile uint32_t large_total_sectors; 
        /* offset 36 bytes */
        volatile uint8_t drive_number; 
        volatile uint8_t nt_flags;
        /* must be 0x28 or 0x29 */ 
        volatile uint8_t signature; 
        /* usually set to 2 */
        volatile uint32_t volume_id; 
        volatile uint8_t volume_label_string[11]; 
        volatile uint8_t system_identifier_string[8];
        volatile uint8_t boot_code[448]; 
        /* the 0x55aa */
        volatile uint16_t end_signature; 
} __attribute__((packed));



struct directory_entry {


        uint8_t filename[11]; 
        uint8_t attributes; 
        uint8_t unused_1;
        uint8_t unused_2;
        uint8_t unused_3;
        uint8_t unused_4;
        uint8_t unused_5;
        uint8_t unused_6;
        uint8_t unused_7;
        uint8_t unused_8;
        uint8_t unused_9;
        uint8_t unused_10;
        uint16_t time;
        uint16_t date;
        uint16_t file_cluster;
        /* size of the file in bytes */
        uint32_t file_size; 


} __attribute__((packed));


volatile uint16_t fat_bytes_per_sector;
volatile uint8_t fat_sectors_per_cluster;
volatile uint16_t fat_reserved_sectors;
volatile uint8_t fat_copies;
volatile uint16_t fat_sectors_per_fat;
volatile uint16_t fat_root_area;
volatile uint16_t fat_data_area;
volatile uint16_t fat_data_cluster_start;
volatile uint16_t maximum_root_directory_entries;
int8_t* temp_directory_ptr;
int8_t fs_flag;

uint32_t fs_j;
uint32_t fs_k;
uint32_t fs_l;
uint32_t fs_i;
uint8_t fs_string_match;

int8_t fs_current_directory[9];
int8_t kb_fs_current_directory[9];
int8_t* kb_cd_ptr;

int8_t* fs_current_directory_ptr;
int8_t fs_flag;
int8_t get_file[12];

int8_t fat16_temp_char1[5];
int8_t* fat16_temp_char1_ptr;

int8_t temp_directory[9];
int8_t* temp_directory_ptr;

int8_t *fs_directories;
int8_t *fs_filenames;
int8_t* fs_filenames_start;
int8_t* fs_filenames_dirinfo;
int8_t* fs_directory_info;

uint32_t* fs_directorylocation_cluster_ptr;
uint32_t* fs_filelocation_cluster_ptr;
uint32_t* fs_filelength;

void set_fs_clusters();
int8_t* fs_directory_ptr[16];
int8_t* fs_filename_ptr[32];

/* cluster size aligned */
uint32_t fs_cluster_counter; 
/* root directory entry counter, 32 bytes aligned */
uint16_t fs_rde_counter; 
/* sector size aligned */
uint8_t fs_fat_sector_counter; 

uint8_t no_more_directories; 
uint16_t rootfile_counter;
uint8_t first_find_directory;
uint32_t fs_cluster_counter;
/* the next 32 bytes in the line */
uint16_t fs_rde_counter; 
/* 9.68. read the 10th sector */
uint8_t fs_fat_sector_counter;
uint8_t have_read_file;
uint8_t have_read_dir;
uint16_t rootdir_counter2;

uint8_t have_read_first_dir;
uint8_t next_dir_after_file;
uint8_t fs_normal_procedure;
uint32_t fs_dir_cluster_counter;

uint32_t end_of_dir_sector;
uint8_t more_dir_in_dir;
uint16_t fs_dir_in_root_count; 

uint16_t scanned_dir_in_dir_count; 
uint16_t digged_dir_from_root; 
uint8_t are_in_root_flag;
void init_fs_variables();

uint32_t temp_dir_ptr_counter;
uint32_t fs_file_cluster_counter;
uint32_t fs_dir_counter_temp;

uint32_t rootdirinfo_counter;
uint32_t have_read_dir_counter;
uint32_t saved_dir_counter;

uint32_t get_bpb_sector();
void get_bpb_struct(uint32_t bpb_pos);

void read_fat16_file(uint32_t cluster_number, uint32_t f_size);
uint32_t* scan_directories(uint32_t sector_start);
void scan_all_directories();
void test_read_write();
void test_temp();


#endif
