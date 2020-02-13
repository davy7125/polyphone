Quand au moins un échantillon est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md), la [boîte à outils](manual/soundfont-editor/tools/index.md) propose des outils rangés dans quatre catégories:

* [analyse](#doc_cat_analyze),
* [fichiers](#doc_cat_files),
* [traitement des échantillons](#doc_cat_processing),
* [échantillons stéréo](#doc_cat_stereo).

L'utilisation d'outils pour plusieurs échantillons simultanément est possible (avec une sélection multiple).


## Analyse {#doc_cat_analyze}


### Visualiser les pics de fréquences {#doc_peakfrequencies}


Cet outil rassemble les informations sur les pics de fréquences de tous les échantillons sélectionnés et le résultat peut être exporté dans un fichier .csv.
Les pics de fréquences sont visibles dans l'[éditeur des échantillons](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq).


![Pics de fréquences](images/tool_show_peak_frequencies.png "Pics de fréquences")


## Fichiers {#doc_cat_files}


### Exporter au format wav {#doc_wavexport}


Après avoir sélectinné un répertoire de destination, tous les échantillons sélectionnés seront exportés dans des fichiers .wav.


## Traitement des échantillons {#doc_cat_processing}


### Bouclage automatique {#doc_autoloop}


Le logiciel détermine une zone stable dans l'échantillon et crée ensuite artificiellement une boucle.
Un segment de son est copié avec un cross-fade, de manière à ce que la transition au point de jonction soit la plus douce possible.

Pour assigner des points de bouclage automatiquement&nbsp;:

1. cliquer dans l'[arborescence](manual/soundfont-editor/tree.md) sur l'échantillon à boucler,
2. dans l'éditeur des [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md):
  * positionnez le début et la fin de la boucle dans le graphique (zone d'affichage de l'onde) autour d'une zone stable, ou
  * laissez l'outil détermine automatiquement un zone stable en définissant 0 pour le début et la fin de boucle,
3. dans la boîte à :guilabel:`outils` sélectionnez :guilabel:`Bouclage automatique`,
4. appuyez sur le bouton :guilabel:`Play` avec l'option de lecture en boucle activée,
5. si les points de bouclage trouvés ne sont pas adaptés vous pouvez [annuler](manual/soundfont-editor/toolbar.md#doc_edit) l'opération, essayez de les repositionner autour d'une zone stable et utiliser la fonction de :guilabel:`Bouclage automatique` une nouvelle fois.

L'outil a besoin de trouver une zone stable de plus de 0.25 seconde.
S'il échoue, un message d'avertissement apparaît indiquant qu'aucune boucle ne peut être faite.

**Attention&nbsp;:** Cet outil modifie l'échantillon original.
Les modifications peuvent être annulée avec la fonction «&nbsp;[annuler](manual/soundfont-editor/toolbar.md#doc_edit)&nbsp;», mais si le fichier est sauvegardé et si Polyphone est fermé il n'y a pas moyen de revenir en arrière.

**Note&nbsp;:** Par défaut, les boucles sont désactivées dans un instrument.
Pour l'entendre, vous devez sélectionner ![boucle activée](images/loop_on.png "boucle activée"){.inline} pour le paramètre :guilabel:`Lecture en boucle` dans soit la division :guilabel:`Globale`, soit dans chaque division séparément dans la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).
![boucle activée](images/loop_on.png "boucle activée"){.inline} active la boucle, ![boucle désactivée](images/loop_off.png "boucle désactivée"){.inline} désactive la boucle.
![boucle activée + fin](images/loop_on_end.png "boucle activée + fin"){.inline} active la boucle et l'échantillon sera lu jusqu'à la fin une fois la note relâchée.


### Modifier le volume {#doc_volume}


Vous pouvez changer le volume d'un échantillon de trois manières différentes&nbsp;:

* en ajoutant une certaine quantité de dB (quantité positive ou négative),
* en multipliant le signal par un nombre,
* en normalisant le signal de manière à ce que le maximum de ce signal représente un certain pourcentage de l'intensité maximale.


![Outil de modification du volume](images/tool_change_volume.png "Outil de modification du volume")


### Commande externe {#doc_externalcommand}


Cet outil vous permet d'utiliser une commande ou un programme externe pour éditer un échantillon.
Vous n'avez ainsi pas besoin d'exporter et de ré-importer l'échantillon dans Polyphone, tout est effectué automatiquement&nbsp;:

1. téléchargez un logiciel d'édition pour les fichiers .wav, comme :program:`Audacity` ou :program:`Sweep` (Linux),
2. utilisez l'outil :guilabel:`Commande externe` pour les échantillons sélectionnés,
3. remplissez la ligne de commande dans le champ correspondant pour appeler le logiciel externe (il est possible que vous deviez lire la documentation de ce logiciel au préalable),
4. éditez les échantillons avec l'outil externe,
5. sauvegardez la version modifiée des fichiers .wav avec le logiciel externe et fermez-le,
6. les échantillons seront automatiquement chargés dans la banque de sons.


![Outil d'appel à une commande externe](images/tool_external_command.png "Outil d'appel à une commande externe")


### Filtrer des fréquences {#doc_filter}


Quand vous appelez cet outil, une fenêtre apparaît pour vous présenter les fréquences contenues dans les échantillons sélectionnés.
Vous pouvez ensuite utiliser la souris pour dessiner une zone rouge.
Tout ce qui se trouve dans cette zone sera supprimé&nbsp;: vous pouvez manuellement réduire ou supprimer certaines fréquences.


![Outil de filtrage des fréquences](images/tool_filter_frequencies.png "Outil de filtrage des fréquences")


**Note&nbsp;:** Ce filtre n'est pas commun puisqu'il coupe les fréquences de manière brutale.
Cet outil peut être utile pour concevoir un filtre «&nbsp;mur de brique&nbsp;» mais dans la plupart des cas vous voudrez probablement utiliser l'[égaliseur](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq).


### Enlever le silence initial {#doc_removeblank}


Tout silence au début de l'échantillon est éliminé.
Ceci réduit le temps avant que le son soit entendu.


![Suppression du silence initial](images/remove_blank.png "Suppression du silence initial")


**Note&nbsp;:** cet outil peut être appliqué automatiquement à chaque fois qu'un échantillon est importé, l'option étant dans les [préférences](manual/settings.md#doc_general) du logiciel.


### Transposer {#doc_transpose}


L'outil demande un décalage en demi-tons et ensuite ré-échantillone le son pour changer sa hauteur de note.


![Outil de transposition](images/tool_transpose_smpl.png "Outil de transposition")


### Ajuster à la fin de boucle {#doc_trimloop}


Si une boucle est définie pour l'échantillon sélectionné, le logiciel coupera le son après la fin de la boucle (laissant une petite marge à la fin).


![Ajuster à la fin de boucle](images/trim_to_loop.png "Ajuster à la fin de boucle")


**Note&nbsp;:** cet outil peut être appliqué automatiquement à chaque fois qu'un échantillon est importé, l'option étant dans les [préférences](manual/settings.md#doc_general) du logiciel.


## Échantillons stéréo {#doc_cat_stereo}


### Équilibrer la balance {#doc_balance}


Si l'échantillon est lié à un autre (stéreo), le logiciel peut égaliser l'intensité moyenne des deux échantillons pour ajuster la balance.
Il sera ensuite plus facile de modifier la balance dans un [instrument](manual/soundfont-editor/editing-pages/instrument-editor.md).


### Trouver un lien {#doc_link}


Parmi les échantillons sélectionnés, les échantillons dont les noms ne diffèrent que par la dernière lettre (R / L) sont automatiquement liés.
Par exemple, si dans une banque de sons deux échantillons sont nommés ``bassoon45L`` et ``bassoon45R``, ``bassoon45L`` sera reconnu comme la partie gauche de ``bassoon45R`` et ``bassoon45R`` sera reconnu comme la partie droite de ``bassoon45L``.

Avec cette information la valeur initiale de la balance sera correctement renseignée lorsque ces échantillons seront ajoutés dans un instrument.
Vous pourrez aussi profiter de l'édition simultanée des parties gauche et droite dans la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) des instruments si l'édition stéréo est activée dans les [préférences](manual/settings.md#doc_general) du logiciel.

**Warning&nbsp;:** Cet outil peut casser des liens existants si les noms ne remplissent pas la condition ci-dessus.


### Dissocier {#doc_unlink}


Tous les échantillons sélectionnés qui sont liés sont séparés.

Ainsi, tous les échantillons seront traités comme des sons mono et auront la même balance lorsqu'ils seront ajoutés dans un instrument.
L'édition stéréo, si elle est activée dans les [préférences](manual/settings.md#doc_general), ne sera plus applicable.