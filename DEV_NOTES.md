# Notes de dev

## réflexions

```
En C, la gestion de la mémoire est manuelle et explicite, on doit gérer la mémoire nous même.

***Utilisation de fgets***
Ne pas utiliser de malloc mais fgets, car pour un shell simple, la gestion statique sur la pile est plus performante et évite les fuites mémoires;
1024 octets pour la ligne de commande et 64 pointeurs pour les argmuments couvrent la plupart des usages classique;

***Cas limite***
Erreurs de lecture : fgets comparé avec NULL vérifie si la lecture a échoué, ou si l'utilisateur a envoyé un signal de fin de fichier(EOF), sinon le programme pourrait boucler à l'infini en traitant une commande vide, strtok découpe une commande qui n'existe pas.

***strtok***
strtok utilise une variable statique interne pour mémoriser sa position dans la chaîne de caractères
strtok n'est pas "thread-safe" dangereuse si on utilise plusieurs threads


***Création du processus fils***
le processus père donne des instructions au processus fils(execvp), et attendre qu'il finit avant de passer à la suivante.
```

## contraintes
```
supposons la commande n'a que 1ko, je réserves statiquement cet espace dès le lancement du programme sur stack

supposons la commande shell peut accepter 63 arguments maximum
```

## Idées
```
***Des built-in***
execvp ne peut pas executer des commandes comme cd, car cela est un built-in, 
```

## TODO
```
suite : on va faire des built-in comme cd ..., donc qu'on ne va pas crée de processus fils
```