En cliquant sur un ou plusieurs échantillons dans l'[arborescence](manual/soundfont-editor/tree.md), l'éditeur d'échantillons apparaît.

Vous y trouverez plusieurs parties&nbsp;:

* un [graphique](#doc_graph) représentant l'échantillon,
* une zone d'[informations](#doc_info),
* une analyse des [fréquences](#doc_freq),
* un [égaliseur](#doc_eq),
* un [lecteur](#doc_player) d'échantillon.


![Éditeur d'échantillons](images/edit_sample.png "Éditeur d'échantillons")


## Le graphique {#doc_graph}


### Zoom et déplacement


Le graphique permet la visualisation de l'onde sonore.
Il est possible de zoomer verticalement et horizontalement en maintenant appuyé le bouton droit de la souris et en la déplaçant&nbsp;:

* un mouvement vers la droite dilate l'axe horizontal,
* un mouvement vers la gauche contracte l'axe horizontal,
* un mouvement vers le haut dilate l'axe vertical,
* un mouvement vers le bas contracte l'axe vertical.

Maintenir le bouton gauche de la souris permet un mouvement horizontal du graphique, si l'axe horizontal a été dilaté.
La molette de la souris fait également défiler le graphique horizontalement.


### Positions de boucle et de lecture


Une barre verticale verte indique la position de départ de la boucle (modifiable par un clic gauche sur le graphique) et une barre verticale rouge indique la position de fin de la boucle (modifiable par un clic droit sur le graphique).
Une ligne en pointillé représentant une portion d'onde est associée à chaque barre verticale, ce qui est utile pour visualiser la superposition du début et de la fin de la boucle.

Pendant la lecture, une barre verticale blanche parcourt le graphique et indique la position de lecture en temps réel.


### Couper l'échantillon


En maintenant la touche :kbd:`Alt`, il est possible de définir une zone à couper&nbsp;:

* l'appui sur le bouton gauche de la souris définit le début de la zone,
* le relâchement du bouton gauche définit la fin de la zone.

Une fois la zone définie, une fenêtre de dialogue apparaît pour demander une confirmation avant de couper l'échantillon.
La réponse :guilabel:`Oui et créer un autre échantillon` coupe l'échantillon et crée également un nouvel échantillon à partir de la partie qui a été retirée.


![Couper un échantillon](images/cutting_sample.png "Couper un échantillon")


### Couleurs du graphique


Les couleurs du graphique sont éditables dans les [préférences](manual/settings.md#doc_interface) du logiciel.


## Section «&nbsp;Information&nbsp;» {#doc_info}


La section :guilabel:`Informations` permet la visualisation — et dans la plupart des cas l'édition — des informations suivantes&nbsp;:

Taille
: Le nombre de valeurs et la durée correspondante en secondes sont affichés.

Boucle
: Le début et la fin de la boucle peuvent être modifiés ici.
  Un premier bouton enlève la boucle en mettant son début et sa fin à 0.
  Un second bouton édite les positions de début et de fin de manière à ce que la boucle représente l'échantillon entier.

Accordage
: Possibilité d'ajuster la note de base d'un échantillon et sa correction en centièmes de demi-ton pour que le son soit accordé dans le tempérament égal.
  Un bouton édite la note de base et la correction de sorte que les valeurs concordent avec l'estimation de la [fréquence](#doc_freq).

Échantillonnage
: Chaque modification déclenche un rééchantillonnage de manière à ce que la hauteur du son ne soit pas altérée.

Canal
: Choix du type parmi :guilabel:`mono`, :guilabel:`gauche`, :guilabel:`droit` ou :guilabel:`lien`.
  Si le type est :guilabel:`gauche`, :guilabel:`droit` ou :guilabel:`lien`, il est possible d'annuler la stéréo en choisissant :guilabel:`mono`.
  Lorsque :guilabel:`mono` est sélectionné, le retour à un état stéréo se fait en sélectionnant un échantillon lié.

Lien
: Sélection de l'échantillon à lier pour rendre le son stéréo.
  Si un échantillon est sélectionné, le type passe à :guilabel:`lien` (ce type pourra ensuite être changé en :guilabel:`gauche` ou :guilabel:`droit`).

Si la [compression](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit) est activée pour la banque de sons ([format sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)), un bouton de compression est affiché pour les échantillons qui ne sont pas encore compressés.
Lors d'une compression, le choix de la qualité de compression (:guilabel:`Haute`, :guilabel:`Moyenne` ou :guilabel:`Basse`) est demandé dans une fenêtre de dialogue.
La boucle d'un échantillon peut être altérée par la compression.\
**Note&nbsp;:** la décompression s'effectue automatiquement lors d'une édition de l'échantillon (application d'un filtre, coupure, amplification, etc.).

![Choix de la qualité de compression](images/sample_compress_dialog.png "Choix de la qualité de compression")

Chaque modification d'un échantillon stéréo peut être répercutée sur l'échantillon lié si l'option :guilabel:`Édition stéréo` est cochée dans les [préférences](manual/settings.md#doc_general) (ou avec le bouton situé en bas à droite de la fenêtre).
Grâce à une sélection multiple dans l'[arborescence](manual/soundfont-editor/tree.md), plusieurs échantillons peuvent être édités simultanément.


## Analyse fréquentielle {#doc_freq}


L'analyse fréquentielle inclut une transformée de Fourier (graphique présentant l'intensité des fréquences contenues dans le signal).

![Graphique de Fourier](images/fourier_graph.png "Graphique de Fourier")

La liste des pics dominants est affichée sur la droite, contenant les informations suivantes&nbsp;:

* ratio entre l'intensité du pic courant et l'intensité du plus haut pic,
* fréquence,
* note correspondante la plus proche,
* correction (écart à la note correspondante la plus proche).

À partir de ces données, une estimation de la hauteur de note de l'échantillon est effectuée.
Cette estimation peut être utilisée pour l'accordage.

Un clic droit sur le graphique ouvre un menu contextuel qui permet&nbsp;:

* l'export du graphique dans une image .png,
* l'affichage de la liste de tous les pics avec la possibilité d'exporter l'ensemble dans un fichier .csv.

![Pics de fréquences](images/tool_show_peak_frequencies.png "Pics de fréquences")


## Égaliseur {#doc_eq}


L'égaliseur permet d'amplifier ou d'atténuer certaines fréquences, de -15 à 15 dB.
Il est possible d'écouter l'effet en direct en activant l'option :guilabel:`aperçu` et ensuite d'appliquer définitivement l'effet avec le bouton :guilabel:`Appliquer`.
Le bouton :guilabel:`Réinitialiser` remet tous les curseurs à 0 dB (mais n'enlève pas un effet déjà appliqué au son&nbsp;: pour cela utilisez le bouton :guilabel:`Annuler` dans la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_edit)).

Chaque modification sur un échantillon stéréo peut être automatiquement répercutée sur l'échantillon lié si l'option est cochée dans les [préférences](manual/settings.md#doc_general).


## Lecteur {#doc_player}


Le lecteur lit le son.
Pendant la lecture, un curseur blanc parcourt le graphique pour indiquer la position de lecture.
Les options de lecture sont&nbsp;:

Boucle
: Lit le son en boucle si les positions de début et de fin de la boucle sont renseignées.

Stéréo
: Lit le son en stéréo, c'est-à-dire avec l'échantillon lié.
  Si la lecture en boucle est également demandée, les deux boucles respectives sont utilisées (elles peuvent être différentes).

Sinus
: Un signal sinusoïdal de calibration est ajouté au son, permettant l'accordage de l'échantillon au centième de demi-ton près.
  Tout d'abord, la note de base doit être précisée.
  À ce stade, des battements peuvent être entendus si la fréquence du sinus diffère légèrement de la fréquence fondamentale du son.
  Il reste à ajuster finement la correction pour rendre les battements aussi lents que possible, de manière à accorder l'échantillon au mieux.
  Voir le tutoriel «&nbsp;[Comment préparer un échantillon](tutorials/how-to-prepare-a-sample.md)&nbsp;» pour plus de détails.

Un curseur règle le volume de la lecture (ce volume n'est pas enregistré dans la banque de sons).

Si la lecture est en cours et que vous sélectionnez un autre échantillon dans l'[arborescence](manual/soundfont-editor/tree.md), elle redémarre automatiquement.
Par ailleurs, la touche :kbd:`Espace` démarre ou arrête la lecture lorsque le focus est sur l'arborescence.