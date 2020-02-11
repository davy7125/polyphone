Используйте эти указания, если вы не можете установить Polyphone в свой дистрибутив Linux с помощью доступных инсталляторов в разделе “[Download]”.
Этот метод был протестирован с Ubuntu 16.04 (Xenial).


## Перед тем как начать


Требуются следующие библиотеки:

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

Установите их с помощью :program:`synaptic`.

Вам также нужны исходники Polyphone, доступные [здесь][Download] или [на GitHub][polyphone].


## Сборка


В корневом каталоге проекта откройте терминал и соберите Polyphone, выполнив следующую команду:

```
qmake polyphone.pro PREFIX=/usr && make
```

Если всё прошло правильно, в каталоге :file:`RELEASE` должен появиться исполняемый файл :file:`polyphone`.

В случае, если библиотеки, такие как :program:`RtMidi`, :program:`Stk`, :program:`QCustomPlot` отсутствуют или не совместимой версии в вашем дистрибутиве, вы можете отредактировать файл :file:`.pro`, чтобы использовать их локальные копии.
Для этого раскомментируйте соответствующие строки. Например,

```
#DEFINES += USE_LOCAL_RTMIDI
```

станет

```
DEFINES += USE_LOCAL_RTMIDI
```

(без начального «#»).

<!-- note begin -->
**Примечание:**
Если вы используете :program:`Qt Creator`, проект можно открыть через его файл :file:`.pro`, находящийся в корневом каталоге.
<!-- note end -->


[Download]:  download                              {target=_blank}
[polyphone]: https://github.com/davy7125/polyphone {target=_blank rel=noopener}
