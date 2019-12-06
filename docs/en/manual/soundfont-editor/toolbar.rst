.. _toolbar:

Toolbar
=======

The toolbar, located on the top of the editor, contains icons for:

* common `editing functions        <toolbar edit_>`_,
* switching the `view              <toolbar view_>`_ in editors,
* displaying the `recorder         <toolbar recorder_>`_,
* displaying the `virtual keyboard <toolbar keyboard_>`_.


.. figure:: images/toolbar.png

   Toolbar


.. _toolbar edit:

Editing functions
-----------------

The elements in the left part of the toolbar are for common editing functions:

:New sample: Add new :ref:`samples <sample editor>` to the soundfont.
  A dialog will be displayed for selecting audio files (``.wav`` and ``.flac`` files can be imported).
:New instruments: Create a new :ref:`instrument <instrument editor>`.
:New preset: Create a new :ref:`preset <preset editor>`.
:Tools: Clicking on this button will show a menu containing all available :ref:`tools <tool menu>` for the selected element(s) in the :ref:`tree <tree>`.
:Undo: Undo the last modification.
:Redo: Redo the last undone modification.
:Save file: Save the soundfont.


.. _toolbar view:

Switch view
-----------

In the right part of the toolbar, some icons may appear depending on which :ref:`editor <editing pages>` is displayed:

* switching between the :ref:`table view <instrument editor table>`, the :ref:`range editor <instrument editor range>` and the :ref:`envelope editor <instrument editor envelope>` is possible when editing an instrument,
* switching between the table view and the range editor is possible when editing a preset,
* nothing displayed otherwise.


.. _toolbar recorder:

Recorder
--------

When clicking on the icon, a window opens showing the recorder that can record the output of Polyphone.


.. figure:: images/recorder.png

   Recorder


The left button can have two possible states:

:Record: Start a new record if the acquisition is off.
  It is possible here to specify the file name in which the audio signal is to be recorded.
:Stop: Stop the current acquisition.

The right button can also have two possible states (only accessible if the acquisition is on):

:Pause: Pause the current acquisition.
:Resume: Resume the current acquisition.


.. _toolbar keyboard:

Virtual keyboard
----------------

When clicking on the icon, a window opens showing the virtual keyboard that has been made for testing samples, instruments and presets.
This window triggers :abbr:`MIDI (Musical Instrument Digital Interface)` signals:

* **note on** / **note off** / **after touch** with the piano keyboard,
* other :abbr:`MIDI` signals if the **MIDI controller part** is expanded.


.. figure:: images/virtual_keyboard.png

   Virtual keyboard


Piano keyboard
^^^^^^^^^^^^^^

The piano keyboard triggers signals that contains key and velocity numbers.
It can be controlled in three ways:

:with the mouse: The desired key may be clicked by the mouse.
  The velocity is defined according to the vertical position of the click.
  After a click, following moves within the same key will trigger aftertouch signals.
:using the computer keyboard: The keys used are defined in the :ref:`settings <keyboard settings>`.
:using an external MIDI keyboard: The :abbr:`MIDI` input is defined in the :ref:`settings <general settings>`.
  If the piano keyboard is visible, it will show what you are playing.


Markers and colors
~~~~~~~~~~~~~~~~~~

* When an :ref:`instrument <instrument editor>` or a :ref:`preset <preset editor>` is selected, the area not covered by the divisions is **grayed**.
  It is thus possible to visualize the full range of an instrument or a preset.
* When a division of an instrument or a preset is selected, its range is displayed the same way than above.
  Moreover, its root key is identified by an **orange marker**.
* When a key is selected on the keyboard, it appears in **blue** (or the selection color selected in the software :ref:`settings <interface settings>`).
  Moreover, the range of its corresponding range(s) appears in **light blue**.
  This applies only at the instrument level.
* Middle C (key 60) is identified by a **black marker**.


Useful shortcuts
~~~~~~~~~~~~~~~~

:kbd:`Ctrl+K`
  This shortcut can put at any time the focus on the virtual keyboard.
  This allows, while editing in a table for instance, to use the computer to play instead of edit a cell.
  A glow effect on the virtual keyboard confirms that the focus is on it.

:kbd:`Ctrl+1`, :kbd:`Ctrl+2`, … :kbd:`Ctrl+8`
  These shortcuts change the octaves played with the computer keyboard.
  After the use of :kbd:`Ctrl+1`, the lowest octave can be played.
  The highest octave is reached using :kbd:`Ctrl+8`.

:kbd:`→` / :kbd:`←`
  The right or left arrow will play the pitch just next or below the last key that has been played, with the same velocity.
  The association of the key :kbd:`Shift` skips keys.

:kbd:`↑` / :kbd:`↓`
  The up or down arrow will play the same pitch than the last key played, but with a stronger or weaker velocity.
  The association of the key :kbd:`Shift` skips velocities.


MIDI controllers
^^^^^^^^^^^^^^^^

When clicking on the bottom right arrow, the MIDI controller area can be displayed or hidden.
If you are playing with modulators, it is advised that you use these features to test your soundfont.


.. figure:: images/virtual_keyboard_2.png

   Virtual keyboard with MIDI controllers


This area is made of different buttons:

:Pitch wheel: The pitch wheel increases or decreases the pitch.
  It goes back automatically to the original position 0.
:Sensitivity: The sensitivity defines the modification range in semi-tones of the pitch wheel.
  By default this is 2 (one full tone).
:Pressure: The pressure is the amount of pressure on the keyboard.
  It can be compared to the aftertouch but while the aftertouch is key-specific, the pressure is a global signal for all keys.
:Knobs: 3 knobs are here so that MIDI controller values can be changed.
  For each knob you can specify a controller number and then its value.
:On / off led: This led is to set a MIDI controller value to 0 or 127 (useful for the sustain pedal for instance).
  It is like a knob but with only two possible values.
