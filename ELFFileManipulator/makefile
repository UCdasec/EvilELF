all: elf_load_write_test add_elf_section example elf_remove_section_header elf_prepend_section create_manipulated_files elf_header_changes elf_fill_gaps elf_fill_with_value

elf_support.o: elf_support.c
	gcc -g -c elf_support.o elf_support.c

elf_load_write_test.o: elf_load_write_test.c
	gcc -g -c elf_load_write_test.o elf_load_write_test.c

add_elf_section.o: add_elf_section.c
	gcc -g -c add_elf_section.o add_elf_section.c

elf_prepend_section.o: elf_prepend_section.c
	gcc -g -c elf_prepend_section.o elf_prepend_section.c

elf_remove_section_header.o: elf_remove_section_header.c
	gcc -g -c elf_remove_section_header.o elf_remove_section_header.c

create_manipulated_files.o: create_manipulated_files.c
	gcc -g -c create_manipulated_files.o create_manipulated_files.c

elf_header_changes.o: elf_header_changes.c
	gcc -g -c elf_header_changes.o elf_header_changes.c

elf_fill_gaps.o: elf_fill_gaps.c
	gcc -g -c elf_fill_gaps.o elf_fill_gaps.c

elf_fill_with_value.o: elf_fill_with_value.c
	gcc -g -c elf_fill_with_value.o elf_fill_with_value.c

elf_fill_with_value: elf_fill_with_value.o elf_support.o
	gcc -g -o elf_fill_with_value elf_fill_with_value.o elf_support.o

elf_fill_gaps: elf_fill_gaps.o elf_support.o
	gcc -g -o elf_fill_gaps elf_fill_gaps.o elf_support.o

elf_header_changes: elf_header_changes.o elf_support.o
	gcc -g -o elf_header_changes elf_header_changes.o elf_support.o

create_manipulated_files: create_manipulated_files.o elf_support.o
	gcc -g -o create_manipulated_files create_manipulated_files.o elf_support.o

elf_load_write_test: elf_load_write_test.o elf_support.o
	gcc -g -o elf_load_write_test elf_load_write_test.o elf_support.o

elf_remove_section_header: elf_remove_section_header.o elf_support.o
	gcc -g -o elf_remove_section_header elf_remove_section_header.o elf_support.o

add_elf_section: add_elf_section.o elf_support.o
	gcc -g -o  add_elf_section add_elf_section.o elf_support.o

elf_prepend_section: elf_prepend_section.o elf_support.o
	gcc -g -o  elf_prepend_section elf_prepend_section.o elf_support.o

example: example.c
	gcc -o example example.c

clean:
	rm -f *.o add_elf_section elf_load_write_test elf_remove_section_header elf_prepend_section elf_header_changes elf_fill_gaps elf_fill_with_value create_manipulated_files example ModifiedElfOutput/*