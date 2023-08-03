#include "elf_support.h"
#include <stdio.h>

int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf_Manager* manager = load_elf_file(argv[1]);
    printf("Loaded file: %s\n",manager->file_path);

    print_elf_header_table_overview(manager);

    print_all_elf_program_header(manager);
    print_all_elf_section_header(manager);

    int index = get_next_section_index_by_name(manager, ".text",0);
    if(index != -1){
        print_elf_section_header(manager,index);
    }

    write_elf_file(manager, argv[1]);
    free_manager(manager);
    printf("File should be unchanged\n");
    return 0;
}