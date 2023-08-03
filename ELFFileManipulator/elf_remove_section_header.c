#include "elf_support.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//testing removing the section header table
int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf_Manager* manager = load_elf_file(argv[1]);
    printf("Loaded file\n");

    print_all_elf_program_header(manager);
    print_all_elf_section_header(manager);

    write_elf_file(manager, argv[1]);

    free_manager(manager);

    return 0;
}