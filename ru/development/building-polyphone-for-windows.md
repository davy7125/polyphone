Следуйте этим инструкциям, чтобы собрать Polyphone для Windows из последней версии исходного кода на <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a>. Вы получите программу на самом последнем этапе разработки, со всеми новыми функциями, но, возможно, и с некоторой нестабильностью.


## Что понадобится


### Установка Qt

Polyphone написан на C++ в среде разработки Qt (версия 6). Чтобы установить Qt, перейдите на эту <a href="https://www.qt.io/download-qt-installer-oss" target="_blank">страницу</a> и скачайте установщик. Предварительно нужно создать учётную запись.

Затем запустите установщик и выберите следующие компоненты:

![Выбираемые компоненты](images/select-qt-components.png "Выбираемые компоненты")

Если варианты выбора не совпадают в точности, выберите последнюю доступную версию.
Компонент «MinGW 64-bit» версии Qt содержит библиотеки Qt, собранные для MinGW.


### Установка компилятора и зависимостей

Polyphone собирается с помощью MinGW по следующим причинам:

* прежде всего по практической причине: перечисленные ниже зависимости совместимы с MinGW,
* а также для корректного импорта / экспорта файлов в формате sfArk (требуется параметр компиляции ``-ffloat-store``).

Компилятор, инструменты сборки :program:`CMake` и :program:`Ninja` и библиотеки, используемые Polyphone, устанавливаются с помощью <a href="https://www.msys2.org/" target="_blank">MSYS2</a>.
Установите его в каталог по умолчанию :file:`C:/msys64`, откройте терминал :program:`MSYS2 MINGW64` и выполните команду:

```
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-zlib mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openssl mingw-w64-x86_64-flac mingw-w64-x86_64-libsndfile
```

Затем в системных настройках Windows добавьте папку :file:`C:/msys64/mingw64/bin` в переменную Path:

* откройте меню Windows,
* введите «Переменные среды»,
* откройте свойства системы и нажмите «Переменные среды»,
* дважды щёлкните по «Path» в системных переменных,
* и добавьте :file:`C:/msys64/mingw64/bin` к каталогам, перечисленным в переменной «Path».

Так Qt Creator и Polyphone смогут найти компилятор, инструменты и библиотеки MSYS2.


## Сборка


### Исходный код

Сначала скачайте исходный код Polyphone <a href="software" target="_blank">здесь</a> или с <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> и распакуйте файлы, например, на рабочий стол Windows.

### Сборка в Qt Creator

Теперь продолжите по руководству по сборке Polyphone в Qt Creator: [Сборка Polyphone в Qt Creator](development/using-qt-creator-to-build-polyphone.md).
Используемый комплект (kit) должен содержать компилятор MSYS2 (:file:`C:\msys64\mingw64\bin\g++.exe`), который находит библиотеки, установленные с помощью MSYS2, и версию Qt для MinGW 64-bit.

### Сборка из командной строки

Именно этим способом собираются Windows-версии Polyphone.
Откройте терминал :program:`MSYS2 MINGW64`, перейдите в корень проекта (каталог, содержащий :file:`sources`) и выполните следующие команды (предварительно поправьте путь к Qt):

```
export PATH="/mingw64/bin:/c/Qt/6.6.2/mingw_64/bin:$PATH"
cmake -S sources -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qt/6.6.2/mingw_64
cmake --build build --parallel 4
```

Если компиляция прошла успешно, в каталоге :file:`build/bin` появится исполняемый файл :file:`polyphone.exe`.


### Запуск Polyphone

Чтобы исполняемый файл мог работать самостоятельно, в его папку нужно добавить библиотеки Qt.
Выполните в терминале следующую команду (предварительно поправьте пути):

```
C:\Qt\6.6.2\mingw_64\bin\windeployqt.exe --release "C:\Users\User\Desktop\polyphone\build\bin\polyphone.exe"
```

Появятся библиотеки Qt (например, :file:`Qt6Core`), и двойной щелчок по файлу :file:`polyphone.exe` запустит программу.
Чтобы Polyphone можно было запустить на другом компьютере с Windows, в папку нужно также добавить следующие файлы из :file:`C:\msys64\mingw64\bin`:

* libgcc_s_seh-1.dll
* libstdc++-6.dll
* libwinpthread-1.dll
* libssp-0.dll
* libsndfile-1.dll
* libFLAC.dll
* libogg-0.dll
* libvorbis-0.dll
* libvorbisenc-2.dll
* libvorbisfile-3.dll
* libopus-0.dll
* libmp3lame-0.dll
* libmpg123-0.dll
* libcrypto-3-x64.dll
* libssl-3-x64.dll
* zlib1.dll


## Устранение неполадок


При необходимости помощь можно получить в этой [категории](forum/support-bug-reports) форума.
