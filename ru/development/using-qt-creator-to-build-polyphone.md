Эти инструкции описывают сборку Polyphone в Qt Creator.
Предполагается, что на вашем компьютере уже установлены Qt Creator с фреймворком Qt, компилятор и зависимости, необходимые для сборки Polyphone (см. соответствующие страницы).


## Что понадобится


Необходимы следующие компоненты:

* компилятор C++ (GCC, Clang, MinGW…),
* Qt Creator с фреймворком (Qt 6 или Qt 5),
* :program:`CMake` (версии 3.16 или новее), который можно установить вместе с Qt Creator,
* зависимости Polyphone.

Инструкции по их установке зависят от используемой операционной системы; см. соответствующие страницы документации.


## Сборка Polyphone


### Копирование исходного кода


Получите исходный код Polyphone <a href="software" target="_blank">здесь</a> или с <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>.


### Открытие и настройка проекта


Запустите Qt Creator и откройте файл :file:`CMakeLists.txt`, расположенный в каталоге :file:`sources`.


![Открытие Qt Creator](images/open-Qt-Creator.png "Открытие Qt Creator")


Затем настройте проект, выбрав комплект (kit), то есть версию Qt и компилятор.

![Настройка проекта](images/configure-project.png "Настройка проекта")


После этого Qt Creator запускает :program:`CMake`, который проверяет, что все зависимости найдены.
Сообщения выводятся на панели :guilabel:`General Messages`: если какой-то библиотеки не хватает, установите её и снова запустите :program:`CMake` пунктом :guilabel:`Run CMake` меню :guilabel:`Build`.

Параметры проекта находятся в разделе :guilabel:`Projects` левого столбца, в :guilabel:`Build` → :guilabel:`CMake`.
В Linux вместо системных библиотек можно использовать копии :program:`RtAudio` и :program:`RtMidi`, включённые в исходный код: включите параметры ``POLYPHONE_LOCAL_RTAUDIO`` и ``POLYPHONE_LOCAL_RTMIDI``, затем нажмите :guilabel:`Run CMake`.
В Windows и macOS это не нужно, поскольку там всегда используются прилагаемые версии.
В том же разделе выберите конфигурацию сборки (предпочтительно :guilabel:`Release`).


### Сборка проекта


Наконец, выберите действие :guilabel:`Build Project "Polyphone"` в меню :guilabel:`Build`. Во время сборки проекта могут появляться предупреждения, но ошибок быть не должно.

Теперь исполняемый файл должен находиться в подкаталоге :file:`bin` каталога сборки, созданного Qt Creator (например, :file:`build/Desktop_Qt_6_…-Release` в каталоге :file:`sources`):

* в Linux — файл :file:`polyphone`,
* в Windows — файл :file:`polyphone.exe`,
* в macOS — пакет :file:`Polyphone.app`.


## Устранение неполадок


Если нужна дополнительная информация или помощь, см. эту [категорию](forum/support-bug-reports).


<p class="endpage">Отдельное спасибо <a href="dashboard/820-tomotello">Tomotello</a> за эту статью.</p>
