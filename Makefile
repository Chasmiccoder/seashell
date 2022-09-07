# --- Makefile for seashell ---
build: seashell.c utils.c shell_manipulation.c commands/cd.c commands/clear.c commands/echo.c commands/exit.c commands/pwd.c commands/ls.c commands/system_commands.c commands/history.c datastructures.c commands/discover.c
	@echo "Building seashell"
	gcc -o seashell seashell.c utils.c shell_manipulation.c commands/cd.c commands/clear.c commands/echo.c commands/exit.c commands/pwd.c commands/ls.c commands/system_commands.c commands/history.c datastructures.c commands/discover.c

clean:
	@echo "Cleaning previous build"
	rm seashell
