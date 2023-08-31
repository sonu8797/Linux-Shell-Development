#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <map>
#include <ctype.h>
#define MAX_LINE_LENGTH 100

using namespace std;

// Function to parse the process information from /proc/[pid]/stat file
void dfs(pid_t curr_pid, map<pid_t, vector<pid_t>> &graph, map<pid_t, bool> &vis, map<pid_t, int> &pid_childs, map<pid_t, pid_t> &proc_parent)
{
  int sum = 0;
  vis[curr_pid] = true;
  for (auto it : graph[curr_pid])
  {
    if (vis[it] || (it == proc_parent[curr_pid]))
      continue;
    dfs(it, graph, vis, pid_childs, proc_parent);
    sum += pid_childs[it] + 1;
  }
  pid_childs[curr_pid] = sum;
}
void get_process_info(pid_t pid, pid_t *ppid)
{
  char filename[100];
  sprintf(filename, "/proc/%d/stat", pid);

  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    perror("Failed to open stat file");
    exit(1);
  }

  char line[MAX_LINE_LENGTH];
  if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
  {
    perror("Failed to read stat file");
    exit(1);
  }

  int ppid_index;
  sscanf(line, "%*d %*s %*c %d", &ppid_index);
  *ppid = ppid_index;
  int utime, stime;
  // fscanf(fp, " %*d %*d %*d %*d %*u %*u %*u %*u %*u %d %d", &utime, &stime);
  // // fclose(fp);

  // int clk_tck = sysconf(_SC_CLK_TCK);
  // double user_time = (double)utime / clk_tck;
  // double sys_time = (double)stime / clk_tck;
  // double total_time = user_time + sys_time;
  fclose(fp);
}

void get_process_time(pid_t pid, long long int &proc_time)
{
  // int pid = atoi(argv[1]);
  char stat_file_path[32];
  sprintf(stat_file_path, "/proc/%d/stat", pid);

  FILE *file = fopen(stat_file_path, "r");
  if (!file)
  {
    printf("Could not open file %s\n", stat_file_path);
    return;
  }

  char *line;
  line = new char[256];
  fgets(line, 256, file);
  fclose(file);

  char *token;
  int i = 0;
  while ((token = strsep(&line, " ")) != NULL)
  {
    i++;
    if (i == 14)
    {
      long utime = atol(token);
      i++;
      token = strsep(&line, " ");
      long stime = atol(token);
      proc_time = utime + stime;
      // printf("Accumulated CPU time: %ld\n", utime + stime);
      break;
    }
  }
}

int main(int argc, char **argv)
{
  map<pid_t, pid_t> proc_parent;
  map<pid_t, long long int> process_time;
  DIR *proc_dir = opendir("/proc");
  if (proc_dir == NULL)
  {
    perror("Failed to open /proc directory");
    exit(1);
  }

  struct dirent *entry;
  while ((entry = readdir(proc_dir)) != NULL)
  {
    if (entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
    {
      pid_t pid = atoi(entry->d_name);
      pid_t ppid;
      get_process_info(pid, &ppid);
      get_process_time(pid, process_time[pid]);
      proc_parent[pid] = ppid;
    }
  }
  closedir(proc_dir);

  printf("Process ID to Parent ID mapping:\n");
  
  map<pid_t, vector<pid_t>> graph;
  for (auto it : proc_parent)
  {
    graph[it.first].push_back(it.second);
    graph[it.second].push_back(it.first);
  }
  map<pid_t, bool> vis;
  map<pid_t, int> pid_childs;
  dfs(1, graph, vis, pid_childs, proc_parent);
  // for (auto it : pid_childs)
  //   cout << it.first << " : " << it.second << endl;
  // for (const auto &[pid, ppid] : proc_parent)
  // {
  //   printf("%d -> %d Number of childs  : %d \n", pid, ppid , pid_childs[pid]);
  //   cout << " process time : " << process_time[pid] << endl;
  // }
  pid_t curr_proc = getpid(), it;
  cout << "Detecting process id is : " << curr_proc << endl;
  it = curr_proc;
  int flag = 0;
  // cout << argv[1] << endl ; 
  if (argc > 1 )
  {
    // cout << "yes" << endl ;
    while (it > 1000)
    {

      cout << "Parent of process " << it << " --- > " << proc_parent[it] << endl;
      if ((process_time[it] < 20 && pid_childs[it] >= 10) || pid_childs[it] >= 150)
      {
        // 
        // flag = 1;
        // break;
         bool sing_child = false ;
         int max1 = pid_childs[it] ; 
         int max2 = 0 ; 
         while( max1 - max2 >= 50 )
         {
             pid_t id  ;  
             max1 =  0 ; 
             max2 =  0 ;
            //  sort( graph[it].begin() , graph[it].end())  ; 
             
             for( auto itr : graph[it])
             {
                if( itr == proc_parent[it])continue; 
                // max1 = max( max1  , pi ) ; 
                if( max1 < pid_childs[itr] )
                {
                   max2 = max1 ; 
                   max1 = pid_childs[itr] ; 
                   id = itr ; 
                }
                else if(max2 < pid_childs[itr] )
                {
                   max2 = pid_childs[itr] ;
                }
                // min1 = min( min1 , pid_childs[itr]) ;
             } 

            //  cout << max1 << " " << max2 << endl ;
             it = id  ; 
             if( graph[proc_parent[it]].size() == 2)
             {
                // cout << 2 ;
                sing_child = true  ; 
                max2 = max1 ; 
             }
         }
        if(sing_child)cout << "Danger : A bug is detected with process1 id : " << it << endl;
        else cout << "Danger : A bug is detected with process id : " << proc_parent[it] << endl;
        flag = 1 ;
        break ; 
      }
      it = proc_parent[it];
    }
    if (flag == 0)
      cout << "Safe : No bug is detected " << endl;
  }
  else
  {
    while (it != 1)
    {
      cout << "Parent of process " << it << " ---> " << proc_parent[it] << endl;
      it = proc_parent[it];
    }
  }
  return 0;
}
