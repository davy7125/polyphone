Polyphone peut être exécuté en ligne de commande pour convertir des banques de sons dans les formats [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) ou [dls](#doc_dls). Les formats de fichiers pris en charge pour la conversion sont [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) et [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Une option peut être appelée pour ouvrir Polyphone en [mode synthé](#doc_synth).

Des fichiers et des répertoires peuvent également être passés en arguments pour les ouvrir dans l'interface graphique, par exemple :command:`polyphone fichier1.sf2 fichier2.sf3 /adresse/du/répertoire` (un répertoire est ouvert dans l'[explorateur de banques de sons](manual/soundfont-explorer.md)).
Quelques [autres options](#doc_other) permettent de réinitialiser la configuration ou d'afficher l'aide.

Lors d'une conversion, le chargement et l'écriture sont consignés dans la console.
Le code retourné est 0 si la conversion a réussi et une autre valeur en cas d'échec (le fichier d'entrée n'existe pas, le fichier ne peut pas être lu ou écrit, un fichier de sortie existe déjà pour les formats sf2, sf3 et dls, etc.).


## Conversion en sf2 {#doc_sf2}


### Ligne de commande


```
polyphone -1 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-1
: Le mode «&nbsp;1&nbsp;» correspond à «&nbsp;convertir en sf2&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même répertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.sf2&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.


### Exemple


```
polyphone -1 -i /adresse/du/fichier.sfArk
```


## Conversion en sf3 {#doc_sf3}


### Ligne de commande


```
polyphone -2 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie> -c <configuration>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-2
: Le mode «&nbsp;2&nbsp;» correspond à «&nbsp;convertir en sf3&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même répertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.sf3&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion.
  Il est possible de spécifier la qualité de compression&nbsp;: «&nbsp;0&nbsp;» pour basse, «&nbsp;1&nbsp;» pour moyenne, «&nbsp;2&nbsp;» pour haute.
  Cet argument est optionnel&nbsp;: par défaut le paramètre «&nbsp;1&nbsp;» (qualité moyenne) est utilisé.


### Exemple


```
polyphone -2 -i /adresse/du/fichier.sf2 -c 2
```


## Conversion en sfz {#doc_sfz}


### Ligne de commande


```
polyphone -3 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie> -c <configuration>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-3
: Le mode «&nbsp;3&nbsp;» correspond à «&nbsp;convertir en sfz&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même répertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.sfz&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion, composée de 3 valeurs séparées par le caractère «&nbsp;|&nbsp;». Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est «&nbsp;0|0|0&nbsp;».
  
  * La première valeur est «&nbsp;1&nbsp;» si chaque ensemble doit être préfixé par son numéro d'ensemble, «&nbsp;0&nbsp;» sinon.
  * La deuxième valeur est «&nbsp;1&nbsp;» si un répertoire par banque doit être créé, «&nbsp;0&nbsp;» sinon.
  * La troisième valeur est «&nbsp;1&nbsp;» si la classification General MIDI doit être utilisée pour trier les ensembles, «&nbsp;0&nbsp;» sinon.


### Exemple


```
polyphone -3 -i /adresse/du/fichier.sf3 -c '0|1|1'
```

La conversion crée un répertoire (portant le nom de sortie) contenant les fichiers sfz et un sous-répertoire :file:`samples`. Si ce répertoire existe déjà, un numéro est ajouté à son nom.


## Conversion en .csv {#doc_csv}


### Ligne de commande


```
polyphone -4 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie> -c <configuration>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-4
: Le mode «&nbsp;4&nbsp;» correspond à «&nbsp;convertir en .csv&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même répertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.csv&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion.
  L'option «&nbsp;raw&nbsp;» permet d'exporter des valeurs brutes (telles qu'elles sont enregistrées dans une banque de sons) plutôt que des valeurs ayant des unités physiques et mesurables. Par exemple, la durée de relâche exportée sera de 0,491 (secondes) ou de -1229 dans sa version brute. Par défaut, ce sont les valeurs avec des unités réelles qui sont exportées.


### Exemple


```
polyphone -4 -i /adresse/du/fichier.sf2 -c 'raw'
```

La conversion crée un répertoire (portant le nom de sortie) contenant le fichier :file:`samples.csv` ainsi qu'un fichier par instrument et par ensemble. Si ce répertoire existe déjà, un numéro est ajouté à son nom.


## Conversion en dls {#doc_dls}


### Ligne de commande


```
polyphone -6 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-6
: Le mode «&nbsp;6&nbsp;» correspond à «&nbsp;convertir en dls&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même répertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.dls&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.


### Exemple


```
polyphone -6 -i /adresse/du/fichier.sf2
```

Le fichier est écrit au format DLS niveau 2, les limitations de la conversion étant décrites [ici](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Mode synthé {#doc_synth}


### Ligne de commande


```
polyphone -s -i <fichier/à/lire> -c <configuration>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.

![Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»](images/synth_mode.png "Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»")


### Arguments


-s
: Le mode «&nbsp;s&nbsp;» correspond à «&nbsp;ouvrir Polyphone en mode synthé&nbsp;».

-c
: Configuration du synthé, composée de 3 valeurs séparées du caractère «&nbsp;|&nbsp;». Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est «&nbsp;0|off|off&nbsp;». L'interface graphique permettra ensuite de changer ces paramètres.
  
  * Le premier champ est le numéro du canal MIDI (entre 1 et 16) à écouter pour jouer. La valeur 0 ou «&nbsp;all&nbsp;» indique que tous les canaux sont utilisés et traités comme un seul canal. La valeur -1 ou «&nbsp;each&nbsp;» écoute chaque canal séparément, ce qui constitue un prérequis pour [lire des fichiers MIDI](tutorials/read-midi-file-with-soundfont.md) par exemple.
  * Le deuxième champ est «&nbsp;off&nbsp;» ou «&nbsp;on&nbsp;», rendant possible ou non la sélection de plusieurs ensembles pour jouer.
  * Le troisième champ est «&nbsp;off&nbsp;», «&nbsp;on&nbsp;», ou «&nbsp;toggle&nbsp;», autorisant ou non la sélection des ensembles par les touches correspondant au numéro de l'ensemble (la touche 0 permettant d'activer par exemple l'ensemble 0).


### Exemple


```
polyphone -s -i /adresse/du/fichier.sf2 -c '1|on|toggle'
```


## Autres options {#doc_other}


-h
: Affiche un court message d'aide.

-r
: Réinitialise la configuration de Polyphone (préférences, géométrie des fenêtres, historique, etc.).
  Cela peut être utile en cas de problème.

```
polyphone -r
```
