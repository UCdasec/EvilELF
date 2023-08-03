#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf_Manager* manager = load_elf_file(argv[1]);
    int new_section_index = append_new_section(manager, 1024*1024);

    uint8_t* file_section = manager->file_sections[new_section_index];
    Elf_Xword section_size = manager->s_hdr[new_section_index].sh_size;

    for(int i = 0; i < section_size; i++){
        file_section[i] = 4;
    }


    write_elf_file(manager, argv[1]);
    free_manager(manager);
    return 0;
}