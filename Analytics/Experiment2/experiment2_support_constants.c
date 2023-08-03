#include <stdio.h>
#include <stdlib.h>
#include "../../ELFFileManipulator/elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


//Uncomment out one of the sections below as needed to get the specific malware modification you wish to create
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
    
    //Start End Appendix (Append Section)
    uint8_t value = 56;
    for(int i = 0; i < 200; i++){
        malware = load_elf_file(argv[1]);
        strcpy(buffer,base_malware_file);
        strcat(buffer,"_append__100k_");
        char buffer2[4];
        snprintf(buffer2,4, "%d",value);
        strcat(buffer,buffer2);

        int new_section_index = append_new_section(malware, 100000);

        uint8_t* file_section = malware->file_sections[new_section_index];
        Elf_Xword section_size = malware->s_hdr[new_section_index].sh_size;

        for(int i = 0; i < section_size; i++){
            file_section[i] = value;
        }
        write_elf_file(malware, buffer);
        free_manager(malware);
        value++;
    }
    //Finish End Appendix (Append Section)

    //Start Dynamic Extension (Extend Dynamic)
    // uint8_t value = 56;
    // for(int i = 0; i < 200; i++){
    //     malware = load_elf_file(argv[1]);
    //     strcpy(buffer,base_malware_file);
    //     strcat(buffer,"_extend_dynamic_50k_");
    //     char buffer2[4];
    //     snprintf(buffer2,4, "%d",value);
    //     strcat(buffer,buffer2);

    //     int original_dynamic_section_size = return_dynamic_size(malware);
    //     int dynamic_section_index = extend_dynamic_segment(malware, 50000);

    //     uint8_t* file_section = malware->file_sections[dynamic_section_index];
    //     Elf_Xword section_size = malware->s_hdr[dynamic_section_index].sh_size;

    //     for(int i = original_dynamic_section_size; i < section_size; i++){
    //         file_section[i] = value;
    //     }
    //     write_elf_file(malware, buffer);
    //     free_manager(malware);
    //     value++;
    // }
    //Finish Dynamic Extension(Extend Dynamic)

    //Start ELF Header Alteration
    // uint8_t value = 56;
    // for(int i = 0; i < 200; i++){
    //     malware = load_elf_file(argv[1]);
    //     strcpy(buffer,base_malware_file);
    //     strcat(buffer,"_header_");
    //     char buffer2[4];
    //     snprintf(buffer2,4, "%d",value);
    //     strcat(buffer,buffer2);

    //     malware->e_hdr.e_flags  = value + (value << 8) + (value << 16) + (value << 24);
        
    //     for(int i = 7; i < 16; i++){
    //         malware->e_hdr.e_ident[i] = value;
    //     }

    //     write_elf_file(malware, buffer);
    //     free_manager(malware);
    //     value++;
    // }
    //Finish ELF Header Alteration

    //Start Padding Alteration
    // uint8_t value = 56;
    // int* gap_start;
    // int* gap_size;
    // int gap_count = 0;
    // malware = load_elf_file(argv[1]);
    // find_gaps_in_elf_file(malware, &gap_start,&gap_size,&gap_count,0);
    // free_manager(malware);

    // for(int i = 0; i < 200; i++){
    //     malware = load_elf_file(argv[1]);
    //     strcpy(buffer,base_malware_file);
    //     strcat(buffer,"_padding_");
    //     char buffer2[4];
    //     snprintf(buffer2,4, "%d",value);
    //     strcat(buffer,buffer2);

    //     char* folder = "ModifiedElfOutput/"; 

    //     char output_path[19+strlen(buffer)];
    //     strcpy(output_path, folder);
    //     strcat(output_path, buffer);

    //     write_elf_file(malware, buffer);
    //     free_manager(malware);

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
    // free(gap_size);
    // free(gap_start);
    //Finish Padding Alteration

    //Start Debug Alteration (Section Manipulation)
    // uint8_t value = 56;
    // for(int k = 0; k < 200; k++){
    //     malware = load_elf_file(argv[1]);
    //     strcpy(buffer,base_malware_file);
    //     strcat(buffer,"_section_");
    //     char buffer2[4];
    //     snprintf(buffer2,4, "%d",value);
    //     strcat(buffer,buffer2);

    //     int* note_section_indexes = find_note(malware);
    //     int i = 0;
    //     while(note_section_indexes[i] != -1) {
    //         int current_index = note_section_indexes[i];
    //         uint8_t* active_section = malware->file_sections[current_index];
    //         for(int j = 0; j < malware->s_hdr[current_index].sh_size; j++){
    //             active_section[j] = value;
    //         }
    //         i++;
    //     }
    //     free(note_section_indexes);

    //     int comment_section_indexes = find_comment(malware);

    //     if(comment_section_indexes != -1) {
    //         int current_index = comment_section_indexes;
    //         uint8_t* active_section = malware->file_sections[current_index];
    //         for(int j = 0; j < malware->s_hdr[current_index].sh_size; j++){
    //             active_section[j] = value;
    //         }
    //     }

    //     int debug_section_indexes = find_debug(malware);
    //     if(debug_section_indexes != -1) {
    //         int current_index = debug_section_indexes;
    //         uint8_t* active_section = malware->file_sections[current_index];
    //         for(int j = 0; j < malware->s_hdr[current_index].sh_size; j++){
    //             active_section[j] = value;
    //         }
    //     }

    //     write_elf_file(malware, buffer);
    //     free_manager(malware);
    //     value++;
    // }
    //Finish Debug Alteration (Section Manipulation)



    return 0;
}