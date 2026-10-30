Les préférences du logiciel sont accessibles soit depuis l'[écran d'accueil](manual/index.md), soit depuis le [menu](manual/menu.md#doc_shortcuts) (raccourci clavier :kbd:`Ctrl`+:kbd:`,`).
Elles sont divisées en cinq sections&nbsp;:

* [Général](#doc_general)
* [Interface](#doc_interface)
* [Son](#doc_sound)
* [Clavier](#doc_keyboard)
* [Banques de sons en ligne](#doc_repository)

Toutes les modifications sont enregistrées immédiatement.


## Général {#doc_general}


La section :guilabel:`Général` permet la modification des paramètres suivants&nbsp;:

Sortie audio
: Spécifie le système audio et le périphérique utilisés pour jouer les sons.
  Les systèmes disponibles dépendent du système d'exploitation (par exemple WASAPI et ASIO sous Windows, ALSA et JACK sous Linux, Core Audio sous macOS).
  Le choix «&nbsp;-&nbsp;» désactive la sortie audio.

Taille du tampon
: Spécifie la taille du tampon audio.
  Une faible valeur induit une faible latence mais le son risque de craquer selon la performance de l'ordinateur.
  Avec la valeur :guilabel:`Défaut`, la taille dépend du système audio.

Polyphonie
: Nombre maximal de voix jouées simultanément, de 64 à 4096 (1024 par défaut).
  Une voix est un échantillon en cours de lecture&nbsp;: une note peut utiliser plusieurs voix, deux avec un échantillon stéréo par exemple.
  Au-delà de ce nombre, des notes s'estompent&nbsp;: d'abord celles qui sont relâchées, puis les plus faibles.
  Les notes qui viennent de commencer, ainsi que la note la plus grave et la note la plus aiguë parmi celles qui sont tenues, sont conservées le plus longtemps possible.

Charge du processeur
: Part maximale du temps processeur utilisée pour calculer les voix, de 20 à 100&nbsp;% (60&nbsp;% par défaut).
  Au-delà, des notes s'estompent de la même manière.
  Une valeur élevée permet de jouer plus de notes simultanément, mais le son risque de craquer.

Entrée MIDI
: Spécifie l'entrée MIDI qui contrôle le synthétiseur et le clavier virtuel.

Par ailleurs, il est possible de cocher les options suivantes&nbsp;:

Édition stéréo&nbsp;: répercuter les modifications
: Lors de l'édition d'un côté d'un échantillon stéréo, l'autre côté est automatiquement édité de la même manière.
  L'option s'applique également lors de l'édition de divisions stéréo d'un instrument&nbsp;: la division correspondant à l'autre canal (et ayant les mêmes étendues de notes et de vélocités) est éditée de la même manière.

Import d'échantillons&nbsp;: ajuster à la boucle
: Lors de l'importation d'un échantillon, les données situées après la fin de la boucle sont automatiquement enlevées (même effet que l'outil «&nbsp;[Ajuster à la fin de boucle](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)&nbsp;»).

Import d'échantillons&nbsp;: enlever le blanc au départ
: Lors de l'importation d'un échantillon, la partie silencieuse située avant l'attaque est automatiquement enlevée (même effet que l'outil «&nbsp;[Enlever le silence initial](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)&nbsp;»).


![Préférences, général](images/settings_general.png "Préférences, général")


## Interface {#doc_interface}


Cette section présente des options concernant l'interface du logiciel&nbsp;:

Langue
: La langue peut être changée ici.

Nom des notes
: Les notes peuvent être nommées selon leur numéro (en accord avec la spécification MIDI), ou selon le nom du Do central (Do3, Do4 ou Do5).
  Le nom des notes peut contenir des dièses ou des bémols.

Instance unique
: Ouvre tous les fichiers dans une seule et même fenêtre.

Vue de démarrage
: Sélectionne la vue à afficher à l'ouverture d'une banque de sons parmi&nbsp;: :guilabel:`informations générales` (par défaut), :guilabel:`liste des échantillons`, :guilabel:`premier échantillon`, :guilabel:`liste des instruments`, :guilabel:`premier instrument`, :guilabel:`liste des ensembles`, :guilabel:`premier ensemble`.

Ordonner les divisions
: L'ordre des divisions au sein d'un instrument ou d'un ensemble peut être changé ici&nbsp;: :guilabel:`par étendue de notes`, :guilabel:`par étendues de vélocités`, :guilabel:`alphabétiquement` ou :guilabel:`non` (ordre de création).

Style
: Le style des éléments de l'interface.
  Le style :guilabel:`Fusion` est celui par défaut et d'autres peuvent être proposés selon le système d'exploitation (Adwaita, Windows, macOS…).

Couleurs
: Différents jeux de couleurs prédéfinis sont disponibles pour personnaliser l'apparence de Polyphone.
  Une sélection manuelle des couleurs du texte, de la fenêtre, des boutons, de la sélection et des listes est aussi possible.
  Les couleurs ne peuvent être modifiées qu'avec les styles :guilabel:`Fusion` et :guilabel:`Windows`.

Bordures de fenêtre
: Affiche ou masque les bordures de fenêtre pour gagner de la place sur l'écran (non disponible sous macOS).

Décorations
: Les décorations en arrière-plan de certaines listes peuvent être désactivées pour augmenter la lisibilité.

Une modification de la langue, du nom des notes, du style, des couleurs, des bordures de fenêtre ou des décorations est appliquée au prochain démarrage du logiciel.


![Préférences, interface](images/settings_interface.png "Préférences, interface")


## Son {#doc_sound}


La section :guilabel:`Son` permet de configurer la réverbération (niveau, profondeur, atténuation et densité) et le chorus (niveau, amplitude et fréquence) du synthétiseur, ainsi que son volume général.
Le volume général peut aussi être modifié avec le contrôleur MIDI du volume si un clavier externe est connecté.

Le modulateur par défaut modifiant la fréquence de coupure du filtre ayant été modifié entre les versions 2.01 et 2.04 des spécifications sf2, il est possible de définir ici quelle version utiliser&nbsp;:

* :guilabel:`SoundFont 2.01`&nbsp;: utilisation de la règle «&nbsp;abaisser la fréquence du filtre en tenant compte de la vélocité SI la vélocité est inférieure à 64&nbsp;» (on entend ici un écart net et problématique entre les vélocités 63 et 64)&nbsp;;
* :guilabel:`SoundFont 2.04` (par défaut)&nbsp;: utilisation de la règle «&nbsp;abaisser la fréquence du filtre en tenant compte de la vélocité&nbsp;» (plus d'écart)&nbsp;;
* :guilabel:`désactivé`&nbsp;: ce modulateur par défaut n'est pas défini.


![Préférences, son](images/settings_sound.png "Préférences, son")


## Clavier {#doc_keyboard}


Les touches du clavier de l'ordinateur assignées pour contrôler le [clavier virtuel](manual/soundfont-editor/toolbar.md#doc_keyboard) sont définies dans la section :guilabel:`Clavier`.
Les hauteurs de notes peuvent être augmentées ou diminuées octave par octave grâce au réglage :guilabel:`Premier Do`.

Pour les musiciens plus avancés, l'accordage peut être modifié ici&nbsp;:

Diapason
: La fréquence du La situé au-dessus du Do central (note 69), entre 390 et 470 Hz (440 Hz par défaut).

Tempérament
: Un tempérament autre que le tempérament égal peut être appliqué lors du jeu.
  Lorsqu'un tempérament est sélectionné, la note de référence de ce tempérament (sa tonique) peut aussi être choisie.

Le diapason et le tempérament peuvent également être modifiés par des messages MIDI RPN (accord fin général, sélection du programme d'accordage et sélection de la banque d'accordage).


![Préférences, clavier](images/settings_keyboard.png "Préférences, clavier")


## Banques de sons en ligne {#doc_repository}


L'adresse mail et le mot de passe d'un compte peuvent être entrés ici pour lier Polyphone à ce compte, un [compte Premium](premium) permettant de débloquer l'ensemble du [dépôt en ligne](manual/online-repository.md).
Après une connexion réussie, Polyphone se connecte automatiquement au compte à chaque démarrage, jusqu'à l'utilisation du bouton :guilabel:`Se déconnecter`.

Le répertoire utilisé pour les téléchargements de banques de sons depuis le [dépôt](manual/online-repository.md) peut aussi être changé ici.


![Préférences, banques de sons en ligne](images/settings_repository.png "Préférences, banques de sons en ligne")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface