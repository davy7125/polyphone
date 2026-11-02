Beaucoup de banques de sons jouent un seul échantillon par note&nbsp;: quelle que soit la force avec laquelle une touche est frappée, c'est le même enregistrement qui est entendu, simplement plus ou moins fort.
Les instruments acoustiques ne se comportent pas ainsi.
Une touche de piano frappée doucement donne un son rond et feutré, alors que la même touche frappée fort donne un son plus brillant, plus riche en harmoniques, avec une attaque plus percussive.

Les couches de vélocités apportent cette richesse à une banque de sons&nbsp;: chaque note est enregistrée à plusieurs nuances, et l'enregistrement joué est choisi selon la vélocité de la note.
Ce tutoriel prend le piano comme exemple, mais la méthode s'applique à tous les instruments dont le timbre change avec la force du jeu&nbsp;: guitares, batteries, cuivres, cordes…
Il se termine par d'autres détails qui rendent un instrument encore plus réaliste.


## Pourquoi un seul échantillon ne suffit pas {#doc_why}


Écoutez la touche Do4 frappée quatre fois, de plus en plus fort (vélocités 30, 60, 90 et 120)&nbsp;:

* avec un <a href="files/velocity-layers-one-layer.mp3" download>seul échantillon</a>, enregistré à une nuance moyenne,

![](files/velocity-layers-one-layer.mp3)

* avec <a href="files/velocity-layers-four-layers.mp3" download>quatre couches</a>, l'enregistrement étant choisi selon la vélocité.

![](files/velocity-layers-four-layers.mp3)

Avec un seul échantillon, les notes douces sonnent comme des notes fortes entendues de loin et la note la plus forte manque de brillance.
Le synthétiseur abaisse légèrement la fréquence de coupure d'un filtre pour les notes douces (c'est l'un des modulateurs par défaut), mais on est loin du changement de timbre d'un vrai piano.
Avec quatre couches, le timbre suit la force du jeu.

La différence est également évidente dans un contexte musical, ici avec des arpèges de plus en plus forts&nbsp;:

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>un seul échantillon</a>,

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>quatre couches</a>.

![](files/velocity-layers-phrase-four-layers.mp3)


## Préparation des échantillons {#doc_samples}


### Enregistrement des couches


Chaque note doit être enregistrée à plusieurs nuances, par exemple *pp* (pianissimo), *mp* (mezzo-piano), *mf* (mezzo-forte) et *ff* (fortissimo).
Avec 3 ou 4 couches l'amélioration est déjà nette, alors que les meilleures banques de piano utilisent de 8 à 16 couches pour des transitions plus douces.
Tous les enregistrements doivent être faits dans les mêmes conditions (micros, distance, gain)&nbsp;: seule la force du jeu doit changer.

L'exemple de ce tutoriel utilise 4 couches, avec un échantillon toutes les 3 touches sur toute l'étendue du piano.


### Importation


Nommez les fichiers avec la note et la nuance, par exemple :file:`C4 pp.wav`, :file:`C4 mp.wav`, :file:`C4 mf.wav` et :file:`C4 ff.wav`.
Lorsque la note de base n'est pas enregistrée dans un fichier audio, Polyphone la trouve dans le nom du fichier s'il contient le nom de la note en notation anglaise (C4 pour le Do4, touche 60) ou le numéro de la touche.
La nuance écrite dans le nom servira à l'étape suivante.

Importez tous les fichiers avec :guilabel:`Ajouter un échantillon` dans la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_edit), ou par un [glisser-déposer](manual/soundfont-editor/tree.md#doc_dragdrop) dans l'arborescence.
Un fichier stéréo donne deux échantillons, dont les noms se terminent par «&nbsp;L&nbsp;» et «&nbsp;R&nbsp;».
Vérifiez ensuite l'accordage de chaque échantillon comme expliqué dans le tutoriel «&nbsp;[Comment préparer un échantillon](tutorials/how-to-prepare-a-sample.md#doc_tuning)&nbsp;».
Les échantillons de piano n'ont besoin d'aucune boucle&nbsp;: le son s'éteint naturellement.


## Un instrument par couche {#doc_instruments}


Chaque couche devient un instrument contenant les échantillons d'une seule nuance.
Pour chaque nuance&nbsp;:

1. tapez son nom («&nbsp;pp&nbsp;» par exemple) dans la [barre de recherche](manual/soundfont-editor/tree.md#doc_search) de l'arborescence, de sorte que seuls les échantillons de cette couche soient affichés,
2. sélectionnez tous ces échantillons&nbsp;: cliquez sur le premier, puis sur le dernier en maintenant la touche :kbd:`Maj`,
3. cliquez sur :guilabel:`Ajouter un instrument` dans la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_edit), nommez l'instrument («&nbsp;Piano pp&nbsp;» par exemple) et laissez cochée l'option :guilabel:`Lier les échantillons sélectionnés`.

Les deux échantillons d'une paire stéréo sont automatiquement positionnés à gauche et à droite, et les étendues de notes sont réparties sur le clavier selon les notes de base (voir l'outil «&nbsp;[Auto-positionnement des échantillons](manual/soundfont-editor/tools/instrument-tools.md#doc_position)&nbsp;»).
Chaque instrument peut déjà être joué seul avec le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard).

Les paramètres communs à toutes les couches peuvent être modifiés en une fois&nbsp;: lorsque les quatre instruments sont sélectionnés dans l'arborescence, la [table](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) affiche leurs divisions globales côte à côte, et une valeur saisie après avoir sélectionné les quatre cellules d'une ligne est écrite dans chacune d'elles.
Dans l'exemple, le paramètre :guilabel:`Vol env relâche (s)` vaut 1 s pour que le son ne s'arrête pas brutalement lorsqu'une touche est relâchée.


## Superposition des couches dans un ensemble {#doc_preset}


Sélectionnez les quatre instruments dans l'arborescence et cliquez sur :guilabel:`Ajouter un ensemble`, l'option :guilabel:`Lier les instruments sélectionnés` étant cochée.
L'ensemble contient alors quatre divisions, chacune couvrant tout le clavier.
Les couches sont superposées en donnant à chaque division de l'ensemble une :guilabel:`Étendue de vélocités` différente dans l'[éditeur d'ensembles](manual/soundfont-editor/editing-pages/preset-editor.md)&nbsp;:

* 0-39 pour la couche *pp*,
* 40-69 pour la couche *mp*,
* 70-99 pour la couche *mf*,
* 100-127 pour la couche *ff*.

Les étendues doivent couvrir toutes les vélocités de 0 à 127, sans trou (certaines vélocités resteraient muettes) ni chevauchement (deux couches seraient entendues en même temps).
Les limites n'ont pas besoin d'être régulières&nbsp;: elles peuvent être déplacées selon les enregistrements.
Les valeurs de la ligne :guilabel:`Atténuation (dB)` sont expliquées dans la section suivante.


![Couches de vélocités dans la table de l'ensemble](images/velocity_layers_table.png "Couches de vélocités dans la table de l'ensemble")


Dans l'[éditeur d'étendues](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range), chaque couche est une bande horizontale&nbsp;: la division sélectionnée (ici *mp*) est hachurée.
Cette vue montre rapidement un trou ou un chevauchement.


![Couches de vélocités dans l'éditeur d'étendues](images/velocity_layers_ranges.png "Couches de vélocités dans l'éditeur d'étendues")


**Note&nbsp;:** les étendues de vélocités peuvent aussi être définies dans un seul instrument contenant tous les échantillons, chaque division ayant sa propre étendue de vélocités.
Dans ce cas, le choix :guilabel:`par étendues de vélocités` de l'option :guilabel:`Ordonner les divisions` dans les [préférences](manual/settings.md#doc_interface) garde ensemble les divisions d'une même couche.


## Équilibrage des couches {#doc_balance}


Au sein d'une couche, le volume change déjà avec la vélocité&nbsp;: c'est le rôle d'un autre modulateur par défaut, défini par le format sf2.
Mais à chaque limite, le volume augmente brusquement car les enregistrements de la couche supérieure sont plus forts&nbsp;: environ 5 à 7 dB dans l'exemple.
Le but est que seul le timbre change en passant d'une couche à la suivante, le volume augmentant progressivement.

Utilisez le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard) pour comparer les deux côtés d'une limite&nbsp;: la vélocité d'une note dépend de la position verticale du clic, et les flèches :kbd:`↑` et :kbd:`↓` rejouent la dernière note avec une vélocité augmentée ou diminuée de 1.
Ensuite, pour chaque limite, de bas en haut&nbsp;:

1. jouez une note avec la dernière vélocité d'une couche (39 par exemple) puis avec la première vélocité de la couche suivante (40),
2. augmentez l'atténuation de la couche supérieure dans la table de l'ensemble jusqu'à ce que les deux notes aient à peu près le même volume,
3. passez à la limite suivante&nbsp;: les atténuations s'additionnent, chaque couche étant au moins aussi atténuée que la couche inférieure.

Dans l'exemple, les atténuations sont de 0, 6, 11 et 16 dB, comme le montre la table ci-dessus.
Vérifiez plusieurs notes sur tout le clavier&nbsp;: si un registre reste déséquilibré, l'atténuation des divisions concernées peut être affinée dans l'instrument de la couche, par exemple avec l'outil «&nbsp;[Paramétrer en fonction des notes](manual/soundfont-editor/tools/instrument-tools.md#doc_global)&nbsp;».

La dynamique globale de l'instrument, de la note la plus douce à la plus forte, est alors donnée par le modulateur par défaut.
Si les notes douces sont trop faibles ou pas assez, ce modulateur peut être modifié avec l'outil «&nbsp;[Surcharger un modulateur par défaut](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)&nbsp;».

Enfin, rien ne vaut un vrai morceau de musique pour tester le résultat&nbsp;: jouez la banque de sons avec un clavier MIDI ou avec le [lecteur MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player) (voir le tutoriel «&nbsp;[Lire un fichier MIDI avec une banque de sons](tutorials/read-midi-file-with-soundfont.md)&nbsp;»).


## Plus de détails pour plus de réalisme {#doc_details}


Les couches de vélocités sont un grand pas, mais un vrai instrument a d'autres caractéristiques qu'une banque de sons peut reproduire.
En voici deux pour le piano.


### Bruits de relâche


Lorsqu'une touche est relâchée, l'étouffoir retombe sur les cordes et la mécanique fait un petit bruit.
Certaines banques d'échantillons fournissent ces bruits&nbsp;: celle utilisée dans l'exemple en a un par touche.

Ils sont placés dans un cinquième instrument, «&nbsp;Piano relâche&nbsp;», ajouté à l'ensemble sur toute l'étendue de vélocités&nbsp;:

* sélectionnez le mode de lecture ![relâche](images/sample_mode_release.png "relâche"){.inline} (relâche) dans la ligne :guilabel:`Lecture en boucle`, pour qu'un échantillon soit joué lorsque la touche est relâchée,
* utilisez une :guilabel:`Vol env relâche (s)` longue (5 s dans l'exemple) pour que le bruit soit joué en entier,
* atténuez fortement l'instrument (40 dB dans l'exemple)&nbsp;: le bruit doit rester discret.

Grâce au modulateur par défaut qui relie la vélocité au volume, le bruit est plus fort lorsque la note a été frappée plus fort.

**Note&nbsp;:** le mode de lecture «&nbsp;relâche&nbsp;» n'est pas documenté par le format sf2 et peut ne pas être pris en charge par d'autres lecteurs.
Le tutoriel «&nbsp;[Utiliser des échantillons pour la relâche d'une note](tutorials/using-custom-releases-in-an-instrument.md)&nbsp;» décrit une méthode qui fonctionne avec tous les lecteurs.


### Cordes sans étouffoirs


Sur un piano, les cordes les plus aiguës n'ont pas d'étouffoirs (la dernière octave et demie, environ)&nbsp;: elles continuent de résonner après le relâchement de la touche.
Dans chaque instrument de couche, les divisions de ces touches (à partir du Fa6, touche 89, dans l'exemple) reçoivent donc une :guilabel:`Vol env relâche (s)` plus longue&nbsp;: 3 s au lieu de 1 s.

Écoutez des notes courtes au milieu du clavier, puis dans l'octave la plus aiguë et enfin un accord dans les graves&nbsp;:

* <a href="files/velocity-layers-without-details.mp3" download>sans ces détails</a>,

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>avec les bruits de relâche et les cordes sans étouffoirs</a>.

![](files/velocity-layers-with-details.mp3)


### Autres idées


D'autres améliorations méritent d'être essayées&nbsp;:

* enregistrer chaque touche au lieu d'une touche sur trois&nbsp;: sur un piano, chaque touche a ses propres cordes, et un échantillon transposé n'est jamais exactement la note voisine,
* utiliser des enregistrements stéréo, qui donnent de la largeur au son,
* calculer des temps de relâche qui varient sur le clavier avec l'outil «&nbsp;[Relâche naturelle](manual/soundfont-editor/tools/instrument-tools.md#doc_release)&nbsp;»,
* ajuster le volume ou le filtre par registre avec l'outil «&nbsp;[Paramétrer en fonction des notes](manual/soundfont-editor/tools/instrument-tools.md#doc_global)&nbsp;».

Et la même démarche fonctionne pour d'autres instruments&nbsp;: une corde de guitare pincée doucement ou fort, une caisse claire entre notes fantômes et accents, une trompette passant d'un son rond à un son cuivré…
Chaque détail emprunté à l'instrument réel rend la banque de sons plus vivante.
