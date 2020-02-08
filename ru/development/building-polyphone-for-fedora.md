Используйте эти указания, если вы не можете установить Polyphone в свой дистрибутив Fedora с помощью доступных установщиков в разделе “[Download](download)” (проверено:&nbsp;7 августа 2016 г. для Fedora 24, 64-битная версия).


## Перед тем как начать


### Установка Qt


После базовой установки Fedora установите следующие пакеты (включая компилятор), необходимые для разработки приложений, использующих Qt:


```
sudo dnf update
sudo dnf groupinstall "C Development Tools and Libraries"
sudo dnf install gcc-c++
sudo dnf install mesa-libGL-devel
```

Затем вы можете установить стандартный фреймворк Qt с помощью следующей команды:

```
sudo dnf install qt-creator
```

или установить последнюю версию фреймворка Qt и Qt Creator, доступные [здесь][get-qt].
Загрузите установщик Qt и выполните эти команды, чтобы добавить разрешение на выполнение и запустить установщик (возможно, вам придётся изменить номер версии):

```
chmod 755 qt-unified-linux-x64-2.0.3-1-online.run
./qt-unified-linux-x64-2.0.3-1-online.run
```

Выберите компоненты по умолчанию, как предлагается, если вам не нужно больше для других целей.


![Установка Qt в Fedora](images/qt-setup-fedora.png "Установка Qt в Fedora")


Если вы хотите использовать Qt Creator, установите зависимости Polyphone, как указано ниже, и продолжите работу по этому документу: [Использование Qt Creator для сборки Polyphone](development/using-qt-creator-to-build-polyphone.md).


### Зависимости Polyphone


Установите следующие зависимости (если у вас установлен :program:`Synaptic`, вы можете использовать его для установки):

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


## Сборка


### Исходники


Сначала загрузите исходники Polyphone [здесь][download] или на [Github][polyphone].


### Изменения в polyphone.pro


Измените файл :file:`polyphone.pro`, чтобы использовать локальные версии :program:`rtmidi`, :program:`stk` и :program:`qcustomplot`, поставляемые вместе с исходниками Polyphone. Раскомментируйте следующие строки, удалив начальный символ «#»:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
DEFINES += USE_LOCAL_QCUSTOMPLOT
```

**Примечание:** Вместо этих изменений вы можете попробовать собрать Polyphone с пакетами RPM, предлагаемыми дистрибутивом Fedora. Команды установки будут следующими:

```
sudo dnf install rtmidi-devel
sudo dnf install stk-devel
sudo dnf install qcustomplot-devel
```

Однако после запуска :command:`qmake` сообщается, что не удалось найти :program:`rtmidi` в системе.
В системах Windows и Mac локальные версии :program:`rtmidi`, :program:`stk` и :program:`qcustomplot` используются принудительно, вы можете не увидеть никакой выгоды в решении проблемы с :program:`rtmidi`.


### Компиляция


Перейдите в каталог с исходниками (где находится файл :file:`polyphone.pro`), откройте терминал и соберите Polyphone с помощью этой команды:

```
qmake && make
```

Если всё пойдёт хорошо, у вас появится скомпилированный :file:`polyphone` в подкаталоге :file:`RELEASE`.


## Отладка


Просмотрите эту [тему](forum/support-bug-reports/9-success-polyphone-fedora-rpm-linux-build), если вам нужна дополнительная информация или помощь.


<p class="endpage">Отдельное спасибо <a href="dashboard/820-tomotello">Tomotello</a> за эту статью.</p>


[get-qt]:    https://www.qt.io/download-open-source/ {target=_blank}
[download]:  download                                {target=_blank}
[polyphone]: https://github.com/davy7125/polyphone   {target=_blank}
