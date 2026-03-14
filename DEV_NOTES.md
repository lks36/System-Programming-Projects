# Notes de dev

## Réflexions

- supposons la commande n'a que 1ko, je réserves statiquement cet espace dès le lancement du programme sur stack

- supposons la commande shell peut accepter 63 arguments maximum


***Architecture***
le shell est séparé de deux catégorie:
- built-in : cd, exit, help
- commandes externes : ls, cat, grep... 

---

## Techinique
En C, la gestion de la mémoire est manuelle et explicite, on doit gérer la mémoire nous même.

***Utilisation de fgets***
Ne pas utiliser de malloc mais fgets, car pour un shell simple, la gestion statique sur la pile est plus performante et évite les fuites mémoires;
1024 octets pour la ligne de commande et 64 pointeurs pour les argmuments couvrent la plupart des usages classique;

***strtok***
strtok utilise une variable statique interne pour mémoriser sa position dans la chaîne de caractères, donc strtok n'est pas "thread-safe" dangereuse si on utilise plusieurs threads

***Cas limite***
Erreurs de lecture : fgets comparé avec NULL vérifie si la lecture a échoué, ou si l'utilisateur a envoyé un signal de fin de fichier(EOF), sinon le programme pourrait boucler à l'infini en traitant une commande vide, strtok découpe une commande qui n'existe pas.

***Création du processus fils***
le processus père donne des instructions au processus fils(execvp), et attendre qu'il finit avant de passer à la suivante.

***execvp***
execvp ne contient pas de built-in, il n'excute que des fichiers executables

***open***
utilisation de open et pas fopen:
 - adapter dup2, car dup2 prend en paramètre un int, et open renvoie un int, et fopen non
 - fopen utilise des permissions 0664 par défaut, open peut gérer les permissions plus en détaille
 - danger de Buffer : fopen est une fonction de bibliothèque, l'appel système se fera après un certains traitement, et open est une fonction qui fait directement un appel système.

 ***dup2***
 redirige un flux existant vers un autre numéro (souvent 0/1/2)

---

## Contraintes

### Limites
- La commande shell peut accepter 63 arguments maximum
- Les "built-in"
    - cd accepte plusieurs arguments sans levé d'erreur
- Utilisation de execvp : les commandes externes ne sont pas codés par nous même
- Commande ***dup2*** indispensable : on doit utiliser dup2 pour faire la fermeture de flux de sorti vers l'écran, et mettre un fichier à la place
- comme j'utilise strtok(command, " ") pour coupé les commandes, donc le mini shell coupe strictement sur les espaces.

### La redirection
- Erreur avec les espace : on a le redirection des flux, mais > et < ne peut pas être coller au argumenets, différent au shell classique, la commande
````
    ls >fichier
````
 ne fonctionnera pas

- Les redirections multiples n'existent pas

- La gestion de canal 2(Erreur) n'existe pas

### Pilelines
- IL faut avoir des espaces entre les | 
- Les Pilepines multiples existent pas


### Les opérateurs logique "&&" et "||"
- Il faut avoir des espaces entre les && et ||
- une descente récursive, mais elle évalue de gauche à droite en coupant la chaîne, dans des cas sans parenthèses (ex: cmd1 || cmd2 && cmd3), l'ordre d'exécution pourrait légèrement différer du vrai Bash

### Arrière plan
- à tester plus de bugs

---

## Idées
***Des built-in***
execvp ne peut pas executer des commandes comme cd, car cela est un built-in,

***Tricher***
On va utilier les built-ins du système, le programme /bin/echo existe déjà sur Linux, on peut éviter l'erreur du redirection avec echo

---

## TODO

### Bugs
- Toujour des erreurs les espaces
    - Le problème d'espace dans cd
    - Le problème d'espace dans redirection
    - Solution : ?
- implémentation de multi redirection
- implémentaion de multi pipeline
- pas de séparation : echo fi & echo fi => affichage : fi & echo fi

### Suites
- les bugs à corriger