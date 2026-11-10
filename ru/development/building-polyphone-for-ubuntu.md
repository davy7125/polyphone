Следуйте этим инструкциям, если не удаётся установить Polyphone в вашем дистрибутиве Linux с помощью установщиков из раздела «[Программа](software)».
Именно этим способом собираются Linux-версии Polyphone в Ubuntu 24.04.


## Что понадобится


Необходимы следующие инструменты и библиотеки:

* компилятор (``build-essential``)
* cmake (``cmake``) и pkg-config (``pkgconf``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` и ``qt6-l10n-tools``)
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

Их можно установить следующей командой:

```
sudo apt-get install build-essential cmake pkgconf qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Также нужен исходный код Polyphone, доступный <a href="software" target="_blank">здесь</a> или на <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Сборка


Polyphone собирается с помощью :program:`CMake`; проект описан файлом :file:`CMakeLists.txt` в каталоге :file:`sources`.
В корне проекта (в каталоге, содержащем :file:`sources`) откройте терминал и выполните следующие команды:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel 4
```

Если всё прошло успешно, в каталоге :file:`build/bin` появится исполняемый файл :file:`polyphone`.
Его можно установить в систему командой :command:`sudo cmake --install build`.

Если в вашем дистрибутиве отсутствуют такие библиотеки, как :program:`RtAudio` или :program:`RtMidi`, или их версии несовместимы, вместо них можно использовать копии, включённые в исходный код Polyphone, добавив к первой команде следующие параметры:

```
-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON
```

Пакеты ``librtaudio-dev`` и ``librtmidi-dev`` в этом случае не нужны.

К первой команде можно добавить и другие параметры:

-DPOLYPHONE_QT_VERSION=5
: Собирает Polyphone с Qt 5 вместо Qt 6 (по умолчанию используется самая новая из найденных версий).
  Пакеты Qt 5 в этом случае — ``qtbase5-dev``, ``libqt5svg5-dev``, ``qttools5-dev`` и ``qttools5-dev-tools``.

-DPOLYPHONE_BUILD_PLUGIN=ON
: Собирает также [плагин](development/building-the-polyphone-plugin.md).

-DPOLYPHONE_WOLFSSL=ON
: Использует :program:`wolfSSL` вместо :program:`OpenSSL`.

**Примечание:** если вы используете :program:`Qt Creator`, проект открывается через файл :file:`CMakeLists.txt` в каталоге :file:`sources` (см. [Сборка Polyphone в Qt Creator](development/using-qt-creator-to-build-polyphone.md)).
