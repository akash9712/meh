// Compile and run with gcc shell.c && a.out

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHELL_BUFFER_SIZE 2048
#define SHELL_TOKENS_SIZE 2048
#define TOKEN_DELIM "\t\r\n\a"

// No built-in support. 

void check_memory_allocation(void* memory)
{
  if(memory == NULL)
  {
    perror("Memory allocation error");
    exit(EXIT_FAILURE);
  }
}

char* shell_read_line()
{
  int buf_size = SHELL_BUFFER_SIZE;
  int position = 0;
  char* buffer = (char*)malloc(sizeof(char) * buf_size);
  check_memory_allocation(buffer);
  while(1)
  {
    int c = getchar();
    if(c == EOF || (char)c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    buffer[position] = c;
    position++;
    if(position == buf_size)
    {
      buf_size += SHELL_BUFFER_SIZE;
      buffer = realloc(buffer, sizeof(char) * buf_size);
      check_memory_allocation(buffer);
    }
  }
  return NULL;
}

char** shell_split_line(char* cmd)
{
  size_t buffer_size = SHELL_TOKENS_SIZE;
  char** args = (char**)malloc(sizeof(char*) * buffer_size);
  check_memory_allocation(args);
  char* token = strtok(cmd, TOKEN_DELIM);
  int position = 0;
  while(token != NULL)
  {
    args[position] = token;
    position++;

    if(position == buffer_size)
    {
      buffer_size += SHELL_TOKENS_SIZE;
      args = realloc(args, buffer_size);
      check_memory_allocation(args);
    }
    token = strtok(NULL, TOKEN_DELIM);
  }
  args[position] = NULL;
  return args;
}

int execute(char** args)
{
  int pid = fork();
  if(pid == 0)
  {
    if(execvp(args[0], args) == -1)
      perror("error executing command");
  }
  else if(pid > 0)
  {
    int status; 
    do
    {
      int _ = waitpid(pid, &status, WUNTRACED);
    }
    while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  else
  {
    perror("exit forking the parent process");
    exit(EXIT_FAILURE);
  }
  return 0;
}

void shell_loop()
{
  int status = 0;
  while(status == 0)
  {
    printf(">");
    char* line = shell_read_line();
    char** args = shell_split_line(line);
    status = execute(args);
    free(line);
    free(args); 
  }
}

int main(int argc, char **argv)
{
  shell_loop();
  return EXIT_SUCCESS;
}
