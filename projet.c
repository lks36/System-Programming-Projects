#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(){
    //je réserves statiquement cet espace dès le lancement du programme sur stack
    char command[1024];
    //accepter 63 arguments maximum, 64 ème est null, pour indiquer la fin
    char *args[64];

    //boucle de processus, le shell doit rester actif
    while(1){
        printf("MyShell>"); // Le prompt

        //***Lecture de la commande*/
        //fgets lit ce que l'utilisateur tapes au clavier
        if (fgets(command, sizeof(command), stdin)== NULL) break;

        //strcspn cherche la position du premier caracètre '\n' dans la chaîne, et nettoyer le '\n' à la fin, n'oublie pas, car presse sur Entrée = \n
        command[strcspn(command, "\n")]='\0';

        //***Découpage des arguments (Parsing)***    
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

        //***Les built-in***/
        //built-in cd
        if(strcmp(args[0],"cd")==0){
            char *arg = args[1];
            //si sans arguments, on retourne au home
            if(arg==NULL){
                arg=getenv("HOME");
            }
            //on change répertoire
            if(chdir(arg)==-1){
                perror("MyShell : cd ");
            }
            //on ne fork pas, sinon erreur d'execution
            continue;
        }

        //built-in echo
        if(strcmp(args[0],"echo")==0){
            int ind=1;
            while(args[ind]!=NULL){
                //si l'argument commence par un $
                if(args[ind][0]=='$'){
                    //on met ce que contient la variable environnement dans env
                    char *env = getenv(args[ind]+1);
                    if(env){
                        printf("%s",env);
                    }
                }
                else{
                    printf("%s",args[ind]);
                }
                if(args[ind+1]){
                    printf(" ");
                }
                ind++;
            }
            printf("\n");
            continue;
        }

        if(strcmp(args[0],"help")==0){
            printf("---MyShell help---\n");
            printf("Commandes intégrés : cd, exit, help\n");
            continue;
        }
        
        

        //*****Test d'affichage pour vérifier le parsing*****
        printf("commande : %s\n", args[0]);
        for(int j = 1; args[j] != NULL; j++) {
            printf("argument %d : %s\n", j, args[j]);
        }

        //****création du processus fils pour exécuter la commande****
        pid_t pid = fork();
        if(pid == 0){
            int j = 0;
            while(args[j]!=NULL){
                //il faut qu'on retrouve la redirection
                if(strcmp(args[j],">")==0){
                    char *filename = args[j+1];

                    if(filename==NULL){
                        fprintf(stderr, "Erreur : fichier NULL");
                        exit(EXIT_FAILURE);
                    }
                    //Sion, on ouvre le fichier (écriture seule, le créer s'il n'existe pas)
                    //ici, on utilise open pour obtenir le descripteur du fichier(int)
                    //O_WRONLY:écriture seule
                    //O_CREAT:créer le fichier s'il n'existe pas
                    //O_TRUNC:vider le fichier s'il existe déjà
                    //0644:permission (lecture/écriture pour moi, mais lecture seule pour les autres)

                    int fd = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);
                    //Buffer à comprendre
                    
                    //à continuer !!!!!!!!!!!
                }
            }


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