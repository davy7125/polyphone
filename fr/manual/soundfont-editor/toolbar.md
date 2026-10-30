La barre d'outils, située dans la partie haute de l'éditeur, contient des icônes pour&nbsp;:

* des fonctions communes d'[édition](#doc_edit),
* changer de [vue](#doc_view) dans les éditeurs,
* afficher le [magnétophone](#doc_recorder),
* afficher le [clavier virtuel](#doc_keyboard).


![Barre d'outils](images/toolbar.png "Barre d'outils")


## Fonctions d'édition {#doc_edit}


Les éléments dans la partie gauche de la barre d'outils présentent des fonctions d'édition&nbsp;:

![nouvel échantillon](images/toolbar_sample.png "nouvel échantillon"){.inline} Ajouter un échantillon
: Ajoute de nouveaux [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md) à la banque de sons.
  Une fenêtre de dialogue s'affiche pour sélectionner les fichiers audio (les fichiers :file:`.wav`, :file:`.flac`, :file:`.mp3`, :file:`.ogg`, :file:`.aif`, :file:`.aiff`, :file:`.aifc` et :file:`.snd` peuvent être importés), plusieurs fichiers pouvant être sélectionnés en une seule fois.

![nouvel instrument](images/toolbar_instrument.png "nouvel instrument"){.inline} Ajouter un instrument
: Ajoute un nouvel [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).
  Une fenêtre de dialogue demande le nom de l'instrument et, si des échantillons sont sélectionnés dans l'[arborescence](manual/soundfont-editor/tree.md), propose de les lier au nouvel instrument.
  Dans ce cas, les deux échantillons d'une paire stéréo sont positionnés respectivement à gauche et à droite, la boucle est activée si tous les échantillons sont bouclés et, lorsque les échantillons ont des notes de référence différentes, les étendues de notes sont définies automatiquement (voir l'outil [Auto-positionnement des échantillons](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

![nouvel ensemble](images/toolbar_preset.png "nouvel ensemble"){.inline} Ajouter un ensemble
: Crée un nouvel [ensemble](manual/soundfont-editor/editing-pages/preset-editor.md).
  Une fenêtre de dialogue demande le nom de l'ensemble et, si des instruments sont sélectionnés dans l'arborescence, propose de les lier au nouvel ensemble.
  Les premiers numéros de banque et d'ensemble disponibles sont utilisés.

![outils](images/toolbar_toolbox.png "outils"){.inline} Boîte à outils
: Cliquer sur ce bouton fera apparaître un menu contenant tous les [outils](manual/soundfont-editor/tools/index.md) disponibles pour les éléments sélectionnés dans l'[arborescence](manual/soundfont-editor/tree.md).

![annuler](images/toolbar_undo.png "annuler"){.inline} Annuler
: Annule la dernière modification, également accessible avec :kbd:`Ctrl`+:kbd:`Z`.

![rétablir](images/toolbar_redo.png "rétablir"){.inline} Rétablir
: Restaure la dernière modification annulée, également accessible avec :kbd:`Ctrl`+:kbd:`Y` ou :kbd:`Ctrl`+:kbd:`Maj`+:kbd:`Z`.

![sauvegarder](images/toolbar_save.png "sauvegarder"){.inline} Sauvegarder
: Sauvegarde la banque de sons, également accessible avec :kbd:`Ctrl`+:kbd:`S`.


## Changer de vue {#doc_view}


Dans la partie droite de la barre d'outils, certaines icônes peuvent apparaître selon l'[éditeur](manual/soundfont-editor/editing-pages/index.md) affiché&nbsp;:

* il est possible de basculer entre la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) ![table](images/toolbar_table.png "table"){.inline}, l'[éditeur d'étendues](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) ![étendues](images/toolbar_range.png "étendues"){.inline} et l'[éditeur d'enveloppes](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) ![enveloppe](images/toolbar_adsr.png "enveloppe"){.inline} lors de l'édition d'un instrument,
* il est possible de basculer entre la table ![table](images/toolbar_table.png "table"){.inline} et l'éditeur d'étendues ![étendues](images/toolbar_range.png "étendues"){.inline} lors de l'édition d'un ensemble,
* rien n'est affiché dans les autres cas.

La vue choisie est conservée lorsqu'un autre instrument ou ensemble est sélectionné. La vue affichée pour le premier instrument ou ensemble à l'ouverture d'une banque de sons peut être choisie avec l'option :guilabel:`Vue de démarrage` des [préférences](manual/settings.md#doc_interface).


## Magnétophone {#doc_recorder}


En cliquant sur l'icône ![magnétophone](images/toolbar_recorder.png "magnétophone"){.inline}, une fenêtre s'ouvre présentant le magnétophone, qui peut enregistrer la sortie de Polyphone dans un fichier :file:`.wav`. Le temps écoulé est affiché dans la fenêtre.


![Magnétophone](images/recorder.png "Magnétophone")


Le bouton de gauche peut être dans deux états différents&nbsp;:

Enregistrer
: Commence un nouvel enregistrement si l'acquisition n'est pas en cours.
  Une fenêtre de dialogue demande le nom du fichier qui contiendra le signal audio enregistré.

Arrêter
: Arrête l'acquisition en cours.
  L'acquisition est aussi arrêtée lorsque la fenêtre est fermée.

Le bouton de droite possède également deux états différents (accessibles seulement lorsque l'acquisition est en cours)&nbsp;:

Mettre en pause
: Met en pause l'acquisition en cours.

Reprendre
: Reprend l'acquisition en cours.


## Clavier virtuel {#doc_keyboard}


En cliquant sur l'icône ![piano](images/toolbar_piano.png "piano"){.inline}, une fenêtre s'ouvre présentant le clavier virtuel qui a été conçu pour tester les échantillons, instruments et ensembles.
Cette fenêtre déclenche des signaux MIDI&nbsp;:

* **note on** / **note off** / **aftertouch** à l'aide du clavier de piano,
* d'autres signaux MIDI si la partie **contrôleurs MIDI** est visible.

La note, la vélocité et l'aftertouch de la note jouée sont affichés en bas de la fenêtre, avec une liste déroulante permettant de choisir la taille du clavier (5 octaves, 6 octaves, 88 notes ou 128 notes).
Deux boutons en bas à droite affichent ou masquent les contrôleurs MIDI au-dessus du clavier et les effets audio en dessous.

La fenêtre conserve les proportions du clavier&nbsp;: modifier sa largeur modifie sa hauteur, et inversement.
Afficher ou masquer une zone ne modifie que la hauteur de la fenêtre.


![Clavier virtuel](images/virtual_keyboard.png "Clavier virtuel")


### Clavier de piano


Le clavier de piano déclenche des signaux qui contiennent des numéros de notes et des valeurs de vélocités.
Il peut être piloté de trois manières différentes&nbsp;:

avec la souris
: La note souhaitée peut être cliquée avec la souris.
  La vélocité est déterminée par la position verticale du clic.
  Après un clic, les mouvements suivants au sein de la même touche déclenchent des signaux d'aftertouch.

avec le clavier de l'ordinateur
: Les touches assignées à des notes sont définies dans les [préférences](manual/settings.md#doc_keyboard).
  Elles jouent dès que la fenêtre du clavier virtuel est active, même si un autre élément de cette fenêtre a le focus (un potentiomètre, la liste déroulante…).

en utilisant un clavier MIDI externe
: L'entrée MIDI est définie dans les [préférences](manual/settings.md#doc_general).
  Si le clavier de piano est visible, vous verrez ce que vous êtes en train de jouer.


#### Marqueurs et couleurs


* Lorsqu'un [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) ou un [ensemble](manual/soundfont-editor/editing-pages/preset-editor.md) est sélectionné, la zone non couverte par les divisions est **grisée**.
  Il est ainsi possible de visualiser l'étendue complète d'un instrument ou d'un ensemble.
* Lorsqu'une division d'un instrument ou d'un ensemble est sélectionnée, son étendue est affichée de la même manière que ci-dessus.
  De plus, la note de référence de l'échantillon de la division est identifiée par un **marqueur orange**.
* Lorsqu'une touche du clavier est enfoncée, elle apparaît en **bleu** (ou dans la couleur de sélection choisie dans les [préférences](manual/settings.md#doc_interface)).
  De plus, l'étendue de la division correspondante apparaît en **bleu clair**.
  Ceci ne s'applique qu'au niveau des instruments.
* Le Do central (note 60) est identifié par un **marqueur noir**.


#### Raccourcis utiles


:kbd:`Ctrl`+:kbd:`K`
: Ce raccourci place à tout moment le focus sur le clavier virtuel (le clavier est affiché s'il était masqué).
  Cela permet, lors de l'édition dans une table par exemple, d'utiliser le clavier de l'ordinateur pour jouer au lieu de continuer l'édition d'une cellule.
  Un effet de surbrillance sur le clavier virtuel confirme qu'il détient le focus.

:kbd:`Ctrl`+:kbd:`1`, :kbd:`Ctrl`+:kbd:`2`, … :kbd:`Ctrl`+:kbd:`8`
: Ces raccourcis changent les octaves jouées par le clavier de l'ordinateur.
  Après l'utilisation de :kbd:`Ctrl`+:kbd:`1`, les octaves les plus graves peuvent être jouées.
  Les octaves les plus hautes sont atteintes en utilisant :kbd:`Ctrl`+:kbd:`8`.
  Ils fonctionnent eux aussi dès que la fenêtre du clavier virtuel est active.

:kbd:`→` / :kbd:`←`
: Les flèches droite et gauche jouent la note située juste au-dessus ou juste en dessous de la dernière note jouée, avec la même vélocité.
  Combinées à la touche :kbd:`Maj`, elles sautent 6 notes au lieu d'une.

:kbd:`↑` / :kbd:`↓`
: Les flèches haut et bas jouent la même note que la précédente, mais avec une vélocité plus forte ou plus faible.
  Combinées à la touche :kbd:`Maj`, la vélocité varie de 10 au lieu de 1.

Les flèches ne jouent des notes que si le clavier de piano a le focus&nbsp;: sinon elles agissent sur l'élément qui a le focus, un potentiomètre par exemple.


### Contrôleurs MIDI


En cliquant sur le bouton MIDI en bas à droite, la zone des contrôleurs MIDI est affichée au-dessus du clavier ou masquée.
La fenêtre se déplace alors pour que le clavier reste à la même place à l'écran (ce n'est pas possible avec Wayland sous Linux).
Si vous jouez avec les modulateurs, il est conseillé d'utiliser ces fonctionnalités pour tester votre banque de sons.


![Clavier virtuel avec les contrôleurs MIDI](images/virtual_keyboard_2.png "Clavier virtuel avec les contrôleurs MIDI")


Cette zone est composée de plusieurs éléments&nbsp;:

Pitch Bend
: Le Pitch Bend augmente ou diminue la hauteur de la note.
  Il revient automatiquement à sa position d'origine 0.

Sensibilité
: La sensibilité définit l'étendue des modifications en demi-tons du Pitch Bend.
  La valeur par défaut est 2, soit un ton complet.

Pression
: La pression correspond à la force exercée sur le clavier.
  Elle peut être comparée à l'aftertouch, la différence étant que l'aftertouch est spécifique à une note alors que la pression est un signal global pour toutes les notes.

Potentiomètres
: 3 potentiomètres permettent de modifier la valeur de contrôleurs MIDI.
  Pour chacun des potentiomètres, vous pouvez préciser un numéro de contrôleur puis sa valeur.

Led on / off
: Cette led permet d'assigner la valeur 0 ou 127 à un contrôleur MIDI (utile pour la pédale sustain par exemple).
  Elle est comme un potentiomètre mais avec seulement deux valeurs possibles.

Tous ces éléments peuvent être synchronisés avec un clavier MIDI raccordé à l'ordinateur si l'entrée MIDI est correctement renseignée dans les [préférences](manual/settings.md#doc_general). Les valeurs relatives pour les contrôleurs MIDI sont prises en charge pour piloter les potentiomètres.


### Effets audio {#doc_keyboard_effects}


En cliquant sur le bouton haut-parleur en bas à droite, une zone s'affiche sous le clavier pour régler les principaux paramètres du synthétiseur tout en jouant&nbsp;:

Réverbération
: Une led active ou désactive la réverbération, puis 4 potentiomètres modifient son niveau, sa profondeur, sa densité et son atténuation.
  Le nom d'un potentiomètre s'affiche lorsque la souris le survole.

Chorus
: Une led active ou désactive le chorus, puis 3 potentiomètres modifient son niveau, son amplitude et sa fréquence.

Volume
: Le curseur modifie le volume général.

Ces réglages sont les mêmes que dans les [préférences du son](manual/settings.md#doc_sound)&nbsp;: une modification faite à un endroit est affichée à l'autre.
Une modification est progressive et n'interrompt pas les notes en cours.

*[MIDI]: Musical Instrument Digital Interface