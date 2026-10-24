Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre distribution Linux avec les installeurs fournis dans la section «&nbsp;[Logiciel](software)&nbsp;».
Cette méthode est celle utilisée pour construire les versions Linux de Polyphone, sous Ubuntu 24.04.


## Prérequis


Les outils et bibliothèques suivants sont nécessaires&nbsp;:

* un compilateur (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` et ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Ils peuvent être installés avec la commande suivante&nbsp;:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Vous aurez également besoin des sources du logiciel, disponibles <a href="software" target="_blank">ici</a> ou sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilation


Dans le répertoire :file:`sources` du projet (là où se trouve le fichier :file:`polyphone.pro`), ouvrez un terminal et compilez Polyphone en utilisant la commande suivante&nbsp;:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

Un fichier exécutable :file:`polyphone` dans le répertoire :file:`bin` devrait ensuite apparaître si tout s'est bien déroulé.
Il peut être installé sur le système avec la commande :command:`sudo make install`.

Par défaut, une copie locale de la bibliothèque :program:`RtAudio` est utilisée.
Dans le cas où des bibliothèques comme :program:`RtMidi` ou :program:`Stk` sont manquantes ou ne sont pas dans une version compatible dans votre distribution, vous pouvez éditer le fichier :file:`.pro` pour utiliser la version fournie dans les sources.
Pour ce faire, décommentez les lignes correspondantes. Par exemple

```
#DEFINES += USE_LOCAL_RTMIDI
```

deviendra

```
DEFINES += USE_LOCAL_RTMIDI
```

(sans «&nbsp;#&nbsp;» en début de ligne).

**Note&nbsp;:** si vous utilisez :program:`Qt Creator`, le projet peut être ouvert avec le fichier :file:`.pro` présent dans le répertoire :file:`sources`.
