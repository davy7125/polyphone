Polyphone peut être exécuté en ligne de commande pour convertir des banques de sons dans les formats [sf2](#doc_sf2), [sf3](#doc_sf3), [sfz](#doc_sfz), [csv](#doc_csv) ou [dls](#doc_dls). Les formats de fichiers pris en charge pour la conversion sont [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [dls](manual/annexes/the-different-soundfont-formats.md#doc_dls), [sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) et [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).

Un [fichier MIDI peut être joué](#doc_render) avec une banque de sons et enregistré dans un fichier wav.
Des options peuvent également être appelées pour ouvrir Polyphone en [mode synthé](#doc_synth) ou avec le [serveur MCP](#doc_mcp) démarré.

Des fichiers et des répertoires peuvent également être passés en arguments pour les ouvrir dans l'interface graphique, par exemple :command:`polyphone fichier1.sf2 fichier2.sf3 /adresse/du/répertoire` (un répertoire est ouvert dans l'[explorateur de banques de sons](manual/soundfont-explorer.md)).
Si Polyphone est déjà ouvert, les fichiers sont ouverts dans l'instance en cours.
Quelques [autres options](#doc_other) permettent de réinitialiser la configuration ou d'afficher l'aide et la version.

La liste de toutes les options est affichée avec :command:`polyphone --help`.
L'ordre des arguments n'a pas d'importance.
Les options des [versions précédentes](#doc_legacy) sont toujours acceptées.

**Note&nbsp;:** sous Windows, il est nécessaire d'écrire le chemin complet de Polyphone au lieu de simplement :command:`polyphone`, qui est par exemple :command:`C:/Program files/Polyphone/polyphone.exe`.


## Conversion {#doc_conversion}


### Ligne de commande


```
polyphone --convert <format> <fichiers/à/convertir> -d <répertoire/de/destination> -o <fichier/de/sortie> -f
```


### Arguments


--convert
: Format de la conversion&nbsp;: «&nbsp;sf2&nbsp;», «&nbsp;sf3&nbsp;», «&nbsp;sfz&nbsp;», «&nbsp;csv&nbsp;» ou «&nbsp;dls&nbsp;», les options propres à chaque format étant décrites ci-dessous.
  Cet argument est optionnel si un fichier de sortie est indiqué avec son extension, par exemple :command:`polyphone fichier.sf2 -o fichier.sf3`.

-d, --output-dir
: Répertoire de destination, qui doit exister.
  Cet argument est optionnel&nbsp;: par défaut le répertoire de chaque fichier d'entrée est utilisé.

-o, --output
: Fichier de sortie, avec ou sans son extension (l'extension du format est ajoutée automatiquement).
  Cet argument est optionnel&nbsp;: par défaut le fichier de sortie porte le nom du fichier d'entrée.
  Un nom seul est écrit dans le répertoire de destination, un chemin relatif part du répertoire courant (ou du répertoire de destination si -d est utilisé).
  Cet argument ne peut pas être utilisé si plusieurs fichiers sont convertis.

-f, --force
: Remplace un fichier de sortie existant.
  Sans cet argument, la conversion échoue si le fichier de sortie existe déjà.
  Le fichier d'entrée peut être remplacé par sa conversion, par exemple :command:`polyphone --convert sf2 fichier.sf2 -f` réécrit :file:`fichier.sf2`.

Plusieurs fichiers peuvent être convertis en une seule commande, chaque fichier de sortie portant le nom de son fichier d'entrée.
En cas d'échec, les autres fichiers sont tout de même convertis.


### Exemples


```
polyphone --convert sf2 /adresse/du/fichier.sfArk
polyphone /adresse/du/fichier.sf2 -o /adresse/du/fichier_converti.sf3
polyphone --convert sf3 *.sf2 -d /adresse/du/répertoire
```


## Conversion en sf2 {#doc_sf2}


Aucune option particulière n'est utilisée pour cette conversion.

```
polyphone --convert sf2 /adresse/du/fichier.dls
```


## Conversion en sf3 {#doc_sf3}


--quality
: Qualité de compression&nbsp;: «&nbsp;low&nbsp;» (basse), «&nbsp;medium&nbsp;» (moyenne) ou «&nbsp;high&nbsp;» (haute).
  Cet argument est optionnel&nbsp;: par défaut la valeur utilisée est «&nbsp;medium&nbsp;».

```
polyphone --convert sf3 /adresse/du/fichier.sf2 --quality high
```


## Conversion en sfz {#doc_sfz}


--preset-prefix
: Chaque ensemble est préfixé par son numéro d'ensemble.

--bank-dir
: Un répertoire par banque est créé.

--gm-sort
: La classification General MIDI est utilisée pour trier les ensembles.

```
polyphone --convert sfz /adresse/du/fichier.sf3 --bank-dir --gm-sort
```

La conversion crée un répertoire (portant le nom du fichier de sortie) contenant les fichiers sfz et un sous-répertoire :file:`samples`.
Si ce répertoire existe déjà, un numéro est ajouté à son nom&nbsp;: rien n'est remplacé, même avec -f.


## Conversion en .csv {#doc_csv}


--raw
: Les valeurs brutes sont exportées (telles qu'elles sont enregistrées dans une banque de sons) plutôt que des valeurs ayant des unités physiques et mesurables.
  Par exemple, la durée de relâche exportée sera de 0,491 (secondes) ou de -1229 dans sa version brute.
  Par défaut, ce sont les valeurs avec des unités réelles qui sont exportées.

```
polyphone --convert csv /adresse/du/fichier.sf2 --raw
```

La conversion crée un répertoire (portant le nom du fichier de sortie) contenant le fichier :file:`samples.csv` ainsi qu'un fichier par instrument et par ensemble.
Si ce répertoire existe déjà, un numéro est ajouté à son nom&nbsp;: rien n'est remplacé, même avec -f.


## Conversion en dls {#doc_dls}


Aucune option particulière n'est utilisée pour cette conversion.

```
polyphone --convert dls /adresse/du/fichier.sf2
```

Le fichier est écrit au format DLS niveau 2, les limites de la conversion étant décrites [ici](manual/annexes/the-different-soundfont-formats.md#doc_dls).


## Rendu d'un fichier MIDI {#doc_render}


### Ligne de commande


```
polyphone --render <fichier/MIDI> <banque/de/sons> -d <répertoire/de/destination> -o <fichier/de/sortie> -f
```


### Arguments


--render
: Adresse du fichier MIDI à jouer (extension .mid, .midi, .kar ou .rmi)&nbsp;: le fichier MIDI est joué avec la banque de sons et le résultat est écrit dans un fichier wav.

-d, --output-dir
: Répertoire de destination qui recevra le fichier wav.
  Cet argument est optionnel&nbsp;: par défaut le répertoire du fichier MIDI est utilisé.

-o, --output
: Fichier wav de sortie, avec ou sans son extension.
  Cet argument est optionnel&nbsp;: par défaut le fichier wav porte le nom du fichier MIDI.

-f, --force
: Remplace un fichier wav existant.


### Exemple


```
polyphone --render /adresse/du/morceau.mid /adresse/du/fichier.sf2
```

Tous les canaux du fichier MIDI jouent la banque de sons, les ensembles étant choisis par les changements de programme du fichier (banque 128 sur le canal 10, pour les percussions).
Les réglages du synthétiseur sont utilisés&nbsp;: [son](manual/settings.md#doc_sound) (réverbération, chorus, volume général), [polyphonie](manual/settings.md#doc_general) et [accordage](manual/settings.md#doc_keyboard).
Le résultat est un fichier wav stéréo 32 bits à virgule flottante à 48&nbsp;000&nbsp;Hz, qui se termine une fois le son des dernières notes éteint.
Les notes d'un ensemble absent de la banque de sons ne sont pas jouées et un avertissement est affiché pour cet ensemble.
Dans l'interface graphique, un fichier MIDI peut aussi être exporté depuis le [lecteur MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), chaque canal pouvant alors jouer une banque de sons ou un ensemble différent.


## Mode synthé {#doc_synth}


### Ligne de commande


```
polyphone --synth <fichier/à/lire> --channel <canal> --multiple-selection --key-selection <mode>
```

![Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»](images/synth_mode.png "Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»")


### Arguments


-s, --synth
: Polyphone est ouvert en mode synthé.

--channel
: Numéro du canal MIDI (entre 1 et 16) à écouter pour jouer.
  La valeur «&nbsp;all&nbsp;» indique que tous les canaux sont utilisés et traités comme un seul canal (valeur par défaut).
  La valeur «&nbsp;each&nbsp;» écoute chaque canal séparément, ce qui constitue un prérequis pour jouer la sortie MIDI d'un séquenceur par exemple (un fichier MIDI peut aussi être lu directement avec le [lecteur MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player)).

--multiple-selection
: Plusieurs ensembles peuvent être sélectionnés pour jouer.

--key-selection
: Les ensembles sont sélectionnés par les touches correspondant au numéro de l'ensemble (la touche 0 permettant d'activer par exemple l'ensemble 0), avec la valeur «&nbsp;on&nbsp;».
  Avec la valeur «&nbsp;toggle&nbsp;», une touche désélectionne aussi son ensemble.

Ces arguments sont optionnels, et l'interface graphique permettra ensuite de changer ces réglages.


### Exemple


```
polyphone --synth /adresse/du/fichier.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## Serveur MCP {#doc_mcp}


### Ligne de commande


```
polyphone --mcp <fichiers/à/ouvrir> --mcp-port <port>
```


### Arguments


--mcp
: Le serveur MCP, qui permet à un assistant IA de parcourir, modifier et analyser les banques de sons ouvertes, est démarré pour cette session même s'il est désactivé dans les [préférences](manual/settings.md).
  L'adresse du serveur est affichée dans la console.
  Une nouvelle instance de Polyphone est ouverte, même si Polyphone est déjà ouvert.

--mcp-port
: Port du serveur MCP (cet argument implique --mcp).
  Cet argument est optionnel&nbsp;: par défaut le port des préférences est utilisé (8642).

Modifier les réglages du serveur MCP dans l'interface graphique remplace ces options.


### Exemple


```
polyphone --mcp-port 8700 /adresse/du/fichier.sf2
```


## Autres options {#doc_other}


-h, --help
: Affiche la liste de toutes les options.

-v, --version
: Affiche la version de Polyphone.

-r, --reset-config
: Réinitialise la configuration de Polyphone (préférences, géométrie des fenêtres, historique, etc.).
  Ceci peut être utile en cas de problème.

```
polyphone --reset-config
```


## Messages et codes retournés {#doc_exit}


Le chargement et l'écriture sont consignés dans la console, les erreurs étant écrites dans la sortie d'erreur.
Les messages et l'aide sont dans la langue choisie dans les [préférences](manual/settings.md).

Le code retourné est&nbsp;:

0
: Succès.

1
: Un fichier d'entrée ou le répertoire de destination n'existe pas, ou le fichier de sortie existe déjà (sans -f).

2
: Les arguments ne sont pas valides&nbsp;: option inconnue, valeur incorrecte, option non utilisée avec le mode choisi, format de fichier d'entrée non pris en charge, etc.

3
: Un fichier d'entrée ne peut pas être lu (banque de sons, fichier MIDI).

4
: La sortie ne peut pas être écrite.

Quand plusieurs fichiers sont convertis, le code retourné est celui du premier échec.


## Options des versions précédentes {#doc_legacy}


Les options des versions précédentes de Polyphone sont toujours acceptées&nbsp;:

-1, -2, -3, -4, -6
: Conversion en sf2, sf3, sfz, csv et dls, comme avec --convert.

-5
: Rendu d'un fichier MIDI, dont l'adresse est indiquée avec -c.

-i
: Fichier d'entrée, qui peut aussi être écrit sans cette option.

-c
: Configuration, composée de valeurs séparées par le caractère «&nbsp;|&nbsp;»&nbsp;:
  
  * conversion en sf3&nbsp;: qualité de compression, «&nbsp;0&nbsp;» pour basse, «&nbsp;1&nbsp;» pour moyenne, «&nbsp;2&nbsp;» pour haute&nbsp;;
  * conversion en sfz&nbsp;: 3 valeurs, «&nbsp;1&nbsp;» ou «&nbsp;0&nbsp;», pour --preset-prefix, --bank-dir et --gm-sort, par exemple «&nbsp;0|1|1&nbsp;»&nbsp;;
  * conversion en csv&nbsp;: «&nbsp;raw&nbsp;» pour --raw&nbsp;;
  * rendu d'un fichier MIDI (-5)&nbsp;: adresse du fichier MIDI&nbsp;;
  * mode synthé&nbsp;: canal («&nbsp;all&nbsp;», «&nbsp;each&nbsp;» ou 1 à 16, 0 équivalant à «&nbsp;all&nbsp;» et -1 à «&nbsp;each&nbsp;»), sélection multiple («&nbsp;on&nbsp;» ou «&nbsp;off&nbsp;») et sélection par les touches («&nbsp;on&nbsp;», «&nbsp;off&nbsp;» ou «&nbsp;toggle&nbsp;»), par exemple «&nbsp;all|on|toggle&nbsp;».

```
polyphone -2 -i /adresse/du/fichier.sf2 -c 2
polyphone -s -i /adresse/du/fichier.sf2 -c 'all|on|toggle'
```
