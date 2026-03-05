# System-Programming-Projects

## Description du projet
### Un mini interpréteur de commandes (Mini-Shell en C)
Le projet est pour l'objectif de développer un terpréteur de commande en C, respectant la norme POSIX, le shell fonctionne selon une boucle infinie appelée REPL (Read-Eval-Print Loop)

1. Read : Lire la commande tapée par l'utilisateur.

2. Parse : Découper la commande dans l'ordre de priorité :
    1. redirections   > < >>
    2. pipeline       |
    3. logique        &&  || (à faire)
    4. séparation     ;  & (à faire)

3. Execute : Créer un processus enfant pour lancer la commande.

4. Repeat : Revenir au début.

## Fonctionnalités 
### Execution de commandes :
- Built-in commands (cd/exit/..)
- Commmandes externes (fork/exec - programmes séparés stoké dans bin/..)
- Redirections simples ls > fichier : cat < fichier, ls >> fichier
- Redirections multiple simple : cat < fichier >> fichier

## Installation et execution
### Prérequis
- Un système d'exploitation basé sur Unix (Linux, macOS, ou WSL sur Windows).
- GCC (GNU Compiler Collection)

### Execution
executer le programme avec 
```
gcc projet.c & ./a.out
```
puis utilisation de mini shell

## Documentation
Voir [DEV_NOTES.md](./DEV_NOTES.md) pour les choix techniques et notes de développement.
