all: experiment3_support_constants experiment3_support

elf_support.o: ../../ELFFileManipulator/elf_support.c
	gcc -g -c elf_support.o ../../ELFFileManipulator/elf_support.c

experiment3_support_constants.o: experiment3_support_constants.c
	gcc -g -c experiment3_support_constants.o experiment3_support_constants.c

experiment3_support.o: experiment3_support.c
	gcc -g -c experiment3_support.o experiment3_support.c

experiment3_support_constants: experiment3_support_constants.o elf_support.o
	gcc -g -o experiment3_support_constants experiment3_support_constants.o elf_support.o

experiment3_support: experiment3_support.o elf_support.o
	gcc -g -o experiment3_support experiment3_support.o elf_support.o

clean:
	rm -f *.o experiment3_support_constants ModifiedElfOutput/*