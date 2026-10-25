Следуйте этим инструкциям, если не удаётся установить Polyphone в вашем дистрибутиве Fedora с помощью установщиков из раздела «[Программа](software)».


## Что понадобится


### Установка Qt

После базовой установки Fedora установите следующие пакеты (включая компилятор), необходимые для разработки на Qt:

```
sudo dnf update
sudo dnf install @development-tools
```

Затем установите фреймворк Qt 6 командой:

```
sudo dnf install qt-creator qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel
```


### Зависимости Polyphone

Установите следующие зависимости (если установлен графический менеджер пакетов, можно воспользоваться им):

```
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
```

**Примечание:** в последних версиях Fedora пакет :program:`jack-audio-connection-kit-devel` может быть заменён пакетом :program:`pipewire-jack-audio-connection-kit-devel`.


## Сборка


### Исходный код


Сначала получите исходный код Polyphone <a href="software" target="_blank">здесь</a> или с <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.

Если вы хотите продолжить сборку в Qt Creator, следуйте этому руководству: [сборка в Qt Creator](development/using-qt-creator-to-build-polyphone.md). В противном случае читайте этот документ дальше.


### Получение исполняемого файла


Перейдите в каталог с исходным кодом (где находится файл :file:`polyphone.pro`), откройте терминал и соберите Polyphone командой:

```
qmake6 && make
```

Если всё пройдёт успешно, скомпилированный файл :file:`polyphone` появится в подкаталоге :file:`bin`.
При сборке в Qt Creator этот файл находится в каталоге :file:`build-XXX`, создаваемом рядом с :file:`sources`.


## Устранение неполадок


При необходимости помощь можно получить в этой [категории](forum/support-bug-reports) форума.
