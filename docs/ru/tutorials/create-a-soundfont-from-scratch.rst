.. _create a soundfont from scratch:

Как создать SoundFont с нуля
============================

To create a :ref:`soundfont <sf2 format>`, click on :guilabel:`Create a soundfont` in the :ref:`home screen <left part>`.
Soundfonts are made from a 3-level structure, so the creation of a new soundfont involves three fundamental steps:

* `sample preparation`_,
* `instrument creation`_, comprising samples,
* `preset creation`_, comprising instruments.


.. _sample preparation:

Подготовка семплов
------------------


Загрузка семплов
^^^^^^^^^^^^^^^^

The sample preparation begins by **loading** .wav files.
To do this first select the :guilabel:`Samples` category in the :ref:`tree <tree>` and select :guilabel:`Import samples` from the :ref:`toolbar <toolbar edit>`.
Samples may be obtained:

* through Internet,
* by recording a real musical instrument,
* by synthesis using a specialized software.


Закольцовка семплов
^^^^^^^^^^^^^^^^^^^

Next, a **loop** may be required in the sample, if you would like it to sound longer than its normal length.
An example of this would be if you only have a 1 second flute sample, but would like it to sound indefinitely.
This can be done by manually or automatically assigning loop points (start + end) within the sample boundaries, so that the loop area can be repeated and thus make the sample play longer.
While you can assign loop points manually, using the “:ref:`sample tool autoloop`“ function to assign them automatically usually produces better results and is a lot faster.

To assign loop points manually:

#. click in the :ref:`tree <tree>` on the sample to be looped,
#. in the :ref:`sample editor <sample editor>`, arbitrarily position the start and end of the loop in the :ref:`graphic <sample editor graph>` (WAV display area) unless a loop is already defined.
   Use left-click for start and right-click for end.
   The right loop point should be entered first (since the left point it by default at the position 0 and that it's not possible to assign a right point before the left point).
#. press the :guilabel:`Play` button after having selected the loop function,
#. adjust the start and / or end of the loop, while the sample is playing, until the transition between the two positions is as smooth as possible.

.. note::
   To hear looping in an instrument, you must enter select |loop on| in the :guilabel:`Loop playback` parameter row of either the Global or individual notes columns in the :ref:`Parameters Table <instrument editor table>`.

   |loop on| turns it On, |loop off| or a blank turns it Off.


.. figure:: images/loop_illustration.png

   Loop illustration


Подстройка семплов
^^^^^^^^^^^^^^^^^^

Finally, a **tuning** has to be done.
To do this, a calibration tool (Sinus) is available in the editing page of :ref:`samples <sample editor player>`.
For each sample the method is as follows:

#. begin the playback (press the :guilabel:`play` button), if possible with loop function selected,
#. select the sinus function,
#. adjust the volume slider to hear the two sounds as clearly as possible,
#. change the Root key until the two sounds most closely match,
#. adjust the Correction (cents) to tune the sample to the Rootkey.
   To do this, pay attention to any beats that occur and make sure they are as slow as possible.

The :ref:`frequency <sample editor frequency>` analysis can be a good indicator to start tuning.


Другие возможности
^^^^^^^^^^^^^^^^^^

Several tools are available to edit samples like:

* :ref:`removing blank <sample tool removeblank>` at start,
* :ref:`equalizing <sample editor equalizer>` the sound,
* :ref:`transposing <sample tool transpose>`,
* :ref:`normalizing <sample tool volume>` the volume,
* :ref:`adjusting the balance <sample tool balance>` for stereo samples.


.. _instrument creation:

Создание инструмента
--------------------

For this step, it is assumed that one or more samples are available for the creation of an instrument.


Создание инструмента и добавление семплов
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, click on :guilabel:`New instrument` in the :ref:`toolbar <toolbar edit>`.
A name must now be entered.

Then add samples to the instrument using a “:ref:`drag & drop <tree dragdrop>`”.
In the tree you will notice that divisions appear in the instrument.

.. note::
   When samples are created the divisions are not copied but linked.
   There are no constraints on the number of divisions.


Предназначение разделов
^^^^^^^^^^^^^^^^^^^^^^^

When samples are added to instruments, they appear as divisions (columns) in the :ref:`instrument editor table <instrument editor table>`.
Each division must then be positioned on the keyboard by changing the “Key range” in the table.
Generally, it is a good practice to have the range of a division include the root key of its represented sample.
The entire surface of the keyboard must be covered (eg, key 36 to key 96 for a classic synthesizer keyboard).

The tool “:ref:`instrument tool position`” automatically distributes the samples over the keyboard.


Настройка разделов
^^^^^^^^^^^^^^^^^^

If the sample(s) are looped, to hear the looping in an instrument, select |loop on| in the Loop playback parameter row of either the Global or individual notes columns in the Parameters Table.

* |loop on| turns it On,
* |loop off| or a blank turns it Off.
* |loop on and end| turns the looping On and the rest of the sample is read after the key is released.

The global division, as its name suggests, allows you to enter a parameter for the entire instrument.
A global parameter applies only to the divisions whose same parameter is not specified.
This means that the parameters of the standard divisions take precedence over the settings of the global division.

In the context of this simple tutorial, no other parameters have to be edited for the instrument to work.
It should be already possible to play with the :ref:`virtual keyboard <toolbar keyboard>`.

However, to improve the instrument it is advisable to enter a value in the “Vol env release (s)” row, which prevents the sound from stopping abruptly, as soon as a key is released.
The other parameters are described in the description of :ref:`tables <instrument editor table>`.


.. _preset creation:

Создание пресетов
-----------------

For this step, it is assumed that one or more instruments are available for the creation of a preset.


Что такое пресет?
^^^^^^^^^^^^^^^^^

A preset is the equivalent of a sound patch name found in hardware synths.
It is visible from outside a soundfont and is identified by a bank number and a preset number.
A soundfont can have one or many presets.
Each preset contains one or more instruments, just as instruments contain one or more samples.


Создание пресета и добавление инструментов
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, click on :guilabel:`New preset` in the :ref:`toolbar <toolbar edit>`.
A name must now be entered.

Then add instruments to the preset using a “:ref:`drag & drop <tree dragdrop>`”.
In the tree you will notice that one or more divisions appear in the preset.
Often there may only be one, as in the case of a preset containing a single intrument.

It is possible to create as many or more presets as instruments, and each preset can contain one or more instruments.


Настройка пресета
^^^^^^^^^^^^^^^^^

Changing the preset parameters may not be required since:

* when creating a preset, the bank number and the preset number are automatically assigned,
* when adding an instrument to a preset the key range is automatically calculated according to the instrument.

For the scope of this tutorial, the creation of a soundfont is finished!


.. inline images:

.. |loop on|         image:: images/loop_on.png
.. |loop off|        image:: images/loop_off.png
.. |loop on and end| image:: images/loop_on_end.png
