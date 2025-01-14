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
boolean rester = true;

int main(int argc, char** argv) {
  while (rester) {
    printf("Commande: ");
    fgets(cmd, MAXLI, stdin);
    mbash(cmd);

    if ((cmd == "exit")||(cmd == "quitter")) {
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
    rester = false;
}