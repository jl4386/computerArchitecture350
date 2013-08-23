/*****************************************************************************/
/*                    VESP COMPUTER-2.1M(Microprogrammed version)             *
 /*                            Author: A. Yavuz Oruc                           *
 /*                            Copyright Â© 2000,2005,2008.                     *
 /*                            AlgoritiX Corporation                           *
 /*                             All rights reserved.                           *
 /*Copying and compiling this program for personal use is permitted. However,  *
 /*no part of this program may be reproduced or distributed in any form or     *
 /*by any means, or stored in a database or  retrieval system, without the     *
 /*prior written permission of the Author and AlgoritiX Corporation.           *                                                            *
 /*Neither AlgoritiX nor the Author makes any direct or implied claims         *                                                            *
 /*that this program will work accurately and/or generate correct results,     *                                                            *
 /*In addition, they provide no warranties direct or implied that using        *                                                            *
 /*this program on any computer will be safe in so far as  guarding against    *
 /*destroying and/or deleting any information, data, code, program or file     *
 /*that may be stored in the memory of user's computer.                        *
 /*****************************************************************************/


/*      Please communicate all your questions and comments to                */
/*                    A. Yavuz Oruc at yavuz@sabatech.com                    */


/*--------------------Changes and additions in this version-------------------*/
/* This is the 1st microprogrammed version of vesp 2.1                        */
/* Warning!!!! This program overwrites any data in the filename  which is     */
/* specified at the console in response to the prompt for saving a vesp       */
/* program.  Save at your own risk!!!!!                                       */
/******************************************************************************/

/* This  computer program simulates the fetch,decode
 /* and execute cycles of a hypothetical 16-bit computer,
 /* called VESP (_V_Ery _Simple _Processor).
 /* This is the microprogrammed version of Vesp.
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
#include <iomanip>
#include <stdio.h>
#include <limits.h>

void initialize(void); int readprogram(void);
void displayprogram(int progend);
void displayregisters(void); void displaymemory(void);
void maincycle(int trace);   void fetch(int microtrace);
void decode(void);           void execute(int microtrace);
void microsequencer(int microtrace);   void loadmicrostore(void);

//AYO: Define the registers of the architecture.
typedef struct
{unsigned short   MAR,PC,IR,MDR;
    unsigned long clock;
    short   MEMORY[8192],S,C,F,Z,reset,add,cmp,lda,mov,jmp,jez,jps,hlt,inc,dec,lnd,lor,shl,shr,mxf,mxt;
    short RET_SET;
    short memory_read;
    short memory_write;
} architecture;  architecture   vesp;
typedef struct
{short MAR,  PC,   MIR,   clock;
    short ABUS, BBUS, OBUS;
    //char RET_SET;
    short MICSTORE[4096];
    short fetch_address,add_address,comp_address,load_address,move_address,jump_address,
    branchzero_address,branchpositive_address,halt_address,inc_address,dec_address,and_address,
    or_address,shl_address,shr_address,mxf_address,mxt_address;
} micarchitecture;  micarchitecture   micro_vesp;

using namespace std;
int j=1;

int main(void)
{int action,action2,progend;
    initialize();//AYO: Initialize all the registers.
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
                {case 0: cout << "Enter 0 for a machine trace, 1 for machine and micro trace:"; cin.clear();
                        cin >> action; cout << "\n";
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
{cout << "\nwelcome to Vesp 2.1M\n\n";
    cout << "resetting VESP's registers\n";
    vesp.PC = vesp.MEMORY[0] = vesp.MEMORY[1] = vesp.IR = 0;
    vesp.reset = 0; vesp.clock = 0; micro_vesp.clock = 0;
    cout << "initializing VESP's microstore\n";
    loadmicrostore();
}

int readprogram(void)
{int address,instruction,progend,action,i;
    char longfilename[100] ="/UMD-Web-Dreamweaver/teaching/courses/enee350/vesp-source-code/vesp2.1XM/", filename[25];
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
        {for (i=0; i <= 24; i++) longfilename[i+73] = filename[i];
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

void maincycle(int microtrace)
{     cout << "Machine Cycle " << j << ":  ";
    j = j+1;
    //AYO: Fetch Step
    cout << "PC = ";  cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
    cout << hex << (vesp.PC & 0x0FFFFFFF) << ", ";
    if(microtrace == 1)
        cout << "\nFETCH SUBCYCLE-MICRO\n";
    else cout << "\nFETCH SUBCYCLE\n";
    fetch(microtrace);
    cout << "\nClock cycle = " << vesp.clock << "\n";
    //AYO: Decode Step
    cout << "DECODE SUBCYCLE\n";
    decode();
    cout << "Clock cycle = " << vesp.clock << "\n";
    //AYO: Execute Step
    if(microtrace == 1)
        cout << "\nEXECUTE SUBCYCLE-MICRO\n";
    else cout << "\nEXECUTE SUBCYCLE\n";
    vesp.add = vesp.cmp = 0;
    execute(microtrace);
    cout << "\nClock cycle = " << vesp.clock << "\n\n";
    //AYO: Display the registers
    
    //   if(trace == 1)
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

void fetch(int microtrace)
{
    micro_vesp.PC = micro_vesp.fetch_address;
    vesp.RET_SET = 0;
    microsequencer(microtrace);
    
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

void execute(int microtrace)
{
    switch(vesp.IR >> 12)
    {//clock cycle 3.
            //Add
        case  0:
            switch(vesp.IR & 0xF)
        {
            case 0: // set micro_PC to add microcode and call microsequencer
                micro_vesp.PC = micro_vesp.add_address; break;
            case 1: // set micro_PC to sub microcode and call microsequencer
                micro_vesp.PC = micro_vesp.sub_address; break;
            case 2: // set micro_PC to adb microcode and call microsequencer
                micro_vesp.PC = micro_vesp.adb_address; break;
            case 3: // set micro_PC to sua microcode and call microsequencer
                micro_vesp.PC = micro_vesp.sua_address; break;
        }
            
            //Complement
        case  1: // set micro_PC to complement microcode and call microsequencer
            micro_vesp.PC = micro_vesp.comp_address; break;
            //Load
        case  2: // set micro_PC to load microcode and call microsequencer
            micro_vesp.PC = micro_vesp.load_address; break;
            //Move
        case  3: // set micro_PC to move microcode and call microsequencer
            micro_vesp.PC = micro_vesp.move_address; break;
            //Jump
        case  4: // set micro_PC to jump microcode and call microsequencer
            micro_vesp.PC = micro_vesp.jump_address; break;
            //Branch if A = 0
        case  5: // set micro_PC to branch if A=0 microcode and call microsequencer
            micro_vesp.PC = micro_vesp.branchzero_address; break;
            //Branch if A is > 0
        case  6: // set micro_PC to branch if A>0 microcode and call microsequencer
            micro_vesp.PC = micro_vesp.branchpositive_address; break;
            //Halt
        case  7: // set micro_PC to halt microcode and call microsequencer
            micro_vesp.PC = micro_vesp.halt_address;break;
            //Increment
        case  8: // set micro_PC to increment microcode and call microsequencer
            micro_vesp.PC = micro_vesp.inc_address;break;
            //Decrement
        case  9: // set micro_PC to decrement microcode and call microsequencer
            micro_vesp.PC = micro_vesp.dec_address;break;
            //AND
        case  10: // set micro_PC to AND microcode and call microsequencer
            micro_vesp.PC = micro_vesp.and_address;break;
            //OR
        case  11: // set micro_PC to OR microcode and call microsequencer
            micro_vesp.PC = micro_vesp.or_address;break;
            //Shift left
        case  12: // set micro_PC to shift left microcode and call microsequencer
            micro_vesp.PC = micro_vesp.shl_address;break;
            //Shift right
        case  13: // set micro_PC to shift right microcode and call microsequencer
            micro_vesp.PC = micro_vesp.shr_address;break;
            //MXF
        case  14: // set micro_PC to MXF microcode and call microsequencer
            micro_vesp.PC = micro_vesp.mxf_address;break;
            //MXT
        case  15: // set micro_PC to MXT microcode and call microsequencer
            micro_vesp.PC = micro_vesp.mxt_address;break;
            
    }
    vesp.RET_SET = 0;
    microsequencer(microtrace);
}

void microsequencer(int microtrace)
{char function[16][16] = {"load A","load B","add","complement A","complement B","logical AND","logical OR",
    "shift right","shift left","reserved","reserved","reserved","reserved","reserved","reserved","reserved"};
    char destination[16][16] = {"A","PC","IR","IX","MDR","MAR","B",
        "RET_SET","C","S","Z","F","memory_read","memory_write","vesp_reset","undefined"};
    while(vesp.RET_SET == 0)
    {
        /* Step 1: Read the next microinstruction into MIR */
        micro_vesp.MAR = micro_vesp.PC;  micro_vesp.PC  = micro_vesp.PC + 1;
        micro_vesp.MIR = micro_vesp.MICSTORE[micro_vesp.MAR];
        if(microtrace == 1)
        {cout << "\nMIR= ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
            cout << hex << (micro_vesp.MIR)<< "\n";
        }
        
        /*
         Step 2: Decode and execute the microinstruction
         */
        switch(micro_vesp.MIR & 0xC000)
        {
            case 0x0000: // transfer Âµ-microinstruction
                switch(micro_vesp.MIR & 0x3800) //ABUS select
            {
                case 0x0000: micro_vesp.ABUS = 0; break;  //select 0 into the ABUS
                case 0x0800: micro_vesp.ABUS = vesp.MEMORY[0]; break; //select A into the ABUS
                case 0x1000: micro_vesp.ABUS = vesp.PC; break; //select PC into the ABUS
                case 0x1800: micro_vesp.ABUS = vesp.MAR; break; //select MAR into the ABUS
                case 0x2000: micro_vesp.ABUS = 1; break; //select 1 into the ABUS
            }
                switch(micro_vesp.MIR & 0x0700) //BBUS select
            {
                case 0x0000: micro_vesp.BBUS = vesp.MDR; break;//select MDR into the BBUS
                case 0x0100: micro_vesp.BBUS = vesp.IR; break; //select IR into the BBUS
                case 0x0200: micro_vesp.BBUS = vesp.MEMORY[1]; break; //select B into the BBUS
                case 0x0300: micro_vesp.BBUS = 1; break; //select 1 into the BBUS
                case 0x0400: micro_vesp.BBUS = vesp.MEMORY[2]; break; //select IX into the BBUS
            }
                
                switch(micro_vesp.MIR & 0x00F0) /* Function select */
            {
                case 0x0000: micro_vesp.OBUS = micro_vesp.ABUS; break;
                case 0x0010: micro_vesp.OBUS = micro_vesp.BBUS; break;
                case 0x0020: micro_vesp.OBUS = micro_vesp.ABUS + micro_vesp.BBUS;
                    
                    if (micro_vesp.MIR & 0x000F == 0x0000) //set flags only if OBUS writes into A.
                    { vesp.F =
                        (micro_vesp.ABUS & 0x8000 == 0x0000) &&
                        (micro_vesp.BBUS & 0x8000 == 0x0000) &&
                        (micro_vesp.OBUS & 0x8000 == 0x8000)
                        ||
                        (micro_vesp.ABUS &0x8000 == 0x8000 ) &&
                        (micro_vesp.BBUS &0x8000 == 0x8000 ) &&
                        (micro_vesp.OBUS &0x8000 == 0);
                        
                        vesp.C =
                        (micro_vesp.ABUS & 0x8000 == 0x8000) && (micro_vesp.BBUS & 0x8000 == 0x8000)
                        ||
                        (micro_vesp.ABUS & 0x8000 == 0x0000) && (micro_vesp.BBUS & 0x8000 == 0x8000)
                        && (micro_vesp.OBUS & 0x8000 == 0x0000)
                        ||
                        (micro_vesp.ABUS & 0x8000 == 0x8000) &&(micro_vesp.BBUS & 0x8000 == 0x0000)
                        && (micro_vesp.OBUS & 0x8000 == 0x0000);
                    }
                    break;
                    
                case 0x0030: micro_vesp.OBUS = ~micro_vesp.ABUS; break;
                case 0x0040: micro_vesp.OBUS = micro_vesp.ABUS + ~micro_vesp.BBUS + 1;
                    
                    if (micro_vesp.MIR & 0x000F == 0x0000) //set flags only if OBUS writes into A.
                    { vesp.F =
                        (micro_vesp.ABUS & 0x8000 == 0x0000) &&
                        (~(micro_vesp.BBUS+1) & 0x8000 == 0x0000) &&
                        (micro_vesp.OBUS & 0x8000 == 0x8000)
                        ||
                        (micro_vesp.ABUS &0x8000 == 0x8000 ) &&
                        (~(micro_vesp.BBUS+1) &0x8000 == 0x8000 ) &&
                        (micro_vesp.OBUS &0x8000 == 0);
                        
                        vesp.C =
                        (micro_vesp.ABUS & 0x8000 == 0x8000) && (~(micro_vesp.BBUS+1) & 0x8000 == 0x8000)
                        ||
                        (micro_vesp.ABUS & 0x8000 == 0x0000) && (~(micro_vesp.BBUS+1) & 0x8000 == 0x8000)
                        && (micro_vesp.OBUS & 0x8000 == 0x0000)
                        ||
                        (micro_vesp.ABUS & 0x8000 == 0x8000) &&(~(micro_vesp.BBUS+1) & 0x8000 == 0x0000)
                        && (micro_vesp.OBUS & 0x8000 == 0x0000);
                    }
                    
                    
                    break;
                    
                case 0x0050: micro_vesp.OBUS = micro_vesp.ABUS & micro_vesp.BBUS; break;
                case 0x0060: micro_vesp.OBUS = micro_vesp.ABUS | micro_vesp.BBUS; break;
                case 0x0070: micro_vesp.OBUS = micro_vesp.ABUS >> 1; break;
                case 0x0080: micro_vesp.OBUS = micro_vesp.ABUS << 1; break;
                    
            }
                
                
                switch(micro_vesp.MIR & 0x000F) /* Destination select */
            {
                case 0x0000: vesp.MEMORY[0] = micro_vesp.OBUS; //save OBUS into A
                    //Set the sign flag
                    vesp.S = ((micro_vesp.OBUS & 0x8000) == 0x8000);
                    //Set the zero flag
                    vesp.Z = (micro_vesp.OBUS == 0);
                    break;
                case 0x0001: vesp.PC =        micro_vesp.OBUS & 0x1FFF; break; //save OBUS into PC
                case 0x0002: vesp.IR =        micro_vesp.OBUS; break; //save OBUS into IR
                case 0x0003: vesp.MEMORY[2] = micro_vesp.OBUS; break; //save OBUS into IX
                case 0x0004: vesp.MDR =       micro_vesp.OBUS; break; //save OBUS into MDR
                case 0x0005: vesp.MAR =       micro_vesp.OBUS& 0x0FFF; break; //save OBUS into MAR
                case 0x0006: vesp.MEMORY[1] = micro_vesp.OBUS; break; //save OBUS into B
                case 0x0007: vesp.RET_SET =   micro_vesp.OBUS &0x0001; break;//set/clear RET_SET.
                case 0x0008: vesp.C =         micro_vesp.OBUS &0x0001; break; //set/clear C.
                case 0x0009: vesp.Z =         micro_vesp.OBUS &0x0001; break; //set/clear Z.
                case 0x000A: vesp.S =         micro_vesp.OBUS &0x0001; break; //set/clear S.
                case 0x000B: vesp.F =         micro_vesp.OBUS &0x0001; break; //set/clear F.
                case 0x000C: vesp.memory_read=micro_vesp.OBUS &0x0001;
                    micro_vesp.clock = micro_vesp.clock +1; // allow one micro_vesp clock cycle for the read.
                    vesp.MDR = vesp.MEMORY[vesp.MAR];
                    break; //For memory read;
                case 0x000D: vesp.memory_write=micro_vesp.OBUS &0x0001;
                    vesp.MEMORY[vesp.MAR] = vesp.MDR; //For memory write;
                    micro_vesp.clock = micro_vesp.clock +1; // allow one micro_vesp clock cycle for the write.
                    break;
                case 0x000E: vesp.reset=micro_vesp.OBUS &0x0001;  break;
                case 0x000F: break; //TBD
            }
                if(microtrace==1)
                {cout << "\nABUS= ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
                    cout << hex << (micro_vesp.ABUS)<< ", ";
                    cout << "BBUS= ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
                    cout << hex << (micro_vesp.BBUS)<< ", ";
                    cout << "OBUS= ";  cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
                    cout << hex << (micro_vesp.OBUS)<< ", ";
                    
                    cout << destination[(micro_vesp.MIR) & 0x000F]<< " = OBUS";
                    cout << "\nFunction = ";
                    cout << function[(micro_vesp.MIR >> 4) & 0x000F]<< "\n";
                }
                break;
                
            case 0x4000: // branch Âµ-microinstruction
                
                switch(micro_vesp.MIR & 0x3C00)
            {
                case 0x0000:
                    /* branch if A is positive*/
                    if(vesp.S == 0 && vesp.Z != 1)
                    {micro_vesp.PC = 0x03FF & micro_vesp.MIR;
                        cout << "\nA > 0 and branch is taken ";
                    }
                    else
                    {//micro_vesp.MAR = micro_vesp.PC + 1;
                        cout << "\nA <= 0 and branch is not taken ";
                    }
                    break;
                    
                case 0x0400:
                    /* branch if A is 0 */
                    if(vesp.Z == 1)
                    {micro_vesp.PC = 0x03FF & micro_vesp.MIR;
                        cout << "\nA = 0 and branch is taken ";
                    }
                    else
                    {//micro_vesp.PC = micro_vesp.PC + 1;
                        cout << "\nA <> 0 and branch is not taken ";
                    }
                    break;
                    
                case 0x0800:
                    /* branch if A is negative */
                    if(vesp.S == 1)
                    {micro_vesp.PC = 0x03FF & micro_vesp.MIR;
                        cout << "\nA < 0 and branch is taken ";}
                    else
                    {//micro_vesp.PC = micro_vesp.PC + 1;
                        cout << "\nA >= 0 and branch is not taken ";}
                    break;
                    
                case 0x0C00:
                    /* always branch */
                    micro_vesp.PC = 0x03FF & micro_vesp.MIR;
                    break;
                    
                case 0x1000: vesp.RET_SET = 1; break; /* exit */
            }
                
        }
        micro_vesp.clock = micro_vesp.clock +1;
    }
    vesp.clock = vesp.clock + micro_vesp.clock;
    if(microtrace==1)
    {cout << "\nRET_SET= ";  cout.fill('0'); cout.width(1); cout.setf(ios::uppercase);
        cout << hex << (vesp.RET_SET )<< "\n";
    }
}

void loadmicrostore(void)
{//initialize the instruction micro-routines' starting addresses
    cout << "initializing micro call addresses\n";
    micro_vesp.fetch_address= 0;
    micro_vesp.add_address  = 5;
    micro_vesp.comp_address = 7;
    micro_vesp.load_address = 9;
    micro_vesp.move_address = 15;
    micro_vesp.jump_address = 23;
    micro_vesp.branchzero_address = 25;
    micro_vesp.branchpositive_address = 29;
    micro_vesp.halt_address = 33;
    micro_vesp.inc_address  = 35;
    micro_vesp.dec_address  = 37;
    micro_vesp.and_address  = 39;
    micro_vesp.or_address   = 41;
    micro_vesp.shl_address  = 43;
    micro_vesp.shr_address  = 45;
    micro_vesp.mxf_address  = 47;
    micro_vesp.mxt_address  = 53;
    micro_vesp.sub_address  = 59;
    micro_vesp.adb_address  = 61;
    micro_vesp.sua_address  = 63;
    
    
    /*
     Fetch
     */
    //begin fetch microprogram
    // ABUS = PC;  BBUS = N/A; OBUS = ABUS; MAR = OBUS;
    micro_vesp.MICSTORE[0x000] = 0x1005; //00 010 xxx  0000 0100;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_read = OBUS; issue a memory read
    micro_vesp.MICSTORE[0x001] = 0x031C; //00 000 011  0001 1100;
    // ABUS = N/A;  BBUS = MDR; OBUS = BBUS; IR = OBUS;
    micro_vesp.MICSTORE[0x002] = 0x0012; //00 xx x 000  0001 0010;
    // ABUS = PC; BBUS = 1; OBUS = ABUS+BBUS; PC = OBUS; PC = PC+1;
    micro_vesp.MICSTORE[0x003] = 0x1321; //00 010 011  0010 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x004] = 0x0317;//00 000 011  0001 0111 ;
    //end fetch microprogram
    
    /*
     Add
     Flags are handled in the microsequencer. See the add code
     in the function select section of microsequencer();
     */
    cout << "loading ADD microprogram into vesp's microstore\n";
    //begin  add microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS + BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x005] = 0x0A20; //00 001 010  0010 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x006] = 0x0317;//00 000 011  0001 0111 ;
    //end add microprogram
    
    /*
     Complement
     Flags are handled in the microsequencer. See the assignment of OBUS into A code in
     the microsequencer.
     */
    cout << "loading CMP microprogram into vesp's microstore\n";
    //begin complement microprogram
    // ABUS = A;  BBUS = N/A; OBUS = ~ABUS; A = OBUS;
    micro_vesp.MICSTORE[0x007] = 0x0830; //00 001 xxx  0011 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x008] = 0x0317;//00 000 011  0001 0111 ;
    //end complement
    
    /* Load
     Flags are handled in the microsequencer. See the assignment of OBUS into A code in
     the microsequencer.
     */
    cout << "loading LDA microprogram into vesp's microstore\n";
    //begin load microprogram
    // ABUS = MAR; BBUS = 1; OBUS = ABUS+BBUS; MAR = OBUS; MAR = MAR+1
    micro_vesp.MICSTORE[0x009] = 0x1B25; //00 011 011  0010 0101;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_read = OBUS; issue a memory read
    micro_vesp.MICSTORE[0x00A] = 0x031C; //00 000 011  0001 1100;
    //ABUS = N/A; BBUS = IR; OBUS = BBUS; MAR = OBUS; MAR = IR&0x0FFF;
    micro_vesp.MICSTORE[0x00B] = 0x0115; //00 000 001  0001 0101;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_write = OBUS; issue a memory write
    micro_vesp.MICSTORE[0x00C] = 0x011D; //00 000 001  0001 1101;
    // ABUS = PC; BBUS = 1; OBUS = ABUS+BBUS; PC = OBUS; PC = PC+1
    micro_vesp.MICSTORE[0x00D] = 0x1321; //00 010 011  0010 0001;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x00E] = 0x0317;//00 000 011  0001 0111 ;
    //end load
    
    /*
     Move
     Flags are handled in the microsequencer. See the assignment of OBUS into A code in
     the microsequencer.
     */
    cout << "loading MOV microprogram into vesp's microstore\n";
    //begin move microprogram
    // ABUS = MAR; BBUS = 1; OBUS = ABUS+BBUS; MAR = OBUS; MAR = MAR+1
    micro_vesp.MICSTORE[0x00F] = 0x1B25; //00 011 011  0010 0101;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_read = OBUS; issue a memory read
    micro_vesp.MICSTORE[0x010] = 0x031C; //00 000 011  0001 1100;
    //ABUS = N/A; BBUS = MDR; OBUS = BBUS; MAR = OBUS; MAR = IR&0x0FFF;
    micro_vesp.MICSTORE[0x011] = 0x0015; //00 000 000  0001 0101;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_read = OBUS; issue a memory read
    micro_vesp.MICSTORE[0x012] = 0x031C; //00 000 011  0001 1100;
    //ABUS = N/A; BBUS = IR; OBUS = BBUS; MAR = OBUS; MAR = IR&0x0FFF;
    micro_vesp.MICSTORE[0x013] = 0x0115; //00 000 001  0001 0101;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_write = OBUS; issue a memory write
    micro_vesp.MICSTORE[0x014] = 0x011D; //00 000 001  0001 1101;
    // ABUS = PC; BBUS = 1; OBUS = ABUS+BBUS; PC = OBUS; PC = PC+1
    micro_vesp.MICSTORE[0x015] = 0x1321; //00 010 011  0010 0001;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x016] = 0x0317;//00 000 011  0001 0111 ;
    //end move
    
    /* Jump */
    cout << "loading JMP microprogram into vesp's microstore\n";
    // beginjump
    // ABUS = N/A; BBUS = IR; OBUS = BBUS; PC = OBUS; PC = IR;
    micro_vesp.MICSTORE[0x017] = 0x1321; //00 010 001  0001 0001;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x018] = 0x0317;//00 000 011  0001 0111 ;
    //end jump
    
    /* Jump if zero */
    cout << "loading JEZ microprogram into vesp's microstore\n";
    // begin jump if A = 0
    //Branch if A = 0 microinstruction
    micro_vesp.MICSTORE[0x019] = 0x441B; //01 0001 00  0001 1011 ;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x01A] = 0x0317; //00 000 011  0001 0111 ;
    // ABUS = N/A; BBUS = IR; OBUS = BBUS; PC = OBUS; PC = IR;
    micro_vesp.MICSTORE[0x01B] = 0x1321; //00 010 001  0001 0001;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x01C] = 0x0317; //00 000 011  0001 0111 ;
    // end jump if A = 0
    
    /* Jump if positive */
    cout << "loading JPS microprogram into vesp's microstore\n";
    
    // begin jump if A is > 0
    //Branch if A > 0 microinstruction
    micro_vesp.MICSTORE[0x01D] = 0x401F;//01 0000 00  0001 1111 ;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x01E] = 0x0317;//00 000 011  0001 0111 ;
    // ABUS = N/A; BBUS = IR; OBUS = BBUS; PC = OBUS; PC = IR;
    micro_vesp.MICSTORE[0x01F] = 0x1321; //00 010 001  0001 0001;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x020] = 0x0317;//00 000 011  0001 0111 ;
    //end  jump if A is > 0
    
    /* Halt */
    cout << "loading HLT microprogram into vesp's microstore\n";
    //begin halt
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.reset = OBUS;
    micro_vesp.MICSTORE[0x021] = 0x031E;//00 000 011  0001 1110 ;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x022] = 0x0317;//00 000 011  0001 0111 ;
    //end halt
    
    /*
     Increment
     Flags are handled in the microsequencer. See the add code
     in the function select section of microsequencer();
     */
    cout << "loading INC microprogram into vesp's microstore\n";
    //begin  increment microprogram
    // ABUS = A;  BBUS = 1; OBUS = ABUS + BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x023] = 0x0B20; //00 001 011  0010 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x024] = 0x0317; //00 000 011  0001 0111 ;
    //end increment microprogram
    
    /*
     Decrement
     Flags are handled in the microsequencer. See the add code
     in the function select section of microsequencer();
     */
    cout << "loading DEC microprogram into vesp's microstore\n";
    //begin  decrement microprogram
    // ABUS = A;  BBUS = 1; OBUS = ABUS + ~BBUS + 1; A = OBUS;
    micro_vesp.MICSTORE[0x025] = 0x0B40; //00 001 011  0100 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x026] = 0x0317; //00 000 011  0001 0111 ;
    //end decrement microprogram
    
    /* And */
    cout << "loading AND microprogram into vesp's microstore\n";
    //begin  AND microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS & BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x027] = 0x0A50; //00 001 010  0101 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x028] = 0x0317; //00 000 011  0001 0111 ;
    //end AND microprogram
    
    /* Or */
    cout << "loading IOR microprogram into vesp's microstore\n";
    //begin OR microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS | BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x029] = 0x0A60; //00 001 010  0110 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x02A] = 0x0317; //00 000 011  0001 0111 ;
    //end OR microprogram
    
    /* Shift left */
    cout << "loading SHL microprogram into vesp's microstore\n";
    //begin OR microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS << 1; A = OBUS;
    micro_vesp.MICSTORE[0x02B] = 0x0A70; //00 001 010  0111 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x02C] = 0x0317; //00 000 011  0001 0111 ;
    //end OR microprogram
    
    /* Shift right */
    cout << "loading SHR microprogram into vesp's microstore\n";
    //begin SHR microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS >> 1; A = OBUS;
    micro_vesp.MICSTORE[0x02D] = 0x0A80; //00 001 010  1000 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x02E] = 0x0317; //00 000 011  0001 0111 ;
    //end SHR microprogram
    
    /* MXF */
    cout << "loading MXF microprogram into vesp's microstore\n";
    //begin MXF microprogram
    // ABUS = N/A;  BBUS = IX; OBUS = BBUS; MAR = OBUS;  MAR = IX
    micro_vesp.MICSTORE[0x02F] = 0x0415; //00 000 100  0001 0101;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_read = OBUS; issue a memory read
    micro_vesp.MICSTORE[0x030] = 0x031C; //00 000 011  0001 1100;
    //ABUS = N/A; BBUS = IR; OBUS = BBUS; MAR = OBUS; MAR = IR&0x0FFF;
    micro_vesp.MICSTORE[0x031] = 0x0115; //00 000 001  0001 0101;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_write = OBUS; issue a memory write
    micro_vesp.MICSTORE[0x032] = 0x011D; //00 000 001  0001 1101;
    // ABUS = 1;  BBUS = IX; OBUS = ABUS +  BBUS; IX = OBUS;
    micro_vesp.MICSTORE[0x033] = 0x2423; //00 100 100  0010 0011;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x034] = 0x0317; //00 000 011  0001 0111 ;
    //end MXF microprogram
    
    /* MXT */
    cout << "loading MXT microprogram into vesp's microstore\n";
    //begin MXT microprogram
    // ABUS = N/A;  BBUS = IR; OBUS = BBUS; MAR = OBUS;
    micro_vesp.MICSTORE[0x035] = 0x0115; //00 000 001  0001 0101;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_read = OBUS; issue a memory read
    micro_vesp.MICSTORE[0x036] = 0x031C; //00 000 011  0001 1100;
    //ABUS = N/A; BBUS = IX; OBUS = BBUS; MAR = OBUS; MAR = IX;
    micro_vesp.MICSTORE[0x037] = 0x0115; //00 000 100  0001 0011;
    //ABUS = N/A; BBUS = 1; OBUS = BBUS; vesp.memory_write = OBUS; issue a memory write
    micro_vesp.MICSTORE[0x038] = 0x011D; //00 000 001  0001 1101;
    // ABUS = 1;  BBUS = IX; OBUS = ABUS +  BBUS; IX = OBUS;
    micro_vesp.MICSTORE[0x039] = 0x2423; //00 100 100  0010 0011;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x03A] = 0x0317; //00 00 0 011  0001 0111 ;
    //end MXF microprogram
    
    /*
     Sub
     Flags are handled in the microsequencer. See the sub code
     in the function select section of microsequencer();
     */
    cout << "loading SUB microprogram into vesp's microstore\n";
    //begin   microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS - BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x03B] = 0x0A40; //00 001 010  0100 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x03C] = 0x0317;//00 000 011  0001 0111 ;
    //end sub microprogram
    
    
    
    /*
     Adb
     Flags are handled in the microsequencer. See the adb code
     in the function select section of microsequencer();
     */
    cout << "loading ADB microprogram into vesp's microstore\n";
    //begin  adb microprogram
    // ABUS = A;  BBUS = B; OBUS = ABUS + BBUS; B = OBUS;
    micro_vesp.MICSTORE[0x03D] = 0x0A26; //00 001 010  0010 0110;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x03E] = 0x0317;//00 000 011  0001 0111 ;
    //end adb microprogram
    
    /*
     Sua
     Flags are handled in the microsequencer. See the sua code
     in the function select section of microsequencer();
     */
    cout << "loading SUA microprogram into vesp's microstore\n";
    //begin   microprogram
    // ABUS = A;  BBUS = N/A; OBUS = ~ABUS; A = OBUS;
    micro_vesp.MICSTORE[0x03F] = 0x0830; //00 001 000  0011 0000;
    // ABUS = A;  BBUS = 1; OBUS = ABUS + BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x040] = 0x0B20; //00 001 011  0010 0000;
    // ABUS = A;  BBUS = B; OBUS = ABUS + BBUS; B = OBUS;
    micro_vesp.MICSTORE[0x041] = 0x0A26; //00 001 010  0010 0110;
    // ABUS = A;  BBUS = N/A; OBUS = ~ABUS; A = OBUS;
    micro_vesp.MICSTORE[0x042] = 0x0830; //00 001 000  0011 0000;
    // ABUS = A;  BBUS = 1; OBUS = ABUS + BBUS; A = OBUS;
    micro_vesp.MICSTORE[0x043] = 0x0B20; //00 001 011  0010 0000;
    // ABUS = N/A; BBUS = 1; OBUS = BBUS; RET_SET = OBUS;
    micro_vesp.MICSTORE[0x044] = 0x0317;//00 000 011  0001 0111 ; 
    //end sua microprogram
    
}