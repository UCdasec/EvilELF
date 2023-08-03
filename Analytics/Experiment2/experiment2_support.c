#include <stdio.h>
#include <stdlib.h>
#include "../../ELFFileManipulator/elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//Uncomment out one of the sections below as needed to get the specific malware modification you wish to create
int main(int argc, char** argv){
    // if(argc < 4){
    //     printf("Need to specify two paths to different files and a value between 0-255\n");
    //     return 1;
    // }

    // long arg = strtol(argv[3], NULL, 10);
    // int loop_count = 1;
    // if(arg == -1){
    //     printf("Creating all 0-255 value versions\n");
    //     loop_count = 256;
    //     arg = 0;
    // }
    // else if(arg < 0 || arg > 255){
    //     printf("Value needs to be between 0-255 inclusive \n");
    //     return 2;
    // }

    Elf_Manager* malware = load_elf_file(argv[1]);
    Elf_Manager* benign = load_elf_file(argv[2]);

    char buffer[strlen(argv[1])+strlen(argv[2])+40];

    int text_section_index = get_next_section_index_by_name(benign,".text",0);
    Elf_Xword text_section_size = benign->s_hdr[text_section_index].sh_size;

    Elf_Xword added_size = 0;

    int size = get_file_name_size_from_path(malware->file_path);
    char base_malware_file[200];
    strcpy(base_malware_file, malware->file_path + strlen(malware->file_path)-size);
    

    size = get_file_name_size_from_path(benign->file_path);
    char base_benign_file[200];
    strcpy(base_benign_file, benign->file_path + strlen(benign->file_path)-size);

    //Start End Appendix (Append Section)
    // strcpy(buffer,base_malware_file);
    // strcat(buffer,"_append_benign_50k_");
    // strcat(buffer,base_benign_file);    

    // while(added_size < 50000){
    //     int new_section_index = append_new_section(malware, text_section_size);
    //     memcpy(malware->file_sections[new_section_index], benign->file_sections[text_section_index], text_section_size); 
    //     added_size = added_size + text_section_size;   
    // }
    // write_elf_file(malware, buffer);
    // free_manager(malware);
    //Finish End Appendix (Append Section)


    //Start Dynamic Extension (Extend Dynamic)
    strcpy(buffer,base_malware_file);
    strcat(buffer,"_extend_dynamic_benign_50k_");
    strcat(buffer,base_benign_file);

    int APPEND_AMOUNT = 50000;

    int original_dynamic_section_size = return_dynamic_size(malware);
    int dynamic_section_index = extend_dynamic_segment(malware, APPEND_AMOUNT);
    
    int left_to_add = APPEND_AMOUNT;
    int copy_amount = text_section_size;
    void* copy_location = malware->file_sections[dynamic_section_index] + original_dynamic_section_size;
    while(left_to_add > 0){
        if(left_to_add < text_section_size){
            copy_amount = left_to_add;
        }
        memcpy(copy_location, benign->file_sections[text_section_index], copy_amount); 
        left_to_add -= copy_amount;
        copy_location += copy_amount;
    }
    write_elf_file(malware, buffer);
    free_manager(malware);
    //Finish Dynamic Extension (Extend Dynamic)

    free_manager(benign);
    return 0;
}