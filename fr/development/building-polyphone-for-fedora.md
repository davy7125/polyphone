Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre distribution Fedora avec les installeurs disponibles dans la section «&nbsp;[Download](download)&nbsp;» (vérifié le 7 août 2016 sur Fedora 24, 64bit).


## Prérequis


### Installation de Qt

Après une installation classique Fedora, installez les paquets suivants (incluant un compilateur) comme prérequis pour le développement avec Qt&nbsp;:

```
sudo dnf update
sudo dnf groupinstall "C Development Tools and Libraries"
sudo dnf install gcc-c++
sudo dnf install mesa-libGL-devel
```

Vous pouvez ensuite installer le framework Qt avec la commande suivante&nbsp;:

```
sudo dnf install qt-creator
```

ou installer la dernière version du framework Qt et Qt Creator disponibles <a href="https://www.qt.io/download-open-source/" target="_blank">ici</a>.
Téléchargez l'installeur Qt et utilisez ces commandes pour ajouter les permessions d'exécution et lancer l'installer (le numéro de version est à ajuster selon votre configuration)&nbsp;:

```
chmod 755 qt-unified-linux-x64-2.0.3-1-online.run
./qt-unified-linux-x64-2.0.3-1-online.run
```

Sélectionnez les composants proposés par défaut — sauf raisons particulières.


![Installation de Qt dans Fedora](images/qt-setup-fedora.png "Installation de Qt dans Fedora")
   

Si vous souhaitez utiliser Qt Creator, installez les dépendances ci-dessous et continuez avec ce tutoriel&nbsp;: [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md).


### Dépendances de Polyphone

Installez les dépendances suivantes (si vous avez :program:`Synaptic` installé vous pouvez aussi l'utiliser)&nbsp;:

```
sudo dnf install qt-devel
sudo ln -s /usr/bin/qmake-qt4 /usr/bin/qmake
sudo dnf install zlib-devel
sudo dnf install alsa-lib-devel
sudo dnf install jack-audio-connection-kit-devel
sudo dnf install portaudio-devel
sudo dnf install libogg-devel
sudo dnf install libvorbis-devel
```


## Compilation


### Sources


Premièrement, télécharger les sources de Polyphone <a href="download" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">Github</a>.


### Modifications du fichier "polyphone.pro"


Modifiez le fichier :file:`polyphone.pro` de manière à utilisez les versions fournies de :program:`rtmidi`, :program:`stk` et :program:`qcustomplot`.
Décommentez les lignes suivantes en enlevant le premier caractère ‘#’:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
DEFINES += USE_LOCAL_QCUSTOMPLOT
```

**Note&nbsp;:** au lieu d'introduire ces modifications vous pouvez essayer de compiler Polyphone avec les paquets distribués par Fedora / RPM. Les commandes d'installations sont&nbsp;:

```
sudo dnf install rtmidi-devel
sudo dnf install stk-devel
sudo dnf install qcustomplot-devel
```

Cependant, en utilisant la commande à venir :command:`qmake`, il est indiqué que :program:`rtmidi` n'est pas détecté sur le système.
Étant donné que sur Windows et Mac l'utilisation des versions distribuées de :program:`rtmidi`, :program:`stk` et :program:`qcustomplot` est obligatoire, vous pouvez considérer qu'il n'y a aucun bénéfice à résoudre le problème de :program:`rtmidi`.


### Obtenir l'exécutable


Placez-vous dans le répertoire des sources (là où se trouve :file:`polyphone.pro`), ouvrez un terminal et compilez Polyphone avec cette commande&nbsp;:

```
qmake && make
```

Si tout se passe bien, vous aurez le fichier compilé :file:`polyphone` dans le sous-répertoire :file:`RELEASE`.


## Dépannage


Voir ce [sujet](forum/support-bug-reports/9-success-polyphone-fedora-rpm-linux-build) pour plus d'informations ou pour demander de l'aide.

<p class="endpage">Un grand merci à <a href="dashboard/820-tomotello">Tomotello</a> pour cet article.</p>