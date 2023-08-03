#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    //Nonfunctional
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }

    int MAGIC_NUMBER = 4096;

    Elf_Manager* manager = load_elf_file(argv[1]);

    printf("Page size: %d\n", getpagesize());

    // int phdr_index = get_next_program_index_by_type(manager, PT_PHDR, 0);

    int phnum = manager->e_hdr.e_phnum;

    int how_to_change_phentry[phnum];//0 dont change, 1 update vaddr, 2 needs to be split and update vaddr

    for(int i = 0; i < phnum; i++){
        Elf_Phdr phdr = manager->p_hdr[i];
        if (phdr.p_offset < manager->e_hdr.e_phoff + manager->e_hdr.e_phentsize*manager->e_hdr.e_phnum){
            char buffer[32];
            get_program_type(buffer,phdr.p_type);
            printf("Section %d of type %s begins before/during program header\n", i, buffer);
            if(phdr.p_offset + phdr.p_filesz >= manager->e_hdr.e_phoff + manager->e_hdr.e_phentsize*manager->e_hdr.e_phnum){
                printf("The above section also occurs after and is likely problematic\n\n");
                how_to_change_phentry[i] = 2;
            }
            else{
                how_to_change_phentry[i] = 0;
            }
        }else{
            how_to_change_phentry[i] = 1;
        }
        
    }

    

    //Not implemented efficiently at all, could be swapped to pointer to pointers, linked list, etc to be faster
    manager->e_hdr.e_shnum++;
    manager->e_hdr.e_shstrndx++;
    void* ptr = realloc(manager->s_hdr, sizeof(Elf_Shdr)*manager->e_hdr.e_shnum);
    if(ptr == NULL){
        printf("Realloc failed when adding new section\n");
        return 1;
    }
    manager->s_hdr = ptr;

    memmove(manager->s_hdr+2, manager->s_hdr+1, sizeof(Elf_Shdr) * (manager->e_hdr.e_shnum-2));


    //This isn't that bad as it's just pointers
    ptr = realloc(manager->file_sections, sizeof(uint8_t *) * manager->e_hdr.e_shnum);
    if(ptr == NULL){
        printf("Realloc failed when adding new file section\n");
        return 1;
    }
    manager->file_sections = ptr;

    memmove(manager->file_sections+2, manager->file_sections+1, sizeof(uint8_t *) * (manager->e_hdr.e_shnum-2));

    memcpy(manager->s_hdr+1, manager->s_hdr+2, sizeof(Elf_Shdr));

    Elf_Shdr* shdr = &(manager->s_hdr[1]);
    shdr->sh_size=MAGIC_NUMBER;

    Elf_Off inserted_offset = manager->s_hdr[1].sh_offset;
    
    // for(int i = 1; i < manager->e_hdr.e_shnum; i++){
    //     manager->s_hdr[i].sh_offset += sizeof(Elf_Phdr);
    // }

    // manager->e_hdr.e_shoff += sizeof(Elf_Phdr);

    for(int i = 2; i < manager->e_hdr.e_shnum;i++){
        manager->s_hdr[i].sh_offset += MAGIC_NUMBER;
    }

    manager->e_hdr.e_shoff += MAGIC_NUMBER;

    manager->file_sections[1] = malloc(MAGIC_NUMBER);

    for(int i = 0; i < MAGIC_NUMBER; i++){
        manager->file_sections[1][i] = 4;
    }


    int bump_by = 0;
    for(int i = 0; i < phnum; i++){
        int j = i + bump_by;
        if(how_to_change_phentry[i] == 1){
            manager->p_hdr[j].p_offset += MAGIC_NUMBER;
            // manager->p_hdr[j].p_vaddr += MAGIC_NUMBER;
            // manager->p_hdr[j].p_paddr += MAGIC_NUMBER; 
            // manager->p_hdr[j].p_vaddr = manager->p_hdr[j].p_offset; //likely bad
            // manager->p_hdr[j].p_paddr = manager->p_hdr[j].p_offset;
        }else if(how_to_change_phentry[i] == 2){
            if(manager->p_hdr[j].p_type == PT_LOAD){
                printf("Load being modified at %d\n",i);
                if (manager->p_hdr[j].p_memsz != manager->p_hdr[j].p_filesz
                    || manager->p_hdr[j].p_offset != manager->p_hdr[j].p_vaddr
                    || manager->p_hdr[j].p_offset != manager->p_hdr[j].p_paddr){
                    printf("Not sure what to do here yet\n");
                    return 1;
                }
                manager->e_hdr.e_phnum++;
                manager->p_hdr[0].p_filesz += sizeof(Elf_Phdr);
                manager->p_hdr[0].p_memsz += sizeof(Elf_Phdr);

                void* ptr = realloc(manager->p_hdr, sizeof(Elf_Phdr)*manager->e_hdr.e_phnum);
                if(ptr == NULL){
                    printf("Realloc failed when adding new section\n");
                    return 1;
                }
                manager->p_hdr = ptr;

                memmove(manager->p_hdr+j+1, manager->p_hdr+j, sizeof(Elf_Phdr) * (manager->e_hdr.e_phnum - j - 1));

                memcpy(manager->p_hdr+j, manager->p_hdr+j+1, sizeof(Elf_Phdr));

                for(int k = 1; k < manager->e_hdr.e_phnum; k++){
                    if(manager->p_hdr[k].p_offset > manager->p_hdr[0].p_offset){
                        manager->p_hdr[k].p_offset += sizeof(Elf_Phdr);
                        // manager->p_hdr[k].p_vaddr += sizeof(Elf_Phdr);//These two might be wrong (this and below)
                        // manager->p_hdr[k].p_paddr += sizeof(Elf_Phdr);
                        
                    }
                    
                }

                manager->e_hdr.e_shoff += sizeof(Elf_Phdr);
                for(int i = 1; i < manager->e_hdr.e_shnum; i++){
                    manager->s_hdr[i].sh_offset += sizeof(Elf_Phdr);
                }



                manager->p_hdr[j].p_filesz = inserted_offset - manager->p_hdr[j].p_offset;

                manager->p_hdr[j].p_memsz = manager->p_hdr[j].p_filesz;

                manager->p_hdr[j+1].p_filesz = manager->p_hdr[j+1].p_filesz - manager->p_hdr[j].p_filesz;
                manager->p_hdr[j+1].p_memsz = manager->p_hdr[j+1].p_filesz;
                manager->p_hdr[j+1].p_offset += MAGIC_NUMBER + manager->p_hdr[j].p_filesz;
                // manager->p_hdr[j+1].p_vaddr = manager->p_hdr[j+1].p_offset;
                // manager->p_hdr[j+1].p_paddr = manager->p_hdr[j+1].p_offset;

                manager->p_hdr[j+1].p_vaddr = manager->p_hdr[j].p_memsz;
                manager->p_hdr[j+1].p_paddr = manager->p_hdr[j].p_memsz;

                bump_by++;
            }
        }
    }

    print_all_elf_program_header(manager);

    write_elf_file(manager, argv[1]);
    free_manager(manager);
    return 0;
}