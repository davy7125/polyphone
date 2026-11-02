Les fichiers au format MIDI contiennent des messages MIDI qui, mis ensemble, forment une mélodie. Ils peuvent être combinés à un synthétiseur utilisant une banque de sons au format SoundFont et ce tutoriel montre comment utiliser un fichier MIDI pour tester le rendu d'une banque de sons.


## Un peu de contexte


<div class="too_complicated">
<p>En jouant sur le clavier d'un synthétiseur, des signaux <a href="https://fr.wikipedia.org/wiki/Musical_Instrument_Digital_Interface" target="_blank">MIDI</a> sont émis et ils contiennent par exemple l'information du numéro des notes jouées avec leur vélocité (terme indiquant la force de frappe sur la touche). Un fichier MIDI, pour simplifier, contient l'enregistrement chronologique des signaux émis par un tel clavier. Jusque-là, aucun son n'a encore été créé.</p>

<p>Pour qu'un son soit émis par le haut-parleur, une étape de conversion est nécessaire et c'est ici qu'intervient le synthétiseur&nbsp;: il déclenche des sons à partir des signaux MIDI reçus. Plusieurs types de synthétiseurs existent et nous parlerons ici des synthétiseurs utilisant la lecture d'échantillons, et plus précisément des synthétiseurs utilisant des banques de sons au format SoundFont.</p>

<p>Un fichier MIDI peut piloter jusqu'à 16 canaux différents, permettant l'usage en parallèle de 16 instruments différents. En plus d'indiquer quelles notes jouer, le fichier MIDI contient des instructions demandant au synthétiseur d'utiliser l'instrument X de la banque Y pour les notes à venir du canal Z. Pour permettre une sélection correcte des instruments, la banque de sons utilisée par le synthétiseur doit respecter la norme <a href="https://fr.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> qui assigne, pour chaque couple de numéros X et Y, un instrument précis. Par exemple, l'instrument <strong>001</strong> doit correspondre à un piano dans la banque de sons.</p>

<p>Pour récapituler, nous allons parler ici de la lecture d'un fichier MIDI pilotant un synthétiseur déclenchant des sons contenus dans une banque de sons au format SoundFont respectant la norme General MIDI. La suite est plus simple&nbsp;!</p>
</div>


## Prérequis


Pour lire un fichier MIDI avec la méthode proposée ci-dessous, vous aurez besoin&nbsp;:

- de la dernière version de Polyphone, disponible sur cette [page](software),
- d'un fichier MIDI, par exemple <a href="files/ff7choco.mid" download>celui-ci</a>,
- d'une banque de sons que vous voulez tester et qui respecte la norme <a href="https://fr.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> (un filtre existe dans le [dépôt](soundfonts/midi-standard/gm)).

Une banque de sons qui ne respecte pas cette norme peut aussi être testée, en [choisissant vous-même ses ensembles](#doc_presets).


## Lire un fichier MIDI


Ouvrez votre banque de sons dans Polyphone, puis cliquez sur l'icône ![MIDI](images/toolbar_midi.png "MIDI"){.inline} à droite de la [barre d'outils](manual/soundfont-editor/toolbar.md) pour afficher le [lecteur MIDI](manual/soundfont-editor/toolbar.md#doc_midi_player).

Cliquez sur le bouton dossier en haut à gauche de la fenêtre, sélectionnez votre fichier MIDI, puis cliquez sur le bouton de lecture (ou appuyez sur :kbd:`Espace`), et le tour est joué.

![Le lecteur MIDI de Polyphone](images/midi_player.png "Le lecteur MIDI de Polyphone")

Chaque canal du fichier contenant des notes a sa propre ligne dans le tableau. La colonne :guilabel:`Ensemble` indique l'ensemble demandé par le fichier, qui peut changer au cours du morceau, et la led de la colonne :guilabel:`Volume` s'allume à chaque début de note. Le curseur sous le nom du fichier permet d'aller à un autre passage du morceau.

Le lecteur MIDI est également disponible lorsque Polyphone est ouvert en [mode synthé](tutorials/use-polyphone-as-a-synthesizer.md).


## Tester la banque de sons


La banque de sons est jouée telle qu'elle est dans l'éditeur, modifications non enregistrées comprises&nbsp;: vous pouvez modifier un instrument pendant la lecture du morceau et entendre immédiatement le résultat, par exemple en ajustant son volume, son enveloppe ou son filtre.

Le lecteur aide aussi à écouter les détails&nbsp;:

- les boutons :guilabel:`S` (solo) et :guilabel:`M` (rendre muet) d'un canal isolent un instrument ou le retirent du mélange, et le curseur de la colonne :guilabel:`Volume` l'équilibre avec les autres&nbsp;;
- une icône d'avertissement dans la colonne :guilabel:`Ensemble` signifie que la banque de sons ne contient pas l'ensemble demandé par le fichier&nbsp;: les notes de ce canal ne sont pas jouées, ce qui révèle une banque de sons ne respectant pas complètement la norme General MIDI&nbsp;;
- le bouton MIDI d'un canal liste les contrôleurs actuellement utilisés par le fichier (molette de modulation, expression, réverbération…), ce qui est utile pour vérifier les [modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) de la banque de sons&nbsp;;
- les [paramètres de lecture](manual/soundfont-editor/toolbar.md#doc_midi_player_settings), à droite du bandeau, permettent de jouer le morceau en boucle, de le ralentir pour écouter un passage rapide ou de le transposer pour tester d'autres notes.


## Choisir les banques de sons et les ensembles {#doc_presets}


Chaque canal peut jouer une banque de sons différente&nbsp;: ouvrez plusieurs banques de sons dans Polyphone, puis double-cliquez sur une cellule de la colonne :guilabel:`Banque de sons` pour choisir celle que joue le canal. Vous pouvez par exemple comparer les pianos de deux banques de sons sur la même partie, ou garder une banque de sons General MIDI pour l'accompagnement pendant que votre propre banque de sons joue la mélodie.

![Choisir la banque de sons d'un canal](images/midi_player_soundfonts.png "Choisir la banque de sons d'un canal")

De la même manière, un double-clic sur une cellule de la colonne :guilabel:`Ensemble` impose un ensemble de la banque de sons à la place de celui demandé par le fichier. C'est ainsi que l'on teste une banque de sons qui ne respecte pas la norme General MIDI, par exemple une banque de sons ne contenant qu'un seul instrument&nbsp;: choisissez son ensemble pour le canal de la mélodie, les changements de programme du fichier étant alors ignorés sur ce canal.


## Exporter le résultat


Lorsque le résultat vous convient, :guilabel:`Exporter l'audio...` dans les paramètres de lecture écrit le morceau dans un fichier wav tel qu'il est entendu dans le lecteur, avec les mêmes banques de sons, ensembles, volumes, tempo et hauteur.
Un fichier MIDI peut aussi être converti en fichier wav en [ligne de commande](manual/annexes/command-line.md#doc_render), par exemple dans un script.
