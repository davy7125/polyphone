Следуйте этим инструкциям, если не удаётся установить Polyphone на Mac с помощью установщиков из раздела «[Программа](software)».


## Что понадобится


Необходимы следующие приложения:

* <a href="https://www.qt.io/download-open-source" target="_blank">Qt Creator с фреймворком</a> (версия 6),
* Xcode или только «Command Line Tools» (нужная версия указана на сайте Qt — например, <a href="https://doc.qt.io/qt-6/supported-platforms.html" target="_blank">здесь</a>).

Также нужен исходный код Polyphone, доступный <a href="software" target="_blank">здесь</a> или на <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Чтобы Qt Creator правильно открыл проект, ему сначала нужно дать разрешение на чтение всех файлов компьютера («Конфиденциальность и безопасность» => «Доступ к диску» => добавить «Qt Creator.app» из его каталога установки).

Библиотеки, используемые Polyphone, легко устанавливаются с помощью <a href="https://brew.sh/" target="_blank">Homebrew</a>. После установки этого инструмента выполните команду:

```
brew install libsndfile libogg libvorbis flac lame mpg123 opus openssl
```

Файл проекта :file:`polyphone.pro` ищет заголовочные файлы и библиотеки в :file:`/opt/homebrew` (Apple Silicon), в :file:`/usr/local` (Intel) и в каталоге :file:`macos`, расположенном рядом с каталогом :file:`sources`.
Каталог :file:`macos` можно также получить, распаковав архив статических библиотек, доступный [здесь](files/macos.zip), но его содержимое может быть устаревшим.

Если какие-то библиотеки устарели или несовместимы с версией вашего Mac (например, SSL), придётся найти их исходный код в интернете и перекомпилировать их со статической компоновкой:

```
configure --enable-static --disable-shared --prefix=...
make
make install
```

Затем скопируйте все файлы .h (заголовочные) в :file:`macos/include`, а все файлы .a (статические библиотеки) — в :file:`macos/lib`.


## Сборка


Откройте файл :file:`polyphone.pro` в :program:`Qt Creator` (см. [Сборка Polyphone в Qt Creator](development/using-qt-creator-to-build-polyphone.md)) и соберите проект.
В каталоге :file:`macos` рядом с каталогом :file:`sources` должен появиться пакет :file:`Polyphone.app`.

Собрать проект можно и из командной строки:

```
cd sources
qmake polyphone.pro CONFIG+=release
make -j4
```

Если компилятор сообщает, что некоторые функции недоступны для указанной целевой версии macOS, можно увеличить значение этой переменной в файле :file:`polyphone.pro` (значение по умолчанию — 12.0):

```
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
```

Это значение также нужно указать в файле :file:`packaging/mac/polyphone.plist` под «LSMinimumSystemVersion».


## Завершение пакета


Выполните следующую команду, чтобы включить библиотеки и фреймворки в пакет (сначала поправьте путь!):

```
/Path/To/Qt/6.7.3/macos/bin/macdeployqt /Path/To/macos/Polyphone.app
```

На Mac с процессором Apple Silicon пакет также нужно подписать (хотя бы ad-hoc подписью), иначе он не запустится:

```
xattr -cr /Path/To/macos/Polyphone.app
codesign --force --deep --sign - /Path/To/macos/Polyphone.app
```

После этого программу можно запустить или упаковать в файл .zip или .dmg, чтобы поделиться ею.


## Устранение неполадок


Если нужна дополнительная информация или помощь, см. эту [тему](forum/support-bug-reports/8-success-build-polyphone-on-osx-10-11-6-qt-5-7).
