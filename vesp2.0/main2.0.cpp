/*****************************************************************************/ 
   /*                              VESP COMPUTER-2.0                             *
   /*                            Author: A. Yavuz Oruc                           * 
   /*                            Copyright ¬© 2000,2004, 2008.                   * 
   /*                      Sabatech Corporation (www.sabatech.com)               * 
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
   
   
   /*--------------------Changes and additions in this version-------------------*/
   /*                                                                            */
   /* -Instruction opcodes 8 through 15 are new in this version of VESP.         */
   /* vesp.MEMORY[2] is assigned to IX which is used with MXF and MXT            */
   /* instructions.                                                              */
   /* -vesp programs which are entered at the console can be saved in a file.    */
   /* Warning!!!! This program overwrites any data in the filename  which is     */
   /* specified at the console in response to the prompt for saving a vesp       */
   /* program.  Save at your own risk!!!!!                                       */
   /******************************************************************************/
   
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
   /* Its instruction repertoire consists of the following instructions: 
   /* ADD: Add            Opcode: 0000  ----- A = A+B                          HexCode: 0
   /* CMP: Compl          Opcode: 0001  ----- A = ~A                           HexCode: 1 
   /* LDA: Load           Opcode: 0010  ----- M[IR[4:15] ] = M[MAR+1]          HexCode: 2
   /* MOV: Move           Opcode: 0011  ----- M[IR[4:15] ] = M[M[MAR+1][3:15]] HexCode: 3 
   /* JMP  Jump           Opcode: 0100  ----- PC = IR[4:15]                    HexCode: 4 
   /* JEZ: Jump if A=0    Opcode: 0101  ----- If (A = 0)  PC = IR[4:15]        HexCode: 5 
   /* JPS: Jump if A=+    Opcode: 0110  ----- If (A > 0)  PC = IR[4:15]        HexCode: 6 
   /* HLT: Halt           Opcode: 0111  ----- reset = 1                        HexCode: 7 
   /* INC: Increment      Opcode: 1000  ----- A = A + 1
   /* DEC: Decrement      Opcode: 1001  ----- A = A - 1
   /* AND: And            Opcode: 1010  ----- A = A & B
   /* IOR: Or             Opcode: 1011  ----- A = A |  B
   /* SHL: Shift left     Opcode: 1100  ----- A << 1
   /* SHR: Shift right    Opcode: 1101  ----- A >> 1
   /* MXF  Move from w/IX Opcode: 1110  ----- M[IR[3:15]] = M[IX]; IX = IX + 1;HexCode: E
   /* MXT  Move to   w/IX Opcode: 1111  ----- M[IX] = M[IR[3:15]]; IX = IX + 1;HexCode: F
   
   /*         Programs are entered and displayed in hex code. */
   
   #include <iostream> 
   #include <cstdlib>
   #include <iomanip>
   #include <stdio.h>
   #include <limits.h> 
   
   void initialize(void); int readprogram(void);
   void displayprogram(int progend); 
   void displayregisters(void); void displaymemory(void);
   void maincycle(int trace);   void fetch(int trace);
   void decode(void);           void execute(void);
   
   //AYO: Define the registers of the architecture.
   typedef struct 
   {unsigned short   MAR,PC,IR,MDR;
   unsigned long clock;
   short  MEMORY[8192],S,C,F,Z,reset,add,cmp,lda,mov,jmp,jez,jps,hlt,inc,dec,lnd,ior,shl,shr,mxf,mxt;
   } architecture;  architecture   vesp;
using namespace std; 
   int j=1;
   
   int main(void)
   {int action,action2,progend;
   initialize();//AYO: Initialize all the registers.
   cout << "\nWelcome to Vesp 2.0\n\n";
   while(1) {vesp.reset = false; //AYO: vesp keeps running unless it is reset.
   //AYO: Input program, diplay registers or memory 
   cin.clear();
   cout << "\nType \n 0 to enter a program\n "
   << "1 to display registers\n 2 to display memory: ";
   cin >> action2; 
   cin.clear();
   cout << "\n";
   //AYO: Read the program, display it, and execute it. 
   if(action2 == 0) 
   {progend = readprogram(); displayprogram(progend);
   //AYO: Step through or execute the program.
   
   while(vesp.reset == 0)
   {cout << "\nEnter 0 if you wish to execute the program, 1 to step it, ";
   cout <<  "2 to exit to the main menu: ";
   cin.clear(); cin >> action2;
   switch(action2)
   {case 0: cout << "Enter 0 for a brief trace 1 for long trace: "; cin.clear(); cin >> action; 
   cout << "\n";
   while(vesp.reset == 0) maincycle(action); break;
   case 1: maincycle(1);    break;
   case 2: vesp.reset = true; break;
   }
   }
   
   //AYO: Display the number of instructions and clock cycles executed.
   if(action2 != 2)
   {cout << "The number of instructions executed  = " << dec << j-1  << "\n"; 
   cout << "The number of clock cycles used = " << dec << vesp.clock << "\n"; 
   j = 1; vesp.clock = 0;
   }
   }
   if (action2 == 1) displayregisters();
   if (action2 == 2) displaymemory();
   //if (action == 3) readprogram();
   } 
   }
   
   void initialize(void)
   {vesp.PC = vesp.MEMORY[0] = vesp.MEMORY[1] = vesp.IR = 0;
   vesp.reset = 0; vesp.clock = 0;
   }
   
   int readprogram(void)
   {int address,instruction,progend,action,i;
   char longfilename[100] ="/UMD-Web-Dreamweaver/teaching/courses/enee350/vesp-source-code/vesp2.0X/", filename[25]; 
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
   while (address <= progend ) 
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
   while (address <= progend ) 
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
   cout << "Z = " <<  vesp.Z << ", ";
   cout << "S = " <<  vesp.S << ", ";
   cout << "C = " <<  vesp.C << ", ";
   cout << "F = " <<  vesp.F << "\n";
   cout << "MAR = "; cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
   cout << hex << vesp.MAR << ", "; 
   cout << "PC = ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
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
   {     cout << "Machine Cycle " << j << ":  ";
   j = j+1;
   //AYO: Fetch Step
   cout << "PC = ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
   cout << hex << (vesp.PC & 0x0FFF) << ", "; 
   cout << "\nFETCH SUBCYCLE\n";
   fetch(trace);
   cout << "\nClock cycle = " << vesp.clock << "\n";
   //AYO: Decode Step
   cout << "DECODE SUBCYCLE\n"; 
   decode();
   cout << "Clock cycle = " << vesp.clock << "\n"; 
   //AYO: Execute Step 
   cout << "EXECUTE SUBCYCLE"; 
   vesp.add = vesp.cmp = 0; 
   execute(); 
   cout << "\nClock cycle = " << vesp.clock << "\n\n";
   //AYO: Display the registers 
   
   if(trace == 1)
   {
   displayregisters(); 
   cout << "add = " << vesp.add << " "; 
   cout << "complement = " << vesp.cmp << "\n\n"; 
   if( (vesp.IR >> 12 ) == 2  ||  (vesp.IR   >> 12 ) == 3) 
   {cout << "Memory["; 
   cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
   cout << hex << (vesp.IR & 0x0FFF) << "] = ";
   cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
   cout << hex << (0x0000FFFF & vesp.MEMORY[vesp.IR & 0x0FFF])<< "\n\n";
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
   
   void decode(void)
   {cout << "Decoded instruction is: "; 
   switch( vesp.IR   >> 12) 
   {//Add                            //Complement
   case  0: cout << "ADD\n"; break; case  1: cout <<  "CMP\n"; break; 
   //Load                           //Move
   case  2: cout << "LDA\n"; break; case  3: cout <<  "MOV\n"; break;
   //Jump                           //Jump if A = 0
   case  4: cout << "JMP\n"; break; case  5: cout <<  "JEZ\n"; break;
   //Jump if A > 0                  //Halt
   case  6: cout << "JPS\n"; break; case  7: cout <<  "HLT\n"; break;
   //Increment                      //Decrement
   case  8: cout << "INC\n"; break; case  9: cout <<  "DEC\n"; break; 
   //And                            //Or
   case  10: cout << "AND\n"; break; case 11: cout << "IOR\n"; break;
   //Shift left                     //Shift right
   case  12: cout << "SHL\n"; break; case 13: cout << "SHR\n"; break;
   //Add with IX+                   //Add with IX-
   case  14: cout << "MXF\n"; break; case 15: cout << "MXT\n"; break;
   } 
   }
   
   void execute(void)
   { short temp;
   switch(vesp.IR >> 12) 
   {//clock cycle 3.
   //Add
   case  0: 
   temp = vesp.MEMORY[0] + vesp.MEMORY[1]; vesp.clock = vesp.clock +1; 
   if(vesp.MEMORY[0] > 0 && vesp.MEMORY[1] > 0 && temp  <  0 || 
   vesp.MEMORY[0] < 0 && vesp.MEMORY[1] < 0  && temp  >= 0) 
   vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
   if (vesp.MEMORY[0] < 0 && vesp.MEMORY[1] < 0  || temp > 0 && 
   (vesp.MEMORY[0] < 0 && vesp.MEMORY[1] > 0 || vesp.MEMORY[0] > 0 && vesp.MEMORY[1] < 0)) 
   vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.add = 1; break;
   //Complement
   case  1:  vesp.MEMORY[0] = ~vesp.MEMORY[0]; 
   vesp.clock = vesp.clock +1; 
   if(vesp.MEMORY[0]  == 0)  vesp.Z = 1; else vesp.Z = 0; 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15; 
   vesp.cmp = 1; break;
   //Load 
   case  2:   vesp.MAR += 1; 
   vesp.clock = vesp.clock +1; 
   vesp.MDR = vesp.MEMORY[vesp.MAR]; 
   vesp.MAR = vesp.IR&0x0FFF; //without an extra clock
   vesp.clock = vesp.clock +1; 
   vesp.MEMORY[vesp.MAR] = vesp.MDR; 
   vesp.clock = vesp.clock +1; vesp.lda = 1;
   //AYO: Set Zero Flag
   if(vesp.MAR == 0 && vesp.MEMORY[0] == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   if(vesp.MAR == 0)
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15; 
   vesp.PC = vesp.PC + 1; break; 
   //Move
   case 3:    vesp.MAR += 1;
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
   if(vesp.MAR == 0 && vesp.MEMORY[0] == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   if(vesp.MAR == 0)
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15; 
   vesp.PC = vesp.PC + 1; break;
   //Jump
   case 4:   vesp.PC = vesp.IR & 0x1FFF; vesp.jmp = 1;
   vesp.clock = vesp.clock +1; break; 
   //Jump if A is  0
   case 5:   if (vesp.MEMORY[0] == 0) 
   {vesp.PC = vesp.IR & 0x0FFF;} vesp.jez = 1;
   vesp.clock = vesp.clock +1; break; 
   //Jump if A is > 0
   case 6: if (vesp.MEMORY[0] > 0) 
   {vesp.PC = vesp.IR & 0x0FFF;} vesp.jps = 1;
   vesp.clock = vesp.clock +1; break; 
   //Halt
   case 7:  vesp.reset = true;  vesp.clock = vesp.clock +1; break; 
   
   //INC
   case  8: 
   temp = vesp.MEMORY[0] + 1; vesp.clock = vesp.clock +1; 
   if(vesp.MEMORY[0] > 0 && vesp.MEMORY[1] > 0 && temp  <  0 || 
   vesp.MEMORY[0] < 0 && vesp.MEMORY[1] < 0  && temp  >= 0) 
   vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
   if (vesp.MEMORY[0] < 0 && vesp.MEMORY[1] < 0  || temp > 0 && 
   (vesp.MEMORY[0] < 0 && vesp.MEMORY[1] > 0 || vesp.MEMORY[0] > 0 && vesp.MEMORY[1] < 0)) 
   vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.inc = 1; break;
   //AYO: In this implementation, vesp.inc is just symbolically set to 1.
   //In a bus implementation, the input bus must be loaded with 1 and the 
   //vesp's computation engine must be programmed to add its operands.
   
   //DEC
   case  9: 
   temp = vesp.MEMORY[0] - 1; vesp.clock = vesp.clock +1; 
   if(vesp.MEMORY[0] > 0 && vesp.MEMORY[1] > 0 && temp  <  0 || 
   vesp.MEMORY[0] < 0 && vesp.MEMORY[1] < 0  && temp  >= 0) 
   vesp.F = 1; else vesp.F = 0; //AYO: Set Overflow Flag
   if (vesp.MEMORY[0] < 0 && vesp.MEMORY[1] < 0  || temp > 0 && 
   (vesp.MEMORY[0] < 0 && vesp.MEMORY[1] > 0 || vesp.MEMORY[0] > 0 && vesp.MEMORY[1] < 0)) 
   vesp.C = 1; else vesp.C = 0; //AYO: Set Carry Flag
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.dec = 1; break;
   //AYO: In this implementation, vesp.dec is just symbolically set to 1.
   //In a bus implementation, the input bus must be loaded with 1 and complemented;
   // carry-in must be set to vesp.dec, and the vesp's computation engine 
   // must be programmed to add its operands.
   
   //AND
   case  10: 
   temp = vesp.MEMORY[0] & vesp.MEMORY[1]; vesp.clock = vesp.clock +1; 
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; vesp.lnd = 1; break;
   
   //IOR
   case  11: 
   temp = vesp.MEMORY[0] | vesp.MEMORY[1]; vesp.clock = vesp.clock +1; 
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0;  vesp.ior = 1; break;
   
   //SHL
   case  12: 
   temp = vesp.MEMORY[0] << 1; vesp.clock = vesp.clock +1; 
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.shl = 1; break;
   
   //SHR
   case  13: 
   temp = vesp.MEMORY[0] >> 1; vesp.clock = vesp.clock +1; 
   vesp.MEMORY[0]  = temp;    //Save the sum in MEMORY[0]
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.shr = 1; break;
   
   //MXF (index is used to copy a list of operands into a location one at a time.)
   case  14: 
   vesp.MAR = vesp.MEMORY[2]; // read the next operand
   vesp.clock = vesp.clock +1; 
   vesp.MDR = vesp.MEMORY[vesp.MAR]; 
   vesp.MAR = vesp.IR&0x0FFF; //without an extra clock
   vesp.clock = vesp.clock +1; 
   vesp.MEMORY[vesp.MAR] = vesp.MDR;  // move the operand to the specified address
   vesp.MEMORY[2]++; //without an extra clock
   vesp.clock = vesp.clock +1; 
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.mxf = 1; break;
   
   //MXT (index is used to copy an operand to a set of locations one at a time.)
   case  15: 
   vesp.MAR = vesp.IR&0x0FFF; // read the operand
   vesp.clock = vesp.clock +1; 
   vesp.MDR = vesp.MEMORY[vesp.MAR];
   vesp.MAR = vesp.MEMORY[2]; //without an extra clock
   vesp.clock = vesp.clock +1;
   vesp.MEMORY[vesp.MAR] = vesp.MDR; //move the operand to the next location
   vesp.MEMORY[2]++; //without an extra clock
   vesp.clock = vesp.clock +1; 
   //AYO: Set Zero Flag
   if(vesp.MEMORY[0]  == 0) vesp.Z = 1; else vesp.Z = 0; 
   //AYO: Set Sign Flag 
   vesp.S = (vesp.MEMORY[0] & 0x8000 ) >> 15;  vesp.mxt = 1; break; 
   } 
   }