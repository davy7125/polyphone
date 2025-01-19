Les fichiers au format MIDI contiennent des messages MIDI qui, mis ensemble, forment une mélodie. Ils peuvent être combinés à un synthétiseur utilisant une banque de sons au format SoundFont et ce tutoriel montre comment utiliser un fichier MIDI pour tester le rendu d'une banque de sons.

## Un peu de contexte

<div class="too_complicated">
<p>En jouant sur le clavier d'un synthétiseur, des signaux <a href="https://fr.wikipedia.org/wiki/Musical_Instrument_Digital_Interface" target="_blank">MIDI</a> sont émis et ils contiennent par exemple l'information du numéro des notes jouées avec leur vélocité (terme indiquant la force de frappe sur la touche). Un fichier MIDI, pour simplifier, contient l'enregistrement chronologique des signaux émis par un tel clavier. Jusque là, aucun son n'est encore créé.</p>

<p>Pour qu'un son soit émis par le haut-parleur, une étape de conversion est nécessaire et c'est ici qu'intervient le synthétiseur&nbsp;: il déclenche des sons à partir des signaux MIDI reçus. Plusieurs types de synthétiseurs existent et nous parleront ici des synthétiseurs utilisant la lecture d'échantillons, et plus précisément des synthétiseurs utilisant des banques de sons au format SoundFont.</p>

<p>Un fichier MIDI peut piloter jusqu'à 16 canaux différents, permettant l'usage en parallèle de 16 instruments différents. En plus d'indiquer quelles notes jouer, le fichier MIDI contient des instructions demandant au synthétiseur d'utiliser l'instrument X de la banque Y pour les notes à venir du canal Z. Pour permettre une sélection correcte des instruments, la banque de sons utilisée par le synthétiseur doit respecter la norme <a href="https://fr.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> qui assigne, pour chaque numéros X et Y, un instrument précis. Par exemple, l'instrument <strong>001</strong> doit correspondre à un piano dans la banque de sons.</p>

<p>Pour récapituler, nous allons parler ici de la lecture d'un fichier MIDI pilotant un synthétiseur déclenchant des sons contenus dans une banque de son au format SoundFont respectant la norme General MIDI. La suite est plus simple&nbsp;!</p>
</div>

## Prérequis

Pour lire un fichier MIDI avec la méthode proposée ci-dessous, vous aurez besoin&nbsp;:
- d'un accès à internet,
- d'un navigateur web,
- d'un fichier MIDI, par exemple <a href="files/ff7choco.mid" download>celui-ci</a>,
- de manière optionnelle, une banque de sons que vous voulez tester et qui respecte la norme <a href="https://fr.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (un filtre existe dans le [dépôt](soundfonts/midi-standard/gm)),
- de manière optionnelle, la dernière version de Polyphone disponible à cette [page](software).

## Lire un fichier MIDI

Un méthode simple consiste à utiliser le synthétiseur <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a>, qui sert à la fois de lecteur de fichiers MIDI et de synthétiseur utilisant des banques de sons. Il a l'avantage d'être disponible en ligne et ne nécessite aucune installation particulière.

Cliquez sur le bouton :guilabel:`Charger un fichier MIDI`, sélectionnez votre fichier MIDI, et le tour est joué.

![Le lecteur MIDI et synthétiseur SpessaSynth](images/SpessaSynth.jpg "Le lecteur MIDI et synthétiseur SpessaSynth")

## Choisir la soundfont

Pour utiliser la banque de sons de votre choix, cliquez sur le bouton :guilabel:`Charger une banque de sons` et sélectionnez votre fichier.

## Connecter la sortie MIDI à Polyphone

Comme indiqué plus haut, **SpessaSynth** est à la fois un lecteur de fichier MIDI et un synthétiseur. Il est possible de n'utiliser que la fonction de lecture de fichier MIDI et d'envoyer les signaux à Polyphone qui prendra le relai pour la synthèse sonore. Cette manipulation peut être utile si la banque de son est trop lourde pour être envoyée par internet.

Pour connecter SpessaSynth à Polyphone (méthode testée avec Linux)&nbsp;:
- lancez le logiciel Polyphone en [mode synthé](tutorials/use-polyphone-as-a-synthesizer.md) et ouvrez les configurations,
- sélectionnez «&nbsp;MIDI through&nbsp;» comme [entrée MIDI](manual/settings.md#doc_general) (retenir le nom),
- ouvrez SpessaSynth dans le navigateur internet et cliquez sur le bouton :guilabel:`Configurations`,
- sélectionnez la sortie MIDI «&nbsp;MIDI through&nbsp;» (nom identique à l'entrée MIDI de Polyphone).

Un fichier MIDI pouvant envoyer des messages à destination de 16 canaux, il faudra ouvrir la banque de sons 16 fois et préciser pour chaque onglet un canal différent de 1 à 16. Par commodité il est possible lors de l'ouverture de Polyphone, de préciser la banque de sons à ouvrir et l'option «&nbsp;each&nbsp;» pour ouvrir les 16 onglets. La commande sera par exemple&nbsp;:

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each/off/off"
```

Voir cette [page](manual/annexes/command-line.md) pour des compléments d'information sur les lignes de commande.
