Avec Polyphone, il est possible d'ouvrir un répertoire depuis l'[écran d'accueil](manual/index.md#doc_left) en cliquant sur le bouton :guilabel:`Ouvrir répertoire`. L'explorateur de banques de sons s'ouvre alors dans un nouvel onglet pour afficher les fichiers de ce répertoire (ses sous-répertoires ne sont pas scannés) en présentant, pour chaque banque de sons, un aperçu de son contenu.


![Explorateur de banques de sons](images/soundfont_explorer.png "Explorateur de banques de sons")


## Aperçu des banques de sons {#doc_preview}


La liste principale, située à gauche de l'interface, contient les fichiers et Polyphone les distingue ainsi&nbsp;:

* les fichiers compatibles et pouvant être scannés rapidement (formats [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) et [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls) par exemple) sont affichés avec des détails sur leur contenu,
* les fichiers compatibles et ne pouvant pas être scannés rapidement (format [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) notamment) sont affichés avec des informations basiques seulement,
* les fichiers qui ne sont pas compatibles sont grisés et ne peuvent pas être ouverts.

Un clic sur un fichier pouvant être scanné affiche, dans les listes de la partie droite de l'interface, les échantillons, instruments et ensembles qu'il contient. Chaque élément de ces listes contient également des informations qui permettent d'évaluer le niveau de détail de la banque de sons et la quantité de travail qui y a été consacrée.

### Détails au niveau du fichier

* la taille et la date de dernière mise à jour sont affichées pour chaque fichier,
* pour les banques de sons pouvant être scannées, les informations suivantes sont ajoutées&nbsp;:
  * nombre d'échantillons, d'instruments et d'ensembles,
  * noms des premiers ensembles.

Ces détails sont alignés en colonnes sous un en-tête. Le bouton :guilabel:`Affichage`, à droite du champ de texte, ouvre un menu permettant de choisir les colonnes affichées (toutes peuvent être masquées sauf le nom) et d'afficher ou non le :guilabel:`Résumé des ensembles`, c'est-à-dire les noms des premiers ensembles sur une seconde ligne. Sans ce résumé, chaque fichier tient sur une seule ligne, comme dans un tableau.

### Informations pour chaque échantillon

* type (mono / stéréo),
* durée totale,
* durée de la boucle s'il y en a une,
* fréquence d'échantillonnage.

### Informations pour chaque instrument ou ensemble

* nombre total de divisions, de paramètres et de générateurs,
* nombre d'étendues de notes distinctes,
* nombre d'étendues de vélocités distinctes.


## Fonctions de tri et de recherche {#doc_filter}


La partie haute présente un champ de texte (le raccourci :kbd:`Ctrl`+:kbd:`F` y place le focus). Lorsque celui-ci est rempli, l'explorateur n'affiche que les fichiers répondant à au moins l'un de ces critères&nbsp;:

* le nom contient le texte recherché,
* le nom de l'un des échantillons, instruments ou ensembles contient le texte recherché.

Les listes des échantillons, instruments et ensembles sont également filtrées pour mettre en évidence les éléments recherchés.

Un clic sur l'en-tête d'une colonne trie les fichiers selon leur nom, leur taille, leur date ou leur nombre d'échantillons, d'instruments ou d'ensembles, et un nouveau clic inverse l'ordre. Le premier clic trie les noms de A à Z et les autres colonnes en commençant par la plus grande valeur ou le fichier le plus récent. Lors d'un tri selon un nombre d'éléments, les fichiers dont le contenu est inconnu sont toujours placés en fin de liste.

Les options d'affichage, le tri ainsi que la largeur et l'état déplié ou replié des listes de droite sont conservés pour la prochaine fois.


## Opérations sur les fichiers {#doc_file_operations}


Lors d'un clic sur un fichier dans la liste principale, 2 icônes cliquables apparaissent avant le nom du fichier&nbsp;:

* un crayon permettant de renommer le fichier (également accessible avec :kbd:`F2`),
* une corbeille permettant de mettre le fichier à la corbeille, après confirmation (également accessible avec :kbd:`Suppr`).

Ces opérations permettent de gérer et de nettoyer rapidement une collection locale de banques de sons.


## Jouer les éléments {#doc_play}


Le bouton ![piano](images/toolbar_piano.png "piano"){.inline}, à droite du bouton :guilabel:`Affichage`, affiche ou masque le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard), et le bouton ![magnétophone](images/toolbar_recorder.png "magnétophone"){.inline} situé à côté affiche ou masque le [magnétophone](manual/soundfont-editor/toolbar.md#doc_recorder), comme les boutons de la barre d'outils de l'éditeur.

L'échantillon, l'instrument ou l'ensemble sélectionné dans les listes de droite peut être joué avec le clavier virtuel, ou avec un clavier MIDI externe même si le clavier virtuel est masqué. Comme dans l'éditeur, le clavier virtuel montre l'étendue de l'élément, ou la note de référence d'un échantillon. Un échantillon est joué en boucle s'il en possède une, et en stéréo si l'option :guilabel:`stéréo` du [lecteur](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player) d'échantillon est cochée.

La première fois qu'un élément d'un fichier est sélectionné, la banque de sons est chargée en arrière-plan sans ouvrir d'onglet. La banque de sons est ensuite conservée tant que les éléments sélectionnés appartiennent au même fichier, et libérée lorsqu'un autre fichier est sélectionné dans la liste principale.

Une banque de sons déjà ouverte dans un onglet est jouée telle qu'elle est dans l'éditeur, avec ses modifications non enregistrées.


## Ouvrir une banque de sons {#doc_open_soundfont}


Un double-clic sur un élément de la liste principale ouvre le fichier — dans le cas où son format est compatible avec Polyphone — à l'endroit configuré dans les [préférences](manual/settings.md#doc_interface) avec l'option :guilabel:`Vue de démarrage` (les [informations générales](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) sont affichées par défaut).

Un double-clic sur un [échantillon](manual/soundfont-editor/editing-pages/sample-editor.md), un [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md) ou un [ensemble](manual/soundfont-editor/editing-pages/preset-editor.md) ouvre le fichier sur l'élément sélectionné.

Si la banque de sons a déjà été chargée pour jouer l'un de ses éléments, l'onglet s'ouvre immédiatement.