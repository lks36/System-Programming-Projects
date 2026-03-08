#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

//cette fonction prend en paramètre le tableau d'arguments et applique les redirections, elle modifie le tableau d'arguments pour que execvp ne voit pas les redirections
void appliquer_redirections(char **args) {
    int j = 0;
    int index_coupe = -1;
    while(args[j] != NULL) {
        //on gère le cas ">"
        if (strcmp(args[j], ">") == 0) {
            int fd = open(args[j+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd < 0) { 
                // Le fils meurt proprement si le fichier est introuvable ou bloqué
                perror("Erreur d'ouverture du fichier"); 
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); close(fd);//redirection de la sortie standard vers le fichier
            if(index_coupe == -1) index_coupe = j;
            
        }
        //on gère le cas ">>"
        else if (strcmp(args[j], ">>") == 0) {
            int fd = open(args[j+1], O_WRONLY|O_CREAT|O_APPEND, 0644);
            if (fd < 0) { 
                // Le fils meurt proprement si le fichier est introuvable ou bloqué
                perror("Erreur d'ouverture du fichier"); 
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); close(fd);//redirection de la sortie standard vers le fichier en mode ajout
            if(index_coupe == -1) index_coupe = j;
        }
        //on gère le cas "<"
        else if (strcmp(args[j], "<") == 0) {
            int fd = open(args[j+1], O_RDONLY);
            if (fd < 0) { 
                // Le fils meurt proprement si le fichier est introuvable ou bloqué
                perror("Erreur d'ouverture du fichier"); 
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO); close(fd); // redirection de l'entrée standard vers le fichier
            if(index_coupe == -1) index_coupe = j;
        }
        j++;
    }
    if(index_coupe != -1) args[index_coupe] = NULL;
}

// Fonction pour exécuter une ligne de commande (à améliorer pour gérer les redirections et les pipes)
int executer_ligne(char **args) {
    //Détection du pipe
    int pipe_trouvee = -1;
    for(int j=0; args[j]!=NULL; j++){
        if(strcmp(args[j],"|") == 0){
            pipe_trouvee = j;
            break;
        }
    }

    if(pipe_trouvee != -1) {
        //on sépare les arguments 
        args[pipe_trouvee]=NULL; //on coupe le premier tableau d'arguments
        char **args1 = args; //arguments pour la première commande


        //on met un & ici pour indiquer que le tableau args2 commence à partir de args[pipe_trouvee+1]
        char **args2 = &args[pipe_trouvee+1]; //arguments pour la deuxième commande

        int fd[2];//tableau avec des descripteur de fichier, fd[0] pour la lecture, fd[1] pour l'écriture
        if(pipe(fd)==-1){
            perror("erreur pipe");
            exit(EXIT_FAILURE);
        }

        //premier fils pour la première commande (partie gauche)
        if(fork()==0){
            signal(SIGINT, SIG_DFL);//comportement par défaut signt à comprendre
            dup2(fd[1], STDOUT_FILENO); //redirection de la sortie standard vers l'écriture du pipe
            close(fd[0]); //fermer la lecture du pipe
            close(fd[1]); //fermer l'écriture du pipe après la redirection
            appliquer_redirections(args1);
            if(execvp(args1[0], args1)==-1){
                perror("Erreur d'exécution de la première commande");
                exit(EXIT_FAILURE);
            }
        }

        //deuxième fils pour la deuxième commande
        if(fork()==0){
            signal(SIGINT, SIG_DFL);//comportement par défaut signt à comprendre
            dup2(fd[0], STDIN_FILENO); //redirection de l'entrée standard
            close(fd[1]); //fermer l'écriture du pipe
            close(fd[0]); //fermer la lecture du pipe après la redirection
            appliquer_redirections(args2);
            if(execvp(args2[0], args2)==-1){
                perror("Erreur d'exécution de la deuxième commande");
                exit(EXIT_FAILURE);
            }
        }

        close(fd[0]); //fermer la lecture du pipe
        close(fd[1]); //fermer l'écriture du pipe
        // On attend les deux fils du pipe
        int status_droit;
        wait(NULL); // On ignore le retour du premier
        wait(&status_droit); // On prend le retour du deuxième (c'est la norme Bash)
        
        // Si le pipe s'est bien passé, on retourne le code de retour de la dernière commande
        if (WIFEXITED(status_droit)) return WEXITSTATUS(status_droit);
        return 1; // Erreur par défaut
    }
    
    //Built-ins (cd, help)
    if(strcmp(args[0],"cd")==0){
        char *arg = args[1];
        if(arg == NULL) arg = getenv("HOME");
        if(chdir(arg) == -1) { perror("MyShell : cd "); return 1; }
        return 0; // Succès
    }
    
    if(strcmp(args[0],"help")==0){
        printf("---MyShell help---\nCommandes intégrés : cd, exit, help\n");
        return 0; // Succès
    }
    //Fork + appliquer_redirections + execvp
    //****Execute : création du processus fils pour exécuter la commande****/
    pid_t pid = fork();
    int code_retour = -1;
    if(pid == 0){
        //dans processus fils
        signal(SIGINT, SIG_DFL); //comportement par défaut signt à comprendre

        appliquer_redirections(args);//rediretion
        //execvp prend en paramètre le nom de la commande et les arguments
        if(execvp(args[0], args)==-1){
            perror("Erreur d'exécution");
        }
        exit(EXIT_FAILURE); // Terminer le processus fils en cas d'erreur
    }
    else if (pid < 0){
        //on a eu une erreur lors de la création du processus fils
        perror("Erreur de fork");
    }
    //waitpid et récupération du status
    else {
        //on est dans le processus père
        //il faut que le père sache comment le fils s'est terminé 
        //on se prépare pour les commandes && et ||, on va attendre la fin du fils pour savoir si la commande s'est bien exécutée ou pas
        int status;
        waitpid(pid, &status, 0); //plus précis
        if(WIFEXITED(status)){
            code_retour = WEXITSTATUS(status);
            if(code_retour==0){
                //test
            }
        }
    }

    return code_retour; // Retourne 0 si succès, autre chose si erreur
}

// Fonction qui sera appelée automatiquement quand on fait Ctrl+C
void gestionnaire_sigint(int sig) {
    printf("\nMyShell>");
    fflush(stdout); // Force l'affichage immédiat du prompt sur la nouvelle ligne
}


int main(){
    char command[1024];
    char *args[64];

    //******************gestion des signaux******************/
        //à comprendre plus tard !!!!!!!!
        struct sigaction sa;
        sa.sa_handler = gestionnaire_sigint; // on lui donne notre fonction
        sigemptyset(&sa.sa_mask);            // on vide le masque
        sa.sa_flags = SA_RESTART;            // on empêche fgets de planter si on fait ctrl+c
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("Erreur de sigaction");
            exit(EXIT_FAILURE);
        }
    //******************************************************/

    //boucle de processus
    while(1){
        printf("MyShell>");

        //****************Lecture de la commande***************/
        //lecture de la commande entrée par l'utilisateur
        if (fgets(command, sizeof(command), stdin)== NULL) break;

        //nettoyer le '\n' à la fin, n'oublie pas, car presse sur Entrée = \n
        command[strcspn(command, "\n")]='\0';

        //***Découpage des arguments (Parsing)***
        int i = 0;
        //strtok découpe une chaîne à chaque fois qu'elle trouve le " ", ici on récupère le premier mot
        args[i] = strtok(command," ");
        while(args[i] != NULL){
            //pré incrémentaion
            args[++i] = strtok(NULL, " ");
        }
        //****************Fin de lecture de la commande***************/

        if(args[0] == NULL) continue;
        if(strcmp(args[0],"exit")==0) break;

        //****Détecte les opérateurs && et || ****/
        
        // à continuer

        executer_ligne(args);

    }
    return 0;
}