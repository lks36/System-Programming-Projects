#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){
    //je réserves statiquement cet espace dès le lancement du programme sur stack
    char command[1024];
    //accepter 63 arguments maximum, 64 ème est null, pour indiquer la fin
    char *args[64];

    //on ne s'arrête jamais tout seul, il affiche un prompt et attend une instruction
    while(1){
        printf("MyShell>"); // Le prompt

        //fgets lit ce que l'utilisateur tapes au clavier
        if (fgets(command, sizeof(command), stdin)== NULL) break;

        //strcspn cherche la position du premier caracètre '\n' dans la chaîne, et nettoyer le '\n' à la fin, n'oublie pas, car presse sur Entrée = \n
        command[strcspn(command, "\n")]='\0';

        //Découpage des arguments (Parsing)      
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

        // 3. Test d'affichage pour vérifier le parsing
        printf("commande : %s\n", args[0]);
        for(int j = 1; args[j] != NULL; j++) {
            printf("argument %d : %s\n", j, args[j]);
        }
        //création du processus fils
        pid_t pid = fork();
        if(pid == 0){
            //on est donc dans le processus fils, on va exécuter la commande
            //execvp prend en paramètre le nom de la commande et les arguments
            if(execvp(args[0], args)==-1){
                //si execvp retourne -1, cela signifie qu'il y a eu une erreur
                perror("Erreur d'exécution");
            }
            exit(EXIT_FAILURE); // Terminer le processus fils en cas d'erreur
            //le fils est mort, le fils ne reviens pas dans la boucle while du père
        }
        else if (pid < 0){
            //on a eu une erreur lors de la création du processus fils
            perror("Erreur de fork");
        }
        else {
            //on est dans le processus père, on attend que le fils termine
            wait(NULL);
        }

    }
    return 0;
}