Le but de ce tutoriel est de créer des instruments expressifs en utilisant des échantillons bouclés ayant une relâche enregistrée.
Vous apprendrez comment utiliser pleinement les capacités d'un tel échantillon, en utilisant son attaque, son maintien et également sa relâche.


## Relâche classique ou relâche personnalisée


Lorsque vous appuyez sur une touche vous entendez tout d'abord le début de l'échantillon (l'attaque) et ensuite, aussi longtemps que vous ne relevez pas la touche, la boucle au sein de l'échantillon sera jouée encore et encore (la phase de maintien).
Lorsque vous relevez la touche, la boucle sera encore utilisée et le son sera progressivement atténué jusqu'à ce qu'il ne soit plus audible.
Ceci est la méthode classique pour faire une relâche avec les banques de sons&nbsp;: elle est facile à configurer mais elle ignore toutes les harmoniques transitoires qui pourraient apparaître lors de l'arrêt réel du son d'un instrument acoustique.
Ce type de relâche peut être vu comme une simple extension de la phase de maintien.

Une relâche plus réaliste est d'utiliser la vraie relâche provenant d'un échantillon enregistré.
L'attaque et la phase de maintien seront jouées de la même manière mais lorsque vient la relâche d'une note, la phase de maintien s'arrête rapidement pour laisser place à la relâche enregistrée qui va alors commencer à être lue.
Une relâche personnalisée est utilisée plutôt qu'une extension de la phase de maintien, ajoutant ainsi des détails au son.

Créer un tel instrument est possible en séparant le problème en deux&nbsp;:

* l'attaque et la phase de maintien sont définies dans un premier temps,
* la relâche est ensuite déclenchée.

L'exemple qui va être utilisé ici est le son d'une trompette sur l'orgue, pour son grand contenu harmonique et sa progression très perceptible.


## Préparation des échantillons


Deux cas peuvent apparaître&nbsp;: vous avez un fichier contenant l'échantillon complet de l'attaque jusqu'à la relâche, ou vous possédez un fichier additionnel contenant seulement la relâche qui a été extraite au préalable.


### Échantillon complet avec relâche


Chargez l'échantillon 2 fois dans Polyphone et ensuite, dans la seconde instance de l'échantillon, effacer tout dans l'échantillon sauf la relâche.
La suppression peut être effectuée en utilisant la touche :kbd:`Alt` et en surlignant la zone à supprimer dans le [graphique](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) de l'échantillon.
Pour trouver le point où la relâche commence, jouer l'échantillon plusieurs fois et essayez de trouver l'endroit où vous pensez qu'il n'y a plus d'énergie pour nourrir le son.
Cette dernière partie sera la relâche.
Suivez ensuite les instructions ci-dessous.


### Relâche séparée de l'échantillon principal


Échantillons fournis pour l'exemple&nbsp;: <a href="files/trumpet.wav" download>échantillon 1</a>, <a href="files/trumpet-release.wav" download>échantillon 2</a>.

Si vous avez 2 échantillons, le premier contenant le début et la phase de maintien du son et le second contenant la relâche du son, vous avez seulement besoin de vous assurer que la relâche commence avec un blanc très court&nbsp;: quelques points ayant une valeur proche de 0.
Une boucle courte et inaudible sera ainsi faite sur cette portion.

Bouclez et accordez le premier échantillon comme expliqué dans le tutoriel «&nbsp;[Comment préparer un échantillon](tutorials/how-to-prepare-a-sample.md)&nbsp;».

Bouclez le second échantillon sur une partie très courte au début, par exemple du point 1 au point 2 ou 3.
Spécifiez la même note de base et la même correction que pour le premier échantillon.


## Premier instrument&nbsp;: attaque et phase de maintien


Créez un premier instrument avec les échantillons contenant l'attaque et la phase de maintien du son.
Spécifiez les paramètres globaux ainsi&nbsp;:

* choisissez l'option de lecture avec la boucle (2ème choix),
* spécifiez 0.01s comme temps de relâche pour l'envelope de volume (relâche très courte).

Vous avez maintenant l'instrument qui ne contient pas de relâche.


## Second instrument&nbsp;: relâche


Créez un second instrument pour la relâche contenant les échantillons de relâche avec un blanc très court au début.
Configurez-le de cette manière&nbsp;:

* sélectionnez l'option de lecture :guilabel:`boucle + relâche` dans :guilabel:`lecture en boucle` (3ème choix),
* sélectionnez un grand nombre comme 100 secondes pour le paramètre :guilabel:`Vol env release (s)` de manière à ce que la relâche ne soit pas atténuée,
* déterminez approximativement la durée de l'attaque du premier échantillon et utilisez cette valeur en tant qu'attaque de l'envelope de volume.

Vous avez à présent un instrument silencieux tant que vous ne relâchez pas la note et qui déclenche ensuite la relâche.
Vous pouvez aussi noter qu'un appui très bref sur une note déclenche la relâche d'une manière atténuée.
Ce point est peut-être délicat à entendre dans le cas de notre trompette car son attaque est très rapide.
La durée de l'attaque a donc été légèrement augmentée dans l'exemple.


## Assemblage des deux instruments


Créez un ensemble en liant les deux instruments précédemment créés.
Le premier instrument est déclenché lorsqu'une touche est appuyée et le second instrument est déclenché lorsque la touche est relâchée.
Vous pouvez ensuite entendre le résultat et essayer de rendre la transition plus douce entre la phase de maintien et la relâche&nbsp;:

* en atténuant le premier ou le second instrument,
* en augmentant la durée de la relâche dans le premier instrument.

Le résultat avec les échantillons en exemple peut être téléchargé <a href="files/custom%20release.sf2" download>ici</a>.

Vous pouvez écouter ci-dessous la différence entre&nbsp;:

* une <a href="files/classic-release.mp3" download>relâche classique</a>

![](files/classic-release.mp3)

* et une <a href="files/custom-release.mp3" download>relâche personnalisée</a>

![](files/custom-release.mp3)