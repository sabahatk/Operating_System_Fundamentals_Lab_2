#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(){
 	//variables
        char *argToken;
        char *execArgs[3];
        char command[20], readArr[1024], yOrN[1];
	const int SIZE = 1024;
	const char* name = "shared";
	int shm_fd, i, rd, pipeCheck;
	int shmPipe[2];
	char *ptr;
	//create a pipe
	pipeCheck = pipe(shmPipe);
	//pipe error checking
	if(pipeCheck <0){
		printf("Error in creating pipe. Error number: %d.", pipeCheck);
		return -1;
		exit(1);
	}
	i=0;
	rd=1;
	//prompt user for command
	printf("--SHELL--214887277>>");
        scanf("%[^\n]%*c", command);

	//split the command by space
	argToken = strtok(command, " ");
        while(argToken != NULL){
                execArgs[i] = argToken;
                i++;
                argToken = strtok(NULL, " ");
        }
	execArgs[i] = NULL;
        i++;

	//creates a read-only shared memory for the parent
        shm_fd = shm_open(name, O_CREAT|O_RDWR, 0666);
        if(shm_fd< 0){
	        printf("Error in opening shared memory # 1. Error number: %d", shm_fd);
                return -1;
                exit(1);
        }
        ftruncate(shm_fd, SIZE);
	ptr = (char *) mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);

	//creates a new process
	pid_t checkFork = fork();
	if(checkFork == -1){
                printf("opening child process failed");
        }
	//checks if it is a parent process
	else if(checkFork != 0){
   //             printf("In parent process. Command: %s. Child PID: %d. Parent PID: %d.\n", command, checkFork, getpid());
                printf("....working on your request....\n");
		//ptr = (char *) mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

		//switch to child process
                wait(NULL);

		//prompt user to display command
                printf("....output is ready. Display it now [Y/N]>>\n");
                scanf("%s", yOrN);
                if(yOrN[0] == 'y' || yOrN[0] == 'Y'){
			//display command
			//printf("Entered yes\n");
			printf("%s", ptr);
			shm_unlink(name);
		}
        }

	else{
     //         	printf("Entered child process. Command: %s Child PID: %d Parent PID: %d\n", command, getpid(), getppid());
		//opens a write only memory for the child process
		/*shm_fd = shm_open(name, O_WRONLY, 0666);
        	if(shm_fd < 0){
                	printf("Error in opening shared memory #2. Error number: %d", shm_fd);
                	return -1;
                	exit(1);
        	}
	        ftruncate(shm_fd, SIZE);
		
		ptr = (char *) mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);*/
		//copy stdout of execvp to pipe
		dup2(shmPipe[1], 1);
				
                execvp(execArgs[0], execArgs);

		//copy pipe contents to the shared memory
		//memcpy(ptr, shmPipe[0], 1024);
		while(rd>0){
                	rd = read(shmPipe[0], readArr, sizeof(readArr));
                        if(rd < 0){
                                       printf("Error in reading.\n");
                                        return -1;
                                        exit(1);
                         }
                         else if(rd == 0){
                         	break;
                         }
                         else{
                                sprintf(ptr, "%s", readArr);
				ptr += sizeof(readArr);
                                }
			}

		
        }
	return 0;
        exit(0);
}
