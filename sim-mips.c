////////////////////////////////////////////////////////////////////////
/////////////////////Computer Systems Lab 2/////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////Dylan Banh, Marisa Liu, Solomon Wang/////////////////////
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>


all assembly language in lowercase, dont worry about upper
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

code should include:
char *filename;                 //name of file to read
FILE *fp;
fp = fopen(filename, "r");
char *currentLine;
char *line;


char progScanner(currentLine){
line = strtok(currentLine, " ,()");           //split by spaces, commas, parentheses
}
  
  take in file
 parse line by line
 should be able to take in names and numbers - eg. zero and 0
 remove duplicate commas, spaces, parenthesises, etc.
 keep reading until halt simulation

char *regNumberConverter(char *line)
{
//  takes in output of progScanner
//  returns pointer to character string, which all register names are converted to numbers
  char *newLine = malloc(sizeof(line));
  char regChar[2];
  int regNum;
  int pos=0;
  int newPos=0;

  while((line[pos] != NULL) || (line[pos] != '$')){ //copy operation until first $
    newLine[newPos++] = line[pos++];
	 }
  while(line[pos] != NULL){		//start going through rest of the line
	 if(line[pos] == '$'){ //do nothing until hit a $
      //check if z, a, v, t, k, g, s, p, r
  	   //cases
	   if(line[++pos] == 't'){ 
        if(isdigit(line[++pos])){		//check what second char is
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
		    if(atoi(line[pos-1])<8){
				regNum = 8+atoi(line[pos-1]); //register number in int
				itoa(regNum, regChar, 10);    //convert to str
				if(regNum > 9){					//if regNum double digit
				  newLine[newPos++] = regChar[0];
				  newLine[newPos++] = regChar[1];
				  newLine[newPos++] = ' ';
				}
				else{
				  newLine[newPos++] = regChar[0];	//if single digit
				  newLine[newPos++] = ' ';
				}
			 }
			 else{
				regNum = 16+atoi(line[pos-1]); //register number in int
				itoa(regNum, regChar, 10);    //convert to str
				newLine[newPos++] = regChar[0];
				newLine[newPos++] = regChar[1];
			   newLine[newPos++] = ' ';
			 }
		  }
		  else{return 1;} //error if not a number
	   }
	   else if(line[++pos] == 's'){				//check for s0-7 or sp
        if(isdigit(line[++pos])){		//if second char is number
	     	 if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
		    if(atoi(line[pos-1])<8){
	 		   regNum = 16+atoi(line[pos-1]); //register number in int
		  	   itoa(regNum, regChar, 10);    //convert to str
			   newLine[newPos++] = regChar[0];
			   newLine[newPos++] = regChar[1];
			   newLine[newPos++] = ' ';
			 }
			 else{return 1;}
		  }
		  else if(line[++pos] == 'p'){   //if sp
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
		    newLine[newPos++] = '2';
			 newLine[newPos++] = '9';
			 newLine[newPos++] = ' ';
		  }
		  else{return 1;} //error if not a number
	   }
	   else if(line[++pos] == 'a'){	//if a0-a3
        if(isdigit(line[++pos])){		
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char 
			 if(atoi(line[pos-1])<4){
	 		   regNum = 4+atoi(line[pos-1]); //register number in int
		      itoa(regNum, regChar, 10);    //convert to str
      	   newLine[newPos++] = regChar[0];
			   newLine[newPos++] = ' ';
			 }
			 else{return 1;}
		  }
		  else{return 1;} //error if not a number
  	   }
	   else if(line[++pos] == 'v'){ //if v0-v1
        if(isdigit(line[++pos])){		
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
			 if(atoi(line[pos-1])<2){
	 		   regNum = 2+atoi(line[pos-1]); //register number in int
		      itoa(regNum, regChar, 10);    //convert to str
      	   newLine[newPos++] = regChar[0];
			   newLine[newPos++] = ' ';
			 }
			 else{return 1;}
		  }
		  else{return 1;} //error if not a number
	   }
	   else if(line[++pos] == 'k'){	//if k0-k1
        if(isdigit(line[++pos])){		
	     	 if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
		  	 if(atoi(line[pos-1])<2){
	 		   regNum = 26+atoi(line[pos-1]); //register number in int
		      itoa(regNum, regChar, 10);    //convert to str
      	   newLine[newPos++] = regChar[0];
			   newLine[newPos++] = regChar[1];
				newLine[newPos++] = ' ';
			 }
			 else(return 1;}
		  }
		  else{return 1;} //error if not a number
	   }
	   //if doesnt begin with t, s, a, v, or k
	   else if(line[++pos] == 'g'){ //check for gp
	     if(line[++pos] == 'p'){
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
      	 newLine[newPos++] = '2';
			 newLine[newPos++] = '8';
			 newLine[newPos++] = ' ';
		  }
		  else{return 1;}
	   }
      else if(line[++pos] == 'f'){ //check for fp
	     if(line[++pos] == 'p'){
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
		    newLine[newPos++] = '3';
			 newLine[newPos++] = '0';
			 newLine[newPos++] = ' ';
		  }
		  else{return 1;}
	   }
	   else if(line[++pos] == 'r'){	//check for ra
		  if(line[++pos] == 'a'){
	       if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
		    newLine[newPos++] = '3';
			 newLine[newPos++] = '1';
			 newLine[newPos++] = ' ';
		  }
		  else{return 1;}
	   }
	   else if(line[++pos] == 'z'){	//check for zero
		  if(line[++pos] == 'e'){
		    if(line[++pos] == 'r'){
			   if(line[++pos] == 'o'){
	           if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
			     newLine[newPos++] = '0';
			     newLine[newPos++] = ' ';
				}
			   else{return 1;}
		    }
		    else{return 1;}
		  }
		  else{return 1;}
	   }
	   else if(isdigit(line[++pos])){	//check if 0-31
		  if((line[++pos] == ' ') || (isdigit(line[pos]))){
			 if((line[++pos] != ' ') || (line[pos] != NULL)){return 1;} //check for no extra char
          regChar[0] = line[pos-2];
			 regChar[1] = line[pos-1];
			 if(atoi(regChar)>31) return 1; //error if number is >31
			 if(isdigit(line[pos-1])){
			   newLine[newPos++] = line[pos-2];
			   newLine[newPos++] = line[pos-1];
				newLine[newPos++] = ' ';
		    }
			 else{
				newLine[newPos++] = line[pos-2];
				newLine[newPos++] = ' ';
			 }
		  }
		  else{return 1;} 		//error if next digit is not empty or a number
	   }
	   else{return 1;}			//return error if no matches
    }
	 pos++;
  }
}

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



 
