#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>



int main(int argc, char** argv){
    if(argc < 3){
        printf("Need to specify a path to a file as an argument and a value between 0-255\n");
        return 1;
    }
    long arg = strtol(argv[2], NULL, 10);
    
    if(arg < 0 || arg > 255){
        printf("Value needs to be between 0-255 inclusive \n");
        return 2;
    }
    uint8_t value = arg;

    Elf_Manager* manager = load_elf_file(argv[1]);

    manager->e_hdr.e_flags = value;
    for(int i = 7; i < 16; i++){
        manager->e_hdr.e_ident[i] = value;
    }

    // int note_index = get_next_section_index_by_name(manager,".note",0);
    // if(note_index != -1){
    //     uint8_t* note_section = manager->file_sections[note_index];
    //     for(int i = 0; i < manager->s_hdr[note_index].sh_size;i++){
    //         note_section[i] = value;
    //     }
    // }

    

    write_elf_file(manager,argv[1]);

    int* gap_start;
    int* gap_size;
    int gap_count = 0;
    find_gaps_in_elf_file(manager, &gap_start,&gap_size,&gap_count,1);

    char* folder = "ModifiedElfOutput/"; 

    int size = strlen(manager->file_path);
    char output_path[19+size];
    strcpy(output_path, folder);
    strcat(output_path, manager->file_path);


    FILE* fp = fopen(output_path, "r+b");
    if(fp == NULL){
        printf("Output path was unable to be opened to fill gaps\n");
        return 0;
    }

    for(int i = 0; i < gap_count; i++){
        fseek(fp,gap_start[i], SEEK_SET);
        for(int j = 0; j < gap_size[i]; j++){
            uint8_t k = value;
            fwrite(&k,1,1,fp);
        }
    }

    fclose(fp);

    free(gap_size);
    free(gap_start);

    free_manager(manager);

    return 0;
}