#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
void stringSlicer(char* string, char** result);

int main(){
    char** res = malloc(sizeof(char*)*1000);
    stringSlicer("yohoho !", res);
    printf("test !\n");
    for(int i = 0; i < sizeof(res)/sizeof(char*); i++){
        printf("|%s|",res[i]);
    }
}

void stringSlicer(char* string, char** result){
    #define S_DEPART 0
    #define S_MOT 1
    #define S_APRES_SIMPLE_COTE 2
    #define S_APRES_DOUBLE_COTE 3
    #define S_ESPACE 4
    #define S_FINI 100
    char caractereCourant;
    char** resultat = malloc(sizeof(char)*1000);
    int nbResultats = 0;
    char mot[256];
    mot[0] = '\0';
    int state = 0;
    int tete = 0;
    while(state < S_FINI){
        caractereCourant = string[tete];
        printf("caractere courant : %c\n", caractereCourant);

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
                printf("mot : %s\n",mot);

                switch(caractereCourant){
                    case ' ':
                        state = S_ESPACE;
                        break;
                    case '\0':
                        printf("mot envoye : %s\n", mot);
                        strcpy(resultat[nbResultats] , mot);
                        mot[0] = '\0';
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
                        strcpy(resultat[nbResultats] , mot);
                        mot[0] = '\0';
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\'':
                        strcpy(resultat[nbResultats] , mot);
                        mot[0] = '\0';
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_APRES_DOUBLE_COTE:
                strncat(mot,&string[tete], 1);

                switch(caractereCourant){
                    case '\0':
                        strcpy(resultat[nbResultats] , mot);
                        mot[0] = '\0';
                        nbResultats++;
                        state = S_FINI;
                        break;
                    case '\"':
                        strcpy(resultat[nbResultats] , mot);
                        mot[0] = '\0';
                        nbResultats++;
                        state = S_DEPART;
                        break;
                }
            break;

            case S_ESPACE :
                if(strlen(mot) != 0){
                    strcpy(resultat[nbResultats] , mot);
                    mot[0] = '\0';
                    nbResultats++;
                    printf("mot dans espace : %s\n",mot);
                }
                switch(caractereCourant){
                    case ' ':
                        state = S_ESPACE;
                        break;
                    case '\'':
                        state = S_APRES_SIMPLE_COTE;
                        break;
                    case '\"':
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
    result = resultat;
}