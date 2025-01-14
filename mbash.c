#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#define MAXLI 2048
char cmd[MAXLI];
char path[MAXLI];
char rep[MAXLI];
char* motsCles[] = {"exit\n", "quitter\n", "pwd\n"};
int rester = 1;
int pathidx;
void mbash();
void quitter();
int estMotCle(char* mot);
char* getRepertoireCourant();

int main(int argc, char** argv) {
  while (rester) {
    printf("Commande: ");
    fgets(cmd, MAXLI, stdin);

    if(estMotCle(cmd)){
      if (strcmp(cmd,"exit\n") == 0||strcmp(cmd,"quitter\n") == 0) {
        quitter();
      }

      if (strcmp(cmd,"pwd\n") == 0) {
        getRepertoireCourant();
      }      
    } else {
      printf("truc\n");
      mbash(cmd);
    }
  }
  return 0;
}

void mbash() {
  printf("Execute: %s", cmd);
  system(cmd);
}

//méthode similaire à ^C
void quitter() {
    rester = 0;
}

//méthode similaire à pwd
char* getRepertoireCourant() {
    if (strlen(rep) == 0) {
        getcwd(rep, MAXLI);
        printf("%s",rep);
    }
    return rep;
}

int estMotCle(char* mot){
  int size = sizeof(motsCles)/sizeof(char*);
  for(int i = 0; i < size; i++){
    if(strcmp(mot, motsCles[i]) == 0){
      return 1;
    }
  }
  return 0;
}