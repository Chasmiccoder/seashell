int main() {



    char *tmp = malloc(100*sizeof(char));
    strcpy(tmp, "\"Hello   world\"");
    // removeUnnecessarySpaces(tmp);
    char *tmp2 = strtok(tmp, "");
    printf("%s\n", tmp2);




}