int main() {



    char *tmp = malloc(100*sizeof(char));
    strcpy(tmp, "\"Hello   world\"");
    // removeUnnecessarySpaces(tmp);
    char *tmp2 = strtok(tmp, "");
    printf("%s\n", tmp2);


    char ch1[] = "hello\0";
    char ch2[] = "hello\0";
    printf("\n%d\n", is_substring(ch1, ch2));




}