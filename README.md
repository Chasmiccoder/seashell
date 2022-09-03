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
