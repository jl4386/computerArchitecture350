/*****************************************************************************/ 
/*                                VESP COMPUTER-3.1XK                         *
/*                              Author: A. Yavuz Oruc                         *       
/*                         Copyright  2000,2004, 2008-2010.                   * 
/*                                    Algoritix                               * 
/*                                All rights reserved.                        *
/*Copying and compiling this program for personal use is permitted. However,  *
/*no part of this program may be reproduced or distributed in any form or     *
/*by any means, or stored in a database or  retrieval system, without the     *
/*prior written permission of the Author and AlgoritiX Corporation.           *                                                           
/*Neither Algoritix nor the Author makes any direct or implied claims         *                                                            
/*that this program will work accurately and/or generate correct results,     *                                                            
/*In addition, they provide no warranties direct or implied that using        *                                                            
/*this program on any computer will be safe in so far as  guarding against    *
/*destroying and/or deleting any information, data, code, program or file     *    
/*that may be stored in the memory of user's computer.                        *
/*****************************************************************************/
                  
                  
/*      Please communicate all your questions and comments to                */     
/*                    A. Yavuz Oruc at yavuz@sabatech.com                    */

/*      This version fixes some bugs in Version3.0K and more reliable.       */
                  
/*--------------------------New---Boot vector---New---------------------------*/
/* Boot vector displays "vesp is booting...." while it is booting, and        */
/* "enter command" when it is done. Thereafter, it blinks a cursor following  */
/*  a ":" while waiting for a Vesp system command.                            */     
/* The following five commands are implemented in full:                       */
/* l: load, s: save, e: execute, r: read, w: write                            */
/*  The Vesp memory space is extended by a BOOTSTRAP[512]                     */
/*  and a HDISK[65536] memory. A virtual address register is added to handle  */
/*  memory references to the HDISK memory.                                    */
/*-------------------------------End Boot vector------------------------------*/
                  
                  
/*--------------------Changes and additions in this version-------------------*/
/*                                                                            */
/* This is the 37-instruction version of vesp                                 */
/* New instructions include extensions of instructions to new operands and    */
/* results. For example, add instruction is extended to include B and IX as   */
/* operands as results.                                                       */
/******************************************************************************/


/* -vesp programs which are entered at the console can be saved in a file.    */
/* Warning!!!! This program overwrites any data in the filename  which is     */
/* specified at the console in response to the prompt for saving a vesp       */
/* program.  Save at your own risk!!!!!                                       */
/*                  
/* This  computer program simulates the fetch,decode
/* and execute cycles of a hypothetical 16-bit computer, 
/* called VESP (_V_Ery _Simple _Processor)
/* Revised to reduce the address field to 12 bits. 2/13/03                        
/* VESP  has the following registers on which it executes 8 instructions:                   
/* A:     16 bits (Implicit) It refers to location 0 in  VESP's memory
/* B:     16 bits (Implicit) It refers to location 1 in  VESP's memory 
/* IX:    16 bits (Implicit) It refers to location 2 in  VESP's memory 
/* IR:    16 bits  
/* MAR:   12 bits  
/* PC:    12 bits
/* VAR:    4 bits (It specifies the block in vesp's memory for instructions with 12-bit address
/* Its instruction repertoire consists of the following instructions: (Legacy instructions)
/* ADA: Add B to A     Opcode: 0000  0000 A = A+B                          HexCode: 00
/* CMP: Compl          Opcode: 0001  ---- A = ~A                           HexCode: 1    
/* LDA: Load           Opcode: 0010  ---- M[IR[4:15] ] = M[MAR+1]          HexCode: 2
/* MOV: Move           Opcode: 0011  ---- M[IR[4:15] ] = M[M[MAR+1][3:15]] HexCode: 3  
/* JMP  Jump           Opcode: 0100  ---- PC = IR[4:15]                    HexCode: 4  
/* JEZ: Jump if A=0    Opcode: 0101  ---- If (A = 0)  PC = IR[4:15]        HexCode: 5   
/* JPS: Jump if A>0    Opcode: 0110  0000 If (A > 0)  PC = IR[4:15]        HexCode: 6  
/* HLT: Halt           Opcode: 0111  ---- reset = 1                        HexCode: 70 
/* INA: Increment      Opcode: 1000  0000 A = A + 1                        HexCode: 80
/* DEA: Decrement      Opcode: 1001  0000 A = A - 1                        HexCode  90
/* ANA: And A          Opcode: 1010  0000 A = A & B                        HexCode  A0
/* IOA: Or  A          Opcode: 1011  0000 A = A | B                        HexCode  B0
/* SLA: Shift left  A  Opcode: 1100  ---- A << 1                           HexCode  C0
/* SRA: Shift right A  Opcode: 1101  ---- A >> 1                           HexCode  D0
/* MXF  Move from w/IX Opcode: 1110  ---- M[IR[3:15]] = M[IX]; IX = IX + 1 HexCode: E
/* MXT  Move to   w/IX Opcode: 1111  ---- M[IX] = M[IR[3:15]]; IX = IX + 1 HexCode: F
/* ADB: Add A to B     Opcode: 0000  0001 B = A+B                          HexCode: 01
/* ADX: Add A to X     Opcode: 0000  0010 IX= A+IX                         HexCode: 02
/* SUB: Subtract B     Opcode: 0000  0011 A = A-B                          HexCode: 03
/* SUA: Subtract A     Opcode: 0000  0100 B = B-A                          HexCode: 04
/* SUX: Subtract IX    Opcode: 0000  0101 IX=IX-A                          HexCode: 05
/* JAZ: Jump if A=0    Opcode: 0111  0001 If (A==0)   PC = B               HexCode: 71  
/* JAP: Jump if A>0    Opcode: 0111  0010 If (A > 0)  PC = B               HexCode: 72  
/* JAN: Jump if A<0    Opcode: 0111  0011 If (A < 0)  PC = B               HexCode: 73 
/* JPZ: Jump if A>=0   Opcode: 0111  0100 If (A >= 0) PC = B               HexCode: 74 
/* JMZ: Jump if A<=0   Opcode: 0111  0101 If (A <= 0) PC = B               HexCode: 75  
/* JNZ: Jump if A!=0   Opcode: 0111  0110 If (A != 0) PC = B               HexCode: 76  
/* NOP: No-operation   Opcode: 0111  0111                                  HexCode: 77 
/* JMB: Jump via B     Opcode: 0111  1000                                  HexCode: 78 
/* INB: Increment B    Opcode: 1000  0001 B = B + 1                        HexCode: 81
/* INX: Increment IX   Opcode: 1000  0010 IX = IX + 1                      HexCode: 82
/* DEB: Decrement B    Opcode: 1001  0001 B = B - 1                        HexCode: 91
/* DEX: Decrement IX   Opcode: 1001  0010 IX = IX - 1                      HexCode: 92
/* ANB: And B          Opcode: 1010  0001 B = A & B                        HexCode  A1
/* IOB: Or  B          Opcode: 1011  0001 B = A | B                        HexCode  B1
/* SLB: Shift left  B  Opcode: 1100  0001 B << 1                           HexCode  C1
/* SRB: Shift right B  Opcode: 1101  0001 B >> 1                           HexCode  D1

                  
/*         Programs are entered and displayed in hex code. */

/*  Vesp Memory Map (Subject to Revision)
------------------------------------------------
Memory[0x000]--A                               |
Memory[0x001]--B                               |
Memory[0x002]--IX                              |
Memory[0x003]--VAR                             |
Memory[0x004]--SP                              |
Memory[0x005]--INT                             |
Memory[0x006]--G                               |
Memory[0x007]--H                               |
-Bootstrap code begins here                    |  Block  0
Memory[0x008]----cursor position -x            |
Memory[0x009]----cursor-position -y            |
Memory[0x00A]----pixel color (or ASCII code)   |
Memory[0x00B]----display mode                  |
Memory[0x00C]----pixel pattern(when not ASCII) |
Memory[0x00D]----space                         |
Memory[0x00E]----carriage return               |
Memory[0x00F]----temp                          | 
Memory[0x010]--Memory[0x01F] reserved          |
                                               |
Memory[0x020]----boot-vector begins here       |
                                               |
											   |
Memory[0x0FF]----boot-vector ends here         |
------------------------------------------------
Memory[0x100]------Keyboard driver and ASCII   |  
Memory[0x101]------tables are stored here      | 
.                                              |  
.                                              | 
.                                              |  Blocks 1,2 (Keyboard input driver-interrupt vectors)
Memory[0x2FF]                                  |  
------------------------------------------------
Memory[0x300]------Frame buffer (video) and    |  
Memory[0x301]------video drivers are           | 
Memory[0x302]------ stored here                | 
.                                              |  
.                                              |  16x64 screen = 2^10  (1024 locations)
.                                              |  Blocks 3,4,5,6 (Screen output)
                                               |  
Memory[0x6FF]------                            |  
-----------------------------------------------|
Memory[0x0700]------VESP Kernel Code           |  
Memory[0x0701]------OS routines and code       | 
Memory[0x09FF]------                           |  
.                                              |  Blocks 7,8,9 Operating System (Kernel)
.                                              |  Interrupt vectors
.                                              |  
------------------------------------------------  
                                               |  256 words
Memory[0x0A00]------User space -1              |                                              
                                               |  
                                               |                                                   
------------------------------------------------  
                                               | 256 words
Memory[0x0B00]------User space -2              |                                              
                                               |                                                    
                                               |   Block 9 through 15 User space 
------------------------------------------------  
                                               |  (application and user programs)
Memory[0x0C00]------User space -3              |                                              
                                               |                                                    
                                               |  
------------------------------------------------  
                                               |  
Memory[0x0D00]------User space -4              |                                              
                                               |                                                    
                                               |  
------------------------------------------------  
                                               |  
Memory[0x0E00]------User space -5              |                                              
                                               |                                                    
                                               |  
------------------------------------------------  
                                               |  
Memory[0x0F00]------User space -6              |                                              
                                               |                                                    
                                               |  
------------------------------------------------  
.
.
.
------------------------------------------------  
                                               |                                                    
Memory[0x1000-E000]------HDISK                 |                                              
                                               |                                                    
                                               |  
------------------------------------------------  

------------------------------------------------  
                                               |                                                    
Memory[0xFF00]------Bootstrap code             |                                              
                                               |                                                    
                                               |  
------------------------------------------------  


*/
                  
#include <iostream> 
#include <iomanip>
#include <stdio.h>
#include <limits.h> 
                  
void initialize(void); int readprogram(void);
void displayprogram(int progend); 
void displayregisters(void); void displaymemory(void);
void maincycle(int trace);        void fetch(int trace);
void decode(int trace);           void execute(void);
void bootstrap(void);        void boot(void);
void kernel (void);          char read_next_key(void);
void updatescreen(short x, short y); void delay(float t);                
//AYO: Define the registers of the architecture.
typedef struct 
{unsigned short   MAR,PC,IR,MDR,VAR;
 unsigned long clock;
 short   MEMORY[8192],S,C,F,Z,reset,add,cmp,lda,mov,jmp,jez,jps,jng,jpz,jnz,jap,jaz,nop,jmb,hlt,ina,dea,inb,deb,inx,dex,ana,anb,ioa,iob,sla,sra,slb,srb,mxf,mxt;
 short   BOOTSTRAP[512],SCREEN[16][64],HDISK[65536];
 short   newcommand, readcommand;
} architecture;  architecture   vesp;

using namespace std;
                  
int j=1;
                  
int main(void)
{//int address = 0,i = 0,action,action2,progend;
 initialize();//AYO:  create and copy the boot vector into vesp's memory. 
 bootstrap(); //Copy from bootstrap into vesp.MEMORY[020,....]
 return 1; 
}
                  
void initialize(void)
{vesp.PC = vesp.MEMORY[0] = vesp.MEMORY[1] = vesp.MEMORY[2] = vesp.MEMORY[3] = vesp.IR = 0;
 vesp.reset = 0; vesp.clock = 0;
 
//vesp's boot code begins at location 0x0020.
// When vesp starts, it first loads the boot code from vesp.Bootsrap into vesp.MEMORY[0x0020-..]
//It then carries out the following steps:
//1- Clear the screen
//2- Display vesp is booting...  on the screen
//3- Display kernel loading... on the screen 
//4- Load kernel into memory blocks 7 and 8
//5- Insert  delay 
//6- Display enter: on the screen
//7- Blink a cursor for command entry

// bootstrap code: initialize the screen variables and clear the screen                                 vesp.address
                                                                                                                     
 vesp.BOOTSTRAP[0]    =  0x2008;  // clear location 8   (cursor-x)        //020-021
 vesp.BOOTSTRAP[1]    =  0x0000;  
 vesp.BOOTSTRAP[2]    =  0x2009;  // clear location 9   (cursor-y)        //022-023
 vesp.BOOTSTRAP[3]    =  0x0000;  
 vesp.BOOTSTRAP[4]    =  0x200A;  // set   location A   (pixel color)     //024-025
 vesp.BOOTSTRAP[5]    =  0x000F;  
 vesp.BOOTSTRAP[6]    =  0x200B;  // set   display mode (ASCII)           //026-027
 vesp.BOOTSTRAP[7]    =  0x0000;  
 vesp.BOOTSTRAP[8]    =  0x200C;  // set   pixel symbol  ('+' sign)       //028-029
 vesp.BOOTSTRAP[9]    =  0x002B;  
 //Clear the frame buffer (insert white space; the frame buffer holds bitmap image of screen)
 // The screen has 16x64 = 1024 ascii characters
 vesp.BOOTSTRAP[0xA]  =  0x2002;  // load IX with 0x300 (768)-screen[0]   //02A-02B
 vesp.BOOTSTRAP[0xB]  =  0x0300;  
 vesp.BOOTSTRAP[0xC]  =  0x2000;  // load 0x400(1024) into register A(l.c)//02C-02D
 vesp.BOOTSTRAP[0xD]  =  0x0400;  
 vesp.BOOTSTRAP[0xE]  =  0x200D;  // load space into location 0x00D       //02E-02F
 vesp.BOOTSTRAP[0xF]  =  0x0020;  
 vesp.BOOTSTRAP[0x10] =  0x5034;  // if A is 0 then jump out of the loop  //030-031
 vesp.BOOTSTRAP[0x11] =  0xF00D;  // load space into the next framebuffer location 
 vesp.BOOTSTRAP[0x12] =  0x9000;  // decrement A                           //032
 vesp.BOOTSTRAP[0x13] =  0x4030;  // loop back to check if A is 0          //033

// insert a carriage return at the end of each line
 vesp.BOOTSTRAP[0x14] = 0x2002;   // load IX with 0x033F                   //034-035
 vesp.BOOTSTRAP[0x15] = 0x033F;  
 vesp.BOOTSTRAP[0x16] = 0x2000;   // load 0x0010 into A (A=16)             //036-037
 vesp.BOOTSTRAP[0x17] = 0x0010;  
 vesp.BOOTSTRAP[0x18] = 0x200E; 
 vesp.BOOTSTRAP[0x19] = '\n';     // load carriage ret into location 0x00E //038-039 
                                  // location for carriage return is E in vesp.MEMORY 
 vesp.BOOTSTRAP[0x1A] = 0x2001;   // load B with 0x03F = 63                //03A-03B
 vesp.BOOTSTRAP[0x1B] = 0x003F;                                                            
 vesp.BOOTSTRAP[0x1C] = 0x5049;   // if A is 0 then jump out of the loop   //03C-03D
 vesp.BOOTSTRAP[0x1D] = 0xF00E;   // load cr into the next framebuffer location 
 vesp.BOOTSTRAP[0x1E] = 0x9000;   // decrement A                           //03E
 vesp.BOOTSTRAP[0x1F] = 0x300F;   // save A into temp [0x00F]              //03F-040
 vesp.BOOTSTRAP[0x20] = 0x0000;   
 vesp.BOOTSTRAP[0x21] = 0x3000;   // copy IX into A                        //041-042
 vesp.BOOTSTRAP[0x22] = 0x0002;  
 vesp.BOOTSTRAP[0x23] = 0x0000;   // add                                   //043
 vesp.BOOTSTRAP[0x24] = 0x3002;   // save A into IX                        //044-045
 vesp.BOOTSTRAP[0x25] = 0x0000;  
 vesp.BOOTSTRAP[0x26] = 0x3000;   // restore A from temp                   //046-047 
 vesp.BOOTSTRAP[0x27] = 0x000F;                      
 vesp.BOOTSTRAP[0x28] = 0x403C;   // loop back to check if A is 0          //048
                                                                                                                   
 // write the message  "vesp is booting..."  ---line 300 to 33F (vesp...)                                                       
 vesp.BOOTSTRAP[0x29] = 0x2300; 
 vesp.BOOTSTRAP[0x2A] = 0x0076;  // write v                                //049-04A 
 vesp.BOOTSTRAP[0x2B] = 0x2301; 
 vesp.BOOTSTRAP[0x2C] = 0x0065;  // write e                                //04B-04C
 vesp.BOOTSTRAP[0x2D] = 0x2302; 
 vesp.BOOTSTRAP[0x2E] = 0x0073;  // write s                                //04D-04E 
 vesp.BOOTSTRAP[0x2F] = 0x2303; 
 vesp.BOOTSTRAP[0x30] = 0x0070;  // write p                                //04F-050
 vesp.BOOTSTRAP[0x31] = 0x2304; 
 vesp.BOOTSTRAP[0x32] = 0x0020;  // write ' '                              //051-052
 vesp.BOOTSTRAP[0x33] = 0x2305; 
 vesp.BOOTSTRAP[0x34] = 0x0069;  // write i                                //053-054
 vesp.BOOTSTRAP[0x35] = 0x2306; 
 vesp.BOOTSTRAP[0x36] = 0x0073;  // write s                                //055-056
 vesp.BOOTSTRAP[0x37] = 0x2307; 
 vesp.BOOTSTRAP[0x38] = 0x0020;  // write ' '                              //057-058
 vesp.BOOTSTRAP[0x39] = 0x2308; 
 vesp.BOOTSTRAP[0x3A] = 0x0062;  // write b                                //059-05A
 vesp.BOOTSTRAP[0x3B] = 0x2309; 
 vesp.BOOTSTRAP[0x3C] = 0x006F;  // write o                                //05B-05C
 vesp.BOOTSTRAP[0x3D] = 0x230A; 
 vesp.BOOTSTRAP[0x3E] = 0x006F;  // write o                                //05D-05E
 vesp.BOOTSTRAP[0x3F] = 0x230B; 
 vesp.BOOTSTRAP[0x40] = 0x0074;  // write t                                //05F-060
 vesp.BOOTSTRAP[0x41] = 0x230C; 
 vesp.BOOTSTRAP[0x42] = 0x0069;  // write i                                //061-062
 vesp.BOOTSTRAP[0x43] = 0x230D; 
 vesp.BOOTSTRAP[0x44] = 0x006E;  // write n                                //063-064
 vesp.BOOTSTRAP[0x45] = 0x230E; 
 vesp.BOOTSTRAP[0x46] = 0x0067;  // write g                                //065-066
 vesp.BOOTSTRAP[0x47] = 0x230F; 
 vesp.BOOTSTRAP[0x48] = 0x002E;  // write .                                //067-068
 vesp.BOOTSTRAP[0x49] = 0x2310; 
 vesp.BOOTSTRAP[0x4A] = 0x002E;  // write .                                //069-06A
 vesp.BOOTSTRAP[0x4B] = 0x2311; 
 vesp.BOOTSTRAP[0x4C] = 0x002E;  // write .                                //06B-06C
 
 // display the prompt: "loading kernel:"  -----line 340 to 37F (load...)
 vesp.BOOTSTRAP[0x4D] = 0x2340; 
 vesp.BOOTSTRAP[0x4E] = 0x006C; // write l                                 //06D-06E
 vesp.BOOTSTRAP[0x4F] = 0x2341; 
 vesp.BOOTSTRAP[0x50] = 0x006F; // write o                                 //06F-070
 vesp.BOOTSTRAP[0x51] = 0x2342; 
 vesp.BOOTSTRAP[0x52] = 0x0061; // write a                                 //071-072 
 vesp.BOOTSTRAP[0x53] = 0x2343; 
 vesp.BOOTSTRAP[0x54] = 0x0064; // write d                                 //073-074
 vesp.BOOTSTRAP[0x55] = 0x2344; 
 vesp.BOOTSTRAP[0x56] = 0x0069; // write i                                 //075-076
 vesp.BOOTSTRAP[0x57] = 0x2345; 
 vesp.BOOTSTRAP[0x58] = 0x006E; // write n                                 //077-078
 vesp.BOOTSTRAP[0x59] = 0x2346; 
 vesp.BOOTSTRAP[0x5A] = 0x0067; // write g                                 //079-07A
 vesp.BOOTSTRAP[0x5B] = 0x2347; 
 vesp.BOOTSTRAP[0x5C] = 0x0020; // write ' '                               //07B-07C
 vesp.BOOTSTRAP[0x5D] = 0x2348; 
 vesp.BOOTSTRAP[0x5E] = 0x006B; // write k                                 //07D-07E
 vesp.BOOTSTRAP[0x5F] = 0x2349; 
 vesp.BOOTSTRAP[0x60] = 0x0065; // write e                                 //07F-080
 vesp.BOOTSTRAP[0x61] = 0x234A; 
 vesp.BOOTSTRAP[0x62] = 0x0072; // write r                                 //081-082
 vesp.BOOTSTRAP[0x63] = 0x234B; 
 vesp.BOOTSTRAP[0x64] = 0x006E; // write n                                 //083-084
 vesp.BOOTSTRAP[0x65] = 0x234C; 
 vesp.BOOTSTRAP[0x66] = 0x0065; // write e                                 //085-086
 vesp.BOOTSTRAP[0x67] = 0x234D; 
 vesp.BOOTSTRAP[0x68] = 0x006C; // write l                                 //087-088
 vesp.BOOTSTRAP[0x69] = 0x234E; 
 vesp.BOOTSTRAP[0x6A] = 0x002E; // .                                       //089-08A
 vesp.BOOTSTRAP[0x6B] = 0x234F; 
 vesp.BOOTSTRAP[0x6C] = 0x002E; // .                                       //08B-08C
 vesp.BOOTSTRAP[0x6D] = 0x2350; 
 vesp.BOOTSTRAP[0x6E] = 0x002E; // .                                       //08D-08E
 vesp.BOOTSTRAP[0x6F] = 0x2351; 
 vesp.BOOTSTRAP[0x70] = 0x002E; // .                                       //08F-090

 // Load the kernel from vesp.HDISK[1000-12FF] into vesp.MEMORY[0700-09FF]
 vesp.BOOTSTRAP[0x71] = 0x2002; 
 vesp.BOOTSTRAP[0x72] = 0x1000;// Load 0x1000 into IX                     //091-092
 vesp.BOOTSTRAP[0x73] = 0x2007; 
 vesp.BOOTSTRAP[0x74] = 0x0700;// Load loc.7 with 0x0700                  //093-094
 vesp.BOOTSTRAP[0x75] = 0x2000; 
 vesp.BOOTSTRAP[0x76] = 0x0300;// Load 0x0300 into A(loop count)          //095-096
 vesp.BOOTSTRAP[0x77] = 0x50A4;// If A is 0 then jump out of the loop 
 vesp.BOOTSTRAP[0x78] = 0xE001;// Move the next instruction into B        //097-098
 vesp.BOOTSTRAP[0x79] = 0x3006; 
 vesp.BOOTSTRAP[0x7A] = 0x0002;// Save IX (the address in HDISK)          //099-09A
 vesp.BOOTSTRAP[0x7B] = 0x3002; 
 vesp.BOOTSTRAP[0x7C] = 0x0007;// Load address in kernel frame into IX    //09B-09C
 vesp.BOOTSTRAP[0x7D] = 0xF001;// move next instruction into kernel fr.   //09D
 vesp.BOOTSTRAP[0x7E] = 0x3007; 
 vesp.BOOTSTRAP[0x7F] = 0x0002;// Save IX(the address in kernel fr.)      //09E-09F
 vesp.BOOTSTRAP[0x80] = 0x3002; 
 vesp.BOOTSTRAP[0x81] = 0x0006;// Restore IX (the address in HDISK)       //0A0-0A1
 vesp.BOOTSTRAP[0x82] = 0x9000;// Decrement A                             //0A2
 vesp.BOOTSTRAP[0x83] = 0x4097;// Loop back to check if A is 0            //0A3
 vesp.BOOTSTRAP[0x84] = 0x4700;// jump to location 0x0700                 //0A4
 /*
 //sample code in core memory (to be removed when read and write are implemented.)
 //stored in 0x0A00--0x0A06 -- one of the user program frames in vesp.MEMORY
 
 vesp.MEMORY[0x0A00] = 0x2000;  vesp.MEMORY[0x0A01] = 0x000F;
 vesp.MEMORY[0x0A02] = 0x2001;  vesp.MEMORY[0x0A03] = 0x000F;
 vesp.MEMORY[0x0A04] = 0x0000;  vesp.MEMORY[0x0A05] = 0x7000;
 vesp.MEMORY[0x0A06] = 0xFFFF; //end of program

 //sample code in disk memory (to be removed when read and write are implemented.)
 //stored in 0x1300--0x1306 -- one of the user program frames in vesp.MEMORY

 vesp.HDISK[0x1300] = 0x2000;  vesp.HDISK[0x1301] = 0x000F;
 vesp.HDISK[0x1302] = 0x2001;  vesp.HDISK[0x1303] = 0x000F;
 vesp.HDISK[0x1304] = 0x0300;  vesp.HDISK[0x1305] = 0x7000;
 vesp.HDISK[0x1306] = 0xFFFF; //end of program

 */
 //jump to kernel once the kernel-code is allowed to run
  
 vesp.HDISK[0x1000] = 0x23C0;//-line 380 to 3BF (blank line) 3C0-3FF (enter:) 
 vesp.HDISK[0x1001] = 0x0065;//write e;     //0x0700-701
 vesp.HDISK[0x1002] = 0x23C1; 
 vesp.HDISK[0x1003] = 0x006E;//write n;     //0x0702-703
 vesp.HDISK[0x1004] = 0x23C2; 
 vesp.HDISK[0x1005] = 0x0074;//write t;     //0x0704-705
 vesp.HDISK[0x1006] = 0x23C3; 
 vesp.HDISK[0x1007] = 0x0065;//write e;     //0x0706-707
 vesp.HDISK[0x1008] = 0x23C4; 
 vesp.HDISK[0x1009] = 0x0072;//write r;     //0x0708-709
 vesp.HDISK[0x100A] = 0x23C5; 
 vesp.HDISK[0x100B] = 0x003A;//write :;     //0x070A-70B
 //This can be extended further

 //load address segments
 vesp.HDISK[0x1081] = 0x2400;//-----line 400 to 43F (d-address:) 
 vesp.HDISK[0x1082] = 0x0064;//write d;      //0x0781-782
 vesp.HDISK[0x1083] = 0x2401; 
 vesp.HDISK[0x1084] = 0x002D;//write -;      //0x0783-784
 vesp.HDISK[0x1085] = 0x2402; 
 vesp.HDISK[0x1086] = 0x0061;//write a;      //0x0785-786
 vesp.HDISK[0x1087] = 0x2403; 
 vesp.HDISK[0x1088] = 0x0064;//write d;      //0x0787-788
 vesp.HDISK[0x1089] = 0x2404; 
 vesp.HDISK[0x108A] = 0x0064;//write d;      //0x0789-78A
 vesp.HDISK[0x108B] = 0x2405; 
 vesp.HDISK[0x108C] = 0x0072;//write r;      //0x078B-78C
 vesp.HDISK[0x108D] = 0x2406; 
 vesp.HDISK[0x108E] = 0x0065;//write e;      //0x078D-78E
 vesp.HDISK[0x108F] = 0x2407; 
 vesp.HDISK[0x1090] = 0x0073;//write s;      //0x078F-790
 vesp.HDISK[0x1091] = 0x2408; 
 vesp.HDISK[0x1092] = 0x0073;//write s;      //0x0791-792
 vesp.HDISK[0x1093] = 0x2409; 
 vesp.HDISK[0x1094] = 0x003A;//write :;      //0x0793-794

 vesp.HDISK[0x1095] = 0x2440;//-----line 440 to 47F (m-address:) 
 vesp.HDISK[0x1096] = 0x006D;//write m;      //0x0795-796
 vesp.HDISK[0x1097] = 0x2441; 
 vesp.HDISK[0x1098] = 0x002D;//write -;      //0x0797-798
 vesp.HDISK[0x1099] = 0x2442; 
 vesp.HDISK[0x109A] = 0x0061;//write a;      //0x0799-79A
 vesp.HDISK[0x109B] = 0x2443; 
 vesp.HDISK[0x109C] = 0x0064;//write d;      //0x079B-79C
 vesp.HDISK[0x109D] = 0x2444; 
 vesp.HDISK[0x109E] = 0x0064;//write d;      //0x079D-79E
 vesp.HDISK[0x109F] = 0x2445;
 vesp.HDISK[0x10A0] = 0x0072;//write r;      //0x079F-7A0
 vesp.HDISK[0x10A1] = 0x2446; 
 vesp.HDISK[0x10A2] = 0x0065;//write e;      //0x07A1-7A2
 vesp.HDISK[0x10A3] = 0x2447; 
 vesp.HDISK[0x10A4] = 0x0073;//write s;      //0x07A3-7A4
 vesp.HDISK[0x10A5] = 0x2448; 
 vesp.HDISK[0x10A6] = 0x0073;//write s;      //0x07A5-7A6
 vesp.HDISK[0x10A7] = 0x2449; 
 vesp.HDISK[0x10A8] = 0x003A;//write :;      //0x07A7-7A8
 
 //load loop (works)
 //disk address is at 0x0010, memory address is at 0x0014
 vesp.HDISK[0x1100] = 0x3002; //initialize IX to disk addr. //0x0800-801 
 vesp.HDISK[0x1101] = 0x0010; 
 
 vesp.HDISK[0x1102] = 0x2001; //load FFFF into B    //0x0802-803  //end marker 
 vesp.HDISK[0x1103] = 0xFFFF; 
 vesp.HDISK[0x1104] = 0xE000; //load next instruction to A  //0x0804
 vesp.HDISK[0x1105] = 0x3018; //save it into temp-1 //0x0805-806 
 vesp.HDISK[0x1106] = 0x0000; 
 vesp.HDISK[0x1107] = 0x0300; //A=A-B               //0x0807

 vesp.HDISK[0x1108] = 0x2001; //set exit address    //0x0808-809 
 vesp.HDISK[0x1109] = 0x0815; 
 vesp.HDISK[0x110A] = 0x7100; //Jump if A = 0       //0x080A

 vesp.HDISK[0x110B] = 0x3010; //save  IX to temp-2 
 vesp.HDISK[0x110C] = 0x0002; //0x080B-80C  //0010 holds the current index to disk memory
 vesp.HDISK[0x110D] = 0x3002; //load  IX from 0x0014 
 vesp.HDISK[0x110E] = 0x0014; //0x080D-80E  //0014 holds the current index to main memory
 vesp.HDISK[0x110F] = 0xF018; //copy temp-1 to MEMORY[IX]   //0x080F
 vesp.HDISK[0x1110] = 0x3014;  
 vesp.HDISK[0x1111] = 0x0002; //store IX back into 0x0014   //0x0810-811
 vesp.HDISK[0x1112] = 0x3002;  
 vesp.HDISK[0x1113] = 0x0010; //restore IX from temp-2      //0x0812-813
 vesp.HDISK[0x1114] = 0x4802; //Jump back to the loop       //0x0814
 vesp.HDISK[0x1115] = 0x3002; //load  IX from 0x0014 
 vesp.HDISK[0x1116] = 0x0014; //0x0815-816  //0014 holds the current index to main memory
 vesp.HDISK[0x1117] = 0xF018; //copy temp-1 to MEMORY[IX]   //0x0817 
 vesp.HDISK[0x1118] = 0x7700; //No-operation-Hang                //0x0818          
 
 //save loop (works)

 //disk address is at 0x0014, memory address is at 0x0010
 vesp.HDISK[0x1120] = 0x3002;  
 vesp.HDISK[0x1121] = 0x0010; //initialize IX to mem. addr.//0x0820-821

 vesp.HDISK[0x1122] = 0x2001;  
 vesp.HDISK[0x1123] = 0xFFFF; //load FFFF into B           //0x0822-823
 vesp.HDISK[0x1124] = 0xE000; //load next instruction to A //0x0824
 vesp.HDISK[0x1125] = 0x3018;  
 vesp.HDISK[0x1126] = 0x0000; //save it into temp-1        //0x0825-826
 vesp.HDISK[0x1127] = 0x0300; //A=A-B                      //0x0827

 vesp.HDISK[0x1128] = 0x2001;  
 vesp.HDISK[0x1129] = 0x0835; //set exit address           //0x0828-829
 vesp.HDISK[0x112A] = 0x7100; //Jump if A = 0              //0x082A

 vesp.HDISK[0x112B] = 0x3010;  
 vesp.HDISK[0x112C] = 0x0002; //save  IX to temp-2         //0x082B-82C
 vesp.HDISK[0x112D] = 0x3002;  
 vesp.HDISK[0x112E] = 0x0014; //load  IX from 0x0014       //0x082D-82E
 vesp.HDISK[0x112F] = 0xF018; //copy temp-1 to MEMORY[IX]  //0x082F
 vesp.HDISK[0x1130] = 0x3014;  
 vesp.HDISK[0x1131] = 0x0002; //store IX back into 0x0014  //0x0830-831
 vesp.HDISK[0x1132] = 0x3002;  
 vesp.HDISK[0x1133] = 0x0010; //restore IX                 //0x0832-833
 vesp.HDISK[0x1134] = 0x4822; //Jump back to the loop      //0x0834
 vesp.HDISK[0x1135] = 0x3002;  
 vesp.HDISK[0x1136] = 0x0014; //load  IX from 0x0014       //0x0835-836
 vesp.HDISK[0x1137] = 0xF018; //copy temp-1 to MEMORY[IX]  //0x0837
 vesp.HDISK[0x1138] = 0x7700; //Hang       //0x0838   
      
 
 //execute loop (works)
 vesp.HDISK[0x1140] = 0x3001;  
 vesp.HDISK[0x1141] = 0x0010; //set B reg. to the addr. ent. by user //0x0840-841          
 vesp.HDISK[0x1142] = 0x7800; //jump to the 1st instruction in specified prog.//0x0842 
  
 //read loop (works)
 vesp.HDISK[0x1150] = 0x2480; 
 vesp.HDISK[0x1151] = 0x0065;     //write e;                //0x0850-851-----line 480 to 4BF (m-address:)
 vesp.HDISK[0x1152] = 0x2481; 
 vesp.HDISK[0x1153] = 0x006E;     //write n;                //0x0852-853
 vesp.HDISK[0x1154] = 0x2482; 
 vesp.HDISK[0x1155] = 0x0074;     //write t;                //0x0854-855
 vesp.HDISK[0x1156] = 0x2483; 
 vesp.HDISK[0x1157] = 0x0065;     //write e;                //0x0856-857
 vesp.HDISK[0x1158] = 0x2484; 
 vesp.HDISK[0x1159] = 0x0072;     //write r;                //0x0858-859
 vesp.HDISK[0x115A] = 0x2485; 
 vesp.HDISK[0x115B] = 0x0020;     //write ' ';              //0x085A-85B
 vesp.HDISK[0x115C] = 0x2486; 
 vesp.HDISK[0x115D] = 0x006E;     //write n;                //0x085C-85D
 vesp.HDISK[0x115E] = 0x2487; 
 vesp.HDISK[0x115F] = 0x0065;     //write e;                //0x085E-85F
 vesp.HDISK[0x1160] = 0x2488; 
 vesp.HDISK[0x1161] = 0x0078;     //write x;                //0x0860-861
 vesp.HDISK[0x1162] = 0x2489; 
 vesp.HDISK[0x1163] = 0x0074;     //write t;                //0x0862-863
 vesp.HDISK[0x1164] = 0x248A; 
 vesp.HDISK[0x1165] = 0x0020;     //write ' ';              //0x0864-865
 vesp.HDISK[0x1166] = 0x248B; 
 vesp.HDISK[0x1167] = 0x0063;     //write c;                //0x0866-867
 vesp.HDISK[0x1168] = 0x248C;
 vesp.HDISK[0x1169] = 0x006F;     //write o;                //0x0868-869
 vesp.HDISK[0x116A] = 0x248D; 
 vesp.HDISK[0x116B] = 0x0064;     //write d;                //0x086A-86B
 vesp.HDISK[0x116C] = 0x248E; 
 vesp.HDISK[0x116D] = 0x0065;     //write e;                //0x086C-86D
 vesp.HDISK[0x116E] = 0x248F; 
 vesp.HDISK[0x116F] = 0x003A;     //write :;                //0x086E-86F

//memory address is at 0x0010
 vesp.HDISK[0x1170] = 0x3002;  
 vesp.HDISK[0x1171] = 0x0010; //initialize IX to mem. addr.//0x0870-871

 vesp.HDISK[0x1172] = 0x2001;  
 vesp.HDISK[0x1173] = 0xFFFF;    //load FFFF into B        //0x0872-873
 vesp.HDISK[0x1174] = 0x3000;  
 vesp.HDISK[0x1175] = 0x0014;    //load next inst. to A    //0x0874-875
 vesp.HDISK[0x1176] = 0x3018;  
 vesp.HDISK[0x1177] = 0x0000;    //save it into temp-1     //0x0876-877
 vesp.HDISK[0x1178] = 0x0300;    //A=A-B                   //0x0878
 vesp.HDISK[0x1179] = 0x2001;  
 vesp.HDISK[0x117A] = 0x087E;    //set exit address        //0x0879-87A
 vesp.HDISK[0x117B] = 0x7100;    //Jump if A = 0           //0x087B

 vesp.HDISK[0x117C] = 0xF018;    //copy temp-1 to MEMORY[IX]   //0x087C
 vesp.HDISK[0x117D] = 0x4872;    //Jump back to the loop       //0x087D
 vesp.HDISK[0x117E] = 0xF018;    //copy temp-1 to MEMORY[IX]   //0x087E
 vesp.HDISK[0x117F] = 0x7700;    //No-operation                //0x087F        

 
 //write loop (works)
 vesp.HDISK[0x1190] = 0x2480; 
 vesp.HDISK[0x1191] = 0x0061;     //write a;                    //0x0890-891
 vesp.HDISK[0x1192] = 0x2481; 
 vesp.HDISK[0x1193] = 0x0064;     //write d;                    //0x0892-893
 vesp.HDISK[0x1194] = 0x2482; 
 vesp.HDISK[0x1195] = 0x0064;     //write d;                    //0x0894-895
 vesp.HDISK[0x1196] = 0x2483; 
 vesp.HDISK[0x1197] = 0x0072;     //write r;                    //0x0896-897
 vesp.HDISK[0x1198] = 0x2484; 
 vesp.HDISK[0x1199] = 0x0065;     //write e;                    //0x0898-899
 vesp.HDISK[0x119A] = 0x2485; 
 vesp.HDISK[0x119B] = 0x0073;     //write s;                    //0x089A-89B
 vesp.HDISK[0x119C] = 0x2486; 
 vesp.HDISK[0x119D] = 0x0073;     //write s;                    //0x089C-89D
 vesp.HDISK[0x119E] = 0x2487; 
 vesp.HDISK[0x119F] = 0x003A;     //write :;                    //0x089E-89F

 vesp.HDISK[0x11A0] = 0x3002;  
 vesp.HDISK[0x11A1] = 0x0010;    //initialize IX to memory add.//0x08A0-8A1
 vesp.HDISK[0x11A2] = 0x2001;  
 vesp.HDISK[0x11A3] = 0xFFFF;    //load FFFF into B            //0x08A2-8A3
 vesp.HDISK[0x11A4] = 0xE000;    //load next instruction to A  //0x08A4
 vesp.HDISK[0x11A5] = 0x3018; 
 vesp.HDISK[0x11A6] = 0x0000;    //save it into temp-1         //0x08A5-8A6
 vesp.HDISK[0x11A7] = 0x0300;    //A=A-B                       //0x08A7
 vesp.HDISK[0x11A8] = 0x2001;  
 vesp.HDISK[0x11A9] = 0x08AC;    //set exit address            //0x08A8-8A9
 vesp.HDISK[0x11AA] = 0x7100;    //Jump if A = 0               //0x08AA

 vesp.HDISK[0x11AB] = 0x48A2;    //Jump back to the loop       //0x08AB
 vesp.HDISK[0x11AC] = 0x7700;    //No-operation                //0x08AC        

 
}

void bootstrap(void)
{unsigned int address = 0;
 //copy the bootstrap code into vesp.MEMORY[0x020...] 
 for(address=0; address <= 0x84; address++) 
 vesp.MEMORY[address + 0x20] = vesp.BOOTSTRAP[address];
 // set vesp.PC to 0x020,i.e.,the first 
 // instruction in the bootstrap code.
 vesp.PC = 0x020; //This is where everything starts at :-)
 boot();
}

void updatescreen(short x, short y)
{ short i,j;
	for  (i=0; i <= 15; i++)
	{for (j=0; j <= 63; j++)
	{vesp.SCREEN[i][j] = vesp.MEMORY[0x300 + 64*i+j];}
	}
	
	for (j=0;j<= y; j++) cout << (char) vesp.SCREEN[x][j];
}

void delay(float t)
{
	clock_t ticks; 
	ticks = clock() + (clock_t) (t * (float) CLOCKS_PER_SEC); 
	while ( ticks > clock() ){};
} 


void boot(void)  
{short i;
// clear the framebuffer 
for(i = 1; i<= 4254 && vesp.reset == 0; i++) maincycle(0); 
//write "vesp booting..."
for(i = 4255; i<= 4272 && vesp.reset == 0; i++) maincycle(0);
//display the message "vesp is booting..."
updatescreen(0,63);
delay(1);

//write the message "loading kernel...."
for(i = 4273; i<= 4290 && vesp.reset == 0; i++) maincycle(0); 
//display the message "loading kernel"
updatescreen(1,63);
updatescreen(2,63);
//load the kernel
for(i = 4291; i<= 11206 && vesp.reset == 0; i++) maincycle(0); 
for(i = 11207; i<= 11207 && vesp.reset == 0; i++) maincycle(0); 
// This line executes the jmp to kernel instruction
//call kernel
kernel();
}


void kernel (void)
{int i, programSize=0; 
vesp.newcommand = 1;
while (vesp.newcommand == 1)
{vesp.PC = 0x700; //execute display "enter" code
 for(i = 12208; i<= 12213 && vesp.reset == 0; i++) maincycle(0); // invite (write "enter") (new command) 
 updatescreen(3,6); //display the frame buffer
 
 vesp.MEMORY[0x0010] = getchar(); //read in the next command
 getchar();

 switch( vesp.MEMORY[0x0010]) // This could be done in vesp as well- but doing it in C simplifies things considerably
 {case 'l': programSize=0;   
  vesp.PC = 0x781; for(i = 12214; i<= 12223 && vesp.reset == 0; i++) maincycle(0); 
  updatescreen(4,10); //display the frame buffer (d-adress:)
  vesp.MEMORY[0x0010] = getchar(); vesp.MEMORY[0x0011] = getchar(); 
  vesp.MEMORY[0x0012] = getchar(); vesp.MEMORY[0x0013] = getchar();
  getchar();
  
  vesp.PC = 0x795; for(i = 12224; i<= 12233 && vesp.reset == 0; i++) maincycle(0); 
  updatescreen(5,10); //display the frame buffer (l-address:)
  vesp.MEMORY[0x0014] = getchar(); vesp.MEMORY[0x0015] = getchar(); 
  vesp.MEMORY[0x0016] = getchar(); vesp.MEMORY[0x0017] = getchar();
  getchar();
  
  //convert ascii digits to hex digits--This could be done in vesp as well-but doing it in C simplifies things considerably
  for(i=0x0010;i<= 0x0017;i++)
  if(vesp.MEMORY[i] >= '0' && vesp.MEMORY[i] <= '9')
     vesp.MEMORY[i] = vesp.MEMORY[i] - '0';
  else
  if(vesp.MEMORY[i] >= 'A' && vesp.MEMORY[i] <= 'F')
     vesp.MEMORY[i] = vesp.MEMORY[i] - 'A' + 10;
  else {cout << "Invalid address. Try again.\n"; break;}
  
  vesp.MEMORY[0x0010] = vesp.MEMORY[0x0010] << 12 | vesp.MEMORY[0x0011] << 8 | vesp.MEMORY[0x0012] << 4 | vesp.MEMORY[0x0013]; //Disk address
  vesp.MEMORY[0x0014] = vesp.MEMORY[0x0014] << 12 | vesp.MEMORY[0x0015] << 8 | vesp.MEMORY[0x0016] << 4 | vesp.MEMORY[0x0017]; //RAM address
  
  vesp.PC = 0x800; //load using vesp.
  for(i = 12234; i<= 12234 && vesp.reset == 0; i++) maincycle(0); //initialize IX
   
  while(programSize++ <= 256)
  {for(i = 12235; i<= 12238 && vesp.reset == 0; i++) maincycle(0);
  if (vesp.MEMORY[0] == 0) 
  {for(i = 12239; i<= 12242 && vesp.reset == 0; i++) maincycle(0); break;}
  else
  for(i = 12239; i<= 12246 && vesp.reset == 0; i++) maincycle(0);
  }
  
  break;
  
  case 's': programSize=0;
  vesp.PC = 0x795; for(i = 12214; i<= 12223 && vesp.reset == 0; i++) maincycle(0); 
  updatescreen(5,10); //display the frame buffer
  vesp.MEMORY[0x0010] = getchar(); vesp.MEMORY[0x0011] = getchar(); 
  vesp.MEMORY[0x0012] = getchar(); vesp.MEMORY[0x0013] = getchar();
  getchar();
  
  vesp.PC = 0x781; for(i = 12224; i<= 12233 && vesp.reset == 0; i++) maincycle(0); 
  updatescreen(4,10); //display the frame buffer
  vesp.MEMORY[0x0014] = getchar(); vesp.MEMORY[0x0015] = getchar(); 
  vesp.MEMORY[0x0016] = getchar(); vesp.MEMORY[0x0017] = getchar();
  getchar();
  
  //convert ascii digits to hex digits--This could be done in vesp as well-but doing it in C simplifies things considerably
  for(i=0x0010;i<= 0x0017;i++)
  if(vesp.MEMORY[i] >= '0' && vesp.MEMORY[i] <= '9')
     vesp.MEMORY[i] = vesp.MEMORY[i] - '0';
  else
  if(vesp.MEMORY[i] >= 'A' && vesp.MEMORY[i] <= 'F')
     vesp.MEMORY[i] = vesp.MEMORY[i] - 'A' + 10;
  else {cout << "Invalid address. Try again.\n"; break;}
  
  vesp.MEMORY[0x0010] = vesp.MEMORY[0x0010] << 12 | vesp.MEMORY[0x0011] << 8 | vesp.MEMORY[0x0012] << 4 | vesp.MEMORY[0x0013]; //RAM address
  vesp.MEMORY[0x0014] = vesp.MEMORY[0x0014] << 12 | vesp.MEMORY[0x0015] << 8 | vesp.MEMORY[0x0016] << 4 | vesp.MEMORY[0x0017]; //Disk address
  
  vesp.PC = 0x820; //save using vesp.
  for(i = 12234; i<= 12234 && vesp.reset == 0; i++) maincycle(0); //initialize IX
  
  while(programSize++ <= 256)
  {for(i = 12235; i<= 12238 && vesp.reset == 0; i++) maincycle(0);
  if (vesp.MEMORY[0] == 0) 
  {for(i = 12239; i<= 12242 && vesp.reset == 0; i++) maincycle(0); break;}
  else
  for(i = 12239; i<= 12246 && vesp.reset == 0; i++) maincycle(0);
  }
  
  break;
  
  
  case 'e': 
  vesp.PC = 0x795; for(i = 12214; i<= 12223 && vesp.reset == 0; i++) maincycle(0); 
  updatescreen(5,10); //display the frame buffer
  vesp.MEMORY[0x0010] = getchar(); vesp.MEMORY[0x0011] = getchar(); 
  vesp.MEMORY[0x0012] = getchar(); vesp.MEMORY[0x0013] = getchar();
  getchar();

  //convert ascii digits to hex digits--This could be done in vesp as well-but doing it in C simplifies things considerably
  for(i=0x0010;i<= 0x0013;i++)
  if(vesp.MEMORY[i] >= '0' && vesp.MEMORY[i] <= '9')
     vesp.MEMORY[i] = vesp.MEMORY[i] - '0';
  else
  if(vesp.MEMORY[i] >= 'A' && vesp.MEMORY[i] <= 'F')
     vesp.MEMORY[i] = vesp.MEMORY[i] - 'A' + 10;
  else {cout << "Invalid address. Try again.\n"; break;}
  
  vesp.MEMORY[0x0010] = vesp.MEMORY[0x0010] << 12 | vesp.MEMORY[0x0011] << 8 | vesp.MEMORY[0x0012] << 4 | vesp.MEMORY[0x0013];

  vesp.PC = 0x0840;
  for(i = 12224; i<= 12225 && vesp.reset == 0; i++) maincycle(0); //jump to the location specified by the user (see the  execute  vesp code above) 
  while(vesp.reset == 0) maincycle(1); //execute the code
  vesp.reset = 0; //To allow for multiple executions. 4/18/2009
  break; 
  
  case 'r': programSize=0;
  vesp.PC = 0x0795; for(i = 12214; i<= 12223 && vesp.reset == 0; i++) maincycle(0); 
  updatescreen(5,10); //display the frame buffer
  vesp.MEMORY[0x0010] = getchar(); vesp.MEMORY[0x0011] = getchar(); //get the memory address to buffer the console
  vesp.MEMORY[0x0012] = getchar(); vesp.MEMORY[0x0013] = getchar();
  getchar();
  
  //convert ascii digits to hex digits--This could be done in vesp as well-but doing it in C simplifies things considerably
  for(i=0x0010;i<= 0x0013;i++)
  if(vesp.MEMORY[i] >= '0' && vesp.MEMORY[i] <= '9')
     vesp.MEMORY[i] = vesp.MEMORY[i] - '0';
  else
  if(vesp.MEMORY[i] >= 'A' && vesp.MEMORY[i] <= 'F')
     vesp.MEMORY[i] = vesp.MEMORY[i] - 'A' + 10;
  else {cout << "Invalid address. Try again.\n"; break;}
  
  vesp.MEMORY[0x0010] = vesp.MEMORY[0x0010] << 12 | vesp.MEMORY[0x0011] << 8 | vesp.MEMORY[0x0012] << 4 | vesp.MEMORY[0x0013];
  
  vesp.PC = 0x0870; //read using vesp
  for(i = 12224; i<= 12224 && vesp.reset == 0; i++) maincycle(0); //Initialize IX.
   
  while(programSize++ <= 256)
  {vesp.PC = 0x0850;
   for(i = 12225; i<= 12240 && vesp.reset == 0; i++) maincycle(0); 
   updatescreen(6,16); //display the frame buffer (enter next code:)
   
  //read in the next insruction
  vesp.MEMORY[0x0014] = getchar(); vesp.MEMORY[0x0015] = getchar(); 
  vesp.MEMORY[0x0016] = getchar(); vesp.MEMORY[0x0017] = getchar();
  getchar();

   //convert ascii digits to hex digits--This could be done in vesp as well-but doing it in C simplifies things considerably
  for(i=0x0014;i<= 0x0017;i++)
  if(vesp.MEMORY[i] >= '0' && vesp.MEMORY[i] <= '9')
     vesp.MEMORY[i] = vesp.MEMORY[i] - '0';
  else
  if(vesp.MEMORY[i] >= 'A' && vesp.MEMORY[i] <= 'F')
     vesp.MEMORY[i] = vesp.MEMORY[i] - 'A' + 10;
  else {cout << "Invalid address. Try again.\n"; break;}
  
  vesp.MEMORY[0x0014] = vesp.MEMORY[0x0014] << 12 | vesp.MEMORY[0x0015] << 8 | vesp.MEMORY[0x0016] << 4 | vesp.MEMORY[0x0017];

  vesp.PC = 0x0872;
  for(i = 12241; i<= 12244 && vesp.reset == 0; i++) maincycle(0);
  //check to stop reading if it is FFFF.
  if (vesp.MEMORY[0] == 0) {for(i = 12245; i<= 12248 && vesp.reset == 0; i++) maincycle(0); break;}
  else
  for(i = 12245; i<= 12248 && vesp.reset == 0; i++) maincycle(0);
  }
  
  break;
  
  case 'w': programSize=0;
  vesp.PC = 0x0890; for(i = 12214; i<= 12221 && vesp.reset == 0; i++) maincycle(0l); 
  updatescreen(6,7); //display the frame buffer
  vesp.MEMORY[0x0010] = getchar(); vesp.MEMORY[0x0011] = getchar(); 
  vesp.MEMORY[0x0012] = getchar(); vesp.MEMORY[0x0013] = getchar();
  getchar();
    
//convert ascii digits to hex digits--This could be done in vesp as well-but doing it in C simplifies things considerably
  for(i=0x0010;i<= 0x0013;i++)
  if(vesp.MEMORY[i] >= '0' && vesp.MEMORY[i] <= '9')
     vesp.MEMORY[i] = vesp.MEMORY[i] - '0';
  else
  if(vesp.MEMORY[i] >= 'A' && vesp.MEMORY[i] <= 'F')
     vesp.MEMORY[i] = vesp.MEMORY[i] - 'A' + 10;
  else {cout << "Invalid address. Try again.\n"; break;}
  
  vesp.MEMORY[0x0010] = vesp.MEMORY[0x0010] << 12 | vesp.MEMORY[0x0011] << 8 | vesp.MEMORY[0x0012] << 4 | vesp.MEMORY[0x0013];

  for(i = 12222; i<= 12222 && vesp.reset == 0; i++) maincycle(0); 
  //write using vesp.
   
  while(programSize++ <= 256)
  {//vesp.PC = 0x08A2;
  //check to stop writing if it is FFFF.
  for(i = 12223; i<= 12226 && vesp.reset == 0; i++) maincycle(0);
  if (vesp.MEMORY[0] == 0) break;
  else
  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
  cout << hex << vesp.MEMORY[0x0018] << "\n";
  for(i = 12227; i<= 12230 && vesp.reset == 0; i++) maincycle(0);
  }
  break;
  
 }

}

}
                  
int readprogram(void)
{int address,instruction,progend,action,i;
 char longfilename[100] ="/UMD-Web-Dreamweaver/teaching/courses/enee350/vesp-source-code/vesp3.0XK/", filename[25]; 
 //The above path needs to be changed if the file is located in another directory. 
 FILE *file;
 do{ cout << "Enter your program's starting " 
      << "address ( >= 3) as a 3-digit hex number: ";
  cin >> hex >> vesp.PC; 
 } while (vesp.PC < 3);
 
 address =  vesp.PC; action = -1;
 cout << "\n";
 do { cout << "Enter 0 to type in your program or 1 to read it from a file: ";
 cin >> action;
 } while (action != 0 && action != 1);
 
 if(action == 1) 
 {cout << "\nEnter the file name: "; cin >> filename;
 
 
 file = fopen(filename,"r");
 if( file != NULL)
 {
 while (fscanf(file,"%x",&instruction) != EOF  &&  address < 8192 ) 
  {
   vesp.MEMORY[address] = instruction; address = address + 1;
   }
 fclose(file); 
 }
 else
 {for (i=0; i <= 24; i++) longfilename[i+72] = filename[i]; 
  file = fopen(longfilename,"r");
  if( file != NULL)
  {
   while (fscanf(file,"%x",&instruction) != EOF  &&  address < 8192 ) 
   {vesp.MEMORY[address] = instruction; address = address + 1;}
  fclose(file); 
  }
  else
  {cout << "The file is not found. Check if file to be opened is in the program directory... \n"; exit(1);}
 
 
 }
 }
 else  
 do {cin.clear();
     cout << "Enter instruction  " 
          << (address -vesp.PC)  
          << " using a 4-digit hex number" << "\n";
     cout << "Or type -1 to end your program: ";
     
     cin >> hex >> instruction;   //AYO: read it in hex.
      vesp.MEMORY[address] = instruction;
      address = address + 1;
    }
 while ( ( vesp.MEMORY[address-1] !=  -1 ) && (address < 8192)); //AYO: -1 marks the end.

if (address >= 4096) 
{cout << "Memory overflow," 
      << "Please quit from the file menu and restart me!"; 
return address-1;}
progend = address - 1; 


//save the program into a file
 cout << "Enter 0 to continue, 1 to save your program into a file: ";
 cin >> action;

 if(action == 1) 
 {cout << "\nEnter the file name: "; cin >> filename;
 
 cout << hex;
 file = fopen(filename,"w");
 if( file != NULL)
 {address = vesp.PC;
 while (address < progend ) 
  {
 fprintf(file,"%04X\n",vesp.MEMORY[address]); 
  address = address + 1; 
   }
 fclose(file); 
 }
 else
 {for (i=0; i <= 24; i++) longfilename[i+72] = filename[i]; 
  file = fopen(longfilename,"w");
  if( file != NULL)
  {address = vesp.PC;
   cout << hex;
  while (address < progend ) 
  {
   fprintf(file,"%04X\n",vesp.MEMORY[address]); 
   address = address + 1; 
   }

  fclose(file); 
  }
  else
  {cout << "The file is not found. Check if file to be opened is in the program directory... \n"; exit(1);}
 
 
 }
 }


return progend;

}

                  
void displayprogram(int progend) 
{int i; 
cout << "\nHere is your program: \n\n";
for (i = vesp.PC; i<= progend; i++)
{
cout << "Location "; cout.fill('0');  cout.width(4); cout.setf(ios::uppercase);
cout <<  hex << i << ": ";  //AYO: display it in uppercase hex. 
                  
cout.fill('0');  cout.width(4); cout.setf(ios::uppercase);
cout << hex << (0x0000FFFF & vesp.MEMORY[i]) << "\n";
}
}
void displayregisters(void)
{
cout << "A = ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
cout << hex << (0x0000FFFF & vesp.MEMORY[0])<< ", "; 
cout << "B = ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase); 
cout << hex << (0x0000FFFF & vesp.MEMORY[1])<< ", "; 
cout << "IX = ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase); 
cout << hex << (0x0000FFFF & vesp.MEMORY[2])<< ", "; 

cout << "Z = " <<  vesp.Z << ", ";
cout << "S = " <<  vesp.S << ", ";
cout << "C = " <<  vesp.C << ", ";
cout << "F = " <<  vesp.F << "\n";
cout << "MAR = "; cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
cout << hex << vesp.MAR << ", "; 
cout << "PC = ";  cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
cout << hex << vesp.PC << ", "; 
cout << "IR = "; cout.fill('0');  cout.width(4); cout.setf(ios::uppercase); 
cout << hex << vesp.IR << ", "; 
cout << "reset = " << vesp.reset << "\n";
}
                  
void displaymemory(void)
{int location1,location2,i; 
 cout << "Enter the first address (3 hex digits): ";
 cin.clear();
 cin >> hex >> location1;
 cin.clear();
 cout << "Enter the last address: (3 hex digits): ";
 cin.clear();
 cin >> hex >> location2;
 cin.clear();
 for (i = location1; i <= location2; i++)
 {cout <<  "Location "; cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
 cout << hex << i << " : "; 
 cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
 cout << hex << (0x0000FFFF & vesp.MEMORY[i]); cout << "\n"; 
}
}
                  
void maincycle(int trace)
{    if(trace == 1) 
     {     
	 cout << "Machine Cycle "; 
	 cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
	 cout << hex << j << ": ";
	 }
       j = j+1;
      
       //AYO: Fetch Step
       if(trace == 1)
        {cout << "PC = ";  cout.fill('0'); 
        cout.width(8); cout.setf(ios::uppercase);
        cout << hex << (vesp.PC & 0x0FFFFFFF) << ", ";     
        cout << "\nFETCH SUBCYCLE\n";
        }
        fetch(trace);
       if(trace == 1)
        { cout << "\nClock cycle = " << vesp.clock << "\n";
        //AYO: Decode Step
        cout << "DECODE SUBCYCLE\n"; 
        }
        decode(trace);
        if(trace == 1)
        {
        cout << "Clock cycle = " << vesp.clock << "\n";               
        //AYO: Execute Step   
        cout << "EXECUTE SUBCYCLE"; 
        } 
        vesp.add = vesp.cmp = 0;  
        execute(); 
        if(trace == 1)
        {
        cout << "\nClock cycle = " << vesp.clock << "\n\n";
        }
        //AYO: Display the registers 
       
       if(trace == 1)
       {
        displayregisters();  
        cout << "add = " << vesp.add << " "; 
        cout << "complement = " << vesp.cmp << "\n\n"; 
        if( (vesp.IR >> 12 ) == 2  ||  (vesp.IR   >> 12 ) == 3 || 
            (vesp.IR >> 12 ) == 14  || (vesp.IR >> 12 ) == 15 ) 
        {cout << "Memory["; 
         cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
         cout << hex << (vesp.MAR) << "] = ";
         cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
         if(vesp.MAR >> 12 == 0) cout << hex << (0x0000FFFF & vesp.MEMORY[vesp.MAR])<< "\n\n";
		 else
		 if((vesp.MAR >> 12) >= 1 && (vesp.MAR >> 12) <= 14) cout << hex << (0x0000FFFF & vesp.HDISK[vesp.MAR])<< "\n\n";
         } 
        }     
}                  
void fetch(int trace)
{       //clock cycle 1.  Load next instruction's address into MAR. 
        vesp.MAR =  vesp.PC; vesp.clock = vesp.clock +1;
        vesp.PC =   vesp.PC +1; //Increment PC.  
        //clock cycle 2.  Fetch the next Instruction into IR 
if(trace == 1)        
        {cout << "MAR = "; cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
        cout << hex << vesp.MAR << ", ";} 
        vesp.IR = vesp.MEMORY[vesp.MAR]; vesp.clock = vesp.clock +1; 
if(trace == 1)         
        {cout << "IR = "; cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
        cout << hex << vesp.IR << ", ";}
}
                  
void decode(int trace)
{
if(trace == 1) 
{cout << "Decoded instruction is: "; 
     switch( vesp.IR >> 12)   
         {
          case  0x0: 
		  switch(vesp.IR >> 8 & 0x000F)
		  { case 0x0: cout << "ADA\n"; break; // A  = A + B
		    case 0x1: cout << "ADB\n"; break; // B  = A + B
		    case 0x2: cout << "ADX\n"; break; // IX = IX + B
		    case 0x3: cout << "SUB\n"; break; // A  = A - B
			case 0x4: cout << "SUA\n"; break; // B  = B - A		
			case 0x5: cout << "SBX\n"; break; // IX = IX - B				
		  }
		  
		  break; 
		  case  0x1: cout <<  "CMP\n"; break;          
          //Load                             //Move
          case  0x2: cout << "LDA\n"; break; case  0x3: cout <<  "MOV\n"; break;
          //Jump                            //Jump if A = 0
          case  0x4: cout << "JMP\n"; break; case  0x5: cout <<  "JEZ\n"; break;
          //Jump if A > 0                   
          case  0x6: cout << "JPS\n"; break; 
		  
		  case  0x7: 
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: cout <<  "HLT\n"; break; //Halt
		    case 0x1: cout <<  "JAZ\n"; break; //Jump if A = 0
			case 0x2: cout <<  "JAP\n"; break; //Jump if A > 0
			case 0x3: cout <<  "JAN\n"; break; //Jump if A < 0
			case 0x4: cout <<  "JPZ\n"; break; //Jump if A >= 0
			case 0x5: cout <<  "JMZ\n"; break; //Jump if A <= 0
			case 0x6: cout <<  "JNZ\n"; break; //Jump if A <= 0
			case 0x7: cout <<  "NOP\n"; break; //No operation
			case 0x8: cout <<  "JMB\n"; break; //Jump via B
		   }
		  
		  break;
		  
          case  0x8:
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0: cout <<  "INA\n"; break; //Increment A
		    case 1: cout <<  "INB\n"; break; //Increment B
			case 2: cout <<  "INX\n"; break; //Increment IX
		   }		  
		  break;		  
		  
          case  0x9:
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: cout <<  "DEA\n"; break; //Decrement A
		    case 0x1: cout <<  "DEB\n"; break; //Decrement B
			case 0x2: cout <<  "DEX\n"; break; //Decrement IX
		   }		  
		  break;
		  		  
          //AND                           
          case  0xA: 
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: cout <<  "ANA\n"; break; //A = A && B
		    case 0x1: cout <<  "ANB\n"; break; //B = A && B
		   }		  
		  break;

		  
          //IOR                            
          case  0xB: 
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: cout <<  "IOA\n"; break; //A = A || B
		    case 0x1: cout <<  "IOB\n"; break; //B = A || B
		   }		  
		  break;
          
		  //Shift left                      
          case  0xC: 
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: cout <<  "SLA\n"; break; //A = A << 1
		    case 0x1: cout <<  "SLB\n"; break; //B = B << 1
		   }		  
		  break;
		  
		  //Shift right                      
          case  0xD: 
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: cout <<  "SLA\n"; break; //A = A >> 1
		    case 0x1: cout <<  "SLB\n"; break; //B = B >> 1
		   }		  
		  break;
 
		 //Move from with index                    //Move to with index
          case  0xE: cout << "MXF\n"; break; case 0xF: cout << "MXT\n"; break;
				         
          }                    
}
}
                  
void execute(void)
{ short temp;
  short A   = vesp.MEMORY[0];
  short B   = vesp.MEMORY[1];
  unsigned short IX  = vesp.MEMORY[2];
  short VAR = vesp.MEMORY[3];
  switch(vesp.IR >> 12) 
         {//clock cycle 3.
          //Add 		  
		  case  0x0: 
		  switch(vesp.IR >> 8 & 0x000F)
		  { case 0x0: // A  = A + B
		    temp = A + B; vesp.clock = vesp.clock +1;                                        
             if(A > 0 && B > 0 && temp  <  0 || 
             A < 0 && B < 0  && temp  >= 0) 
             vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
             if (A < 0 && B < 0  || temp > 0 && 
             (A < 0 && B > 0 || A > 0 && B < 0)) 
             vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
             A  = temp;   vesp.MEMORY[0] = A; //Save the sum in MEMORY[0]
             //AYO: Set Zero Flag
             if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
             //AYO: Set Sign Flag          
             vesp.S = (A & 0x8000 ) >> 15;  vesp.add = 1; 
			 break;
			
		    case 0x1:  // B  = A + B 
			temp = A + B; vesp.clock = vesp.clock +1;                                        
             if(A > 0 && B > 0 && temp  <  0 || 
             A < 0 && B < 0  && temp  >= 0) 
             vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
             if (A < 0 && B < 0  || temp > 0 && 
             (A < 0 && B > 0 || A > 0 && B < 0)) 
             vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
             B  = temp;   vesp.MEMORY[1] = B; //Save the sum in MEMORY[1]
            //AYO: Set Zero Flag
            if(B  == 0) vesp.Z = 1; else vesp.Z = 0; 
			//AYO: Set Sign Flag          
            vesp.S = (B & 0x8000 ) >> 15;  vesp.add = 1; 
			break; 
			
		    case 0x2: // IX = IX + B
            temp = A + IX; vesp.clock = vesp.clock +1;                                        
             if(A > 0 && (short) IX > 0 &&  temp  <  0 ||  A < 0 && (short) IX < 0  &&  temp  >= 0) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if (A < 0 && (short) IX < 0  || temp > 0 && (A < 0 && (short) IX > 0 || A > 0 && (short) IX < 0)) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            IX  = temp;   vesp.MEMORY[2] = IX; //Save the sum in MEMORY[2]
            //AYO: Set Zero Flag
            if(IX  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (IX & 0x8000 ) >> 15;  vesp.add = 1; 
			break;
			 
		    case 0x3: // A  = A - B
			temp = A - B; vesp.clock = vesp.clock +1;                                        
             if(A > 0 && B < 0 && temp  <  0 || 
             A < 0 && B > 0  && temp  >= 0) 
             vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
             if (A < 0 && B > 0  || temp > 0 && 
             (A < 0 && B < 0 || A > 0 && B > 0)) 
             vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
             A  = temp;   vesp.MEMORY[0] = A; //Save the sum in MEMORY[0]
             //AYO: Set Zero Flag
             if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
             //AYO: Set Sign Flag          
             vesp.S = (A & 0x8000 ) >> 15;  vesp.add = 1; 
			break;
			
			case 0x4: // B  = B - A	
			temp = B - A; vesp.clock = vesp.clock +1;                                        
			 if(B > 0 && A < 0 && temp  <  0 || 
             B < 0 && A > 0  && temp  >= 0) 
             vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
             if (B < 0 && A > 0  || temp > 0 && 
             (B < 0 && A < 0 || B > 0 && A > 0)) 
             vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
             B  = temp;   vesp.MEMORY[1] = B; //Save the sum in MEMORY[1]
            //AYO: Set Zero Flag
            if(B  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (B & 0x8000 ) >> 15;  vesp.add = 1; 
			
			break;	
			
			case 0x5: // IX = IX - B	
			temp = IX - A; vesp.clock = vesp.clock +1;                                        
             if((short) IX > 0 && A < 0 && temp  <  0 || (short) IX < 0 && A > 0  && temp  >= 0) 
             vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
             if ((short) IX < 0 && A > 0  || temp > 0 && ((short) IX < 0 && A < 0 || (short) IX > 0 && A > 0)) 
             vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
             IX  = temp;   vesp.MEMORY[1] = IX; //Save the sum in MEMORY[1]
             //AYO: Set Zero Flag
             if(IX  == 0) vesp.Z = 1; else vesp.Z = 0; 
             //AYO: Set Sign Flag          
             vesp.S = (IX & 0x8000 ) >> 15;  vesp.add = 1;
			
			break;			
		  }
		  
		  break; 
		  
         //Complement
          case  0x1:  A = ~A; vesp.MEMORY[0] = A; 
                    vesp.clock = vesp.clock +1;  
                    if(A  == 0)  vesp.Z = 1; else vesp.Z = 0; 
                    vesp.S = (A & 0x8000 ) >> 15;  
                    vesp.cmp = 1; break;
         //Load         
          case  0x2:  vesp.MAR += 1; 
                    vesp.clock = vesp.clock +1; 
                    vesp.MDR = vesp.MEMORY[vesp.MAR]; 
                    vesp.MAR = vesp.IR&0x0FFF; //without an extra clock
                    vesp.clock = vesp.clock +1; 
                    vesp.MEMORY[vesp.MAR] = vesp.MDR; 
                    vesp.clock = vesp.clock +1; vesp.lda = 1;
                    //AYO: Set Zero Flag
                    if(vesp.MAR == 0 && A == 0) vesp.Z = 1; else vesp.Z = 0; 
                    //AYO: Set Sign Flag  
                    if(vesp.MAR == 0)
                    vesp.S = (A & 0x8000 ) >> 15;                           
                    vesp.PC = vesp.PC + 1; break;  
         //Move
         case 0x3:    vesp.MAR += 1;
                    vesp.clock = vesp.clock +1; 
                    vesp.MDR = vesp.MEMORY[vesp.MAR]; 
                    vesp.clock = vesp.clock +1; 
                    vesp.MAR = vesp.MDR;
                    vesp.clock = vesp.clock +1; 
                    vesp.MDR = vesp.MEMORY[vesp.MAR];
                    vesp.MAR = vesp.IR&0x0FFF; //without an extra clock
                    vesp.clock = vesp.clock +1;                    
                    vesp.MEMORY[vesp.MAR] = vesp.MDR;  
                    vesp.clock = vesp.clock + 1; vesp.mov = 1;
                    //AYO: Set Zero Flag
                    if(vesp.MAR == 0 && A == 0) vesp.Z = 1; else vesp.Z = 0; 
                    //AYO: Set Sign Flag  
                    if(vesp.MAR == 0)
                    vesp.S = (A & 0x8000 ) >> 15;        
                    vesp.PC = vesp.PC + 1; break;
         //Jump
         case 0x4:   vesp.PC = vesp.IR & 0x1FFF; vesp.jmp = 1;
                   vesp.clock = vesp.clock +1; break;  
         //Jump if A is  0
         case 0x5:   if (A == 0) 
                    {vesp.PC = vesp.IR & 0x0FFF;} vesp.jez = 1;
                    vesp.clock = vesp.clock +1; break;  
         //Jump if A is > 0
         case 0x6: if (A > 0) 
                   {vesp.PC = vesp.IR & 0x0FFF;} vesp.jps = 1;
                    vesp.clock = vesp.clock +1; break;  
         //Halt and extended branch
         case 0x7:  
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: vesp.reset = true;  vesp.clock = vesp.clock +1; break; //HLT
		    case 0x1: if (A == 0) 
                   {vesp.PC = B;} vesp.jap = 1;
                    vesp.clock = vesp.clock +1; break;  //Jump if A > 0
			case 0x2:  if (A > 0) 
                   {vesp.PC = B;} vesp.jaz = 1;
                    vesp.clock = vesp.clock +1; break;  //Jump if A = 0
		    case 0x3: if (A < 0) 
                   {vesp.PC = B;} vesp.jng = 1;
                    vesp.clock = vesp.clock +1; break;  //Jump if A < 0
			case 0x4:  if (A >= 0) 
                   {vesp.PC = B;} vesp.jpz = 1;
                    vesp.clock = vesp.clock +1; break;  //Jump if A >= 0
			case 0x5:  if (A <= 0) 
                   {vesp.PC = B;} vesp.jnz = 1;
                    vesp.clock = vesp.clock +1; break;  //Jump if A <= 0
			case 0x6:  if (A != 0) 
                   {vesp.PC = B;} vesp.jnz = 1;
                    vesp.clock = vesp.clock +1; break;  //Jump if A <= 0

			case 0x7: vesp.clock = vesp.clock +1; vesp.nop = 1; break;  //NOP

			case 0x8:  
                   vesp.PC = B; vesp.jmb = 1;
				   vesp.clock = vesp.clock +1; break;  //Jump via register B

		   }
		   break;
          
		   case  0x8: //Increment 
		   switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0://Increment A
		    temp = A + 1; vesp.clock = vesp.clock +1;                                        
            if(A > 0  && temp  <  0 ) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if ( A == -1) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            A  = temp;   vesp.MEMORY[0] = A;  //Save the sum in MEMORY[0]
            //AYO: Set Zero Flag
            if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (A & 0x8000 ) >> 15;  vesp.ina = 1; break;
            //AYO: In this implementation, vesp.inc is just symbolically set to 1.
            //In a bus implementation, the input bus must be loaded with 1 and the 
            //vesp's computation engine must be programmed to add its operands.

		    case 0x1: //Increment B
			temp = B + 1; vesp.clock = vesp.clock +1;                                        
            if(B > 0  && temp  <  0 ) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if ( B == -1) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            B  = temp;   vesp.MEMORY[1] = B;  //Save the sum in MEMORY[1]
            //AYO: Set Zero Flag
            if(B  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (B & 0x8000 ) >> 15;  vesp.inb = 1; break;
            //AYO: In this implementation, vesp.inc is just symbolically set to 1.
            //In a bus implementation, the input bus must be loaded with 1 and the 
            //vesp's computation engine must be programmed to add its operands.

			case 0x2: //Increment IX
            temp = IX + 1; vesp.clock = vesp.clock +1;                                        
            if(IX > 0  && temp  <  0 ) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if ( (short) IX == -1) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            IX  = temp;   vesp.MEMORY[2] = IX;  //Save the sum in MEMORY[2]
            //AYO: Set Zero Flag
            if(IX  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (IX & 0x8000 ) >> 15;  vesp.inx = 1; break;
            //AYO: In this implementation, vesp.inc is just symbolically set to 1.
            //In a bus implementation, the input bus must be loaded with 1 and the 
            //vesp's computation engine must be programmed to add its operands.
		   }		  
		  break;		  
		
          case  0x9: //Decrement 
		  switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0: //Decrement A
			temp = A - 1; vesp.clock = vesp.clock +1;                                        
            if( A < 0  && temp  >= 0) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if (A == 0x0000) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            A  = temp;  vesp.MEMORY[0] = A;   //Save the sum in MEMORY[0]
		    //AYO: Set Zero Flag
            if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (A & 0x8000 ) >> 15;  vesp.dea = 1; break;		   
          //AYO: In this implementation, vesp.dec is just symbolically set to 1.
          //In a bus implementation, the input bus must be loaded with 1 and complemented;
          // carry-in must be set to vesp.dec, and the vesp's computation engine  
          // must be programmed to add its operands.

		    case 0x1: //Decrement B
			temp = B - 1; vesp.clock = vesp.clock +1;                                        
            if( B < 0  && temp  >= 0) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if (B == 0x0000) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            B  = temp;  vesp.MEMORY[1] = B;   //Save the sum in MEMORY[1]
            //AYO: Set Zero Flag
            if(B  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (B & 0x8000 ) >> 15;  vesp.deb = 1; break;
            //AYO: In this implementation, vesp.dec is just symbolically set to 1.
            //In a bus implementation, the input bus must be loaded with 1 and complemented;
            // carry-in must be set to vesp.dec, and the vesp's computation engine  
            // must be programmed to add its operands.

			case 0x2: //Decrement IX
			temp = IX - 1; vesp.clock = vesp.clock +1;                                        
            if( (short) IX < 0  && temp  >= 0) 
            vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
            if (IX == 0x0000) 
            vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
            IX  = temp;  vesp.MEMORY[2] = IX;   //Save the sum in MEMORY[2]
            //AYO: Set Zero Flag
            if(IX  == 0) vesp.Z = 1; else vesp.Z = 0; 
            //AYO: Set Sign Flag          
            vesp.S = (IX & 0x8000 ) >> 15;  vesp.dex = 1; break;
            //AYO: In this implementation, vesp.dec is just symbolically set to 1.
            //In a bus implementation, the input bus must be loaded with 1 and complemented;
            // carry-in must be set to vesp.dec, and the vesp's computation engine  
            // must be programmed to add its operands.
			
		   }	
		   break;	  

         //And
         case  0xA: 
		 switch(vesp.IR >> 8 & 0x000F)
		 {case 0x0: //A = A && B
		  temp = A & B; vesp.clock = vesp.clock +1;                                        
          A  = temp;   vesp.MEMORY[0] = A;  //Save the sum in MEMORY[0]
          //AYO: Set Zero Flag
          if(A  == 0) vesp.Z = 1; else vesp.Z = 0; vesp.ana = 1; break;

		  case 0x1://B = A && B
          temp = A & B; vesp.clock = vesp.clock +1;                                        
          B  = temp;   vesp.MEMORY[1] = B;  //Save the sum in MEMORY[1]
          //AYO: Set Zero Flag
          if(B  == 0) vesp.Z = 1; else vesp.Z = 0; vesp.anb = 1;  break;
		 }		  
		  break;
 
                  
          //Or
         case  0xB: 
		 switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0:  //A = A || B
			temp = A | B; vesp.clock = vesp.clock +1;                                        
            A  = temp;   vesp.MEMORY[0] = A;  //Save the sum in MEMORY[0]
            //AYO: Set Zero Flag
            if(A  == 0) vesp.Z = 1; else vesp.Z = 0;  vesp.ioa = 1; break;

		    case 0x1: //B = A || B
			temp = A | B; vesp.clock = vesp.clock +1;                                        
            B = temp;   vesp.MEMORY[1] = B;  //Save the sum in MEMORY[1]
           //AYO: Set Zero Flag
           if(B  == 0) vesp.Z = 1; else vesp.Z = 0;  vesp.iob = 1; break;
		   }		  
		  break;

 
		 //shift left
         case  0xC: 
		 switch(vesp.IR >> 8 & 0x000F)
		 {case 0x0://A = A << 1
		  temp = A << 1; vesp.clock = vesp.clock +1;                                        
          A  = temp;  vesp.MEMORY[0] = A;  //Save the sum in MEMORY[0]
          //AYO: Set Zero Flag
          if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
          //AYO: Set Sign Flag          
          vesp.S = (A & 0x8000 ) >> 15;  vesp.sla = 1; break;

		  case 0x1: //B = B << 1
		  temp = B << 1; vesp.clock = vesp.clock +1;                                        
          B  = temp;  vesp.MEMORY[1] = B;  //Save the sum in MEMORY[1]
          //AYO: Set Zero Flag
          if(B  == 0) vesp.Z = 1; else vesp.Z = 0; 
          //AYO: Set Sign Flag          
          vesp.S = (B & 0x8000 ) >> 15;  vesp.slb = 1; break;
		 }		  
		  break;
		  
			
         //Shift right
         case  0xD: 
		 switch(vesp.IR >> 8 & 0x000F)
		   {case 0x0://A = A >> 1
			temp = A >> 1; vesp.clock = vesp.clock +1;                                        
           A  = temp;  vesp.MEMORY[0] = A;   //Save the sum in MEMORY[0]
           //AYO: Set Zero Flag
           if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
           //AYO: Set Sign Flag          
           vesp.S = (A & 0x8000 ) >> 15;  vesp.sra = 1; break;

		  case 0x1: //B = B >> 1
		  temp = B >> 1; vesp.clock = vesp.clock +1;                                        
          B  = temp;  vesp.MEMORY[1] = B;   //Save the sum in MEMORY[1]
          //AYO: Set Zero Flag
          if(B  == 0) vesp.Z = 1; else vesp.Z = 0; 
          //AYO: Set Sign Flag          
          vesp.S = (B & 0x8000 ) >> 15;  vesp.srb = 1; break;
		   }		  
		  break;
                  
//MXF (index is used to copy a list of operands into a location one at a time.)
         case  14: 
         if((VAR >> 12) == 0)
          {vesp.MAR = IX; // read the next operand (assuming that IX holds a 16 bit address always.)
          vesp.clock = vesp.clock +1;  
          if((( vesp.MAR) >> 12) == 0)
          vesp.MDR = vesp.MEMORY[vesp.MAR]; 
          else if ((vesp.MAR >> 12) >= 1 && ((vesp.MAR >> 12) <= 14))
          vesp.MDR = vesp.HDISK[vesp.MAR]; 
          else
          {cout << "\npage fault- instruction not executed!\n";
          // This will be a place to call a trap and exit to the OS. 
          }                  
          vesp.MAR = vesp.IR&0x0FFF; //without an extra clock
          vesp.clock = vesp.clock +1;  
          vesp.MEMORY[vesp.MAR] = vesp.MDR;  // move the operand to the specified address
          IX = IX+1; vesp.MEMORY[2] = IX;//without an extra clock
          vesp.clock = vesp.clock +1;
		  
          }
         else if((VAR >> 12) >= 1) 
         {vesp.MAR = IX; // read the next operand
          vesp.clock = vesp.clock +1; 
          if (((vesp.MAR) >> 12) == 0) 
          vesp.MDR = vesp.MEMORY[vesp.MAR]; 
          else if ((vesp.MAR >> 12) >= 1 && ((vesp.MAR >> 12) <= 14))
          vesp.MDR = vesp.HDISK[vesp.MAR]; 
          else
          {cout << "\npage fault- instruction not executed!\n";
          // This will be a place to call a trap and exit to the OS. 
          }                  
          vesp.MAR = vesp.IR&0x0FFF; //without an extra clock
          vesp.clock = vesp.clock +1;  
          vesp.HDISK[vesp.MAR] = vesp.MDR;  // move the operand to the specified address
          IX = IX+1; vesp.MEMORY[2] = IX; //without an extra clock
          vesp.clock = vesp.clock +1;
		 }
          else
          {cout << "\npage fault- instruction not executed!\n";
          // This will be a place to call a trap and exit to the OS. 
          }                                  
          //AYO: Set Zero Flag
          if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
          //AYO: Set Sign Flag          
          vesp.S = (A & 0x8000 ) >> 15;  vesp.mxf = 1; break;
        
         //MXT (index register is used to copy an operand to a set of locations one at a time.)
         case  15: 
		 if((IX >> 12)  == 0) //destination is in vesp.MEMORY (and IX always holds a 16-bit address.)
         { vesp.MAR = vesp.IR&0x0FFF; // read the operand
          vesp.clock = vesp.clock +1; 
          if((vesp.VAR >> 12) == 0)           
          vesp.MDR = vesp.MEMORY[vesp.MAR];
          else
          if((VAR >> 12) == 1) 
          vesp.MDR = vesp.MEMORY[vesp.MAR];
          else  
         {cout << "\npage fault- instruction not executed!\n";
          // This will be a place to call a trap and exit to the OS. 
         }                                                              
          vesp.MAR = IX; //without an extra clock
          vesp.clock = vesp.clock +1; 
          vesp.MEMORY[vesp.MAR] = vesp.MDR; //move the operand to the next location
          IX = IX+1; vesp.MEMORY[2] = IX; //without an extra clock
          vesp.clock = vesp.clock +1; 
          
         }
         else if((IX>> 12) >= 1  && ( IX >> 12) <= 14) //destination is in vesp.HDISK
          {vesp.MAR = vesp.IR&0x0FFF; // read the operand
          vesp.clock = vesp.clock +1; 
          if((VAR >> 12) == 0) 
          vesp.MDR = vesp.MEMORY[vesp.MAR];
          else if((vesp.VAR >> 12) == 1)
          vesp.MDR = vesp.HDISK[vesp.MAR]; 
         else
         {cout << "\npage fault- instruction not executed!\n";
          // This will be a place to call a trap and exit to the OS. 
         }                                            
          vesp.MAR = (short) IX; //without an extra clock
          vesp.clock = vesp.clock +1; 
		  vesp.HDISK[vesp.MAR] = vesp.MDR; //move the operand to the next location
          IX = IX+1; vesp.MEMORY[2] = IX; //without an extra clock
          vesp.clock = vesp.clock +1; 
		  }
         else
         {cout << "\npage fault- instruction not executed!\n";  
          // This will be a place to call a trap and exit to the OS. 
         }                                  
          //AYO: Set Zero Flag
          if(A  == 0) vesp.Z = 1; else vesp.Z = 0; 
          //AYO: Set Sign Flag          
          vesp.S = (A & 0x8000 ) >> 15;  vesp.mxt = 1; break;        
          }  
    
}

//Sample runs
/*
 [Session started at 2009-04-23 20:21:57 -0400.]
 vesp is booting...                                             
 loading kernel....                                             
 
 enter: r
 m-address: 0A00
 enter next code: 2000
 enter next code: 0007
 enter next code: 2002
 enter next code: E000
 enter next code: F000
 enter next code: 2002
 enter next code: E000
 enter next code: E001
 enter next code: 7000
 enter next code: FFFF
 enter: e
 m-address: 0A00
 Machine Cycle 00002CFC: PC = 00000A00, 
 FETCH SUBCYCLE
 MAR = 0A00, IR = 2000, 
 Clock cycle = CEF3
 DECODE SUBCYCLE
 Decoded instruction is: LDA
 Clock cycle = CEF3
 EXECUTE SUBCYCLE
 Clock cycle = CEF6
 
 A = 0007, B = 0A00, IX = 0A09, Z = 1, S = 0, C = 0, F = 0
 MAR = 0000, PC = 00000A02, IR = 2000, reset = 0
 add = 0 complement = 0
 
 Memory[0000] = 0007
 
 Machine Cycle 00002CFD: PC = 00000A02, 
 FETCH SUBCYCLE
 MAR = 0A02, IR = 2002, 
 Clock cycle = CEF8
 DECODE SUBCYCLE
 Decoded instruction is: LDA
 Clock cycle = CEF8
 EXECUTE SUBCYCLE
 Clock cycle = CEFB
 
 A = 0007, B = 0A00, IX = E000, Z = 0, S = 0, C = 0, F = 0
 MAR = 0002, PC = 00000A04, IR = 2002, reset = 0
 add = 0 complement = 0
 
 Memory[0002] = E000
 
 Machine Cycle 00002CFE: PC = 00000A04, 
 FETCH SUBCYCLE
 MAR = 0A04, IR = F000, 
 Clock cycle = CEFD
 DECODE SUBCYCLE
 Decoded instruction is: MXT
 Clock cycle = CEFD
 EXECUTE SUBCYCLE
 Clock cycle = CF00
 
 A = 0007, B = 0A00, IX = E001, Z = 0, S = 0, C = 0, F = 0
 MAR = E000, PC = 00000A05, IR = F000, reset = 0
 add = 0 complement = 0
 
 Memory[E000] = 0007
 
 Machine Cycle 00002CFF: PC = 00000A05, 
 FETCH SUBCYCLE
 MAR = 0A05, IR = 2002, 
 Clock cycle = CF02
 DECODE SUBCYCLE
 Decoded instruction is: LDA
 Clock cycle = CF02
 EXECUTE SUBCYCLE
 Clock cycle = CF05
 
 A = 0007, B = 0A00, IX = E000, Z = 0, S = 0, C = 0, F = 0
 MAR = 0002, PC = 00000A07, IR = 2002, reset = 0
 add = 0 complement = 0
 
 Memory[0002] = E000
 
 Machine Cycle 00002D00: PC = 00000A07, 
 FETCH SUBCYCLE
 MAR = 0A07, IR = E001, 
 Clock cycle = CF07
 DECODE SUBCYCLE
 Decoded instruction is: MXF
 Clock cycle = CF07
 EXECUTE SUBCYCLE
 Clock cycle = CF0A
 
 A = 0007, B = 0007, IX = E001, Z = 0, S = 0, C = 0, F = 0
 MAR = 0001, PC = 00000A08, IR = E001, reset = 0
 add = 0 complement = 0
 
 Memory[0001] = 0007
 
 Machine Cycle 00002D01: PC = 00000A08, 
 FETCH SUBCYCLE
 MAR = 0A08, IR = 7000, 
 Clock cycle = CF0C
 DECODE SUBCYCLE
 Decoded instruction is: HLT
 Clock cycle = CF0C
 EXECUTE SUBCYCLE
 Clock cycle = CF0D
 
 A = 0007, B = 0007, IX = E001, Z = 0, S = 0, C = 0, F = 0
 MAR = 0A08, PC = 00000A09, IR = 7000, reset = 1
 add = 0 complement = 0
 
 enter:  
 ----------
*/
/*
 
 [Session started at 2008-11-11 08:47:26 -0500.]
 vesp is booting...                                             
 loading kernel....                                             
 
 enter: r
 m-address: 0A00
 enter next code: 2000
 enter next code: 000E
 enter next code: 2001
 enter next code: 000D
 enter next code: 0000
 enter next code: 7000
 enter next code: FFFF
 enter: w
 address:x0A00
 2000
 000E
 2001
 000D
 0000
 7000
 enter: e
 m-address: 0A00
 Machine Cycle 00002CE9: PC = 00000A00, 
 FETCH SUBCYCLE
 MAR = 0A00, IR = 2000, 
 Clock cycle = CE6E
 DECODE SUBCYCLE
 Decoded instruction is: LDA
 Clock cycle = CE6E
 EXECUTE SUBCYCLE
 Clock cycle = CE71
 
 A = 000E, B = 0A00, IX = 0A07, Z = 1, S = 0, C = 0, F = 0
 MAR = 0000, PC = 00000A02, IR = 2000, reset = 0
 add = 0 complement = 0
 
 Memory[0000] = 000E
 
 Machine Cycle 00002CEA: PC = 00000A02, 
 FETCH SUBCYCLE
 MAR = 0A02, IR = 2001, 
 Clock cycle = CE73
 DECODE SUBCYCLE
 Decoded instruction is: LDA
 Clock cycle = CE73
 EXECUTE SUBCYCLE
 Clock cycle = CE76
 
 A = 000E, B = 000D, IX = 0A07, Z = 0, S = 0, C = 0, F = 0
 MAR = 0001, PC = 00000A04, IR = 2001, reset = 0
 add = 0 complement = 0
 
 Memory[0001] = 000D
 
 Machine Cycle 00002CEB: PC = 00000A04, 
 FETCH SUBCYCLE
 MAR = 0A04, IR = 0000, 
 Clock cycle = CE78
 DECODE SUBCYCLE
 Decoded instruction is: ADA
 Clock cycle = CE78
 EXECUTE SUBCYCLE
 Clock cycle = CE79
 
 A = 001B, B = 000D, IX = 0A07, Z = 0, S = 0, C = 0, F = 0
 MAR = 0A04, PC = 00000A05, IR = 0000, reset = 0
 add = 1 complement = 0
 
 Machine Cycle 00002CEC: PC = 00000A05, 
 FETCH SUBCYCLE
 MAR = 0A05, IR = 7000, 
 Clock cycle = CE7B
 DECODE SUBCYCLE
 Decoded instruction is: HLT
 Clock cycle = CE7B
 EXECUTE SUBCYCLE
 Clock cycle = CE7C
 
 A = 001B, B = 000D, IX = 0A07, Z = 0, S = 0, C = 0, F = 0
 MAR = 0A05, PC = 00000A06, IR = 7000, reset = 1
 add = 0 complement = 0
 
 enter: 
 */
                  
                  
