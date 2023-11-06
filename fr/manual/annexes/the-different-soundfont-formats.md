Polyphone est compatible avec plusieurs formats de banques de sons&nbsp;:

* [sf2](#doc_sf2),
* [sf3](#doc_sf3),
* [sfz](#doc_sfz),
* [sfArk](#doc_sfark).


## Banques de sons sf2 {#doc_sf2}


Les banques de sons au format sf2 constituent les principaux fichiers édités par Polyphone.
Chaque fichier sf2 contient au moins un instrument de musique virtuel, constitué d'échantillons audio et de nombreux paramètres.
Des paramètres définissent comment les échantillons doivent être lus partout sur le clavier, potentiellement modulés par des signaux prédéfinis ([modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)).
Les fichiers sf2 sont construits selon une architecture à trois niveaux&nbsp;:

* [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md), provenant d'une trompette ou d'un piano par exemple,
* [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md), constitués d'échantillons,
* [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md), constitués d'instruments.

En plus de la définition et de la configuration de ces éléments, un fichier sf2 contient des [informations générales](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md) (l'auteur, la licence et la date de création par exemple).

Les banques de sons sont utilisées par des synthétiseurs logiciels, comme <a href="http://www.fluidsynth.org/" target="_blank">Fluidsynth</a>, qui sont généralement pilotés par des signaux MIDI.
Il est aussi possible d'utiliser les banques de sons au format sf2 pour écouter des fichiers MIDI avec <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>, <a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> ou QuickTime et écouter des partitions de musique avec <a href="https://musescore.org/" target="_blank">MuseScore</a>.
Du matériel tel que sampleurs, cartes sons ou encore synthétiseurs peuvent aussi supporter le format .sf2.

D'autres informations sur les banques de sons peuvent être trouvées par exemple sur <a href="https://fr.wikipedia.org/wiki/SoundFont" target="_blank">Wikipedia</a>.
Les spécifications sf2 sont disponibles [ici](files/sf_specifications_v2.01.pdf) pour la version 2.01 et [là](files/sf_specifications_v2.04.pdf) pour la version 2.04.


## Banques de sons sf3 {#doc_sf3}


Le format sf3, développé par <a href="https://musescore.org/" target="_blank">MuseScore</a>, est similaire en tout point au format sf2 sauf pour les [échantillons](manual/soundfont-editor/editing-pages/sample-editor.md) qui sont stockés dans le format OGG (version open-source du format MP3) au lieu d'être stockés en données brutes.
La conséquence est que le format sf3 est environ 10 fois plus léger que le format sf2 pour une qualité comparable.

Une banque de son exportée dans ce format n'a pas pour but d'être ré-éditée ultérieurement à cause des compressions successives qui pourraient induire un abaissement de la qualité du son.
Ce format doit être considéré comme un produit final et est très intéressant pour les utilisateurs finaux des banques de sons car&nbsp;:

* les téléchargements sont plus rapides sur internet,
* de l'espace mémoire est économisé sur les ordinateurs,
* la combinaison MIDI + sf3 constitue une solution légère et puissante pour jouer des bandes sons (jeux vidéo, applications mobiles, …).

Contrairement au format <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a> qui répond aux mêmes problématiques, le format sf3 est entièrement open-source.
Le code source est disponible, assurant ainsi sa pérennité (évitons les erreurs commises avec sfArk ou sfPack&nbsp;!).


## Banques de sons sfz {#doc_sfz}


Le format sfz a le même but que le format sf2&nbsp;: créer un instrument de musique virtuel en disposant et configurant des échantillons sur un clavier.
La principale différence réside dans le fait que le format sf2 décrit un seul fichier qui contient tout, alors que le format sfz consiste en un fichier texte délivré avec un ensemble d'échantillons .wav.
Étant donné que le fichier sfz est conçu pour être éditable par un humain, le principal avantage est qu'un éditeur complexe n'est pas nécessaire.
Mais un éditeur est toutefois hautement recommandé pour des grands instruments, la quantité des paramètres devenant en effet rapidement décourageant.
Par ailleurs le format sfz est défini de manière moins stricte que le format sf2&nbsp;: des différences peuvent apparaître dans la manière d'éditer et dans la manière dont les instruments sfz sont joués. Mais par la même occasions, les capacités des instruments sfz ont été augmentées par rapport au standard sf2.


### Limitations sur l'import sfz


* Seuls les opcodes ayant une contrepartie dans les [tables](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table) sont importés.
* L'enveloppe appliquée à la hauteur de note et au filtre étant la même dans les fichiers sf2 (Mod env), importer différentes enveloppes pour la hauteur de note et pour le filtre n'est pas possible (opcodes ``fileg_*`` et ``pitcheg_*``).
* Le LFO appliqué au volume et au filtre étant le même dans les fichiers sf2 (Mod LFO), importer différents LFOs pour le volume et le filtre n'est pas possible (opcodes ``amplfo_*`` et ``fillfo_*``).
* Le seul filtre qui peut être importé est le filtre passe-bas du deuxième ordre (``fil_type=lpf_2p``).
* Pour être importé correctement, les opcodes ``group`` et ``off_by`` doivent contenir la même valeur au sein du même groupe.

**Attention&nbsp;:** les opcodes ``width`` et ``position`` peuvent ne pas être interprétés correctement.


### Limitations sur l'export sfz


* Les [modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) ne sont pour l'instant pas exportés.
* Le format sfz (version 1) n'autorisant qu'une modulation de la hauteur de note, les deux paramètres :guilabel:`Vib LFO → note (c)` and :guilabel:`Mod LFO → note (c)` ne peuvent pas être exportés simultanément.
* Une boucle 0-1 peut être exportée mais elle sera ignorée par la plupart des lecteurs sfz.
* Les échantillons 24 bits peuvent ne pas être supportés par certains lecteurs sfz, vérifiez tout d'abord leurs spécifications.


### En général


Les correspondances de niveaux du son (en dB), durant les processus de conversion sfz / sf2, ont été testées avec le logiciel Sforzando.
Il peut cependant subsister quelques différences.

Par ailleurs, la manière dont les paramètres varient selon la note (via :guilabel:`Note → Vol env maintien / déclin` et :guilabel:`Note → Mod env maintien / déclin`) n'ont pas d'équivalents exacts.
Le format sf2 utilise une loi exponentielle pour définir la durée des phases de maintien et de déclin si ces durées dépendent de la note, alors que le format sf2 utilise une loi linéaire.


## Archives sfArk {#doc_sfark}


Les archives sfArk, tout comme le format sf3, est un format utilisant la compression avec perte.
Ce qui signifie que la qualité des sons est légèrement abaissée (mais souvent imperceptible).
Ce format tend à être obsolète dû à de nombreux problèmes de conception, c'est pourquoi Polyphone ne propose que l'import des archives sfArk et non leur export.



*[MIDI]: Musical Instrument Digital Interface