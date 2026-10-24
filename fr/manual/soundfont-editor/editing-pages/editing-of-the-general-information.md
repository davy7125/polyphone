L'éditeur des informations générales est affiché quand l'entête :guilabel:`Général` est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md).
Cet éditeur est composé de trois types d'éléments&nbsp;:

* des [champs éditables](#doc_edit),
* des [informations générales](#doc_info) non éditables,
* des [statistiques](#doc_stats) sur la banque de sons.


![Éditeur des informations générales](images/edit_general.png "Éditeur des informations générales")


## Champs éditables {#doc_edit}


L'éditeur des informations générales fournit des champs pour modifier les attributs suivants&nbsp;:

Titre
: Nom interne de la banque de sons, qui n'a pas forcément de rapport avec le nom du fichier.

Auteur
: Nom de l'auteur de la banque de sons.

Date
: Date de création du fichier (un calendrier aide à la choisir).

Produit
: Produit pour lequel la banque de sons est conçue.

Copyright
: Mention à remplir si le fichier possède un copyright.

Commentaires
: Pour ajouter des informations non incluses dans les catégories précédentes (le texte peut contenir n'importe quel caractère UTF-8).

Profondeur de bits
: Indique si les échantillons sont stockés avec une précision de 16 bits ou de 24 bits (auquel cas la compression n'est pas autorisée).
  Les échantillons 24 bits peuvent ne pas être pris en charge par tous les synthétiseurs.

Compression
: Rend possible la compression des échantillons (auquel cas la profondeur de bits est fixée à 16 bits). Si cette option est activée, un bouton de compression apparaît dans l'éditeur d'[échantillons](manual/soundfont-editor/editing-pages/sample-editor.md).
  La compression est une fonctionnalité apportée par le [format sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) et peut ne pas être prise en charge par tous les synthétiseurs.
  L'activation ou la désactivation de cette option définit le format lors de la sauvegarde, et Polyphone propose un changement d'extension .sf2 / .sf3 si nécessaire.


## Informations générales {#doc_info}


L'éditeur présente les informations non éditables suivantes&nbsp;:

Format SoundFont
: Version du [format sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) utilisé par le fichier.

Nom et version de la ROM
: Nom et version d'une ROM d'échantillons si utilisée.

Moteur de son
: Nom d'un moteur de son.

Logiciel(s) d'édition
: Logiciels d'édition utilisés pour la création de la banque de sons.

Ces informations sont affichées dans une info-bulle lorsque la souris survole l'icône d'information, à la fin de la ligne indiquant l'auteur et la date.

Nom et taille du fichier
: Adresse et nom du fichier, ainsi que sa taille sur le disque, affichés en bas de la fenêtre (la taille est celle du fichier lors de sa dernière sauvegarde).


## Statistiques {#doc_stats}


Un compte est effectué sur&nbsp;:

* les [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md) et les échantillons non utilisés,
* les [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) et les instruments non utilisés,
* les [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md),
* les paramètres et modulateurs édités dans les instruments,
* les paramètres et modulateurs édités dans les ensembles.

Le bouton :guilabel:`Détails` à côté des échantillons, des instruments et des ensembles affiche le [résumé de configuration](manual/soundfont-editor/configuration-summaries.md) correspondant.

Les échantillons et instruments non utilisés peuvent être facilement enlevés avec l'outil «&nbsp;[Enlever les éléments non utilisés](manual/soundfont-editor/tools/global-tools.md#doc_unused)&nbsp;».

**Note&nbsp;:** Le nombre de 65536 paramètres dans les instruments et les ensembles ne devrait pas être dépassé.
Certains synthétiseurs pourraient ne pas être en mesure de tous les lire.