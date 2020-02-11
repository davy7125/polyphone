Polyphone has been designed to support translations in order to increase the accessibility.
Everyone can help in this process.


## Prerequisite


### Translation file


Download the translation package corresponding to the language you want to translate from this [page][translations].
If the language is not in the list yet, use the [contact] form to ask for it (you just need to specify the new language you want).
You will quickly be provided with a new translation package.


### Qt Linguist


The only software you need to translate is :program:`Qt Linguist`: a tool for generating translation files.
This software can be found [here][get-qt-linguist] for Windows users or in repositories for Linux users (via :program:`Synaptic`, :program:`Yum`, …).


## Opening the translation file


Unzip the translation package you just downloaded.
You will find a .ts file :file:`polyphone_XX.ts`, :file:`XX` being related to your language.
With :program:`Qt Linguist`, open :file:`polyphone_XX.ts` and you will be able to start the translation.


## Translating


Each item in each context needs to be translated.
All you have to do is to review each element one by one and fill your translation.
Three cases may appear depending on your confidence level.

* If you don't know how to translate don't hesitate to skip the element.
  This is not an issue: other people can complete it.
* You think you have the right translation but you are unsure: write your proposition and then use :kbd:`Ctrl`+:kbd:`J`.
  The item will not be validated but the translation will nevertheless be used in the translated interface.
  It just tells other people improving the translation that you were unsure.
* You have the right translation: write it and use :kbd:`Ctrl`+:kbd:`Enter`.
  This item will be validated.

A translation needs attention (in a world where everything is being made for [soliciting it][article]).
Please take care of:

* the punctuation,
* the first and final spaces,
* the character case (lowercase / uppercase),
* singular and plural forms.

A final advice: **take your time!**
And don't forget to save your work with :kbd:`Ctrl`+:kbd:`S`.


## Testing your work


At anytime you can check your work.
In :program:`Qt Linguist` go to :menuselection:`File --> Release As…` and create a .qm file corresponding to the translation you are providing.
Follow the instructions in the translation package to install it (:file:`readme.txt` file), then open Polyphone.
Go to the [configurations][settings-interface], select your translation and restart Polyphone.
You will now be able to review your translation.

<!-- note begin -->
**Note:** the operation :guilabel:`Release As…` may translate only text comprising no accent. If this bug happens, open a terminal and use the following command: :command:`lrelease polyphone_XX.ts` (adapt it to your current file).
<!-- note end -->


## Submitting the translation


Once you are done (or bothered!), send the .ts file (not .qm!) you were editing to <contact@polyphone-soundfonts.com>.
The translation package of this [page][translations] will be updated.


[article]:         https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3 {target=_blank rel=noopener}
[get-qt-linguist]: https://github.com/thurask/Qt-Linguist/releases {target=_blank rel=noopener}

[contact]:            contact
[settings-interface]: manual/settings.md#doc_interface
[translations]:       download/translations
