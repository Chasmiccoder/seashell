void run_exit();
void run_echo();
void run_cd(struct ShellVariables *sv);
void run_clear();
void run_pwd(const struct ShellVariables *sv);
void run_ls(const struct ShellVariables *sv);
void run_system_command(char *command, struct ShellVariables *sv);
void run_history(const struct ShellVariables *sv);
void add_command_to_history(struct ShellVariables *sv, const char *command);
void update_history_db(struct ShellVariables *sv);
void fetch_history_db(struct ShellVariables *sv);
void run_discover(struct ShellVariables *sv);