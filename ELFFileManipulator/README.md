# Repository for Editing ELF Files

## By Andrew Kosikowski and Daniel Jun Cho

---

## Overview

This folder is for editing ELF files without altering any functionality in order to allow them to evade detection from end-to-end deep learning malware detectors. The code is written in C using existing elf.h as a basis for our implementations of modifying and reading ELF files. This code was only tested on x64 least significant bit endianness ELF files but should work on all ELF files.

---

## Import 
The elf_support.c and elf_support.h are the key files to import to modify existing Linux ELF binaries. Add 
```include "elf_support.h" ``` and follow the compilations in the MAKEFILE to make sure the elf_support.c is compiled with it into an object file.

---


## Basic Usage
See the example .c files for usage of some of the functions in how to manipulate Linux ELF files. In general, you will need to run the following
```Elf_Manager* manager = load_elf_file(argv[1]);```

```Insert code here to modify manager (ELF File loaded in memory) ```

```write_elf_file(manager,argv[1]);```

```Insert code here for additional modifications that require directly writing to the file```

```free_manager(manager);```

The load and free are for setting up the ELF file in memory, while write_elf_file will write the current modified ELF file in memory to a file specified by the second argument.

### General Breakdown of ELF_Support and the ELF_Manager Struct
The ELF Manager is a C struct that maps out the contents of an ELF file based on the reference specifications from the following :

https://refspecs.linuxfoundation.org/elf/elf.pdf

It contains a pointer to the ELF_Header struct, Section Header Table struct, Program Header table struct, as well as the file's sections and some basic information on the name and path of a file. The basic types and structs were provided by:

```#include <elf.h>```

https://man7.org/linux/man-pages/man5/elf.5.html

Almost all of our modifications modify these structs directly or the file sections loaded into memory to make it as easy as possible to add future modification methods that apply to every ELF file. One notable example is our find_gaps_in_elf_file() method, which finds the location of gaps in the file (content that is merely padding for memory/file alignment) that doesn't normally make up the content of the file. For modifications like this you can analyze the manager struct to get information about them, but you must modify the file directory afterwards (after write_elf_file()) in order to actually do anything with their information. 

We also support numerous helper methods for finding specific sections and segments by name and type, and printing other extra information about the flags and binary of specific parts of the file. 

---


## Helpful misc commands
Useful command for comparing hex in files:

cmp -l example ModifiedElfOutput/example | gawk '{printf "%08X %02X %02X\n", $1, strtonum(0$2), strtonum(0$3)}'


Valgrind because you are using C:

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./add_elf_section example
