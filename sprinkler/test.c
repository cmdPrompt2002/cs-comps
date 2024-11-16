#include<stdio.h>
#include<regex.h>
#include<string.h>
#include<stdlib.h>

union testy {
    char *success;
    char *fail;
    int i;
    int j;
};

int main() {

    union testy var1;
    var1.fail = "abc";

    printf("%s\n",var1.success);
    /*Shows that rm.eo is the index AFTER the last one in the regex*/
    // regex_t regex;
    // regmatch_t match[2];
    // int contentLength;
    // char *contentLengthString = "Content-Length: ([^\r\n]*)\r\n.*\r\n\r\n"; 
    // char *fullResponse = malloc(100);
    // strcpy(fullResponse, "Content-Length: 135\r\nHeader: abc\r\n\r\nbodybodybody\r\n\r\nbody2");
    // int fullResponseSize = strlen(fullResponse) + 1;



    // if (regcomp(&regex, contentLengthString ,REG_EXTENDED) != 0) {
    //     fprintf(stderr, "Could not compile contentLength regex\n");
    //     exit(1);
    // }

    // int regexResult = regexec(&regex, fullResponse, 2, match, 0);

    // if (regexResult == 0) {

    //     fullResponse[match[1].rm_eo +1] = '\0';

    //     contentLength = atoi(fullResponse + match[1].rm_so);
   
    //     fullResponse[match[1].rm_eo +1] = '\n';
    //     printf("OK\n");
            
    // } else if (regexResult == REG_NOMATCH) {
    //     contentLength = -1;
    // } else {
    //     fprintf(stderr,"Regexec of contentLengthString failed\n");
    //     exit(1);
    // }
   
    // printf("%s\n",fullResponse +  match[0].rm_eo);
    // printf("%s\n",strstr(fullResponse, "\r\n\r\n"));
    // int numContentBytesRead = strstr(fullResponse, "\r\n\r\n") + 3 - fullResponse + 1;
    // printf("%i, %i\n", numContentBytesRead, fullResponseSize);
    // free(fullResponse);

    // char *testString = "cat";
    // printf("%lu\n", strlen(testString));
    return 0;
}

