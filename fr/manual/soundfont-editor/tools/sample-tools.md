Quand au moins un échantillon est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md), la [boîte à outils](manual/soundfont-editor/tools/index.md) propose des outils rangés dans quatre catégories&nbsp;:

* [boucle / lecture](#doc_cat_loop_playback),
* [échantillons stéréo](#doc_cat_stereo),
* [édition rapide](#doc_cat_fast_editing),
* [fichiers](#doc_cat_files),
* [traitement du son](#doc_cat_processing),

L'utilisation d'outils pour plusieurs échantillons simultanément est possible (avec une sélection multiple).


## Boucle / lecture


### Ajuster à la fin de boucle {#doc_trimloop}

Si une boucle est définie pour l'échantillon sélectionné, le logiciel coupera le son après la fin de la boucle (laissant une petite marge à la fin).

![Ajuster à la fin de boucle](images/trim_to_loop.png "Ajuster à la fin de boucle")

**Note&nbsp;:** cet outil peut être appliqué automatiquement à chaque fois qu'un échantillon est importé, l'option étant dans les [préférences](manual/settings.md#doc_general) du logiciel.


### Bouclage automatique {#doc_autoloop}

Le logiciel détermine deux positions dans l'échantillon, distantes et appropriées pour créer une boucle.
Si besoin, un court segment de son est copié avec un fondu enchaîné de manière à ce que la transition au point de jonction soit la plus douce possible.

S'il échoue, un message d'avertissement apparaît indiquant qu'aucune boucle ne peut être faite.

**Attention&nbsp;:** Cet outil modifie l'échantillon original.
Les modifications peuvent être annulées avec la fonction «&nbsp;[annuler](manual/soundfont-editor/toolbar.md#doc_edit)&nbsp;», mais si le fichier est sauvegardé et si Polyphone est fermé il n'y a pas moyen de revenir en arrière.

**Note&nbsp;:** Par défaut, les boucles sont désactivées dans un instrument.
Pour les entendre, vous devez sélectionner ![boucle activée](images/loop_on.png "boucle activée"){.inline} pour le paramètre :guilabel:`Lecture en boucle` dans soit la division :guilabel:`Global`, soit dans chaque division séparément dans la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
![boucle activée](images/loop_on.png "boucle activée"){.inline} active la boucle, ![boucle désactivée](images/loop_off.png "boucle désactivée"){.inline} désactive la boucle.
![boucle activée + fin](images/loop_on_end.png "boucle activée + fin"){.inline} active la boucle et l'échantillon sera lu jusqu'à la fin une fois la note relâchée.


### Enlever le silence initial {#doc_removeblank}

Tout silence au début de l'échantillon est éliminé, réduisant le temps avant que le son soit entendu.

![Suppression du silence initial](images/remove_blank.png "Suppression du silence initial")

**Note&nbsp;:** cet outil peut être appliqué automatiquement à chaque fois qu'un échantillon est importé, l'option étant dans les [préférences](manual/settings.md#doc_general) du logiciel.


### Fondu enchaîné {#doc_crossfadeloop}

L'outil de fondu enchaîné peut être vu comme la version manuelle de l'[outil de bouclage automatique](#doc_autoloop). Tout d'abord, vous devez positionner le début et la fin de la boucle autour d'une zone stable dans le graphique présentant l'onde sonore. Ensuite, cet outil crée une boucle à partir de ces positions en appliquant un fondu enchaîné dont la durée est paramétrable. Tout est donc configurable mais en contrepartie la recherche du début et de la fin de la boucle doit être minutieuse&nbsp;:

* les harmoniques du son doivent correspondre pour ne pas entendre en effet de disque rayé lors de la transition,
* les phases des différentes harmoniques doivent correspondre également pour ne pas entendre d'annulation de fréquence pendant le fondu, altérant fortement le timbre d'un instrument,
* autant que possible, la fin de la boucle doit être éloigné du début pour éviter un son trop synthétique.

Un conseil est de se concentrer sur les positions où l'onde croise l'axe des x (valeur 0).

![Fondu enchaîné](images/tool_crossfade_loop.png "Fondu enchaîné")


## Échantillons stéréo {#doc_cat_stereo}


### Dissocier {#doc_unlink}

Tous les échantillons sélectionnés qui sont liés sont séparés.

Ainsi, tous les échantillons seront traités comme des sons mono et auront la même balance lorsqu'ils seront ajoutés dans un instrument.
L'édition stéréo, si elle est activée dans les [préférences](manual/settings.md#doc_general), ne sera plus applicable.


### Équilibrer la balance {#doc_balance}

Si l'échantillon est lié à un autre (stéréo), le logiciel peut égaliser l'intensité moyenne des deux échantillons pour ajuster la balance.
Il sera ensuite possible de modifier la balance de manière plus précise dans un [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).


### Trouver un lien {#doc_link}

Parmi les échantillons sélectionnés, les échantillons dont les noms ne diffèrent que par la dernière lettre (L&nbsp;/&nbsp;R) sont automatiquement liés.
Par exemple, si dans une banque de sons deux échantillons sont nommés ``bassoon45L`` et ``bassoon45R``, ``bassoon45L`` sera reconnu comme la partie gauche de ``bassoon45R`` et ``bassoon45R`` sera reconnu comme la partie droite de ``bassoon45L``.

Avec cette information la valeur initiale de la balance sera correctement renseignée lorsque ces échantillons seront ajoutés dans un instrument.
Vous pourrez aussi profiter de l'édition simultanée des parties gauche et droite dans la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) des instruments si l'édition stéréo est activée dans les [préférences](manual/settings.md#doc_general) du logiciel.

**Attention&nbsp;:** Cet outil peut casser des liens existants si les noms ne remplissent pas la condition ci-dessus.


## Édition rapide {#doc_cat_fast_editing}


### Décaler un paramètre {#doc_shift_parameter}

Cet outil permet l'édition simultanée, par l'ajout ou la multiplication d'une valeur, d'un de ces paramètres pour tous les échantillons sélectionnés&nbsp;:

* note de base,
* correction,
* début de la boucle,
* fin de la boucle.

![Décaler un paramètre](images/tool_shift_parameter.png "Décaler un paramètre")


### Récupérer un paramètre depuis les instruments {#doc_load_from_instruments}

Les paramètres au niveau de l'instrument peuvent remplacer ou décaler les paramètres définis au niveau de l'échantillon.
Avec cet outil, il est possible de corriger ou complèter les paramètres de l'échantillon avec ce qui est défini au niveau des instruments utilisant ces échantillons&nbsp;:

* en modifiant la note de base de l'échantillon avec la surchage utilisée au niveau de l'instrument,
* en décalant la note de base de l'échantillon avec le paramètre «&nbsp;Accordage (demi-tons)&nbsp;»,
* en décalant la correction de hauteur de l'échantillon avec le paramètre «&nbsp;Accordage (centièmes)&nbsp;»,
* en définissant le début et la fin de la boucle de l'échantillon avec les offsets de boucle.

![Récupérer un paramètre depuis les instruments](images/tool_load_from_instruments.png "Récupérer un paramètre depuis les instruments")


## Fichiers {#doc_cat_files}


### Exporter au format wav {#doc_wavexport}

Après avoir sélectionné un répertoire de destination, tous les échantillons sélectionnés seront exportés dans des fichiers .wav.


## Traitement du son {#doc_cat_processing}


### Commande externe {#doc_externalcommand}

Cet outil vous permet d'utiliser une commande ou un programme externe pour éditer un échantillon.
Vous n'avez ainsi pas besoin d'exporter et de ré-importer l'échantillon dans Polyphone, tout est effectué automatiquement&nbsp;:

1. téléchargez un logiciel d'édition pour les fichiers .wav, comme :program:`Audacity` ou :program:`Sweep` (Linux),
2. utilisez l'outil :guilabel:`Commande externe` pour les échantillons sélectionnés,
3. remplissez la ligne de commande dans le champ correspondant pour appeler le logiciel externe (il est possible que vous deviez lire la documentation du logiciel concerné au préalable),
4. éditez les échantillons avec l'outil externe,
5. sauvegardez la version modifiée des fichiers .wav avec le logiciel externe et fermez-le,
6. les échantillons seront automatiquement chargés dans la banque de sons.

![Outil d'appel à une commande externe](images/tool_external_command.png "Outil d'appel à une commande externe")

**Note&nbsp;:** Pendant le processus, Polyphone exporte un fichier .wav et réimportera ce même fichier après exécution de la commande externe. Si la commande externe crée d'autres fichiers ou si le résultat est sauvegardé dans un autre fichier, ces fichiers seront ignorés et Polyphone réimportera le fichier exporté initialement qui ne sera probablement pas modifié.


### Filtrer des fréquences {#doc_filter}

Quand vous appelez cet outil, une fenêtre apparaît pour vous présenter les fréquences contenues dans les échantillons sélectionnés.
Vous pouvez ensuite utiliser la souris pour dessiner une zone rouge.
Tout ce qui se trouve dans cette zone sera supprimé&nbsp;: vous pouvez manuellement réduire ou supprimer certaines fréquences.

![Outil de filtrage des fréquences](images/tool_filter_frequencies.png "Outil de filtrage des fréquences")

**Note&nbsp;:** Ce filtre n'est pas commun puisqu'il coupe les fréquences de manière brutale.
Cet outil peut être utile pour concevoir un filtre «&nbsp;mur de brique&nbsp;» mais dans la plupart des cas vous voudrez probablement utiliser l'[égaliseur](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq).


### Modifier le volume {#doc_volume}

Vous pouvez changer le volume d'un échantillon de trois manières différentes&nbsp;:

* en ajoutant un certain nombre de décibels (quantité positive ou négative),
* en multipliant le signal par un nombre,
* en normalisant le signal de manière à ce que le maximum de ce signal représente un certain pourcentage de l'intensité maximale.

![Outil de modification du volume](images/tool_change_volume.png "Outil de modification du volume")


### Transposer {#doc_transpose}

L'outil demande un décalage en demi-tons et ensuite ré-échantillonne le son pour changer sa hauteur de note.

![Outil de transposition](images/tool_transpose_smpl.png "Outil de transposition")
