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
À l'ouverture d'une banque de sons, sa structure est lue immédiatement&nbsp;: les instruments sont listés et peuvent être joués.
Ses échantillons sont ensuite lus en arrière-plan, avec une icône de chargement devant son nom (son infobulle indique la progression)&nbsp;: une note dont les échantillons ne sont pas encore lus reste muette.
Les échantillons des notes jouées et des instruments choisis sont lus en premier, ceux d'un fichier sf3 étant décompressés.
Une fois tout lu, rien n'est lu sur le disque pendant que le plugin joue&nbsp;; un export hors ligne attend que tous les échantillons soient lus.
Lorsque plusieurs instances du plugin jouent le même fichier (une par piste par exemple), le fichier n'est lu et gardé en mémoire qu'une seule fois.
Si le fichier a changé, par exemple après avoir été de nouveau enregistré dans Polyphone, il est relu.


## Modes de jeu {#doc_modes}


Trois boutons au-dessus de la liste des instruments (les ensembles de la banque de sons) choisissent la manière dont les instruments sont joués&nbsp;:

:guilabel:`Un instrument`
: L'instrument sélectionné dans la liste est joué sur les 16 canaux MIDI.
  Les instruments sont affichés avec leurs numéros de banque et de programme, par exemple «&nbsp;000:024 Guitar&nbsp;».

:guilabel:`16 canaux`
: Chaque canal MIDI joue son propre instrument, choisi sur sa ligne.
  Le choix :guilabel:`Program change` joue l'instrument désigné par les messages de sélection de banque et de changement de programme reçus par le canal, comme le fait un synthétiseur General MIDI&nbsp;: la banque 128 est alors utilisée sur le canal 10, pour les percussions.
  L'instrument joué est affiché à droite de chaque ligne.
  Ce mode est pratique pour jouer un fichier MIDI arrangé pour plusieurs instruments.

:guilabel:`MPE`
: L'instrument sélectionné est joué par un contrôleur MPE (MIDI Polyphonic Expression)&nbsp;: chaque note a son propre canal, et donc son propre Pitch Bend, sa propre pression et son propre timbre (CC 74).
  Une zone inférieure de 15 canaux est utilisée jusqu'à ce que le contrôleur envoie sa configuration (RPN 6).
  Le canal maître d'une zone agit sur toutes ses notes&nbsp;: son Pitch Bend s'ajoute à celui de chaque note et ses contrôleurs s'appliquent à toutes.
  La sensibilité du Pitch Bend est de 48 demi-tons pour les notes et de 2 demi-tons pour le canal maître, et peut être modifiée par le contrôleur (RPN 0).


![Plugin en mode «&nbsp;16 canaux&nbsp;»](images/plugin_channels.png "Plugin en mode «&nbsp;16 canaux&nbsp;»")


## Paramètres {#doc_parameters}


Les paramètres sont ceux du synthétiseur de Polyphone, que l'on retrouve dans ses préférences du [son](manual/settings.md#doc_sound) et du [clavier](manual/settings.md#doc_keyboard).
:guilabel:`Gain` (de -24 à +24&nbsp;dB) se trouve dans l'en-tête, les autres paramètres sont regroupés à droite&nbsp;:

:guilabel:`Note`
: Le mode de jeu, dans la liste de la première ligne (voir les [modes de jeu](manual/plugin.md#doc_play_modes))&nbsp;: :guilabel:`Poly` (par défaut), :guilabel:`Mono` ou :guilabel:`Legato`, le glissement du legato se réglant à sa droite.
  :guilabel:`Diapason`&nbsp;: une note de La#3 à La4 et sa fréquence (de 400 à 480&nbsp;Hz pour le La4, 440&nbsp;Hz par défaut), cette note sonnant à cette fréquence quel que soit le tempérament.
  :guilabel:`Transposition`&nbsp;: de -24 à +24 demi-tons, les percussions n'étant pas transposées.

:guilabel:`Vélocité`
: La réponse à la vélocité des notes, chaque réglage étant illustré par un graphique (vélocité de 0 à 127 de gauche à droite)&nbsp;:
  
  * :guilabel:`Fréquence de coupure`&nbsp;: modulateur par défaut abaissant la fréquence de coupure du filtre selon la vélocité, jusqu'à 2 octaves, comme dans les préférences du [son](manual/settings.md#doc_sound) de Polyphone. La version 2.04 du format SoundFont (par défaut) l'abaisse d'autant plus que la vélocité est faible. La version 2.01 comportait un saut à la vélocité 64, une erreur corrigée par la version 2.04&nbsp;: c'est un mode de compatibilité pour les anciennes banques de sons faites en compensant ce saut. Avec :guilabel:`Désactivé`, la fréquence de coupure ne dépend pas de la vélocité. Il n'a pas d'effet sur une banque de sons ayant ses propres [modulateurs par défaut](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_default_mod).
  * :guilabel:`Courbe`&nbsp;: courbe appliquée à la vélocité des notes reçues, linéaire au milieu (par défaut, rétablie par un double clic), concave vers la gauche (vélocité plus molle&nbsp;: les vélocités élevées demandent un toucher plus appuyé) et convexe vers la droite (la vélocité monte rapidement).

:guilabel:`Tempéraments et gammes`
: Les tempéraments et gammes de Polyphone dans leurs catégories, puis le tempérament personnalisé, transposés dans la :guilabel:`Tonalité` choisie.
  Le clavier à droite montre l'accord fin de chaque note en cents et se modifie comme dans les [préférences du clavier](manual/settings.md#doc_keyboard) de Polyphone&nbsp;: molette de la souris ou glissement vertical, double clic pour revenir à 0, menu contextuel pour changer l'utilisation d'une note ou y placer la tonalité. Un tempérament modifié devient le tempérament personnalisé.
  Les notes inutilisées par une gamme ne sont pas jouées, sauf si :guilabel:`Tout jouer` est coché.

:guilabel:`Enveloppe`
: L'attaque, le déclin, le soutien et la relâche des instruments, modifiés (:guilabel:`Relatif`) ou remplacés (:guilabel:`Fixe`), voir l'[enveloppe](manual/plugin.md#doc_envelope).

:guilabel:`Réverbération`
: :guilabel:`Niveau`, :guilabel:`Taille`, :guilabel:`Densité` et :guilabel:`Atténuation`.

:guilabel:`Chorus`
: :guilabel:`Niveau`, :guilabel:`Amplitude` et :guilabel:`Fréquence`.

Les groupes :guilabel:`Tempéraments et gammes`, :guilabel:`Enveloppe`, :guilabel:`Réverbération` et :guilabel:`Chorus` ont un interrupteur à droite de leur titre&nbsp;: éteint, le groupe est replié et son tempérament, son enveloppe ou son effet n'est pas appliqué (tempérament égal avec toutes les notes, enveloppes de la banque de sons, aucun effet), ses réglages étant conservés.
La hauteur de la fenêtre suit les groupes dépliés.

L'hôte peut automatiser tous ces paramètres, sauf la note du diapason (la fréquence du La4 étant automatisée) et les accords fins du tempérament personnalisé, qui sont enregistrés avec le projet.


## Modes de jeu {#doc_play_modes}


La liste de la première ligne du groupe :guilabel:`Note` choisit la façon de jouer les notes de chaque canal MIDI&nbsp;:

:guilabel:`Poly`
: Les notes sont jouées ensemble (par défaut).

:guilabel:`Mono`
: Une note à la fois&nbsp;: une note commence par son attaque, la précédente s'arrêtant rapidement.

:guilabel:`Legato`
: Une note à la fois&nbsp;: une note jouée avant le relâchement de la précédente la prolonge, sans nouvelle attaque.
  Si une division jouant la note précédente contient aussi la nouvelle touche, sa voix continue avec le même échantillon, seule sa hauteur changeant.
  Les autres échantillons de la nouvelle touche démarrent au début de leur boucle (à leur début s'ils n'ont pas de boucle), avec un fondu enchaîné.
  La barre à droite de la liste règle le glissement&nbsp;: :guilabel:`Aucun` (par défaut, la hauteur changeant immédiatement) ou la durée du glissement de hauteur depuis la note précédente, jusqu'à 5&nbsp;s.
  Une note jouée après le relâchement de la précédente commence par son attaque, comme dans le mode :guilabel:`Mono`.

Entre parenthèses figure la touche entendue lorsque plusieurs touches sont tenues&nbsp;: la dernière, la plus haute ou la plus basse.
Lorsque la touche entendue est relâchée alors que d'autres sont encore tenues, la note de celle qui a la priorité est de nouveau entendue&nbsp;: tenir Ré et répéter Mi joue un trille, glissé en legato.
Les percussions (banque 128) et le mode :guilabel:`MPE` restent polyphoniques.


## Enveloppe {#doc_envelope}


Le groupe :guilabel:`Enveloppe` modifie l'enveloppe de volume de tous les instruments joués, sauf les percussions (banque 128), sans modifier la banque de sons.
Son mode est choisi à gauche de son interrupteur, chaque mode conservant ses propres valeurs&nbsp;:

:guilabel:`Relatif`
: L'enveloppe de chaque son de la banque de sons est modifiée (par défaut)&nbsp;: :guilabel:`Attaque`, :guilabel:`Déclin` et :guilabel:`Relâche` multiplient ses durées, de ×0,10 à ×10, et :guilabel:`Soutien` relève ou abaisse son niveau de soutien, de -20 à +20 dB (jusqu'au niveau maximal).
  Chaque son garde la forme de son enveloppe&nbsp;: une relâche qui varie selon les touches ou les couches de vélocité varie toujours.
  Les durées de l'enveloppe de modulation (filtre et hauteur) changent dans les mêmes proportions, de sorte que le timbre d'un son dont le filtre s'ouvre avec l'attaque suit la nouvelle attaque.
  Un facteur de ×1,00 et une variation de 0 dB conservent la banque de sons.

:guilabel:`Fixe`
: Les valeurs remplacent celles de la banque de sons&nbsp;: :guilabel:`Attaque` de 1 ms à 10 s, :guilabel:`Déclin` et :guilabel:`Relâche` de 1 ms à 20 s, :guilabel:`Soutien` de -96 à 0 dB.
  Un bouton tourné complètement à gauche (:guilabel:`Origine`) conserve la valeur de la banque de sons, pour ne remplacer que la relâche par exemple.
  L'enveloppe de modulation n'est pas modifiée.

Le graphique à droite trace l'enveloppe, ses durées étant compressées pour que les étapes courtes et longues soient visibles.
En mode :guilabel:`Relatif`, un exemple d'enveloppe d'une banque de sons est tracé en pointillés sous l'enveloppe jouée&nbsp;; en mode :guilabel:`Fixe`, les étapes conservant la banque de sons sont tracées en pointillés.

Un échantillon sans boucle (un piano par exemple) s'arrête à sa fin quelle que soit son enveloppe&nbsp;: un déclin plus long ou un soutien plus haut ne peut pas le faire durer plus longtemps.
Les modulateurs de la banque de sons s'appliquent toujours (une vélocité raccourcissant l'attaque, par exemple).
Les changements s'appliquent aussitôt aux notes en cours, l'enveloppe pouvant ainsi être automatisée&nbsp;: une note tenue prend le nouveau niveau de soutien et est relâchée avec la relâche réglée à ce moment.
Un double clic sur un bouton rétablit sa valeur par défaut (la banque de sons).


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

:guilabel:`Latence fixe (notes jouées à leur position exacte)`
: Lorsque cette option est désactivée (par défaut), les messages MIDI d'un bloc audio sont avancés ensemble pour que la première note du bloc commence à son début&nbsp;: les notes jouées en direct sonnent plus tôt, d'une demi-durée de tampon en moyenne, mais leur latence varie selon leur position dans le tampon de l'hôte.
  Lorsqu'elle est activée, chaque message est traité à sa position exacte&nbsp;: la latence des notes jouées en direct est constante.
  Pendant un export hors ligne, les messages sont toujours traités à leur position exacte.

:guilabel:`Écrêtage à [-1 ; 1]`
: Les sorties sont limitées à l'intervalle [-1&nbsp;; 1] (par défaut), comme dans Polyphone.
  Sans écrêtage, les valeurs plus grandes sont transmises à l'hôte, qui les mixe en virgule flottante&nbsp;: le gain peut alors être augmenté sans saturation, le niveau étant ajusté avant la sortie de l'hôte.

:guilabel:`Enregistrer la banque de sons dans le projet`
: La banque de sons est enregistrée dans le projet de l'hôte, qui peut alors être ouvert sur un autre ordinateur sans le fichier.
  Ce n'est pas possible avec un fichier sfz, dont les échantillons sont d'autres fichiers.

:guilabel:`Canaux et effets sur leurs propres sorties`
: En plus de la sortie principale, les sorties «&nbsp;Canal 1&nbsp;» à «&nbsp;Canal 16&nbsp;» reçoivent les notes de leur canal sans effet, et la sortie «&nbsp;Effets&nbsp;» reçoit la part envoyée à la réverbération et au chorus.
  Un canal dont la sortie n'est pas activée par l'hôte reste dans la sortie principale.
  Lorsque cette option est désactivée (par défaut), tout est mixé dans la sortie principale.


## Utilisation dans un projet {#doc_project}


Le projet de l'hôte enregistre les paramètres, les instruments joués, la polyphonie et le chemin de la banque de sons (ou son contenu, si elle est enregistrée dans le projet).
À l'ouverture d'un projet, seule la structure de la banque de sons est lue avant que le reste ne soit restauré&nbsp;: l'hôte n'attend pas les échantillons, et un export hors ligne peut commencer immédiatement (il attend les échantillons pas encore lus).

Une note commence exactement à la position de son message MIDI, quelle que soit la taille du tampon de l'hôte, pendant un export hors ligne et en temps réel avec une [latence fixe](manual/plugin.md#doc_settings).
Pendant un export hors ligne, les voix ne sont jamais limitées par la charge du processeur.
