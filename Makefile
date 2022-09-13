# --- Makefile for seashell ---
build: seashell.c utils.c shell_manipulation.c commands/cd.c commands/echo.c commands/pwd.c commands/ls.c commands/system_commands.c commands/history.c datastructures.c commands/discover.c commands/pinfo.c commands/exit.c
	@echo "Building seashell"
	gcc -o seashell seashell.c utils.c shell_manipulation.c commands/cd.c commands/echo.c commands/pwd.c commands/ls.c commands/system_commands.c commands/history.c datastructures.c commands/discover.c commands/pinfo.c commands/exit.c

clean:
	@echo "Cleaning previous build"
	rm seashell
