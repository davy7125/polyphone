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
: Date de création du fichier.

Produit
: Produit pour lequel la banque de sons est conçue.

Copyright
: Mention à remplir si le fichier possède un copyright.

Commentaires
: Pour ajouter des informations non incluses dans les catégories précédentes.

Type d'échantillons
: Indique si les échantillons sont stockés avec une précision de 16 bits ou 24 bits.
  Les échantillons 24 bits peuvent ne pas être supportés par tous les synthétiseurs.


## Informations générales {#doc_info}


L'éditeur présente les informations non éditables suivantes&nbsp;:

Version de la banque de sons
: Version du [format sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) utilisé par le fichier.

Nom et version ROM
: Nom et version d'un échantillon ROM si utilisé.

Moteur de son
: Nom d'un moteur de sons.

Logiciel(s) d'édition
: Logiciels d'édition utilisés pour la création de la banque de sons.

Nom du fichier
: Adresse et nom du fichier.

Taille du fichier
: Taille de la banque de sons après sauvegarde.


## Statistiques {#doc_stats}


Un compte est effectué sur&nbsp;:

* les [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md) et échantillons non utilisés,
* les [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) et instruments non utilisés,
* les [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md),
* les paramètres et modulateurs édités dans les instruments,
* les paramètres et modulateurs édités dans les ensembles.

Les échantillons et instruments non utilisés peuvent être facilement enlevés avec l'outil «&nbsp;[Enlever les éléments non utilisés](manual/soundfont-editor/tools/global-tools.md#doc_unused)&nbsp;».

**Note&nbsp;:** Le nombre de 65536 paramètres dans les instruments et les ensembles ne devrait pas être dépassé.
Certains synthétiseurs pourraient ne pas être adaptés pour tous les lire.