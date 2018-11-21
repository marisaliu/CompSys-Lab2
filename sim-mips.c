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


///Structure that holds data about the type of instruction 
struct inst
{
  int opcode;
  int rs;
  int rt;
  int rd;
  int Imm;

};

 ///////////////////////////////////////////////////////////////////////
 /////////////////////GLOBAL VARIABLES//////////////////////////////////
 ///////////////////////////////////////////////////////////////////////
  struct inst IFIDLatch;
  struct inst IDEXLatch;
  struct inst EXMEMLatch;
  struct inst MEMWBLatch;
  int rawHaz[32]; //array of flags for each reg
  struct inst instMem[512];  
  int pc;
  struct inst EXout;
  struct inst MEMout;
  int *DMem;
  long reg[32]; //what is in each register
  int branchUnresolved;
  int IFcount;
  int IDcount;
  int EXcount;
  int MEMcount;
  int WBcount; 
  int halt;
  int mode; 
  int c,m,n;
  int pgm_c;
 
////////////Enumeration type describing opcodes////////////
//enum inst{ADD, ADDI, SUB, MULT, BEQ, LW, SW};                                 //we have to add this but i'm not sure what it's for since we already using structs


  
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////FUNCTIONS/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
/////////////////////////PROGSCANNER////////////////////////////////////////
//Input a string and get rid of any extra punction, spaces or parentheses///
////////////////////////////////////////////////////////////////////////////
char *progScanner(char* currentLine){
  printf("Input line: %s \n", currentLine);
  char copy[strlen(currentLine)+1];   //make empty array of size currentLine
  int i;
  int pos=0;

for(char *p = currentLine; *p; ++p)
  if((isalpha(*p) != 0) || (isdigit(*p) != 0) || (*p == '$') || (*p == ' '))
	currentLine[pos++] =*p;

  currentLine[pos]='\0';


printf("Removed punctuation: %s \n", currentLine);
///////////remove and leave only 1 space
  int x;
  for(i=x=0; currentLine[i]; ++i){
    if(!isspace(currentLine[i]) || (i > 0 && !isspace(currentLine[i-1]))) currentLine[x++] = currentLine[i];
  }
  currentLine[x] = '\0';
  printf("Fixed spaces: %s\n", currentLine);
  return currentLine;
}

////////////////////////////////////////////////////////////////////////////
////////////////////REGNUMBERCONVERTER//////////////////////////////////////
//Takes in input from progScanner() output and converts any registers that//
//start with $ to numbers and returns error if invalid register/////////////
///////////////////////////////////////////////////////////////////////////
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
	      regNum = 16+(line[pos++]-'0'); //register number in int
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
    printf("%s\n", newLine);
    return newNewLine;
//return newLine;
}

////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Error functions for Parser/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void Error_IllegalOpcode(){
printf("%s", "Error: Illegal opcode!\n");
exit(1);
}

void Error_ImmediateField(){
printf("%s", "Error: Immediate Field!\n");
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
//////Return --> inst struct with fields for each of the files of MIPS instructions//
///////////////add - 1, sub - 2, mul - 3, lw - 4, sw - 5, addi - 6, beq - 7//////////
//////////////////////////////////////////////////////////////////////////////////////

struct inst parser(char *line){
  struct inst newInst;
  newInst.opcode=0;
  newInst.rs=0;
  newInst.rd=0;
  newInst.rt=0;
  newInst.Imm=0;
  char* haltpls = "haltSimulation\n";
  
if(strcmp(line, haltpls) != 0){
  char *p = malloc(100*sizeof(char));                //store each section after parse
  //char *instrname = malloc(4 * sizeof(char));         //name of instruction
  int arg[4];             //integer array of argument values
  int c=0, a=1;               //counters
  //parse by whitespace and create an array of strings/numbers 
  p = strtok (line, " ");      //get first argument (should be name of instruction)
while (p != NULL){            //loop through until done
  //printf("p is: %s \n", p);  
    if(strcmp(p, "add") == 0){                       //if first arg, put it into instrname
      arg[0] = 1;                       //add is 1
    }
    else if(strcmp(p, "sub")==0){        //sub is 2
	arg[0]=2;
    }
    else if(strcmp(p, "mul")==0){         //mul is 3
	arg[0]=3;
    }
    else if(strcmp(p, "lw")==0){            //lw is 4
      arg[0]=4;
    }
   else if(strcmp(p, "sw")==0){                //sw is 5
	arg[0]=5;
    }
   else if(strcmp(p, "addi")==0){           //addi is 6
	arg[0]=6;
    }
   else if(strcmp(p, "beq")==0){          //beq is 7
	arg[0]=7;
    }
    else{                          //convert p to integer and put it in arg array
      int ll = strlen(p);             //get length of argument
    //  printf("size of p is: %d \n", strlen(p));
      int isD = 0, i;   
      for(i=0; i < ll; i++){
//	printf("%c \n", p[i]);
	if(p[i] == '\n') {
	  isD=3;
//	  printf("derp");
	}
	else if(isdigit(p[i])){ 
	  isD = 1;
	}                   //check if each character is a digit
	else{
	 isD = 0;             //there is a character that is not a digit, so break
	 break;
	}

       }
	
  //  printf("isD: %d \n", isD);  
    if(isD == 3){                     //if there is a \n
	if(ll-1 <= 5){	
        char withoutn[ll-1];	
	int z;
//	for(z=0; z < ll-1; z++) printf("withoutn is: %d ", withoutn[z]);
//	printf("\n\n");	
	int j;
	  for(j=0;j < (ll-1);j++){
//	  printf("isDigit: %d", isdigit(p[j]));
	   if(isdigit(p[j]) != 0)  
	    withoutn[j] = p[j];
      	   else{
	     //if((((arg[0]==4)  || (arg[0]==5)) && (c==2)) || (((arg[0]==6)  || (arg[0]==7)) && (c==3))){
	     //  Error_ImmediateField();
	     //}
	     //else{
	     //printf("derp");
	     Error_InvalidRegister();                                 //checks if any of the last characters are not digits
	     //}
	   }

	  }
//	 printf("atoi: %d \n", atoi(withoutn)); 
	  arg[a] = atoi(withoutn);
        memset(withoutn, 0, (ll-1));
	}
	else{
	  printf("no\n");
	Error_ImmediateField();
        }
    
    }
    else if(isD == 1){        //if number, convert into int to put into int array
          if(ll == 1){
	    arg[a] = p[0] - '0';
	  }
	  else if(ll <= 5){           //arugment is less than 5 characters (65535 is 5 characters)
          arg[a] = atoi(p);
	  }
	  else{
	    printf("there\n");
		Error_ImmediateField();                //throw immediate field error 
	  }
	  a++;                           //increment counter for index
      }
      else                                   //not a number
      {
	if((((arg[0]==4)  || (arg[0]==5)) && (c==2)) || (((arg[0]==6)  || (arg[0]==7)) && (c==3))){
	     Error_ImmediateField();
	   }
	   else{
	     Error_InvalidRegister();                                 //checks if any of the last characters are not digits
	   }

	//Error_InvalidRegister();      //return not valid number error(or missing the $, ex. s0 instead of $s0 so it didn't get converted)
      }
    }
    p = strtok (NULL, " ");          //get next section
    //printf("%s", p);
    c++;         
  }
  //  printf("%d \n", arg[0]);
   //////////////actually check which instructions it is and put the arguments in the right variables in the inst
   //if(index 0 is ADD, SUB, MUL) --> contains opcode, rs, rt, rd, funct (r)
//printf("%d",arg[0]);
  if((arg[0]==1) || (arg[0]==2) || (arg[0])==3){      //if opcode is "add" or "sub" or "mul", respectively
  int i;
    for(i=1;i < 4; i++)
    {
	//printf("%d \n", arg[i]);
	  if((arg[i] <= 31) && (arg[i] >= 0)){
	    newInst.opcode = arg[0];
	    newInst.rs = arg[2];
	    newInst.rt = arg[3];
	    newInst.rd = arg[1];
	  }
	  else{
		Error_InvalidRegister();            //throw invalid register error
	  }
    }

  }
  else if((arg[0]==4) || (arg[0]==5)){              // if opcode is "lw" or "sw", respectively
  newInst.opcode=arg[0];
    if((arg[1] <= 31) && (arg[1] >= 0)){
	newInst.rt = arg[1];
      }
    else{
	Error_InvalidRegister();           //return invalid registor error
    }
	

    if((arg[3] <= 31) && (arg[3] >= 0)){
	newInst.rs = arg[3];
    }
    else{
	Error_InvalidRegister();             //return invalid registor error
    }


    if((arg[2] <= 65535) && (arg[2] >= 0)){
	if((arg[2] % 4) == 0){
	  newInst.Imm = arg[2];
        }
        else{
	  printf("%d", arg[2]);
	  Error_MemoryMisalignment();           //return error memory misalignment error
        }
    }
    else{
	Error_ImmediateField();                   //return error, either too small or bigger than largest unsigned 16-bit number
    }
  }
  else if ((arg[0]==6) || (arg[0]==7)){                           // if opcode is "addi" or "beq" respectively
    newInst.opcode=arg[0];

    if((arg[1] <= 31) && (arg[1] >= 0)){
	newInst.rt = arg[1];
      }
    else{
	Error_InvalidRegister();                                         //return invalid registor error
    }
	

    if((arg[2] <= 31) && (arg[2] >= 0)){
	newInst.rs = arg[2];
    }
    else{
	Error_InvalidRegister();                                   //return invalid registor error
    }

    //printf("%d", arg[0]);
    if(arg[0]==6){
	if(arg[3] <= 65535){                       //if opcode is addi
	newInst.Imm = arg[3];
        }
        else{
	printf("here\n");
	Error_ImmediateField();                                //return immediate field too big
        }
    }

    if(arg[0]==7){                                     //if it's beq also check memory misalignment
     if(arg[3] <= 65535){
      if((arg[3]%4)==0){	
      		newInst.Imm = arg[3];
	}
        else{	
          Error_MemoryMisalignment();
        }
      }
      else{
	Error_ImmediateField();                //return too large immediate field
      }
    }
 
  }
  else{
    Error_IllegalOpcode();              //return illegal opcode error because it's not any of the valid ones
  }
  //*/
	/*
printf("Opcode: %d\n", newInst.opcode);
printf("rs: %d\n", newInst.rs);
printf("rd: %d\n", newInst.rd);
printf("rt: %d\n", newInst.rt);
printf("Imm: %d\n", newInst.Imm);
*/
return newInst;
}
else{
newInst.opcode=8;
return newInst;
}


}


              
/////////////////////////////////////////////////////////////////////
///////////////////////IF////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//Fetches instruction from instruction memory
//If there is a branch that is unresolved finish resolving the branch before continuing
void IF(){
  printf("IF\n");
  static int CycleCount = 0;

  if(CycleCount == 0) CycleCount = c;
	  if(CycleCount == 1){
		  if((IFIDLatch.opcode == 0)&&(branchUnresolved == 0)){
			  IFIDLatch = instMem[pgm_c/4];          
		   	if(IFIDLatch.opcode == 7){    
				branchUnresolved = 1;
			}
			pgm_c +=4;
			IFcount++;
		}
	}
	CycleCount--;
printf("\nIF!");
printf("\nCycle count: %d\nPC: %d\nBranch unresolved: %d", CycleCount, pgm_c, branchUnresolved);
printf("\ninstMem: \n  opcode: %d\n  rs: %d\n  rt: %d\n  rd: %d\n  imm: %d", instMem[pgm_c/4].opcode, instMem[pgm_c/4].rs, instMem[pgm_c/4].rt, instMem[pgm_c/4].rd, instMem[pgm_c/4].Imm);
printf("\nIFIDLatch \nopcode: %d\nrs: %d\nrt: %d\nrd: %d\nimm: %d\n", IFIDLatch.opcode, IFIDLatch.rs, IFIDLatch.rt, IFIDLatch.rd, IFIDLatch.Imm);

}


////////////////////////////////////////////////////////////////////
//////////////////////////ID////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//For each type of instruction check the appropriate raw hazard
//If there is a raw hazard do nothing 
//If there is no raw hazard continue and set the appropriate raw hazard 
//flag for the register that is being calculated 
//Put in the values that are in the registers and set them equal to the 
//output instructions rt and rs (depending on instruction opcode)
//Check that the IDEXLatch is empty and if it is put the instruction 
//into the latch, and set the IFIDLatch to empty

void ID(){
  printf("ID\n");
	struct inst in = IFIDLatch;
	struct inst out = in;
printf("in opcode: %d\n", in.opcode);
	if((in.opcode == 1) || (in.opcode == 2) || (in.opcode==3)){  //add, sub, or mul  
    
		if(!(rawHaz[in.rs] || rawHaz[in.rt])){               
			rawHaz[in.rd] = 1; 		
			out.rt = reg[in.rt];
			out.rs = reg[in.rs];
			if(IDEXLatch.opcode == 0){                                  
				IFIDLatch.opcode = 0;                                     
				IDEXLatch = out;
				IDcount++;
			}
		}
  }
  else if((in.opcode == 4) || (in.opcode == 6)){         //LW or addi
    if(!rawHaz[in.rs]){
			rawHaz[in.rt] = 1;
			out.rs = reg[in.rs];
		  if(IDEXLatch.opcode == 0){
				IFIDLatch.opcode = 0;
			IDEXLatch = out;
			IDcount++;
			} 
		} 
  }
  else if(in.opcode == 5){          //SW
     if(!(rawHaz[in.rs] || rawHaz[in.rt])){               
			rawHaz[in.rt] = 1;
			out.rs = reg[in.rs];
			if(IDEXLatch.opcode == 0){                                  
				IFIDLatch.opcode = 0;                                     
				IDEXLatch = out;
				IDcount++;
			}
		}

  }
  else if(in.opcode == 7){ //beq	
     if(!(rawHaz[in.rs] || rawHaz[in.rt])){               
			branchUnresolved = 1;                                      
			out.rs = reg[in.rs];
			out.rt = reg[in.rt];
			if(IDEXLatch.opcode == 0){                                  
				IFIDLatch.opcode = 0;                                     
				IDEXLatch = out;
				IDcount++;
			}
		}
  }
  else{
	 //halt simulation
	 if(in.opcode == 8) halt = 1;
      //Return error // assertion
	 else if(in.opcode != 0){
	 printf("ID: OPCODE ERROR\n");
	 assert(in.opcode<8 || in.opcode>0);
	 exit(0);
	 }
  }
	printf("\nID!: \n");
	printf("\nIDEXLatch \nopcode: %d\nrs: %d\nrt: %d\nrd: %d\nimm: %d\n", IDEXLatch.opcode, IDEXLatch.rs, IDEXLatch.rt, IDEXLatch.rd, IDEXLatch.Imm);


}


////////////////////////////////////////////////////////////////////
//////////////////////////EX////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//When the previous instruction has finished the EX stage (when EXCycleCount = 0)
//Do the correct operations based on the instruction and set how many cycles it should take
//Then every time EX runs it will decrement the EXCycleCount by 1 
//When the counter = 1 you will have finished the operation and set the output to the correct latch
//based on if it needs the MEM stage or not
void EX(){

  struct inst in;
  printf("EX\n");
	static int CycleCount = 0;
	if(CycleCount == 0){
		in = IDEXLatch;
		EXout = in;
		if(in.opcode == 1){       //add
			EXout.rs = in.rs + in.rt;
			CycleCount = n;
			EXcount++;
		}
		else if(in.opcode == 2){ //sub
			EXout.rs = in.rs - in.rt;
			CycleCount = n;
			EXcount++;
		}
		else if(in.opcode == 3){  //mul
			EXout.rs = in.rt*in.rs;
			CycleCount = m;
			EXcount++;
		} 
		else if(in.opcode == 4){  //lw
			EXout.rs = in.rs + in.Imm;         
			CycleCount = n;
			EXcount++;
		}
		else if(in.opcode == 5){  //sw
			EXout.rs = in.rs + in.Imm;
			CycleCount = n;
			EXcount++;
		}
		else if(in.opcode == 6){  //addi
			EXout.rs = in.rs + in.Imm;
			CycleCount = n;
			EXcount++;
		}
		else if(in.opcode == 7){  //bq
		  	//printf("pgm c: %d \n", pgm_c);
			if(in.rs == in.rt) pgm_c += 4*in.Imm;

			CycleCount = n;
			branchUnresolved = 0;
			EXcount++;
		}
		else{
      //Return error // assertion
		  if(in.opcode == 8){
				halt = 1;
			}
			else if(in.opcode != 0){
			printf("EX: OPCODE ERROR\n");
		  assert(in.opcode<8 && in.opcode>0);
		  exit(0);
			}	
		}
	}
	if(CycleCount == 1){
		if((EXout.opcode == 5) || (EXout.opcode == 4)){

			if(EXMEMLatch.opcode == 0){
				EXMEMLatch = EXout;
			  EXcount++;
				IDEXLatch.opcode = 0;
				CycleCount--;

				printf("\n EX!");
printf("\nEXMEMLatch \nopcode: %d\nrs: %d\nrt: %d\nrd: %d\nimm: %d\n", EXMEMLatch.opcode, EXMEMLatch.rs, EXMEMLatch.rt, EXMEMLatch.rd, EXMEMLatch.Imm);

			} 
		} 
	  else{ 
			if(MEMWBLatch.opcode == 0){
			  MEMWBLatch = EXout;
				EXcount++;
				IDEXLatch.opcode = 0;
				CycleCount--;

			} 
              } 
				printf("\n EX!");
	printf("\nEXMEMLatch \nopcode: %d\nrs: %d\nrt: %d\nrd: %d\nimm: %d\n", EXMEMLatch.opcode, EXMEMLatch.rs, EXMEMLatch.rt, EXMEMLatch.rd, EXMEMLatch.Imm);

		} 
  
	else{
		if(CycleCount > 0)		CycleCount--;
	}
////////////////////////
//printf("pgmc at end is: %d \n", pgm_c);

}

////////////////////////////////////////////////////////////////////
/////////////////////////MEM////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void MEM(){
  printf("MEM\n");
	static int CycleCount = 0;
	if(CycleCount == 0){
		struct inst in = EXMEMLatch;
		MEMout = in;
		if(in.opcode == 4){  // lw
			printf("lw - in.rs: %d\n", in.rs);
			MEMout.rs = DMem[in.rs];
			CycleCount = c;
		}
		else if(in.opcode == 5){  //sw
			printf("sw - in.rs: %d\n",in.rs);
			DMem[reg[in.rt]] = in.rs;
			printf("DMem: %d \n",DMem[reg[in.rt]]);
			CycleCount = c;
		}
		else{
		  //halt simulation
		  if(in.opcode == 8) halt = 1;
		  //invalid opcode
		  else if(in.opcode != 0){
			 printf("MEM: INVALID OPCODE\n");
			 assert(in.opcode == 4 || in.opcode == 5);
			 exit(0);
		  }	
		}
	}
	if(CycleCount == 1){
		MEMWBLatch = MEMout;
		printf("FINISHED MEM - MEMWBLatch: %d", MEMWBLatch.rs);
		EXMEMLatch.opcode = 0;
		CycleCount--;
	}
	else{
		CycleCount--;
	}
}
                    



/////////////////////////////////////////////////////////////////////////////
//////////////////////////////WB/////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void WB(){
  printf("WB\n");
	struct inst in = MEMWBLatch;
  if((in.opcode == 1) || (in.opcode == 2) || (in.opcode==3) ){  //add, sub, mul  
    reg[in.rd] = in.rs;
		rawHaz[in.rd] = 0;
		WBcount++;
		MEMWBLatch.opcode = 0;
		printf("reg %d: %d\n", in.rd, reg[in.rd]);
  }
	else if((in.opcode == 5) || (in.opcode == 6)){   //addi,sw
		reg[in.rt] = in.rs;
		rawHaz[in.rt] = 0;
		WBcount++;
		MEMWBLatch.opcode = 0;
		printf("reg %d: %d\n", in.rt, reg[in.rt]);
	}
  else{
    //halt simulation
    if(in.opcode == 8) halt = 1;
	 //Return error // assertion
	 else if(in.opcode != 0){
	   printf("WB: OPCODE ERROR\n");
	   assert(in.opcode<7 || in.opcode>0);
	   exit(0);
	 }
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////MAIN/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
void main (int argc, char *argv[]){

//////////////////////Initialize variables////////////////////////////  
//	IFIDLatch = {0,0,0,0,0};
	IFIDLatch.opcode = 0;
	IFIDLatch.rs = 0;
	IFIDLatch.rt = 0;
	IFIDLatch.rd = 0;
	IFIDLatch.Imm = 0;

	//IDEXLatch = {0,0,0,0};
	IDEXLatch.opcode = 0;
	IDEXLatch.rs = 0;
	IDEXLatch.rd = 0;
	IDEXLatch.rt = 0;
	IDEXLatch.Imm = 0;
	
	//EXMEMLatch = {0,0,0,0};
	EXMEMLatch.opcode = 0;
	EXMEMLatch.rs = 0;
	EXMEMLatch.rd = 0;
	EXMEMLatch.rt = 0;
	EXMEMLatch.Imm = 0;
	
	//MEMWBLatch = {0,0,0,0};
	MEMWBLatch.opcode = 0;
	MEMWBLatch.rs = 0;
	MEMWBLatch.rt = 0;
	MEMWBLatch.rd = 0;
	MEMWBLatch.Imm = 0;
	
	IFcount = 0;
	IDcount = 0;
	EXcount = 0;
	MEMcount = 0;
	WBcount = 0;
	branchUnresolved = 0;
  int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
  int i;//for loop counter
  long pgm_c=0;//program counter
  long sim_cycle=0;//simulation cycle counter
  halt = 0;
  rawHaz[32] = 0;
  reg[32] = 0; 
  DMem = (int *)malloc(500 * sizeof(int));
  int dataAddress=0;

	////////////////////////Set cmdline arguements to variables/////////////////////////
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
			reg[i]=0;
			rawHaz[i] = 0;
    }
  }

	//////////////////////Read from input file to instruction memory/////////////////////
  char *traceEntry;
  //FILE *ifp;
  //ifp = fopen("./program.txt", "r");
 
  traceEntry = malloc(200*sizeof(char)); 
  char *hs = "haltSimulation\n";
  int instIndex = 0;

  fgets(traceEntry, 100, input);                 //get first line
  while(strcmp(traceEntry, hs) != 0){                  //if it doesn't reach haltSimulation
  printf("String input is %s \n", traceEntry);    
//  printf("%d", strcmp(traceEntry, hs));
  		//progscannner
		//regnumberconverter  
		instMem[instIndex++] = parser(traceEntry);
    fgets(traceEntry, 100, input);
  }
///////////////////////Run mips instructions through pipleine///////////////////////////
	/*struct inst finalInst;
	finalInst.opcode = 8;
	instMem[instIndex++] = finalInst;
  fclose(input);
 
  while(!halt){
		WB();
		MEM();
		EX();
		ID();
		IF();
		sim_cycle++;
		if(sim_mode){
		  for(int i=1; i<32; i++){
			 printf("Register %d: %d, ", i, reg[i]);
		  }
		  printf("Cycle: %d\nPC: %d\nPress enter to advance\n", sim_cycle, pgm_c);
		  while(getchar()!='\n'){
			 getchar();
		  }
		}
	} 
*/

/*c=3;
struct inst inst1;
inst1.opcode = 1;
inst1.rs = 7;
inst1.rt = 8;
inst1.rd = 9;
inst1.Imm = 0;
struct inst inst2;
inst2.opcode = 5;
inst2.rs = 10;
inst2.rt = 11;
inst2.rd = 0;
inst2.Imm = 8;
instMem[0] = inst1;
instMem[1] = inst2;
reg[inst1.rs] = 1;
reg[inst1.rt] = 2;
reg[inst1.rd] = 3;
reg[inst2.rs] = 4;
reg[inst2.rt] = 12;
int o = 7;
MEMWBLatch = inst2;
while(o>0){
WB();
//ID();
//IF();
o--;
}
*/



/*
  //output statistics in batch mode

  ///////////////////////output statistics in batch mode/////////////////////////:

  if(sim_mode==0){
    fprintf(output,"program name: %s\n",argv[5]);
    fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n", IFcount, IDcount, EXcount, MEMcount, WBcount);
    // add the (double) stage_counter/sim_cycle for each 
    // stage following sequence IF ID EX MEM WB		
    fprintf(output,"register values ");
    for (i=1;i<REG_NUM;i++){
      fprintf(output,"%d  ",reg[i]);
    }
    fprintf(output,"%d\n",pgm_c);
  }
   
  //close input and output files at the end of the simulation
//	fclose(input);
//	fclose(output);
	return 0;
*/
}
  

