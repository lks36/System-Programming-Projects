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

***Architecture***
le shell est séparé de deux catégorie:
    - built-in : cd, exit, help
    - commandes externes : ls, cat, grep...

***execvp***
execvp ne contient pas de built-in, il n'excute que des fichiers executables

***open***
utilisation de open et pas fopen:
 - adapter dup2, car dup2 prend en paramètre un int, et open renvoie un int, et fopen non
 - fopen utilise des permissions 0664 par défaut, open peut gérer les permissions plus en détaille
 - danger de Buffer : fopen risque d'avoir un moment critique au moment du fork, le message sera dupliquer 2 fois et open envoie au système immédiatement. On veut que les choses se passent immédiatement et précisément là où on a branché les tuyaux.

```

## contraintes
```
- supposons la commande n'a que 1ko, je réserves statiquement cet espace dès le lancement du programme sur stack

- supposons la commande shell peut accepter 63 arguments maximum

- le cas de "built-in"
    - cd accepte plusieurs arguments sans levé d'erreur

- Utilisation de execvp : les commandes externes ne sont pas codés par nous même

- dup2 indispensable : on doit utiliser dup2 pour faire la fermeture de flux de sorti vers l'écran, et mettre un fichier à la place
```

## Idées
```
***Des built-in***
execvp ne peut pas executer des commandes comme cd, car cela est un built-in, 
```

# Doc
```

```

## TODO
```
suite : on va commancer à prépater les redirections .. 
```