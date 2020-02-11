Pour créer une [soundfont][sf2], cliquez sur :guilabel:`Nouvelle soundfont` sur la [page de démarrage][home-left].
Les soundfonts sont composées d'une structure à 3 niveaux et c'est ainsi que la création d'une soundfont implique 3 étapes fondamentales&nbsp;:

* la [préparation des échantillons](#doc_sample),
* la [création d'un instrument](#doc_instrument), faisant référence aux échantillons préalablement préparés,
* la [création d'un ensemble](#doc_preset), faisant référence à des instruments.


## Préparation des échantillons {#doc_sample}


### Chargement d'un échantillon


La préparation d'un échantillon commence par le **chargement** d'un fichier .wav.
Pour ce faire, sélectionnez la catégorie :guilabel:`Échantillons` dans l'[arborescence][tree] et cliquez sur l'icône :guilabel:`Importer des échantillons` dans la [barre d'outils][toolbar-edit].
Les échantillons peuvent être obtenus&nbsp;:

* sur Internet,
* en enregistrant un instrument de musique réel,
* par synthèse audio en utilisant un logiciel spécialisé.


### Bouclage d'un échantillon


Ensuite, une **boucle** peut être requise dans l'échantillon si vous voulez que le son généré puisse durer plus longtemps que la longueur initiale de l'échantillon.
Pour illustrer ceci, imaginez que vous possédez un échantillon d'une flûte qui ne dure qu'une seconde et que vous souhaitez pouvoir le faire durer indéfiniment.
Ce peut être fait en assignant manuellement ou automatiquement des points de bouclage (début + fin) entre les bornes de l'échantillon de manière à ce que la zone de bouclage puisse être répétée, rendant possible une plus longue lecture de l'échantillon.
Bien que vous puissiez positionner des points de bouclage manuellement, l'outil «&nbsp;[Bouclage automatique][s-tool-autoloop]&nbsp;» permet d'en créer automatiquement et produit généralement de meilleurs résultats tout en étant beaucoup plus rapide.

Pour positionner des points de bouclage manuellement&nbsp;:

1. cliquez sur l'échantillon à boucler dans l'[arborescence][tree],
2. dans l'[éditeur d'échantillons][s-editor], positionner arbitrairement le début et la fin de la bouche dans le [graphique][s-editor-graph] (zone d'affichage WAV) à moins qu'une boucle soit déjà définie.
  Utilisez un clic gauche pour le début et un clic droit pour la fin.
  Le point de bouclage de droite doit être placé en premier (puisque le point de droite est par défaut placé sur la position 0 et qu'il n'est pas possible d'assigner le point de gauche après le point de droite).
3. appuyer sur le bouton :guilabel:`Lecture` après avoir autorisé la fonction de bouclage,
4. ajustez le début et / ou la fin de la boucle, l'échantillon étant toujours en cours de lecture, jusqu'à ce que la transition entre les deux positions soit la plus douce possible.


**Note&nbsp;:** pour entendre le bouclage dans un instrument, vous devez sélectionner ![loop on][loop_on] dans :guilabel:`Lecture en boucle` pour soit la division globale soit la division de l'échantillon concerné dans la [table de paramétrage][i-editor-table]. ![loop on][loop_on] active la lecture en boucle, ![loop off][loop_off] ou une cellule vide désactive la boucle.


![Illustration d'une boucle](images/loop_illustration.png "Illustration d'une boucle")


### Accordage d'un échantillon


Enfin, un **accordage** doit être effectué.
Pour ce faire, un outil de calibration (Sinus) est disponible dans la page d'édition des [échantillons][s-editor-player].
Pour chaque échantillon, la méthode est la suivante&nbsp;:

1. démarrez la lecture (en appuyant sur le bouton :guilabel:`Lecture`), si possible avec la fonction de bouclage sélectionnée,
2. sélectionnez la fonction sinus,
3. ajustez le bouton de volume pour entendre les deux sons aussi clairement que possible,
4. changez la :guilabel:`Note de base` jusqu'à ce que les deux sons se superposent à peu près,
5. ajustez la :guilabel:`Correction (centièmes)` pour accorder l'échantillon sur sa note de base.
   Pour y parvenir, faîtes attention aux battements qui pourraient apparaître et rendez-les aussi lents que possible.

L'[analyse fréquencielle][s-editor-freq] peut être un bon indicateur pour commencer l'accordage.


### Pour aller plus loin


Différents outils sont disponibles pour l'édition des échantillons comme&nbsp;:

* [supprimer le blanc][s-tool-removeblank] au démarrage,
* [égaliser][s-tool-eq] le son,
* [transposer][s-tool-transpose],
* [normaliser][s-tool-volume] le volume,
* [ajuster la balance][s-tool-balance] pour les échantillons stéréo.


## Création d'un instrument {#doc_instrument}


Pour cette étape, il est supposé qu'au moins un échantillon est disponible pour la création d'un instrument.


### Créer un instrument et lier des échantillons


Premièrement, cliquez sur :guilabel:`Nouvel instrument` dans la [barre d'outils][toolbar-edit].
Un nom doit à présent être renseigné.

Ajoutez ensuite des échantillons dans l'instrument en utilisant un «&nbsp;[drag & drop][tree-dragdrop]&nbsp;».
Dans l'arborescence vous pourrez noter que des divisions apparaissent dans l'instrument.

**Note&nbsp;:** Lorsque des divisions sont créées les échantillons ne sont pas copiés mais liés. Il n'y a par ailleurs aucune contrainte sur le nombre de divisions.


### Répartition des divisions


Quand des échantillons sont liés dans des instruments, ils apparaissent comme divisions (colonnes) dans la [table d'édition des instruments][i-editor-table].
Chaque division doit ensuite être positionnée sur le clavier en changeant le paramètre :guilabel:`Étendue de notes` dans la table.
Pour un meilleur son, il est préférable que l'étendue d'une division inclut la note de base de l'échantillon représenté.
L'étendue complète du clavier doit être couverte (soit de la note 36 à la note 96 pour le clavier classique d'un synthétiseur).

L'outil «&nbsp;[Auto-positionnement des échantillons][i-tool-position]&nbsp;» répartit automatiquement les échantillons sur le clavier.


### Paramétrer les divisions


Pour autoriser la lecture en boucle dans un instrument contenant des échantillons bouclés, sélectionner ![loop on][loop_on] dans la ligne correspondant au paramètre :guilabel:`Lecture en boucle` pour soit  :guilabel:`Global`, soit des colonnes individuelles dans la table des paramètres.

* ![loop on][loop_on] active la lecture en boucle,
* ![loop off][loop_off] ou une ligne vide la désactive.
* ![loop on and end][loop_on_end] active la lecture en boucle et lit le reste de l'échantillon une fois la touche relâchée.

La division globale, comme son nom l'indique, vous permet d'entrer un paramètre qui sera valable pour l'instrument entier.
Un paramètre global ne s'applique qu'aux divisions dont le même paramètre n'est pas spécifié.
Cela signifie que les paramètres des divisions standards ont priorité sur les paramètres de la division globale.

Dans le contexte de ce simple tutoriel, aucun autre paramètre n'aura besoin d'être édité pour que l'instrument fonctionne.
Il serait déjà possible de jouer avec le [clavier virtuel][toolbar-keyboard].

Cependant, pour améliorer l'instrument il est conseillé de spécifier une valeur dans la ligne :guilabel:`Vol env relâche (s)` afin d'éviter au son de s'arrêter brutalement dès qu'une touche est relâchée.
Les autres paramètres sont décrits dans la description des [tables][i-editor-table].


## Création d'un ensemble {#doc_preset}


Pour cette étape, il est supposé qu'au moins un instrument est disponible pour la création d'un preset.


### Qu'est ce qu'un ensemble&nbsp;?


Un ensemble est l'équivalent d'un «&nbsp;sound patch name&nbsp;» que l'on peut trouver sur les synthétiseurs hardware.
Les ensembles, chacun caractérisé par un numéro de banque, un numéro d'ensemble et un nom, constituent la partie visible extérieurement de la soundfont.
Une soundfont peut avoir un ou plusieurs ensembles.
Chaque ensemble est lié à un ou plusieurs instruments, tout comme les instruments sont liés à un ou plusieurs échantillons.


### Créer un ensemble et lier des instruments


Tout d'abord, cliquez sur :guilabel:`Nouvel ensemble` dans la [barre d'outils][toolbar-edit].
Un nom doit maintenant être spécifié.

Ensuite liez des instruments à l'ensemble en utilisant un «&nbsp;[drag & drop][tree-dragdrop]&nbsp;».
Dans l'arborescence vous pourrez noter qu'une ou plusieurs divisions sont apparues sous l'ensemble.
La plupart du temps les ensembles ne sont liés qu'à un seul instrument.

Il est possible de créer autant d'ensembles que d'instruments, voire plus, et chaque ensemble peut contenir un instrument ou plus.


### Paramétrer un ensemble


Modifier les paramètres dans les ensembles peut ne pas être nécessaire étant donné que&nbsp;:

* lors de la création d'un ensemble, les numéros de banque et d'ensemble sont automatiquement assignés,
* lors du lien entre un instrument et un ensemble, l'étendue de notes est automatiquement calculé en fonction de l'instrument.

Dans le cadre de ce tutoriel, la création de la soundfont est terminée&nbsp;!


[loop_off]:    images/loop_off.png    "loop off"        {.inline}
[loop_on]:     images/loop_on.png     "loop on"         {.inline}
[loop_on_end]: images/loop_on_end.png "loop on and end" {.inline}

[home-left]:          manual/index.md#doc_left
[i-editor-table]:     manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table
[i-tool-position]:    manual/soundfont-editor/tools/instrument-tools.md#doc_position
[s-editor-freq]:      manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq
[s-editor-graph]:     manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph
[s-editor-player]:    manual/soundfont-editor/editing-pages/sample-editor.md#doc_player
[s-editor]:           manual/soundfont-editor/editing-pages/sample-editor.md
[s-tool-autoloop]:    manual/soundfont-editor/tools/sample-tools.md#doc_autoloop
[s-tool-balance]:     manual/soundfont-editor/tools/sample-tools.md#doc_balance
[s-tool-eq]:          manual/soundfont-editor/tools/sample-tools.md#doc_eq
[s-tool-removeblank]: manual/soundfont-editor/tools/sample-tools.md#doc_removeblank
[s-tool-transpose]:   manual/soundfont-editor/tools/sample-tools.md#doc_transpose
[s-tool-volume]:      manual/soundfont-editor/tools/sample-tools.md#doc_volume
[sf2]:                manual/annexes/the-different-soundfont-formats.md#doc_sf2
[toolbar-edit]:       manual/soundfont-editor/toolbar.md#doc_edit
[toolbar-keyboard]:   manual/soundfont-editor/toolbar.md#doc_keyboard
[tree-dragdrop]:      manual/soundfont-editor/tree.md#doc_dragdrop
[tree]:               manual/soundfont-editor/tree.md
