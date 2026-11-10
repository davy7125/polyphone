Utilisez ces instructions pour compiler Polyphone sur Windows avec la dernière version des sources disponibles sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Vous aurez ainsi le logiciel à son niveau de développement le plus avancé, incluant les dernières fonctionnalités mais peut-être aussi des instabilités.


## Prérequis


### Installation de Qt

Polyphone est écrit en C++ avec l'environnement de développement Qt (version 6). Pour installer Qt, rendez-vous sur cette <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">page</a> pour télécharger l'installateur. Il vous faudra au préalable créer un compte.

Lancez ensuite l'installateur et sélectionnez les éléments suivants&nbsp;:

![Éléments à sélectionner](images/select-qt-components.png "Éléments à sélectionner")

Prenez les versions les plus à jour si ces choix ne sont pas exactement les mêmes.
Le composant «&nbsp;MinGW 64-bit&nbsp;» de la version de Qt fournit les bibliothèques Qt compilées pour MinGW.


### Installation du compilateur et des dépendances

Polyphone est compilé avec MinGW pour ces raisons&nbsp;:

* tout d'abord une raison pratique car les dépendances ci-dessous sont compatibles avec MinGW,
* mais également pour permettre un import / export correct des fichiers au format sfArk (l'option de compilation ``-ffloat-store`` est requise).

Le compilateur, les outils de compilation :program:`CMake` et :program:`Ninja` et les bibliothèques utilisées par Polyphone s'installent avec <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Installez-le dans le répertoire par défaut :file:`C:/msys64`, ouvrez le terminal :program:`MSYS2 MINGW64` et exécutez cette commande&nbsp;:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Dirigez-vous ensuite dans les paramètres système de Windows pour ajouter le dossier :file:`C:/msys64/mingw64/bin` dans le Path&nbsp;:

* ouvrez le menu Windows,
* cherchez «&nbsp;variables d'environnement&nbsp;»,
* ouvrez les propriétés système et cliquez sur «&nbsp;Variables d'environnement&nbsp;»,
* double-cliquez sur «&nbsp;Path&nbsp;» dans les variables système,
* et ajoutez :file:`C:/msys64/mingw64/bin` parmi les répertoires listés dans la variable «&nbsp;Path&nbsp;».

De cette manière, le compilateur, les outils et les bibliothèques de MSYS2 pourront être trouvés par Qt Creator et par Polyphone.


## Compilation


### Sources

Tout d'abord, téléchargez les sources de Polyphone <a href="software" target="_blank">ici</a> ou depuis <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> et désarchivez l'ensemble sur votre bureau Windows par exemple.

### Compilation avec Qt Creator

Continuez maintenant avec ce tutoriel pour la compilation de Polyphone avec Qt Creator&nbsp;: [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md).
Le kit utilisé doit comporter le compilateur de MSYS2 (:file:`C:\msys64\mingw64\bin\g++.exe`), qui trouve les bibliothèques installées avec MSYS2, et la version de Qt pour MinGW 64-bit.

### Compilation en ligne de commande

Cette méthode est celle utilisée pour construire les versions Windows de Polyphone.
Ouvrez le terminal :program:`MSYS2 MINGW64`, placez-vous à la racine du projet (le répertoire contenant :file:`sources`) et exécutez les commandes suivantes (adaptez le chemin de Qt au préalable)&nbsp;:

```
export PATH="/mingw64/bin:/c/Qt/6.6.2/mingw_64/bin:$PATH"
cmake -S sources -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/6.6.2/mingw_64
cmake --build build --parallel 4
```

Si la compilation s'est bien déroulée, vous devriez voir l'exécutable :file:`polyphone.exe` dans le répertoire :file:`build/bin`.


### Lancer Polyphone

Pour rendre possible l'exécution de manière indépendante, il est nécessaire de compléter le dossier de l'exécutable avec les bibliothèques de Qt.
Exécutez la commande suivante dans un terminal (adaptez les chemins au préalable)&nbsp;:

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe --release "C:\Users\User\Desktop\polyphone\build\bin\polyphone.exe"
```

Vous verrez apparaître les bibliothèques Qt (:file:`Qt6Core` par exemple) et un double-clic sur :file:`polyphone.exe` permettra de lancer le logiciel.
Pour permettre le lancement de Polyphone sur un autre PC Windows, ces fichiers en provenance de :file:`C:\msys64\mingw64\bin` sont également nécessaires pour compléter le dossier&nbsp;:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Dépannage


Cette [section](forum/support-bug-reports) dans le forum vous permettra d'obtenir de l'aide.
