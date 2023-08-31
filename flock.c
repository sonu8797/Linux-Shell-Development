#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/file.h>
#include<sys/types.h>
int main(int argc, char *argv[]) {
  int fd = open("file.txt", O_RDWR);
  pid_t id = getpid() ;
  printf("%d\n",id) ;
  if (fd == -1) {
    perror("open");
    return 1;
  }

  if (flock(fd, LOCK_SH) == -1) {
    perror("flock");
    return 1;
  }
  // while(1);
  // The file is now locked, do some operations on the file
  int x ; 
  scanf("%d", &x) ;
  if (flock(fd, LOCK_UN) == -1) {
    perror("flock");
    return 1;
  }
  
  close(fd);
  return 0;
}
