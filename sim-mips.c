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

////////////Enumeration type describing opcodes////////////
enum instr{ADD=1, SUB=2, MULT=3, LW=4, SW=5, ADDI=6, BEQ=7};                                 //we have to add this but i'm not sure what it's for since we already using structs

///Structure that holds data about the type of instruction 
struct inst
{
  enum instr opcode;
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
  int rawHaz[REG_NUM]; //array of flags for each reg
  struct inst instMem[512];  
  int pc;
  struct inst EXout;
  struct inst MEMout;
  int *DMem;
  long mips_reg[REG_NUM]; //what is in each register
  int branchUnresolved;
  int IFcount;
  int IDcount;
  int EXcount;
  int MEMcount;
  int WBcount; 
  int halt;
  int stopReceive;
  int mode; 
  int c,m,n; 
  long pgm_c;
  int IFrun=0, IDrun=0, EXrun=0, MEMrun=0, WBrun=0;
   
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////FUNCTIONS/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void Error_ParanthesesMismatch(){
printf("Error: Parantheses Mismatch!");
exit(1);
}

////////////////////////////////////////////////////////////////////////////
/////////////////////////PROGSCANNER////////////////////////////////////////
//Input a string and get rid of any extra punction, spaces or parentheses///
////////////////////////////////////////////////////////////////////////////
char *progScanner(char* currentLine){

  assert(currentLine != NULL);          //ASSERTION

  char copy[strlen(currentLine)+1];   //make empty array of size currentLine
  int i;
  int pos=0;
  int cp[2]={0};
  int l=0, r=0;
  char *p;

for(p = currentLine; *p; ++p){
  if(*p == '(') {cp[0]=1; l++;}                 //if ( increase left counter
  if(*p == ')'){                             //if ) increase right counter
    r++;
    if (cp[0]!=0) {cp[1]=2;}                         //if putting ) before a ( throw error
    else Error_ParanthesesMismatch();
  }

  if((isalpha(*p) != 0) || (isdigit(*p) != 0) || (*p == '$') || (*p == ' ')){
	currentLine[pos++] =*p;

  }
  else if((*p=='(') || (*p==')')){
	 currentLine[pos++] = ' ';
  }

}
  currentLine[pos]='\0';

	if(l != r) Error_ParanthesesMismatch();                //if left and right don't match, throw error

///////////remove and leave only 1 space
	int x;
	for(i=x=0; currentLine[i]; ++i){
		if(!isspace(currentLine[i]) || (i > 0 && !isspace(currentLine[i-1]))) currentLine[x++] = currentLine[i];
	}
	currentLine[x] = '\0';
	return currentLine;
}

////////////////////////////////////////////////////////////////////////////
/////////////////////////REGNUMBERCONVERTER ERRORS/////////////////////////
///////////////////////////////////////////////////////////////////////////
void regIllegal(){
printf("Error: Illegal register name detected\n");
exit(1);
}
void regOOB(){
printf("Error: Register number is out of bounds\n");
exit(1);
}
////////////////////////////////////////////////////////////////////////////
////////////////////REGNUMBERCONVERTER//////////////////////////////////////
//Takes in input from progScanner() output and converts any registers that//
//start with $ to numbers and returns error if invalid register/////////////
///////////////////////////////////////////////////////////////////////////
char *regNumberConverter(char *line){

  assert(line != NULL);             //ASSERTION

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
	       if(!((line[pos+1] == ' ') || line[pos+1] == '\0') || (line[pos+1] == '\n')){regIllegal();}//check for no extra char
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
	     else{regIllegal();} //error if not a number
	   }
	   else if(line[pos] == 's'){				//check for s0-7 or sp
        if(isdigit(line[++pos])){		//if second char is number
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))){regIllegal();} //check for no extra char
	       if((line[pos]-'0')<8){
	         regNum = 16+(line[pos++]-'0'); //register number in int
	         sprintf(regChar,"%d", regNum);	
	         newLine[newPos++] = regChar[0];
	         newLine[newPos++] = regChar[1];
	       }
	       else{regIllegal();}
	     }
	     else if(line[pos++] == 'p'){   //if sp
	       if(!((line[pos] == ' ') || (line[pos] == '\0') || (line[pos++] == '\n'))){regIllegal();} //check for no extra char
	       newLine[newPos++] = '2';
	       newLine[newPos++] = '9';
		  }
	     else{regIllegal();} //error if not a number
	   }
	   else if(line[pos] == 'a'){	//if a0-a3
        if(isdigit(line[++pos])){		
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))){regIllegal();} //check for no extra char 
	       if((line[pos]-'0')<4){
	         regNum = 4+(line[pos++]-'0'); //register number in int
	         sprintf(regChar,"%d", regNum);
		      newLine[newPos++] = regChar[0];
	       }
	       else{regIllegal();}
	     }
	     else if(line[pos] == 't'){
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))) {regIllegal();}
	       pos++;
	       newLine[newPos++] = '1';
	     }
	     else{regIllegal();} //error if not a number
  	   }
	   else if(line[pos] == 'v'){ //if v0-v1
        if(isdigit(line[++pos])){		
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))){regIllegal();} //check for no extra char
	       if((line[pos]-'0')<2){
	         regNum = 2+(line[pos++]-'0'); //register number in int
      	   sprintf(regChar,"%d", regNum);
	         newLine[newPos++] = regChar[0];
	       }
	       else{regIllegal();}
	     }
	     else{regIllegal();} //error if not a number
	   }
	   else if(line[pos] == 'k'){	//if k0-k1
        if(isdigit(line[++pos])){		
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))){regIllegal();} //check for no extra char
	       if((line[pos]-'0')<2){
	         regNum = 26+(line[pos++]-'0'); //register number in int
	         sprintf(regChar,"%d", regNum);
	         newLine[newPos++] = regChar[0];
	         newLine[newPos++] = regChar[1];
	       }
	       else{regIllegal();}
	     }
	     else{regIllegal();} //error if not a number
	   }
	   //if doesnt begin with t, s, a, v, or k
	   else if(line[pos] == 'g'){ //check for gp
	     if(line[++pos] == 'p'){
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos++ +1] == '\n'))){regIllegal();} //check for no extra char
      	 pos++;
	       newLine[newPos++] = '2';
	       newLine[newPos++] = '8';
	     }
	     else{regIllegal();}
	   }
      else if(line[pos] == 'f'){ //check for fp
	     if(line[++pos] == 'p'){
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos++ +1] == '\n'))){regIllegal();} //check for no extra char
	       pos++;
	       newLine[newPos++] = '3';
	       newLine[newPos++] = '0';
	     }
	     else{regIllegal();}
	   }
	   else if(line[pos] == 'r'){	//check for ra
	     if(line[++pos] == 'a'){
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))){regIllegal();} //check for no extra char
	       pos++;
	       newLine[newPos++] = '3';
	       newLine[newPos++] = '1';
	     }
	     else{regIllegal();}
	   }
	   else if(line[pos] == 'z'){	//check for zero
	     if(line[++pos] == 'e'){
	       if(line[++pos] == 'r'){
	         if(line[++pos] == 'o'){
	           if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos++ +1] == '\n'))){regIllegal();} //check for no extra char
	           pos++; 
	           newLine[newPos++] = '0';
	         }
	         else{regIllegal();}
	       }
	       else{regIllegal();}
	     }
	     else{regIllegal();}
	   }
	   else if(isdigit(line[pos])){	//check if 0-31
	     if(isdigit(line[++pos])){
	       if(!((line[pos+1] == ' ') || (line[pos+1] == '\0') || (line[pos+1] == '\n'))){regIllegal();}
	       regNum = 10*(line[pos-1]-'0');
	       regNum += (line[pos]-'0');
	       if(regNum > 31) regOOB();
	       newLine[newPos++] = line[pos-1];
	       newLine[newPos++] = line[pos++];
	     }      
	     else if((line[pos] == ' ') || (line[pos] == '\0') || (line[pos] == '\n')) {
          regNum = (line[pos-1]-'0');
	       newLine[newPos++] = line[pos-1];
	     }
        else{regIllegal();}
	   }
	   else{regIllegal();}			//return error if no matches  
    } 
	 if(line[pos]!='\n') newLine[newPos++] = line[pos];
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
  
  assert(line != NULL);       //ASSERTION

  struct inst newInst;
  newInst.opcode=0;
  newInst.rs=0;
  newInst.rd=0;
  newInst.rt=0;
  newInst.Imm=0;
  
  char *p = malloc(100*sizeof(char));                //store each section after parse
  int arg[4];             //integer array of argument values 
  int c=0, a=1;               //counters
  //parse by whitespace and create an array of strings/numbers 
  p = strtok (line, " ");      //get first argument (should be name of instruction)
  while (p != NULL){            //loop through until done  
    if(strcmp(p, "add") == 0){                       //if first arg, put it into instrname
      arg[0] = ADD;                       //add is 1
    }
    else if(strcmp(p, "sub")==0){        //sub is 2
	   arg[0]=SUB;
    }
    else if(strcmp(p, "mul")==0){         //mul is 3
	   arg[0]=MULT;
    }
    else if(strcmp(p, "lw")==0){            //lw is 4
      	   arg[0]=LW;
    }
    else if(strcmp(p, "sw")==0){                //sw is 5
	   arg[0]=SW;
    }
    else if(strcmp(p, "addi")==0){           //addi is 6
	   arg[0]=ADDI;
    }
    else if(strcmp(p, "beq")==0){          //beq is 7
	   arg[0]=BEQ;
    }
    else{                          //convert p to integer and put it in arg array
      int ll = strlen(p);             //get length of argument
      int isD = 0, i;   
      for(i=0; i < ll; i++){	
	     if((p[i] == '\n') || (p[i] == '\0')) { 
	       isD=3;
	     }
	     else if(isdigit(p[i])){          //check if it's a digit 
	       isD = 1;
	     }
	     else if((p[i] - '0') == -43){}   //weird end of line thing
	     else{
	       isD = 0;             //there is a character that is not a digit, so break
	       break;
	     }
      }

      if(isD == 3){                     //if there is a \n
	     if(ll-1 <= 5){	
	       char withoutn[ll-1];	
	       int z;
	       int j;
	       for(j=0;j < (ll-1);j++){
	         if(isdigit(p[j]) != 0)  
	           withoutn[j] = p[j];
      	       else{
	           Error_InvalidRegister();                                 //checks if any of the last characters are not digits
	           }
	       }
	       printf("%d /n", atoi(withoutn));
	       arg[a] = atoi(withoutn);
               memset(withoutn, 0, (ll-1));
	     }
	     else{
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
		    Error_ImmediateField();                //throw immediate field error 
	     }
	     a++;                           //increment counter for index
      }
      else{                                   //not a number
	     if((((arg[0]==4)  || (arg[0]==5)) && (c==2)) || (((arg[0]==6)  || (arg[0]==7)) && (c==3))){
	       printf("this is %d\n", isD); 
	       Error_ImmediateField();
	     }
	     else{
	       Error_InvalidRegister();                                 //checks if any of the last characters are not digits
	     }
      }
    }
    p = strtok (NULL, " ");          //get next section
    c++;         
  }
   //////////////actually check which instructions it is and put the arguments in the right variables in the inst

	assert(arg[0] != 0);                          //ASSERTION

  if((arg[0]==1) || (arg[0]==2) || (arg[0])==3){      //if opcode is "add" or "sub" or "mul", respectively
    int i;
    for(i=1;i < 4; i++)
    {
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
    if(c == 3){
      if((arg[1] <= 31) && (arg[1] >= 0)){
	     newInst.rt = arg[1];
      }
      else{ Error_InvalidRegister(); }
      
		if((arg[2] <= 31) && (arg[2] >= 0)){
 	     newInst.rs = arg[2];
      }
      else{ Error_InvalidRegister();}
    }
    else {
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
	       Error_MemoryMisalignment();           //return error memory misalignment error
        }
      }
      else{
	     Error_ImmediateField();                   //return error, either too small or bigger than largest unsigned 16-bit number
      }
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
    if(arg[0]==6){
	   if(arg[3] <= 65535){                       //if opcode is addi
	     newInst.Imm = arg[3];
      }
      else{
	     Error_ImmediateField();                                //return immediate field too big
      }
    }

    if(arg[0]==7){                                     //if it's beq also check memory misalignment
      if(arg[3] <= 65535){	
        newInst.Imm = arg[3];
      }
      else{
	     Error_ImmediateField();                //return too large immediate field
      }
    }
  }
  else{
    Error_IllegalOpcode();              //return illegal opcode error because it's not any of the valid ones
  }

  return newInst;
}
/////////////////////////////////////////////////////////////////////
///////////////////////IF////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//Fetches instruction from instruction memory and takes (c) cycle time
//If there is a branch that is unresolved finish resolving the branch before continuing
void IF(){
  static int CycleCount = 0;

  if(CycleCount == 0){
	 CycleCount = c;
	 IFrun=1;
	 if(instMem[pgm_c/4].opcode == 8){
		stopReceive=1;
		IFrun = 0;
		if(IFIDLatch.opcode == 0) IFIDLatch = instMem[pgm_c/4];
	 }
  }
  if(CycleCount == 1){
	 if((IFIDLatch.opcode == 0)&&(branchUnresolved == 0)){
		IFIDLatch = instMem[pgm_c/4];
		assert(IFIDLatch.opcode != 0);                  //ASSERTION
	   if(instMem[pgm_c/4].opcode == 7){    
		  branchUnresolved = 1;
	   }
		if(!stopReceive) pgm_c +=4;
		IFcount++;
		CycleCount--;
	 }
  }
  else if(CycleCount>0){
	 CycleCount--;
	 IFcount++;
  }
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
  struct inst in = IFIDLatch;
  struct inst out = in;
  if((in.opcode == 1) || (in.opcode == 2) || (in.opcode==3)){  //add, sub, or mul    
	 IDrun = 1;
	 if(!(rawHaz[in.rs] || rawHaz[in.rt])){               
		if(IDEXLatch.opcode == 0){                                  
		  rawHaz[in.rd] = 1; 		
		  out.rt = mips_reg[in.rt];
		  out.rs = mips_reg[in.rs];
		  IFIDLatch.opcode = 0;                                     
		  IDEXLatch = out;
		  IDcount++;
		}
	 }
  }
  else if((in.opcode == 4) || (in.opcode == 6)){         //LW or addi
    IDrun = 1;
	 if(!rawHaz[in.rs]){
		if(IDEXLatch.opcode == 0){
		  rawHaz[in.rt] = 1;
		  out.rs = mips_reg[in.rs];
		  IFIDLatch.opcode = 0;
		  IDEXLatch = out;
		  IDcount++;
		} 
	 }
  }
  else if(in.opcode == 5){          //SW	
    IDrun = 1;
	 if(!(rawHaz[in.rs] || rawHaz[in.rt])){               
		if(IDEXLatch.opcode == 0){                                  
		  rawHaz[in.rt] = 1;
		  out.rs = mips_reg[in.rs];
		  IFIDLatch.opcode = 0;                                     
		  IDEXLatch = out;
		  IDcount++;
		}
	 } 
  }
  else if(in.opcode == 7){ //beq	
    IDrun = 1;
	 if(!(rawHaz[in.rs] || rawHaz[in.rt])){               
		if(IDEXLatch.opcode == 0){                                  
		  branchUnresolved = 1;                                      
		  out.rs = mips_reg[in.rs];
		  out.rt = mips_reg[in.rt];
		  IFIDLatch.opcode = 0;                                     
		  IDEXLatch = out;
		  IDcount++;
		}
	 }
  }
  else{
	 //halt simulation
	 if(in.opcode == 8){
		stopReceive = 1;
		IDrun = 0;
		if(IDEXLatch.opcode == 0){
		  IDEXLatch.opcode = 8;
		}
	 }
      //Return error // assertion
	 else if(in.opcode != 0){
	   printf("ID: OPCODE ERROR\n");
	   assert(in.opcode<8 && in.opcode>0);              //ASSERTION
	   exit(0);
	 }
  }
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
	static int CycleCount = 0;
	if(CycleCount == 0){
		struct inst in = IDEXLatch;
		EXout = in;
		if(in.opcode == 1){       //add
			EXrun = 1;
		   EXout.rs = in.rs + in.rt;
			if(in.rd == 0) EXout.rs = 0;
			CycleCount = n;
		}
		else if(in.opcode == 2){ //sub
			EXrun = 1;
		   EXout.rs = in.rs - in.rt;
			if(in.rd == 0) EXout.rs = 0;
			CycleCount = n;
		}
		else if(in.opcode == 3){  //mul
			EXrun = 1;
		   EXout.rs = in.rt*in.rs;
			if(in.rd == 0) EXout.rs = 0;
			CycleCount = m;
		}
		else if(in.opcode == 4){  //lw
			EXrun = 1;
		   EXout.rs = in.rs + (in.Imm/4);
			if(in.rt == 0) EXout.rs = 0;
			CycleCount = n;
		}
		else if(in.opcode == 5){  //sw
			EXrun = 1;
		   EXout.rs = in.rs + (in.Imm/4);
			if(in.rt == 0) EXout.rs = 0;
			CycleCount = n;
		}
		else if(in.opcode == 6){  //addi
			EXrun = 1;
		   EXout.rs = in.rs + in.Imm;
			if(in.rt == 0) EXout.rs = 0;
			CycleCount = n;
		}
		else if(in.opcode == 7){  //beq
			EXrun = 1;
		    if(in.rs == in.rt){ 
			  
			  if(((pgm_c + 4*in.Imm) < 0) || (pgm_c + 4*in.Imm) >= 512){     //check if the resulting pgm_c is negative or bigger than allotted array
				printf("Invalid program counter value!");
				exit(1);
			  }

			  int x;
			  struct inst checkHalt;
			  for(x=pgm_c; x < (pgm_c + (4*in.Imm)); x+=4)                //check if it skips over haltSimulation
			  {
				checkHalt = instMem[x/4];
				if(checkHalt.opcode == 8){
				  printf("haltSimulation reached!");
				  exit(1);
				}
			  }

			  pgm_c += 4*in.Imm;          //if it passes all checks, increase pgm_c
			}
			CycleCount = n;
			branchUnresolved = 0;
		}
		else{
      //Return error // assertion
		  if(in.opcode == 8){
			 EXrun = 0;
			 if(EXMEMLatch.opcode == 0) EXMEMLatch.opcode = 8;
		  }
		  else if(in.opcode != 0){
			 printf("EX: OPCODE ERROR\n");
		    assert(in.opcode<8 && in.opcode>0);                  //ASSERTION
		    exit(0);
		  }
		}
	}
	if(CycleCount == 1){
	  if(EXMEMLatch.opcode == 0){
		 EXMEMLatch = EXout;
	    if(EXout.opcode != 8) EXcount++;
		 IDEXLatch.opcode = 0;
		 CycleCount--;
	  }
	}
	else{
	  if(CycleCount > 0){
	    CycleCount--;
		   if(EXout.opcode != 8)EXcount++;
	  }
   }
}

////////////////////////////////////////////////////////////////////
/////////////////////////MEM////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/*If the instruction is a lw or sw MEM will take c cycle times 
 * (the amount of cycles to access memory). 
 * If it is a lw it will
 * take the data in the DMEM at the calculated index and put the value 
 * into rs in the inst struct. 
 * If it is a sw it will load the calculated value into the DMEM at the 
 * specific index
 *If it is any other instruction it will take 1 cycle to complete
 and do nothing
 If there is an illegal opcode it will return an error
 */
void MEM(){
	static int CycleCount = 0;
	if(CycleCount == 0){
		struct inst in = EXMEMLatch;
		MEMout = in;
		if(in.opcode == 4){  // lw
		  MEMrun = 1;	
		  MEMout.rs = DMem[in.rs];
		  if(MEMWBLatch.opcode == 0){
			 MEMWBLatch = MEMout; 
			 CycleCount = c;
		  }
	        }
		else if(in.opcode == 5){  //sw
			MEMrun = 1;
			DMem[mips_reg[in.rt]] = in.rs;
			rawHaz[in.rt] = 0;
			if(MEMWBLatch.opcode == 0){
			  MEMWBLatch = MEMout;
			  CycleCount = c;
			}
		}
		else{
		  //halt simulation
		  if(in.opcode == 8){
			 MEMrun = 0;
			 if(MEMWBLatch.opcode == 0) MEMWBLatch.opcode = 8;
		  }
		  //invalid opcode
		  else if((in.opcode<0) || (in.opcode>8)){
			 printf("MEM: INVALID OPCODE\n");
			 assert(in.opcode>0 && in.opcode<8);                     //ASSERTION
			 exit(0);
		  }
		  else{
			 if(in.opcode != 0) MEMrun = 1;
			 CycleCount = 1;
		  }
	  }
  }
  if(CycleCount == 1){
	 MEMWBLatch = MEMout;
	 EXMEMLatch.opcode = 0;
	 if(MEMout.opcode != 8 && MEMout.opcode != 0)MEMcount++;
	 CycleCount--;
  }
  else{
    if(CycleCount > 0){
			CycleCount--;
			if(MEMout.opcode != 8 && MEMout.opcode != 0)MEMcount++;
		}
  }
}




/////////////////////////////////////////////////////////////////////////////
//////////////////////////////WB/////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//*
//If the instruction is an add, sub, mul, addi, or lw it will write the 
//calculated value into the specific registor. It is is sw or beq it will
//not write anything into any of the registers. If there is a haltSimulation (8)
//then the WB stage will turn off. If there are any other opcodes then
//it will stop and return an error
//*/
void WB(){
  struct inst in = MEMWBLatch;
  if((in.opcode == 1) || (in.opcode == 2) || (in.opcode==3) ){  //add, sub, mul
    WBrun = 1;
    mips_reg[in.rd] = in.rs;
	 rawHaz[in.rd] = 0;
	 WBcount++;
	 MEMWBLatch.opcode = 0;
  }
  else if((in.opcode == 4) || (in.opcode == 6)){   //addi,lw
	 WBrun = 1;
	 mips_reg[in.rt] = in.rs;
	 rawHaz[in.rt] = 0;
	 WBcount++;
	 MEMWBLatch.opcode = 0;
  }
  else if(in.opcode == 5 || in.opcode == 7){  //sw beq
	 MEMWBLatch.opcode = 0;
  	 WBrun = 1;
  }
  else{
    //halt simulation
    if(in.opcode == 8){
		halt = 1;
		WBrun = 0;
	 }
	 //Return error // assertion
	 else if(in.opcode != 0){
	   printf("WB: OPCODE ERROR\n");
	   assert(in.opcode<8 && in.opcode>0);                  //ASSERTION
	   exit(0);
	 }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////MAIN/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
void main (int argc, char *argv[]){
//////////////////////Initialize variables////////////////////////////  

	//Initialize IFIDLatch
	IFIDLatch.opcode = 0;
	IFIDLatch.rs = 0;
	IFIDLatch.rt = 0;
	IFIDLatch.rd = 0;
	IFIDLatch.Imm = 0;

	//Initialize IDEXLatch
	IDEXLatch.opcode = 0;
	IDEXLatch.rs = 0;
	IDEXLatch.rd = 0;
	IDEXLatch.rt = 0;
	IDEXLatch.Imm = 0;
	
	//Intialize EXMEMLatch
	EXMEMLatch.opcode = 0;
	EXMEMLatch.rs = 0;
	EXMEMLatch.rd = 0;
	EXMEMLatch.rt = 0;
	EXMEMLatch.Imm = 0;
	
	//Initialize MEMWBLatch
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
  pgm_c=0;//program counter
  long sim_cycle=0;//simulation cycle counter
  float IFutil;
	float IDutil;
	float EXutil;
	float MEMutil;
	float WButil;
  halt = 0; 
  stopReceive = halt = 0;//flags for haltSimulation
  DMem = (int *)calloc(500, sizeof(int));
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
    if(!(m && n && c)){
		printf("Cycle time cannot be zero\n");
		assert(m && n && c);                                      //ASSERTION
		exit(1);
	 }
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
			rawHaz[i] = 0;
    }
  }

	//////////////////////Read from input file to instruction memory/////////////////////
 char traceEntry[100];
  char *hs = "haltSimulation\n";
  int instIndex = 0;

  fgets(traceEntry, 100, input);                 //get first line
  while(strcmp(traceEntry, "haltSimulation\n") != 0){                  //if it doesn't reach haltSimulation      
		instMem[instIndex++] = parser(regNumberConverter(progScanner(traceEntry)));
    fgets(traceEntry, 100, input);
  }
///////////////////////Run mips instructions through pipleine///////////////////////////

  
  struct inst finalInst;
	finalInst.opcode = 8;
	instMem[instIndex++] = finalInst;
 
  while(!halt){
		WB();
		if(halt) break;
		MEM();
		EX();
		ID();
		IF();
		if(WBrun || MEMrun || EXrun || IDrun || IFrun)sim_cycle++;
		if(sim_mode==1){
			printf("cycle: %d register value: ",sim_cycle);
			for (i=1;i<REG_NUM;i++){
				printf("%d  ",mips_reg[i]);
			}
			printf("program counter: %d\n",pgm_c);
			printf("press ENTER to continue\n");
			while(getchar() != '\n');
	}
	} 


  //output statistics in batch mode
	if(sim_cycle != 0){
     IFutil = (float) IFcount/ sim_cycle;
	  IDutil = (float) IDcount/sim_cycle;
	  EXutil = (float) EXcount/sim_cycle;
	  MEMutil = (float) MEMcount/sim_cycle;
	  WButil = (float) WBcount/sim_cycle;
	}
	else{
	  IFutil = IDutil = EXutil = MEMutil = WButil = 0;
	}
	assert((IFutil <= 1) || (IDutil <= 1) || (EXutil <= 1) || (MEMutil <= 1) || (WButil <= 1));       //ASSERTION
  ///////////////////////output statistics in batch mode/////////////////////////:

  if(sim_mode==0){
    fprintf(output,"program name: %s\n",argv[5]);
    fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n", IFutil, IDutil, EXutil, MEMutil, WButil);
    // add the (double) stage_counter/sim_cycle for each 
    // stage following sequence IF ID EX MEM WB		
    fprintf(output,"register values ");
    for (i=1;i<REG_NUM;i++){
      fprintf(output,"%d  ",mips_reg[i]);
    } 
		fprintf(output,"%d\n",pgm_c);
		fprintf(output,"Total time in cycles: %d\n",sim_cycle);
  }
   
  //close input and output files at the end of the simulation
	fclose(input);
	fclose(output);


}
  
