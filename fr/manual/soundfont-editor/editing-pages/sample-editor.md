En cliquant sur un échantillon ou plus dans l'[arborescence](manual/soundfont-editor/tree.md), l'éditeur des échantillons apparaît.

Vous y trouverez plusieurs parties&nbsp;:

* un [graphique](#doc_graph) représentant l'échantillon,
* une zone d'[informations](#doc_info),
* une analyse des [fréquences](#doc_freq),
* un [égaliseur](#doc_eq),
* un [lecteur](#doc_player) d'échantillon.


![Éditeur des échantillons](images/edit_sample.png "Éditeur des échantillons")


## Le graphique {#doc_graph}


### Zoom & drag


Le graphique permet la visualisation de l'onde sonore.
Il est possible de zoomer verticalement et horizontalement en maintenant appuyé le bouton droite de la souris et en la déplaçant&nbsp;:

* un mouvement vers la droite dilate l'axe horizontal,
* un mouvement vers la gauche contracte l'axe horizontal,
* un mouvement vers le haut dilate l'axe vertical,
* un mouvement vers le bas contracte l'axe vertical.

Maintenir le bouton gauche de la souris permet un mouvement horizontal du graphique, si l'axe horizontal a été dilaté.


### Positions de boucle et de lecture


Une barre verticale verte indique la position de départ de la boucle (modifiable par un clic gauche) et une barre verticale rouge indique la position de fin de la boucle (modifiable par un clic droit).
Une ligne en pointillé représentant une portion d'onde est associée à chaque barre verticale, ce qui est utile pour visualiser la superposition du début et de la fin de la boucle.

Pendant la lecture, une barre verticale blanche parcourt le graphique et indique la position de lecture en temps réel.


### Couper l'échantillon


En maintenant la touche :kbd:`Alt`, il est possible de définir une zone à couper&nbsp;:

* un appui du bouton gauche de la souris définit le début de la zone,
* la fin de l'appui du bouton de gauche définit la fin de la zone.

Une fois la zone définie, une fenêtre de dialogue apparaît pour demander une confirmation avant de couper l'échantillon.


![Couper un échantillon](images/cutting_sample.png "Couper un échantillon")


### Couleurs du graphique


Les couleurs du graphique sont éditables dans les [préférences](manual/settings.md#doc_interface) du logiciel.


## Section «&nbsp;Information&nbsp;» {#doc_info}


La section «&nbsp;Information&nbsp;» permet l'édition des informations suivantes&nbsp;:

* début et fin de la boucle,
* note de base d'un échantillon et sa correction à appliquer pour que le son soit accordé dans le tempérament égal,
* fréquence d'échantillonnage du son (une modification déclenchera un ré-échantillonnage),
* type de son (non lié&nbsp;: mono, sinon gauche, droite ou lié),
* l'échantillon lié si l'échantillon est stéréo.

Par ailleurs, la taille de l'échantillon en nombre de valeurs et en secondes est affiché.

Un bouton édite les positions de début et de fin de la boucle de manière à ce que la boucle représente l'échantillon entier.
Un autre bouton édite la note de base et la correction de sorte que les valeurs concordent avec l'estimation de la [fréquence](#doc_freq).

Chaque modification d'un échantillon stéréo peut être répercutée sur l'échantillon lié si l'option :guilabel:`Édition stéréo` est cochée dans les [préférences](manual/settings.md#doc_general).
Grâce à une sélection multiple dans l'[arborescence](manual/soundfont-editor/tree.md), plusieurs échantillons peuvent être édités simultanément.


## Analyse fréquentielle {#doc_freq}


L'analyse fréquentielle inclut une transformée de Fourier (graphique présentant l'intensité des fréquences contenues dans le signal).
La liste des pics dominants est affichée sur la droite, contenant les informations suivantes&nbsp;:

* ratio entre l'intensité du pic courant et l'intensité du plus haut pic,
* fréquence,
* note correspondante la plus proche,
* correction (écart à la note correspondante la plus proche).

À partir de ces données, une estimation de la hauteur de note de l'échantillon est effectué.
Cette estimation peut être utilisée pour l'accordage.

Un export du graphique dans une image .png est possible en utilisant le menu contextuel du graphique (clic droit).
Un [outil](manual/soundfont-editor/tools/sample-tools.md#doc_peakfrequencies) est également disponible pour exporter les informations sur les pics de tous les échantillons.


![Graphique de Fourier](images/fourier_graph.png "Graphique de Fourier")


## Égaliseur {#doc_eq}


L'égaliseur permet d'amplifier ou d'atténuer certaines fréquences, de -15 à 15 dB.
Il est possible d'écouter l'effet en direct en activant l'option :guilabel:`aperçu` et ensuite d'appliquer définitivement l'effet avec le bouton :guilabel:`appliquer`.
Le bouton :guilabel:`réinitialiser` réinitialise toutes les valeurs à 0 dB (mais n'enlève pas les effets sur le son&nbsp;: pour cela utilisez le bouton :guilabel:`annuler` dans la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_edit)).

Chaque modification sur un échantillon stéréo peut être automatiquement répercutée sur l'échantillon lié si l'option est cochée dans les [préférences](manual/settings.md#doc_general).


## Lecteur {#doc_player}


Le lecteur lit le son.
Pendant la lecture, un curseur blanc parcourt le graphique pour indiquer la position de lecture.
Les options de lecture sont&nbsp;:

* **Boucle**\
  Lit le son en boucle si les positions de début et fin de la boucle sont renseignées.
* **Stéréo**\
  Lit le son en stéréo, c'est à dire avec l'échantillon lié.
  Si la lecture en boucle est en plus requise, les deux boucles respectives seront utilisées (elles peuvent être différentes).
* **Sinus**\
  Un signal sinusoïdal de calibration est ajouté au son, permettant l'accordage de l'échantillon au centième de demi-ton près.
  Tout d'abord la note de base doit être précisée.
  À ce stade, des battements peuvent être entendus si la fréquence du sinus diffère légèrement de la fréquence fondamentale du son.
  Il reste à ajuster finement la correction pour rendre les battements aussi lents que possible, de manière à accorder l'échantillon au mieux.
  Voir le tutoriel «&nbsp;[Comment préparer un échantillon](tutorials/how-to-prepare-a-sample.md)&nbsp;» pour plus de détails.

Si la lecture est en cours et que vous sélectionnez un autre échantillon dans l'[arborescence](manual/soundfont-editor/tree.md), elle reprend automatiquement.
Par ailleurs, la touche du clavier :kbd:`espace` active ou désactive la lecture lors d'une navigation dans l'arborescence.