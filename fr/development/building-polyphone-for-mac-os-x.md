Utilisez ces instructions si vous ne pouvez pas installer Polyphone sur votre Mac avec les installeurs disponibles dans la section «&nbsp;[Logiciel](software)&nbsp;».


## Prérequis


Les éléments suivants seront nécessaires&nbsp;:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator avec son framework</a> (version 6),
* Xcode ou seulement les «&nbsp;Command Line Tools&nbsp;» (la version à utiliser est indiquée sur le site de Qt — <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">ici</a> par exemple).

Vous aurez également besoin des sources de Polyphone, disponibles <a href="software" target="_blank">ici</a> ou sur <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Pour pouvoir ouvrir correctement un projet avec Qt Creator, il faut au préalable lui donner la permission de lire tous les fichiers (dans «&nbsp;Confidentialité et sécurité&nbsp;» => «&nbsp;Accès complet au disque&nbsp;» => ajouter «&nbsp;Qt Creator.app&nbsp;» qui se trouve dans son répertoire d'installation).

L'outil de compilation :program:`CMake` et les bibliothèques utilisées par Polyphone s'installent facilement avec <a href="https://brew.sh/" target="_blank">Homebrew</a>. Après l'installation de Homebrew, exécutez cette commande&nbsp;:

```
brew install cmake pkgconf libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

Le fichier :file:`CMakeLists.txt` du répertoire :file:`sources` cherche les en-têtes et les bibliothèques dans :file:`/opt/homebrew` (Apple Silicon), dans :file:`/usr/local` (Intel) et dans le répertoire :file:`macos` situé à côté du répertoire :file:`sources`.
L'archive de bibliothèques statiques disponible [ici](files/macos.zip) peut aussi être décompressée pour obtenir le répertoire :file:`macos`, mais son contenu peut ne pas être à jour.

Si certaines bibliothèques ne sont pas à jour ou pas compatibles avec la version de votre Mac, comme SSL par exemple, il faudra aller chercher leurs sources sur internet et les recompiler en autorisant une liaison statique&nbsp;:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Ensuite, copiez tous les .h (en-têtes) dans :file:`macos/include` et tous les .a (bibliothèques statiques) dans :file:`macos/lib`.


## Compilation


Ouvrez le fichier :file:`CMakeLists.txt` du répertoire :file:`sources` avec :program:`Qt Creator` (voir [Compiler avec Qt Creator](development/using-qt-creator-to-build-polyphone.md)) et compilez le projet.
Un bundle :file:`Polyphone.app` devrait apparaître dans le sous-répertoire :file:`bin` du répertoire de compilation.

La compilation peut aussi être effectuée en ligne de commande, à la racine du projet (le répertoire contenant :file:`sources`), en indiquant le chemin de Qt (ajustez-le au préalable&nbsp;!)&nbsp;:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/Path/To/Qt/6.7.3/macos
cmake --build build --parallel 4
```

Le bundle :file:`Polyphone.app` se trouve alors dans le répertoire :file:`build/bin`.

Si le compilateur indique que certaines fonctionnalités ne sont pas disponibles dans la cible macOS spécifiée, vous pouvez augmenter la version minimale de macOS en ajoutant cette option à la première commande (la valeur par défaut est 12.0)&nbsp;:

```
-DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
```

Cette valeur est à reporter dans le fichier :file:`packaging/mac/polyphone.plist` sous «&nbsp;LSMinimumSystemVersion&nbsp;».


## Compléter le bundle


Lancez la commande suivante pour intégrer les dépendances et le framework Qt dans le bundle (ajustez les chemins au préalable&nbsp;!)&nbsp;:

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/build/bin/Polyphone.app
```

Sur un Mac équipé d'un processeur Apple Silicon, le bundle doit également être signé (même avec une signature ad hoc) pour pouvoir s'exécuter&nbsp;:

```
xattr -cr /Path/To/build/bin/Polyphone.app
codesign --force --deep --sign - /Path/To/build/bin/Polyphone.app
```

Vous pourrez ensuite exécuter le programme grâce à un double-clic sur le bundle ou le compresser en fichier .zip ou .dmg pour le partager.


## Dépannage


Voir ce [sujet](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7) pour plus d'informations ou pour obtenir de l'aide.
