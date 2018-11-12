////////////////////////////////////////////////////////////////////////
/////////////////////Computer Systems Lab 2/////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////Dylan Banh, Marisa Liu, Solomon Wang/////////////////////
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
//feel free to add here any additional library names you may need 
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

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
 
char *regNumberConverter(char *line){
  char *newLine = (char *)malloc(strlen(line)*sizeof(char));
  char regChar[3];
  int regNum;
  int pos=0;
  int newPos=0;
  for(pos=0; pos<strlen(line); pos++){
    if(line[pos] == '$'){ //do nothing until hit a $
      pos++;
	//check if z, a, v, t, k, g, s, p, r
       //cases
        if(line[pos] == 't'){ 
          if(isdigit(line[++pos])){		//check what second char is
	    if(!((line[pos+1] == ' ') || line[pos+1] == '\0')){return "1";} //|| (line[pos+1] == '\0')){return "1";} //check for no extra char
	    if((line[pos]-'0')<8){
	      regNum = 8+(line[pos++]-'0'); //register number in int
	      sprintf(regChar,"%d", regNum);
	      if(regNum > 9){					//if regNum double digit
	        newLine[newPos++] = regChar[0];
	        newLine[newPos++] = regChar[1];
	      }
	      else{
	        newLine[newPos++] = regChar[0];	//if single digi
	      }
	    }
	    else{
	      regNum = 16+(line[pos-1]-'0'); //register number in int
	      sprintf(regChar,"%d", regNum);
	      newLine[newPos++] = regChar[0];
	      newLine[newPos++] = regChar[1];
	    }
	  }
	  else{return "a";} //error if not a number
	}
	else if(line[pos] == 's'){				//check for s0-7 or sp
          if(isdigit(line[++pos])){		//if second char is number
	    if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))){return "2";} //check for no extra char
	    if((line[pos]-'0')<8){
	      regNum = 16+(line[pos++]-'0'); //register number in int
	      sprintf(regChar,"%d", regNum);	
	      newLine[newPos++] = regChar[0];
	      newLine[newPos++] = regChar[1];
	    }
	    else{return "b";}
	  }
	  else if(line[pos++] == 'p'){   //if sp
	    if(!((line[pos] == ' ') || (line[pos++] == '\0'))){return "3";} //check for no extra char
	    newLine[newPos++] = '2';
	    newLine[newPos++] = '9';
		  }
	    else{return "c";} //error if not a number
	   }
	  else if(line[pos] == 'a'){	//if a0-a3
            if(isdigit(line[++pos])){		
	      if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))){return "4";} //check for no extra char 
	      if((line[pos]-'0')<4){
	        regNum = 4+(line[pos++]-'0'); //register number in int
	        sprintf(regChar,"%d", regNum);
		newLine[newPos++] = regChar[0];
	      }
	      else{return "d";}
	    }
	    else if(line[pos] == 't'){
	      if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))) {return "17";}
	      pos++;
	      newLine[newPos++] = '1';
	    }
	    else{return "e";} //error if not a number
  	 }
	 else if(line[pos] == 'v'){ //if v0-v1
           if(isdigit(line[++pos])){		
	     if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))){return "5";} //check for no extra char
	     if((line[pos]-'0')<2){
	       regNum = 2+(line[pos++]-'0'); //register number in int
      	       sprintf(regChar,"%d", regNum);
	       newLine[newPos++] = regChar[0];
	     }
	     else{return "f";}
	   }
	   else{return "g";} //error if not a number
	 }
	 else if(line[pos] == 'k'){	//if k0-k1
           if(isdigit(line[++pos])){		
	     if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))){return "6";} //check for no extra char
	     if((line[pos]-'0')<2){
	       regNum = 26+(line[pos++]-'0'); //register number in int
	       sprintf(regChar,"%d", regNum);
	       newLine[newPos++] = regChar[0];
	       newLine[newPos++] = regChar[1];
	     }
	     else{return "h";}
	   }
	   else{return "i";} //error if not a number
	 }
	   //if doesnt begin with t, s, a, v, or k
	 else if(line[pos] == 'g'){ //check for gp
	  if(line[++pos] == 'p'){
	    if(!((line[pos+1] == ' ') || (line[pos++ +1] == '\0'))){return "7";} //check for no extra char
      	    pos++;
	    newLine[newPos++] = '2';
	    newLine[newPos++] = '8';
	  }
	  else{return "j";}
	 }
         else if(line[pos] == 'f'){ //check for fp
	   if(line[++pos] == 'p'){
	     if(!((line[pos+1] == ' ') || (line[pos++ +1] == '\0'))){return "8";} //check for no extra char
	     pos++;
	     newLine[newPos++] = '3';
	     newLine[newPos++] = '0';
	   }
	   else{return "k";}
	 }
	 else if(line[pos] == 'r'){	//check for ra
	   if(line[++pos] == 'a'){
	     if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))){return "9";} //check for no extra char
	     pos++;
	     newLine[newPos++] = '3';
	     newLine[newPos++] = '1';
	   }
	   else{return "l";}
	 }
	 else if(line[pos] == 'z'){	//check for zero
	   if(line[++pos] == 'e'){
	     if(line[++pos] == 'r'){
	       if(line[++pos] == 'o'){
	         if(!((line[pos+1] == ' ') || (line[pos++ +1] == '\0'))){return "10";} //check for no extra char
	         pos++; 
	         newLine[newPos++] = '0';
	       }
	       else{return "m";}
	     }
	     else{return "n";}
	   }
	   else{return "o";}
	 }
	 else if(isdigit(line[pos])){	//check if 0-31
	   if(isdigit(line[++pos])){
	     if(!((line[pos+1] == ' ') || (line[pos+1] == '\0'))){ return "11";}
	     regNum = 10*(line[pos-1]-'0');
	     regNum += (line[pos]-'0');
	       if(regNum > 31) return "p";
	       newLine[newPos++] = line[pos-1];
	       newLine[newPos++] = line[pos++];
	     }      
	   else if((line[pos] == ' ') || (line[pos] == '\0')) {
             regNum = (line[pos-1]-'0');
	     newLine[newPos++] = line[pos-1];
	   }
           else{ return "q";}
	 }
	 else{return "r";}			//return error if no matches  
      }  
      newLine[newPos++] = line[pos];
    }
  
    char *newNewLine = (char *)realloc(newLine, newPos*sizeof(char));
    return newNewLine;

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
//////Parameter --> output of regNumberConverter()///////////////////////////////////
//////Return --> inst struct with fields for each of the files of MIPS instructions///
//////////////////////////////////////////////////////////////////////////////////////

struct inst parser(char *line){
  struct inst *newInst;
  char *p;                //store each section after parse
  char *instrname;         //name of instruction
  int arg[3];             //integer array of argument values
  int c=0, a=0;               //counters
  
  //parse by whitespace and create an array of strings/numbers 
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
    
   //////////////actually check which instructions it is and put the arguments in the right variables in the inst
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

 
 /*take in file
 parse line by line
 should be able to take in names and numbers - eg. zero and 0
 remove duplicate commas, spaces, parenthesises, etc.
 keep reading until halt simulation*/
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


/*main (int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long mips_reg[REG_NUM];
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here
	
	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for(i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	if(argc==7){
		if(strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		}
		else if(strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		}
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
		
	}
	
	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	if(input==NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	if(output==NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	//initialize registers and program counter
	if(sim_mode==1){
		for (i=0;i<REG_NUM;i++){
			mips_reg[i]=0;
		}
	}
	
	//start your code from here
}*/
  

