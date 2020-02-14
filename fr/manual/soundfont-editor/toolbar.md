La barre d'outils, située dans la partie haute de l'éditeur, contient des icônes pour&nbsp;:

* des fonctions communes d'[édition](#doc_edit),
* changer de [vue](#doc_view) dans les éditeurs,
* afficher le [magnétophone](#doc_recorder),
* afficher le [clavier virtuel](#doc_keyboard).


![Barre d'outils](images/toolbar.png "Barre d'outils")


## Fonctions d'édition {#doc_edit}


Les éléments dans la partie gauche de la barre d'outils présentent des fonctions d'édition&nbsp;:

![nouvel échantillon](images/toolbar_sample.png "new sample"){.inline} Nouvel échantillon
: Ajoute de nouveaux [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md) à la banque de sons.
  Une fenêtre de dialogue s'affiche pour sélectionner les fichiers audio (les fichiers :file:`.wav` et :file:`.flac` peuvent être importés).

![nouvel instrument](images/toolbar_instrument.png "new instrument"){.inline} Nouvel instrument
: Ajoute un nouvel [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).

![nouvel ensemble](images/toolbar_preset.png "new preset"){.inline} Nouvel ensemble
: Crée un nouvel [ensemble](manual/soundfont-editor/editing-pages/preset-editor.md).

![outils](images/toolbar_toolbox.png "tools"){.inline} Outils
: Cliquer sur ce bouton fera apparaître un menu contenant tous les [outils](manual/soundfont-editor/tools/index.md) disponibles pour les éléments sélectionnés dans l'[arborescence](manual/soundfont-editor/tree.md).

![annuler](images/toolbar_undo.png "undo"){.inline} Annuler
: Annule la dernière modification.

![refaire](images/toolbar_redo.png "redo"){.inline} Refaire
: Restaure la dernière modification annulée.

![sauvegarder](images/toolbar_save.png "save"){.inline} Sauvegarder le fichier
: Sauvegarde la banque de sons.


## Changer de vue {#doc_view}


Dans la partie droite de la barre d'outils, certaines icônes peuvent apparaître selon quel [éditeur](manual/soundfont-editor/editing-pages/index.md) est affiché&nbsp;:

* basculer entre la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) ![table](images/toolbar_table.png "table"){.inline}, l'[éditeur d'étendues](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) ![étendues](images/toolbar_range.png "étendues"){.inline} et l'[éditeur d'enveloppes](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) ![enveloppe](images/toolbar_adsr.png "enveloppe"){.inline} est possible lors de l'édition d'un instrument,
* basculer entre la table ![table](images/toolbar_table.png "table"){.inline} et l'éditeur d'étendues ![étendues](images/toolbar_range.png "étendues"){.inline} est possible lors de l'édition d'un ensemble,
* rien d'affiché sinon.


## Magnétophone {#doc_recorder}


En cliquant sur l'icône ![magnétophone](images/toolbar_recorder.png "magnétophone"){.inline}, une fenêtre s'ouvre présentant le magnétophone qui peut enregistrer la sortie de Polyphone.


![Magnétophone](images/recorder.png "Magnétophone")


Le bouton de gauche peut être dans deux états différents&nbsp;:

Enregistrer
: Commence un nouvel enregistrement si l'acquisition n'est pas en cours.
  Il est possible ici de spécifier le nom du fichier qui contiendra le signal audio enregistré.

Arrêter
: Arrête l'acquisition en cours.

Le bouton de droite possède également deux états différents (seulement accessibles lorsque l'acquisition est en cours)&nbsp;:

Mettre en pause
: Met en pause l'acquisition en cours.

Reprendre
: Reprend l'acquisition en cours.


## Clavier virtuel {#doc_keyboard}


En cliquant sur l'icône ![piano](images/toolbar_piano.png "piano"){.inline}, une fenêtre s'ouvre présentant le clavier virtuel qui a été conçu pour tester les échantillons, instruments et ensembles.
Cette fenêtre déclenche des signaux MIDI&nbsp;:

* **note on** / **note off** / **aftertouch** à l'aide du clavier de piano,
* d'autres signaux MIDI si la partie **contrôleurs MIDI** est visible.


![Clavier virtuel](images/virtual_keyboard.png "Clavier virtuel")


### Clavier de piano


Le clavier de piano déclenche des signaux qui contiennent des valeurs de notes et de vélocités.
Il peut être piloté de trois manières différentes&nbsp;:

avec la souris
: La note choisie peut être cliquée par la souris.
  La vélocité est déterminée par rapport à la position verticale du clic.
  Après un clic, les mouvements suivants au sein de la même touche déclencheront des signaux aftertouch.

avec le clavier de l'ordinateur
: Les touches assignées à des notes sont définies dans les [préférences](manual/settings.md#doc_keyboard).

en utilisant un clavier MIDI externe
: L'entrée MIDI est définie dans les [préférences](manual/settings.md#doc_general).
  Si le clavier de piano est visible, vous verrez ce que vous êtes en train de jouer.


#### Marqueurs et couleurs


* Lorsqu'un [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) ou un [ensemble](manual/soundfont-editor/editing-pages/preset-editor.md) est sélectionné, la zone non couverte par les divisions est **grisée**.
  Il est ainsi possible de visualiser l'étendue complète d'un instrument ou d'un ensemble.
* Lorsqu'une division d'un instrument ou d'un ensemble est sélectionnée, son étendue est affichée de la même manière que ci-dessus.
  De plus, la note de base d'un échantillon dans la division est identifiée par un **marqueur orange**.
* Lorsqu'une touche est appuyée sur le clavier, elle apparaît en **bleu** (selon la couleur de sélection choisie dans les [préférences](manual/settings.md#doc_interface)).
  De plus, l'étendue de la division correspondante apparaît en **bleu clair**.
  Ceci ne s'applique qu'au niveau des instruments.
* Le Do central (note 60) est identifié par un **marqueur noir**.


#### Raccourcis utiles


:kbd:`Ctrl`+:kbd:`K`
: Ce raccourci place à tout moment le focus sur le clavier virtuel.
  Ceci permet, lors de l'édition dans une table par exemple, d'utiliser l'ordinateur pour jouer au lieu de continuer l'édition d'une cellule.
  Un effet de surbrillance sur le clavier virtuel confirmera qu'il détient le focus.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: Ces raccourcis changent les octaves jouées par le clavier de l'ordinateur.
  Après l'utilisation de :kbd:`Ctrl`+:kbd:`1`, l'octave la plus grave sera jouée.
  L'octave la plus haute sera atteinte en utilisant :kbd:`Ctrl`+:kbd:`8`.

:kbd:`→` / :kbd:`←`
: Les flèches droite et gauche joueront la note juste après ou juste avant la dernière qui a été jouée, en gardant la même vélocité.
  L'association de la touche :kbd:`Shift` sautera des notes.

:kbd:`↑` / :kbd:`↓`
: Les flèches haut et bas joueront la même note que la précédente mais avec une vélocité plus grande ou plus faible.
  L'association de la touche :kbd:`Shift` sautera des vélocités.


### Contrôleurs MIDI


En cliquant sur la flèche en bas à droite, la zone des contrôleurs MIDI peut être affichée ou masquée.
Si vous jouez avec les modulateurs, utiliser ces fonctionnalités est conseillé pour que vous puissiez tester votre banque de sons.


![Clavier virtuel avec les contrôleurs MIDI](images/virtual_keyboard_2.png "Clavier virtuel avec les contrôleurs MIDI")


Cette zone est composée de plusieurs boutons&nbsp;:

Pitch Bend
: Le Pitch Bend augmente ou diminue la hauteur de note.
  Il revient automatiquement à sa position d'origine 0.

Sensibilité
: La sensibilité définit l'étendue des modifications en demi-tons du Pitch Bend.
  La valeur par défaut est 2, soit un ton complet.

Pression
: La pression est la quantité de pression sur le clavier.
  Elle peut être comparée à l'aftertouch, la différence étant que l'aftertouch est spécifique à une note alors que la pression est un signal global pour toutes les notes.

Potentiomètres
: 3 potentiomètres sont ici pour que vous puissiez modifier la valeur de contrôleurs MIDI.
  Pour chacun des potentiomètres vous pouvez préciser un numéro de contrôleur et ensuite sa valeur.

Led on / off
: Cette led permet d'assigner la valeur 0 ou 127 à un contrôleur MIDI (utile pour la pédale sustain par exemple).
  Elle est comme un potentiomètre mais avec seulement deux valeurs possibles.



*[MIDI]: Musical Instrument Digital Interface