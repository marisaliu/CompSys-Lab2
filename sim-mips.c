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
 


//void main(){
//char *filename;                 //name of file to read
//FILE *fp;
//fp = fopen(filename, "r");
//char *currentLine;
//}

/*
///Structure that holds data about the type of instruction 
struct inst{
  char* type;
  int opcode;
  int rs;
  int rt;
  int rd;
  int Imm;
}
*/ 
char *regNumberConverter(char *line){
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
//  takes in output of progScanner
//  returns pointer to character string, which all register names are converted to numbers
  char *newLine = (char *)malloc(strlen(line)*sizeof(char));
  char regChar[2];
  int regNum;
  int pos=0;
  int newPos=0;

//  while((line[pos] != '\0') || (line[pos] != '$')){ //copy operation until first $
//    newLine[newPos++] = line[pos++];
//	 }
//  while(line[pos] != '\0'){		//start going through rest of the line
  for(pos=0; pos<strlen(line); pos++){
	 printf("\nline: %s", line);
    if(line[pos] == '$'){ //do nothing until hit a $
      pos++;
		//check if z, a, v, t, k, g, s, p, r
  	   //cases
	   if(line[pos] == 't'){ 
		  if(isdigit(line[++pos])){		//check what second char is
			 printf("\n%d, %d", pos, strlen(line));
	//		 if(line[pos+1] == '\0'){return 1;}
			 if((line[pos+1] != ' ')){return 1;} //|| (line[pos+1] == '\0')){return "1";} //check for no extra char
			 if((line[pos]-'0')<8){
				regNum = 8+(line[pos++]-'0'); //register number in int
				//itoa(regNum, regChar, 10);    //convert to str
				sprintf(regChar,"%d", regNum);
				if(regNum > 9){					//if regNum double digit
				  printf("\n adding");
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
				regNum = 16+(line[pos-1]-'0'); //register number in int
				//itoa(regNum, regChar, 10);    //convert to str
				sprintf(regChar,"%d", regNum);
				newLine[newPos++] = regChar[0];
				newLine[newPos++] = regChar[1];
			   newLine[newPos++] = ' ';
			 }
		  }
		  else{return "a";} //error if not a number
	   }
	   else if(line[pos] == 's'){				//check for s0-7 or sp
        if(isdigit(line[++pos])){		//if second char is number
	     	 if((line[pos+1] != ' ') || (line[pos+1] == '\0')){return "2";} //check for no extra char
		    if((line[pos]-'0')<8){
	 		   regNum = 16+(line[pos++]-'0'); //register number in int
		  	   //itoa(regNum, regChar, 10);    //convert to str
	 	   	sprintf(regChar,"%d", regNum);	
				newLine[newPos++] = regChar[0];
			   newLine[newPos++] = regChar[1];
			   newLine[newPos++] = ' ';
			 }
			 else{return "b";}
		  }
		  else if(line[pos] == 'p'){   //if sp
	       if((line[pos] != ' ') || (line[pos++] == '\0')){return "3";} //check for no extra char
		    newLine[newPos++] = '2';
			 newLine[newPos++] = '9';
			 newLine[newPos++] = ' ';
		  }
		  else{return "c";} //error if not a number
	   }
	   else if(line[pos] == 'a'){	//if a0-a3
        if(isdigit(line[++pos])){		
	       if((line[pos+1] != ' ') || (line[pos+1] == '\0')){return "4";} //check for no extra char 
			 if((line[pos]-'0')<4){
	 		   regNum = 4+(line[pos++]-'0'); //register number in int
		      //itoa(regNum, regChar, 10);    //convert to str
      	   sprintf(regChar,"%d", regNum);
				newLine[newPos++] = regChar[0];
			   newLine[newPos++] = ' ';
			 }
			 else{return "d";}
		  }
		  else{return "e";} //error if not a number
  	   }
	   else if(line[pos] == 'v'){ //if v0-v1
        if(isdigit(line[++pos])){		
	       if((line[pos+1] != ' ') || (line[pos+1] == '\0')){return "5";} //check for no extra char
			 if((line[pos-1]-'0')<2){
	 		   regNum = 2+(line[pos++]-'0'); //register number in int
		      //itoa(regNum, regChar, 10);    //convert to str
      	 	sprintf(regChar,"%d", regNum);
				newLine[newPos++] = regChar[0];
			   newLine[newPos++] = ' ';
			 }
			 else{return "f";}
		  }
		  else{return "g";} //error if not a number
	   }
	   else if(line[pos] == 'k'){	//if k0-k1
        if(isdigit(line[++pos])){		
	     	 if((line[pos+1] != ' ') || (line[pos+1] == '\0')){return "6";} //check for no extra char
		  	 if((line[pos]-'0')<2){
	 		   regNum = 26+(line[pos++]-'0'); //register number in int
		      //itoa(regNum, regChar, 10);    //convert to str
      	   sprintf(regChar,"%d", regNum);
				newLine[newPos++] = regChar[0];
			   newLine[newPos++] = regChar[1];
				newLine[newPos++] = ' ';
			 }
			 else{return "h";}
		  }
		  else{return "i";} //error if not a number
	   }
	   //if doesnt begin with t, s, a, v, or k
	   else if(line[pos] == 'g'){ //check for gp
	     if(line[++pos] == 'p'){
	       if((line[pos+1] != ' ') || (line[pos++ +1] == '\0')){return "7";} //check for no extra char
      	 newLine[newPos++] = '2';
			 newLine[newPos++] = '8';
			 newLine[newPos++] = ' ';
		  }
		  else{return "j";}
	   }
      else if(line[pos] == 'f'){ //check for fp
	     if(line[++pos] == 'p'){
	       if((line[pos+1] != ' ') || (line[pos++ +1] == '\0')){return "8";} //check for no extra char
		    newLine[newPos++] = '3';
			 newLine[newPos++] = '0';
			 newLine[newPos++] = ' ';
		  }
		  else{return "k";}
	   }
	   else if(line[pos] == 'r'){	//check for ra
		  if(line[++pos] == 'a'){
	       if((line[pos+1] != ' ') || (line[pos++ +1] == '\0')){return "9";} //check for no extra char
		    newLine[newPos++] = '3';
			 newLine[newPos++] = '1';
			 newLine[newPos++] = ' ';
		  }
		  else{return "l";}
	   }
	   else if(line[pos] == 'z'){	//check for zero
		  if(line[++pos] == 'e'){
		    if(line[++pos] == 'r'){
			   if(line[++pos] == 'o'){
	           if((line[pos+1] != ' ') || (line[pos++ +1] == '\0')){return "10";} //check for no extra char
			     newLine[newPos++] = '0';
			     newLine[newPos++] = ' ';
				}
			   else{return "m";}
		    }
		    else{return "n";}
		  }
		  else{return "o";}
	   }
	   else if(isdigit(line[pos])){	//check if 0-31
		  if((line[++pos] == ' ') || (isdigit(line[pos]))){
			 if((line[pos+1] != ' ') || (line[pos+1] == '\0')){return "11";} //check for no extra char
          if(isdigit(line[pos])){
			   regNum = 10*(line[pos-1]-'0');
			   regNum += (line[pos]-'0');
			 }
			 else{
            regNum = (line[pos-1]-'0');
			 }
			 if(regNum>31) return "p"; //error if number is >31
			 if(isdigit(line[pos])){
			   newLine[newPos++] = line[pos-1];
			   newLine[newPos++] = line[pos];
				newLine[newPos++] = ' ';
		    }
			 else{
				newLine[newPos++] = line[pos-1];
				newLine[newPos++] = ' ';
			 }
		  }
		  else{return "q";} 		//error if next digit is not empty or a number
	   }
	   else{return "r";}			//return error if no matches
    }
	 newLine[newPos++] = line[pos];
    printf("\nnewLine: %s", newLine);
  }
  char *newNewLine = (char *)realloc(newLine, newPos*sizeof(char));
  return newNewLine;
}
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////PARSER/////////////////////////////////////////////////////
//////Parameter --> output of regNumberConvereter()///////////////////////////////////
//////Return --> inst struct with fields for each of the files of MIPS instructions///
//////////////////////////////////////////////////////////////////////////////////////
//struct inst parser(char *line){
//  inst *newInst;
/*  parse by whitespace and create an array of strings/numbers
   if(index 0 is ADD, SUB, MULT) --> contains opcode, rs, rt, rd, funct

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
  void main(void){
  char *strin = "add $t0 $t1 $t2";
  char *strout;
  strout = regNumberConverter(strin);
  printf("\n%s", strout);
  }
