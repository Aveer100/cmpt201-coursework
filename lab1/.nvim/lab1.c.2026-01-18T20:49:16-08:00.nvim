#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  for (int i = 0; i < 20; i++) {
    char *myLine = NULL;
    char *storage = NULL;
    size_t length = 0;

    printf("Please enter some text: ");
    getline(&myLine, &length, stdin);
    printf("Tokens:\n");

    char *current = strtok_r(myLine, " ", &storage);
    while (current != NULL) {
      printf("%s\n", current);
      current = strtok_r(NULL, " ", &storage);
    }
    free(myLine);
  }
  return 0;
}
