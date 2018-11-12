////////////////////////////////////////////////////////////////////////
/////////////////////Computer Systems Lab 2/////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////Dylan Banh, Marisa Liu, Solomon Wang/////////////////////
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/*all assembly language in lowercase, dont worry about upper
everything is in offsets
**offsets(in terms of words) are in relation to the next line of where they are called

read through fgets
instructions limited to 100 characters

2 modes
  single cycle mode
  program executes cycle by cycle
  displays contents of 31 registers after each cycle and program counter
  moves on to next cycle after some key is hit

  batch mode
  entire program is executed
  statistics and register contents output at end
  statistics - utilization of pipeline stage and total execution time
*/
//code should include:

char *progScanner(char* currentLine){
///////remove punctuation first
char copy[strlen(currentLine)];   //make empty array of size currentLine
int i;
int pos=0;
for(i=0; i<strlen(currentLine); i++){   //loops through and only copy stuff we want
	if((currentLine[i] != ',') && (currentLine[i] != '(') && (currentLine[i] != ')')){
	copy[pos] = currentLine[i];
	pos++;
	}
}

strcpy(currentLine, copy);     //copy end result to currentLine

///////////remove and leave only 1 space
  char *from , *to;
  int space=0;
  to=from=currentLine;      

  while(1){
    if(space && *from == ' ' && to[-1] == ' ') 
    ++from;
    else{
      space = (*from==' ')? 1 : 0;
      *to++ = *from++;
      if(!to[-1])break;
    }
  } 
  return currentLine;
}

////////////Enumeration type describing opcodes////////////
//enum inst{ADD, ADDI, SUB, MULT, BEQ, LW, SW};                                 //we have to add this but i'm not sure what it's for since we already using structs


///Structure that holds data about the type of instruction 
struct inst
{
  char* opcode;
  int rs;
  int rt;
  int rd;
  int Imm;
};
///////////////////////////////////////////// 
char *regNumberConverter(){
   /*goes through line until reaches $ 
    * take the string to the right of the $ until whitespace
    *if(t s a v k)
        if second letter is number
	  if no third letter convert register to appropriate number
	    if(first letter is t)
	       if(second letter is 0 - 7) reg = 8 + second letter
	       else reg = 16 + second letter
	    elseif(first letter is v) 
	       if(second letter is 0 or 1) reg = 2 + second letter
	       else report error
	    elseif(first letter is a)
	      if(second letter is 0-3) reg = 4 + second letter
	      else report error
	    elseif(first letter is s)
	      if(second letter is 0 - 7) reg = 16 + second letter
	      else report error
	    else
	      if(second letter is 0 or 1) reg = 26 + second letter
	      else report error
	  else report error
	else if(string is at) convert to 1
	else if(string is sp) convert to 29
	else report error
     else if(gp) convert to 28
     else if(fp) convert to 30
     else if(ra) convert to 31
     else if(zero) convert to 0
     else if(0 - 31) delete $ from before 
     else return error
    * */

}

////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Error functions for Parser/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void Error_IllegalOpcode(){
printf("%s", "Error: Illegal opcode!\n");
exit(1);
}

void Error_ImmediateField(){
printf("%s", "Error: Immediate Field contains number too large to store in 16 bits!\n");
exit(1);
}

void Error_InvalidRegister(){
printf("%s", "Error: Invalid register!\n");
exit(1);
}

void Error_MemoryMisalignment(){
printf("%s", "Error: Memory misalignment!\n");
exit(1);
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PARSER/////////////////////////////////////////////////////
//////Parameter --> output of regNumberConvereter()///////////////////////////////////
//////Return --> inst struct with fields for each of the files of MIPS instructions///
//////////////////////////////////////////////////////////////////////////////////////
struct inst parser(char *line){
  struct inst *newInst;
  char *p;                //store each section after parse
  char *instrname;         //name of instruction
  int arg[3];             //integer array of argument values
  int c=0, a=0;               //counters
  p = strtok (line, " ");      //get first argument (should be name of instruction)
  while (p != NULL){            //loop through until done
    if(c==0){                       //if first arg, put it into instrname
    instrname = p;
    }
    else{                          //convert p to integer and put it in arg array
      int ll = strlen(p);             //get length of argument
      int isD = 0, i;   
      for(i=0; i < ll; i++){
	if(isdigit(p[i]))
	  isD = 1;                   //check if each character is a digit
	else
	  isD = 0;
      }
      
      if(isD == 1){        //check that each argument is a number, if not return error
        if(ll == 1){
	  arg[a] = p[0] - '0';
	}
	else if(ll <= 5){           //arugment is less than 5 characters (65535 is 5 characters)
        arg[a] = atoi(p);
	}
	else{
		Error_ImmediateField();                //throw immediate field error 
	}
	a++;                           //increment counter for index
      }
      else
      {
	    Error_InvalidRegister();      //return not valid number error(or missing the $, ex. s0 instead of $s0 so it didn't get converted)
      }
    }
    p = strtok (line, " ");          //get next section
    c++;         
  }

  //parse by whitespace and create an array of strings/numbers
   //if(index 0 is ADD, SUB, MUL) --> contains opcode, rs, rt, rd, funct (r)
  if((strcmp(instrname, "add")==0) || (strcmp(instrname, "sub")==0) || (strcmp(instrname, "mul")==0)){      //if opcode is "add" or "sub" or "mul"
  int i;
    for(i=0;i < 3; i++)
    {
	  if((arg[i] <= 31) && (arg[i] >= 0)){
	    newInst->opcode = instrname;
	    newInst->rs = arg[1];
	    newInst->rt = arg[2];
	    newInst->rd = arg[0];
	  }
	  else{
		Error_InvalidRegister();            //throw invalid register error
		break;
	  }
    }

  }
  else if((strcmp(instrname, "lw")==0) || (strcmp(instrname, "sw")==0)){              // if opcode is "lw" or "sw"
    if((arg[0] <= 31) && (arg[0] >= 0)){
	newInst->rt = arg[0];
      }
    else{
	Error_InvalidRegister();           //return invalid registor error
    }
	

    if((arg[2] <= 31) && (arg[2] >= 0)){
	newInst->rs = arg[2];
    }
    else{
	Error_InvalidRegister();             //return invalid registor error
    }


    if((arg[1] <= 65535) && (arg[1] >= 0)){
	if((arg[1] % 4) == 0){
	  newInst->Imm = arg[1];
        }
        else{
	  Error_MemoryMisalignment();           //return error memory misalignment error
        }
    }
    else{
	Error_ImmediateField();                   //return error, either too small or bigger than largest unsigned 16-bit number
    }
  }
  else if ((strcmp(instrname, "addi")==0) || (strcmp(instrname, "beq")==0)){                           // if opcode is "beq" or "addi"
     if((arg[0] <= 31) && (arg[0] >= 0)){
	newInst->rt = arg[0];
      }
    else{
	Error_InvalidRegister();                                         //return invalid registor error
    }
	

    if((arg[1] <= 31) && (arg[1] >= 0)){
	newInst->rs = arg[1];
    }
    else{
	Error_InvalidRegister();                                   //return invalid registor error
    }

    if((strcmp(instrname, "addi")==0) && (arg[2] < 65535)){
	newInst->Imm = arg[2];
    }
    else{
	Error_ImmediateField();                                //return immediate field too big
    }

    if((strcmp(instrname, "beq")==0) && ((arg[2]%4)==0)){
	if(arg[2] < 65535){
		newInst->Imm = arg[2];
	}
	else{
		Error_ImmediateField();                //return too large immediate field
	}

    }
    else{
	Error_MemoryMisalignment();           // return memory misalignment error
    }
 
  }
  else{
	Error_IllegalOpcode();              //return illegal opcode error because it's not any of the valid ones
  }



}



/*char *regNumberConverterZ(char *l){
int pos=0;
while(l[pos] != '\0'){

}

}*/
 // takes in output of progScanner
  //returns pointer to character string, which all register names are converted to numbers
/*
struct inst parser()
  uses output of regNumberConverter()
  returns instruction as inst struct with fields for each of the fields of MIPS instructions
    eg. opcode, rs, rt, rd, Imm.
	 not all fields will be used for each line
  if illegal opcode, error in simulation and stops
*/


void IF(int currentInstrAddress){
//fetch from instruction memory
//freeze if unresolved branch
//if branch, finish branch first before continuing


}

void main(int argc, char *argv[])        //take in memory acces time cycles, multiply time cycles, all other EX operation cycles, filename 
{
assert(argc = 5);

int memaccesscycle = atoi(argv[1]);              //these were on the ppt but im not sure exactly what these are
int multiplytimecycle = atoi(argv[2]);
int exoperationcycle = atoi(argv[3]);
char filename = atoi(argv[4]);


int clockcount=0;                    //clock counter that represents the passage of time  
//char *filename;                 //name of file to read
FILE *fp;
//fp = fopen(filename, "r");
//char *currentLine;

int *instructionMemory;
instructionMemory = (int *)malloc(500 * sizeof(int));                //2000 bytes / 4 byte ints = 500 ints
int instructionAddress=0;

int *dataMemory;
dataMemory = (int *)malloc(500 * sizeof(int));
int dataAddress=0;




}


/*
void ID()
void EX()
void MEM()
void WB()
 all data, structural and control hazards must be taken into account
 several operations are multicycle
 stages themselves are not pipelined
   eg. if an add takes 4 cycles, next instruction cannot enter EX until cycles have elapsed



 */
















