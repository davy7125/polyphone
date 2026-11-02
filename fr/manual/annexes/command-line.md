Polyphone peut être exécuté en ligne de commande pour convertir des banques de sons dans les formats [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) ou [dls](#doc_dls). Les formats de fichiers pris en charge pour la conversion sont [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) et [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Un [fichier MIDI peut être joué](#doc_render) avec une banque de sons et enregistré dans un fichier wav.
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


## Rendu d'un fichier MIDI {#doc_render}


### Ligne de commande


```
polyphone -5 -i <banque/de/sons> -c <fichier/MIDI> -d <répertoire/de/destination> -o <nom_de_sortie>
```

**Note&nbsp;:** avec Windows vous devez écrire l'adresse complète de Polyphone au lieu de simplement écrire :command:`polyphone`, ce qui donne par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


### Arguments


-5
: Le mode «&nbsp;5&nbsp;» correspond à «&nbsp;rendu d'un fichier MIDI&nbsp;»&nbsp;: le fichier MIDI est joué avec la banque de sons et le résultat est écrit dans un fichier wav.

-d
: Répertoire de destination qui recevra le fichier wav.
  Cet argument est optionnel&nbsp;: par défaut le même répertoire que celui qui contient le fichier MIDI est utilisé.

-o
: Nom de sortie sans l'extension du fichier wav (l'extension «&nbsp;.wav&nbsp;» sera ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le nom du fichier MIDI est utilisé.

-c
: Adresse du fichier MIDI à jouer (extension .mid, .midi, .kar ou .rmi).


### Exemple


```
polyphone -5 -i /adresse/du/fichier.sf2 -c /adresse/du/morceau.mid
```

Tous les canaux du fichier MIDI jouent la banque de sons, les ensembles étant choisis par les changements de programme du fichier (banque 128 sur le canal 10, pour les percussions).
Les réglages du synthétiseur sont utilisés&nbsp;: [son](manual/settings.md#doc_sound) (réverbération, chorus, volume général), [polyphonie](manual/settings.md#doc_general) et [accordage](manual/settings.md#doc_keyboard).
Le résultat est un fichier wav stéréo 32 bits à virgule flottante à 48&nbsp;000&nbsp;Hz, qui se termine une fois le son des dernières notes éteint. Un fichier wav existant est remplacé.
Les notes d'un ensemble absent de la banque de sons ne sont pas jouées et un avertissement est affiché pour cet ensemble.
Dans l'interface graphique, un fichier MIDI peut aussi être exporté depuis le [lecteur MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), chaque canal pouvant alors jouer une banque de sons ou un ensemble différent.


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
  
  * Le premier champ est le numéro du canal MIDI (entre 1 et 16) à écouter pour jouer. La valeur 0 ou «&nbsp;all&nbsp;» indique que tous les canaux sont utilisés et traités comme un seul canal. La valeur -1 ou «&nbsp;each&nbsp;» écoute chaque canal séparément, ce qui constitue un prérequis pour jouer la sortie MIDI d'un séquenceur par exemple (un fichier MIDI peut aussi être lu directement avec le [lecteur MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player)).
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
