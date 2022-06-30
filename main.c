#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <stdbool.h>

#define MAXNUM 500
#define MAXLEN 1000

static FILE *fp; // log file pointer
static bool background = false;

// set directory
void set_env(){
    char dir[MAXLEN];
    getcwd(dir, sizeof(dir));
    chdir(dir);
}

void print_dir(){
    char dir[MAXLEN];
    getcwd(dir, sizeof(dir));
    printf("%s ", dir);
}

void proc_exit(){
    waitpid(getpid(), NULL, 0);
    fputs("Child process was terminated\n", fp);
    fflush(fp);

}

void parser(char *command[], char *st){
    int i;
    for(i = 0; i < MAXNUM; i++){
        command[i] = strsep(&st, " ");
        if(command[i] == NULL)
            break;
    }
}

 char* eval_exp(char *st){

    if(st[strlen(st) - 1] == '&'){
        background = true;
        st[strlen(st) - 2] = '\0';
    }

    char *parsed1[MAXNUM]; // parsed according $
    char *parsed2[MAXNUM]; // parsed according spaces
    int i, j = 0, k = 0;
    char temp[MAXLEN] = "";
    char temp2[MAXLEN] = "";
    char temp3[MAXLEN] = "";
    bool eval = false;
    strcpy(temp2, st);
    strcpy(temp, st); // to store the original command without changes
    for(i = 0; i < MAXNUM; i++){
        parsed1[i] = strsep(&st, "$");
        if(parsed1[i] == NULL){
            if(i > 1)
                eval = true;
            break;
        }
    }

    // remove spaces
    for(i = 1; parsed1[i] != NULL; i++)
        parser(parsed2, parsed1[i]);


    // remove " at the end of token
    for(i = 0; parsed2[i] != NULL; i++){
        int len = strlen(parsed2[i]);
        if(parsed2[i][ len - 1] == '\"')
            parsed2[i][len - 1] = '\0';
    }

    for(i = 0; i < strlen(temp) ; i++){
        if(temp[i] == '$' && parsed2[j] != NULL){
            strcat(temp3, getenv(parsed2[j++]));
            i ++;
            k += strlen(getenv(parsed2[j - 1]));
        }
        else
            temp3[k++] = temp[i];
    }

    char *ptc;

    // expression contains $
    if(eval){
        ptc = temp3;
    }
    // no $ found then return same expression
    else{
        ptc = temp;
    }
    return ptc;
}

void exec_command(char *command[]){
    pid_t pid = fork();

    if(pid == -1){
        printf("child creation failure\n");
        return;
    }
    else if(pid == 0){
        execvp(command[0], command);
        printf("ERROR HAPPENED or command does't exist\n");
        exit(1);
    }
    else if(background == false && pid != 0)
        waitpid(pid, NULL, 0);

}

void execute(char *command[], char *st){

    if(strcmp(command[0], "exit") == 0){
        return;
    }
    else if(strcmp(command[0], "cd") == 0){
        // hande cd ~ or cd
        if( st[2] == '\0' || (strcmp(command[1], "~") == 0))
            chdir(getenv("HOME"));
        else
            chdir(command[1]);
    }
    else if(strcmp(command[0], "echo") == 0){
        char *parsed[2];
        int i, j = 0;
        char temp2[MAXLEN]; // stores string without echo word

        // removing echo word from command
        for(i = 5; i < strlen(st); i++)
            temp2[j++] = st[i];

        strcpy(st, temp2);
        for(i = 0; i < 2; i++)
            parsed[i] = strsep(&st, "\"");

        printf("%s\n", parsed[1]);

    }
    else if(strcmp(command[0], "export") == 0){
        char *parsed[2];
        char temp[MAXLEN]; // stores string between double quotes
        int i, j = 0;
        char temp2[MAXLEN] = ""; // stores string without export word

        // removing export word from the command
        for(i = 7; i < strlen(st); i++) // i = 7 as length of export + space
            temp2[j++] = st[i];

        strcpy(st, temp2);
        for(i = 0; i < 2; i++)
            parsed[i] = strsep(&st, "=");

        // taking value between double quotes e.g " a b c" => a b c
        j = 0;
        if(parsed[1][0] == '\"'){
            for(i = 1; parsed[1][i] != '\"'; i++)
                temp[j++] = parsed[1][i];
            temp[j] = '\0';
            strcpy(parsed[1], temp);
        }
        setenv(parsed[0], parsed[1], 1);
    }
    else{
        exec_command(command);
    }

}

void shell (){
    char *parsedCommand[MAXNUM] ;
    char st[MAXLEN]; //input string (edited one as parsing will add \0 to the end of each word)
    char original[MAXLEN]; // input string
    char temp[MAXLEN];
    do{
        if(background == false){
            print_dir();
        }
        scanf("%[^\n]%*c",st); // to allow scanning spaces using scanf
        strcpy(temp, eval_exp(st));
        strcpy(original, temp);
        parser(parsedCommand, temp);
        execute(parsedCommand, original);
        background = false;
    }
    while(strcmp(st, "exit") != 0);
}

int main()
{
    fp = fopen("log.txt", "w");
    signal(SIGCHLD, proc_exit);
    set_env();
    shell();
    fclose(fp);
    return 0;
}
