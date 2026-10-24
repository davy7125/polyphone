Le menu, situé dans le coin en haut à droite du logiciel, contient plusieurs éléments vous permettant d'accéder à différentes fonctionnalités.


![Menu principal](images/menu.png "Menu principal")


## Ouverture d'un éditeur de banques de sons {#doc_open}


Nouveau
: Crée une nouvelle banque de sons vide, tout devant être fait à partir de zéro.
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`N`.

Ouvrir…
: Ouvre un fichier [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3), [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz), [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) ou [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ).
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`O`.


## Sauvegarde d'une banque de sons {#doc_save}


Sauvegarder
: Sauvegarde le fichier courant.
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`S`.

Sauvegarder sous…
: Sauvegarde le fichier courant dans un autre fichier.
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`Maj`+:kbd:`S`.

Exporter banques de sons…
: Fusionne et exporte un ou plusieurs ensembles depuis les banques de sons ouvertes dans le format [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2), [.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3) ou [.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz). Le format additionnel .csv exporte tous les paramètres pour une analyse fine du contenu de la banque de sons.
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`E`.


![Fenêtre d'export](images/export.png "Fenêtre d'export")


Dans la fenêtre d'export, les ensembles de toutes les banques de sons ouvertes sont listés et ceux à exporter doivent être cochés (les boutons :guilabel:`Tout cocher` et :guilabel:`Tout décocher` permettent de changer rapidement la sélection).
Les autres paramètres sont&nbsp;:

Emplacement
: Le répertoire dans lequel les fichiers sont écrits.
  Un fichier n'est jamais écrasé&nbsp;: si un fichier de même nom existe déjà, un numéro est ajouté à la fin du nom du nouveau fichier.

Format
: L'un des formats :guilabel:`.sf2`, :guilabel:`.sf3`, :guilabel:`.sfz` ou :guilabel:`.csv`.
  Si les ensembles proviennent de plusieurs banques de sons, ils sont fusionnés dans une seule banque de sons dans laquelle chaque banque de sons source devient une banque.
  C'est pourquoi un maximum de 127 banques de sons, chacune avec au plus 127 ensembles sélectionnés, peut être exporté en une seule fois.

Qualité
: Pour le format .sf3 uniquement, qualité de la compression des échantillons.

Un fichier par ensemble
: Pour les formats .sf2 et .sf3 uniquement, création d'un fichier par ensemble sélectionné au lieu d'un fichier unique.

Un répertoire par banque, numéro de l'ensemble en préfixe, classement général MIDI
: Pour le format .sfz uniquement. L'export crée un répertoire contenant les fichiers sfz et un sous-répertoire :file:`samples`.
  Les fichiers sfz peuvent être placés dans un répertoire par banque, leur nom peut être préfixé par le numéro de l'ensemble, et ils peuvent être classés dans des répertoires correspondant aux catégories General MIDI.

Valeurs brutes
: Pour le format .csv uniquement. Les valeurs sont écrites telles qu'elles sont stockées dans le format sf2, au lieu d'être converties dans les unités usuelles (secondes, décibels, cents, etc.).
  L'export crée un répertoire contenant le fichier :file:`samples.csv` ainsi qu'un fichier par instrument et par ensemble.


## Options de visualisation {#doc_view}


Plein écran
: Active ou désactive le mode «&nbsp;plein écran&nbsp;».
  Accessible également avec le raccourci clavier :kbd:`F11`.


## Raccourcis {#doc_shortcuts}


Préférences
: Ouvre les [préférences](manual/settings.md) du logiciel.
  Accessibles également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`,`.

Aide en ligne
: Affiche ce manuel utilisateur dans un navigateur web.
  Accessible également avec le raccourci clavier :kbd:`F1`.

À propos de Polyphone…
: Affiche la version du logiciel et le copyright. Le bouton :guilabel:`Crédits` affiche la liste des personnes ayant contribué au projet.


## Fermeture {#doc_close}


Fermer l'onglet
: Ferme l'onglet courant (un éditeur de banque de sons, un explorateur de banques de sons, les préférences, etc.).
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`W`.

Quitter
: Ferme le logiciel.
  Accessible également avec le raccourci clavier :kbd:`Ctrl`+:kbd:`Q`.