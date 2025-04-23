Cette page liste les défauts typiques des banques de sons (pouvant être relevés par des revues) et, pour chacun d'eux, des techniques et des liens vers la documentation pour les corriger&nbsp;:

* [échantillons de mauvaise qualité ou mal préparés](#sample),
* [mauvais accordage](#tuning),
* [enveloppe ADSR inadéquate](#adsr),
* [boucles manquantes ou imprécises](#loop),
* [standard MIDI non respecté](#standard).


## Échantillons de mauvaise qualité ou mal préparés {#sample}


Il est possible qu'une banque de sons ne fournisse pas d'instrument convainquant et si la cause provient d'échantillons de mauvaise qualité, la seule solution est de les remplacer&nbsp;:

* recherche d'autres échantillons sur internet,
* enregistrement avec un micro de meilleur qualité (ou utilisation d'outils externes de traitement du signal),
* création d'échantillons par un autre moyen.

D'autre fois, le problème peut se résoudre en préparant ou utilisant mieux les échantillons.

### Son mauvais, surtout vers les aigus

Typiquement, un seul échantillon audio ne permet pas de constituer un instrument de bonne qualité. Un échantillon possède en effet sa propre hauteur de note (sauf pour les percussions non mélodiques) et l'utiliser pour une étendue de notes très large va modifier désagréablement le son et surtout dans les aigus. Pour corriger ce défaut il est donc nécessaire d'utiliser plusieurs échantillons ou, si un seul échantillon est disponible, il est possible de&nbsp;:

* dupliquer l'échantillon,
* transposer l'échantillon créé d'une octave par exemple avec cet [outil](manual/soundfont-editor/tools/sample-tools.md#doc_transpose),
* filtrer les harmoniques hautes avec cet [outil](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq) ou [celui-ci](manual/soundfont-editor/tools/sample-tools.md#doc_filter).

Les échantillons seront ensuite placés sur des étendues de notes au sein desquelles leur écoute ne déforme pas le timbre initial.

### Le son contient des «&nbsp;clics&nbsp;» à intervalles réguliers

Le problème vient probablement d'une boucle mal positionnée et cet autre [paragraphe](#loop) décrit le sujet.

### Échantillons mal découpés

Les échantillons ne sont souvent utilisés que partiellement et enlever le superflu permet de réduire la taille d'un fichier&nbsp;:

* la zone utile dans les échantillons est parfois située avant le point de bouclage final&nbsp;: il est donc possible de supprimer la partie de l'échantillon située après la fin de boucle et cet [outil](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop) permet de le faire en quelques clics&nbsp;;
* les échantillons peuvent débuter par un blanc qu'il est important de supprimer pour diminuer la latence lors du déclenchement d'une note (il existe aussi un [outil](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank) pour le faire rapidement)&nbsp;;
* enfin, des échantillons peuvent tout simplement ne pas être utilisés&nbsp;: cette [page](manual/soundfont-editor/configuration-summaries.md#doc_sample) les liste et cet [outil](manual/soundfont-editor/tools/global-tools.md#doc_unused) les enlève automatiquement.


## Mauvais accordage {#tuning}


(en cours de rédaction)


## Enveloppe ADSR inadéquate {#adsr}


L'enveloppe ADSR représente l'évolution du volume lorsqu'une note est lue mais aussi lorsqu'une cette même note est arrêtée. Chacune des 4 lettres du mot ADSR correspond à une phase particulière et l'enveloppe de volume utilisée dans les banques de sons au format Soundfont contient en réalité 6 phases&nbsp;:

* délais,
* attaque (A),
* maintien,
* déclin (D),
* sountien (S),
* relâche (R).

Ces phases sont décrites plus en détails sur cette [page](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope) et ci-dessous sont listés des problèmes possibles liés à l'enveloppe.

### Un son démarre trop soudainement

Pour adoucir le démarrage d'une note, précisez ou augmentez la durée d'attaque de la note.

### Un son est trop mou à démarrer

À l'inverse du paragraphe précédent, diminuez ou supprimez la durée d'attaque de la note.

### Un délai est ressenti avant le démarrage du son

Trois solutions possibles et pouvant être complémentaires&nbsp;:

* supprimez la durée du délai si elle est précisée au niveau des instruments ou des ensembles,
* supprimer le silence au début de l'échantillon audio (voir la découpe de l'échantillon sur cette [page](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph) ou l'outil de suppression du silence au démarrage [ici](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)),
* enfin, la latence entendue peut être causée par le serveur audio que vous utilisez sur votre ordinateur (il ne s'agit donc pas d'un défaut interne à une banque de sons et il est possible de changer la sortie audio dans les [configurations](manual/settings.md#doc_general)).

### Le son s'arrête subitement

Si le son s'arrête subitement alors qu'il devrait continuer tant que la note n'est pas relâchée, voir la section des [boucles](#loop).

Si le son s'arrête subitement lorsqu'une note est relâchée, précisez ou augmenter la durée de relâche. Cet [outil](manual/soundfont-editor/tools/instrument-tools.md#doc_release) permet de configurer cette durée pour qu'elle semble naturelle à l'oreille des graves jusqu'aux aigus.


## Boucles manquantes ou imprécises {#loop}


(en cours de rédaction)


## Standard MIDI non respecté {#standard}


(en cours de rédaction)
