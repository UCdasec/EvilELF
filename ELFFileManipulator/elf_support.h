#ifndef ELF_SUPPORT_H
#define ELF_SUPPORT_H
#include <elf.h>
#include <stdio.h>

#ifndef IS_32_Bit
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Word Elf_Word;
typedef Elf64_Sword Elf_Sword;
typedef Elf64_Xword Elf_Xword;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Off Elf_Off;

#else

typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Word Elf_Word;
typedef Elf32_Sword Elf_Sword;
typedef Elf32_Xword Elf_Xword;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Off Elf_Off;

#endif

typedef struct elf_manager {
    Elf_Ehdr e_hdr;
    Elf_Phdr* p_hdr;
    Elf_Shdr* s_hdr;
    char file_path[4096];
    uint8_t** file_sections;
} Elf_Manager;

Elf_Manager* initialize_manager(int num_phdr, int num_shdr);

void print_elf_header_table_overview(Elf_Manager* manager);

//gap_start and gap_size will have their size realloacted based on the number of gaps, need to free them
void find_gaps_in_elf_file(Elf_Manager* manager, int** gap_start, int** gap_size, int* gap_count, int display);

void free_manager(Elf_Manager* manager);

int append_new_section(Elf_Manager* manager, int section_size);

int get_next_section_index_by_name(Elf_Manager* manager, char* name, int index);

int get_next_program_index_by_type(Elf_Manager* manager, int type, int index);

void print_elf_program_header(Elf_Manager* manager, int index);

void print_all_elf_program_header(Elf_Manager* manager);

void print_elf_section_header(Elf_Manager* manager, int index);

void print_all_elf_section_header(Elf_Manager* manager);

void get_program_type(char* string, uint32_t value);

void get_program_flags(char* string, uint32_t value);
void get_section_flags(char* string, uint32_t value);

void get_section_type(char* string, uint32_t value);

int cp(const char *to, const char *from);

Elf_Manager* load_elf_file(char* file_path);

int get_file_name_size_from_path(char* file_path);

void section_in_segment(Elf_Manager* manager, Elf_Phdr segment, FILE* fp);
void print_sections_in_segments(Elf_Manager* manager, FILE* fp);

struct seg_sect* segment_table(Elf_Manager* manager);
void free_seg_table(Elf_Manager* manager, struct seg_sect* seg_table);

int* find_note(Elf_Manager* manager);
int find_comment(Elf_Manager* manager);
int find_debug(Elf_Manager* manager);

int return_dynamic_size(Elf_Manager* manager);
int extend_dynamic_segment(Elf_Manager* manager, Elf_Xword ADDENDUM);

void change_elf_header(Elf_Manager* malware, uint8_t value, char* buffer, char* argv);
void append_value(Elf_Manager* malware, uint8_t value, char* buffer, char* argv);
void append_benign_x1(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, Elf_Xword text_section_size, char* buffer, char* argv);
void append_benign_x10(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, Elf_Xword text_section_size, char* buffer, char* argv);
void write_extended_dynamic(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, Elf_Xword text_section_size, char* buffer, char* argv);
void change_note_section(Elf_Manager* malware, Elf_Manager* benign, int text_section_index);
void change_comment_section(Elf_Manager* malware, Elf_Manager* benign, int text_section_index);
void change_debug_section(Elf_Manager* malware, Elf_Manager* benign, int text_section_index);
void change_note_comment_debug(Elf_Manager* malware, Elf_Manager* benign, int text_section_index, char* buffer, char* argv);

void write_elf_file(Elf_Manager* manager, char* file_path);
#endif