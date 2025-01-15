#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#define MAXLI 2048
#define S_FINI 1000

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
void automateCd(char* commande);
void commandeCd();
void stringSlicer(char* string, char* result[MAXLI]);
void stringSlicerPutIntoList(char* mot, char* liste[MAXLI], int index);

int main(int argc, char** argv) {
  printf("##################################################\n");
  printf("#                                                #\n");
  printf("#   M     M   BBBBB    AAAAA   SSSSS   H   H     #\n");
  printf("#   MM   MM   B    B  A     A  S       H   H     #\n");
  printf("#   M M M M   BBBBB   AAAAAAA  SSSSS   HHHHH     #\n");
  printf("#   M  M  M   B    B  A     A      S   H   H      #\n");
  printf("#   M     M   BBBBB   A     A  SSSSS   H   H     #\n");
  printf("#                                                #\n");
  printf("##################################################\n");
  while (rester) {
    printf("\n");
    printf("%s", getRepertoireCourant());
    printf(" : ");
    fgets(cmd, MAXLI, stdin);

    if(estMotCle(cmd)){
      lancerCommandeListe(cmd);
    } else {
      printf("truc\n");
      automateCd(cmd);
      //mbash(cmd);
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
        getcwd(rep, 200);
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

void commandeCd(char* commande) {
    mbash(commande);
    //getRepertoireCourant();
    getcwd(rep, MAXLI);
}

void automateCd(char* commande) {
    #define S_DEPART 0
    #define S_UN_SLASH 1
    #define S_UNE_VAGUE 2
    #define S_UN_POINT 3
    #define S_DEUX_POINTS 4
    #define S_DEUX_POINT_UN_SLASH 5
    #define S_C 6
    #define S_CD 7
    #define S_CD_ESPACE 8
    #define S_CD_DES_ESPACES 9

    #define S_ERREUR 1001

    int state = S_DEPART;
    int i = 0;
    int compteurRetour = 0;
    char caractereCourant;

      while ( state < S_FINI ) {
          caractereCourant = commande[i];
          i = i + 1;
        switch (state) {
            case S_DEPART: // ------------------------------------------------
              switch (caractereCourant) {
                  case 'c':
                    //printf("cas: c\n");
                    state = S_C;
                    break;
                  default:
                    state = S_ERREUR;
              }
              break;

            case S_C: // ------------------------------------------------
                switch (caractereCourant) {
                    case 'd': {
                        //printf("cas: cd\n");
                        state = S_CD;
                        break;
                    }
                    default :
                        state = S_ERREUR;
                        break;
                }
                break;

            case S_CD: // ------------------------------------------------
                switch (caractereCourant) {
                    case ' ':
                        //printf("cas: cd espace\n");
                        state = S_CD_ESPACE;
                        break;
                    case '\n':
                        //printf("commande cd");
                         //code pour "cd"
                         commandeCd(cmd);
                         state = S_FINI;
                         break;
                    default :
                        state = S_ERREUR;
                        break;
                }
                break;

            case S_CD_ESPACE: // ------------------------------------------------
                switch (caractereCourant) {
                    case '~':
                        //printf("cas: cd ~\n");
                        state = S_UNE_VAGUE;
                        break;
                    case '.':
                        //printf("cas: cd .\n");
                        state = S_UN_POINT;
                        break;
                    case '/':
                        //printf("cas: cd /\n");
                        state = S_UN_SLASH;
                        break;
                    case ' ':
                        //printf("cas: cd espaces\n");
                        state = S_CD_DES_ESPACES;
                        break;
                    default :
                        state = S_ERREUR;
                        break;
                }
                break;

            case S_CD_DES_ESPACES: // ------------------------------------------------
                switch (caractereCourant) {
                    case ' ':
                        //printf("cas: cd espaces\n");
                        state = S_CD_DES_ESPACES;
                        break;
                    case '\n':
                        //printf("commande cd");
                        //commande "cd"
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
                    default:
                        state = S_ERREUR;
                        break;
                }
                break;

            case S_UNE_VAGUE: // ------------------------------------------------
                switch (caractereCourant) {
                    case '\n':
                        //printf("commande cd ~\n");
                        //commande "cd ~"
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
                    default :
                        //printf("commande cd ~nom\n");
                        //on récupère la fin de la commande qui est le nom après la vague
                        //commande "cd ~nom"
                        //erreur ou non
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
                }
                break;
            case S_UN_POINT: // ------------------------------------------------
                switch (caractereCourant) {
                    case '.':
                        //printf("cas: cd ..\n");
                        state = S_DEUX_POINTS;
                        break;
                    default:
                        state = S_ERREUR;
                        break;
                }
                break;
            case S_DEUX_POINTS: // ------------------------------------------------
                switch (caractereCourant) {
                    case '\n':
                        //printf("commande cd ..\n");
                        //commande "cd .."
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
                    case '/':
                        //printf("cas: cd ../\n");
                        state = S_DEUX_POINT_UN_SLASH;
                        break;
                    default :
                        state = S_ERREUR;
                        break;
                }
                break;
            case S_DEUX_POINT_UN_SLASH: // ------------------------------------------------
                switch (caractereCourant) {
                    case '.':
                        //printf("cas: cd ../.\n");
                        compteurRetour ++;
                        state = S_UN_POINT;
                        break;
                    default:
                        //printf("commande cd ../nom\n");
                        //on récupère la fin de la commande qui est le nom
                        //commande "cd ../nom"
                        //erreur ou non
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
                }
                break;

            case S_UN_SLASH: // ------------------------------------------------
                switch (caractereCourant) {
                    case '\n':
                        //printf("commande cd /\n");
                        //commande "cd /"
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
//                    case ' ':
//                        state = S_SLASH_DES_ESPACES;
//                        break;
                    default :
                        //printf("commande cd/chemin\n");
                        //on récupère la fin de la commande qui est le chemin
                        //commande "cd /chemin"
                        commandeCd(cmd);
                        state = S_FINI;
                        break;
                }
                break;
        }
      }
}

void stringSlicer(char* string, char* result[MAXLI]){
    //taille max d'un mot
    #define MEMOT 128

    #define S_DEPART 0
    #define S_MOT 1
    #define S_APRES_SIMPLE_COTE 2
    #define S_APRES_DOUBLE_COTE 3
    #define S_ESPACE 4

    char caractereCourant;
    int nbResultats = 0;
    char* mot = calloc(MEMOT, 1);
    int state = 0;
    int tete = 0;

    while(state < S_FINI){
        caractereCourant = string[tete];

        switch(state){
            case S_DEPART :
                switch(caractereCourant){
                    case ' ' :
                        state = S_ESPACE;
                        break;
                    case '\0':
                        state = S_FINI;
                        break;
                    case '\'':
                        state = S_APRES_SIMPLE_COTE;
                        break;
                    case '\"':
                        state = S_APRES_DOUBLE_COTE;
                        break;
                    default:
                        strncat(mot,&string[tete], 1);
                        state = S_MOT;
                        break;
                }
            break;

            case S_MOT :
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case ' ':
                        state = S_ESPACE;
                        break;
                    case '\0':
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\'':
                        state = S_APRES_SIMPLE_COTE;
                        break;
                    case '\"':
                        state = S_APRES_DOUBLE_COTE;
                        break;
                    default:
                    break;
                }
            break;

            case S_APRES_SIMPLE_COTE:
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case '\0':
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\'':
                        stringSlicerPutIntoList(mot, result, nbResultats);
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_APRES_DOUBLE_COTE:
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case '\0':
                        stringSlicerPutIntoList(mot, result, nbResultats);
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\"':
                        stringSlicerPutIntoList(mot, result, nbResultats);
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_ESPACE :
                if(strlen(mot) != 0){
                    mot[strlen(mot)-1] = '\0';
                    stringSlicerPutIntoList(mot, result, nbResultats);
                    nbResultats++;
                }
                switch(caractereCourant){
                    case ' ':
                        state = S_ESPACE;
                        break;
                    case '\'':
                        strncat(mot,&string[tete], 1);
                        state = S_APRES_SIMPLE_COTE;
                        break;
                    case '\"':
                        strncat(mot,&string[tete], 1);
                        state = S_APRES_DOUBLE_COTE;
                        break;
                    case '\0':
                        state = S_FINI;
                        break;
                    default:
                        strncat(mot,&string[tete], 1);
                        state = S_MOT;
                        break;

                }

            break;

        }
        tete++;
    }
}

void stringSlicerPutIntoList(char* mot, char* liste[MAXLI], int index){
    liste[index] = calloc(strlen(mot), 1);
    strcpy(liste[index], mot);
    free(mot);
    mot = calloc(MEMOT, 1);
}