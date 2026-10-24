Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre distribution Fedora avec les installeurs disponibles dans la section «&nbsp;[Logiciel](software)&nbsp;».


## Prérequis


### Installation de Qt

Après une installation classique de Fedora, installez les paquets suivants (incluant un compilateur) comme prérequis pour le développement avec Qt&nbsp;:

```
sudo dnf update
sudo dnf install @development-tools
```

Installez ensuite le framework Qt 6 avec la commande suivante&nbsp;:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Dépendances de Polyphone

Installez les dépendances suivantes (si vous avez un gestionnaire de paquets graphique, vous pouvez aussi l'utiliser)&nbsp;:

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**Note&nbsp;:** avec les versions récentes de Fedora, le paquet :program:`jack-audio-connection-kit-devel` peut être remplacé par :program:`pipewire-jack-audio-connection-kit-devel`.


## Compilation


### Sources


Premièrement, téléchargez les sources de Polyphone <a href="software" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Si vous souhaitez continuer la compilation avec Qt Creator, suivez maintenant ce tutoriel&nbsp;: [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md). Sinon, continuez la lecture de ce document.


### Obtenir l'exécutable


Placez-vous dans le répertoire des sources (là où se trouve :file:`polyphone.pro`), ouvrez un terminal et compilez Polyphone avec cette commande&nbsp;:

```
qmake6 && make
```

Si tout se passe bien, vous aurez le fichier compilé :file:`polyphone` dans le sous-répertoire :file:`bin`.
Lorsque la compilation est effectuée avec Qt Creator, ce fichier se trouve plutôt dans un répertoire :file:`build-XXX` créé à côté de :file:`sources`.


## Dépannage


Cette [section](forum/support-bug-reports) dans le forum vous permettra d'obtenir de l'aide.
