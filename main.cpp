#include <bits/stdc++.h>
#include <sys/types.h>
#include <stdlib.h>

#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <glob.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
using namespace std;
vector<string> command_history;
int current_history_index = -1;
char *command = NULL;

int saved_stdout;
int saved_stdin;
static sigjmp_buf env;
static volatile sig_atomic_t jump_active = 0;
int counter = 0;
void sigint_handler(int signo)
{
    dup2(saved_stdout, STDOUT_FILENO); /* repeat until done ....         */
    dup2(saved_stdin, STDIN_FILENO);
    siglongjmp(env, 42);
}
void sighandler(int sig_num)
{
    // Reset handler to catch SIGTSTP next time
    signal(SIGTSTP, sighandler);
    // printf("Cannot execute Ctrl+Z\n");
    dup2(saved_stdout, STDOUT_FILENO); /* repeat until done ....         */
    dup2(saved_stdin, STDIN_FILENO);
    if (command)
        printf("\n[%d]+   Stopped         %s", counter, command);
    else
        printf("\n[%d]+   Stopped         ", counter);
    counter++;
    siglongjmp(env, 42);
}

void add_to_history(const string &command)
{
    if (command_history.size() == 1000)
    {
        command_history.erase(command_history.begin());
        command_history.push_back(command);
    }
    else
        command_history.push_back(command);
    current_history_index = command_history.size();
}

int up_arrow_handler(int count, int key)
{
    if (count == 0)
    {
        return 0;
    }
    // Go to the previous command in history
    //   HIST_ENTRY *entry = previous_history();
    //   if (entry) {
    //     command = strdup(entry->line);
    //     rl_replace_line(command, 0);
    //     rl_point = rl_end;
    //   }
    if (current_history_index > 0)
    {
        current_history_index--;
        command = strdup(command_history[current_history_index].c_str());
        rl_replace_line(command, 0);
        rl_point = rl_end;
        // cout << command << endl;
    }
    return 1;
}

int down_arrow_handler(int count, int key)
{
    if (count == 0)
    {
        return 0;
    }
    // Go to the next command in history
    // HIST_ENTRY *entry = next_history();
    // if (entry)
    // {
    //     command = strdup(entry->line);
    //     rl_replace_line(command, 0);
    //     rl_point = rl_end;
    // }
    if (current_history_index < command_history.size() - 1)
    {
        current_history_index++;
        command = strdup(command_history[current_history_index].c_str());
        rl_replace_line(command, 0);
        rl_point = rl_end;
        // cout << command << endl;
    }
    else if (current_history_index == command_history.size() - 1)
    {
        command = strdup("");
        rl_replace_line(command, 0);
        rl_point = rl_end;
    }
    return 1;
}

int beginning_of_line_handler(int count, int key)
{
    rl_point = 0;
    return 0;
}

int end_of_line_handler(int count, int key)
{
    rl_point = rl_end;
    return 0;
}
void parse(string cmd, vector<string> &argv, int &inp, int &outp, vector<int> &pipes, vector<bool> &wc)
{
    string str;
    bool val = false;
    for (int i = 0; cmd[i] != '\0'; i++)
    {
        if (cmd[i] == '?' || cmd[i] == '*')
            val = true;
        if ((i == cmd.size() - 1) && (cmd[i] == '&'))
            continue;
        if (cmd[i] == '|')
        {
            if (!str.empty())
                argv.push_back(str);
            pipes.push_back(argv.size());
            str.clear();
            wc.push_back(val);
            val = false;
            continue;
        }
        if (cmd[i] == '<')
        {
            if (!str.empty())
                argv.push_back(str);
            inp = argv.size();
            str.clear();
            wc.push_back(val);
            val = false;
        }
        else if (cmd[i] == '>')
        {
            if (!str.empty())
                argv.push_back(str);
            outp = argv.size();
            // cout << outp << endl ;
            str.clear();
            wc.push_back(val);
            val = false;
        }
        else if (cmd[i] == ' ' || cmd[i] == '\n' || cmd[i] == '\t')
        {
            while (cmd[i] == ' ' || cmd[i] == '\n' || cmd[i] == '\t')
                i++;
            if (!str.empty())
                argv.push_back(str);
            i--;
            str.clear();
            wc.push_back(val);
            val = false;
        }
        else
            str.push_back(cmd[i]);
    }
    if (!str.empty())
    {
        argv.push_back(str);
        wc.push_back(val);
    }

    // argv.push_back(NULL);
}

void execute(string cmd)
{
    vector<string> argv;
    vector<int> pipes;
    vector<bool> wc;
    pid_t pid;
    int status;
    int inp = -1;
    int outp = -1;

    parse(cmd, argv, inp, outp, pipes, wc);
    if (argv[0] == "cd")
    {
        char path[1000];
        if (argv.size() == 1)
        {
            strcpy(path, "/home/vinod");
        }
        else
        {
            strcpy(path, argv[1].c_str());
        }
        int res = chdir(path);
        if (res != 0)
        {
            cout << "bash: cd: " << argv[1] << ": No such file or directory" << endl;
        }
        return;
    }
    if( argv[0] == "delep")
    {
        if ((pid = fork()) < 0)
        { /* fork a child process           */
            cout << "*** ERROR: forking child process failed\n";
            exit(1);
        }
        else if (pid == 0)
        { /* for the child process: 
                */
            // argv[1].c_str() ;
            char *path  = new char[argv[1].size() + 1]; 
            strcpy(path , argv[1].c_str()) ;
            // char commands[2][1000] = {"./fp" , path} ;
            vector< char *>  cmds ; 
            char *s = "./fp" ;
            cmds.push_back(s) ; 
            cmds.push_back(path) ;
            if (execvp( cmds[0] , cmds.data()) < 0)
            { /* execute the command  */
                printf("*** ERROR: exec failed\n");
                exit(1);
            }
        }
        else
        {
            /* for the parent:      */
            while (wait(&status) != pid) /* wait for completion  */
                ;
        }
        return;
    }
    if( argv[0] == "sb")
    {
       if ((pid = fork()) < 0)
        { /* fork a child process           */
            cout << "*** ERROR: forking child process failed\n";
            exit(1);
        }
        else if (pid == 0)
        { /* for the child process: 
                */
            // argv[1].c_str() ;
            // char commands[2][1000] = {"./fp" , path} ;
            vector< char *>  cmds ; 
            char *s = "./sb" ;
            cmds.push_back(s) ;
            if(argv.size() > 1)
            {
                char *s1 = strdup(argv[1].c_str()) ;
                
                cmds.push_back(s1) ; 
            }
            // cmds.push_back(path) ;
            if (execvp( cmds[0] , cmds.data()) < 0)
            { /* execute the command  */
                printf("*** ERROR: exec failed ok\n");
                exit(1);
            }
        }
        else
        {
            /* for the parent:      */
            while (wait(&status) != pid) /* wait for completion  */
                ;
        }
        return; 
    }
    if (pipes.size() != 0)
    {
        int ind = 0;
        vector<vector<char *>> commands;
        vector<char *> cmd;
        int k = 0;
        for (int i = 0; i < argv.size(); i++)
        {
            if (i == pipes[k])
            {
                cmd.push_back(NULL);
                commands.push_back(cmd);
                cmd.clear();
                k++;
            }
            char *arr1 = new char[argv[i].length() + 1];
            if (wc[i])
            {
                strcpy(arr1, argv[i].c_str());
                glob_t results;
                int ret = glob(arr1, 0, NULL, &results);
                for (size_t i = 0; i < results.gl_pathc; i++)
                {
                    cmd.push_back(results.gl_pathv[i]);
                }
                globfree(&results);
            }
            else
            {
                strcpy(arr1, argv[i].c_str());
                cmd.push_back(arr1);
            }
        }
        if (!cmd.empty())
        {
            cmd.push_back(NULL);
            commands.push_back(cmd);
        }
        int pipefd[2];
        pid_t pid;
        int n = commands.size();
        for (int i = 0; i < n - 1; i++)
        {
            if (pipe(pipefd) == -1)
            {
                cout << "Error creating pipe" << endl;
                return;
            }

            pid = fork();
            if (pid == -1)
            {
                cout << "Error forking process" << endl;
                return;
            }

            if (pid == 0)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);

                execvp(commands[i][0], commands[i].data());

                cout << "Error executing command" << endl;
                return;
            }
            else
            {
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
            }
        }

        // execvp(commands[n - 1][0], commands[n - 1].data());
        if ((pid = fork()) < 0)
        { /* fork a child process           */
            cout << "*** ERROR: forking child process failed\n";
            exit(1);
        }
        else if (pid == 0)
        { /* for the child process:         */
            if (execvp(commands[n - 1][0], commands[n - 1].data()) < 0)
            { /* execute the command  */
                printf("*** ERROR: exec failed\n");
                exit(1);
            }
        }
        else
        {
            /* for the parent:      */
            while (wait(&status) != pid) /* wait for completion  */
                ;
        }
        return;
    }
    // const char *cstr = argv[0].c_str();
    // char **arr = new char *[argv.size() + 1];
    // for( int i = 0 ; i < argv.size() ;i++ )cout << argv[i] << " " ;
    // for( int i = 0 ; i < wc.size() ;i++ )cout << wc[i] << " " ;
    vector<char *> arr;
    for (int i = 0; i < argv.size(); i++)
    {
        char *arr1 = new char[argv[i].length() + 1];
        if (wc[i])
        {
            // cout << 1 ;
            strcpy(arr1, argv[i].c_str());
            glob_t results;
            int ret = glob(arr1, 0, NULL, &results);
            for (size_t i = 0; i < results.gl_pathc; i++)
            {
                arr.push_back(results.gl_pathv[i]);
                // cout << arr[arr.size() - 1] << endl ;
                // for( int i = 0 ; i < arr.size() ;i++ )if(arr[i] != NULL)cout << arr[i] << " " ;
                // cout << endl ;
            }
            // globfree(&results);
        }
        else
        {
            strcpy(arr1, argv[i].c_str());
            // cout << 2;
            arr.push_back(arr1);
        }
    }
    // for( int i = 0 ; i < arr.size() ;i++ )if(arr[i] != NULL)cout << arr[i] << endl ;
    int saved_stdout = dup(STDOUT_FILENO);
    arr.push_back(NULL);
    if (inp != -1)
    {
        int fd = open(argv[inp].c_str(), O_RDONLY);
        dup2(fd, STDIN_FILENO);
    }
    if (outp != -1)
    {
        // cout << outp << endl ;
        int fd = open(argv[outp].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
    }

    if ((pid = fork()) < 0)
    { /* fork a child process           */
        cout << "*** ERROR: forking child process failed\n";
        exit(1);
    }
    else if (pid == 0)
    { /* for the child process:         */
        if (execvp(arr[0], arr.data()) < 0)
        { /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else
    {
        if (cmd.back() == '&')
            return;                  /* for the parent:      */
        while (wait(&status) != pid) /* wait for completion  */
            ;
    }
}
int main()
{
    // vector<string> history;
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sighandler);
    fstream new_file;
    new_file.open("shell_history.txt", ios::in);

    if (new_file.is_open())
    {
        string sa;
        while (getline(new_file, sa))
            command_history.push_back(sa);

        // new_file.close();
    }

    // Bind the up arrow key to the up_arrow_handler function
    rl_bind_keyseq("\033\[A", up_arrow_handler);
    // Bind the down arrow key to the down_arrow_handler function
    rl_bind_keyseq("\033\[B", down_arrow_handler);
    // Bind the key sequence "Ctrl + a" to the beginning_of_line_handler function
    rl_bind_keyseq("\001", beginning_of_line_handler);
    // Bind the key sequence "Ctrl + e" to the end_of_line_handler function
    rl_bind_keyseq("\005", end_of_line_handler);

    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);
    if (sigsetjmp(env, 1) == 42)
    {
        cout << endl;
    }
    while ((command = readline("$ ")) != NULL)
    {
        jump_active = 1;
        string cmd(command);
        if (!cmd.empty())
        {
            add_to_history(cmd);
        }
        if (cmd == "exit")
        {
            break;
        }
        execute(cmd); /*  execute the command */
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        free(command);
        command = NULL;
    }
    // new_file.open("/home/vinod/Documents/IIT_Kharagpur/CSE_Sem_6/OS_Lab/Assgn2/shell_history.txt", ios::out);

    if (new_file.is_open())
    {
        for (auto it : command_history)
            new_file << (it + "\n");

        new_file.close();
    }
}