#!/bin/sh
for file in /home/reu/Documents/elfsample/ELFDataset/malware/*
do
  ./create_manipulated_files "$file" x86__64__lsb__unix-system-v__gcc-10.1.0__O0__no-obf__unstripped__coreutils-8.30__install -1
done
