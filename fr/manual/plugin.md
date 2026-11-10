Polyphone existe aussi sous la forme d'un plugin pour les stations audionumériques (DAW) et les autres hôtes de plugins audio.
Le plugin joue les banques de sons aux formats sf2, sf3, sfz et dls avec le synthétiseur de Polyphone&nbsp;: une banque de sons sonne de la même manière dans un hôte et dans l'éditeur.
Il joue les banques de sons mais ne les modifie pas&nbsp;: les modifications sont faites dans Polyphone, puis la banque de sons est rouverte dans le plugin.

Le plugin est disponible aux formats VST3, CLAP et LV2 sous Linux, Windows et macOS, ainsi qu'au format Audio Unit (AU) sous macOS.


![Plugin Polyphone](images/plugin.png "Plugin Polyphone")


## Installation {#doc_install}


Le plugin est distribué dans une archive pour chaque système d'exploitation, disponible dans la section «&nbsp;[Logiciel](software)&nbsp;».
Copiez les formats utilisés par votre hôte dans les dossiers où il cherche les plugins, puis relancez la recherche des plugins dans l'hôte.

Linux
: :file:`Polyphone.vst3` dans :file:`~/.vst3/`,\
  :file:`Polyphone.clap` dans :file:`~/.clap/`,\
  :file:`Polyphone.lv2` dans :file:`~/.lv2/`.

macOS
: :file:`Polyphone.vst3` dans :file:`~/Library/Audio/Plug-Ins/VST3/`,\
  :file:`Polyphone.clap` dans :file:`~/Library/Audio/Plug-Ins/CLAP/`,\
  :file:`Polyphone.component` dans :file:`~/Library/Audio/Plug-Ins/Components/` (format Audio Unit, utilisé par Logic Pro et GarageBand),\
  :file:`Polyphone.lv2` dans :file:`~/Library/Audio/Plug-Ins/LV2/`.

Windows
: :file:`Polyphone.vst3` dans :file:`C:\Program Files\Common Files\VST3`,\
  :file:`Polyphone.clap` dans :file:`C:\Program Files\Common Files\CLAP`,\
  :file:`Polyphone.lv2` dans :file:`%APPDATA%\LV2`.

Sous macOS, les plugins ne sont pas notariés&nbsp;: si l'hôte refuse de les charger, retirez l'attribut de quarantaine des fichiers téléchargés, par exemple&nbsp;:

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## Banque de sons {#doc_soundfont}


Le haut de la fenêtre affiche le nom de la banque de sons et son chemin.
Une banque de sons s'ouvre&nbsp;:

* avec le bouton :guilabel:`Ouvrir...`,
* avec la flèche à sa droite, qui liste les 10 dernières banques de sons ouvertes,
* en déposant un fichier sur la fenêtre.

Les fichiers sfz et dls sont convertis comme Polyphone les importe, les échantillons d'un fichier sfz étant les fichiers audio auxquels il fait référence.
Toute la banque de sons est lue à son ouverture, les échantillons d'un fichier sf3 étant décompressés&nbsp;: rien n'est lu sur le disque pendant que le plugin joue.
Lorsque plusieurs instances du plugin jouent le même fichier (une par piste par exemple), le fichier n'est lu et gardé en mémoire qu'une seule fois.
Si le fichier a changé, par exemple après avoir été de nouveau enregistré dans Polyphone, il est relu.


## Modes de jeu {#doc_modes}


Trois boutons au-dessus de la liste des ensembles choisissent la manière dont les ensembles sont joués&nbsp;:

:guilabel:`Un ensemble`
: L'ensemble sélectionné dans la liste est joué sur les 16 canaux MIDI.
  Les ensembles sont affichés avec leurs numéros de banque et d'ensemble, par exemple «&nbsp;000:024 Guitar&nbsp;».

:guilabel:`16 canaux`
: Chaque canal MIDI joue son propre ensemble, choisi sur sa ligne.
  Le choix :guilabel:`Program change` joue l'ensemble désigné par les messages de sélection de banque et de changement de programme reçus par le canal, comme le fait un synthétiseur General MIDI&nbsp;: la banque 128 est alors utilisée sur le canal 10, pour les percussions.
  L'ensemble joué est affiché à droite de chaque ligne.
  Ce mode est pratique pour jouer un fichier MIDI arrangé pour plusieurs instruments.

:guilabel:`MPE`
: L'ensemble sélectionné est joué par un contrôleur MPE (MIDI Polyphonic Expression)&nbsp;: chaque note a son propre canal, et donc son propre Pitch Bend, sa propre pression et son propre timbre (CC 74).
  Une zone inférieure de 15 canaux est utilisée jusqu'à ce que le contrôleur envoie sa configuration (RPN 6).
  Le canal maître d'une zone agit sur toutes ses notes&nbsp;: son Pitch Bend s'ajoute à celui de chaque note et ses contrôleurs s'appliquent à toutes.
  La sensibilité du Pitch Bend est de 48 demi-tons pour les notes et de 2 demi-tons pour le canal maître, et peut être modifiée par le contrôleur (RPN 0).


![Plugin en mode «&nbsp;16 canaux&nbsp;»](images/plugin_channels.png "Plugin en mode «&nbsp;16 canaux&nbsp;»")


## Paramètres {#doc_parameters}


Les paramètres à droite sont ceux du synthétiseur de Polyphone, que l'on retrouve dans ses préférences du [son](manual/settings.md#doc_sound) et du [clavier](manual/settings.md#doc_keyboard)&nbsp;:

:guilabel:`Synthé`
: :guilabel:`Gain` (de -50 à +50&nbsp;dB), :guilabel:`Transposition` (de -24 à +24 demi-tons, les percussions n'étant pas transposées), :guilabel:`Accordage` (fréquence du La situé au-dessus du Do central, de 400 à 480&nbsp;Hz) et :guilabel:`Tempérament`, dont les accords fins sont relatifs à la tonalité choisie sous la liste.

:guilabel:`Réverbération`
: :guilabel:`Niveau`, :guilabel:`Taille`, :guilabel:`Densité` et :guilabel:`Atténuation`.

:guilabel:`Chorus`
: :guilabel:`Niveau`, :guilabel:`Amplitude` et :guilabel:`Fréquence`.

Tous ces paramètres peuvent être automatisés par l'hôte.


## Clavier et barre d'état {#doc_keyboard}


Le clavier en bas de la fenêtre montre les notes reçues et peut être joué avec la souris.
En dessous sont affichés le nombre de voix en cours et la part du processeur utilisée par le plugin.

Le bouton :guilabel:`Panique` arrête tous les sons et réinitialise les contrôleurs, ce qui est utile si une note reste bloquée.


## Préférences {#doc_settings}


Le bouton :guilabel:`Préférences` ouvre un menu avec les options suivantes&nbsp;:

:guilabel:`Polyphonie`
: Nombre maximal de voix, de 64 à 4096 (1024 par défaut).
  Au-delà de ce nombre, les notes les plus faibles s'estompent.

:guilabel:`Clavier`
: Canal MIDI des notes jouées avec le clavier de la fenêtre.

:guilabel:`Langue`
: Langue du plugin, par défaut celle du système.
  Ce choix est commun à toutes les instances du plugin.
  Les noms des paramètres et des sorties transmis à l'hôte sont ceux de la langue utilisée au chargement du plugin (toujours l'anglais avec LV2).

:guilabel:`Enregistrer la banque de sons dans le projet`
: La banque de sons est enregistrée dans le projet de l'hôte, qui peut alors être ouvert sur un autre ordinateur sans le fichier.
  Ce n'est pas possible avec un fichier sfz, dont les échantillons sont d'autres fichiers.

:guilabel:`Canaux et effets sur leurs propres sorties`
: En plus de la sortie principale, les sorties «&nbsp;Canal 1&nbsp;» à «&nbsp;Canal 16&nbsp;» reçoivent les notes de leur canal sans effet, et la sortie «&nbsp;Effets&nbsp;» reçoit la part envoyée à la réverbération et au chorus.
  Un canal dont la sortie n'est pas activée par l'hôte reste dans la sortie principale.
  Lorsque cette option est désactivée (par défaut), tout est mixé dans la sortie principale.


## Utilisation dans un projet {#doc_project}


Le projet de l'hôte enregistre les paramètres, les ensembles joués, la polyphonie et le chemin de la banque de sons (ou son contenu, si elle est enregistrée dans le projet).
À l'ouverture d'un projet, la banque de sons est lue avant que le reste ne soit restauré, afin qu'un export hors ligne puisse commencer immédiatement.

Une note commence exactement à la position de son message MIDI, quelle que soit la taille du tampon de l'hôte.
Pendant un export hors ligne, les voix ne sont jamais limitées par la charge du processeur.
