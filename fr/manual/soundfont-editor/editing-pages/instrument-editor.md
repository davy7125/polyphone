En cliquant sur un instrument ou plus dans l'[arborescence](manual/soundfont-editor/tree.md), l'éditeur d'instruments apparaît.

Cette interface comprend plusieurs parties&nbsp;:

* la [table](#doc_table),
* l'[éditeur d'étendues](#doc_range) quand le bouton :guilabel:`Éditer les étendues` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est cliqué,
* l'[éditeur d'enveloppes](#doc_envelope) quand le bouton :guilabel:`Éditer les enveloppes` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est cliqué,
* la [vue du timbre](#doc_timbre) quand le bouton :guilabel:`Timbre` de la [barre d'outils](manual/soundfont-editor/toolbar.md#doc_view) est cliqué,
* la [section modulateurs](#doc_modulator).


![Édition des instruments](images/edit_instrument.png "Édition des instruments")


Si un seul instrument est sélectionné, les paramètres globaux de l'instrument et les paramètres de chacune de ses divisions peuvent être édités.

Dans le cas où plusieurs instruments sont sélectionnés, les paramètres globaux de chacun de ces instruments peuvent être édités.


![Édition de plusieurs instruments](images/edit_multiple_instruments.png "Édition de plusieurs instruments")


## Table {#doc_table}


### Présentation


La table offre une vue globale de la configuration d'un instrument et permet son édition.
Une modification d'un paramètre dans la division :guilabel:`Global` s'applique à toutes les divisions, sauf à celles dont le même paramètre est déjà spécifié (surcharge locale).

Les lignes écrites en italique (:guilabel:`Note de l'échantillon`, :guilabel:`Longueur de l'échantillon` et :guilabel:`Boucle`) ne sont pas éditables&nbsp;: elles rappellent seulement, à titre d'information, les propriétés de l'échantillon lié à chaque division.

Les paramètres éditables sont&nbsp;:

Étendue de notes
: Étendue de notes sur laquelle l'échantillon sera entendu.

Étendue de vélocités
: Étendue de vélocités sur laquelle l'échantillon sera entendu.
  Il est possible de configurer plusieurs sons sur la même note, avec différentes étendues de vélocités, pour distinguer différentes nuances d'une touche de piano par exemple.

Atténuation (dB)
: Atténuation en décibels appliquée à l'échantillon.\
  Note&nbsp;: le format sf2 n'autorise pas cette valeur à être négative au niveau des instruments, aucune amplification n'est ainsi possible.

Balance
: Balance appliquée au son, pour l'entendre à gauche ou à droite en stéréo.

Lecture en boucle
: ![Boucle désactivée](images/loop_off.png "boucle désactivée"){.inline} indique un son reproduit sans boucle.
  ![Boucle activée](images/loop_on.png "boucle activée"){.inline} indique un son reproduit avec une boucle continue.
  ![Relâche](images/sample_mode_release.png "relâche"){.inline} permet de déclencher la lecture d'un échantillon lorsqu'une touche est relâchée. L'enveloppe de volume est toujours utilisée pour définir le volume et une courte attaque (0,2 ms) est ajoutée pour lisser le début de l'échantillon lors de la relâche.
  ![Boucle activée + relâche](images/loop_on_end.png "boucle activée + relâche"){.inline} indique un son reproduit avec une boucle tant que la touche est appuyée et qui continue sa lecture jusqu'à la fin une fois la touche relâchée.\
  Note&nbsp;: le mode de lecture «&nbsp;relâche&nbsp;» n'est PAS documenté par le format sf2 et d'autres synthétiseurs comme FluidSynth peuvent décider de ne pas le prendre en charge.
  Une manière de parvenir au même comportement est de doubler chaque échantillon avec un échantillon contenant un silence très court suivi du son à émettre lors de la relâche.
  L'échantillon ajouté sera lu avec le paramètre «&nbsp;boucle activée + relâche&nbsp;».
  Voir le tutoriel «&nbsp;[Utiliser des relâches personnalisées dans un instrument](tutorials/using-custom-releases-in-an-instrument.md)&nbsp;».

Note de base
: Permet de changer la note de base configurée dans un échantillon par une autre.
  Si par exemple un échantillon possède la note de base 50 et que cet échantillon est lu sur l'étendue de notes [49-51], alors l'échantillon sera lu avec une transposition initiale de -1, 0 et +1 demi-ton sur les notes 49, 50, 51.
  Si à présent la note de base est surchargée à 51 au niveau de l'instrument, alors ce même échantillon sera lu avec une transposition initiale de -2, -1 et 0 demi-ton(s) sur les notes 49, 50, 51.

Accordage (demi-tons et centièmes)
: Modifie l'accordage en demi-tons et en centièmes de demi-ton.
  Cette transposition s'ajoute à la transposition initiale liée à la note déclenchée (par exemple la transposition initiale d'un échantillon dont la note de base est 60 sera +2 demi-tons si la touche appuyée le clavier est 62).

Réglage de tonalité
: Ce paramètre modifie la distance entre deux notes consécutives et modifiera ainsi la transposition initiale liée à la note déclenchée.
  Une valeur de 100 décrit la gamme tempérée&nbsp;: 100 centièmes de demi-tons sont utilisés pour transposer un échantillon d'une touche à l'autre.
  Une valeur de 0 désactive cette transposition initiale et l'échantillon sera ainsi lu à l'identique sur toutes les touches du clavier.

Filtre
: Applique un filtre passe-bas, en spécifiant la fréquence de coupure et la résonance.

Enveloppe de volume
: Modifie l'enveloppe de volume et également la manière dont elle évolue en fonction de la note.

Modulation
: Configure une modulation (enveloppe, délai, fréquence, évolution en fonction de la note) et son effet sur le ton, le filtre et le volume.

Vibrato
: Configure un vibrato (délai, fréquence) et son effet sur le ton.

Classe exclusive
: Ce paramètre peut définir une ou plusieurs classes exclusives, en assignant à un ensemble de divisions au sein d'une même classe une même valeur différente de 0.
  Quand une classe exclusive est définie, une note déclenchée par l'une des divisions de la classe exclusive termine tous les autres sons de la même classe.
  La portée d'une classe exclusive est l'ensemble dans lequel la classe est définie (un son produit dans un instrument terminera le son d'un autre instrument si les deux instruments se trouvent dans le même ensemble).
  Cet effet peut être utilisé pour des instruments de type percussion comme une cymbale.
  En résumé, jouer une note en termine automatiquement une autre dans le même groupe.

Chorus et réverbération
: Indique l'intensité du chorus et de la réverbération.
  Ces effets sont configurables dans les [préférences](manual/settings.md#doc_sound) du logiciel.

Note fixe et vélocité fixe
: Force la valeur de la note et de la vélocité sur toute l'étendue de la division.

Offsets
: Change les positions de début et de fin d'un son pendant la lecture, ainsi que les positions de début et fin de la boucle.
  Un offset s'exprime en nombre de points.


### Facilités d'édition dans la table


Pour faciliter et accélérer l'édition dans une table, les fonctionnalités suivantes ont été implémentées&nbsp;:

* la molette de la souris peut être utilisée pour modifier une valeur (la cellule doit d'abord se trouver en mode édition),
* un clavier MIDI externe (ou le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard)) peut être utilisé pour définir une note de base, une étendue de notes et une étendue de vélocités (la cellule doit d'abord se trouver en mode édition),
* une sélection multiple, suivie de l'édition de la dernière cellule, édite toutes les cellules simultanément,
* les touches :kbd:`Suppr` et :kbd:`Retour arrière` effacent le contenu des cellules sélectionnées,
* les divisions stéréo peuvent être éditées simultanément si l'option est cochée dans les [préférences](manual/settings.md#doc_general) du logiciel,
* les notes peuvent être nommées par un nombre ou un texte (le Do central pouvant être Do3, Do4 ou Do5), selon l'option choisie dans les [préférences](manual/settings.md#doc_interface),
* copier / coller / couper un ensemble de cellules (pas forcément adjacentes) est possible avec :kbd:`Ctrl`+:kbd:`C` / :kbd:`Ctrl`+:kbd:`V` / :kbd:`Ctrl`+:kbd:`X`.

N'oubliez pas les [outils](manual/soundfont-editor/tools/instrument-tools.md) disponibles&nbsp;!
En particulier l'outil permettant de [paramétrer en fonction des notes](manual/soundfont-editor/tools/instrument-tools.md#doc_global).


### Rendre des divisions muettes {#doc_solo}


À des fins de test, il est possible de rendre muette une division ou de rendre muettes toutes les divisions sauf une dans les tables.
Les options sont accessibles par un clic droit sur l'entête d'une division ainsi que dans le menu contextuel de l'[arborescence](manual/soundfont-editor/tree.md#doc_menu).


![Rendre des divisions muettes dans une table](images/table_mute_divisions.png "Rendre des divisions muettes dans une table")


## Éditeur d'étendues {#doc_range}


L'éditeur d'étendues vous permet de disposer rapidement et graphiquement les échantillons selon des étendues de notes et de vélocités.
Le graphique met en avant certains problèmes de configuration, comme des zones non couvertes par des échantillons ou des superpositions non souhaitées.


![Éditeur d'étendues](images/edit_range.png "Éditeur d'étendues")


Chaque échantillon est représenté par un rectangle semi-transparent, de manière à ce que la superposition de plusieurs éléments soit identifiable.


### Sélection de rectangles


Un simple clic gauche sur un rectangle le sélectionne et enlève toutes les autres sélections.
Si plusieurs rectangles sont superposés, des clics consécutifs changent la sélection sous le curseur de la souris.
Un double-clic sur un rectangle ouvre l'[échantillon](manual/soundfont-editor/editing-pages/sample-editor.md) correspondant (ou l'instrument correspondant dans l'[éditeur d'ensembles](manual/soundfont-editor/editing-pages/preset-editor.md)).

Une sélection multiple est possible&nbsp;:

* en maintenant la touche :kbd:`Ctrl` enfoncée puis en cliquant sur les différents rectangles pour les sélectionner un par un,
* en utilisant la touche :kbd:`Maj`&nbsp;: tous les rectangles entre le premier rectangle cliqué et le suivant sont sélectionnés.


### Modifier les rectangles


Après avoir sélectionné au moins un rectangle, il est possible avec le bouton gauche de la souris de&nbsp;:

* changer la taille, en saisissant un des côtés d'un rectangle&nbsp;;
* changer la position, en pointant au centre d'un rectangle.

Si plusieurs rectangles sont sélectionnés, tous les changements sont effectués simultanément.
La touche :kbd:`Échap` annule un changement en cours.


### Déplacement et zoom


Maintenir le clic droit permet de modifier le zoom autour du point de départ.

Maintenir le clic gauche permet de déplacer le graphique si le point initial n'est pas sur un rectangle.

Un clic sur le bouton du milieu joue une note&nbsp;: la note est donnée par la position horizontale de la souris et la vélocité par sa position verticale.


## Éditeur d'enveloppes {#doc_envelope}


L'éditeur d'enveloppes fournit un aperçu graphique des 8 paramètres définissant l'enveloppe de volume et des 8 paramètres définissant l'enveloppe de modulation.
C'est ainsi un outil permettant de mieux comprendre l'impact de chacun de ces paramètres.


![Éditeur d'enveloppes](images/edit_envelope.png "Éditeur d'enveloppes")


Les boutons au-dessus du graphique sélectionnent alternativement l'enveloppe de volume (affichée en vert) ou l'enveloppe de modulation (affichée en rouge) pour l'édition.
Pendant l'édition de l'enveloppe de modulation, l'enveloppe de volume est affichée en trait fin.

La partie gauche du graphique représente l'évolution de l'enveloppe lorsqu'une touche est **appuyée**.
7 des 8 paramètres sont impliqués dans cette représentation&nbsp;:

Délai
: Durée pendant laquelle l'enveloppe reste à 0 juste après le déclenchement d'une touche. Dans la majorité des cas, on souhaite entendre le son immédiatement après avoir pressé une touche et ce paramètre reste donc à 0. Cependant le **délai** peut être utile dans le cas d'un son évoluant au cours du temps, construit à partir de plusieurs couches sonores. Supposons que l'on souhaite un second son introduit 0,5 s après l'appui sur une touche, on entrera donc «&nbsp;0,5&nbsp;» dans ce champ. Il est à noter qu'un délai ne retarde pas la lecture du son — il annule simplement son volume. Après avoir mis un délai de 0,5 s, le son sera entendu après une durée de 0,5 s pendant laquelle il aura été lu en silence.

Attaque
: Durée après le **délai** pendant laquelle l'enveloppe passe de 0 à sa valeur maximale. Par exemple, un son de corde frottée comme un violon possède une attaque relativement longue, alors qu'un son de corde frappée comme une guitare a une attaque courte. Même lorsque l'on souhaite utiliser une attaque aussi courte que possible, il est conseillé d'inscrire une valeur d'au moins 0,001, sans quoi un «&nbsp;clic&nbsp;» pourrait se faire entendre après avoir appuyé sur une touche.

Maintien
: Durée après l'**attaque** pendant laquelle l'enveloppe se maintient à sa valeur maximale.

Déclin
: Durée après le **maintien** pendant laquelle l'enveloppe passe progressivement de sa valeur maximale à la valeur spécifiée dans le **soutien**. Lorsqu'aucune réduction n'est spécifiée dans le **soutien** (0 dB, valeur par défaut), le volume pendant le **déclin** reste à son maximum quelle que soit la valeur renseignée.

Soutien
: Valeur maintenue après le **déclin** tant que la touche enfoncée n'est pas relâchée. Tout comme l'**atténuation**, la valeur de soutien s'exprime en réduction de décibels. Dans le cas où l'on souhaite diminuer le son de moitié après la phase de déclin, une valeur de 6 dB devra être renseignée. Le nombre «&nbsp;144&nbsp;» peut être renseigné si l'on souhaite couper complètement le son.

Note → Maintien
: Modification de la durée de **maintien** en fonction de la note (voir ci-dessous).

Note → Déclin
: Modification de la durée de **déclin** en fonction de la note (voir ci-dessous).

Une valeur positive dans **Note → XXX** réduira progressivement la durée de XXX vers les aigus, alors qu'une valeur négative fera l'inverse.
Par exemple&nbsp;:

* la valeur 100 réduit de moitié la durée de XXX pour chaque octave au-dessus du Do central (60) et double la durée de XXX pour chaque octave en dessous du Do central,
* la valeur -100 double la durée de XXX pour chaque octave au-dessus du Do central (60) et réduit de moitié la durée de XXX pour chaque octave en dessous du Do central.

Ces paramètres peuvent être utiles lors de la conception d'une guitare ou d'un piano avec la pédale sustain enfoncée&nbsp;: plus le son est grave et plus il dure avant de s'estomper entièrement.

La partie droite représente l'évolution de l'enveloppe quand une touche est **relâchée**.
Seule la durée de **relâche** est impliquée dans cette représentation.
L'enveloppe retourne progressivement à la valeur 0 pendant cette durée.
Les sons de violons ou de pads ont une durée de relâche relativement longue, alors que d'autres sons comme ceux de l'orgue s'arrêtent plus brutalement. Dans la pratique, même pour des relâches courtes, il est conseillé d'utiliser une valeur d'au moins 0,2 s car une valeur plus faible ne donne pas un rendu très plaisant ou naturel (mais cela dépend de l'instrument que l'on souhaite créer).

Lorsqu'une seule division d'un instrument est sélectionnée dans l'arborescence, le son de l'[échantillon](manual/soundfont-editor/editing-pages/sample-editor.md) correspondant est affiché en fond.
Il est ainsi possible de mieux visualiser l'évolution de l'enveloppe par rapport à l'échantillon en train d'être joué.


### Déplacer les points


Chaque point de la courbe peut être déplacé avec le bouton gauche de la souris&nbsp;:

* les fins du **délai**, de l'**attaque**, du **maintien** et de la **relâche** se déplacent horizontalement&nbsp;;
* la fin du **déclin** se déplace dans les deux directions&nbsp;: horizontalement pour le **déclin**, verticalement pour le **soutien**&nbsp;;
* le point où la touche est relâchée se déplace verticalement, pour le **soutien**.

Les paramètres du point sous le curseur de la souris et leurs valeurs sont affichés dans un coin supérieur du graphique.
Lorsque plusieurs points sont au même endroit, le cercle est divisé en parties, de la première étape à gauche jusqu'à la dernière à droite.

Les paramètres **Note → Maintien** et **Note → Déclin** se modifient avec une petite réglette qui traverse la fin du **maintien** ou du **déclin**&nbsp;: son extrémité haute indique la durée pour la note la plus aiguë de la division, son extrémité basse la durée pour la note la plus grave.
Déplacer une extrémité ne change que la durée de sa note, l'autre extrémité restant en place&nbsp;; en maintenant la touche :kbd:`Ctrl` enfoncée, les deux extrémités bougent autour du milieu de la réglette.
La réglette apparaît lorsque la souris s'approche du point et reste visible lorsque le paramètre est différent de 0.

La touche :kbd:`Maj` permet un réglage plus fin.
Pousser un point contre le bord droit du graphique fait défiler la vue ou effectue un zoom arrière, pour atteindre des durées plus longues.
La touche :kbd:`Échap` annule un changement en cours.
Un double-clic sur un point réinitialise ses paramètres (uniquement **Note → Maintien** ou **Note → Déclin** pour une extrémité de la réglette)&nbsp;: la valeur de la division :guilabel:`Global` ou la valeur par défaut s'applique à nouveau.

Si plusieurs divisions ou instruments sont sélectionnés, toutes leurs enveloppes sont modifiées ensemble&nbsp;: les durées dans la même proportion, les autres valeurs du même écart.


### Déplacement et zoom


Maintenir le clic droit permet de modifier le zoom autour du point de départ.

Maintenir le clic gauche permet de déplacer le graphique si le clic ne commence pas sur un point de la courbe.


## Vue du timbre {#doc_timbre}


La vue du timbre montre, tout au long du clavier, le timbre d'un instrument **tel qu'il est entendu**&nbsp;: le niveau de chaque harmonique, une fois chaque échantillon transposé vers les notes qu'il joue et une fois appliqués l'atténuation, le filtre et les modulateurs de sa division.
Les irrégularités deviennent visibles, en particulier les cassures aux jonctions entre échantillons.
À partir de cette vue, des notes calculées peuvent être créées, des échantillons redondants supprimés, et le timbre ou le niveau d'un échantillon corrigé.
La vue mesure et propose, rien n'est appliqué automatiquement.


![Vue du timbre](images/edit_timbre.png "Vue du timbre")


### Divisions prises en compte


La vue montre ce qui est joué à la vélocité choisie en haut à gauche (100 par défaut, puis la dernière utilisée).
La barre voisine montre les couches de vélocité de l'instrument&nbsp;: un clic sur une couche choisit la vélocité en son milieu.

Les divisions prises en compte sont celles qui jouent à cette vélocité, qui ne sont pas [muettes](#doc_solo) et qui ne sont pas jouées seulement au relâchement de la touche (mode de lecture «&nbsp;relâchement&nbsp;»).
Rendre une division muette est donc le moyen de l'exclure de la vue, par exemple un bruit ou un son de relâchement.
Quand le [solo sur sélection](manual/soundfont-editor/tree.md#doc_menu) est activé, les divisions muettes suivent la sélection et ne sont pas prises en compte.
Les divisions jouant une même note (paire stéréo, couches) sont combinées&nbsp;: leurs puissances s'additionnent.

Une **note de base** regroupe toutes les divisions ayant cette note de base, éventuellement avec plusieurs étendues de notes.
Les échantillons sont analysés en arrière-plan (le nombre d'échantillons restants est affiché en haut à droite)&nbsp;: les harmoniques sont mesurées sur la boucle, ou sur la partie stable du son s'il n'y a pas de boucle.
Ces analyses sont conservées tant que les données, la boucle, la note de base ou la correction de hauteur d'un échantillon ne changent pas.


### Carte


Chaque colonne est une note, alignée sur le clavier affiché en dessous, et chaque ligne un rang d'harmonique (H1&nbsp;: fondamentale, H2&nbsp;: deuxième harmonique...), le rang 1 étant en bas comme sur un spectrogramme.
Les premiers rangs sont affichés un par un, les suivants étant regroupés en bandes (voir [Partiels](#doc_timbre_partials)).

Au-dessus de la carte, des lignes de résumé montrent pour chaque note&nbsp;:

* :guilabel:`Niveau`&nbsp;: niveau total,
* :guilabel:`Timbre`&nbsp;: écart de timbre de la note de base à sa référence (hachuré quand la référence est moins sûre, aux extrémités du clavier),
* :guilabel:`Transposition`&nbsp;: bleu quand l'échantillon est transposé vers le bas, rouge quand il est transposé vers le haut,
* et, au choix dans le menu :guilabel:`Options` > :guilabel:`Résumés`&nbsp;: :guilabel:`Brillance` (centre de gravité des rangs), :guilabel:`Impairs / pairs` (rangs impairs comparés aux rangs pairs), :guilabel:`Bruit` (rapport harmoniques / bruit), :guilabel:`Justesse` (cents par rapport à la note la plus proche du tempérament égal) et :guilabel:`Boucle` (qualité de la boucle).

Sur la carte, des lignes verticales marquent les jonctions entre divisions (plus épaisses quand l'échantillon change), un point marque chaque note de base, des hachures montrent les rangs absents (hors de la bande de l'échantillon ou de la sortie, au-dessus de la limite haute) ou mesurés de façon peu sûre, et les notes qui ne jouent rien sont grisées.
Une infobulle donne les valeurs de la case sous le curseur de la souris&nbsp;: note, rang, niveau, écart, échantillons, transposition et facteur de durée (l'attaque, le trémolo et les battements sont plus rapides quand un échantillon est transposé vers le haut).

La liste :guilabel:`Affichage` choisit ce que représentent les couleurs&nbsp;:

Niveaux absolus
: Niveau de chaque rang en dB.

Niveaux relatifs
: Niveau de chaque rang par rapport au plus fort de la note&nbsp;: le timbre indépendamment du niveau.

Écart à la référence
: Différence entre la note de base et sa référence pour chaque rang, sur toutes les notes jouées par la note de base&nbsp;: trop fort (rouge) ou trop faible (bleu).

Sauts
: Différence avec la note précédente&nbsp;: les cassures aux jonctions.

Transposition
: Demi-tons entre la note et la note de base de son échantillon.

Confiance
: Part de la puissance mesurée de façon sûre.

Le menu :guilabel:`Options` permet aussi d'afficher le clavier entier ou une seule colonne par note de base, de colorer par paliers des seuils (les écarts dans la tolérance étant neutres), et de jouer les notes cliquées.
Le clavier sous la carte joue l'instrument à la vélocité de la vue.


### Référence et décisions


La **référence** d'une note de base est ce que prédisent ses voisines&nbsp;: les notes de base voisines sont jouées à sa note (avec les paramètres de leurs divisions) et leurs niveaux sont interpolés rang par rang, en dB.
Aux deux extrémités du clavier, la référence est extrapolée à partir des deux voisines du même côté, ce qui est moins sûr.
Avec :guilabel:`Régression locale`, une régression robuste sur plusieurs voisines de chaque côté est utilisée à la place, pour qu'une voisine défectueuse pèse moins.

L'écart d'une note de base à sa référence est résumé par deux nombres&nbsp;:

* l'écart de **niveau**, différence des niveaux totaux, qui se corrige par l'atténuation,
* l'écart de **timbre**, moyenne des écarts des lignes une fois l'écart de niveau retiré, chaque ligne pesant selon son intensité (une harmonique faible compte, sans dominer).

La bande :guilabel:`Décisions` donne une étiquette à chaque note de base et à chaque jonction, avec une forme et une couleur&nbsp;:

Conforme ✓ (vert)
: L'écart de timbre est dans la tolérance.

Redondante − (bleu)
: La note de base est conforme, et la supprimer changerait ses notes de moins que le seuil :guilabel:`Redondance`, sans cassure à la nouvelle jonction&nbsp;: elle peut être supprimée.

À corriger ~ (orange)
: L'écart de timbre est entre la tolérance et le maximum&nbsp;: le timbre peut être corrigé.

Trop différente × (rouge)
: L'écart de timbre dépasse le maximum&nbsp;: la note de base peut être supprimée ou remplacée par une note calculée.

Cassure + (violet)
: À un changement d'échantillon, le saut de timbre dépasse le seuil :guilabel:`Cassure` et une note sans échantillon existe entre les deux notes de base&nbsp;: une note calculée peut y être créée.

Niveau ▲ ▼ (jaune)
: L'écart de niveau dépasse le seuil :guilabel:`Niveau` (▲ trop fort, ▼ trop faible)&nbsp;: l'atténuation peut être corrigée.

Les seuils se règlent dans le menu :guilabel:`Seuils`.
Leurs valeurs par défaut (:guilabel:`Tolérance` 4,5&nbsp;dB, :guilabel:`Maximum` 8&nbsp;dB, :guilabel:`Redondance` 2,5&nbsp;dB, :guilabel:`Cassure` 6&nbsp;dB, :guilabel:`Niveau` 2,5&nbsp;dB) viennent de jeux d'orgue échantillonnés, où 70&nbsp;% des notes de base s'écartent de moins de 4,5&nbsp;dB de leurs voisines&nbsp;: elles peuvent être adaptées à l'instrument.
Une jonction ou un écart peut être voulu (un instrument qui en devient un autre, une reprise de mixture)&nbsp;: les étiquettes ne sont que des propositions.


### Partiels {#doc_timbre_partials}


Le menu :guilabel:`Partiels` règle les lignes de la carte, partagées par l'affichage, les écarts et les corrections&nbsp;:

Rangs affichés un par un
: Nombre de rangs affichés et comparés un par un (10 par défaut). L'oreille distingue individuellement les premières harmoniques, les suivantes étant perçues ensemble comme une brillance.

Rangs suivants
: Les rangs suivants sont regroupés en bandes d'un tiers d'octave (11-12, 13-16, 17-20...) ou d'une octave, pour que de nombreux rangs aigus et faibles ne pèsent pas plus que les premiers.

Limite haute
: Les partiels au-dessus de cette fréquence sont ignorés (16&nbsp;kHz par défaut, ou la moitié de la fréquence d'échantillonnage).

Dynamique
: Les rangs plus faibles que cette valeur sous le plus fort de la note ne sont pas comparés (60&nbsp;dB par défaut).


### Détail et actions


Un clic sur une note montre sous la carte ce qu'elle joue&nbsp;: les échantillons avec leur transposition, leur atténuation et leur filtre, le facteur de durée, la hauteur, les écarts de la note de base et ce que changerait sa suppression.
Le graphique compare les niveaux entendus à la référence, ou à la note calculée que pourrait recevoir une note sans échantillon (colonne en pointillés sur la carte).

Un clic sur une note de base sélectionne ses divisions dans l'[arborescence](manual/soundfont-editor/tree.md), et la sélection de divisions dans l'arborescence sélectionne leurs notes de base.
Plusieurs notes de base peuvent être sélectionnées avec :kbd:`Ctrl` ou :kbd:`Maj`, ou en faisant glisser la souris sur des notes.

Les boutons à droite et le menu contextuel de la carte s'appliquent aux notes de base sélectionnées&nbsp;:

Créer une note ici...
: Sur une note sans échantillon&nbsp;: la note de base la plus proche est transposée et les niveaux de ses harmoniques sont interpolés (ou extrapolés) à partir d'une deuxième note de base. Les notes de base utilisées peuvent être choisies, et un avertissement apparaît quand la note en est éloignée. Les étendues de notes autour sont ajustées&nbsp;; les divisions entièrement comprises dans l'étendue de la nouvelle note sont supprimées.

Supprimer...
: Les divisions de la note de base sont supprimées et ses voisines prennent ses notes. Les échantillons qui ne sont plus utilisés peuvent être supprimés aussi.

Remplacer par une note calculée...
: La note de base est supprimée, puis recréée à partir de ses voisines.

Corriger le timbre...
: Ouvre la fenêtre de [correction](#doc_timbre_correction).

Corriger le niveau
: L'atténuation des divisions est modifiée pour que l'écart de niveau devienne nul (une atténuation ne peut pas être négative).

Vérifier par un rendu
: La note est jouée par le moteur de son et les harmoniques du résultat sont mesurées&nbsp;: elles s'affichent sur le graphique pour vérifier le calcul de la vue.

Rendre muettes ces divisions
: Dans le menu contextuel&nbsp;: exclut les divisions de la vue.

Le menu :guilabel:`Proposer` sélectionne toutes les notes de base (ou jonctions) ayant une étiquette et leur applique l'action correspondante, après confirmation&nbsp;: corriger les notes de base à corriger, remplacer les trop différentes, supprimer les redondantes, créer des notes calculées aux cassures, corriger les niveaux qui s'écartent.
Elles sont traitées de la pire à la moins mauvaise, les références étant recalculées après chaque action&nbsp;: une note de base défectueuse fausse aussi la référence de ses voisines, qui paraissent à moitié défectueuses tant qu'elle n'est pas corrigée.

Chaque action, ou série d'actions, s'annule en une étape (:kbd:`Ctrl`+:kbd:`Z`).


### Correction du timbre {#doc_timbre_correction}


![Correction du timbre](images/timbre_correction.png "Correction du timbre")


La fenêtre montre pour chaque ligne les niveaux entendus, la cible et le résultat attendu, et propose les paramètres suivants&nbsp;:

Référence
: Cible de la correction&nbsp;: :guilabel:`Interpolation entre les voisines`, :guilabel:`Régression locale` sur plusieurs voisines de chaque côté, ou :guilabel:`Alignement sur une note de base` (une autre note de base jouée à la note).

Corriger le timbre (données de l'échantillon)
: Le gain de chaque ligne est appliqué aux données des échantillons avec une courbe à phase minimale qui suit la hauteur&nbsp;: la correction reste valable sur toutes les notes de la division, quelle que soit la transposition, et la boucle est conservée. La même correction s'applique aux deux échantillons d'une paire stéréo.

Corriger le niveau (atténuation)
: L'écart de niveau est corrigé par l'atténuation des divisions, ce qui ne modifie pas les données. Si l'atténuation devenait négative, le reste passe dans les données, dans la limite de la pleine échelle.

Tolérance
: Un écart plus petit d'une ligne n'est pas corrigé, seule la partie au-delà l'est.

Force
: Part de l'écart corrigée.

Correction maximale
: Plus grand gain d'une ligne, de part et d'autre.

Rangs corrigés
: Toutes les lignes, ou seulement les rangs de 1 à un rang donné.

Brillance, Rangs impairs
: Pente ajoutée à la cible (positive pour un son plus brillant), et niveau ajouté aux rangs impairs 3, 5, 7... de la cible.

La section :guilabel:`Écoute` compare le son original (A) et le son corrigé (B) avec le bouton :guilabel:`Lecture`.
Si la correction dépassait la pleine échelle, l'échantillon est baissé et l'atténuation compense.
Quand un échantillon est aussi utilisé par d'autres divisions ou instruments, Polyphone demande s'il faut le dupliquer pour les notes de base corrigées ou le modifier partout (la réponse peut être mémorisée jusqu'à la fermeture de Polyphone).


## Modulateurs {#doc_modulator}


La partie basse de l'éditeur des instruments est dédiée à la création des modulateurs, de manière globale pour l'instrument entier ou pour une division particulière.
Lorsque vous jouez d'un instrument, un modulateur écoute un ou deux signaux MIDI et modifie l'un des paramètres de la table selon la règle mathématique configurée.
La sortie d'un modulateur peut aussi être utilisée comme entrée d'un autre modulateur (cette possibilité est apparue avec la version 2.04 du [format sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2) et peut ne pas être prise en charge par tous les synthétiseurs).


![Édition des modulateurs](images/modulator_editor_2.png "Édition des modulateurs")


Les boutons à gauche sont prévus pour&nbsp;:

* afficher / masquer la zone des modulateurs,
* ajouter / supprimer un modulateur,
* copier / coller / dupliquer un modulateur ou plus.

Lorsqu'un modulateur est ajouté, la bordure de la cellule correspondante dans la table s'épaissit (comme la cellule d'accordage dans la colonne :guilabel:`Global` ci-dessus).

Les modulateurs d'une division (ou de la division globale) ne sont affichés que si une seule division est sélectionnée.
Lorsque la section est masquée, un résumé indique le nombre de modulateurs et les paramètres qu'ils modifient.
La sélection de cellules dans la table sélectionne les modulateurs agissant sur les paramètres correspondants, et inversement.

Chaque modulateur est composé des éléments suivants, de gauche à droite&nbsp;:

Sources
: La première source est un signal MIDI&nbsp;: la vélocité ou la note jouée, la pression polyphonique, la pression globale, le Pitch Bend, la sensibilité du Pitch Bend ou un contrôleur MIDI (:guilabel:`CC`).
  Ce peut aussi être la sortie d'un ou plusieurs autres modulateurs (:guilabel:`Lien`), ceux qui ont ce modulateur pour destination.
  La seconde source est optionnelle et modifie l'effet de la première.

Courbes
: À côté de chaque source, un bouton donne accès à une grille de courbes pour choisir la manière dont le signal est converti&nbsp;: linéaire, concave, convexe ou commutateur, ascendante ou descendante, unipolaire (de 0 à 1) ou bipolaire (de -1 à 1).

Quantité
: Amplitude maximale de la modulation.

Transformation
: :guilabel:`aucune opération` ou :guilabel:`valeur absolue` (la valeur absolue du résultat de la modulation est utilisée).

Destination
: Le paramètre modifié par le modulateur, ou un autre modulateur (:guilabel:`Modulateur`).
  Le texte sous la destination indique l'étendue de la modulation&nbsp;: les valeurs ajoutées au paramètre (:guilabel:`Ajout de` … :guilabel:`À`) ou les facteurs qui lui sont appliqués (:guilabel:`Multiplication de` … :guilabel:`À`).

Lorsqu'un modulateur est identique à un modulateur situé plus haut dans la liste, la mention :guilabel:`surchargé par` est affichée.
Au niveau des instruments, la mention :guilabel:`surcharge un mod. par défaut` indique un modulateur qui remplace l'un des [modulateurs par défaut](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod) du format sf2, et la mention :guilabel:`désactive un mod. par défaut` indique que sa quantité vaut 0 (le modulateur par défaut est alors désactivé).
