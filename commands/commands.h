void run_exit();
void run_echo();
void run_cd(struct ShellVariables *sv);
void run_clear();
void run_pwd(const struct ShellVariables *sv);
void run_ls(const struct ShellVariables *sv);
void run_system_command(char *command, struct ShellVariables *sv);
void run_history(const struct ShellVariables *sv);