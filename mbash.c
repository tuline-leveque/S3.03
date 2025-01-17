#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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

//Déclaration des structures -------------------------------------------------------------------------------------------
struct tm tm;
struct commandeHistorique{
    char* commande;
    struct tm time;
};

//Déclaration des variables --------------------------------------------------------------------------------------------
struct commandeHistorique historiqueCommandes[MAXLI] = {};
int nbCommandesHistorique = 0;
char cmd[MAXNBSTR];
char path[MAXLI];
char rep[MAXLI];
char* couleur = ANSI_COLOR_RESET;
int rester = 1;
int pathidx;
char* commande[MAXNBSTR] = {};

//Déclaration des fonctions --------------------------------------------------------------------------------------------
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



/* ------------------------------------------- MAIN ------------------------------------------- */
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
    struct commandeHistorique ch;
    ch.commande = calloc(sizeof(cmd), 1);
    strcpy(ch.commande,cmd);
    time_t t = time(NULL);
    ch.time = *localtime(&t);
    historiqueCommandes[nbCommandesHistorique] = ch;
    nbCommandesHistorique++;
    //Permet de séparer les différents appels de commande
    stringSlicer(cmd, commande, ';');
    //Lance la commande écrite
    lancerCommandeListe(commande);
    memset(commande, 0, sizeof commande);
  }
  return 0;
}


/* ------------------------------------------- MBASH ------------------------------------------- */
/**
  * Fonction mbash qui permet d'exécuter une commande
  * Paramètre com : commande à exécuter
 **/
void mbash(char* com) {
  printf("Execute: %s\n", com);
  system(com);
}


/* ------------------------------------------- QUITTER ------------------------------------------- */
/**
  * Fonction quitter qui permet de fermer le programme
 **/
void quitter() {
    //Termine la boucle du main
    rester = 0;
    printf(ANSI_COLOR_RESET"");
}


/* ------------------------------------------- GETREPERTOIRECOURANT ------------------------------------------- */
/**
  * Fonction getRepertoireCourant qui permet de récupérer le chemin du répertoire courant (équivalent à "pwd")
  * Retourne le répertoire courant
 **/
char* getRepertoireCourant() {
    getcwd(rep, 200);
    return rep;
}


/* ------------------------------------------- LANCERCOMMANDELISTE ------------------------------------------- */
/**
  * Fonction lancerCommandeListe qui permet de lancer une commande de la liste de commande
  * Paramètre commande : liste des commandes à exécuter
 **/
void lancerCommandeListe(char* commande[MAXNBSTR]) {
    int i = 0;
    //Tant qu'il y a des commandes à exécuter
    while(commande[i] != NULL){

        char* mot = commande[i];
        char* listeAvecEspaces[MAXNBSTR] = {};
        /*On récupère chaque partie de la commande (séparée par un espace)
          Par exemple, echo bla ==> [echo, bla]*/
        stringSlicer(mot, listeAvecEspaces, ' ');

        //Cas dans lequel la commande entrée est "quitter" ou "exit"
        if ((strcmp(listeAvecEspaces[0],"quitter") == 0)||(strcmp(listeAvecEspaces[0],"exit")) == 0) {
            quitter();
        }
        //Cas dans lequel la commande entrée est "pwd"
        else if (strcmp(listeAvecEspaces[0],"pwd") == 0) {
            printf( "%s", getRepertoireCourant()) ;
        }
        //Cas dans lequel la commande entrée est "cd"
        else if (strcmp(listeAvecEspaces[0],"cd") == 0) {
            //Lance l'automate qui vérifie si la commande est valide et l'exécute
            automateCd(mot);
        }
        //Cas dans lequel la commande entrée est "echo"
        else if (strcmp(listeAvecEspaces[0], "echo") == 0) {
            ecrire(listeAvecEspaces[1]);
        }
        //Cas dans lequel la commande entrée est "color"
        else if(strcmp(listeAvecEspaces[0], "color") == 0){
            changeColor(listeAvecEspaces[1]);
        }
        //Cas dans lequel la commande entrée est "ls"
        else if (strcmp(listeAvecEspaces[0], "ls") == 0) {
            //L'équivalent de la commande "ls" en c est la commande "dir"
            mbash("dir");
        }
        //Cas dans lequel la commande entrée est "history" ou "historique"
        else if ((strcmp(listeAvecEspaces[0], "history") == 0)||(strcmp(listeAvecEspaces[0], "historique") == 0)) {
            if (listeAvecEspaces[1] == NULL) {
                printf("--------Historique--------\n");
                //Pour chaque ligne de l'historique
                for (int i = 0; i < nbCommandesHistorique; i++) {
                    struct commandeHistorique c = historiqueCommandes[i];
                    //On écrit l'historique de cette commande
                    printf("%d - %d-%02d-%02d %02d:%02d:%02d : %s\n", i,c.time.tm_year + 1900, c.time.tm_mon + 1, c.time.tm_mday, c.time.tm_hour, c.time.tm_min, c.time.tm_sec, c.commande );
                }
                printf("-----------------------\n");
            } else {
                //S'il y a un numéro, alors on reexécute la commande contenant le numéro donné (chaque commande de l'historique est numérotée)
                char* commandeARefaire[MAXNBSTR] = {};
                stringSlicer(historiqueCommandes[atoi(listeAvecEspaces[1])].commande, commandeARefaire, ';');
                lancerCommandeListe(commandeARefaire);
            }
        }
        //Cas dans lequel la commande entrée ne correspond à aucun autre cas fait par nos soins
        else {
            mbash(mot);
        }
        i++;
    }
}


/* ------------------------------------------- ECRIRE ------------------------------------------- */
/**
  * Fonction ecrire qui permet d'écrire un texte donné dans la fenêtre (équivalent à "echo")
  * Paramètre texte : texte à écrire
 **/
void ecrire(char* texte) {
    printf("%s\n", texte);
}


/* ------------------------------------------- COMMANDECD ------------------------------------------- */
/**
  * Fonction commandeCd qui permet d'exécuter une commande "cd"
  * Paramètre commande : commandes à exécuter
 **/
void commandeCd(char* commande) {
    //Si le dossier dans lequel on souhaite se situer n'est pas valide
    if (chdir(commande) == -1) {
        printf("%s%s\n","Impossible de se placer dans le dossier ", commande);
    } else {
        printf("%s%s\n","Deplacement dans le dossier ", commande);
        //On n'oublie pas de changer le répertoire
        getcwd(rep, MAXLI);
    }
}


/* ------------------------------------------- CHANGECOLOR ------------------------------------------- */
/**
  * Fonction changeColor qui permet de changer la couleur du texte du programme
  * Paramètre mot : couleur à mettre
 **/
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
        printf("Couleur inexistante : %s",mot);
    }
    printf("%s", couleur);
}



/* ------------------------------------------- AUTOMATE CD ------------------------------------------- */
/**
  * Fonction automateCd qui est l'automate permettant de vérifier la validité d'une commande "cd"
  * Paramètre commande : commandes "cd" à vérifier
 **/
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
    #define S_ERREUR 999

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
                    state = S_C;
                    break;
                  default:
                    state = S_ERREUR;
              }
              break;

            case S_C: // ------------------------------------------------
                switch (caractereCourant) {
                    case 'd': {
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
                        state = S_CD_ESPACE;
                        break;
                    case '\0':
                         //Cas où la commande entrée est "cd"
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
                        state = S_UNE_VAGUE;
                        break;
                    case '.':
                        state = S_UN_POINT;
                        break;
                    case '/':
                        state = S_UN_SLASH;
                        break;
                    case ' ':
                        state = S_CD_DES_ESPACES;
                        break;
                    default :
                        //Le cas par défaut est le cas où un chemin est donné après un "cd "
                        if (strlen(commande) > 3) {
                            directory = calloc(MAXLI,1);
                            //On récupère uniquement le chemin (&commande[2] permet de retirer "cd")
                            strcpy(directory,&commande[2]);
                            directory[0] = '/';
                            commandeCd(directory);
                        } else {
                            commandeCd("/");
                        }
                        state = S_FINI;
                        break;
                }
                break;

            case S_CD_DES_ESPACES: // ------------------------------------------------
                switch (caractereCourant) {
                    case ' ':
                        state = S_CD_DES_ESPACES;
                        break;
                    case '\0':
                        //Cas où la commande entrée est "cd"
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
                        //Cas où la commande entrée est "cd ~"
                        commandeCd("/");
                        state = S_FINI;
                        break;
                    default :
                        //Le cas par défaut est le cas où un chemin est donné après la vague ~
                        temporaire2 = calloc(MAXLI,1);

                        //On récupère uniquement le chemin (&commande[3] permet de retirer "cd ~")
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
                        //Cas où la commande entrée est "cd .."
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                    case '/':
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
                        //Cas où la commande entrée est "cd ../nom"
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                }
                break;

            case S_UN_SLASH: // ------------------------------------------------
                switch (caractereCourant) {
                    case '\0':
                        //Cas où la commande entrée est "cd /"
                        commandeCd("/");
                        state = S_FINI;
                        break;
                    default :
                        //Cas où la commande entrée est "cd /chemin"
                        commandeCd(&commande[3]);
                        state = S_FINI;
                        break;
                }
                break;

            case S_ERREUR: // ------------------------------------------------
                printf("Erreur : La commande cd entree n'est pas valide.\n");
                state = S_FINI;
                break;
        }
      }
}


/* ------------------------------------------- AUTOMATE STRINGSLICER ------------------------------------------- */
/**
  * Fonction stringSlicer qui est l'automate permettant de découper une chaîne en sous-chaînes
  * Paramètre string : chaîne à séparer
  * Paramètre result : liste dans laquelle on va stocker les différentes sous-chaînes
  * Paramètre caractereSeparateur : caractère qui doit servir de séparateur
 **/
void stringSlicer(char* string, char* result[MAXNBSTR], char caractereSeparateur){
    //Taille maximale d'un mot
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
            case S_DEPART : // ------------------------------------------------
                if (caractereCourant == caractereSeparateur) {
                    state = S_ESPACE;
                } else if ((caractereCourant == 32)&&(caractereSeparateur != 32)) {
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

            case S_MOT : // ------------------------------------------------
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

            case S_APRES_SIMPLE_COTE : // ------------------------------------------------
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case '\0':
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\'':
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_APRES_DOUBLE_COTE : // ------------------------------------------------
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case '\0':
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\"':
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_ESPACE : // ------------------------------------------------
                if(strlen(mot) != 0){
                    mot[strlen(mot)-1] = '\0';
                    stringSlicerPutIntoList(mot, result, nbResultats);
                    nbResultats++;
                }

                if (caractereCourant == caractereSeparateur) {
                    state = S_ESPACE;
                } else if ((caractereCourant == 32)&&(caractereSeparateur != 32)) {
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


/* ------------------------------------------- STRINGSLICERPUTINTOLIST ------------------------------------------- */
/**
  * Fonction stringSlicerPutIntoList qui permet d'insérer une chaîne dans une liste
  * Paramètre mot : chaîne à insérer
  * Paramètre liste : liste dans laquelle on va stocker le mot
  * Paramètre index : index de l'endroit à insérer dans la liste
 **/
void stringSlicerPutIntoList(char* mot, char* liste[MAXNBSTR], int index){
    liste[index] = calloc(strlen(mot), 1);
    strcpy(liste[index], mot);
    free(mot);
    mot = calloc(MEMOT, 1);
}