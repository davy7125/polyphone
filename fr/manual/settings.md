Les préférences du logiciel sont accessibles soit depuis l'[écran d'accueil](manual/index.md) soit depuis le [menu](manual/menu.md#doc_shortcuts).
Elles sont divisées en cinq sections&nbsp;:

* [Général](#doc_general)
* [Interface](#doc_interface)
* [Son](#doc_sound)
* [Clavier virtuel](#doc_keyboard)
* [Banques de sons en ligne](#doc_repository)


## Général {#doc_general}


La section :guilabel:`Général` permet la modification des paramètres suivants&nbsp;:

Sortie audio
: Spécifie la sortie audio utilisée pour lire les échantillons.
  Les pilotes JACK et ASIO peuvent être choisis (le second pour Windows seulement).

Taille du tampon
: Spécifie la taille du tampon utilisé pour les pilotes JACK et ASIO.
  Une faible valeur induit une faible latence mais le son risque de craquer selon la performance de l'ordinateur.

Entrée MIDI
: Spécifie l'entrée MIDI qui contrôle le clavier virtuel.

Par ailleurs, il est possible de cocher les options suivantes&nbsp;:

Édition stéréo&nbsp;: répercuter les modifications
: Lors de l'édition d'un côté d'un échantillon stéréo, l'autre côté est automatiquement édité de la même manière.
  L'option s'applique également lors de l'édition de divisions stéréo d'un instrument&nbsp;: la division correspondant à l'autre canal (et ayant les mêmes étendues de notes et de vélocités) est éditée de la même manière.

Import d'échantillons&nbsp;: ajuster à la boucle
: Lors de l'import d'un échantillon, les données situées après la fin de la boucle sont automatiquement enlevées (même effet que l'outil «&nbsp;[Ajuster à la fin de boucle](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)&nbsp;»).

Import d'échantillons&nbsp;: enlever le blanc au départ
: Lors de l'import d'un échantillon, la partie silencieuse située avant l'attaque est automatiquement enlevée (même effet que l'outil «&nbsp;[Enlever le silence initial](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)&nbsp;»).


![Préférences, général](images/settings_general.png "Préférences, général")


## Interface {#doc_interface}


Cette section présente des options concernant l'interface du logiciel&nbsp;:

Langue
: La langue peut être changée ici (un redémarrage du logiciel est requis).

Nom des notes
: Les notes peuvent être nommées selon leur numéro (en accord avec la spécification MIDI), ou selon le nom du Do central (Do3, Do4 ou Do5).
  Le nom des notes peut contenir des dièses ou des bémols.

Ordonner les divisions
: L'ordre des divisions au sein d'un instrument ou d'un ensemble peut être changé ici.

Instance unique
: Ouvre tous les fichiers dans une seule et même fenêtre.

Thème
: Différents styles et jeux de couleurs prédéfinis sont disponibles pour personnaliser l'apparence de Polyphone.
  Une sélection manuelle des couleurs est aussi possible.

Bordures de fenêtre
: Affiche ou masque les bordures de fenêtre pour gagner de la place sur l'écran.

Décorations
: Les décorations en arrière-plan de certaines listes peuvent être désactivées pour augmenter la lisibilité.


![Préférences, interface](images/settings_interface.png "Préférences, interface")


## Son {#doc_sound}


La section :guilabel:`Son` permet de configurer le chorus et la réverbération lors du rendu du son, ainsi que son volume général.
Le volume général peut aussi être modifié avec le contrôleur MIDI du volume si un clavier externe est connecté.

Le modulateur par défaut modifiant la fréquence du filtre ayant été modifié entre les versions 2.01 et 2.04 des spécifications sf2, il est possible de définir ici quelle version utiliser&nbsp;:

* 2.01&nbsp;: utilisation de la règle «&nbsp;abaisser la fréquence du filtre en tenant compte de la vélocité SI la vélocité est inférieure à 64&nbsp;» (on entend ici un écart net et problématique entre les vélocités 63 et 64)&nbsp;;
2.04&nbsp;: utilisation de la règle «&nbsp;abaisser la fréquence du filtre en tenant compte de la vélocité&nbsp;» (plus d'écart)&nbsp;;
* désactivé&nbsp;: ce modulateur par défaut n'est pas défini.


![Préférences, son](images/settings_sound.png "Préférences, son")


## Clavier virtuel {#doc_keyboard}


Les touches du clavier de l'ordinateur assignées pour contrôler le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard) sont définies dans la section :guilabel:`Clavier virtuel`.
Les hauteurs de notes peuvent être augmentées ou diminuées octave par octave.

Pour les musiciens plus avancés, le diapason et le tempérament peuvent être réglés ici.


![Préférences, keyboard](images/settings_keyboard.png "Préférences, keyboard")


## Banques de sons en ligne {#doc_repository}


Les identifiants de connexion peuvent être entrés pour lier Polyphone à un compte, un compte Premium permettant de débloquer l'[explorateur de banques de sons](manual/soundfont-browser.md).

Le répertoire de téléchargement utilisé par l'[explorateur](manual/soundfont-browser.md) peut être changé ici.


![Préférences, banques de sons en ligne](images/settings_repository.png "Préférences, banques de sons en ligne")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface