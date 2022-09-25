# --- Makefile for seashell ---
build: globals.c seashell.c utils.c shell_manipulation.c commands/cd.c commands/echo.c commands/pwd.c commands/ls.c commands/system_commands.c commands/history.c datastructures.c commands/discover.c commands/pinfo.c commands/exit.c commands/jobs.c commands/sig.c
	@echo "Building seashell"
	gcc -o seashell globals.c seashell.c utils.c shell_manipulation.c commands/cd.c commands/echo.c commands/pwd.c commands/ls.c commands/system_commands.c commands/history.c datastructures.c commands/discover.c commands/pinfo.c commands/exit.c commands/jobs.c commands/sig.c

clean:
	@echo "Cleaning previous build"
	rm seashell
