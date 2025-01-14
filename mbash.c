#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#define MAXLI 2048

//déclaration des variables
char cmd[MAXLI];
char path[MAXLI];
char rep[MAXLI];
char* motsCles[] = {"exit\n", "quitter\n", "pwd\n"};
int rester = 1;
int pathidx;

//déclaration des fonctions
void mbash();
void quitter();
int estMotCle(char* mot);
char* getRepertoireCourant();
void lancerCommandeListe(char* mot);



int main(int argc, char** argv) {
  while (rester) {
    printf("\n");
    printf("%s", getRepertoireCourant());
    printf(" : ");
    fgets(cmd, MAXLI, stdin);

    if(estMotCle(cmd)){
      lancerCommandeListe(cmd);
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
    }
    return rep;
}

//méthode qui permet de savoir si une commande entrée fait partie de la liste des mots clefs (motsClefs) ou non
int estMotCle(char* mot){
  int size = sizeof(motsCles)/sizeof(char*);
  for(int i = 0; i < size; i++){
    if(strcmp(mot, motsCles[i]) == 0){
      return 1;
    }
  }
  return 0;
}

//méthode qui permet de lancer une commande de la liste de commandes
void lancerCommandeListe(char* mot) {
    if ((strcmp(mot,"quitter\n") == 0)||(strcmp(mot,"exit\n")) == 0) {
        quitter();
    }
    else if (strcmp(mot,"pwd\n") == 0) {
        printf("%s", getRepertoireCourant());
    }
}