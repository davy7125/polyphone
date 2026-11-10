Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre distribution Linux avec les installeurs fournis dans la section «&nbsp;[Logiciel](software)&nbsp;».
Cette méthode est celle utilisée pour construire les versions Linux de Polyphone, sous Ubuntu 24.04.


## Prérequis


Les outils et bibliothèques suivants sont nécessaires&nbsp;:

* un compilateur (``build-essential``)
* cmake (``cmake``) et pkg-config (``pkgconf``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` et ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Ils peuvent être installés avec la commande suivante&nbsp;:

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Vous aurez également besoin des sources du logiciel, disponibles <a href="software" target="_blank">ici</a> ou sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Compilation


Polyphone est compilé avec :program:`CMake`, le projet étant décrit par le fichier :file:`CMakeLists.txt` du répertoire :file:`sources`.
À la racine du projet (le répertoire contenant :file:`sources`), ouvrez un terminal et exécutez les commandes suivantes&nbsp;:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

Un fichier exécutable :file:`polyphone` dans le répertoire :file:`build/bin` devrait ensuite apparaître si tout s'est bien déroulé.
Il peut être installé sur le système avec la commande :command:`sudo cmake --install build`.

Dans le cas où des bibliothèques comme :program:`RtAudio` ou :program:`RtMidi` sont manquantes ou ne sont pas dans une version compatible dans votre distribution, les copies incluses dans les sources de Polyphone peuvent être utilisées à la place, en ajoutant ces options à la première commande&nbsp;:

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

Les paquets ``librtaudio-dev`` et ``librtmidi-dev`` ne sont alors pas nécessaires.

D'autres options peuvent être ajoutées à la première commande&nbsp;:

-DPOLYPHONE_QT_VERSION=5
: Compiler avec Qt 5 au lieu de Qt 6 (par défaut, la version la plus récente trouvée est utilisée).
  Les paquets de Qt 5 sont alors ``qtbase5-dev``, ``libqt5svg5-dev``, ``qttools5-dev`` et ``qttools5-dev-tools``.

-DPOLYPHONE_BUILD_PLUGIN=ON
: Compiler également le [plugin](development/building-the-polyphone-plugin.md).

-DPOLYPHONE_WOLFSSL=ON
: Utiliser :program:`wolfSSL` à la place de :program:`OpenSSL`.

**Note&nbsp;:** si vous utilisez :program:`Qt Creator`, le projet s'ouvre avec le fichier :file:`CMakeLists.txt` présent dans le répertoire :file:`sources` (voir [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md)).
