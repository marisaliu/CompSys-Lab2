////////////////////////////////////////////////////////////////////////
/////////////////////Computer Systems Lab 2/////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////Dylan Banh, Marisa Liu, Solomon Wang/////////////////////
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*all assembly language in lowercase, dont worry about upper
everything is in offsets
**offsets(in terms of words) are in relation to the next line of where they are called

read through fgets
instructions limited to 100 instructions

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
 


void main(){
char *filename;                 //name of file to read
FILE *fp;
//fp = fopen(filename, "r");
//char *currentLine;
char *line;

char currentLine[] = "(add)  ,    $s4,  ()$s3   ($t0";

//char *output = progScanner(currentLine);
printf("%s", progScanner(currentLine));
}

 
 /*take in file
 parse line by line
 should be able to take in names and numbers - eg. zero and 0
 remove duplicate commas, spaces, parenthesises, etc.
 keep reading until halt simulation*/

char *regNumberConverterZ(char *l){
int pos=0;
while(l[pos] != '\0'){

}

}
 // takes in output of progScanner
  //returns pointer to character string, which all register names are converted to numbers
/*
struct inst parser()
  uses output of regNumberConverter()
  returns instruction as inst struct with fields for each of the fields of MIPS instructions
    eg. opcode, rs, rt, rd, Imm.
	 not all fields will be used for each line
  if illegal opcode, error in simulation and stops

void IF(){


}


void ID()
void EX()
void MEM()
void WB()
 all data, structural and control hazards must be taken into account
 several operations are multicycle
 stages themselves are not pipelined
   eg. if an add takes 4 cycles, next instruction cannot enter EX until cycles have elapsed



 */
