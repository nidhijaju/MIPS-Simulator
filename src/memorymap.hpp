#include <stdint.h>
int ADDR_REGISTER[32] = {0}; //Registers
char ADDR_INSTR[0x1000000] = {0}; //Executable memory. The Binary should be loaded here.
uint8_t ADDR_DATA[0x4000000] = {0}; //Read-write data area. Should be zero-initialised.

//char ADDR_NULL[0x4]; //Jumping to this address means the Binary has finished execution.
//char ADDR_GETC[0x4]; //Location of memory mapped input. Read-only.

/*
Offset     |  Length     | Name       | R | W | X |
-----------|-------------|------------|---|---|---|--------------------------------------------------------------------
0x00000000 |        0x4  | ADDR_NULL  |   |   | Y | Jumping to this address means the Binary has finished execution.
0x00000004 |  0xFFFFFFC  | ....       |   |   |   |
0x10000000 |  0x1000000  | ADDR_INSTR | Y |   | Y | Executable memory. The Binary should be loaded here.
0x11000000 |  0xF000000  | ....       |   |   |   |
0x20000000 |  0x4000000  | ADDR_DATA  | Y | Y |   | Read-write data area. Should be zero-initialised.
0x24000000 |  0xC000000  | ....       |   |   |   |
0x30000000 |        0x4  | ADDR_GETC  | Y |   |   | Location of memory mapped input. Read-only.
0x30000004 |        0x4  | ADDR_PUTC  |   | Y |   | Location of memory mapped output. Write-only.
0x30000008 | 0xCFFFFFF8  | ....       |   |   |   |
-----------|-------------|------------|---|---|---|--------------------------------------------------------------------

*/
