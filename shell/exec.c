#include "exec.h"

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void
set_environ_vars(char **eargv, int eargc)
{
	// Your code here
	for (int i = 0; i < eargc; i ++) {
	
		char * key;
		get_environ_key(eargv[i], key);
		char * value;
		get_environ_value(eargv[i], value, block_contains(eargv[i], '='));
	
		setenv(key, value, 1);
		
		}
}	

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int
open_redir_fd(char *file, int flags)
{
	// Your code here
	int fd = open(file, flags, 0644);
	return fd;

	return -1;
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option
void
exec_cmd(struct cmd *cmd)
{
	// To be used in the different cases
	struct execcmd *e;
	struct backcmd *b;
	struct execcmd *r;
	struct pipecmd *p;

	switch (cmd->type) {
		case EXEC:

			e =  (struct execcmd *) cmd;
			set_environ_vars(e->eargv, e->eargc);
			execvp(e->argv[0], e->argv);
			_exit(-1);
			break;

		case BACK: {

			b = (struct backcmd *) cmd;
			e = (struct execcmd *) b->c;
			execvp(e->argv[0], e->argv);
			_exit(-1);
			break;
		}

		case REDIR: {

			r = (struct execcmd *) cmd;
			int fd_out;
			int fd_in;
			int fd_err;
			if (strlen(r->out_file) > 0) {
				fd_out = open_redir_fd(r->out_file, O_WRONLY | O_CREAT | S_IWUSR | S_IRUSR);
				dup2(fd_out, 1);
				close(fd_out);
				
			} if (strlen(r->in_file) > 0) {
				fd_in = open_redir_fd(r->in_file, O_RDONLY);
				dup2(fd_in, 0);
				close(fd_in);
				
			} if (strlen(r->err_file) > 0) {
				if (strstr("&1", r->err_file) != NULL) {
					dup2(1,2);
				} else {
					fd_err = open_redir_fd(r->err_file, O_WRONLY | O_CREAT | S_IWUSR | S_IRUSR);
					dup2(fd_err, 2);
					close(fd_err);
				}
				
			}
			if (fd_out < 0 && fd_in < 0 && fd_err < 0) { //todos abrieron mal
				_exit(-1);
			}
			execvp(r->argv[0], r->argv);
			
			break;
		}

		case PIPE: {

			p = (struct pipecmd *) cmd;
			struct execcmd *p_right = (struct execcmd *) p->rightcmd;
			struct execcmd *p_left = (struct execcmd *) p->leftcmd;
			int fds[2];
			int err = pipe(fds);
			if (err < 0) {
				_exit(-1);
			}
			

			int pid_izq = fork();

			if (pid_izq == 0) {
				close(fds[1]);
				dup2(fds[0],0);
				close(fds[0]);
				execvp(p_right->argv[0], p_right->argv);
			} 
			
			int pid_der = fork();
			if (pid_der == 0) {
				close(fds[0]);
				dup2(fds[1],1);
				close(fds[1]);
				execvp(p_left->argv[0], p_left->argv);
			}
			free_command(parsed_pipe);
			close(fds[0]);
			close(fds[1]);
			waitpid(pid_der, NULL, 0);
			waitpid(pid_izq, NULL, 0);
			exit(0);

			

			// free the memory allocated
			// for the pipe tree structure
			
		
			break;
		}
	}
}
