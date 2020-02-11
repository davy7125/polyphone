# Polyphone Soundfont Editor Documentation

Here are the source files, written with the markdown syntax, of the [Polyphone documentation](https://www.polyphone-soundfonts.com/documentation). Feel free to improve everything you want:
* edit an existing page,
* create another page,
* add a translation.

After the pull-request is validated, the documentation of the website will be updated.


## Structure


### Root

The root folder comprises this README file and one folder per language: "en" for English and "ru" for Russian for example.

### Language folder

Inside the language folder, you will find a file "config.txt", one folder per documentation category and the folder "images" that contains all images used in the documentation pages, in the .png format.

The file "config.txt" comprises pairs {key}={value} which are the translations of the word "Documentation" and of the name of the language in the related language. For example:

```
documentation_name=Documentation
language_name=English
```

### Category folder

A category folder comprises at least these two special files:

* **category_description.txt**: description of the category,
* **toc.txt**: table of content listing all pages within the category, representing a tree.

```
index.md=User manual
|soundfont-editor/index.md=Soundfont editor
||soundfont-editor/tree.md=Tree
||soundfont-editor/toolbar.md=Toolbar
(...)
|menu.md=Menu
|annexes/index.md=Annexes
||annexes/the-different-soundfont-formats.md=The different soundfont formats
||annexes/command-line.md=Command line
```

All the other files in the category contain the documentation and can be in sub-folders.

## Language

The MarkDown-Extra language is used for writing the documentation. It's been extended with the following features:

* a backslash ``\`` at the end of a line insert a new line ``<br>``,
* ``![](XXX.mp3)`` displays an audio player,
* writing ``-->`` results in a right arrow ``‣``,
* ``<span class="{CLASSNAME}">{CONTENT}</span>`` can be inserted with the syntax ``:{CLASSNAME}:`{CONTENT}` ``, the useful classnames being:
  * **kbd** for representing a key on the keyboard,
  * **guilabel** and **menuselection** for mentioning a graphical element on the software interface,
  * **file** for writing a folder or file name,
  * **program** for a program name,
  * **command** for a command line.
