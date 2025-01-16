#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLI 2048
#define MAXNBSTR 64
#define S_FINI 1000

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//déclaration des variables
char cmd[MAXNBSTR];
char path[MAXLI];
char rep[MAXLI];
char* couleur = ANSI_COLOR_RESET;
int rester = 1;
int pathidx;
char* commande[MAXNBSTR] = {};

//déclaration des fonctions
void mbash();
void quitter();
char* getRepertoireCourant();
void lancerCommandeListe(char* commande[MAXNBSTR]);
void automateCd(char* commande);
void commandeCd();
void stringSlicer(char* string, char* result[MAXNBSTR], char caractereSeparateur);
void stringSlicerPutIntoList(char* mot, char* liste[MAXNBSTR], int index);
void ecrire(char* commande);
void mainPere();
void mainFils();
void changeColor(char* mot);

int main(int argc, char** argv) {
  printf("#######################################################\n");
  printf("#                                                     #\n");
  printf("#   |M     |M   |BBBBB    |AAAAA   |SSSSS   |H  |H    #\n");
  printf("#   |MM   |MM   |B   |B  |A    |A  |S       |H  |H    #\n");
  printf("#   |M|M |M|M   |BBBBB   |AAAAAAA  |SSSSS   |HHHHH    #\n");
  printf("#   |M  |M |M   |B   |B  |A    |A      |S   |H  |H    #\n");
  printf("#   |M     |M   |BBBBB   |A    |A  |SSSSS   |H  |H    #\n");
  printf("#                                                     #\n");
  printf("#######################################################\n");

  while (rester) {
    printf("%s", getRepertoireCourant());
    printf(" : ");
    fgets(cmd, MAXLI, stdin);
    cmd[strlen(cmd)-1]= '\0';
    stringSlicer(cmd, commande, ';');
    int i = 0;
    while (commande[i] != NULL) {
        printf("|%s|", commande[i]);
        i++;
    }
    printf("\n");
    //automateCd(cmd);
    lancerCommandeListe(commande);
    memset(commande, 0, sizeof commande);
  }
  return 0;
}


void mbash(char* com) {
  printf("Execute: %s\n", com);
  system(com);
}

//méthode similaire à ^C
void quitter() {
    rester = 0;
    printf(ANSI_COLOR_RESET"");
}

//méthode similaire à pwd
char* getRepertoireCourant() {
    getcwd(rep, 200);
    return rep;
}

//méthode qui permet de lancer une commande de la liste de commandes
void lancerCommandeListe(char* commande[MAXNBSTR]) {
    int i = 0;
    while(commande[i] != NULL){
        //printf("nb iteration : %d\n",i+1);
        //printf("commande i : %s\n",commande[i]);
        char* mot = commande[i];
        char* listeAvecEspaces[MAXNBSTR] = {};
        stringSlicer(mot, listeAvecEspaces, ' ');

//        int j = 0;
//        while (j != 5) {
//            printf("comm|%s|\n", commande[j]);
//            j++;
//        }

        if ((strcmp(listeAvecEspaces[0],"quitter") == 0)||(strcmp(listeAvecEspaces[0],"exit")) == 0) {
            quitter();
        }
        else if (strcmp(listeAvecEspaces[0],"pwd") == 0) {
            printf( "%s", getRepertoireCourant()) ;
        } else if (strcmp(listeAvecEspaces[0],"cd") == 0){
            //printf("yahaha !\n");
            //printf("cd :%s",commande[i+1]);
            automateCd(mot);
        } else if (strcmp(listeAvecEspaces[0], "echo") == 0) {
            ecrire(listeAvecEspaces[1]);
        } else if(strcmp(listeAvecEspaces[0], "color") == 0){
            changeColor(listeAvecEspaces[1]);
        } else if (strcmp(listeAvecEspaces[0], "ls") == 0) {
            mbash("dir");
        } else {
            mbash(mot);
        }
        i++;
    }
}

void ecrire(char* commande) {
    printf("%s\n", commande);
}

void commandeCd(char* commande) {
    if (chdir(commande) == -1) {
        printf("%s%s\n","Impossible de se placer dans le dossier ", commande);
    } else {
        printf("%s%s\n","Deplacement dans le dossier ", commande);
        getcwd(rep, MAXLI);
    }
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
    char* temporaire2;
    char* directory;

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
                    case '\0':
                        //printf("commande cd");
                         //code pour "cd"
                         commandeCd("/");
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
                        if (strlen(commande) > 3) {
                            printf("ici\n");
                            directory = calloc(MAXLI,1);
                            strcpy(directory,&commande[2]);
                            directory[0] = '/';
                            printf("directory : %s\n",directory);
                            commandeCd(directory);
                        } else {
                            commandeCd("/");
                        }    
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
                    case '\0':
                        //printf("commande cd");
                        //commande "cd"
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                    default:
                        state = S_ERREUR;
                        break;
                }
                break;

            case S_UNE_VAGUE: // ------------------------------------------------
                switch (caractereCourant) {
                    case '\0':
                        //printf("commande cd ~\n");
                        //commande "cd ~"
                        commandeCd("/");
                        state = S_FINI;
                        break;
                    default :
                        //printf("commande cd ~nom\n");
                        //on récupère la fin de la commande qui est le nom après la vague
                        //commande "cd ~nom"
                        //erreur ou non

                        temporaire2 = calloc(MAXLI,1);
                        strcpy(temporaire2,&commande[3]);
                        temporaire2[0]= '/';
                        commandeCd(temporaire2);
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
                    case '\0':
                        //printf("commande cd ..\n");
                        //commande "cd .."
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                    case '/':
                        //printf("cas: cd ../\n");
                        state = S_DEUX_POINT_UN_SLASH;
                        break;
                    default :
                        commandeCd(&commande[3]);
                        state = S_FINI;
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
                    case '\0':
                        compteurRetour++;
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                    default:
                        compteurRetour ++;
                        //printf("commande cd ../nom\n");
                        //on récupère la fin de la commande qui est le nom
                        //commande "cd ../nom"
                        //erreur ou non
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                }
                break;

            case S_UN_SLASH: // ------------------------------------------------
                switch (caractereCourant) {
                    case '\0':
                        //printf("commande cd /\n");
                        //commande "cd /"
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
//                    case ' ':
//                        state = S_SLASH_DES_ESPACES;
//                        break;
                    default :
                        //printf("commande cd/chemin\n");
                        //on récupère la fin de la commande qui est le chemin
                        //commande "cd /chemin"
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                }
                break;
        }
      }
}

void stringSlicer(char* string, char* result[MAXNBSTR], char caractereSeparateur){
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
                if (caractereCourant == caractereSeparateur) {
                    state = S_ESPACE;
                } else if ((caractereCourant == 32)&&(caractereSeparateur != 32)) {
                    printf("test\n");
                    state = S_DEPART;
                } else {
                    switch(caractereCourant){
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
                }
            break;

            case S_MOT :
                strncat(mot,&string[tete], 1);

                if (caractereCourant == caractereSeparateur) {
                    state = S_ESPACE;
                } else {
                    switch(caractereCourant){
                        case '\0':
                            stringSlicerPutIntoList(mot, result, nbResultats);
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
                        //stringSlicerPutIntoList(mot, result, nbResultats);
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_APRES_DOUBLE_COTE:
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case '\0':
                        //stringSlicerPutIntoList(mot, result, nbResultats);
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\"':
                        //stringSlicerPutIntoList(mot, result, nbResultats);
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

                if (caractereCourant == caractereSeparateur) {
                    state = S_ESPACE;
                } else if ((caractereCourant == 32)&&(caractereSeparateur != 32)) {
                    printf("test2\n");
                    state = S_DEPART;
                } else {
                    switch(caractereCourant){
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
                }
            break;
        }
        tete++;
    }
}

void stringSlicerPutIntoList(char* mot, char* liste[MAXNBSTR], int index){
    liste[index] = calloc(strlen(mot), 1);
    strcpy(liste[index], mot);
    free(mot);
    mot = calloc(MEMOT, 1);
}

void changeColor(char* mot){
    if (strcmp(mot, "blue") == 0){
        couleur = ANSI_COLOR_BLUE;
    } else if (strcmp(mot, "cyan") == 0){
        couleur = ANSI_COLOR_CYAN;
    } else if (strcmp(mot, "green") == 0){
        couleur = ANSI_COLOR_GREEN;
    } else if (strcmp(mot, "magenta") == 0){
        couleur = ANSI_COLOR_MAGENTA;
    } else if (strcmp(mot, "red") == 0){
        couleur = ANSI_COLOR_RED;
    } else if (strcmp(mot, "yellow") == 0){
        couleur = ANSI_COLOR_YELLOW;
    } else if (strcmp(mot, "default") == 0){
        couleur = ANSI_COLOR_RESET;
    } else {
        printf("couleur inexistante : %s",mot);
    }
    printf("%s", couleur);
}