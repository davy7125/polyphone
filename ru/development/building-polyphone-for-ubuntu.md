Следуйте этим инструкциям, если не удаётся установить Polyphone в вашем дистрибутиве Linux с помощью установщиков из раздела «[Программа](software)».
Именно этим способом собираются Linux-версии Polyphone в Ubuntu 24.04.


## Что понадобится


Необходимы следующие инструменты и библиотеки:

* компилятор (``build-essential``)
* qt 6 (``qt6-base-dev``, ``qt6-svg-dev``, ``qt6-tools-dev``, ``qt6-tools-dev-tools`` и ``qt6-l10n-tools``)
* alsa (``libasound2-dev``)
* jack (``libjack-jackd2-dev``)
* rtaudio (``librtaudio-dev``)
* rtmidi (``librtmidi-dev``)
* stk (``libstk-dev``)
* vorbis (``libvorbis-dev``)
* ogg (``libogg-dev``)
* flac (``libflac-dev``)
* zlib (``zlib1g-dev``)
* ssl (``libssl-dev``)
* sndfile (``libsndfile1-dev``)

Их можно установить следующей командой:

```
sudo apt-get install build-essential qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libasound2-dev libjack-jackd2-dev librtaudio-dev librtmidi-dev libstk-dev libvorbis-dev libogg-dev libflac-dev zlib1g-dev libssl-dev libsndfile1-dev
```

Также нужен исходный код Polyphone, доступный <a href="software" target="_blank">здесь</a> или на <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


## Сборка


В каталоге :file:`sources` проекта (где находится файл :file:`polyphone.pro`) откройте терминал и соберите Polyphone, выполнив команду:

```
qmake6 polyphone.pro PREFIX=/usr && make
```

Если всё прошло успешно, в каталоге :file:`bin` появится исполняемый файл :file:`polyphone`.
Его можно установить в систему командой :command:`sudo make install`.

По умолчанию используется локальная копия библиотеки :program:`RtAudio`.
Если в вашем дистрибутиве отсутствуют такие библиотеки, как :program:`RtMidi` или :program:`Stk`, или их версии несовместимы, можно отредактировать файл :file:`.pro`, чтобы использовать их локальные копии.
Для этого раскомментируйте соответствующие строки. Например,

```
#DEFINES += USE_LOCAL_RTMIDI
```

превратится в

```
DEFINES += USE_LOCAL_RTMIDI
```

(без «#» в начале).

**Примечание:** если вы используете :program:`Qt Creator`, проект можно открыть через его файл :file:`.pro` в каталоге :file:`sources`.
