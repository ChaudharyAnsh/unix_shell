#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <string.h>
#include <fcntl.h>

static sigjmp_buf env;

struct history
{
        char cmd_text[1024];
        int freq;
} hist;
int buffer = 1024,
    status;
struct history H[100];
int NO_OF_USED_COMMANDS = 0;
int min(int a, int b) { return a < b ? a : b; }
void printhistory(int n)
{
        for (int i = 0; i < min(NO_OF_USED_COMMANDS, n); i++)
                printf("%d. %s : %d\n", i + 1, H[i].cmd_text, H[i].freq);
}
void signal_handler(int signum) { siglongjmp(env, 98); }
int format_cmd(char *inputBuffer, char *inputcopy, char **args, int *flag)
{
        scanf("%[^\n]s", inputBuffer);
        getchar();
        int len = strlen(inputBuffer);
        for (int i = 0; i <= len; ++i)
        {
                inputcopy[i] = inputBuffer[i];
        }
        inputcopy[len] = '\0';
        inputBuffer[len] = '\n';

        int st = -1,
            arguments = 0;

        if (len == 0)
                exit(0);

        if (len < 0)
                return -1;

        for (int i = 0; i <= len; i++)
        {
                switch (inputBuffer[i])
                {
                case ' ':
                case '\t':
                        if (st != -1)
                        {
                                args[arguments] = inputBuffer + st;
                                arguments++;
                        }
                        inputBuffer[i] = '\0';
                        st = -1;
                        break;
                case '\n':
                        if (st != -1)
                        {
                                args[arguments] = inputBuffer + st;
                                arguments++;
                        }
                        inputBuffer[i] = '\0';
                        st = -1;
                        break;

                default:
                        if (st == -1)
                                st = i;
                        if (inputBuffer[i] == '&')
                        {
                                *flag = 1;
                                inputBuffer[i] = '\0';
                        }
                }
        }
        args[arguments] = NULL;
        return arguments;
}

int main()
{
        int flag = 0;
        size_t MAX_SIZE = 1024;
        signal(SIGINT, signal_handler);
        char input[1024];
        char input2[1024];
        char *command[50];
        while (1)
        {
                if (sigsetjmp(env, 1) == 98)
                {
                        printf("current process exit.\n");
                        fflush(stdout);
                }
                fflush(stdout);
                printf("sh > ");
                fflush(stdout);
                if (format_cmd(&input[0], &input2[0], command, &flag) < 0)
                {
                        printf("read fail");
                        break;
                }
                int ind = 0, i = 0, fd[2], saved_fds[2], io[2] = {-1, -1};
                saved_fds[0] = dup(STDIN_FILENO);
                saved_fds[1] = dup(STDOUT_FILENO);

                // adding command to H.
                for (int search = 0; search < NO_OF_USED_COMMANDS; search++)
                {
                        if (strcmp(input2, H[search].cmd_text) == 0)
                        {
                                H[search].freq += 1;
                                goto sort_commands;
                        }
                }
                while (input2[i] != '\0')
                {
                        ((H[NO_OF_USED_COMMANDS]).cmd_text)[i] = input2[i];
                        i++;
                }
                (H[NO_OF_USED_COMMANDS]).freq = 1;
                NO_OF_USED_COMMANDS += 1;

        sort_commands:
                // sort commands in history.
                for (int i = 0; i < NO_OF_USED_COMMANDS - 1; i++)
                {
                        if (H[i].freq < H[i + 1].freq)
                        {
                                int j = i + 1;
                                while (j != 0 || H[j].freq <= H[j - 1].freq)
                                {
                                        struct history temp = H[j];
                                        H[j] = H[j - 1];
                                        H[j - 1] = temp;
                                        j--;
                                }
                                break;
                        }
                }

                pid_t pid = fork();
                if (pid == 0)
                {
                        signal(SIGINT, SIG_DFL);
                        if (strcmp(command[0], "exit") == 0)
                                exit(0);

                        if (strcmp(command[0], "cd") == 0)
                        {
                                sleep(0.5);
                                exit(0);
                        }

                        if (strcmp(command[0], "history") == 0)
                        {
                                printhistory(10);
                                exit(0);
                        }

                        while (command[ind] != NULL)
                        {
                                if (strcmp(command[ind], ">") == 0)
                                {
                                        fd[1] = open(command[ind + 1], O_CREAT | O_WRONLY | O_APPEND, 0666);
                                        dup2(fd[1], STDOUT_FILENO);
                                        command[ind] = NULL;
                                        if (execvp(command[0], command) < 0)
                                                exit(1);

                                        exit(0);
                                }
                                if (strcmp(command[ind], "<") == 0)
                                {
                                        fd[0] = open(command[ind + 1], O_RDONLY);
                                        dup2(fd[0], STDIN_FILENO);
                                        command[ind] = NULL;
                                        if (execvp(command[0], command) < 0)
                                                exit(1);
                                }

                                if (strcmp(command[ind], "|") == 0)
                                {
                                        pipe(fd);

                                        pid_t subchild = fork();
                                        if (subchild > 0)
                                        {
                                                close(fd[1]);
                                                dup2(fd[0], STDIN_FILENO);
                                                dup2(saved_fds[1], STDOUT_FILENO);
                                                wait(NULL);
                                                if (execvp(command[ind + 1], command + ind + 1) == -1)
                                                {
                                                        printf("Error executing second command.\n");
                                                }
                                        }
                                        else
                                        {
                                                close(fd[0]);
                                                dup2(saved_fds[0], STDIN_FILENO);
                                                dup2(fd[1], STDOUT_FILENO);
                                                command[ind] = NULL;
                                                if (execvp(command[0], command) == -1)
                                                {
                                                        printf("Error executing first command.\n");
                                                }
                                        }
                                        exit(0);
                                }
                                ind++;
                        }
                        if (execvp(command[0], command) == -1)
                                printf("%s failed.\n", command[0]);
                        exit(0);
                }
                else
                {
                        if (strcmp(command[0], "cd") == 0)
                                if (chdir(command[1]) < 0)
                                        printf("cd failed\n");

                        if (strcmp(command[0], "exit") == 0)
                                exit(0);

                                
                        if (flag == 0)
                                waitpid(pid, &status, 0);
                        else
                                flag = 0;
                }
        }
}