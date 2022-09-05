# seashell 🐚
A simple shell written in C :)


## Features
* exit  
* echo (handling quotes as well)  
* clear  
* pwd  
* cd (with flags like '.' '..' './' '/' '~/')  





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