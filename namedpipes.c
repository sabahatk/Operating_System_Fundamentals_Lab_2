#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(){
        int fdc, fdp, cd, rd, checkfifo, i;
        char * pPath = "/home/user/Downloads/EECS_3221/q2/pPath";
	char *argToken;
	char *execArgs[3];
	char command[20], readArr[1], yOrN[1];
        
	checkfifo = mkfifo(pPath, 0777);
        if(checkfifo < 0){
                printf("Error in mkfifo. Error num: %d\n", checkfifo);
        }
	
	rd = 1;
	i=0;
	
	printf("--SHELL--214887277>>");
	scanf("%[^\n]%*c", command);
	
	argToken = strtok(command, " ");
	while(argToken != NULL){
		execArgs[i] = argToken;
		i++;
		argToken = strtok(NULL, " ");
	}
	execArgs[i] = NULL;
	i++;

	pid_t checkFork = fork();
	if(checkFork == -1){
		printf("opening child process failed");
	}
	else if(checkFork != 0){
        	//printf("In parent process. Command: %s. Child PID: %d. Parent PID: %d.\n", command, checkFork, getpid());
        	printf("....working on your request....\n");
		fdp = open(pPath, O_RDONLY);		
		if(fdp < 0){
                	printf("Error in opening (parent).\n");
                        return -1;
                        exit(1);
                }
		wait(NULL);
                printf("....output is ready. Display it now [Y/N]>>\n");
		scanf("%s", yOrN);
		if(yOrN[0] == 'y' || yOrN[0] == 'Y'){
			//printf("Entered 'Yes'\n");
			while(rd > 0){
				rd = read(fdp, readArr, sizeof(readArr));
				if(rd < 0){
					printf("Error in reading.\n");
					return -1;
					exit(1);	
				}
				else if(rd == 0){
					//printf("\n");
					break;
				}
				else{
					printf("%c", readArr[0]);
				}
			}
		cd = close(fdp);
		if(cd < 0){
			printf("Error in closing file. Error number: %d", cd);
			return -1;
			exit(1);
		}
		system("rm pPath");
		}	
	}

	else{	
//		printf("Entered child process. Command: %s Child PID: %d Parent PID: %d\n", command, getpid(), getppid());
	
		fdc = open(pPath, O_WRONLY);
		if(fdc < 0){
			printf("Error in opening. (child).\n");
			exit(1);
		}
		dup2(fdc, 1);
		execvp(execArgs[0], execArgs);
		cd = close(fdc);
		if(cd < 0){
                        printf("Error in closing file. Error number: %d", cd);
                        return -1;
                       	exit(1);

		}
	
	}
//}	
	return 0;
	exit(0);
}
