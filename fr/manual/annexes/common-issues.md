Cette page liste les défauts typiques des banques de sons (pouvant être relevés par des revues) et, pour chacun d'eux, des techniques et des liens vers la documentation pour les corriger&nbsp;:

* [échantillons de mauvaise qualité ou mal préparés](#doc_sample),
* [mauvais accordage](#doc_tuning),
* [enveloppe ADSR inadéquate](#doc_adsr),
* [boucles manquantes ou imprécises](#doc_loop),
* [standard MIDI non respecté](#doc_standard).


## Échantillons de mauvaise qualité ou mal préparés {#doc_sample}


Il est possible qu'une banque de sons ne fournisse pas d'instrument convaincant et si la cause provient d'échantillons de mauvaise qualité, la seule solution est de les remplacer&nbsp;:

* recherche d'autres échantillons sur internet,
* enregistrement avec un micro de meilleur qualité (ou utilisation d'outils externes de traitement du signal),
* création d'échantillons par un autre moyen.

D'autres fois, le problème peut se résoudre en préparant ou utilisant mieux les échantillons.

### Son mauvais, surtout vers les aigus

Typiquement, un seul échantillon audio ne permet pas de constituer un instrument de bonne qualité. Un échantillon possède en effet sa propre hauteur de note (sauf pour les percussions non mélodiques) et l'utiliser pour une étendue de notes très large va modifier désagréablement le son et surtout dans les aigus. Pour corriger ce défaut il est donc nécessaire d'utiliser plusieurs échantillons ou, si un seul échantillon est disponible, il est possible de&nbsp;:

* dupliquer l'échantillon,
* transposer l'échantillon créé d'une octave par exemple avec cet [outil](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* filtrer les harmoniques hautes avec cet [outil](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) ou [celui-ci](manual/soundfont-editor/tools/sample-tools.md#doc_filter).

Les échantillons seront ensuite placés sur des étendues de notes au sein desquelles leur écoute ne déforme pas le timbre initial.

### Le son contient des «&nbsp;clics&nbsp;» à intervalles réguliers

Le problème vient probablement d'une boucle mal positionnée et cet autre [paragraphe](#doc_loop) décrit le sujet.

### Échantillons mal découpés

Les échantillons ne sont souvent utilisés que partiellement et enlever le superflu permet de réduire la taille d'un fichier&nbsp;:

* la zone utile dans les échantillons est parfois située avant le point de bouclage final&nbsp;: il est donc possible de supprimer la partie de l'échantillon située après la fin de boucle et cet [outil](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) permet de le faire en quelques clics&nbsp;;
* les échantillons peuvent débuter par un blanc qu'il est important de supprimer pour diminuer la latence lors du déclenchement d'une note (il existe aussi un [outil](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) pour le faire rapidement)&nbsp;;
* enfin, des échantillons peuvent tout simplement ne pas être utilisés&nbsp;: ce [résumé de configuration](manual/soundfont-editor/configuration-summaries.md#doc_sample) les liste et cet [outil](manual/soundfont-editor/tools/global-tools.md#doc_unused) les enlève automatiquement.


## Mauvais accordage {#doc_tuning}


L'accordage d'un instrument virtuel passe par plusieurs étapes et il est nécessaire de toutes les passer en revue et dans l'ordre.

### Accordage au niveau des échantillons

Cet accordage est le plus important et est décrit sur cette [page](tutorials/how-to-prepare-a-sample.md#doc_tuning).

### Accordage au niveau des instruments

4 paramètres, décrits sur cette [page](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table), sont utilisés pour définir la transposition avec laquelle un échantillon est lu&nbsp;: **note de base**, **accordage (demi-tons)**, **accordage (centièmes)** et **réglage de tonalité**. Ces valeurs peuvent être modifiées lors de l'utilisation de l'instrument par des [modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator), qu'il est donc important de passer en revue également.

Et si aucun paramètre ou modulateur n'est défini pour la division d'un instrument associée à un échantillon, il faudra alors vérifier si des paramètres ou modulateurs sont définis dans la **division globale** de cet instrument (cette division définit le paramétrage par défaut de toutes les autres divisions).

### Accordage au niveau des ensembles

Au niveau des ensembles, 3 paramètres vont modifier la transposition avec laquelle un échantillon est lu&nbsp;: **accordage (demi-tons)**, **accordage (centièmes)** et **réglage de tonalité**. Ces valeurs s'ajoutent à la configuration des instruments présents dans les divisions de l'ensemble.

Tout comme les instruments, les ensembles peuvent contenir des modulateurs et leur division globale constitue une configuration par défaut qu'il ne faut pas oublier.


## Enveloppe ADSR inadéquate {#doc_adsr}


L'enveloppe ADSR représente l'évolution du volume lorsqu'une note est lue mais aussi lorsque cette même note est arrêtée. Chacune des 4 lettres du mot ADSR correspond à une phase particulière et l'enveloppe de volume utilisée dans les banques de sons au format Soundfont contient en réalité 6 phases&nbsp;:

* délai,
* attaque (A),
* maintien,
* déclin (D),
* sountien (S),
* relâche (R).

Ces phases sont décrites plus en détails sur cette [page](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) et ci-dessous sont listés des problèmes possibles liés à l'enveloppe.

### Un son démarre trop soudainement

Pour adoucir le démarrage d'une note, précisez ou augmentez la **durée d'attaque** de la note au niveau des [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) ou des [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md).

### Un son est trop mou à démarrer

À l'inverse du paragraphe précédent, diminuez ou supprimez la **durée d'attaque** de la note.

### Un délai est ressenti avant le démarrage du son

Trois solutions possibles et pouvant être complémentaires&nbsp;:

* supprimez la **durée du délai** si elle est précisée au niveau des [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) ou des [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md),
* supprimer le silence au début de l'échantillon audio (voir la découpe de l'échantillon sur cette [page](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) ou l'outil de suppression du silence au démarrage [ici](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* enfin, la latence entendue peut être causée par le serveur audio que vous utilisez sur votre ordinateur (il ne s'agit donc pas d'un défaut interne à une banque de sons et il est possible de changer la sortie audio dans les [configurations](manual/settings.md#doc_general)).

### Le son s'arrête subitement

Si le son s'arrête subitement alors qu'il devrait continuer tant que la note n'est pas relâchée, voir la section des [boucles](#doc_loop).

Si le son s'arrête subitement lorsqu'une note est relâchée, précisez ou augmenter la **durée de relâche** au niveau des [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) ou des [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md). Cet [outil](manual/soundfont-editor/tools/instrument-tools.md#doc_release) permet de configurer cette durée pour qu'elle semble naturelle à l'oreille des graves jusqu'aux aigus.


## Boucles manquantes ou imprécises {#doc_loop}


Les boucles permettent, à partir d'un échantillon ayant une durée déterminée, de produire un son d'une durée infinie sans que l'oreille entende les reprises. Les boucles sont nécessaires pour certains types d'instruments comme les flûtes mais pas pour d'autres comme les percussions. Des boucles peuvent aussi se trouver dans des sons de piano pour simuler une pédale sustain avec un échantillon de courte durée.

### Le son s'arrête avant que la note soit relâchée

Si le son s'arrête alors qu'il devrait continuer, les causes les plus probables sont les suivantes&nbsp;:

* la boucle au niveau de l'échantillon n'est pas définie, dans quel cas vous pouvez vous référer à cette [page](tutorials/how-to-prepare-a-sample.md#doc_loop) pour définir une boucle,
* l'utilisation de la boucle n'est pas activée et il faut donc changer le **mode de lecture** au niveau des [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table).

### Le son contient des «&nbsp;clics&nbsp;» à intervalles réguliers

La boucle est a priori mauvaise et cette [page](tutorials/how-to-prepare-a-sample.md#doc_loop) décrit comment créer des boucles.


## Standard MIDI non respecté {#doc_standard}


Les standards MIDI imposent une numérotation précise pour les instruments contenus dans les banques de sons. Il est ainsi possible d'interchanger des banques de sons différentes mais respectant le même standard sans qu'il en résulte une grande différence dans les timbres joués&nbsp;: un piano et une flûte seront par exemple appelés de la même manière. Si une revue indique qu'un standard n'est pas respecté, 2 solutions possibles&nbsp;

* soit enlever l'information indiquant que la banque de sons respecte un standard,
* soit vérifier que tous les ensembles de la banque de sons sont bien associés au bon type d'instrument (voir cette [page](manual/annexes/midi-standards.md) pour la liste des instruments) et que tous les instruments prévus par le standard sont bien présents.
