#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>

char *getWord(char *end) {
    int i = 0;
    char ch;
    char *word = NULL;

    if (*end == '\n') {
        return NULL;
    }

    ch = getchar();

    if (ch == '\n') {
        *end = ch;
        return NULL;
    }
    while (i == 0 && (ch == ' ' || ch == '\t' )) { 
        ch = getchar();
        if (ch == '\n') {
            *end = ch;
            return NULL;
        }
    }
    while (ch != ' ' && ch != '\t' && ch != '\n') {
        word = (char *)realloc(word, (i + 1) * sizeof(char));
        word[i] = ch;
        i++;
        ch = getchar();
    }
    word = realloc(word, (i + 1) * sizeof(char));
    word[i] = '\0';
    *end = ch;
    return word;
}

char **getList() {
    char end = 0;
    char **list = NULL;
    int i = 0;
    while (end != '\n') {
        list = (char **)realloc(list, (i + 1) * sizeof(char*));
        list[i] = getWord(&end);
        i++;
    }
    list = (char **)realloc(list, (i + 1) * sizeof(char*));
    list[i] = NULL;
    return list; 
} 

void freeList(char **list) {
    for (int i = 0; list[i]; i++) {
        free(list[i]);
    }
    free(list);
}


int isExit(char **list) {
    if (list[0] != NULL) {
        if ((strcmp(list[0], "exit") == 0) || (strcmp(list[0], "quit") == 0)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void pipeForTwo(char **list, int iForP) {
	int fdForP[2];
	int i, k, j, p;
    char **cmd_A, **cmd_B;

	cmd_A = (char **)malloc((iForP + 1) * sizeof(char*));
	for (i = 0; i < iForP; i++) {
		cmd_A[i] = list[i];
	}

	cmd_A[i] = NULL;
	i = iForP + 1;
	while (list[i] != NULL) {
		i++;
    }

	cmd_B = (char **)malloc((i - iForP) * sizeof(char*));
    k = 0;
	for (j = iForP + 1; j < i;j++) {
		cmd_B[k] = list[j];
        k++; 
	}

	cmd_B[k] = NULL;
    list[iForP] = NULL;
    iForP = 0;
    while (list[i] != NULL) {
        if (strcmp(list[i], "|") == 0) {
            iForP = i;
        }
        i++;
    }
	pipe(fdForP);
	if (fork() == 0) {
		dup2(fdForP[1], 1);
		close(fdForP[0]);
		close(fdForP[1]);
        execvp(cmd_A[0], cmd_A);
        _exit(1);
	}
	if (fork() == 0) {
		dup2(fdForP[0], 0);
		close(fdForP[0]);
		close(fdForP[1]);
		execvp(cmd_B[0], cmd_B);
        _exit(1);
	}
    close(fdForP[0]);
	close(fdForP[1]);
	wait(NULL);
	wait(NULL);
	return;
}

char ***getCmdArr(char **list) {
    int i = 0;
    char ***getCmdArr = NULL;
    while (list[i] != NULL) {
        if (strcmp(list[i], "|") == 0) {
            
        } else if 
           
    }

    return;
}

void pipeForN(char **list, int *iForP, int n) {
    int fdForP[n][2], pid;
    char ***cmd_array = ;

    for (int i = 0; i < n + 1; i++) {
        if (i != n) {
            pipe(fdForP[i]);
        }
        if ((pid = fork ()) == 0) {
            if (i != 0) {
                dup2(fdForP[i - 1][0], 0);
                close(fdForP[i - 1][0]);
                close(fdForP[i - 1][1]);
            }

            if (i != n) {
                dup2(fdForP[i][1], 1);
            }
            close(fdForP[i][0]);
            close(fdForP[i][1]);
            execvp(cmd_array[i][0], cmd_array[i]);
        } else {
            if (i != 0) {
                close(fdForP[i - 1][0]);
            }
            close(fdForP[i][1]);
            waitpid(pid, NULL, 0);
        }
    }   
}

int *background_pids = NULL;
int background = 0;

int flow(char **list) {
    int flag = 0;
    int fd = 0;
    int i = 0;
    int iForP = 0;
   
    int pid;


    while (list[i] != NULL) {
        if (strcmp(list[i], "<") == 0) {
            fd = open(list[i + 1], O_RDONLY);
            flag = 0;
            if (fd < 0) {
                perror("Open failed");
                exit(1);
            }
            free(list[i + 1]);
            list[i] = NULL;
            break;
        } else if (strcmp(list[i], ">") == 0) {
            fd = open(list[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            flag = 1;       
            if (fd < 0) {
                perror("Open failed");
                exit(1);
            }
            free(list[i + 1]);
            list[i] = NULL;
            break;
        } else if (strcmp(list[i], "|") == 0) {
            iForP = i;
        } else if (strcmp(list[i], "&") == 0) {
            background++;
            free(list[i]);
            list[i] = NULL;
            break;
        }
        i++;
    }

    if (iForP != 0) {
        pipeForTwo(list, iForP);
    } else {
        if ((pid = fork()) > 0) {
            background_pids = realloc(background_pids, background * sizeof(int));
            background_pids[background - 1] = pid;
            if (background == 0) {
                wait(NULL);
            }
        } else {
            int tmp;
            if (fd) {
                dup2(fd, flag);
            }
            if (execvp(list[0], list) < 0) {
                perror("exec failed");
                return 1;
            }
            return 1;
        }
    }

    if (fd) {
        close(fd);
    }
    fd = 0;
}

int main(void) {
    

    printf("SUPER EVA'S TRMNAL >>");
    char **list = getList();
    while (!isExit(list)) {
        printf("SUPER EVA'S TRMNAL >>");
        flow(list);
        freeList(list);
        list = getList();
    }
    
    freeList(list);

    for (int i = 0; i < background; i++) {
        waitpid(background_pids[i], NULL, 0);
    }
    return 0;
}
