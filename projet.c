#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
    //supposons ici, 1ko, je réserves statiquement cet espace dès le lancement du programme sur stack, (solution dynamique ?)
    char command[1024];
    //supposons que la commande shell peut accepter 63 arguments maximum, 64 ème est null, pour indiquer la fin
    char *args[64];

    //on ne s'arrête jamais tout seul, il affiche un prompt et attend une instruction
    while(1){
        printf("MyShell>"); // Le prompt

        //fgets lit ce que l'utilisateur tapes au clavier
        if (fgets(command, sizeof(command), stdin)== NULL) break;

        //strcspn cherche la position du premier caracètre '\n' dans la chaîne, et nettoyer le '\n' à la fin, n'oublie pas, car presse sur Entrée = \n
        command[strcspn(command, "\n")]='\0';

        //Découpage des arguments (Parsing) 
        // test
        printf("test : %s\n",command);
        
        int i = 0;
        //strtok découpe une chaîne à chaque fois qu'elle trouve le " ", ici on récupère le premier mot
        args[i] = strtok(command," ");
        while(args[i] != NULL){
            //pré incrémentaion
            args[++i] = strtok(NULL, " ");
        }

        //si la commande est vide
        if(args[0]==NULL) continue;

        //on quite 
        if(strcmp(args[0],"exit")==0) break;

        //création du processus fils

    }
    return 0;
}