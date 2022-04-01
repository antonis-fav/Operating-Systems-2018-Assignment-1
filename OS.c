#include <stdio.h>   
#include <unistd.h>  
#include <stdlib.h>  
#include <dirent.h>
#include <errno.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

//In case the user uses the command "cd".This command must be exececuted in a different way.
void changedirectory(char *d){
	int z, hold;
	char *cdback="..";
	char buf[100];
	hold=0;
	z=0;
	getcwd(buf, sizeof(buf));	//Gives me the path of the currently working directory
	if(d=='\0'){
		chdir("/home");
	}
	//cd ..
	else if(!(strcmp(d, cdback))){ 
		z=0;
		while(buf[z]!='\0'){
			if(buf[z]=='/'){
 				hold=z;
			}
			z=z+1;
		}
		buf[hold+1]='\0';
		z=chdir(buf);
		if(z==-1){
			printf("i cant access the given directory\n");
		}
	}
	//When i want to open a folder by using its whole path. 
	else if(d==(strchr(d, '/'))){
		z=chdir(d);
		if(z==-1){
			printf("i cant access the given directory\n");
		}
	}
	//When the user gives as input a folder that already exists in the currently working directory. 
	else{
		strcat(buf,"/");
		strcat(buf, d);
		z=chdir(buf);
		if(z==-1){
			printf("i cant access the given directory\n");
		}
	}
}



//Main funtion that does all the work...
void mainfunction(char *s){
	pid_t pid;
	int *pd;
	int status, counter, i, j, k, l, cnt, c, cp, cp2, cp3, z, cntpipe, startindicator, endindicator, flag, orend, orstart, errorflag, identification;
	char **tokens;
	char *p;
	char buffer[100];
	char *tmp;
	char *n[512];
	char *erotimatiko=";";
	char *AndAnd="&&";
	char *Or="|";
	char space = ' ';
	char *quit = "quit";
	char *cd = "cd";
	char *sp = ">";
	
	counter=0;
	cp=0;
	
	//The next lines make sure wont stop even if the user gives empty lines as input.
	for(i=0;i<(strlen(s)-1);i++){
		if(s[i]!=space){
			flag=1;
		}
	}
	if(strlen(s)>512){
		printf("too many arguments. The max number of characters for input is 512 \n");
	}
	else if(s[0]=='\n'){
	}
	else if(flag==0){
		
	}
	else{
		//strtok puts a "\n" at the end of the string something that is not wanted here, so i change it.
		if ((strlen(s) > 0) && (s[strlen(s) - 1] == '\n')){
			s[strlen(s) - 1] = '\0';
		}
		tokens=(char **)malloc(256*sizeof(char*));
		
		//breaking the input string into tokens.
		i=0;
		p = strtok(s, " ");
		tokens[i]=p;
		while(p!=NULL){
			i=i+1;
			p=strtok(NULL, " ");
			if(p!=NULL){
				tokens[i]=p;
			}
			counter=counter+1;
		}
		if ((strcmp(tokens[i-1], erotimatiko))){
			tokens[i]=";";
			counter = counter + 1;
		}
		for(i=0;i<counter;i++){
			if((!(strcmp(tokens[i],erotimatiko)))||(!(strcmp(tokens[i],AndAnd)))){
				cp=cp+1;//counter that represents how many commands must be ecexuted.
			}
		}	
		for(j=0;j<cp;j++){

			if(j==0){
				endindicator=-1;
			}
			for(i=(endindicator+1);i<counter;i++){
				if(!(strcmp(tokens[i], Or))){
					flag=1;
					cp2=0;
					for(l=(endindicator+1);l<counter;l++){
						if((!(strcmp(tokens[l],erotimatiko)))||(!(strcmp(tokens[l],AndAnd)))){
							orstart=endindicator+1;//start of the command that  needs piping.
							orend=l;//end of the command that needs piping
							break;
						}
						if(!(strcmp(tokens[l], Or))){
							cp2=cp2+1;//counter that shows how many "|" exist.
						}
					
					}

					int cntpipe=3;//counter helpfull for pipes
					pd=(int*)malloc((2*cp2)*sizeof(int));
					for(l=0;l<cp2;l++){
						pipe(pd+(l*2));				//creating pipes.
					}
					for(l=0;l<(cp2+1);l++){
						//the first part of the command that needs piping.
						if(l==0){
							//clearing the content of n array.
							for(z=0;z<512;z++){
								n[z]=NULL;
							}
							//n the array that stores the arguments for the command that is going to be executed.
							cnt=0;
							for(k=(endindicator+1);k<i;k++){
								n[cnt]=tokens[k];
								cnt=cnt+1;
							}
							endindicator=i;

							if(!strcmp(n[0], cd)){
								changedirectory(n[1]);
								continue;
							}
							if(!(strcmp(n[0], quit))){
								printf("key word  (quit) was catched the program will now exit\n");
								exit(-1);
							}
	
							pid=fork();
							if(pid==0){
								dup2(pd[1],1);
								for(z=0;z<(cp2*2);z++){
									close(pd[z]);
								}
								identification=execvp(n[0], n);//in case the excecvp was not successful.
								if(identification==-1){
									printf("the command %s is not valid \n", n[0]);
									exit(-1);
								}	
							}
							
						}
						//the last part of the command that needs piping.
						else if(l==cp2){
							startindicator=endindicator+1;
							for(k=(endindicator+1);k<(orend+1);k++){
								if((!(strcmp(tokens[k],erotimatiko)))||(!(strcmp(tokens[k],AndAnd)))){
									endindicator=k;
									break;
								}
	
							}
							for(z=0;z<512;z++){
								n[z]=NULL;
							}
							cnt=0;
							for(k=startindicator;k<endindicator;k++){
								n[cnt]=tokens[k];
								cnt=cnt+1;
							}
							if(!strcmp(n[0], cd)){
								changedirectory(n[1]);
								continue;
							}
							if(!(strcmp(n[0], quit))){
								printf("key word  (quit) was catched the program will now exit\n");
								exit(-1);
							}
							pid=fork();
							if(pid==0){
								dup2(pd[((cp2*2)-2)],0);
								for(z=0;z<(cp2*2);z++){
									close(pd[z]);
								}
								identification=execvp(n[0], n);
								if(identification==-1){
									printf("the command  %s is not valid \n", n[0]);
									exit(-1);
								}
							}
						}
						//others parts of the command that needs piping.
						else{
							startindicator=endindicator+1;
							for(k=(endindicator+1);k<orend;k++){
								if(!(strcmp(tokens[k], Or))){
									endindicator=k;
									break;
								}
							}
							for(z=0;z<512;z++){
								n[z]=NULL;
							}
							cnt=0;
							for(k=startindicator;k<endindicator;k++){
								n[cnt]=tokens[k];
								cnt=cnt+1;
							}
							if(!strcmp(n[0], cd)){
								changedirectory(n[1]);
								continue;
							}
							if(!(strcmp(n[0], quit))){
								printf("key word  (quit) was catched the program will now exit\n");
								exit(-1);
							}
							pid=fork();
							if(pid==0){
								dup2(pd[cntpipe],1);
								dup2(pd[cntpipe-3],0);
								for(z=0;z<(cp2*2);z++){
									close(pd[z]);
								}
								identification=execvp(n[0], n);
								if(identification==-1){
									printf("the command  %s  is not valid \n", n[0]);
									exit(-1);
								}
								
							}
							cntpipe=cntpipe+2;
						}
					}

					if(pid>0){
						for(z=0;z<(cp2*2);z++){
							close(pd[z]);
						}
						for(i=0;i<(cp2+1);i++){
							wait(&status);
						}
					}
					break;
				}
				//Normal commands.
				else if((!(strcmp(tokens[i],erotimatiko)))||(!(strcmp(tokens[i],AndAnd)))){
					flag=0;
					errorflag=0;//flag in case we have the characters "&&".
					if(!(strcmp(tokens[i],AndAnd))){
						errorflag=1;
					}
					if((cp==1)||(j==0)){
						startindicator=0;
						endindicator=i;
						break;
					}
					else{
						startindicator=endindicator+1;
						endindicator=i;
						break;
					}
				}
				//When we have command that their output must be stored into files.
				else if(!(strcmp(tokens[i], sp))){
					flag=1;
					startindicator=endindicator+1;
					cp3=0;
					for(l=i;l<counter;l++){
						if((!(strcmp(tokens[l],erotimatiko)))||(!(strcmp(tokens[l],AndAnd)))){
							endindicator=l;
							break;
						}
						if(!(strcmp(tokens[l], sp))){
							cp3=cp3+1;
						}
					}
					for(z=0;z<512;z++){
						n[z]=NULL;
					}
					z=0;
					for(l=startindicator;l<i;l++){
						n[z]=tokens[l];
						z=z+1;
					}
					c=i+1;
					for(l=0;l<cp3;l++){
						tmp=tokens[c];
						getcwd(buffer, sizeof(buffer));
						strcat(buffer, "/");
						strcat(buffer, tmp);
						FILE *fp2;
						fp2=fopen(buffer, "w");
						
						if(fp2==NULL){
							printf("cant open file\n");
							exit(-1);
							
						}
						pid=fork();
						if(pid==0){
							//Making the output of the execvp ipnut for a file.
							dup2(fileno(fp2), STDOUT_FILENO);
							fclose(fp2);
							identification=execvp(n[0], n);
							if(identification==-1){
								printf("the command was not valid");
								exit(-1);
							}
						}
						c=c+2;
						fclose(fp2);
					}
					if(pid>0){
						for(l=0;l<cp3;l++){
							wait(&status);
						}
						break;
					}
				}
			}
		
			if(flag==0){
				for(l=0;l<512;l++){
					n[l]=NULL;
				}
				l=0;
				for(i=startindicator;i<endindicator;i++){
					n[l]=tokens[i];
					l=l+1;
				}
				n[511]=NULL;
				if(!strcmp(n[0], cd)){
					changedirectory(n[1]);
					break;
				}
				if(!(strcmp(n[0], quit))){
					printf("key word  (quit) was catched the program will now exit\n");
					exit(-1);
				}
				pid=fork();
				if(pid<0){
					printf("error cannot fork exit");
					exit(-1);
				}
				else if(pid==0){
					identification=execvp(tokens[startindicator], n);
					if(identification==-1){
						printf("the command %s is not valid \n", n[0]);
						if(errorflag==1){
							break;
						}
					}
				}
				else{
					wait(&status);
				}
			}	
		}
	}
}



int main(int argc, char**argv){
	pid_t pid;
	int i, h, tmp;
	char path[100];
	char temp[100];
	char str[512];
	char *p;
	char **t;
	t=(char**)malloc(100*sizeof(char*));
	i=0;
	printf("args is  %d\n", argc);
	if(argc==1){
		printf("you are in interactive mode\n");
		while(1){
			//i print the current folder after every command in order the user can have better control of the program.
			getcwd(path, sizeof(path));
			i=0;
			p = strtok(path, "/");
			t[i]=p;
			while(p!=NULL){
				p=strtok(NULL, "/");
				if(p!=NULL){
					i=i+1;
					t[i]=p;
				}
			}
			if(t[i]==NULL){
				t[i]="/";
				printf("favvas_8675 ~ %s>", t[i]);
			}
			else{
				printf("favvas_8675 ~/ %s>", t[i]);
			}
			fgets(str, sizeof(str), stdin);
			mainfunction(str);
		}
	}
	else if(argc==2){
		printf("you are in batch mode we are processing the input file\n");
		if (getcwd(path, sizeof(path)) != NULL){
			strcat(path, "/");
			strcat(path, argv[1]);
			printf("%s\n", path);
			FILE *fp;
			fp=fopen(path, "r");
			if(fp==NULL){
				printf("cannot open file properly or the file is not included in the currently working directory");
				exit(-1);
				
			}
			else{
				while(!feof(fp)){
					while(fgets(str,sizeof(str),fp)!=NULL){
						mainfunction(str);
					}
				}
				printf("the file didnt include \"quit\" however it will terminate");
				exit(0);
			}
		}
			
	}
	else{
		printf("the execution command was not valid please run again the executable file with this form \"executable.c\" for interactive mode and if you want batch mode type \"executable.c file\" \n");
		exit(-1);
	}
}
