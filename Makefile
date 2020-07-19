CC = g++
CPPFLAGS =-std=c++11

simulator: bin/main.o bin/instructions.o
	$(CC) $(CPPFLAGS) $^ -o bin/mips_simulator

bin_exists:
	mkdir -p bin

bin/main.o: src/main.cpp | bin_exists
	$(CC) $(CPPFLAGS) src/main.cpp -c -o bin/main.o

bin/instructions.o: src/instructions.cpp src/instructions.hpp | bin_exists
	$(CC) $(CPPFLAGS)  src/instructions.cpp -c -o bin/instructions.o


run: simulator
	./bin/mips_simulator

create_folder:
	mkdir -p mips_testbench/
	mkdir -p mips_testbench/binaries

clear:
	rm -rf bin
	rm -rf mips_testbench/bin/


testbench: create_folder
	cp src/testbench bin/mips_testbench
	chmod +x bin/mips_testbench
	chmod +x mips_testbench/binaries/*
