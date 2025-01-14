#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#define MAXLI 2048
char cmd[MAXLI];
char path[MAXLI];
int pathidx;
void mbash();
void quitter();
char getRepertoireCourant();
char rep[MAXLI];
int rester = 1;

int main(int argc, char** argv) {
  while (rester) {
    printf("Commande: ");
    fgets(cmd, MAXLI, stdin);
    mbash(cmd);

    if (strcmp(cmd,"exit\n")||strcmp(cmd,"quitter\n")) {
        quitter();
    }

  }
  return 0;
}

void mbash() {
  printf("Execute: %s", cmd);
  system(cmd);
}

void quitter() {
    rester = 0;
}

char getRepertoireCourant() {
    getcwd(rep, MAXLI);
    println(rep);
    return rep;
}