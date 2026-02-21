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
            int redirection_trouvee = -1; //savoir où se coupe le tableau args

            while(args[j]!=NULL){
                //gestion de ">"
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

                    if(fd<0){
                        perror("Erreur open");
                        exit(EXIT_FAILURE);
                    }
                    //On remplace la sortie standard (1) par notre fichier (fd)
                    dup2(fd, STDOUT_FILENO);
                    //On ferme le descripteur fd
                    close(fd);
                    // On "coupe" le tableau args pour que execvp ne voie pas le ">" et le nom du fichier
                    if(redirection_trouvee==-1){
                        redirection_trouvee=j;
                    }
                }

                //gestion de "<"
                else if(strcmp(args[j], "<")==0){
                    char *filename = args[j+1];
                    if(filename == NULL){
                        fprintf(stderr,"Erreur : fichier manquant après <\n");
                        exit(EXIT_FAILURE);
                    }
                    //ouverture : lecture seule
                    int fd = open(filename, O_RDONLY);
                    if(fd<0){
                        perror("Erreur open <");
                        exit(EXIT_FAILURE);
                    }

                    dup2(fd, STDIN_FILENO); //On remplace l'entrée standard
                    close(fd);
                    if(redirection_trouvee==-1){
                        redirection_trouvee=j;
                    }
                }

                else if (strcmp(args[j],">>") == 0){
                    if(args[j+1]==NULL){
                        fprintf(stderr,"Erreur : fichier manquant arpès >>\n");
                        exit(EXIT_FAILURE);
                    }
                    // O_APPEND ajout à la fin du fichier sans écrasser
                    int fd = open(args[j+1], O_WRONLY|O_CREAT|O_APPEND, 0644);
                    if(fd<0){
                        perror("Erreur open >>");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    if(redirection_trouvee==-1){
                        redirection_trouvee=j;
                    }
                }
                j++;
            }

            //On coupe le tableau d'arguments au niveeau de la premieere redirection
            if(redirection_trouvee!=-1){
                args[redirection_trouvee]=NULL;
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
            //on est dans le processus père
            //il faut que le père sache comment le fils s'est terminé 
            //on se prépare pour les commandes && et ||, on va attendre la fin du fils pour savoir si la commande s'est bien exécutée ou pas
            int status;
            waitpid(pid, &status, 0); //plus précis
            if(WIFEXITED(status)){
                int code_retour = WEXITSTATUS(status);
                if(code_retour==0){
                    //test
                }
            }
        }

    }
    return 0;
}