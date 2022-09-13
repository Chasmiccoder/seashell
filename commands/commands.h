void run_exit(const char *args);

void run_echo(char *args);

void run_cd(const char *args, struct ShellVariables *sv);

void run_pwd(const char *args, const struct ShellVariables *sv);

void run_ls(const char *args, const struct ShellVariables *sv);

void run_system_command(char *command, struct ShellVariables *sv);

void run_history(const struct ShellVariables *sv);

void add_command_to_history(struct ShellVariables *sv, const char *command);

void update_history_db(struct ShellVariables *sv);

void fetch_history_db(struct ShellVariables *sv);

void run_discover(struct ShellVariables *sv);

void run_pinfo();
