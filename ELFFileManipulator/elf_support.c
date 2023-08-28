#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

Elf_Manager* initialize_manager(int num_phdr, int num_shdr){
    Elf_Manager* manager = (Elf_Manager*) malloc(sizeof(Elf_Manager));
    manager->p_hdr = (Elf_Phdr*) malloc(sizeof(Elf_Phdr)* num_phdr); 
    manager->s_hdr = (Elf_Shdr*) malloc(sizeof(Elf_Shdr)* num_shdr); 
    manager->file_sections = (uint8_t**) malloc(sizeof(uint8_t*) * num_shdr);
    for(int i = 0; i < num_shdr;i++){
        manager->file_sections[i] = (uint8_t*) malloc(0);
    }
    return manager;
}


void free_manager(Elf_Manager* manager){
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        free(manager->file_sections[i]);
    }
    free(manager->file_sections);
    free(manager->p_hdr);
    free(manager->s_hdr);
    free(manager);
}

void load_elf_file_sections(Elf_Manager* manager){
    FILE* fp = fopen(manager->file_path, "r+b"); 
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        Elf_Off offset = manager->s_hdr[i].sh_offset;
        fseek(fp, offset, SEEK_SET);
        uint8_t* ptr = (uint8_t*) realloc(manager->file_sections[i],manager->s_hdr[i].sh_size);
        if(manager->s_hdr[i].sh_size == 0 && ptr == NULL && errno == ENOMEM){
            printf("Realloc failed because section %d is too large \n", i); // Could write to file instead to prevent this
            fclose(fp);
            return;
        }
        manager->file_sections[i] = ptr;
        fread(manager->file_sections[i], manager->s_hdr[i].sh_size, 1, fp);
    }
    fclose(fp);
}

void print_elf_header_table_overview(Elf_Manager* manager){
    Elf_Ehdr hdr = manager->e_hdr;
    printf("Number of program headers: %d\nNumber of section headers: %d\n",hdr.e_phnum,hdr.e_shnum);
    printf("Program Header Offset: %#lx, size %#x\n",hdr.e_phoff, hdr.e_phentsize*hdr.e_phnum);
    printf("Section Header Offset: %#lx, size %#x\n",hdr.e_shoff, hdr.e_shentsize*hdr.e_shnum);
}

//Currently takes an ELF file and parses it into a struct of elf headers and tables
Elf_Manager* load_elf_file(char* file_path){

    FILE* fp = fopen(file_path, "r+b");
    if(fp == NULL){
        printf("failed to load (likely invalid file path)\n");
        exit(1);
    }

    Elf_Ehdr hdr;
    if (1 != fread(&hdr, sizeof(hdr), 1, fp)){
        printf("failed to read elf header\n");
        exit(1);
    }

    Elf_Manager* manager = initialize_manager(hdr.e_phnum,hdr.e_shnum);
    strncpy(manager->file_path,file_path,4095);
    manager->file_path[4095] = '\0';
    memcpy(&(manager->e_hdr), &hdr, sizeof(Elf_Ehdr));

    fseek(fp,hdr.e_phoff ,SEEK_SET);
    
    for(int i = 0; i < hdr.e_phnum; i++){
        if(1 != fread(&(manager->p_hdr[i]), sizeof(Elf_Phdr), 1, fp)){
            printf("failed to read program header\n");
            exit(1);
        }
    }

    fseek(fp,hdr.e_shoff ,SEEK_SET);

    for(int i = 0; i < hdr.e_shnum; i++){
        if(1 != fread(&(manager->s_hdr[i]), sizeof(Elf_Shdr), 1, fp)){
            printf("failed to read section header\n");
            exit(1);
        }
    }


    fclose(fp);

    load_elf_file_sections(manager);

    return manager;
}

int get_file_name_size_from_path(char* file_path){
    int length = strlen(file_path);
    for(int i = length -1; i > 0; i--){
        if(file_path[i] == '/' || file_path[i] == '\\'){
            return length-i-1;
        }
    }
    return length;
}

void print_elf_program_header(Elf_Manager* manager, int index){
    Elf_Phdr phdr = manager->p_hdr[index];

    char buffer[1024];
    printf("Program Index %d\n",index);
    get_program_type(buffer, phdr.p_type);
    printf("p_type: %d %s\n", phdr.p_type, buffer);
    printf("p_offset: %#lx\n", phdr.p_offset);
    printf("p_vaddr: %#lx\n", phdr.p_vaddr);
    printf("p_paddr: %#lx\n", phdr.p_paddr);
    printf("p_filesz: %#lx\n", phdr.p_filesz);
    printf("p_memsz: %#lx\n", phdr.p_memsz);
    get_program_flags(buffer,phdr.p_flags);
    printf("p_flags: %x %s\n", phdr.p_flags,buffer);
    printf("p_align: %#lx\n\n", phdr.p_align);

}

void print_all_elf_program_header(Elf_Manager* manager){
    for(int i = 0; i < manager->e_hdr.e_phnum; i++){
        print_elf_program_header(manager,i);
    }
}

void print_elf_section_header(Elf_Manager* manager, int index){
    FILE* fp = fopen(manager->file_path, "r+b");
    Elf_Shdr shdr = manager->s_hdr[index];
    Elf_Off string_table_offset = manager->s_hdr[manager->e_hdr.e_shstrndx].sh_offset;
    fseek(fp, string_table_offset + shdr.sh_name,SEEK_SET);

    char buffer[1024];
    fgets(buffer, 1024, fp);
    printf("Section Index %d\n",index);
    printf("sh_name: (offset) %d (entry in string table) %s\n", shdr.sh_name,buffer);
    get_section_type(buffer,shdr.sh_type);
    printf("sh_type: %d %s\n", shdr.sh_type,buffer);
    get_section_flags(buffer,shdr.sh_type);
    printf("sh_flags: %#lx %s\n", shdr.sh_flags,buffer);
    printf("sh_addr: %#lx\n", shdr.sh_addr);
    printf("sh_off: %#lx\n", shdr.sh_offset);
    printf("sh_size: %#lx\n", shdr.sh_size);
    printf("sh_link: %#x\n", shdr.sh_link);
    printf("sh_info: %#x\n", shdr.sh_info);
    printf("sh_addralign: %#lx\n", shdr.sh_addralign);
    printf("sh_entsize: %#lx\n\n", shdr.sh_entsize);
    fclose(fp);
}

void print_all_elf_section_header(Elf_Manager* manager){
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        print_elf_section_header(manager,i);
    }
}

void get_program_type(char* string, Elf_Word value){
    switch (value) {
        case 0: strcpy(string, "PT_NULL"); break;
        case 1: strcpy(string, "PT_LOAD"); break;
        case 2: strcpy(string, "PT_DYNAMIC"); break;
        case 3: strcpy(string, "PT_INTERP"); break;
        case 4: strcpy(string, "PT_NOTE"); break;
        case 5: strcpy(string, "PT_SHLIB"); break;
        case 6: strcpy(string, "PT_PHDR"); break;
        case 0x70000000: strcpy(string, "PT_LOPROC"); break;
        case 0x7fffffff: strcpy(string, "PT_HIPROC"); break;
        default: strcpy(string, "UNKNOWN"); break;
    }
}

void get_program_flags(char* string, Elf_Word value){
    string[0] = '\0';
    if ((value & 0x1) != 0) strcat(string, "PF_X,");
    if ((value & 0x2) != 0) strcat(string, "PF_W,");
    if ((value & 0x4) != 0) strcat(string, "PF_R,");
    if ((value & 0xf0000000) != 0) strcat(string, "PF_MASKPROC,");
    return;
}

void get_section_flags(char* string, Elf_Word value){
    string[0] = '\0';
    if ((value & 0x1) != 0) strcat(string, "SHF_WRITE,");
    if ((value & 0x2) != 0) strcat(string, "SHF_ALLOC,");
    if ((value & 0x4) != 0) strcat(string, "SHF_EXECINSTR,");
    if ((value & 0xf0000000) != 0) strcat(string, "SHF_MASKPROC,");
    return;
}

void get_section_type(char* string, Elf_Word value){
    switch (value) {
        case 0: strcpy(string, "SHT_NULL"); break;
        case 1: strcpy(string, "SHT_PROGBITS"); break;
        case 2: strcpy(string, "SHT_SYMTAB"); break;
        case 3: strcpy(string, "SHT_STRTAB"); break;
        case 4: strcpy(string, "SHT_RELA"); break;
        case 5: strcpy(string, "SHT_HASH"); break;
        case 6: strcpy(string, "SHT_DYNAMIC"); break;
        case 7: strcpy(string, "SHT_NOTE"); break;
        case 8: strcpy(string, "SHT_NOBITS"); break;
        case 9: strcpy(string, "SHT_REL"); break;
        case 10: strcpy(string, "SHT_SHLIB"); break;
        case 11: strcpy(string, "SHT_DYNSYM"); break;
        case 0x70000000: strcpy(string, "SHT_LOPROC"); break;
        case 0x7fffffff: strcpy(string, "SHT_HIPROC"); break;
        case 0x80000000: strcpy(string, "SHT_LOUSER"); break;
        case 0xffffffff: strcpy(string, "SHT_HIUSER"); break;
        default: strcpy(string, "UNKNOWN"); break;
    }
}

//gets index of next occurence of section going by name starting at index
int get_next_section_index_by_name(Elf_Manager* manager, char* name, int index){
    int num_sections = manager->e_hdr.e_shnum;
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];

    for(int i = index; i < num_sections; i++){
        char* found = string_table_file_section + manager->s_hdr[i].sh_name;
        if (strcmp(name,found) == 0){
            return i;
        }
    }
    return -1;
}

//gets index of next occurence of program of type starting at index
int get_next_program_index_by_type(Elf_Manager* manager, int type, int index){
    int num_sections = manager->e_hdr.e_shnum;
    for(int i = index; i < num_sections; i++){
        if (type == manager->p_hdr[i].p_type){
            return i;
        }
    }
    return -1;
}



//Appends new section of section_size to manager, updates ELF header e_shnum, Section table, and file_sections
int append_new_section(Elf_Manager* manager, int section_size){
    int final_section = 0;
    int final_offset = 0;
    for(int i = 1; i < manager->e_hdr.e_shnum; i++){
        if(manager->s_hdr[i].sh_offset > final_offset){
            final_section = i;
            final_offset = manager->s_hdr[i].sh_offset;
        }
    }

    void* ptr = realloc(manager->s_hdr, sizeof(Elf_Shdr) * (manager->e_hdr.e_shnum + 1));
    if(ptr == NULL){
        printf("Realloc failed when adding section header entry\n");
        return -1;
    }
    manager->s_hdr = ptr;

    Elf_Shdr new_section; //Point to new last section
    memcpy(&new_section, &(manager->s_hdr[final_section]), sizeof(Elf_Shdr)); //Copy old last section into it
    manager->e_hdr.e_shnum += 1;
    new_section.sh_offset = (final_offset + new_section.sh_size + 7) & (-8); //Get next multiple of 8 at end of last section
    new_section.sh_size = section_size;  

    manager->e_hdr.e_shoff += new_section.sh_size;

    int new_section_index = manager->e_hdr.e_shnum-1;

    memcpy(&(manager->s_hdr[new_section_index]),&new_section,sizeof(Elf_Shdr));

    ptr = realloc(manager->file_sections, sizeof(uint8_t*) * manager->e_hdr.e_shnum);
    if(ptr == NULL){
        printf("Realloc failed when adding file section\n");
        return -1;
    }
    manager->file_sections = ptr;
    ptr = malloc(sizeof(uint8_t) * section_size);
    if(ptr == NULL){
        printf("Realloc failed when adding file section\n");
        return -1;
    }
    manager->file_sections[new_section_index] = ptr;
    return new_section_index;
}

//helper function for below function print_sections_in_segments
void section_in_segment(Elf_Manager* manager, Elf_Phdr segment, FILE* fp) {
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];
    for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
        Elf_Shdr section = manager->s_hdr[i];
        if (section.sh_addr >= segment.p_vaddr &&
            section.sh_addr + section.sh_size <= segment.p_vaddr + segment.p_memsz) {
            printf("%s ", (char*)(string_table_file_section + section.sh_name));
        }
    }
}

//prints all the sections that are in the virtual memory space of each segment
void print_sections_in_segments(Elf_Manager* manager, FILE* fp) {
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];
    for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
        printf("%i      ", i);
        Elf_Phdr segment = manager->p_hdr[i];
        section_in_segment(manager, segment, fp);
        printf("\n");
    }
}

//a struct for grouping segments and respective sections
struct seg_sect {
    Elf_Phdr segment;
    Elf_Shdr* sections;
    int section_count;
};

//creates an array of above struct that groups segments with sections
struct seg_sect* segment_table(Elf_Manager* manager) {
    struct seg_sect* seg_table = malloc(sizeof(struct seg_sect) * manager->e_hdr.e_phnum);
    if (seg_table == NULL) {
        printf("Insufficient memory during allocation\n");
        return NULL;
    }
    for (int j = 0; j < manager->e_hdr.e_phnum; j++) {
        Elf_Phdr segment = manager->p_hdr[j];
        seg_table[j].segment = manager->p_hdr[j];
        seg_table[j].sections = malloc(sizeof(Elf_Shdr));
        int counter = 1;
        for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
            Elf_Shdr section = manager->s_hdr[i];
            if (section.sh_addr >= segment.p_vaddr &&
                section.sh_addr + section.sh_size <= segment.p_vaddr + segment.p_memsz) {
                seg_table[j].sections[counter - 1] = section;
                counter++;
                void* ptr = realloc(seg_table[j].sections, sizeof(Elf_Shdr) * counter);
                if (ptr == NULL) {
                    printf("Insufficient memory during realloc\n");
                    for (int k = 0; k < j; k++) {
                        free(seg_table[k].sections);
                    }
                    free(seg_table);
                    return NULL;
                }
                seg_table[j].sections = ptr;
            }
        }
        seg_table[j].section_count = counter - 1;
    }
    return seg_table;
}

//frees the array of segment-section structs
void free_seg_table(Elf_Manager* manager, struct seg_sect* seg_table) {
    for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
        free(seg_table[i].sections);
    }
    free(seg_table);
}

//changes all note section's bytes with 4s
int* find_note(Elf_Manager* manager) {
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];
    int* indexes = malloc(sizeof(int));
    int counter = 1;
    for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
        Elf_Shdr section = manager->s_hdr[i];
        char* found = string_table_file_section + section.sh_name;
        if (strncmp(".note", found, 5) == 0) {
            // printf(".note: %lx\n", section.sh_offset);
            indexes[counter - 1] = i;
            counter++;
            void* ptr = realloc(indexes, sizeof(int) * counter);
                if (ptr == NULL) {
                    printf("Insufficient memory during realloc\n");
                    return NULL;
                }
            indexes = ptr;
        }
    }
    indexes[counter - 1] = -1; //creating terminator value at end of array
    return indexes;
}

//changes comment section's bytes with 4s
int find_comment(Elf_Manager* manager) {
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];
    for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
        Elf_Shdr section = manager->s_hdr[i];
        char* found = string_table_file_section + section.sh_name;
        if (strcmp(".comment", found) == 0) {
            // printf(".comment: %lx\n", section.sh_offset);
            return i;
        }
    }
    return -1;
}

//changes debug section's bytes with 4s
int find_debug(Elf_Manager* manager) {
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];
    for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
        Elf_Shdr section = manager->s_hdr[i];
        char* found = string_table_file_section + section.sh_name;
        if (strcmp(".debug", found) == 0) {
            // printf(".debug: %lx\n", section.sh_offset);
            return i;
        }
    }
    return -1;
}

int return_dynamic_size(Elf_Manager* manager) {
    int cutoff;
    int file_size;
    for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
        Elf_Phdr segment = manager->p_hdr[i];
        if (segment.p_type == 2) {
            cutoff = segment.p_offset;
            file_size = segment.p_filesz;
            return file_size;
        }
    }
}

int extend_dynamic_segment(Elf_Manager* manager, Elf_Xword ADDENDUM) {
    int cutoff;
    int file_size;
    for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
        Elf_Phdr segment = manager->p_hdr[i];
        if (segment.p_type == 2) { //will need to change this for PT_LOAD segment
            cutoff = segment.p_offset;
            file_size = segment.p_filesz;
            manager->p_hdr[i].p_filesz += ADDENDUM;
            break;
        }
    }
    int dynam_section;
    for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
        Elf_Shdr section = manager->s_hdr[i];
        if (section.sh_offset >= cutoff && section.sh_offset < cutoff + file_size) {
            dynam_section = i;
        }
    }

    void* ptr = realloc(manager->file_sections[dynam_section], sizeof(uint8_t*) * (file_size + ADDENDUM));
    if (ptr == NULL) {
        printf("Insufficient memory during realloc\n");
        return -1;
    }
    manager->file_sections[dynam_section] = ptr;
    manager->s_hdr[dynam_section].sh_size += ADDENDUM;

    uint8_t* section = manager->file_sections[dynam_section];
    for (int i = file_size; i < file_size + ADDENDUM; i++) {
        section[i] = 4;
    }
    memcpy(&(manager->file_sections[dynam_section]), &section, sizeof(uint8_t*));

    for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
        Elf_Phdr segment = manager->p_hdr[i];
        if (segment.p_offset >= cutoff + file_size) {
            manager->p_hdr[i].p_offset += ADDENDUM;
        }
    }
    for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
        Elf_Shdr section = manager->s_hdr[i];
        if (section.sh_offset >= cutoff + file_size) {
            manager->s_hdr[i].sh_offset += ADDENDUM;
        }
    }
    manager->e_hdr.e_shoff += ADDENDUM;

    return dynam_section;
}

int section_offset_sort(const void *a, const void *b){
    Elf_Shdr x = *(Elf_Shdr *) a;
    Elf_Shdr y = *(Elf_Shdr *) b;
    //just x-y doesn't work due to difference in number of bytes of types
    if ((x.sh_offset - y.sh_offset) < 0) return -1;
    if (x.sh_offset == y.sh_offset) return 0;
    return 1;
}

void find_gaps_in_elf_file(Elf_Manager* manager, int** gap_start_final, int** gap_size_final, int* gap_count, int display){
    //Might have an SHT_NOBITS issue for finding gaps when SHT_NOBITS size is non zero as it shares the same offset as SHT_PROGBITS
    for(int i = 1; i < manager->e_hdr.e_shnum;i++){
        if(manager->s_hdr[i].sh_offset < manager->s_hdr[i-1].sh_offset){
            printf("Section in table are not ordered by offset, sorting them\n");
            qsort(manager->s_hdr, manager->e_hdr.e_shnum,sizeof(void*),section_offset_sort);
            break;
        }
    }

    int current = manager->e_hdr.e_phoff;
    int size = manager->e_hdr.e_phnum * manager->e_hdr.e_phentsize;

    int num_gaps = 0;

    int* gap_start = malloc(num_gaps*sizeof(int));
    int* gap_size = malloc(num_gaps*sizeof(int));


    if(current+size < manager->s_hdr[1].sh_offset && manager->s_hdr[1].sh_type != SHT_NULL){
        num_gaps++;
        gap_start = realloc(gap_start,num_gaps*sizeof(int));
        gap_size = realloc(gap_size,num_gaps*sizeof(int));
        gap_start[num_gaps-1] = current+size;
        gap_size[num_gaps-1] = manager->s_hdr[1].sh_offset - (current+size);
    }


    for(int i = 2; i < manager->e_hdr.e_shnum;i++){
        current = manager->s_hdr[i-1].sh_offset;
        size = manager->s_hdr[i-1].sh_size;
        if(current+size < manager->s_hdr[i].sh_offset && manager->s_hdr[i-1].sh_type != SHT_NULL){
            num_gaps++;
            gap_start = realloc(gap_start,num_gaps*sizeof(int));
            gap_size = realloc(gap_size,num_gaps*sizeof(int));
            gap_start[num_gaps-1] = current+size;
            gap_size[num_gaps-1] = manager->s_hdr[i].sh_offset - (current+size);
        }
    }

    current = manager->s_hdr[manager->e_hdr.e_shnum-1].sh_offset;
    size = manager->s_hdr[manager->e_hdr.e_shnum-1].sh_size;

    if(current+size < manager->e_hdr.e_shoff && manager->s_hdr[manager->e_hdr.e_shnum-1].sh_type != SHT_NULL){
        num_gaps++;
        gap_start = realloc(gap_start,num_gaps*sizeof(int));
        gap_size = realloc(gap_size,num_gaps*sizeof(int));
        gap_start[num_gaps-1] = current+size;
        gap_size[num_gaps-1] = manager->e_hdr.e_shoff - (current+size);
    }

    if(display){
        for(int i = 0; i < num_gaps; i++){
            printf("Gap: %x - %x\n",gap_start[i],gap_size[i]+gap_start[i]);
        }
    }

    *gap_count = num_gaps;
    *gap_start_final = gap_start;
    *gap_size_final = gap_size;
}

void change_elf_header(Elf_Manager* malware, uint8_t value, char* buffer, char* argv) {
    strcpy(buffer,argv);

    malware->e_hdr.e_flags = value;
    for(int i = 7; i < 16; i++){
        malware->e_hdr.e_ident[i] = value;
    }
    char buffer2[19];
    snprintf(buffer2,30,"_eflags_eident_%d",value);
    strcat(buffer,buffer2);
    write_elf_file(malware, buffer);
    free_manager(malware);
}

void append_value(Elf_Manager* malware, uint8_t value, char* buffer, char* argv) {
    strcpy(buffer,argv);

    int new_section_index = append_new_section(malware, 1024*1024);

    uint8_t* file_section = malware->file_sections[new_section_index];
    Elf_Xword section_size = malware->s_hdr[new_section_index].sh_size;

    for(int i = 0; i < section_size; i++){
        file_section[i] = value;
    }
    char buffer2[4];
    snprintf(buffer2,4, "%d",value);
    strcat(buffer,"_append_");
    strcat(buffer,buffer2);
    write_elf_file(malware, buffer);
    free_manager(malware);
}

void append_benign_x1(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, Elf_Xword text_section_size, char* buffer, char* argv) {
    strcpy(buffer,argv);

    int new_section_index = append_new_section(malware, text_section_size);
    memcpy(malware->file_sections[new_section_index], benign->file_sections[text_section_index], text_section_size);

    strcat(buffer,"_append_benign_x1");
    write_elf_file(malware, buffer);
    free_manager(malware);
}

void append_benign_x10(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, Elf_Xword text_section_size, char* buffer, char* argv) {
    strcpy(buffer,argv);

    for(int i = 0; i < 10; i++){
        int new_section_index = append_new_section(malware, text_section_size);
        memcpy(malware->file_sections[new_section_index],benign->file_sections[text_section_index], text_section_size);
    }

    strcat(buffer,"_append_benign_x10");
    write_elf_file(malware, buffer);
    free_manager(malware);
}   

void write_extended_dynamic(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, Elf_Xword text_section_size, char* buffer, char* argv) {
    strcpy(buffer,argv);

    int cut_text_section_size = text_section_size - (text_section_size % 4096); //for alignment purposes

    int dynamic_section_size = return_dynamic_size(malware);
    int dynamic_section_index = extend_dynamic_segment(malware, cut_text_section_size);

    memcpy(malware->file_sections[dynamic_section_index] + dynamic_section_size, benign->file_sections[text_section_index], cut_text_section_size);

    strcat(buffer,"_extend_dynamic");
    write_elf_file(malware, buffer);
    free_manager(malware);
}

void change_note_section(Elf_Manager* malware, Elf_Manager* benign, int text_section_index) {
    int* note_section_indexes = find_note(malware);
    int i = 0;
    if (note_section_indexes[0] == -1) {
        puts(".note not present");
        return;
    }
    while(note_section_indexes[i] != -1) {
        int current_index = note_section_indexes[i];
        memcpy(malware->file_sections[current_index], benign->file_sections[text_section_index], malware->s_hdr[current_index].sh_size);
        i++;
    }
}

void change_comment_section(Elf_Manager* malware, Elf_Manager* benign, int text_section_index) {
    int comment_section_index = find_comment(malware);
    if (comment_section_index != -1) {
        memcpy(malware->file_sections[comment_section_index], benign->file_sections[text_section_index], malware->s_hdr[comment_section_index].sh_size);
    }
    else {
        puts(".comment not present");
    }
}

void change_debug_section(Elf_Manager* malware, Elf_Manager* benign, int text_section_index) {
    int debug_section_index = find_debug(malware);
    if (debug_section_index != -1) {
        memcpy(malware->file_sections[debug_section_index], benign->file_sections[text_section_index], malware->s_hdr[debug_section_index].sh_size);
    }
    else {
        puts(".debug not present");
    }
}

void change_note_comment_debug(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, char* buffer, char* argv) {
    strcpy(buffer,argv);

    change_note_section(malware, benign, text_section_index);
    change_comment_section(malware, benign, text_section_index);
    change_debug_section(malware, benign, text_section_index);

    strcat(buffer,"_note_comment_debug");
    write_elf_file(malware, buffer);
    free_manager(malware);
}

void write_elf_file(Elf_Manager* manager, char* file_path){
    char* folder = "ModifiedElfOutput/"; 

    struct stat st = {0}; 
    if (stat("ModifiedElfOutput", &st) == -1) {//Creates base directory if needed
        mkdir("ModifiedElfOutput", S_IRWXU | S_IRWXG | S_IRWXO);
    }
    int size = get_file_name_size_from_path(file_path);
    char output_path[19+size];
    strcpy(output_path, folder);
    strcat(output_path, file_path+strlen(file_path)-size);

    // printf("made output_path %s\n",output_path);
    
    FILE* fp;

    fp = fopen(output_path,"w+b");
    if(fp == NULL){
        printf("Output path was unable to be created\n");
        return;
    }
    fchmod(fileno(fp), S_IRWXU | S_IRWXG | S_IRWXO);

    fwrite(&(manager->e_hdr), sizeof(Elf_Ehdr), 1, fp);

    fseek(fp, manager->e_hdr.e_phoff, SEEK_SET);
    for(int i = 0; i < manager->e_hdr.e_phnum; i++){
        fwrite(&(manager->p_hdr[i]), sizeof(Elf_Phdr),1,fp);
    }
    

    fseek(fp, manager->e_hdr.e_shoff, SEEK_SET);
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        fwrite(&(manager->s_hdr[i]), sizeof(Elf_Shdr),1,fp);
    }

    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        if(manager->s_hdr[i].sh_type == 8){// SHT_NOBITS
            continue;
        }
        Elf_Off offset = manager->s_hdr[i].sh_offset;
        Elf_Xword size = manager->s_hdr[i].sh_size;
        fseek(fp, offset, SEEK_SET);
        if(manager->file_sections[i] == NULL){
            continue;
        }
        fwrite(manager->file_sections[i], 1, size, fp);
    }

    fclose(fp);
}