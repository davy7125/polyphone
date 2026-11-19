L'éditeur des informations générales est affiché quand l'entête :guilabel:`Général` est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md).
Cet éditeur est composé de trois types d'éléments&nbsp;:

* des [champs éditables](#doc_edit),
* des [informations générales](#doc_info) non éditables,
* des [statistiques](#doc_stats) sur la banque de sons.

Le bouton :guilabel:`Modulateurs par défaut` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) affiche les [modulateurs par défaut](#doc_default_mod) de la banque de sons, et le bouton :guilabel:`Général` réaffiche cette page.


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

* les [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md), les échantillons non utilisés et les échantillons en double,
* les [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) et les instruments non utilisés,
* les [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md),
* les paramètres et modulateurs édités dans les instruments,
* les paramètres et modulateurs édités dans les ensembles.

Le bouton :guilabel:`Détails` à côté des échantillons, des instruments et des ensembles affiche le [résumé de configuration](manual/soundfont-editor/configuration-summaries.md) correspondant.

Les échantillons et instruments non utilisés peuvent être facilement enlevés avec l'outil «&nbsp;[Enlever les éléments non utilisés](manual/soundfont-editor/tools/global-tools.md#doc_unused)&nbsp;».
Les échantillons en double peuvent être enlevés avec l'outil «&nbsp;[Nettoyer les doublons](manual/soundfont-editor/tools/global-tools.md#doc_duplicates)&nbsp;».

**Note&nbsp;:** Le nombre de 65536 paramètres dans les instruments et les ensembles ne devrait pas être dépassé.
Certains synthétiseurs pourraient ne pas être en mesure de tous les lire.



## Modulateurs par défaut {#doc_default_mod}


Les modulateurs par défaut s'appliquent à chaque instrument de la banque de sons, avant les [modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) de l'instrument&nbsp;: un modulateur d'instrument ayant les mêmes sources, la même destination et la même transformation remplace un modulateur par défaut (indication :guilabel:`surcharge un mod. par défaut`).

Sans liste propre, une banque de sons utilise les modulateurs par défaut du format SoundFont, affichés sans pouvoir être modifiés&nbsp;: vélocité vers atténuation et vers fréquence de coupure, pression du canal et molette de modulation vers vibrato, volume, panoramique, expression, réverbération, chorus et molette de hauteur.
Le modulateur vélocité vers fréquence de coupure dépend des [réglages du son](manual/settings.md#doc_sound).

Le bouton :guilabel:`Personnaliser` donne à la banque de sons sa propre liste, qui commence par ces modulateurs et se modifie ensuite comme les modulateurs d'un instrument (ajout, suppression, copie, collage, modification).
Cette liste remplace les modulateurs par défaut du format SoundFont&nbsp;: un modulateur retiré de la liste ne s'applique plus à aucun instrument, un modulateur ajouté s'applique à tous.
Le bouton :guilabel:`Revenir aux modulateurs du format SoundFont` supprime la liste et rétablit les modulateurs par défaut du format.

La liste est enregistrée dans le bloc DMOD des fichiers sf2 et sf3, une [extension du format](manual/annexes/the-different-soundfont-formats.md#doc_sf2) lue par FluidSynth 2.5 et ultérieur, SpessaSynth et Polyphone 2.7 et ultérieur (y compris le [plugin](manual/plugin.md)).
Les autres synthétiseurs l'ignorent et appliquent les modulateurs par défaut du format SoundFont.

Quand un instrument est copié dans une autre banque de sons dont les modulateurs par défaut diffèrent, des modulateurs globaux sont ajoutés à l'instrument pour qu'il sonne de la même façon.
L'[export DLS](manual/annexes/the-different-soundfont-formats.md#doc_dls) convertit les modulateurs par défaut de la banque de sons&nbsp;; l'export sfz ne garde que leur effet de la vélocité sur le volume et sur la fréquence de coupure.