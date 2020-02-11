Un instrument construit sous la forme d'une soundfont est composé d'échantillons, provenant par exemple de l'enregistrement d'un vrai instrument.
Ce tutoriel décrit comment ajouter et préparer un échantillon dans une soundfont.


## Import


Tout d'abord, assurez-vous d'avoir des échantillons sur votre ordinateur.
Beaucoup d'échantillons gratuits peuvent être trouvés sur internet, provenant de l'enregistrement d'instruments réels (comme une flûte, guitare, violoncelle, …) ou créés à l'aide de diverses méthodes de synthèse audio.

Pour ce tutoriel, cet échantillon de flûte est fourni&nbsp;: <a href="downloads/tutorials/flute.wav" download>flute.wav</a>.

Lorsque vous l'avez, créez une nouvelle soundfont avec Polyphone (:menuselection:`Menu --> Nouveau`) et importez l'échantillon (:guilabel:`Nouvel échantillon`).


![Import d'un échantillon](images/import-sample.png "Import d'un échantillon")


## Bouclage


### Introduction


Une boucle est une zone au sein de l'échantillon qui peut être répétée pour augmenter la durée de l'échantillon.
Supposons que nous ayons un échantillon qui dure une demi-seconde, avec une boucle le son pourra être étendu jusqu'à 10 secondes ou plus.
Ceci est utile uniquement dans le cas d'échantillons non percussifs comme des flûtes, trompettes, orgues ou violons.
À l'opposé, les tambours ne requièrent aucune boucle.
Ce peut être applicable également pour un piano ou une guitare, selon la manière où les sons sont synthétisés.

Ainsi, cette section est nécessaire si et seulement si vous ressentez le besoin d'étendre la durée initiale de l'échantillon.
Si ce n'est pas le cas, vous pouvez aller directement à la prochaine section «&nbsp;Accordage&nbsp;».


### Configurer une boucle


Dans l'arborescence, cliquez sur l'échantillon que vous voulez boucler et portez votre attention sur les paramètres de boucle.


![Bouclage d'un échantillon&nbsp;: paramètres de boucle](images/loop-sample-1.png "Bouclage d'un échantillon&nbsp;: paramètres de boucle")


Si vous êtes chanceux, ces valeurs ont été enregistrées dans l'échantillon .wav et les champs sont déjà renseignés.
Si ce n'est pas le cas, ou s'ils ont été mal déterminés (ce qui peut malheureusement arriver…), cliquez arbitrairement dans le graphique (zone de visualisation WAVE) avec le bouton de droite et avec le bouton de gauche pour définir une fin et un début de boucle.
Ceci est généralement effectué dans la phase stationnaire du son, en excluant l'attaque et la relâche.
Ensuite, zoomez autour du début de boucle et essayez de superposer la courbe verte (ou rouge) et la courbe bleue en modifiant légèrement les paramètres de boucle.


![Bouclage d'un échantillon&nbsp;: alignement des ondes](images/loop-sample-2.png "Bouclage d'un échantillon&nbsp;: alignement des ondes")


### Écouter le résultat


Quand une boucle est configurée, appuyez sur :guilabel:`lecture` dans le lecteur avec la case :guilabel:`boucle` cochée pour écouter le résultat.


![Bouclage d'un échantillon&nbsp;: résultat](images/loop-sample-3.png "Bouclage d'un échantillon&nbsp;: résultat")


Si vous n'entendez pas la transition entre le début et la fin de la boucle, cela signifie que la boucle est correcte.
Sinon, maintenez la lecture de l'échantillon avec la boucle activée et modifiez légèrement la fin et / ou le début de la boucle jusqu'à ce que vous trouviez la transition la plus douce possible.
Écoutez les enregistrements suivants pour entendre la différence entre une mauvaise et une bonne boucle&nbsp;:

* <a href="downloads/tutorials/bad-loop.mp3" download>mauvaise boucle</a>

![](downloads/tutorials/bad-loop.mp3)

* <a href="downloads/tutorials/good-loop.mp3" download>boucle correcte</a>

![](downloads/tutorials/good-loop.mp3)


**Note&nbsp;:** Un outil existe pour "tricher" si vous ne pouvez pas trouver de boucles facilement&nbsp;: l'outil de [bouclage automatique](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop).
Il créera automatiquement une boucle en coupant l'échantillon et en superposant deux sections, de manière à ce que la fin de la section corresponde exactement à son début.
Voir la documentation en rapport avec l'outil pour plus d'informations.


## Accordage


Tous les échantillons doivent être accordés si vous ne voulez pas vous retrouver avec un instrument désaccordé.
En indiquant dans la soundfont la note exacte de l'échantillon importé, la soundfont sera capable de l'adapter pour n'importe quelle touche du clavier.
Certains échantillons contiennent déjà ces informations, dans quel cas vous pourriez ne pas avoir besoin de modifier les valeurs dans :guilabel:`Note de base` and :guilabel:`Correction (centièmes)`.
Mais en précaution il est toujours mieux de vérifier.


![Accordage des échantillons&nbsp;: information sur la hauteur de note](images/tune-sample-1.png "Accordage des échantillons&nbsp;: information sur la hauteur de note")


Activez la boucle si l'échantillon en contient une et appuyez sur :guilabel:`lecture` dans le lecteur.
Ensuite ajoutez le sinus de calibration et ajustez son volume de manière à entendre les deux sons aussi clairement que possible.


![Accordage des échantillons&nbsp;: lire l'échantillon avec le signal de calibration](images/tune-sample-2.png "Accordage des échantillons&nbsp;: lire l'échantillon avec le signal de calibration")


Commencez l'accordage en modifiant la note de base jusqu'à ce que les deux sons correspondent le mieux possible.
Continuez en ajustant la correction.
Pour cette étape, portez votre oreille sur les battements qui apparaissent et faites en sorte qu'ils soient le plus lent possible.
Voici des exemples contenant le son d'une flûte et le sinus de calibration&nbsp;:

* <a href="downloads/tutorials/very-bad-tune.mp3" download>très mauvais accordage</a> (la note de base est mauvaise)

![](downloads/tutorials/very-bad-tune.mp3)

* <a href="downloads/tutorials/bad-tune.mp3" download>mauvais accordage</a> (la correction n'est pas bonne et vous pouvez entendre des battements)

![](downloads/tutorials/bad-tune.mp3)

* <a href="downloads/tutorials/good-tune.mp3" download>bon accordage</a> (les deux valeurs sont correctes)

![](downloads/tutorials/good-tune.mp3)


**Note&nbsp;:** L'[analyse fréquentielle](manual/soundfont-editor/editing-pages/sample-editor.md#doc_fred) peut être un bon indicateur pour commencer l'accordage. Mais gardez en tête qu'il n'y a pas d'algorithme parfait pour cette tâche et que seules vos oreilles trouverons le bon accordage.


![Accordage des échantillons&nbsp;: analyse fréquentielle](images/tune-sample-3.png "Accordage des échantillons&nbsp;: analyse fréquentielle")


## Pour aller plus loin


Différents outils sont disponibles pour l'édition des échantillons comme&nbsp;:

* [supprimer le blanc](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) au démarrage,
* [égaliser](manual/soundfont-editor/editing-pages/sample-editor#doc_eq) le son,
* [transposer](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* [normaliser](manual/soundfont-editor/tools/sample-tools.md#doc_volume) le volume,
* [ajuster la balance](manual/soundfont-editor/tools/sample-tools.md#doc_balance) pour les échantillons stéréo.


## Note sur les échantillons stéréo


Les échantillons stéréo sont composés de deux échantillons mono.
Le lien est conservé parmi les informations des échantillons.


![Échantillon stéréo](images/stereo-sample.png "Échantillon stéréo")


Gérer deux échantillons mono peut ajouter de la difficulté pour les boucler car une boucle peut être correcte d'un côté et pas de l'autre.
Dans ce cas, n'hésitez pas à tricher en configurant des boucles différentes pour le côté gauche et pour le côté droit.
La qualité du son n'en sera pas affectée.