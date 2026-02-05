# System-Programming-Projects

## Un mini interpréteur de commandes (Mini-Shell en C)

Un shell fonctionne selon une boucle infinie appelée REPL (Read-Eval-Print Loop)
1. Read : Lire la commande tapée par l'utilisateur.

2. Parse : Découper la commande (ex: "ls -l" devient ["ls", "-l"]).

3. Execute : Créer un processus enfant pour lancer la commande.

4. Repeat : Revenir au début.

```
En C, la gestion de la mémoire est manuelle et explicite, on doit gérer la mémoire nous même.

Ne pas utiliser de malloc mais fgets, car pour un shell simple, la gestion statique sur la pile est plus performante et évite les fuites mémoires;
1024 octets pour la ligne de commande et 64 pointeurs pour les argmuments couvrent la plupart des usages classique;
***Cas limite***
Erreurs de lecture : fgets comparé avec NULL vérifie si la lecture a échoué, ou si l'utilisateur a envoyé un signal de fin de fichier(EOF), sinon le programme pourrait boucler à l'infini en traitant une commande vide.

à continuer sur la partie parsing


```