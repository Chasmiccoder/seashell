# seashell 🐚

A simple shell written in C :)

### Features
* Name and System displayed in the shell prompt
* Color coded output :D
* Supports foreground and background processes
* Supports execution of multiple commands in one prompt
* Command history is stored over sessions

### Custom Commands Supported
* `cd [paths]`
* `echo [strings]`
* `pwd`
* `ls [flags -l -a -la -al] [paths]`
* `discover [target directory] [flags -d -f] [target file]`
* `history [number of items]`

* `pinfo ???`

Other commands are supported through their in-built implementation (not custom made).


### seashell implementation

Commands that can be executed have been implemented in separate `c` files in the folder `~/commands`.
Here, `~/` is the home directory, which has been set to the location of the executable.

Flow control starts in the file `seashell.c`. The structure `ShellVariables` is used to maintain important information that gets carried over the entire project. It is not a global var (global variables have been avoided where ever possible).

The input statement is first tokenized to give its constituent commands. Then, the commands are processed one by one by their separate implementations in `~/commands`.

The file `globals.h` contains some macros that are used throughout the project.
The file `shell_manipulation.c` contains functions related to the frontend.
The file `utils.c` contains some utility functions.



