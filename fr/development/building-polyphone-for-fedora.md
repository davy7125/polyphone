Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre distribution Fedora avec les installeurs disponibles dans la section «&nbsp;[Download](download)&nbsp;» (vérifié le 27 août 2020 sur Fedora 32, 64bit).


## Prérequis


### Installation de Qt

Après une installation classique Fedora, installez les paquets suivants (incluant un compilateur) comme prérequis pour le développement avec Qt&nbsp;:

```
sudo dnf update
sudo dnf install @development-tools
```

Vous pouvez ensuite installer le framework Qt avec la commande suivante&nbsp;:

```
sudo dnf install qt-creator qt5-qtsvg-devel qt5-qttools-devel
```


### Dépendances de Polyphone

Installez les dépendances suivantes (si vous avez :program:`Synaptic` installé vous pouvez aussi l'utiliser)&nbsp;:

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel portaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel
```


## Compilation


### Sources


Premièrement, télécharger les sources de Polyphone <a href="download" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">Github</a>.

Si vous souhaitez continuer la compilation avec Qt Creator, suivez maintenant ce tutoriel&nbsp;: [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md). Sinon, continuez la lecture de ce document.


### Modifications du fichier "polyphone.pro"


Modifiez le fichier :file:`polyphone.pro` de manière à utiliser les versions fournies de :program:`rtmidi`, :program:`stk` et :program:`qcustomplot`.
Décommentez les lignes suivantes en enlevant le premier caractère "#"&nbsp;:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
DEFINES += USE_LOCAL_QCUSTOMPLOT
```

**Note&nbsp;:** au lieu d'introduire ces modifications vous pouvez essayer de compiler Polyphone avec les paquets distribués par Fedora / RPM. La commande d'installation est&nbsp;:

```
sudo dnf install rtmidi-devel stk-devel qcustomplot-devel
```

Cependant, l'exécution de **Polyphone** engendre un crash immédiat (bug dans PortAudio ?).
Étant donné que sur Windows et Mac l'utilisation des versions distribuées de :program:`rtmidi`, :program:`stk` et :program:`qcustomplot` est obligatoire, vous pouvez considérer qu'il n'y a aucun bénéfice à résoudre le problème de ces dépendances.


### Obtenir l'exécutable


Placez-vous dans le répertoire des sources (là où se trouve :file:`polyphone.pro`), ouvrez un terminal et compilez Polyphone avec cette commande&nbsp;:

```
qmake-qt5 && make
```

Si tout se passe bien, vous aurez le fichier compilé :file:`polyphone` dans un répertoire :file:`build-XXX` à côté de :file:`sources`.


## Dépannage


Cette [section](forum/support-bug-reports/) dans le forum vous permettra d'obtenir de l'aide.