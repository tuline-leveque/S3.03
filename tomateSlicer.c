#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLI 64

void stringSlicer(char* string, char* result[MAXLI]);
void stringSlicerPutIntoList(char* mot, char* liste[MAXLI], int index);

int main(){
    char* res[MAXLI] = {};
    stringSlicer("Ceci est un test...   hahaha\"ca fonctionne '? \" '", res);
    int i = 0;
    while(res[i] != NULL){
        printf("|%s|",res[i]);
        i++;
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
    #define S_FINI 100

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