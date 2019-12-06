.. _command line:

Командная строка
================

Polyphone can be run with a command line to convert soundfonts in the format `sf2 <convert to sf2_>`_, `sf3 <convert to sf3_>`_ or `sfz <convert to sfz_>`_.

Supported file formats for the conversion are :ref:`sf2 <sf2 format>`, :ref:`sf3 <sf3 format>`, :ref:`sfz <sfz format>` and :ref:`sfArk <sfark format>`.


.. _convert to sf2:

Конвертация в sf2
-----------------


Командная строка
^^^^^^^^^^^^^^^^

::

   polyphone -1 -i [file/to/convert] -d [output/directory] -o [output/file/name]

.. note::
   With Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


Аргументы
^^^^^^^^^

-1  Mode “1” is “convert to sf2”.
-d  Output directory in which the input file will be converted.
    This argument is optional, by default this is the same directory than the input file.
-o  Output name of the converted file without the extension (the extension “.sf2” will be automatically added).
    This argument is optional, by default this is the same name than the input file.


Пример
^^^^^^

::

   polyphone -1 -i /path/to/file.sfArk


.. _convert to sf3:

Конвертация в sf3
-----------------


Командная строка
^^^^^^^^^^^^^^^^

::

   polyphone -2 -i [file/to/convert] -d [output/directory] -o [output/file/name] -c [config]

.. note::
   With Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


Аргументы
^^^^^^^^^

-2  Mode “2” is “convert to sf3”.
-d  Output directory in which the input file will be converted.
    This argument is optional, by default this is the same directory than the input file.
-o  Output name of the converted file without the extension (the extension “.sf3” will be automatically added).
    This argument is optional, by default this is the same name than the input file.
-c  Conversion configuration.
    It is possible to specify the compression quality:

    * “0” is low,
    * “1” is medium,
    * “2” is high.

    This argument is optional, by default this is “1” (medium quality).


Пример
^^^^^^

::

   polyphone -2 -i /path/to/file.sf2 -c 2


.. _convert to sfz:

Конвертация в sfz
-----------------


Командная строка
^^^^^^^^^^^^^^^^

::

   polyphone -3 -i [file/to/convert] -d [output/directory] -o [output/file/name] -c [config]

.. note::
   With Windows you need to write the full path of Polyphone instead of just :command:`polyphone`, which is for example :command:`C:/Program files/Polyphone/polyphone.exe`.


Аргументы
^^^^^^^^^

-3  Mode “3” is “convert to sfz”.
-d  Output directory in which the input file will be converted.
    This argument is optional, by default this is the same directory than the input file.
-o  Output name of the converted file without the extension (the extension “.sfz” will be automatically added).
    This argument is optional, by default this is the same name than the input file.
-c  Conversion configuration made of 3 characters.

    1. The first character is “1” if each preset must be prefixed by its preset number, “0” otherwise.
    2. The second character is “1” if a directory per bank must be created, “0” otherwise.
    3. The third character is “1” if the General MIDI classification must be used to sort presets, “0” otherwise.

    This argument is optional, by default this is “000”.


Пример
^^^^^^

::

   polyphone -3 -i /path/to/file.sf3 -c 011
