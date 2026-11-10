Следуйте этим инструкциям, если не удаётся установить Polyphone в вашем дистрибутиве Fedora с помощью установщиков из раздела «[Программа](software)».


## Что понадобится


### Установка Qt

После базовой установки Fedora установите следующие пакеты (включая компилятор и :program:`CMake`), необходимые для разработки на Qt:

```
sudo dnf update
sudo dnf install @development-tools cmake pkgconf
```

Затем установите фреймворк Qt 6 командой:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Зависимости Polyphone

Установите следующие зависимости (если установлен графический менеджер пакетов, можно воспользоваться им):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel libsndfile-devel
```

**Примечание:** в последних версиях Fedora пакет :program:`jack-audio-connection-kit-devel` может быть заменён пакетом :program:`pipewire-jack-audio-connection-kit-devel`.


## Сборка


### Исходный код


Сначала получите исходный код Polyphone <a href="software" target="_blank">здесь</a> или с <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Если вы хотите продолжить сборку в Qt Creator, следуйте этому руководству: [сборка в Qt Creator](development/using-qt-creator-to-build-polyphone.md). В противном случае читайте этот документ дальше.


### Получение исполняемого файла


Polyphone собирается с помощью :program:`CMake`; проект описан файлом :file:`CMakeLists.txt` в каталоге :file:`sources`.
В корне проекта (в каталоге, содержащем :file:`sources`) откройте терминал и соберите Polyphone командами:

```
cmake -S sources -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

Если всё пройдёт успешно, скомпилированный файл :file:`polyphone` появится в каталоге :file:`build/bin`.
При сборке в Qt Creator этот файл находится в подкаталоге :file:`bin` каталога сборки, выбранного в Qt Creator.

Если :program:`RtAudio` или :program:`RtMidi` недоступны в совместимой версии, можно использовать их копии, включённые в исходный код Polyphone, добавив к первой команде параметры ``-DPOLYPHONE_LOCAL_RTAUDIO=ON -DPOLYPHONE_LOCAL_RTMIDI=ON``.
Остальные параметры описаны на странице [Сборка Polyphone для Ubuntu](development/building-polyphone-for-ubuntu.md).


## Устранение неполадок


При необходимости помощь можно получить в этой [категории](forum/support-bug-reports) форума.
