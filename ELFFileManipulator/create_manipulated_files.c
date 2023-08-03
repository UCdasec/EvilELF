#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    if(argc < 4){
        printf("Need to specify two paths to different files and a value between 0-255\n");
        return 1;
    }

    long arg = strtol(argv[3], NULL, 10);
    int loop_count = 1;
    if(arg == -1){
        printf("Creating all 0-255 value versions\n");
        loop_count = 256;
        arg = 0;
    }
    else if(arg < 0 || arg > 255){
        printf("Value needs to be between 0-255 inclusive \n");
        return 2;
    }

    Elf_Manager* malware = load_elf_file(argv[1]);
    Elf_Manager* benign = load_elf_file(argv[2]);

    uint8_t user_value = arg;
    int text_section_index = get_next_section_index_by_name(benign,".text",0);
    Elf_Xword text_section_size = benign->s_hdr[text_section_index].sh_size;

    char buffer[strlen(argv[1])+40];
    uint8_t user_loop_value = user_value;


    int* gap_start;
    int* gap_size;
    int gap_count = 0;
    find_gaps_in_elf_file(malware, &gap_start,&gap_size,&gap_count,0);

    for(int i = 0; i < loop_count; i++){
        
        //change e_flags and e_ident in ELF header
        change_elf_header(malware, user_loop_value, buffer, argv[1]);

        //inserting values to new section at end of malware file
        malware = load_elf_file(argv[1]);
        append_value(malware, user_loop_value, buffer, argv[1]);

        //changing the gaps between segments/sections
        malware = load_elf_file(argv[1]);
        

        char* folder = "ModifiedElfOutput/"; 

        int size = get_file_name_size_from_path(malware->file_path);
        char output_path[18+size+6+4];
        strcpy(output_path, folder);
        strcat(output_path, malware->file_path + strlen(malware->file_path)-size);
        strcat(output_path,"_gaps_");
        char buffer2[4];
        snprintf(buffer2,4,"%d",user_loop_value);
        strcat(output_path,buffer2);

        write_elf_file(malware, output_path+18);
        printf("%s\n",output_path);
        FILE* fp = fopen(output_path, "r+b");
        if(fp == NULL){
            printf("Output path was unable to be opened to fill gaps\n");
            return 0;
        }

        for(int i = 0; i < gap_count; i++){
            fseek(fp,gap_start[i], SEEK_SET);
            for(int j = 0; j < gap_size[i]; j++){
                uint8_t k = user_value;
                fwrite(&k,1,1,fp);
            }
        }

        fclose(fp);
        
        user_loop_value = user_loop_value + 1;
    }

    free(gap_size);
    free(gap_start);

    //inserting benign text section to new section at end of malware file
    malware = load_elf_file(argv[1]);
    append_benign_x1(malware, benign, text_section_index, text_section_size, buffer, argv[1]);

    //inserting benign text section ten times
    malware = load_elf_file(argv[1]);
    append_benign_x10(malware, benign, text_section_index, text_section_size, buffer, argv[1]);

    //extending dynamic segment and inserting benign text section
    malware = load_elf_file(argv[1]);
    write_extended_dynamic(malware, benign, text_section_index, text_section_size, buffer, argv[1]);

    //changing note, comment, debug sections if the exist
    malware = load_elf_file(argv[1]);
    change_note_comment_debug(malware, benign, text_section_index, buffer, argv[1]);


        

    free_manager(benign);
    return 0;
}