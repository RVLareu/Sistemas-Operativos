#include "builtin.h"

// returns true if the 'exit' call
// should be performed
//
// (It must not be called from here)
int
exit_shell(char *cmd)
{
	// Your code here
	if (strstr(cmd, "exit") != NULL) {
		return 1;
	}

	return 0;
}

// returns true if "chdir" was performed
//  this means that if 'cmd' contains:
// 	1. $ cd directory (change to 'directory')
// 	2. $ cd (change to $HOME)
//  it has to be executed and then return true
//
//  Remember to update the 'prompt' with the
//  	new directory.
//
// Examples:
//  1. cmd = ['c','d', ' ', '/', 'b', 'i', 'n', '\0']
//  2. cmd = ['c','d', '\0']
int
cd(char *cmd)
{
	// Your code here

	if (strlen(cmd) == 2 && strstr("cd", cmd) != NULL) {
		if (chdir(getenv("HOME")) == 0) {
			return 1;
		} else {
			return 0;
		}
	} else if (cmd[0] == 'c' && cmd[1] == 'd') {
		char *dir = &cmd[3];
		if (chdir(dir) == 0) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}

// returns true if 'pwd' was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
pwd(char *cmd)
{
	if (strstr("pwd", cmd) != NULL) {
		printf_debug("%s\n",get_current_dir_name());
		return true;
	}

	return 0;
}
