Используйте эти указания, чтобы узнать, как собрать Polyphone с помощью Qt Creator.
Предполагается, что на вашем компьютере уже установлен Qt Creator с фреймворком Qt, компилятором и необходимыми зависимостями для сборки Polyphone (см. соответствующие страницы).


## Перед тем как начать


Требуются следующие элементы:

* компилятор C++ (VC++, Clang, GCC, …),
* Qt Creator с фреймворком,
* зависимости Polyphone.

Инструкции по их установке зависят от используемой вами ОС, см. соответствующие страницы документации для этого.


## Сборка Polyphone


### Скопируйте исходники


Загрузите исходники Polyphone [здесь][download] или на [Github][polyphone].


### Откройте и настройте проект


Сначала измените файл :file:`polyphone.pro`, чтобы использовать локальные версии :program:`rtmidi`, :program:`stk` и :program:`qcustomplot`, поставляемые вместе с исходниками Polyphone, если ваша ОС их не предоставляет.
Раскомментируйте следующие строки, удалив начальный символ «#»:

```
DEFINES += USE_LOCAL_RTMIDI
DEFINES += USE_LOCAL_STK
DEFINES += USE_LOCAL_QCUSTOMPLOT
```

Вы можете пропустить эти изменения, если хотите использовать библиотеки, уже установленные на вашем компьютере (убедитесь, что они у вас есть!).

Запустите Qt Creator и откройте файл :file:`polyphone.pro`.


![Запуск Qt Creator](images/open-Qt-Creator.png "Запуск Qt Creator")


Затем настройте проект.


![Настройка проекта](images/configure-project.png "Настройка проекта")


### Запустите qmake и соберите проект


Зайдите в меню :guilabel:`Build` и выберите пункт :guilabel:`Run qmake`.


![Запуск qmake](images/run-qmake.png "Запуск qmake")


Работа :command:`qmake` завершится, когда полоса в правом нижнем углу достигнет 100% зелёного цвета.


![Завершение работы qmake](images/qmake-finished.png "Завершение работы qmake")


В разделе :guilabel:`Project` в левом столбце, затем в разделе :guilabel:`Build&Run` выберите конфигурацию сборки (предпочтительнее :guilabel:`Release`).

Наконец, нажмите на действие :guilabel:`Build project Polyphone` в меню :guilabel:`Build`.


![Сборка проекта](images/build-project.png "Сборка проекта")


Во время сборки проекта вы можете увидеть предупреждения, но ошибок быть не должно.

Теперь у вас должен быть исполняемый файл :file:`polyphone` в подпапке :file:`RELEASE` или :file:`DEBUG` исходного кода, в зависимости от того, что вы выбрали в конфигурации сборки QtCreator.


## Отладка


Просмотрите эту [тему][topic], если вам нужна дополнительная информация или помощь.

<p class="endpage">Отдельное спасибо <a href="dashboard/820-tomotello">Tomotello</a> за эту статью.</p>


[download]:  download                              {target=_blank}
[polyphone]: https://github.com/davy7125/polyphone {target=_blank rel=noopener}

[topic]: forum/polyphone/support-bug-reports
