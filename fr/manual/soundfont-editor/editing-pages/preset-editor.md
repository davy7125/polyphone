En sélectionnant au moins un ensemble dans l'[arborescence](manual/soundfont-editor/tree.md), l'éditeur des ensembles apparait.
Un ensemble est une combinaison d'un instrument ou plus et est l'élément visible de l'extérieur d'une [soundfont](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

Tout comme l'[éditeur des instruments](manual/soundfont-editor/editing-pages/instrument-editor.md), vous y retrouverez plusieurs parties&nbsp;:

* la table,
* l'éditeur d'étendues lorsque le bouton :guilabel:`Éditer les étendues` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est sélectionné,
* la section des modulateurs.


![Éditeur des ensembles](images/edit_preset.png "Éditeur des ensembles")


Si seulement un ensemble est sélectionné, la page permettra l'édition des paramètres globaux ainsi que les paramètres de toutes les divisions.
Dans le cas où plusieurs ensembles sont sélectionnés, les paramètres globaux de chacun de ces ensembles peuvent être édités.


## Différences avec les instruments {#doc_diff}


Les paramètres édités ici sont relatifs alors que les paramètres des instruments sont, sauf cas particulier, absolus.
Les valeurs des paramètres d'un ensemble sont additionnés ou multipliés (selon le paramètre) aux paramètres des instruments qui le compose.
Certains paramètres ne sont pas éditables au niveau des ensembles&nbsp;:

* lecture en boucle,
* note de base,
* classe exclusive,
* note fixe,
* vélocité fixe,
* les quatre offsets.

Un ensemble est identifié par un numéro d'ensemble et un numéro de banque, cette combinaison étant unique et ajustable sous la table d'édition des paramètres.
Chaque banque contient jusque 127 presets.

La dernière banque (128) est habituellement utilisée pour les kits de percussion.