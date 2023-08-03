#include "elf_support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf_Manager* manager = load_elf_file(argv[1]);
    printf("Loaded file\n");

    

    for(int i = 1; i < manager->e_hdr.e_shnum;i++){
        if(manager->s_hdr[i].sh_offset < manager->s_hdr[i-1].sh_offset){
            printf("Section in table are not ordered by offset, exiting\n");
            free_manager(manager);
            return 1;
        }
    }

    int current = manager->e_hdr.e_phoff;
    int size = manager->e_hdr.e_phnum * manager->e_hdr.e_phentsize;

    int gap_count = 0;

    int* gap_start = malloc(gap_count*sizeof(int));
    int* gap_size = malloc(gap_count*sizeof(int));


    if(current+size < manager->s_hdr[1].sh_offset){
        gap_count++;
        gap_start = realloc(gap_start,gap_count*sizeof(int));
        gap_size = realloc(gap_size,gap_count*sizeof(int));
        gap_start[gap_count-1] = current+size;
        gap_size[gap_count-1] = manager->s_hdr[1].sh_offset - (current+size);
    }


    for(int i = 2; i < manager->e_hdr.e_shnum;i++){
        current = manager->s_hdr[i-1].sh_offset;
        size = manager->s_hdr[i-1].sh_size;
        if(current+size < manager->s_hdr[i].sh_offset){
            gap_count++;
            gap_start = realloc(gap_start,gap_count*sizeof(int));
            gap_size = realloc(gap_size,gap_count*sizeof(int));
            gap_start[gap_count-1] = current+size;
            gap_size[gap_count-1] = manager->s_hdr[i].sh_offset - (current+size);
        }
    }

    current = manager->s_hdr[manager->e_hdr.e_shnum-1].sh_offset;
    size = manager->s_hdr[manager->e_hdr.e_shnum-1].sh_size;
    if(current+size < manager->e_hdr.e_shoff){
        gap_count++;
        gap_start = realloc(gap_start,gap_count*sizeof(int));
        gap_size = realloc(gap_size,gap_count*sizeof(int));
        gap_start[gap_count-1] = current+size;
        gap_size[gap_count-1] = manager->e_hdr.e_shoff - (current+size);
    }

    for(int i = 0; i < gap_count; i++){
        printf("Gap: %x - %x\n",gap_start[i],gap_size[i]+gap_start[i]);
    }




    write_elf_file(manager, argv[1]);

    char* folder = "ModifiedElfOutput/"; 

    size = strlen(manager->file_path);
    char output_path[19+size];
    strcpy(output_path, folder);
    strcat(output_path, manager->file_path);


    FILE* fp = fopen(output_path, "r+b");
    if(fp == NULL){
        printf("Output path was unable to be opened to fill gaps\n");
        return 1;
    }

    for(int i = 0; i< gap_count; i++){
        fseek(fp,gap_start[i],SEEK_SET);
        for(int j = 0; j < gap_size[i];j++){
            uint8_t k = 4;
            fwrite(&k,1,1,fp);
        }
    }

    fclose(fp);

    free(gap_size);
    free(gap_start);

    free_manager(manager);

    return 0;
}