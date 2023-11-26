/* This is the kernel's FAT16 driver. */

#include "../types.h"
#include "fat16.h"
#include "../device/device.h"
#include "../mem.h"
#include "../string.h"
#include "../graphics/screen.h"
#include "../device/usb/usb_mass_storage.h"
#include "../device/device.h"
#include "../device/usb/usb.h"


/* This array is unused. */
int8_t fs_dir_char[153] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

/* This array is unused. */
int8_t fs_file_char[297] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

/* Initializes the current dir char array. It is, of course, set
 * to root at first.
 */
int8_t fs_current_directory[9] = {'R', 'O', 'O', 'T', '\0', '\0', '\0', '\0', '\0'};

int8_t kb_fs_current_directory[9] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

int8_t get_file[12] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}; 

int8_t temp_directory[9] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

int8_t fat16_temp_char1[5] = {'\0', '\0', '\0', '\0', '\0'};

/* Initializes some some variables and memory regions. */
void set_fs_clusters()
{
        are_in_root_flag = 1;
        have_read_dir_counter = 0;
        saved_dir_counter = 0;
        rootfile_counter = 0;
        have_read_dir = 0;
        no_more_directories = 0;
        // Commented code: read_dirs = 0;
        have_read_file = 0;
        have_read_first_dir = 0;
        temp_dir_ptr_counter = 0;
        rootdir_counter2 = 0;
        rootdirinfo_counter = 1;
        fs_dir_cluster_counter = 0;
        fs_file_cluster_counter = 0;
        fs_dir_counter_temp = 0;
        fs_current_directory_ptr = fs_current_directory;
        /* Initializes fs_filenames where names of files and
         * directories are stored, I think.
         */
        fs_filenames = (int8_t*)kmem_4k_allocate();
        fs_filenames_start = (int8_t*)kmem_4k_allocate();
        /* Initializes fs_directories where names of directories
         * are stored. 
         */
        fs_directories = (int8_t*)kmem_4k_allocate();
        fs_filenames_dirinfo = (int8_t*)kmem_4k_allocate();
        fs_directory_info = (int8_t*)kmem_4k_allocate();
        zero_usbms_mem_7(fs_filenames);
        zero_usbms_mem_7(fs_filenames_start);
        zero_usbms_mem_7(fs_filenames_dirinfo);
        zero_usbms_mem_7(fs_directory_info);
        fs_filenames_start = fs_filenames;
        *fs_filenames = 'R';
        *(fs_filenames + 1) = 'O';
        *(fs_filenames + 2) = 'O';
        *(fs_filenames + 3) = 'T';
        *(fs_filenames + 4) = '\0';
        *fs_filenames_dirinfo = 'R';
        *(fs_filenames_dirinfo + 1) = 'O';
        *(fs_filenames_dirinfo + 2) = 'O';
        *(fs_filenames_dirinfo + 3) = 'T';
        *(fs_filenames_dirinfo + 4) = '\0';
        *(fs_filenames_dirinfo + 9) = (int8_t)0x25;
        *fs_directory_info = 'R';
        *(fs_directory_info + 1) = 'O';
        *(fs_directory_info + 2) = 'O';
        *(fs_directory_info + 3) = 'T';
        *(fs_directory_info + 4) = '\0';

        *fs_directories = 'R';
        *(fs_directories + 1) = 'O';
        *(fs_directories + 2) = 'O';
        *(fs_directories + 3) = 'T';
        *(fs_directories + 4) = '\0';

        fs_directorylocation_cluster_ptr = (uint32_t*)kmem_4k_allocate();
        fs_filelocation_cluster_ptr = (uint32_t*)kmem_4k_allocate();
        fs_filelength = (uint32_t*)kmem_4k_allocate();
        zero_usbms_mem_6(fs_directorylocation_cluster_ptr);
        zero_usbms_mem_6(fs_filelocation_cluster_ptr);
        zero_usbms_mem_6(fs_filelength);
        for(uint32_t a = 0; a < 512; a++) {
                (*(fs_filelength + a)) = (uint32_t)0x00000000;
        }
        kb_cd_ptr = kb_fs_current_directory;
        fat16_temp_char1_ptr = fat16_temp_char1;

        fs_k = 0;
        fs_l = 0;
        fs_j = 0;
        fs_i = 0;

        fs_string_match = 0;
        fs_cluster_counter = 0x9b0;
        /* the next 32 bytes in the line */
        fs_rde_counter = 13; 
        /* 9.68. read the 10th sector */
        fs_fat_sector_counter = 10;
        temp_directory_ptr = temp_directory;


        fs_flag = 0;
} 

/* Initializes some file system variables. */
void init_fs_variables()
{
        no_more_directories = 0; 
        first_find_directory = 1;
        have_read_file = 0;
        have_read_dir = 0;
        next_dir_after_file = 0;
        fs_normal_procedure = 1;
        fs_dir_cluster_counter = 0;
        end_of_dir_sector = 0;
        more_dir_in_dir = 0;
        fs_dir_in_root_count = 0;
        scanned_dir_in_dir_count = 0;
        digged_dir_from_root = 0;
        are_in_root_flag = 1;
}

/*  This function will use a driver to get the bios parameter block position
 * from the first sector on a storage device if it's using a master boot record.                 
 */
uint32_t get_bpb_sector() 
{
        struct device* dev = usbdev;
        uint8_t bpb_high = 0;
        uint8_t bpb_low = 0;
        uint32_t bpb = 0;
        volatile uint8_t* bpb_position = (volatile uint8_t*)0;
        asm("cli");
        if(dev->current_device_type == USB_MASS_STORAGE) {
                usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_LBA;
                usbms_extra_data[1] = 0;
                bpb_position = (volatile uint8_t*)prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ10, USB_DEVICE_TO_HOST, 0, 512, usbms_extra_data);
        }
        asm("sti");
        if(bpb_position) {
                bpb_high = (*(bpb_position + 455));
                bpb_low = (*(bpb_position + 454));
                bpb = 0 | ((bpb_high << 8) | bpb_low);
                asm("cli");
                return bpb;
        }
        else {
                print("\n\nError: Could not fetch the bpb position.\n");
                asm("cli");

        }
        return 0;

}

/* This function will use a driver to get the bios parameter 
 * block on a FAT partition. Then some of the values will be 
 * copied into variables.                 
 */
void get_bpb_struct(uint32_t bpb_pos)
{

        volatile uint8_t* get_bpb = (volatile uint8_t*)0;
        struct device* dev = usbdev;
        asm("cli");

        /* reading the bpb and putting it in a bpb struct */
        if(dev->current_device_type == USB_MASS_STORAGE) {
                usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_LBA;
                usbms_extra_data[1] = bpb_pos;
                get_bpb = (volatile uint8_t*)prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ10, USB_DEVICE_TO_HOST, 0, 512, usbms_extra_data);
        }
        asm("sti");


        if (get_bpb != ((volatile uint8_t*)0x600000)) {
                struct bios_parameter_block_fat32* bpb_struct = (struct bios_parameter_block_fat32*)kmem_4k_allocate();
                memset2((uint8_t*)bpb_struct, (uint8_t*)get_bpb, 512);
                maximum_root_directory_entries = bpb_struct->root_directory_entries;
                fat_bytes_per_sector = bpb_struct->bytes_per_sector;
                /* setting some global variables with data from the struct */
                fat_sectors_per_cluster = bpb_struct->sectors_per_cluster;
                fat_reserved_sectors = bpb_struct->reserved_sectors;
                fat_copies = bpb_struct->fat_copies;
                fat_sectors_per_fat = bpb_struct->sectors_per_fat; 

                /* in sectors */
                fat_root_area = (volatile uint16_t)(bpb_pos + fat_reserved_sectors + (fat_sectors_per_fat*fat_copies));
                /* print("\nfat root area, should be 0x84c: ");
                print_hex(fat_root_area); */

                /* in sectors. getting an important value in the fat_data_cluster_start: the base address from which clusters are counted */
                fat_data_cluster_start = fat_root_area + ((maximum_root_directory_entries*32) / fat_bytes_per_sector) - 8;

                asm("cli");

                /* returning the fat root area. not the base adress for clusters */
                return;
        }

        else {
                print("\n\nError: could not get the bpb pointer values");
                asm("cli");
                return;
        }

}

/* Reads a FAT16 file. */
void read_fat16_file(uint32_t cluster_number, uint32_t f_size)  
{

        volatile uint8_t* file_data_area;
        volatile uint32_t file_location_sector = 0;
        struct device* dvc = usbdev;
        dvc->current_device_type = USB_MASS_STORAGE;
        file_location_sector = (volatile uint32_t)((uint32_t)fat_data_cluster_start + (uint32_t)((uint32_t)fat_sectors_per_cluster*cluster_number));



        if (cluster_number < 0x3500) {
                if(dvc->current_device_type == USB_MASS_STORAGE) {
                        usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_LBA;
                        usbms_extra_data[1] = (uint32_t)file_location_sector;
                        file_data_area = (volatile uint8_t*)prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ10, USB_DEVICE_TO_HOST, 0, 512, usbms_extra_data);
                }
                f_size -=1;


                if(file_data_area != (volatile uint8_t*)0x600000) {

                        print("\n\nFile content: \"");
                        for(uint32_t a = 0; a < f_size; a++) {

                                if(((a % 76) == 0) && (a > 0))
                                        print("\n");
                                if((*(file_data_area + a)) > 127)
                                        print(" ");
                                else 
                                        printch((int8_t)(*(file_data_area + a)), 0);

                        }
                                print("\"");

                }

                else
                        print("\n\nfat 16 error: could not read the file content");

        }
        else
                print("\n\nerror reading from fat 16: Bad cluster number or file length");

}


/*  This function will get directories from a storage device and put it in RAM
 *  the kernel should initialize bpb struct before this. get_bpb_struct                          
 */                                                                                                
uint32_t* scan_directories(uint32_t cluster_start)
{
        /* suggestion: return 0 */
        volatile uint8_t* get_data_area = (volatile uint8_t*)kmem_4k_allocate();
        zero_usbms_mem_5((uint8_t*)get_data_area);


        uint8_t root_directory_end = 0;
        uint8_t sector_read = 0;

        uint32_t temp1 = 0;
        uint32_t temp2 = 0;
        uint32_t temp3 = 0;
        uint32_t temp4 = 0;
        uint32_t temp_dir_ptr_counter2 = 0;
        uint32_t temp_directory_ptr2 = 0;
        uint32_t fs_b = 0;

        struct device* dev = usbdev;


        volatile uint16_t d_entries = maximum_root_directory_entries;
        uint16_t directory_entries = (uint16_t)d_entries;

        if (directory_entries > 512)
                return 0; 

        uint32_t data_area;
        if (cluster_start == 0)
                data_area = fat_root_area;
        else
                data_area = fat_data_cluster_start + (fat_sectors_per_cluster*cluster_start);

        uint8_t dir_counter = 0;
        uint8_t dir_counter2 = 0;
        uint8_t rootdir_counter = 0;
        uint32_t fs_dir_cluster_counter2 = 0;
        uint32_t fs_file_cluster_counter2 = 0;
        uint32_t have_read_dir_counter2 = have_read_dir_counter;

        fs_dir_in_root_count = 0;

        uint8_t error_count = 0;

        /* scans every directory from the current directory and places the names of them in some containers in RAM memory */

        while(((root_directory_end < 4) && (directory_entries > 0)) && (error_count == 0) && (rootdir_counter2 < 455)) {

                /* checks if (while) there are less than 16 directories in the dir int8_t buffer or 
                 * less than a 3-4 32bytes area full with zeroes or no errors or directory entries 
                 * less than max (512) get a new data area and set root dir counter to 0 
                 */
		        rootdir_counter = 0;
                if(dev->current_device_type == USB_MASS_STORAGE) {
                        usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_LBA;
                        usbms_extra_data[1] = data_area;
                        get_data_area = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ10, USB_DEVICE_TO_HOST, 0, 512, usbms_extra_data);
                }
                /* at the root area now. maybe a directory is found? */			
		        if((get_data_area != (volatile uint8_t*)0x600000) && (get_data_area > (volatile uint8_t*)0x300000) && (get_data_area < (volatile uint8_t*)0x500000)) {
		                /* while there are correct addresses for the driver */

				        temp_directory_ptr2 = rootdir_counter2;
				        while((root_directory_end < 4) && (rootdir_counter < 16) && (rootdir_counter2 < 455)) {

					            /* while there are less than a 3-4 32bytes area full with zeroes and sector scan not completed (sector / 32) and the dir buffer is not filled */
						        if (((*(get_data_area)) != 0x00) && ((*(get_data_area)) != 0x20) && ((*(get_data_area)) != 0x2e) && (((((*(get_data_area + 11)) >> 5) & 0x01) == 0x01) || ((((*(get_data_area + 11)) >> 4) & 0x01) == 0x01))) {

								        if ((((*(get_data_area + 11)) >> 5) & 0x01) == 0x01)
										        temp2 += 32;
								        if ((((*(get_data_area + 11)) >> 4) & 0x01) == 0x01)
									            temp3 += 32;


								        if((((*(get_data_area + 11)) >> 4) & 0x01) == 0x01) {

                                                /* we found a directory
										         * point to the first dir that is read somehow and then increase the pointer as many times as there are dirs. when no more, you go into the first dir and into the dirst dir there. 
                                                 */
										        have_read_dir_counter++;

										        dir_counter2 = 8;
										        dir_counter = 0;
										        have_read_dir = 1;
										        rootdir_counter2++;

										        (*(fs_directorylocation_cluster_ptr + fs_dir_cluster_counter + fs_dir_cluster_counter2)) = (uint32_t)((uint32_t)0x00000000 | ((uint32_t)(((*(get_data_area + 26)) & 0x000000ff) | ((uint32_t)(((*(get_data_area + 27)) & 0x000000ff) << 8))))); 


										        fs_dir_cluster_counter2++;

										        while (dir_counter < 8) {
												        if((*(get_data_area + dir_counter)) == 0x20) {
														        if(dir_counter2 > 0)
															            dir_counter2--;

															        (*(fs_directories + ((rootdir_counter2)*9) + dir_counter)) = '\0';
												        }
												        dir_counter++;
										        }

										        dir_counter = 0;

										        while (((*(get_data_area + dir_counter)) != '\0') && (dir_counter < dir_counter2)) {

												        if((*(get_data_area + dir_counter)) > 127)
													        *(fs_directories + (rootdir_counter2 * 9) + dir_counter) = (int8_t)'\0';
												        else 
													        *(fs_directories + (rootdir_counter2 *9) + dir_counter) = (int8_t)(*(get_data_area + dir_counter));																		

													        
												        dir_counter++;
										        }
										        (*(fs_directories + (rootdir_counter2 *9) + dir_counter)) = (int8_t)'\0';

			        

								        }


								        else if ((((*(get_data_area + 11)) >> 5) & 0x01) == 0x01) { 
                                                /* we found a file */
										        rootfile_counter++;
										        have_read_file = 1;
										        (*(fs_filelocation_cluster_ptr + rootfile_counter))  = (uint32_t)  ((uint32_t)0x00000000 | ((uint32_t)(((*(get_data_area + 26)) & 0x000000ff))) | ((uint32_t)(((*(get_data_area + 27)) & 0x000000ff) << 8)));


										        (*(fs_filelength + rootfile_counter)) = (uint32_t)((uint32_t)0x00000000 | ((uint32_t)(((*(get_data_area + 28)) & 0x000000ff))) | ((uint32_t)(((*(get_data_area + 29)) & 0x000000ff) << 8)) | ((uint32_t)(((*(get_data_area + 30)) & 0x000000ff) << 16)) | ((uint32_t)(((*(get_data_area + 31)) & 0x000000ff) << 24)));


										        fs_file_cluster_counter2++;

										        dir_counter = 8;
									            /* truncate */
										        dir_counter2 = 0;
										        while (dir_counter2 < 8) {
												        if((*(get_data_area + dir_counter2)) == 0x20) {

														        if(dir_counter > 0) {
															            dir_counter--;
                                                                }

														        (*(fs_filenames + ((rootfile_counter)*10) + dir_counter2)) = '\0';
												        }
												        dir_counter2++;
										        }

										        dir_counter2 = 0;

										        while (((*(get_data_area + dir_counter2)) != '\0') && (dir_counter2 < dir_counter)) {
												        if((*(get_data_area + dir_counter2)) > 127)
													            (*(fs_filenames + ((rootfile_counter)*10) + dir_counter2)) = '\0'; 
												        else 
													            (*(fs_filenames + ((rootfile_counter)*10) + dir_counter2)) = (int8_t)(*(get_data_area + dir_counter2));

												        dir_counter2++;

										        }

										        (*(fs_filenames + ((rootfile_counter)*10) + dir_counter2)) = '\0';
										        for(uint32_t fs_o = 0; fs_o < 3; fs_o++)
											            (*(fat16_temp_char1_ptr + fs_o)) = (*(get_data_area + 8 + fs_o));

										        (*(fat16_temp_char1_ptr + 3)) = '\0';
										        (*(fat16_temp_char1_ptr + 4)) = '\0';
										        (*(fat16_temp_char1_ptr + 5)) = '\0';

										        if(strEql((fat16_temp_char1_ptr), "TXT")) {

												        (*(fs_filenames + ((rootfile_counter)*10) + 8)) = 't';
												        (*(fs_filenames + ((rootfile_counter)*10) + 9)) = '\0';
										        }

										        dir_counter = 0;
								        }
									        

						        } 
			        

						        else if ((*(get_data_area)) == 0x00) {
                                        /* if it is a bad directory format */
								        root_directory_end++;
                                }

						        get_data_area = (uint8_t*)(get_data_area +  32);
						        rootdir_counter += 1;


				        } 

                            /* directory end (if going upwards) */
				        if ((root_directory_end > 3)) {

                            /*  end of sector or root end counter > 4 */
						        if(have_read_dir_counter != have_read_dir_counter2) {
                                     /* suggestion: maybe make an array here for keeping track of differences */
							        saved_dir_counter = saved_dir_counter + (have_read_dir_counter - have_read_dir_counter2);
                                    /* keeps it in track */
                                }
						        else if (saved_dir_counter != 0)
							            saved_dir_counter--;


						        sector_read = 0;
						        fs_dir_counter_temp++;
						        uint32_t fs_m = 0;
						        if (rootdir_counter2 > 454) {
								        no_more_directories = 1;
								        temp4 = 0x00000000;
								        temp4 = (uint32_t)get_data_area;
								        free_mem_uint(temp4);
								        return 0;
						        }

						        rootfile_counter++;
						        rootdirinfo_counter++;

						        if(have_read_dir == 1) {
							            have_read_dir_counter2++;
							            for(fs_b = have_read_dir_counter2; fs_b <= have_read_dir_counter; fs_b++) {
                                                /* write all found dirs into dir_info */
									            for(;fs_m < 8;fs_m++)
										                (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9) + fs_m)) = (*(fs_directories + ((fs_b)*9) + fs_m));
									            rootdirinfo_counter++;
									            fs_m = 0;
							            }

						        }

                                     /* now dirinfo is filled with read dirs. next step is to put the eod (end of directory) */
							        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9))) = (int8_t)'e';
							        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9) + 1)) = (int8_t)'o';
							        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9) + 2)) = (int8_t)'d';
							        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9) + 3)) = (int8_t)'\0';
							        temp_dir_ptr_counter2++;
							        rootdirinfo_counter++;

                                    /* now the eod has been written to the dir_info. next step is to write the first dir read after the eod. go to if have_read_dir=1 */
							        (*(fs_filenames + ((rootfile_counter)*10))) = (int8_t)'e';
							        (*(fs_filenames + ((rootfile_counter)*10) + 1)) = (int8_t)'o';
							        (*(fs_filenames + ((rootfile_counter)*10) + 2)) = (int8_t)'d';
							        (*(fs_filenames + ((rootfile_counter)*10) + 3)) = (int8_t)'\0';
							        (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
							        (*(fs_filelength + rootfile_counter)) = 0;
							        have_read_file = 0;

						        fs_dir_cluster_counter += fs_dir_cluster_counter2;
						        if((fs_file_cluster_counter + fs_file_cluster_counter2) < 455)
							            fs_file_cluster_counter += fs_file_cluster_counter2;

						        are_in_root_flag = 0;
						        have_read_first_dir = 1;
						        if(have_read_dir == 0) {

								        /* now the eod has been put at place in dir_info. what dir should now be put here? the next dir in the list but need to check if it exists first */
								        rootfile_counter++;
								        temp_dir_ptr_counter2 = temp_dir_ptr_counter;

								        while((!(*(fs_directories + ((temp_dir_ptr_counter2 - 1)*9))) && ((455 - (temp_dir_ptr_counter2 - 1)) > 0))) {
										        temp_dir_ptr_counter2++;
								        }
								        if(temp_dir_ptr_counter2 > 455) {
										        no_more_directories = 1;
										        /* done */
										        return 0;
								        }

								        
							        if (((455 - temp_dir_ptr_counter2) > 0)) {
									        temp_directory_ptr2++;

									        if((*(fs_directorylocation_cluster_ptr + temp_dir_ptr_counter)) == 0) {
											        no_more_directories = 1;
                                                    /* done */
											        return 0;
									        }

									        for(fs_b = 0; fs_b < 8; fs_b++) {

											        (*(fs_filenames + ((rootfile_counter)*10) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));
											        (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));

									        }

									        (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
									        (*(fs_filelength + rootfile_counter)) = 0;
									        rootdirinfo_counter++;
									        (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9))) = (int8_t)0x25;


									        return fs_directorylocation_cluster_ptr + temp_dir_ptr_counter;
							        }

							        else {
									        no_more_directories = 1;
									        temp4 = 0x00000000;
									        temp4 = (uint32_t)get_data_area;
									        free_mem_uint(temp4);
									        return 0;
							        }


						        }
						         /* you should now have read a directory. go to the next + (temp_dir_ptr_counter - 1) */
						        if(have_read_dir == 1) {

								        rootfile_counter++;
								        for(fs_b = 0; fs_b < 8; fs_b++) {

										        (*(fs_filenames + ((rootfile_counter*10)) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp) *9) + fs_b)); //boot for example. so if so then save "boot" in a global int8_t file.
										        (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));

								        }

								        (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
								        (*(fs_filelength + rootfile_counter)) = 0;
								        rootdirinfo_counter++;
								        (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9))) = (int8_t)0x25;

								        have_read_dir = 0;

								        temp4 = 0x00000000;
								        temp4 = (uint32_t)get_data_area;
								        free_mem_uint(temp4);

								        return fs_directorylocation_cluster_ptr + temp_dir_ptr_counter; 
						        }

						        else {
							            temp4 = 0x00000000;
							            temp4 = (uint32_t)get_data_area;
							            free_mem_uint(temp4);
							            return 0;
						        }
				        }

				        if (rootdir_counter2 > 454) {
						        no_more_directories = 1;
						        temp4 = 0x00000000;
						        temp4 = (uint32_t)get_data_area;
						        free_mem_uint(temp4);
						        return 0;
				        }
		        } 
                /* if the driver read valid data or not */
		        else
			            error_count = 1;

		        if(error_count == 0) {
				        directory_entries -= 1;
				        if (rootdir_counter2 > 454) {
						        no_more_directories = 1;
						        temp4 = 0x00000000;
						        temp4 = (uint32_t)get_data_area;
						        free_mem_uint(temp4);
						        return 0;
				        }
		        }
		        else {
				        print("\n\nDriver error: Could not process data");
				        no_more_directories = 1;
				        temp1 = 0x00000000;
				        temp1 = (uint32_t)get_data_area;
				        free_mem_uint(temp1);
				        error_count = 0;
				        return 0;
		        } 

		        if (rootdir_counter > 15) {

				        data_area++;
				        sector_read++;
				        dir_counter = 0;
				        dir_counter2 = 0;
		        }

		        if(sector_read == 4) {


				        sector_read = 0;
				        fs_dir_counter_temp++;
				        uint32_t fs_m = 0;
				        if (rootdir_counter2 > 454) {
						        no_more_directories = 1;
						        temp4 = 0x00000000;
						        temp4 = (uint32_t)get_data_area;
						        free_mem_uint(temp4);
						        return 0;
				        }
				        rootfile_counter++;
				        rootdirinfo_counter++; 
				        if(have_read_dir == 1) {
        					    have_read_dir_counter2++;
                                /* write all found dirs into dir_info */
					        for(fs_b = have_read_dir_counter2; fs_b <= have_read_dir_counter; fs_b++) {
							        for(;fs_m < 8;fs_m++)
								            (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9) + fs_m)) = (*(fs_directories + ((fs_b)*9) + fs_m));
							        rootdirinfo_counter++;
					        }

				        } 


					        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9))) = (int8_t)'e';
					        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9) + 1)) = (int8_t)'o';
					        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9) + 2)) = (int8_t)'d';
					        (*(fs_filenames_dirinfo + (rootdirinfo_counter*9) + 3)) = (int8_t)'\0';
					        temp_dir_ptr_counter2++;
					        rootdirinfo_counter++;


                            /* now the eod has been written to the dir_info. next step is to write the first dir read after the eod. go to if have_read_dir=1 */
					        (*(fs_filenames + ((rootfile_counter)*10))) = (int8_t)'e';
					        (*(fs_filenames + ((rootfile_counter)*10) + 1)) = (int8_t)'o';
					        (*(fs_filenames + ((rootfile_counter)*10) + 2)) = (int8_t)'d';
					        (*(fs_filenames + ((rootfile_counter)*10) + 3)) = (int8_t)'\0';
					        (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
					        (*(fs_filelength + rootfile_counter)) = 0;
					        have_read_file = 0;


				        fs_dir_cluster_counter += fs_dir_cluster_counter2;
				        if((fs_file_cluster_counter + fs_file_cluster_counter2) < 455)
					            fs_file_cluster_counter += fs_file_cluster_counter2;

				        are_in_root_flag = 0;
				        have_read_first_dir = 1;
				        if(have_read_dir == 0) {

						        rootfile_counter++;
						        temp_dir_ptr_counter2 = temp_dir_ptr_counter;

						        while((!(*(fs_directories + ((temp_dir_ptr_counter2 - 1)*9))) && ((455 - (temp_dir_ptr_counter2 - 1)) > 0)))
							            temp_dir_ptr_counter2++;
						        if(temp_dir_ptr_counter2 > 455) {
								        no_more_directories = 1;
                                        /* done */
								        return 0;
						        }

								        
						        if (((455 - temp_dir_ptr_counter2) > 0)) {
								        temp_directory_ptr2++;
                                        (*(fs_filenames_dirinfo + ((rootfile_counter)*9) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));
						        

						                if((*(fs_directorylocation_cluster_ptr + temp_dir_ptr_counter)) == 0) {
								                no_more_directories = 1;
                
								                return 0;
						                }

						                for(fs_b = 0; fs_b < 8; fs_b++) {

								                (*(fs_filenames + ((rootfile_counter)*10) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));
								                (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));
						                } 
						                (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
						                (*(fs_filelength + rootfile_counter)) = 0;

						                rootdirinfo_counter++;
						                (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9))) = (int8_t)0x25;
						                return fs_directorylocation_cluster_ptr + temp_dir_ptr_counter;

                                }

				        

						        else {
								        no_more_directories = 1;
								        temp4 = 0x00000000;
								        temp4 = (uint32_t)get_data_area;
								        free_mem_uint(temp4);
								        return 0;
						        }
                        }


						            
                            
				        if(have_read_dir == 1) {

						        rootfile_counter++;
						        for(fs_b = 0; fs_b < 8; fs_b++) {

								        (*(fs_filenames + ((rootfile_counter*10)) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp) *9) + fs_b)); 
								        (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*9) + fs_b)) = (*(fs_directories + ((fs_dir_counter_temp)*9) + fs_b));
						        }
						        (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
						        (*(fs_filelength + rootfile_counter)) = 0;

						        rootdirinfo_counter++;
						        (*(fs_filenames_dirinfo + ((rootdirinfo_counter)*10))) = (int8_t)0x25;
                                
						        have_read_dir = 0;

						        temp4 = 0x00000000;
						        temp4 = (uint32_t)get_data_area;
						        free_mem_uint(temp4);

						        return fs_directorylocation_cluster_ptr + temp_dir_ptr_counter;

				        }

				        else {
					            temp4 = 0x00000000;
					            temp4 = (uint32_t)get_data_area;
					            free_mem_uint(temp4);
					            return 0;
				        }

			            
	            } 
        }

        /* sectors end (if going upwards) */
	    if (rootdir_counter2 > 454) {
			    no_more_directories = 1;
			    temp4 = 0x00000000;
			    temp4 = (uint32_t)get_data_area;
			    free_mem_uint(temp4);
			    return 0;
	    }

	    (*(fs_directories + (rootdir_counter2*9))) = (int8_t)'e';
	    (*(fs_directories + (rootdir_counter2*9) + 1)) = (int8_t)'o';
	    (*(fs_directories + (rootdir_counter2*9) + 2)) = (int8_t)'d';
	    (*(fs_directories + (rootdir_counter2*9) + 3)) = (int8_t)'\0';

	    (*(fs_filenames + ((rootfile_counter + 1)*10))) = (int8_t)'e';
	    (*(fs_filenames + ((rootfile_counter + 1)*10) + 1)) = (int8_t)'o';
	    (*(fs_filenames + ((rootfile_counter + 1)*10) + 2)) = (int8_t)'d';
	    (*(fs_filenames + ((rootfile_counter + 1)*10) + 3)) = (int8_t)'\0';
	    (*(fs_filelocation_cluster_ptr + rootfile_counter)) = 0;
	    (*(fs_filelength + rootfile_counter)) = 0;
	    are_in_root_flag = 0;


	    temp1 = 0x00000000;
	    temp1 = (uint32_t)get_data_area;
	    free_mem_uint(temp1);

	    return 0;


}

/* Attempts to scan all directories in a FAT16 file system. */
void scan_all_directories()
{
	    uint32_t* fs_dir_cluster_ptr = (uint32_t*)0;
	    uint32_t fs_dir_cluster = 0;
	    while(no_more_directories == 0) {

			    fs_dir_cluster_ptr = scan_directories(fs_dir_cluster);
			    temp_dir_ptr_counter++;
			    if ((!fs_dir_cluster_ptr) && (no_more_directories == 0)) {

					    print("\n\nError reading directories");
					    return;
			    }
			    else if (no_more_directories == 1) {
                        fs_filenames = fs_filenames_start;
                        /* Commented code: 
                        print("\nthe directory names below\n\n");
                        for(int32_t a = 0; a < 1024;a++) {
                                printch((*(fs_directories + a)), 0);
                                if(((a % 64) == 0) && (a > 0)) {
                                        print("\n");

                                }
                        }
                        print("\nthe file names below\n\n");
                        for(int32_t a = 0; a < 1024;a++) {
                                printch((*(fs_filenames + a)), 0);
                                if(((a % 64) == 0) && (a > 0)) {
                                        print("\n");

                                }
                        }
                        print("\nthe file names dirinfo below\n\n");
                        for(int32_t a = 0; a < 1024;a++) {
                                printch((*(fs_filenames_dirinfo + a)), 0);
                                if(((a % 64) == 0) && (a > 0)) {
                                        print("\n");

                                }
                        } 
                        print("\nthe file clusters below\n\n");
                        for(int32_t a = 0; a < 64;a++) {
                                print_hex((*(fs_filelocation_cluster_ptr + a)));
                                print(" ");
                                if(((a % 14) == 0) && (a > 0)) {
                                        print("\n");

                                }
                        } */


					    return;
			    }
			    else
				    fs_dir_cluster = (*(fs_dir_cluster_ptr));
	    }

	    print("\n\nAll directories scanned or a driver error occured");

}

/* Tests a write operation on a specific cluster. */
void test_read_write()
{	
	    /* will read filea at a fixed location and then write file a on the next cluster after filej at a fixed location */
	    /* and then read filea again. */
	    print("hi");
	    uint32_t tmp1 = 0;
	    volatile uint8_t* tmp2 = (volatile uint8_t*)0;
	    volatile uint8_t* fs_transfer = (volatile uint8_t*)kmem_4k_allocate();
	    zero_usbms_mem_4(fs_transfer);
	    tmp2 = fs_transfer;

        for(uint32_t a = 0; a < 512; a+=4) {
		        (*(fs_transfer + a)) = 0x6a;
		        (*(fs_transfer + a + 1)) = 0x6f;
		        (*(fs_transfer + a + 2)) = 0x68;
		        (*(fs_transfer + a + 3)) = 0x61;
        }

        print("\n\nDriver:");

        uint8_t b = transfer_data_10_w(set_cbw_w_10_data(0x21, 0x2f), 0, 0, tmp2);
        if (b == 3) {
                usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_CTL;
                volatile uint8_t* c = prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_REQUEST_SENSE, USB_DEVICE_TO_HOST, 0, 0x12, usbms_extra_data);
		        if (c!= (volatile uint8_t*)0x600000) {
				        uint8_t d = transfer_data_10(set_cbw_10_data(0x21, 0x2f, 512), 0, 0);
				        if (d == 3)
					            print("\n\nWriting and reading data successfully completed.\n\n");
				        else
					            print("\n\nFailed to read the data which was written to the drive.\n\n");
		        }
		        else
				        print("\n\n Error during request sense operation.\n\n");


        }

        else
		        print("\n\n Error during write operation.\n\n");

        print(csptr);

        tmp1 = 0x00000000;
        tmp1 = (uint32_t)fs_transfer;
        free_mem_uint(tmp1);
}

/* Tests if the resulting string prints i386. */
void test_temp()
{
        struct device* dev = usbdev;
	    volatile uint8_t* get_data_area2 = (volatile uint8_t*)0;
	    uint8_t* get_data_area3 = (uint8_t*)0;
	    volatile uint8_t* get_data_area = (volatile uint8_t*)0;
        if(dev->current_device_type == USB_MASS_STORAGE) {
                usbms_extra_data[0] = USB_BULKSTORAGE_SCSI_LBA;
                usbms_extra_data[1] = 0x869;
                get_data_area = (volatile uint8_t*)prepare_usb_storage_bulk_transfer(USB_BULKSTORAGE_SCSI_READ10, USB_DEVICE_TO_HOST, 0, 512, usbms_extra_data);
        } 
	    get_data_area2 = get_data_area;
	    get_data_area3 = (uint8_t*)get_data_area2;

	    get_data_area3 +=160;

	    print("\n\nTesting if it can print i386: ");
	    for(uint32_t fs_g = 0;fs_g < 7; fs_g++) {
			    if((*(get_data_area3 + fs_g)) < 128)
				    printch((int8_t)(*(get_data_area3 + fs_g)), 0);
			    else if ((*(get_data_area3 + fs_g)) > 127)
				    print(" ");
	    }



}

