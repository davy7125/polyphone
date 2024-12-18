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
: Mode «&nbsp;1&nbsp;» est «&nbsp;convertir en sf2&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même repertoire que celui qui contient le fichier d'entrée est utilisé.

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

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui nous donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-2
: Mode «&nbsp;2&nbsp;» est «&nbsp;convertir en sf3&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même repertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.sf3&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion.
  Il est possible to spécifier la qualité de compression&nbsp;: «&nbsp;0&nbsp;» pour basse, «&nbsp;1&nbsp;» pour moyenne, «&nbsp;2&nbsp;» pour haute.
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

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui nous donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-3
: Mode «&nbsp;3&nbsp;» est «&nbsp;convertir en sfz&nbsp;».

-d
: Répertoire de destination qui recevra la conversion du fichier d'entrée.
  Cet argument est optionnel&nbsp;: par défaut le même repertoire que celui qui contient le fichier d'entrée est utilisé.

-o
: Nom de sortie sans l'extension du fichier converti (l'extension «&nbsp;.sfz&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier d'entrée est utilisé.

-c
: Configuration de la conversion sur 3 caractères. Le premier caractère est «&nbsp;1&nbsp;» si chaque ensemble doit être préfixé par son numéro d'ensemble, «&nbsp;0&nbsp;» sinon. Le deuxième caractère est «&nbsp;1&nbsp;» si un répertoire par banque doit être créé, «&nbsp;0&nbsp;» sinon. Le troisième caractère est «&nbsp;1&nbsp;» si la classification General MIDI doit être utilisée pour trier les ensembles, «&nbsp;0&nbsp;» sinon.
  Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est «&nbsp;000&nbsp;».


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

![Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»](images/synth_mode.png "Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»")


### Arguments


-s
: Mode «&nbsp;s&nbsp;» est «&nbsp;ouvrir Polyphone en mode synthé&nbsp;».

-c
: Configuration du synthé. Le champ comprend 3 champs séparés du caractère «&nbsp;|&nbsp;». Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est «&nbsp;0|off|off&nbsp;». L'interface graphique permettra ensuite de changer ces paramètres.
  * Le premier champ est le numéro du canal MIDI (entre 1 et 16) à écouter pour jouer, ou 0 pour indiquer que tous les canaux seront utilisés.
  * Le deuxième champ est «&nbsp;off&nbsp;» ou «&nbsp;on&nbsp;», rendant possible ou non la sélection de plusieurs ensembles pour jouer.
  * Le troisième champ est «&nbsp;off&nbsp;», «&nbsp;on&nbsp;», ou «&nbsp;toggle&nbsp;», autorisant ou non la sélection des ensembles par les touches correspondant au numéro de l'ensemble (la touche 0 permettant d'activer par exemple l'ensemble 0).


### Exemple


```
polyphone -s -i /adresse/du/fichier.sf2 -c '1|on|toggle'
```
