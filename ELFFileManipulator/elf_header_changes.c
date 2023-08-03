#include "elf_support.h"
#include <stdio.h>

int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf_Manager* manager = load_elf_file(argv[1]);

    manager->e_hdr.e_flags = 420042;

    for(int i = 7; i < 16; i++){
        manager->e_hdr.e_ident[i] = i;
    }
    


    write_elf_file(manager, argv[1]);
    free_manager(manager);
    return 0;
}