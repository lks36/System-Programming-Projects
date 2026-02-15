# System-Programming-Projects

## Un mini interpréteur de commandes (Mini-Shell en C)
Le projet est pour l'objectif de développer un terpréteur de commande en C, respectant la norme POSIX, le shell fonctionne selon une boucle infinie appelée REPL (Read-Eval-Print Loop)

1. Read : Lire la commande tapée par l'utilisateur.

2. Parse : Découper la commande (ex: "ls -l" devient ["ls", "-l"]).

3. Execute : Créer un processus enfant pour lancer la commande.

4. Repeat : Revenir au début.

## Fonctionnalités 
- Process management (fork/exec)

- Execution de commandes :
    - Built-in commands (cd/exit/..)
    - Commmandes externes (programmes séparés stoké dans bin/..)

- Redirections (WIP).


## Documentation interne
Voir [DEV_NOTES.md](./DEV_NOTES.md) pour les choix techniques et notes de développement.