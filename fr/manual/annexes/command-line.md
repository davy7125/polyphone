Polyphone peut être exécuté en ligne de commande pour convertir des banques de sons dans les formats [sf2](#doc_sf2), [sf3](#doc_sf3) ou [sfz](#doc_sfz). Les formats de fichiers supportés pour la conversion sont [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) et [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Une option peut être appelée pour ouvrir Polyphone en [mode synthé](#doc_synth).


## Conversion en sf2 {#doc_sf2}


### Ligne de commande


```
polyphone -1 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui nous donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-1
: Mode “1” est “convertir en sf2”.

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même repertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension “.sf2” sera ajoutée automatiquement).
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

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui nous donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-2
: Mode “2” est “convertir en sf3”.

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même repertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension “.sf3” sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion.
  Il est possible to spécifier la qualité de compression&nbsp;: “0” pour basse, “1” pour moyenne, “2” pour haute.
  Cet argument est optionnel&nbsp;: par défaut le paramètre “1” (qualité moyenne) est utilisé.


### Exemple


```
polyphone -2 -i /adresse/du/fichier.sf2 -c 2
```


## Conversion en sfz {#doc_sfz}


### Ligne de commande


```
polyphone -3 -i <fichier/à/convertir> -d <répertoire/de/destination> -o <nom_de_sortie> -c <configuration>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui nous donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-3
: Mode “3” est “convertir en sfz”.

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même repertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension “.sfz” sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion sur 3 caractères. Le premier caractère est “1” si chaque ensemble doit être préfixé par son numéro d'ensemble, “0” sinon. Le deuxième caractère est “1” si un répertoire par banque doit être créé, “0” sinon. Le troisième caractère est “1” si la classification General MIDI doit être utilisée pour trier les ensembles, “0” sinon.
  Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est “000”.


### Exemple


```
polyphone -3 -i /adresse/du/fichier.sf3 -c 011
```


## Mode synthé {#doc_synth}


### Ligne de commande


```
polyphone -s -i <fichier/à/lire> -c <configuration>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui nous donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-s
: Mode “s” est “ouvrir Polyphone en mode synthé”.

-c
: Configuration du synthé. Le champ comprend 3 champs séparés du caractère “|”. Le premier champ est le numéro du canal MIDI (entre 1 et 16) à écouter pour jouer, ou 0 pour indiquer que tous les canaux seront utilisés. Le deuxième champ est “off” ou “on”, rendant possible ou non la sélection de plusieurs ensembles pour jouer. Le troisième champ est “off”, “on”, ou “toggle”, autorisant ou non la sélection des ensembles par les touches correspondant au numéro de l'ensemble (la touche 0 permettant d'activer par exemple l'ensemble 0).
  Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est “0|off|off”. L'interface graphique permettra ensuite de changer ces paramètres.


### Exemple


```
polyphone -s -i /adresse/du/fichier.sf2 -c '1|on|toggle'
```
