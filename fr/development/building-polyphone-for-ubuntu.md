Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre distribution Linux avec les installeurs fournis dans la section «&nbsp;[Download]&nbsp;».
Cette méthode a été testée avec Ubuntu 16.04 (Xenial).


## Prérequis


Les bibliothèques suivantes sont nécessaires&nbsp;:

* qt (``qt5-default`` + ``libqt5svg5-dev``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* portaudio (``portaudio19-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk0-dev``)
* qcustomplot (``libqcustomplot-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* ssl (``libssl-dev``)

Utilisez par exemple :program:`synaptic` pour les installer.

Vous aurez également besoin des sources du logiciel, disponibles [ici][Download] ou sur [GitHub][polyphone].


## Compilation


À la racine du projet, ouvrez un terminal et compilez Polyphone en utilisant la commande suivante&nbsp;:

```
qmake polyphone.pro PREFIX=/usr && make
```

Un fichier exécutable :file:`polyphone` dans le répertoire :file:`RELEASE` devrait ensuite apparaître si tout s'est bien déroulé.

Dans le cas où des bibliothèques comme :program:`RtMidi`, :program:`Stk`, :program:`QCustomPlot` sont manquantes ou ne sont pas dans une version compatible dans votre distribution, vous pouvez éditer le fichier :file:`.pro` pour utiliser la version fournie dans les sources.
Pour ce faire, décommentez les lignes correspondantes. Par exemple

```
#DEFINES += USE_LOCAL_RTMIDI
```

deviendra

```
DEFINES += USE_LOCAL_RTMIDI
```

(sans “#” en début de ligne).

<!-- note begin -->
**Note&nbsp;:** si vous utilisez :program:`Qt Creator`, le projet peut être ouvert avec le fichier :file:`.pro` présent à la racine des sources.
<!-- note end -->


[Download]:  download                              {target=_blank}
[polyphone]: https://github.com/davy7125/polyphone {target=_blank rel=noopener}
