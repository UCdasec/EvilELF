#include <stdio.h>
#include <stdlib.h>
#include "../../ELFFileManipulator/elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify an ELF file to modify\n");
        return 1;
    }

    Elf_Manager* malware = load_elf_file(argv[1]);

    char buffer[strlen(argv[1])+40];

    Elf_Xword added_size = 0;


    int size = get_file_name_size_from_path(malware->file_path);
    char base_malware_file[200];
    strcpy(base_malware_file, malware->file_path + strlen(malware->file_path)-size);

    free_manager(malware);
    
    //Start Padding Alteration & End Appendix (Append Section)

    // uint8_t value = 56;
    // for(int i = 0; i < 200; i++){
    //     malware = load_elf_file(argv[1]);
    //     strcpy(buffer,base_malware_file);
    //     strcat(buffer,"_padding_append_200k_");
    //     char buffer2[4];
    //     snprintf(buffer2,4, "%d",value);
    //     strcat(buffer,buffer2);

    //     int new_section_index = append_new_section(malware, 200000);

    //     uint8_t* file_section = malware->file_sections[new_section_index];
    //     Elf_Xword section_size = malware->s_hdr[new_section_index].sh_size;

    //     for(int i = 0; i < section_size; i++){
    //         file_section[i] = value;
    //     }

    //     int* gap_start;
    //     int* gap_size;
    //     int gap_count = 0;
    //     find_gaps_in_elf_file(malware, &gap_start,&gap_size,&gap_count,0);

    //     write_elf_file(malware, buffer);
    //     free_manager(malware);

    //     char* folder = "ModifiedElfOutput/"; 

    //     char output_path[19+strlen(buffer)];
    //     strcpy(output_path, folder);
    //     strcat(output_path, buffer);

    //     FILE* fp = fopen(output_path, "r+b");
    //     if(fp == NULL){
    //         printf("Output path was unable to be opened to fill gaps\n");
    //         return 0;
    //     }

    //     for(int i = 0; i < gap_count; i++){
    //         fseek(fp,gap_start[i], SEEK_SET);
    //         for(int j = 0; j < gap_size[i]; j++){
    //             uint8_t k = value;
    //             fwrite(&k,1,1,fp);
    //         }
    //     }

    //     fclose(fp);

    //     value++;
    // }
    //Finish Padding Alteration & End Appendix (Append Section)

    //Starting Padding Alteration & Dynamic Extension (Extend Dynamic)
    uint8_t value = 56;
    for(int i = 0; i < 200; i++){
        malware = load_elf_file(argv[1]);
        strcpy(buffer,base_malware_file);
        strcat(buffer,"_padding_extend_dynamic_200k_");
        char buffer2[4];
        snprintf(buffer2,4, "%d",value);
        strcat(buffer,buffer2);

        int original_dynamic_section_size = return_dynamic_size(malware);
        int dynamic_section_index = extend_dynamic_segment(malware, 200000);

        uint8_t* file_section = malware->file_sections[dynamic_section_index];
        Elf_Xword section_size = malware->s_hdr[dynamic_section_index].sh_size;

        for(int i = original_dynamic_section_size; i < section_size; i++){
            file_section[i] = value;
        }

        int* gap_start;
        int* gap_size;
        int gap_count = 0;
        find_gaps_in_elf_file(malware, &gap_start,&gap_size,&gap_count,0);

        write_elf_file(malware, buffer);
        free_manager(malware);

        char* folder = "ModifiedElfOutput/"; 

        char output_path[19+strlen(buffer)];
        strcpy(output_path, folder);
        strcat(output_path, buffer);

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

        value++;
    }
    //Finish Padding Alteration & Dynamic Extension (Extend Dynamic)

    

    return 0;
}