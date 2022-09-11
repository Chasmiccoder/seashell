# seashell 🐚
A simple shell written in C :)


## Features
* exit  
* echo (handling quotes as well)  
* clear  
* pwd  
* cd (which supports '.' '..' './' '/' '~/')  





### Dev Corner -
Keep in mind:  
make sure to handle all TODOs within the project  
make sure to deallocate dynamically allocated memory  
make sure to fully tokenize a string before moving on to another segment (when using strtok) [or save it to a buffer]  

Bugs/Features
handle tabs (echo) [remove all tabs]
echo echo not working
echo "hello   world" asklas asas fasf - not working
If someone gives ~/dir, it should take it as ~ (home) as the location of the shell

For ls `ls world dir -l -l -a -la folder1` not working


cd ~/dir is not working

check const-argument consistency


The specifications say - "You do not have to handle background processing for built-in commands (ls, echo, cd, pwd, pinfo). Commands not implemented by you should be runnable in the background."
In the future, handle this propery. Custom commands will be used normally, and when & is involved, use system() for it.


mkdir works for upto 2 args, but not more
Error is around `char **arguments = malloc(MAX_NUMBER_OF_ARGS * sizeof(char*));` in system_commands.c
Error is regardingnumber of chars in total

`mkdir q1 q2 q3 q4 q5 q6 q7 q8 q9 q10` doesn't work
`mkdir o1 o2 o3 o4 o5` works

Not sure if this is a bug or feature, but if gedit is already open, then running `gedit` will not run it as a foreground process


Print `took 3 seconds` in prompt (foreground thing)
& getting passed as arg to gedit

Test multiple statements with &

Need to handle ~/dir in ls and cd

If gedit & and then close gedit and then sleep 2, then the foreground process does not happen

REDO specification 4

Create a path handler that takes care of relative home ~


We won't need so many malloc statements. Since we're using macros

Need to free shell variables (sv) properly.
Write custom function in shell_manip.c

NEED to do error handling with perror

The user can type in any command, including, ./a.out, which starts a new process out of your shell program. (Testing is left)

We can reduce redundancy/modularize more. E.g. init_ls_flag_bitmap and init_discover_flag_bitmap do the same thing.

create a function that processes the paths before feeding them directly. This is to account for using ~/ as the shell directory. resolve_path

study links

handle memory wastage in ls and discover

discover '~' not working

ifndef in all header files

Add colours to discover outpu

check if & is present elsewhere like, sleep 2&

ls after cd not working :(




raw mode for tab autocomplete

### Potential future bugs
Multiple commands with ; might get messed up because of same var names