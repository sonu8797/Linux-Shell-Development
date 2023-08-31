#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <signal.h>
#include <sys/wait.h>
using namespace std;
#define MAX_COMMAND_LENGTH 100
#define MAX_PID_LENGTH 10
// Returns the process ID that has the target file open
void find_process_with_open_file(const char *target_file, vector<int> &pids, vector<int> &pids_lock)
{
  DIR *dir;
  struct dirent *ent;

  dir = opendir("/proc");
  if (dir != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      // Check if the entry is a process ID
      if (ent->d_type == DT_DIR && atoi(ent->d_name) > 0)
      {
        char path[1024];
        sprintf(path, "/proc/%s/fd", ent->d_name);

        DIR *fd_dir;
        struct dirent *fd_ent;

        fd_dir = opendir(path);
        if (fd_dir != NULL)
        {
          while ((fd_ent = readdir(fd_dir)) != NULL)
          {
            char link_path[1024];
            snprintf(link_path, sizeof(link_path), "%s/%s", path, fd_ent->d_name);

            char buf[1024];
            ssize_t len = readlink(link_path, buf, sizeof(buf) - 1);
            if (len != -1)
            {
              buf[len] = '\0';
              if (strcmp(buf, target_file) == 0)
              {
                // Found the process with the target file open
                pids.push_back(atoi(ent->d_name));
                char path1[1024];
                sprintf(path1, "/proc/%d/fdinfo/%d", atoi(ent->d_name), atoi(fd_ent->d_name));
                FILE *file_info = fopen(path1, "r");
                if (file_info == NULL)
                  continue;

                char line[256];
                while (fgets(line, sizeof(line), file_info) != NULL)
                {
                  if (strcmp(strtok(line, ":"), "lock") == 0)
                    pids_lock.push_back(atoi(ent->d_name));
                }
              }
            }
          }
          closedir(fd_dir);
        }
      }
    }
    closedir(dir);
  }
}

void kill_process(set<int> pids)
{
  for (auto it : pids)
  {
    int result = kill(it, SIGKILL);
    if (result == -1)
    {
      printf("Error killing process %d \n" ,it);
      return;
    }
  }
}

void delete_file(const char *filepath) {
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "rm %s", filepath);

  int status = system(command);
  if (status == -1) {
    perror("Failed to delete file");
    exit(1);
  }
}
int main(int argc, char *argv[])
{
  vector<int> pids;
  vector<int> pids_lock;
  // char *filepath = "/home/vinod/Documents/IIT_Kharagpur/CSE_Sem_6/OS_Lab/Assgn2/file.txt" ;
  find_process_with_open_file(argv[1], pids, pids_lock);
  if (pids.size() > 0)
  {
    for (auto it : pids)
    {
      printf("Process with ID %d has the target file open\n", it);
    }
  }
  else
  {
    printf("No process was found with the target file open\n");
  }
  if (pids_lock.size() > 0)
  {
    for (auto it : pids_lock)
    {
      printf("Process with ID %d has the lock the file open\n", it);
    }
  }
  else
  {
    printf("No process lock the file\n");
  }
  set<pid_t> all_proc ; 
  for( auto it : pids )
  {
     all_proc.insert(it) ;
  }
  // set<pid_t> all_proc ; 
  for( auto it : pids_lock )
  {
     all_proc.insert(it) ;
  }
  cout << "Do you want to kill these processes and delete the file (y/n)? " << endl  ;
  // char input[10];
  string inp ;
  cin >> inp ; 
  if( inp == "y")
  {
      kill_process( all_proc) ;
      delete_file(argv[1]) ; 
  }
  
  return 0;
}
