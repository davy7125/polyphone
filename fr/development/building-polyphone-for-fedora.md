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
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel portaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel qcustomplot-qt5-devel
```


## Compilation


### Sources


Premièrement, télécharger les sources de Polyphone <a href="download" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">Github</a>.

Si vous souhaitez continuer la compilation avec Qt Creator, suivez maintenant ce tutoriel&nbsp;: [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md). Sinon, continuez la lecture de ce document.


### Modifications du fichier "polyphone.pro"


Ouvrez le fichier :file:`polyphone.pro` et changez **-lqcustomplot** en **-lqcustomplot-qt5** (ajoutez "-qt5") dans le bloc suivant&nbsp;:

```
# Location of QCustomplot
contains(DEFINES, USE_LOCAL_QCUSTOMPLOT) {
    INCLUDEPATH += lib/_option_qcustomplot
    HEADERS += lib/_option_qcustomplot/qcustomplot.h
    SOURCES += lib/_option_qcustomplot/qcustomplot.cpp
} else {
    LIBS += -lqcustomplot
}
```

Cette modification est requise sinon Polyphone plantera avec une segmentation fault au démarrage.

Une autre solution est de décommenter (enlever le "#" au début d'une ligne) la ligne suivante pour utiliser une copie embarquée de la bibliothèque qcustomplot&nbsp;:

```
DEFINES += USE_LOCAL_QCUSTOMPLOT
```


### Obtenir l'exécutable


Placez-vous dans le répertoire des sources (là où se trouve :file:`polyphone.pro`), ouvrez un terminal et compilez Polyphone avec cette commande&nbsp;:

```
qmake-qt5 && make
```

Si tout se passe bien, vous aurez le fichier compilé :file:`polyphone` dans un répertoire :file:`build-XXX` à côté de :file:`sources`.


## Dépannage


Cette [section](forum/support-bug-reports/) dans le forum vous permettra d'obtenir de l'aide.