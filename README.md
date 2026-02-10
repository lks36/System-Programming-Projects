# System-Programming-Projects

## Un mini interpréteur de commandes (Mini-Shell en C)

Un shell fonctionne selon une boucle infinie appelée REPL (Read-Eval-Print Loop)
1. Read : Lire la commande tapée par l'utilisateur.

2. Parse : Découper la commande (ex: "ls -l" devient ["ls", "-l"]).

3. Execute : Créer un processus enfant pour lancer la commande.

4. Repeat : Revenir au début.

## Features
- Process management (fork/exec)

- Built-in commands (cd/exit)

- Redirections (WIP).


## Documentation interne
Voir [DEV_NOTES.md](./DEV_NOTES.md) pour les choix techniques et notes de développement.