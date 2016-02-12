/**
 * @file dsh.h
 * @author Yanlin Li
 * @date 12, Feb 2016
 * @This program will introduce unix environment, system calls, signals, 
 * the proc filesystem and the fork / exec system calls. 
 * It should present a prompt with a prompt string:  dsh> and accept commands
 * to be executed at the prompt.    This program will be the background for  
 * several programs to follow.  
 * @@.
 * @see 
 * @see 
 */
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#define dsh_TOK_BUFSIZE 64
#define dsh_TOK_DELIM " \t\r\n\a"
#define dsh_RL_BUFSIZE 1024

/**
 * @list of command that can be run
 *
 * 
 */
char *cmd_str[] = 
{
  "cd",
  "exit",
  "cmdnm",
  "systat", 
  "signal",
  "pwd"
  
};


/**
 * @Functioncalls
 *
 * 
 */
int dsh_cd(char **args);
int dsh_exit(char **args);
int dsh_cmdnm(char **args);
int dsh_signal(char **args);
int dsh_systat(char **args);
int dsh_pwd(char **args);


/**
 * @Functioncalls
 *
 */
int (*cmd_func[]) (char **) = 
{
  &dsh_cd,
  &dsh_exit,
  &dsh_cmdnm,
  &dsh_systat,
  &dsh_signal,
  &dsh_pwd
};


/**
 * @Functioncalls
 *
 * 
 */
int num_cmd() 
{
  return sizeof(cmd_str) / sizeof(char *);
}

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int dsh_cd(char **args)
{
  if (args[1] == NULL) 
  {
    fprintf(stderr, "dsh: expected argument to \"cd\"\n");
  } 
  else 
  {
    if (chdir(args[1]) != 0) 
    {
      perror("dsh");
    }
  }
  return 1;
}


int dsh_systat(char **args)
{ 
  FILE *fp;
  char cwd[1024];
  long numbytes;
  char *buffer;
  char *memtotal;
  char *memfree;
  

  getcwd(cwd, sizeof(cwd));

  chdir("/proc");
  fp = fopen("version", "r");

  //read the file and print out result
  printf("linux-version: \n");
  buffer = (char*)calloc(numbytes, sizeof(char));
  fread(buffer, sizeof(char), numbytes, fp);
  printf("%s\n\n\n", buffer);
  fclose(fp);

  printf("Uptime: \n");
  memset(buffer, 0, sizeof buffer);
  fp = fopen("uptime", "r");
  buffer = (char*)calloc(numbytes, sizeof(char));
  fread(buffer, sizeof(char), numbytes, fp);
  printf("%s\n", buffer);
  fclose(fp);

  printf("memory information: \n");
  memset(buffer, 0, sizeof buffer);
  fp = fopen("meminfo", "r");
  fgets(buffer, 256, fp);
  printf("%s", buffer);

  memset(buffer, 0, sizeof buffer);
  fgets(buffer, 256, fp);
  printf("%s\n", buffer);
  fclose(fp);

  printf("CPU information: \n");
  memset(buffer, 0, sizeof buffer);
  fp = fopen("cpuinfo", "r");
  fgets(buffer, 256, fp);
  memset(buffer, 0, sizeof buffer);
  fgets(buffer, 256, fp);
  fgets(buffer, 256, fp);
  fgets(buffer, 256, fp);
  fgets(buffer, 256, fp);
  fgets(buffer, 256, fp);
  fgets(buffer, 256, fp);
  fgets(buffer, 256, fp);
  memset(buffer, 0, sizeof buffer);
  fgets(buffer, 256, fp);
  printf("%s\n", buffer);

  fclose(fp);



  //change back to original directory.
  chdir(cwd);
  
  return 1;
}

int dsh_cmdnm (char **args)
{
  char cwd[1024];
  char buff[255];
  FILE *fp;
  char temp;

  //if there is not enough arguement
  if (args[1] == NULL) 
  {
    fprintf(stderr, "dsh: expected argument to \" cmdnm\" <pid> \n");
  } 
  else
  {
  	getcwd(cwd, sizeof(cwd));

  	chdir("/proc");
 	chdir(args[1]);

 	//if the file does not exist
 	if ((fp = fopen("comm", "r")) == NULL)
 	{
 		chdir(cwd);
 		printf("Pid is does not exsit\n");
        return 1;
 	}

  	//read the file and print out result
  	fscanf(fp, "%s", buff);
  	printf("%s\n", buff);

  	//change back to original directory.
  	chdir(cwd);
  	fclose(fp);
  }
  return 1;
}

int dsh_signal(char **args)
{
  if (args[1] == NULL && args[2] == NULL) 
  {
    fprintf(stderr, "dsh: expected argument to \"signal\"<signal_num> <pid>\n");
  } 
  else 
  {
  	kill(atoi(args[2]), atoi(args[1]));
  	printf("Killed process %s\n", args[2]);
  }
  return 1;
}

int dsh_pwd(char **args)
{
  char cwd[1024];
  printf("pwd\n");
  getcwd(cwd, sizeof(cwd));
  printf("current working dir: %s\n", cwd);
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int dsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int dsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) 
  {
    // Child process
    if (execvp(args[0], args) == -1) 
    {
      perror("dsh");
    }
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0) 
  {
    // Error forking
    perror("dsh");

  } 
  else 
  {

    struct rusage usage;

    wait3( &status, 0, &usage);
    //Parent process

    printf("Pid is %d\n", pid);
    printf("System time: %d seconds, %d microseconds\n", 
    	(int)usage.ru_utime.tv_sec, (int) usage.ru_utime.tv_usec );
    printf("User time: %d seconds, %d microseconds\n", 
    	(int)usage.ru_stime.tv_sec, (int) usage.ru_stime.tv_usec);
    printf("page fault: %d\n", (int) usage.ru_minflt );
    printf("page swaps: %d\n", (int) usage.ru_majflt);
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int dsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) 
  {
    // An empty command was entered.
    return 1;
  }

  //compare command with string
  for (i = 0; i < num_cmd(); i++) 
  {
    if (strcmp(args[0], cmd_str[i]) == 0) 
    {
      return (*cmd_func[i])(args);
    }
  }

  return dsh_launch(args);
}


/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *dsh_read_line(void)
{
  int bufsize = dsh_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) 
  {
    fprintf(stderr, "dsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) 
  {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') 
    {
      buffer[position] = '\0';
      return buffer;
    } 
    else 
    {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) 
    {
      bufsize += dsh_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) 
      {
        fprintf(stderr, "dsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}


/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **dsh_split_line(char *line)
{
  int bufsize = dsh_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) 
  {
    fprintf(stderr, "dsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, dsh_TOK_DELIM);
  while (token != NULL) 
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize) 
    {

      bufsize += dsh_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      
      if (!tokens) 
      {
		free(tokens_backup);
        fprintf(stderr, "dsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, dsh_TOK_DELIM);
  }


  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void dsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do 
  {
    printf("dsh> ");
    line = dsh_read_line();
    args = dsh_split_line(line);
    status = dsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  dsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

