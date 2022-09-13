#ifndef seashell_utils_h_
#define seashell_utils_h_

#define COLOR_WHITE 0
#define COLOR_BLUE  1
#define COLOR_GREEN 2

void clear_string(char *str);
int is_substring(const char *substring, const char *string);

void trim_string(char *dest, const char *source);

void format_string(char *dest, const char *source);
void add_color_to_string(char *string, int color_code);
void string_to_lower(char *string);
void string_swap(char *str1, char *str2);
void int_swap(int *num1, int *num2);
int handle_system_call(int status, char *system_call);
void prepend_string(const char *prefix, char *string);

#endif
