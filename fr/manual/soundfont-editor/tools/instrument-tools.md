Quand un instrument ou l'une de ses divisions est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md), la [boîte à outils](manual/soundfont-editor/tools/index.md) contient différents outils répartis dans quatre catégories&nbsp;:

* [analyse](#doc_cat_analyze),
* [édition rapide](#doc_cat_fast),
* [modulateurs](#doc_cat_mod),
* [transformation](#doc_cat_transformation).

Utiliser les outils simultanément sur plusieurs instruments est possible (avec une sélection multiple), sauf pour créer des accords, des mixtures et pour visualiser les paramètres.


## Analyse {#doc_cat_analyze} 


### Visualiser les paramètres {#doc_display}


Cet outil affiche l'évolution d'un paramètre en fonction de la note.
Une échelle logarithmique est disponible (attention aux valeurs égales ou inférieures à 0&nbsp;!).


![Outil de visualisation des paramètres](images/tool_display_parameters.png "Outil de visualisation des paramètres")


## Édition rapide {#doc_cat_fast}


### Modifier les atténuations {#doc_attenuation}


L'outil calcule les atténuations minimales et maximales dans tous les instruments sélectionnés, et vous donne la possibilité d'ajouter une valeur à toutes les atténuations sans dépasser les limites.

Par exemple, si l'atténuation minimale des divisions au sein des instruments sélectionnés est de 16 dB et que l'atténuation minimale désirée est de 5 dB (pour augmenter le volume général), l'atténuation de toutes les divisions de tous les instruments peut être diminuée de 11 dB.


![Outil de modification des atténuations](images/tool_change_attenuation.png "Outil de modification des atténuations")


### Désaccorder {#doc_detune}


Cette fonction désaccorde légèrement les notes d'un instrument, de manière à produire un effet ondulant en conjonction avec un instrument accordé.
L'intensité de l'effet s'exprime en nombre de battements par seconde et est ajustable&nbsp;:

* nombre de battement par seconde au do 60, au milieu du clavier&nbsp;;
* évolution du nombre de battements en fonction de la note, en divisant ou multipliant le nombre de battements par un facteur constant d'une octave à l'autre (une valeur de "1" signifie que le nombre de battements sera constant sur tout le clavier).


![Outil de désaccordage](images/tool_detune.png "Outil de désaccordage")


Voici l'exemple d'un instrument <a href="downloads/tutorials/celeste_tuning_off.mp3" download>accordé</a>&nbsp;:

![](downloads/tutorials/celeste_tuning_off.mp3)

Et cet instrument a été combiné avec ce même instrument <a href="downloads/tutorials/celeste_tuning_on.mp3" download>désaccordé</a>&nbsp;:

![](downloads/tutorials/celeste_tuning_on.mp3)

Le fichier .sf2 correspondant peut être téléchargé <a href="downloads/tutorials/celeste_tuning.sf2" download>ici</a>.


### Paramétrer en fonction des notes {#doc_global}


Cet outil permet l'édition d'un paramètre pour toutes les divisions d'un instrument simultanément.


![Outil de paramétrage en fonction des notes](images/tool_keybased_configuration.png "Outil de paramétrage en fonction des notes")


Le graphique représente l'intensité de la modification, la première valeur sur la gauche correspondant à la modification appliquée à la division contenant la note 0, et la dernière valeur sur la droite correspondant à la modification appliquée à la division contenant la note 127.
En changeant le motif, il est possible d'éditer la courbe&nbsp;:

* manuellement avec la souris,
* en utilisant une loi linéaire,
* en utilisant une loi exponentielle,
* en utilisant une génération aléatoire.

Dans le cas où une règle linéaire ou exponentielle est utilisée, un clic gauche sur le graphique définit le début de la zone linéaire ou exponentielle et un clic droit définit la fin.

Le paramètre à modifier peut être sélectionné dans :guilabel:`Paramètres` et le type de modification est choisi dans :guilabel:`Modification`&nbsp;:

Addition
: Le modificateur est ajouté à la valeur du paramètre choisi.

Multiplication
: Le modificateur est multiplié avec la valeur du paramètre choisi.

Remplacement
: Le modificateur remplace la valeur du paramètre choisi.

Il est possible de n'appliquer les changements qu'aux divisions dont l'étendue de vélocités est comprise dans une étendue spécifique, utile par exemple pour le cas d'un piano qui auraient plusieurs couches de vélocités.


### Relâche naturelle {#doc_release}


Cet outil crée une relâche pour chaque division de l'instrument.
Les durées de relâche sont calculées en fonction des deux premiers paramètres et une modification du ton peut être ajoutée en utilisant le troisième.


![Outil de relâche naturelle](images/tool_natural_release.png "Outil de relâche naturelle")


### Auto-positionnement des échantillons {#doc_position}


Le logiciel détermine une étendue de notes pour toutes les divisions d'un instrument, de manière à optimiser leur utilisation sur toute l'étendue du clavier.


### Spatialiser le son {#doc_spatialization}


Cet outil spatialise les sons en fonction de leur note, ce qui peut être utile pour un piano par exemple (les basses à gauche et les aigus à droite).


![Outil de spatialisation](images/tool_sound_spatialization.png "Outil de spatialisation")


Les barres dans le graphique représentent chacune une corde ou un tuyau, ayant chacune une longueur en relation avec la hauteur de note.
La barre la plus courte correspond à la note la plus aiguë et la barre la plus longue correspond à la note la plus grave.

Chaque note est positionnée dans l'espace, l'extrême gauche correspondant à un son venant de la gauche et l'extrême droite correspondant à un son venant de la droite.
Plusieurs motifs peuvent être sélectionnés&nbsp;:

Ascendant
: Les aigus sont à gauche, les basses sont à droite.

Descendant
: Les basses sont à gauche, les aigus sont à droite.

Creux
: Les basses sont aux deux extrémités, les aigus sont centrés.

Pointe
: Les aigus sont aux deux extrémités, les basses sont centrées.

Aléatoire
: Les aigus et les basses sont positionnés aléatoirement.

D'autres options sont disponibles&nbsp;:

* nombre de répétitions du motif (:guilabel:`Nombre divisions`),
* intensité de l'effet (:guilabel:`Occupation`),
* remplissage d'une division (:guilabel:`Étalement`),
* offset gauche — droite si l'occupation n'est pas de 100% (:guilabel:`Offset`),
* :guilabel:`Renversements`.

Le graphique est mis à jour à chaque fois qu'un paramètre change pour une meilleure visualisation.
Au survol de la souris, la correspondance numéro de note / balance est affichée.


### Transposer {#doc_transpose}


L'outil de transposition permet la transposition d'un instrument complet.
Le son de l'instrument est ainsi le même, mais à une position différente sur le clavier.

Comme entrée, un décalage en termes de demi-tons est requis.
Pour que l'instrument soit plus aigu, le décalage doit être positif.
Inversement, pour que l'instrument soit plus grave, le décalage doit être négatif.
Les opérations effectuées par cet outil, pour chaque division de l'instrument, sont les suivantes&nbsp;:

* la note de base est ajustée,
* modification de l'accordage fin si la transposition est effectuée avec un nombre de demi-tons contenant des décimales,
* décalage de l'étendue de notes selon le nombre de demi-tons si :guilabel:`Adapter l'étendue de notes` est coché.


![Outil de transposition](images/tool_transpose_inst.png "Outil de transposition")


## Modulateurs {#doc_cat_mod}


### Surcharger un modulateur par défaut {#doc_default_mod}


Les modulateurs par défaut sont automatiquement assignés à chaque instrument.
Cet outil peut créer des modulateurs qui les surchargeront ou les annuleront.


![Outil de surcharge des modulateurs par défaut](images/tool_default_mod.png "Outil de surcharge des modulateurs par défaut")


### Enlever les modulateurs {#doc_remove_mod}


Tous les modulateurs de tous les instruments sélectionnés seront enlevés.

**Note&nbsp;:** Les modulateurs par défaut s'appliqueront toujours.
La seule manière de désactiver un modulateur par défaut est de le surcharger avec l'outil «&nbsp;[Surcharger un modulateur par défaut](#doc_default_mod)&nbsp;».


## Transformation {#doc_cat_transformation}


### Créer des accords {#doc_chords}


Se basant sur un instrument, l'outil vous permet de créer tous les accords possibles.
Ce peut être intéressant par exemple pour la création de chœurs ou de nappes de sons


![Outil de création d'accords](images/tool_create_chords.png "Outil de création d'accords")


Voici un exemple, <a href="downloads/tutorials/chord_base.mp3" download>avant</a>&nbsp;:

![](downloads/tutorials/chord_base.mp3)

Et <a href="downloads/tutorials/chord_done.mp3" download>après</a>&nbsp;:

![](downloads/tutorials/chord_done.mp3)

Un fichier .sf2 d'exemple, se basant sur une flûte, peut être téléchargé <a href="downloads/tutorials/chords.sf2" download>ici</a>.


### Dupliquer les divisions {#doc_division}


Cette action a pour effet la duplication de toutes les divisions d'un instrument.
Par exemple, si un échantillon lié à un instrument a pour étendue de notes 36-38, le logiciel duplique cette division de manière à avoir l'échantillon lié pour les étendues de notes 36-36, 37-37 et 38-38.
Cela permet alors d'éditer plus précisément chaque paramètre d'échantillon pour chaque note différente, au lieu d'éditer des divisions entières avec les mêmes paramètres.

Il est également possible de dupliquer les divisions en fonction des étendues de vélocités.


![Outil de duplication des divisions](images/tool_division_duplication.png "Outil de duplication des divisions")


### Créer une mixture {#doc_mixture}


Spécifique à l'orgue, cet outil permet la création de mixtures à partir d'un instrument existant.
Une interface apparaît pour définir les différentes divisions de la mixture et pour chacune d'elle la liste des rangs.
Les types possibles des rangs sont très divers, allant de l'octave jusqu'à la 27ème harmonique.


![Outil de création de mixtures](images/tool_mixture.png "Outil de création de mixtures")


Pour fonctionner, cet outil s'appuie sur tous les échantillons associés à l'instrument sélectionné, l'étendue définie pour chaque échantillon et leur atténuation correspondante.
Les échantillons doivent être accordés sans tenir compte du paramétrage de l'instrument (à l'aide de l'[éditeur des échantillons](manual/soundfont-editor/editing-pages/sample-editor.md)) et la note de base spécifiée ne doit pas tenir compte de l'appartenance à un jeu (pas de transposition si le son est un principal 4' par exemple).

Une option permet un bouclage automatique des échantillons créés, une autre modifie l'intervalle de création des échantillons.
Enfin, il est possible de choisir le type des sons créés&nbsp;: mono ou stéréo.

Voici un exemple, <a href="downloads/tutorials/mixture_base.mp3" download>avant</a>&nbsp;:

![](downloads/tutorials/mixture_base.mp3)

Et <a href="downloads/tutorials/mixture_done.mp3" download>après</a>&nbsp;:

![](downloads/tutorials/mixture_done.mp3)

Un fichier .sf2 d'exemple, se basant sur un jeu d'orgue, peut être téléchargé <a href="downloads/tutorials/mixture.sf2" download>ici</a>.
