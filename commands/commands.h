void run_exit(const char *args);

void run_echo(char *args);

void run_cd(const char *args);

void run_pwd(const char *args);

void run_ls(const char *args);

void run_system_command(char *command, const char *args);

void run_history(const char *args);

void add_command_to_history(const char *command);

void update_history_db();

void fetch_history_db();

void run_discover();

void run_pinfo();

void run_jobs(char *args);

void run_sig(char *args);