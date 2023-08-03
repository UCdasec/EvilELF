all: experiment2_support experiment2_support_constants

elf_support.o: ../../ELFFileManipulator/elf_support.c
	gcc -g -c elf_support.o ../../ELFFileManipulator/elf_support.c

experiment2_support.o: experiment2_support.c
	gcc -g -c experiment2_support.o experiment2_support.c

experiment2_support_constants.o: experiment2_support_constants.c
	gcc -g -c experiment2_support_constants.o experiment2_support_constants.c

experiment2_support: experiment2_support.o elf_support.o
	gcc -g -o experiment2_support experiment2_support.o elf_support.o

experiment2_support_constants: experiment2_support_constants.o elf_support.o
	gcc -g -o experiment2_support_constants experiment2_support_constants.o elf_support.o


clean:
	rm -f *.o experiment2_support experiment2_support_constants ModifiedElfOutput/*