Avec Polyphone, il est possible d'ouvrir un répertoire depuis l'[écran d'accueil](manual/index.md#doc_left) en cliquant sur le bouton :guilabel:`Ouvrir répertoire`. L'explorateur de banque de sons s'ouvre alors dans un onglet pour afficher le contenu de ce répertoire et en présentant, pour chaque banque de sons, un aperçu de son contenu.


![Explorateur de banques de sons](images/soundfont_explorer.png "Explorateur de banques de sons")


## Aperçu des banques de sons {#doc_preview}


La liste principale, située à gauche de l'interface, contient la liste des fichiers et Polyphone les distingue ainsi&nbsp;:

* les fichiers compatibles et pouvant être scannés rapidement (formats [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) et [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) par exemple) sont affichés avec des détails sur leur contenu,
* les fichiers compatibles et ne pouvant pas être scannés rapidement (format [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) notamment) sont affichés avec des informations basiques seulement,
* les fichiers qui ne sont pas compatibles sont grisés et ne peuvent être ouverts.

Un clic sur un fichier pouvant être scanné permet d'afficher dans les listes de la partie droite de l'interface les échantillons, instruments et ensembles qui sont contenus. Chaque élément de ces listes contient également des informations qui permettent d'évaluer le niveau de détails et de travail sur la banque de sons.

Détails au niveau du fichier&nbsp;:

* la taille et la date de dernière mise à jour sont affichées pour chaque fichier,
* pour les banques de sons pouvant être scannées, ces informations sont ajoutées&nbsp;:
  * nombre d'échantillons, d'instruments et d'ensembles,
  * noms des premiers ensembles.

Informations pour chaque [échantillon](manual/soundfont-editor/editing-pages/sample-editor.md)&nbsp;:

* type (mono / stéréo),
* durée totale,
* durée de la boucle s'il y en a une,
* fréquence d'échantillonnage.

Informations pour chaque [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) ou [ensemble](manual/soundfont-editor/editing-pages/preset-editor.md)&nbsp;:

* nombre total de divisions, paramètres et générateurs,
* nombre d'étendues distinctes de notes,
* nombre d'étendues distinctes de vélocités.


## Fonctions de tri et de recherche {#doc_filter}


La partie haute présente un champ de texte. Lorsque celui-ci est rempli, l'explorateur n'affiche que les fichiers répondant à au moins l'un de ces critères&nbsp;:

* le nom contient le texte recherché,
* l'un des échantillons, instruments ou ensembles contient le texte recherché.

Les listes des échantillons, instruments et ensembles sont également filtrées pour mettre en évidence les éléments recherchés.

À droite du champ de texte se trouve une liste déroulante permettant de trier les fichiers selon leur nom, leur date de dernière modification ou leur poids.


## Opérations sur les fichiers {#doc_file_operations}


Lors d'un clic sur un fichier dans la liste principale, 2 icônes liquables apparaissent avant le nom du fichier&nbsp;:

* un crayon permettant de renommer le fichier,
* une corbeille permettant de mettre le fichier à la corbeille.

Ces opérations permettent de gérer / assainir rapidement une collection locale de banques de sons.


## Ouvrir une banque de sons {#doc_open_soundfont}


Un double-clic sur un élément de la liste principale ouvre le fichier — dans le cas où son format est compatible avec Polyphone — à l'endroit configuré dans les [paramètres](manual/settings.md#doc_interface) (les [informations générales](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) sont affichées par défaut).

Un double-clic sur un échantillon, instrument ou ensemble ouvre le fichier sur l'élément sélectionné.