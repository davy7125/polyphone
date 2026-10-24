En cliquant sur un instrument ou plus dans l'[arborescence](manual/soundfont-editor/tree.md), l'éditeur d'instruments apparaît.

Cette interface comprend plusieurs parties&nbsp;:

* la [table](#doc_table),
* l'[éditeur d'étendues](#doc_range) quand le bouton :guilabel:`Éditer les étendues` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est cliqué,
* l'[éditeur d'enveloppes](#doc_envelope) quand le bouton :guilabel:`Éditer les enveloppes` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est cliqué,
* la [section modulateurs](#doc_modulator).


![Édition des instruments](images/edit_instrument.png "Édition des instruments")


Si un seul instrument est sélectionné, les paramètres globaux de l'instrument et les paramètres de chacune de ses divisions peuvent être édités.

Dans le cas où plusieurs instruments sont sélectionnés, les paramètres globaux de chacun de ces instruments peuvent être édités.


![Édition de plusieurs instruments](images/edit_multiple_instruments.png "Édition de plusieurs instruments")


## Table {#doc_table}


### Présentation


La table offre une vue globale de la configuration d'un instrument et permet son édition.
Une modification d'un paramètre dans la division :guilabel:`Global` s'applique à toutes les divisions, sauf à celles dont le même paramètre est déjà spécifié (surcharge locale).

Les lignes écrites en italique (:guilabel:`Note de l'échantillon`, :guilabel:`Longueur de l'échantillon` et :guilabel:`Boucle`) ne sont pas éditables&nbsp;: elles rappellent seulement, à titre d'information, les propriétés de l'échantillon lié à chaque division.

Les paramètres éditables sont&nbsp;:

Étendue de notes
: Étendue de notes sur laquelle l'échantillon sera entendu.

Étendue de vélocités
: Étendue de vélocités sur laquelle l'échantillon sera entendu.
  Il est possible de configurer plusieurs sons sur la même note, avec différentes étendues de vélocités, pour distinguer différentes nuances d'une touche de piano par exemple.

Atténuation (dB)
: Atténuation en décibels appliquée à l'échantillon.\
  Note&nbsp;: le format sf2 n'autorise pas cette valeur à être négative au niveau des instruments, aucune amplification n'est ainsi possible.

Balance
: Balance appliquée au son, pour l'entendre à gauche ou à droite en stéréo.

Lecture en boucle
: ![Boucle désactivée](images/loop_off.png "boucle désactivée"){.inline} indique un son reproduit sans boucle.
  ![Boucle activée](images/loop_on.png "boucle activée"){.inline} indique un son reproduit avec une boucle continue.
  ![Relâche](images/sample_mode_release.png "relâche"){.inline} permet de déclencher la lecture d'un échantillon lorsqu'une touche est relâchée. L'enveloppe de volume est toujours utilisée pour définir le volume et une courte attaque (0,2 ms) est ajoutée pour lisser le début de l'échantillon lors de la relâche.
  ![Boucle activée + relâche](images/loop_on_end.png "boucle activée + relâche"){.inline} indique un son reproduit avec une boucle tant que la touche est appuyée et qui continue sa lecture jusqu'à la fin une fois la touche relâchée.\
  Note&nbsp;: le mode de lecture «&nbsp;relâche&nbsp;» n'est PAS documenté par le format sf2 et d'autres synthétiseurs comme FluidSynth peuvent décider de ne pas le prendre en charge.
  Une manière de parvenir au même comportement est de doubler chaque échantillon avec un échantillon contenant un silence très court suivi du son à émettre lors de la relâche.
  L'échantillon ajouté sera lu avec le paramètre «&nbsp;boucle activée + relâche&nbsp;».
  Voir le tutoriel «&nbsp;[Utiliser des relâches personnalisées dans un instrument](tutorials/using-custom-releases-in-an-instrument.md)&nbsp;».

Note de base
: Permet de changer la note de base configurée dans un échantillon par une autre.
  Si par exemple un échantillon possède la note de base 50 et que cet échantillon est lu sur l'étendue de notes [49-51], alors l'échantillon sera lu avec une transposition initiale de -1, 0 et +1 demi-ton sur les notes 49, 50, 51.
  Si à présent la note de base est surchargée à 51 au niveau de l'instrument, alors ce même échantillon sera lu avec une transposition initiale de -2, -1 et 0 demi-ton(s) sur les notes 49, 50, 51.

Accordage (demi-tons et centièmes)
: Modifie l'accordage en demi-tons et en centièmes de demi-ton.
  Cette transposition s'ajoute à la transposition initiale liée à la note déclenchée (par exemple la transposition initiale d'un échantillon dont la note de base est 60 sera +2 demi-tons si la touche appuyée le clavier est 62).

Réglage de tonalité
: Ce paramètre modifie la distance entre deux notes consécutives et modifiera ainsi la transposition initiale liée à la note déclenchée.
  Une valeur de 100 décrit la gamme tempérée&nbsp;: 100 centièmes de demi-tons sont utilisés pour transposer un échantillon d'une touche à l'autre.
  Une valeur de 0 désactive cette transposition initiale et l'échantillon sera ainsi lu à l'identique sur toutes les touches du clavier.

Filtre
: Applique un filtre passe-bas, en spécifiant la fréquence de coupure et la résonance.

Enveloppe de volume
: Modifie l'enveloppe de volume et également la manière dont elle évolue en fonction de la note.

Modulation
: Configure une modulation (enveloppe, délai, fréquence, évolution en fonction de la note) et son effet sur le ton, le filtre et le volume.

Vibrato
: Configure un vibrato (délai, fréquence) et son effet sur le ton.

Classe exclusive
: Ce paramètre peut définir une ou plusieurs classes exclusives, en assignant à un ensemble de divisions au sein d'une même classe une même valeur différente de 0.
  Quand une classe exclusive est définie, une note déclenchée par l'une des divisions de la classe exclusive termine tous les autres sons de la même classe.
  La portée d'une classe exclusive est l'ensemble dans lequel la classe est définie (un son produit dans un instrument terminera le son d'un autre instrument si les deux instruments se trouvent dans le même ensemble).
  Cet effet peut être utilisé pour des instruments de type percussion comme une cymbale.
  En résumé, jouer une note en termine automatiquement une autre dans le même groupe.

Chorus et réverbération
: Indique l'intensité du chorus et de la réverbération.
  Ces effets sont configurables dans les [préférences](manual/settings.md#doc_sound) du logiciel.

Note fixe et vélocité fixe
: Force la valeur de la note et de la vélocité sur toute l'étendue de la division.

Offsets
: Change les positions de début et de fin d'un son pendant la lecture, ainsi que les positions de début et fin de la boucle.
  Un offset s'exprime en nombre de points.


### Facilités d'édition dans la table


Pour faciliter et accélérer l'édition dans une table, les fonctionnalités suivantes ont été implémentées&nbsp;:

* la molette de la souris peut être utilisée pour modifier une valeur (la cellule doit d'abord se trouver en mode édition),
* un clavier MIDI externe (ou le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard)) peut être utilisé pour définir une note de base, une étendue de notes et une étendue de vélocités (la cellule doit d'abord se trouver en mode édition),
* une sélection multiple, suivie de l'édition de la dernière cellule, édite toutes les cellules simultanément,
* les touches :kbd:`Suppr` et :kbd:`Retour arrière` effacent le contenu des cellules sélectionnées,
* les divisions stéréo peuvent être éditées simultanément si l'option est cochée dans les [préférences](manual/settings.md#doc_general) du logiciel,
* les notes peuvent être nommées par un nombre ou un texte (le Do central pouvant être Do3, Do4 ou Do5), selon l'option choisie dans les [préférences](manual/settings.md#doc_interface),
* copier / coller / couper un ensemble de cellules (pas forcément adjacentes) est possible avec :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X`.

N'oubliez pas les [outils](manual/soundfont-editor/tools/instrument-tools.md) disponibles&nbsp;!
En particulier l'outil permettant de [paramétrer en fonction des notes](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Rendre des divisions muettes {#doc_solo}


À des fins de test, il est possible de rendre muette une division ou de rendre muettes toutes les divisions sauf une dans les tables.
Les options sont accessibles par un clic droit sur l'entête d'une division ainsi que dans le menu contextuel de l'[arborescence](manual/soundfont-editor/tree.md#doc_menu).


![Rendre des divisions muettes dans une table](images/table_mute_divisions.png "Rendre des divisions muettes dans une table")


## Éditeur d'étendues {#doc_range}


L'éditeur d'étendues vous permet de disposer rapidement et graphiquement les échantillons selon des étendues de notes et de vélocités.
Le graphique met en avant certains problèmes de configuration, comme des zones non couvertes par des échantillons ou des superpositions non souhaitées.


![Éditeur d'étendues](images/edit_range.png "Éditeur d'étendues")


Chaque échantillon est représenté par un rectangle semi-transparent, de manière à ce que la superposition de plusieurs éléments soit identifiable.


### Sélection de rectangles


Un simple clic gauche sur un rectangle le sélectionne et enlève toutes les autres sélections.
Si plusieurs rectangles sont superposés, des clics consécutifs changent la sélection sous le curseur de la souris.
Un double-clic sur un rectangle ouvre l'[échantillon](manual/soundfont-editor/editing-pages/sample-editor.md) correspondant (ou l'instrument correspondant dans l'[éditeur d'ensembles](manual/soundfont-editor/editing-pages/preset-editor.md)).

Une sélection multiple est possible&nbsp;:

* en maintenant la touche :kbd:`Ctrl` enfoncée puis en cliquant sur les différents rectangles pour les sélectionner un par un,
* en utilisant la touche :kbd:`Maj`&nbsp;: tous les rectangles entre le premier rectangle cliqué et le suivant sont sélectionnés.


### Modifier les rectangles


Après avoir sélectionné au moins un rectangle, il est possible avec le bouton gauche de la souris de&nbsp;:

* changer la taille, en saisissant un des côtés d'un rectangle&nbsp;;
* changer la position, en pointant au centre d'un rectangle.

Si plusieurs rectangles sont sélectionnés, tous les changements sont effectués simultanément.
La touche :kbd:`Échap` annule un changement en cours.


### Déplacement et zoom


Maintenir le clic droit permet de modifier le zoom autour du point de départ.

Maintenir le clic gauche permet de déplacer le graphique si le point initial n'est pas sur un rectangle.

Un clic sur le bouton du milieu joue une note&nbsp;: la note est donnée par la position horizontale de la souris et la vélocité par sa position verticale.


## Éditeur d'enveloppes {#doc_envelope}


L'éditeur d'enveloppes fournit un aperçu graphique des 8 paramètres définissant l'enveloppe de volume et des 8 paramètres définissant l'enveloppe de modulation.
C'est ainsi un outil permettant de mieux comprendre l'impact de chacun de ces paramètres.


![Éditeur d'enveloppes](images/edit_envelope.png "Éditeur d'enveloppes")


Les boutons au-dessus du graphique sélectionnent alternativement l'enveloppe de volume (affichée en vert) ou l'enveloppe de modulation (affichée en rouge) pour l'édition.
Pendant l'édition de l'enveloppe de modulation, l'enveloppe de volume est affichée en trait fin.

La partie gauche du graphique représente l'évolution de l'enveloppe lorsqu'une touche est **appuyée**.
7 des 8 paramètres sont impliqués dans cette représentation&nbsp;:

Délai
: Durée pendant laquelle l'enveloppe reste à 0 juste après le déclenchement d'une touche. Dans la majorité des cas, on souhaite entendre le son immédiatement après avoir pressé une touche et ce paramètre reste donc à 0. Cependant le **délai** peut être utile dans le cas d'un son évoluant au cours du temps, construit à partir de plusieurs couches sonores. Supposons que l'on souhaite un second son introduit 0,5 s après l'appui sur une touche, on entrera donc «&nbsp;0,5&nbsp;» dans ce champ. Il est à noter qu'un délai ne retarde pas la lecture du son — il annule simplement son volume. Après avoir mis un délai de 0,5 s, le son sera entendu après une durée de 0,5 s pendant laquelle il aura été lu en silence.

Attaque
: Durée après le **délai** pendant laquelle l'enveloppe passe de 0 à sa valeur maximale. Par exemple, un son de corde frottée comme un violon possède une attaque relativement longue, alors qu'un son de corde frappée comme une guitare a une attaque courte. Même lorsque l'on souhaite utiliser une attaque aussi courte que possible, il est conseillé d'inscrire une valeur d'au moins 0,001, sans quoi un «&nbsp;clic&nbsp;» pourrait se faire entendre après avoir appuyé sur une touche.

Maintien
: Durée après l'**attaque** pendant laquelle l'enveloppe se maintient à sa valeur maximale.

Déclin
: Durée après le **maintien** pendant laquelle l'enveloppe passe progressivement de sa valeur maximale à la valeur spécifiée dans le **soutien**. Lorsqu'aucune réduction n'est spécifiée dans le **soutien** (0 dB, valeur par défaut), le volume pendant le **déclin** reste à son maximum quelle que soit la valeur renseignée.

Soutien
: Valeur maintenue après le **déclin** tant que la touche enfoncée n'est pas relâchée. Tout comme l'**atténuation**, la valeur de soutien s'exprime en réduction de décibels. Dans le cas où l'on souhaite diminuer le son de moitié après la phase de déclin, une valeur de 6 dB devra être renseignée. Le nombre «&nbsp;144&nbsp;» peut être renseigné si l'on souhaite couper complètement le son.

Note → Maintien
: Modification de la durée de **maintien** en fonction de la note (voir ci-dessous).

Note → Déclin
: Modification de la durée de **déclin** en fonction de la note (voir ci-dessous).

Une valeur positive dans **Note → XXX** réduira progressivement la durée de XXX vers les aigus, alors qu'une valeur négative fera l'inverse.
Par exemple&nbsp;:

* la valeur 100 réduit de moitié la durée de XXX pour chaque octave au-dessus du Do central (60) et double la durée de XXX pour chaque octave en dessous du Do central,
* la valeur -100 double la durée de XXX pour chaque octave au-dessus du Do central (60) et réduit de moitié la durée de XXX pour chaque octave en dessous du Do central.

Ces paramètres peuvent être utiles lors de la conception d'une guitare ou d'un piano avec la pédale sustain enfoncée&nbsp;: plus le son est grave et plus il dure avant de s'estomper entièrement.

La partie droite représente l'évolution de l'enveloppe quand une touche est **relâchée**.
Seule la durée de **relâche** est impliquée dans cette représentation.
L'enveloppe retourne progressivement à la valeur 0 pendant cette durée.
Les sons de violons ou de pads ont une durée de relâche relativement longue, alors que d'autres sons comme ceux de l'orgue s'arrêtent plus brutalement. Dans la pratique, même pour des relâches courtes, il est conseillé d'utiliser une valeur d'au moins 0,2 s car une valeur plus faible ne donne pas un rendu très plaisant ou naturel (mais cela dépend de l'instrument que l'on souhaite créer).

Lorsqu'une seule division d'un instrument est sélectionnée dans l'arborescence, le son de l'[échantillon](manual/soundfont-editor/editing-pages/sample-editor.md) correspondant est affiché en fond.
Il est ainsi possible de mieux visualiser l'évolution de l'enveloppe par rapport à l'échantillon en train d'être joué.


## Modulateurs {#doc_modulator}


La partie basse de l'éditeur des instruments est dédiée à la création des modulateurs, de manière globale pour l'instrument entier ou pour une division particulière.
Lorsque vous jouez d'un instrument, un modulateur écoute un ou deux signaux MIDI et modifie l'un des paramètres de la table selon la règle mathématique configurée.
La sortie d'un modulateur peut aussi être utilisée comme entrée d'un autre modulateur (cette possibilité est apparue avec la version 2.04 du [format sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) et peut ne pas être prise en charge par tous les synthétiseurs).


![Édition des modulateurs](images/modulator_editor_2.png "Édition des modulateurs")


Les boutons à gauche sont prévus pour&nbsp;:

* afficher / masquer la zone des modulateurs,
* ajouter / supprimer un modulateur,
* copier / coller / dupliquer un modulateur ou plus.

Lorsqu'un modulateur est ajouté, la bordure de la cellule correspondante dans la table s'épaissit (comme la cellule d'accordage dans la colonne :guilabel:`Global` ci-dessus).

Les modulateurs d'une division (ou de la division globale) ne sont affichés que si une seule division est sélectionnée.
Lorsque la section est masquée, un résumé indique le nombre de modulateurs et les paramètres qu'ils modifient.
La sélection de cellules dans la table sélectionne les modulateurs agissant sur les paramètres correspondants, et inversement.

Chaque modulateur est composé des éléments suivants, de gauche à droite&nbsp;:

Sources
: La première source est un signal MIDI&nbsp;: la vélocité ou la note jouée, la pression polyphonique, la pression globale, le Pitch Bend, la sensibilité du Pitch Bend ou un contrôleur MIDI (:guilabel:`CC`).
  Ce peut aussi être la sortie d'un ou plusieurs autres modulateurs (:guilabel:`Lien`), ceux qui ont ce modulateur pour destination.
  La seconde source est optionnelle et modifie l'effet de la première.

Courbes
: À côté de chaque source, un bouton donne accès à une grille de courbes pour choisir la manière dont le signal est converti&nbsp;: linéaire, concave, convexe ou commutateur, ascendante ou descendante, unipolaire (de 0 à 1) ou bipolaire (de -1 à 1).

Quantité
: Amplitude maximale de la modulation.

Transformation
: :guilabel:`aucune opération` ou :guilabel:`valeur absolue` (la valeur absolue du résultat de la modulation est utilisée).

Destination
: Le paramètre modifié par le modulateur, ou un autre modulateur (:guilabel:`Modulateur`).
  Le texte sous la destination indique l'étendue de la modulation&nbsp;: les valeurs ajoutées au paramètre (:guilabel:`Ajout de` … :guilabel:`À`) ou les facteurs qui lui sont appliqués (:guilabel:`Multiplication de` … :guilabel:`À`).

Lorsqu'un modulateur est identique à un modulateur situé plus haut dans la liste, la mention :guilabel:`surchargé par` est affichée.
Au niveau des instruments, la mention :guilabel:`surcharge un mod. par défaut` indique un modulateur qui remplace l'un des [modulateurs par défaut](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) du format sf2, et la mention :guilabel:`désactive un mod. par défaut` indique que sa quantité vaut 0 (le modulateur par défaut est alors désactivé).
