En sélectionnant au moins un ensemble dans l'[arborescence](manual/soundfont-editor/tree.md), l'éditeur d'ensembles apparaît.
Un ensemble est une combinaison d'un ou plusieurs instruments et est l'élément visible depuis l'extérieur d'une [banque de sons](manual/annexes/the-different-soundfont-formats.md#doc_sf2).

Tout comme l'[éditeur d'instruments](manual/soundfont-editor/editing-pages/instrument-editor.md), vous y retrouverez plusieurs parties&nbsp;:

* la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table),
* l'[éditeur d'étendues](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range) lorsque le bouton :guilabel:`Éditer les étendues` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est sélectionné,
* la [section des modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator).


![Éditeur d'ensembles](images/edit_preset.png "Éditeur d'ensembles")


Si un seul ensemble est sélectionné, la page permet l'édition des paramètres globaux ainsi que des paramètres de toutes les divisions.
Dans le cas où plusieurs ensembles sont sélectionnés, les paramètres globaux de chacun de ces ensembles peuvent être édités.


## Différences avec les instruments {#doc_diff}


Les paramètres édités ici sont relatifs alors que les paramètres des instruments sont, sauf cas particulier, absolus.
Les valeurs des paramètres d'un ensemble sont additionnées ou multipliées (selon le paramètre) aux paramètres des instruments qui le composent.
Pour cette raison, une atténuation (ou tout autre paramètre) peut être négative au niveau des ensembles, ce qui n'est pas le cas au niveau des instruments.
Certains paramètres ne sont pas éditables au niveau des ensembles&nbsp;:

* lecture en boucle,
* note de base,
* classe exclusive,
* note fixe,
* vélocité fixe,
* les quatre offsets.

Un ensemble est identifié par un numéro d'ensemble et un numéro de banque, cette combinaison étant unique et ajustable sous la table d'édition des paramètres.
Chaque banque contient jusqu'à 128 ensembles (numérotés de 0 à 127).

La dernière banque (128) est habituellement utilisée pour les kits de percussion.