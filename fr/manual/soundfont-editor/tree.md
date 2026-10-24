L'arborescence est le premier élément de l'éditeur sur lequel vous devriez vous concentrer.
Elle représente l'architecture d'une banque de sons ouverte et pilote la partie droite du logiciel lorsque vous sélectionnez un ou plusieurs éléments.

L'arborescence est toujours affichée avec la même [structure](#doc_structure), possède un [menu contextuel](#doc_menu) et dispose de fonctions de [glisser-déposer](#doc_dragdrop), de [copier / coller](#doc_copypaste) et de [recherche](#doc_search).


## Structure {#doc_structure}


La structure de l'arborescence est faite de 4 entêtes et de contenu sous les entêtes :guilabel:`Échantillons`, :guilabel:`Instruments` et :guilabel:`Ensembles`.


![Structure de l'arborescence](images/tree_1.png "Structure de l'arborescence")


### Entête «&nbsp;Général&nbsp;»


Un clic sur l'entête :guilabel:`Général` ouvre l'éditeur des [informations générales](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md).


### Section «&nbsp;Échantillons&nbsp;»


Cette section liste tous les échantillons contenus dans la banque de sons, qui sont les matériaux de base à partir desquels les instruments sont construits.
La sélection d'un ou plusieurs échantillons amène à l'[éditeur d'échantillons](manual/soundfont-editor/editing-pages/sample-editor.md).
Un clic sur l'entête :guilabel:`Échantillons` affiche le [résumé de configuration](manual/soundfont-editor/configuration-summaries.md#doc_sample) de tous les échantillons.


### Section «&nbsp;Instruments&nbsp;»


Cette section liste tous les instruments contenus dans la banque de sons, un instrument étant un élément intermédiaire composé d'échantillons et utilisé par les ensembles.
Les instruments contiennent des liens vers les échantillons appelés «&nbsp;divisions&nbsp;», affichées sous la forme d'une liste sous chaque instrument.
La sélection d'un instrument ou de l'une de ses divisions amène à l'[éditeur d'instruments](manual/soundfont-editor/editing-pages/instrument-editor.md).
Un clic sur l'entête :guilabel:`Instruments` affiche le [résumé de configuration](manual/soundfont-editor/configuration-summaries.md#doc_instrument) de tous les instruments.


### Section «&nbsp;Ensembles&nbsp;»


Cette section liste tous les ensembles contenus dans la banque de sons, un ensemble étant l'élément final composé d'instruments et qui est accessible en dehors de la banque de sons.
Les ensembles contiennent des liens vers les instruments appelés «&nbsp;divisions&nbsp;», affichées sous la forme d'une liste sous chaque ensemble.
La sélection d'un ensemble ou de l'une de ses divisions affiche l'[éditeur d'ensembles](manual/soundfont-editor/editing-pages/preset-editor.md).
Un clic sur l'entête :guilabel:`Ensembles` affiche le [résumé de configuration](manual/soundfont-editor/configuration-summaries.md#doc_preset) de tous les ensembles.


## Menu contextuel {#doc_menu}


Un clic droit sur un élément affiche un menu contenant les actions suivantes&nbsp;:

Associer à…
: Associe les échantillons sélectionnés à un instrument, ou les instruments sélectionnés à un ensemble.
  La destination est choisie dans une liste qui peut être filtrée.
  Lorsque des échantillons ayant des notes de référence différentes sont associés à un instrument vide, leurs étendues de notes sont définies automatiquement (comme avec l'outil [Auto-positionnement des échantillons](manual/soundfont-editor/tools/instrument-tools.md#doc_position)).

Remplacer par…
: Remplace un échantillon par un autre échantillon dans une division d'instrument, ou remplace un instrument par un autre instrument dans une division d'ensemble.
  La configuration de la division est préservée.

Copier
: Copie les éléments sélectionnés, également accessible avec :kbd:`Ctrl`+:kbd:`C`.

Coller
: Colle les éléments précédemment copiés, également accessible avec :kbd:`Ctrl`+:kbd:`V`.

Dupliquer
: Duplique les échantillons, instruments, ensembles ou divisions sélectionnés.
  Également accessible avec :kbd:`Ctrl`+:kbd:`D`.

Supprimer
: Supprime les éléments sélectionnés, également accessible avec :kbd:`Suppr`.
  Un échantillon utilisé par un instrument et un instrument utilisé par un ensemble ne peuvent pas être supprimés.

Rendre muet
: À des fins de test, une division au sein d'un instrument ou d'un ensemble peut être rendue muette. Cette option peut aussi être trouvée dans le menu contextuel de l'[entête des tables](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_solo).

Toujours jouer
: À l'opposé de la fonction «&nbsp;Rendre muet&nbsp;», un ou plusieurs instruments ou ensembles peuvent être ajoutés au moment de jouer. Il est ainsi possible de trouver un bon équilibre entre plusieurs instruments ou ensembles jouant simultanément.

Rétablir la lecture
: Annule les effets de «&nbsp;Rendre muet&nbsp;» et de «&nbsp;Toujours jouer&nbsp;» dans toute la banque de sons.

Renommer ou renommage de masse
: Renomme les éléments sélectionnés, également accessible avec :kbd:`F2`.
  Renommer une division renomme l'échantillon ou l'instrument auquel elle est liée.
  Lorsque plusieurs éléments sont sélectionnés, une fenêtre propose différentes options&nbsp;:
  
  * écraser nom existant avec nom de la note en suffixe (pour les échantillons et les divisions d'instrument seulement),
  * écraser nom existant avec nom de la note et vélocité en suffixe (pour les divisions d'instrument seulement),
  * écraser nom existant avec incrément en suffixe,
  * remplacer une chaîne de caractères,
  * insérer à la position,
  * supprimer une étendue.


![Menu contextuel de l'arborescence](images/tree_2.png "Menu contextuel de l'arborescence")


## Glisser-déposer {#doc_dragdrop}


L'opération «&nbsp;glisser-déposer&nbsp;» (ou «&nbsp;drag & drop&nbsp;») dans l'arborescence permet d'effectuer rapidement des associations et des copies.
Avant un glisser-déposer, vous devez sélectionner des éléments.
Il peut s'agir d'un élément seul ou d'une sélection multiple rendue possible par l'utilisation des touches :kbd:`Ctrl` et :kbd:`Maj`.

Ensuite, le résultat d'un glisser-déposer dépend du type de l'élément déplacé et du type de l'élément ciblé.

Échantillon → instrument
: Association de l'échantillon à l'instrument ciblé, créant ainsi une division d'instrument.

Échantillon → entête :guilabel:`Instruments`
: Création d'un instrument qui contient tous les échantillons déplacés ou création d'un instrument par échantillon déplacé.

Instrument → ensemble
: Association de l'instrument à l'ensemble ciblé, créant ainsi une division d'ensemble.

Instrument → instrument
: Copie de toutes les divisions de l'instrument source dans l'instrument de destination (ce peut être le même instrument).

Division d'un instrument → instrument ou division d'un instrument
: Copie des divisions sélectionnées de l'instrument source dans l'instrument de destination (ce peut être le même instrument).

Instrument → entête :guilabel:`Ensembles`
: Création d'un ensemble qui contient tous les instruments déplacés ou création d'un ensemble par instrument déplacé.

Ensemble → ensemble
: Copie de toutes les divisions de l'ensemble source dans l'ensemble de destination (ce peut être le même ensemble).

Division d'un ensemble → ensemble ou division d'un ensemble
: Copie des divisions sélectionnées de l'ensemble source dans l'ensemble de destination (ce peut être le même ensemble).

Fichier audio (depuis l'explorateur de fichiers de l'ordinateur) → arborescence
: Import du fichier comme nouvel échantillon, si son format est pris en charge (voir l'[éditeur d'échantillons](manual/soundfont-editor/editing-pages/sample-editor.md)).


## Copier / coller {#doc_copypaste}


Tout ce qui est possible avec un glisser-déposer comme vu précédemment est aussi possible avec une opération copier / coller, accessible via le menu contextuel ou à l'aide des raccourcis clavier :kbd:`Ctrl`+:kbd:`C` et :kbd:`Ctrl`+:kbd:`V`.

L'opération copier / coller est aussi permise entre différentes banques de sons.
Il est possible de copier un élément d'une première banque de sons et de le coller dans une deuxième banque de sons.
Une sélection multiple est autorisée.

Sélection d'au moins un échantillon
: Copie des échantillons sélectionnés.

Sélection d'au moins un instrument
: Copie des instruments sélectionnés en incluant tous les échantillons liés.

Sélection d'au moins un ensemble
: Copie des ensembles sélectionnés en incluant tous les instruments et échantillons liés.

Lors de la copie d'éléments entre différentes banques de sons, si un élément ayant le même nom existe déjà, plusieurs options sont proposées&nbsp;:

* ignorer l'opération, auquel cas la copie est annulée pour l'élément ayant le même nom&nbsp;;
* remplacer l'élément existant, auquel cas l'élément existant est écrasé par l'élément copié&nbsp;;
* dupliquer l'élément, auquel cas l'élément est copié à côté de l'élément existant avec un nom différent.


## Recherche {#doc_search}


Pour faciliter la recherche d'éléments dans l'arborescence, une barre de recherche permettant de les filtrer est disponible (le raccourci :kbd:`Ctrl`+:kbd:`F` y place le focus).


![Barre de recherche](images/tree_3.png "Barre de recherche")


Lorsqu'un texte est écrit dans la barre de recherche, les éléments le contenant sont affichés.
De plus&nbsp;:

* si le texte est inclus dans le nom d'un échantillon, les instruments qui utilisent l'échantillon et les ensembles qui utilisent ces instruments sont affichés&nbsp;;
* si le texte est inclus dans le nom d'un instrument, les échantillons utilisés par l'instrument et les ensembles qui utilisent l'instrument sont affichés&nbsp;;
* si le texte est inclus dans le nom d'un ensemble, les instruments utilisés par l'ensemble et les échantillons utilisés par ces instruments sont affichés.

Un clic sur la croix à droite du champ éditable annule le filtre.