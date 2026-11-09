Polyphone intègre un serveur suivant le Model Context Protocol (MCP), un standard qui permet aux assistants IA d'utiliser d'autres logiciels.
Une fois ce serveur activé, un assistant comme Claude peut travailler sur les banques de sons ouvertes dans Polyphone&nbsp;: vous décrivez ce que vous voulez avec vos propres mots, et l'assistant parcourt les banques de sons, les modifie, utilise les outils de Polyphone et mesure le résultat.

Chaque modification apparaît immédiatement dans l'éditeur et peut être annulée, comme si vous l'aviez faite vous-même.
Ce tutoriel montre comment connecter Claude à Polyphone, puis donne des exemples de demandes, avec des fichiers audio pour entendre les résultats.


## Ce que l'assistant peut faire {#doc_principles}


L'assistant a accès à tout ce qui est ouvert dans Polyphone&nbsp;:

* **navigation**&nbsp;: les banques de sons, leurs échantillons, instruments et presets, les paramètres de chaque division, ainsi que les éléments sélectionnés dans l'[arborescence](manual/soundfont-editor/tree.md) (de sorte que «&nbsp;cet échantillon&nbsp;» ou «&nbsp;les instruments sélectionnés&nbsp;» soient compris),
* **édition**&nbsp;: créer, renommer, dupliquer ou supprimer des éléments, modifier les paramètres, les étendues de notes et de vélocités, les points de boucle, les modulateurs…
* **outils**&nbsp;: la plupart des outils de la [boîte à outils](manual/soundfont-editor/tools/index.md), comme le bouclage automatique, la modification du volume, l'auto-positionnement des échantillons ou la relâche naturelle,
* **analyse du son**&nbsp;: mesures du volume perçu, de la hauteur, des harmoniques, de l'attaque, de la qualité des boucles… et rendu d'une note jouée par le synthétiseur.

Les fonctions d'analyse sont essentielles&nbsp;: **l'assistant ne peut pas entendre**.
À la place, il mesure le son avant et après une modification, comme un ingénieur lirait un analyseur de spectre.
C'est ainsi qu'il peut vérifier qu'une boucle ne claque pas, que les notes sont justes ou que le volume est régulier sur tout le clavier — mais vos oreilles restent juges en dernier ressort.

Chaque modification faite par l'assistant est une étape dans l'historique de la banque de sons&nbsp;: :kbd:`Ctrl`+:kbd:`Z` dans Polyphone l'annule.
Rien n'est écrit sur le disque tant que vous ne demandez pas à l'assistant d'enregistrer la banque de sons, ou que vous ne l'enregistrez pas vous-même.


## Mise en place {#doc_setup}


### Activer le serveur dans Polyphone


Ouvrez les [préférences](manual/settings.md#doc_ai_assistant) et allez dans la section :guilabel:`Assistant IA`.
Cochez :guilabel:`Autoriser les assistants IA à accéder aux banques de sons ouvertes`&nbsp;: la ligne en dessous affiche alors l'adresse du serveur, http://127.0.0.1:8642/mcp par défaut.
Si le port 8642 est déjà utilisé par un autre logiciel, choisissez-en un autre avec :guilabel:`Port`.

Le serveur n'accepte que les connexions venant de votre propre ordinateur.
La même page liste toutes les fonctions proposées à l'assistant, regroupées en quatre catégories&nbsp;: :guilabel:`Navigation`, :guilabel:`Édition`, :guilabel:`Outils` et :guilabel:`Analyse du son`.


### Connecter Claude Code


<a href="https://claude.com/claude-code" target="_blank">Claude Code</a> est un assistant conçu par Anthropic qui fonctionne dans un terminal (il existe aussi en extension pour les éditeurs VS Code et JetBrains).
Une fois qu'il est installé, copiez la commande affichée dans les préférences de Polyphone avec le bouton :guilabel:`Copier`, et lancez-la dans un terminal&nbsp;:

```
claude mcp add --transport http polyphone http://127.0.0.1:8642/mcp
```

Par défaut, Claude Code n'utilise le serveur que lorsqu'il est lancé depuis le dossier où cette commande a été exécutée.
Pour utiliser Polyphone depuis n'importe quel dossier, ajoutez l'option `--scope user`&nbsp;:

```
claude mcp add --scope user --transport http polyphone http://127.0.0.1:8642/mcp
```

Démarrez ensuite une nouvelle session de Claude Code&nbsp;: les fonctions de Polyphone sont chargées au démarrage d'une session, et Polyphone doit déjà être ouvert avec le serveur activé.
La commande `/mcp` dans Claude Code indique si le serveur «&nbsp;polyphone&nbsp;» est connecté.
La première fois que l'assistant veut utiliser une fonction de Polyphone, Claude Code vous demande l'autorisation, et vous pouvez choisir de ne plus être sollicité pour cette fonction.

**Note&nbsp;:** d'autres assistants compatibles avec MCP par le transport HTTP peuvent se connecter à la même adresse.


### Premières questions


Commencez par des questions simples pour voir comment l'assistant explore les banques de sons&nbsp;:

* *«&nbsp;Quelles banques de sons sont ouvertes dans Polyphone&nbsp;?&nbsp;»*
* *«&nbsp;Que contient cette banque de sons&nbsp;? Certains échantillons ne sont-ils utilisés par aucun instrument&nbsp;?&nbsp;»*
* *«&nbsp;Donne-moi la liste des presets avec leurs numéros de banque et de preset.&nbsp;»*

Lorsque vous sélectionnez un élément dans l'arborescence de Polyphone, l'assistant peut s'y référer&nbsp;: *«&nbsp;Baisse de 6 dB le volume de l'échantillon sélectionné&nbsp;»* ou *«&nbsp;Renomme ces instruments avec le préfixe “Orgue”&nbsp;»*.
Pour les petites modifications, le résultat est immédiat et visible dans l'éditeur.


## Exemple&nbsp;: vérifier et corriger un instrument {#doc_check}


Les erreurs sont vite arrivées lorsqu'on construit un instrument avec de nombreux échantillons, et certaines sont difficiles à remarquer&nbsp;: une note légèrement fausse, un échantillon un peu plus fort que ses voisins, une boucle qui ne claque qu'au bout de quelques secondes…
L'assistant peut vérifier un instrument entier en quelques secondes.

Pour essayer vous-même, téléchargez <a href="files/flute-with-mistakes.sf2" download>flute-with-mistakes.sf2</a> et ouvrez-le dans Polyphone.
Ce fichier contient un instrument de flûte dans lequel quelques erreurs ont été introduites volontairement.
Écoutez une gamme de Do4 à Fa6 suivie d'un long Sol6&nbsp;:

![](files/ai-assistant-flute-before.mp3)

Puis demandez&nbsp;:

*«&nbsp;Vérifie l'instrument “Flute”&nbsp;: y a-t-il des notes fausses, trop fortes ou trop faibles, des touches sans son ou des boucles mal faites&nbsp;?&nbsp;»*

L'assistant analyse l'instrument (une seule fonction mesure le volume, la hauteur, le timbre et la boucle de chaque division), puis examine en détail les échantillons suspects.
Sa réponse liste les problèmes&nbsp;:

* la touche Mi4 ne joue rien&nbsp;: la division de «&nbsp;Flute 63&nbsp;» s'arrête à Ré♯4 et la suivante commence à Fa4,
* «&nbsp;Flute 69&nbsp;» sonne un demi-ton trop haut&nbsp;: il joue un La4 (440 Hz) mais sa note de base est Sol♯4,
* «&nbsp;Flute 78&nbsp;» est 30 cents trop haut&nbsp;: sa correction est de +30 cents alors que l'enregistrement est juste,
* la division de «&nbsp;Flute 84&nbsp;» est 6 dB plus forte que les autres (touches Si5 à Do♯6),
* la boucle de «&nbsp;Flute 90&nbsp;» claque&nbsp;: les formes d'onde ne coïncident pas au point de bouclage et la longueur de la boucle n'est pas un nombre entier de périodes.

Demandez alors&nbsp;: *«&nbsp;Corrige ces problèmes.&nbsp;»*

L'assistant étend l'étendue de notes de «&nbsp;Flute 63&nbsp;» jusqu'à Mi4, règle la note de base de «&nbsp;Flute 69&nbsp;» sur La4 et la correction de «&nbsp;Flute 78&nbsp;» sur 0, donne une atténuation de 6 dB à la division de «&nbsp;Flute 84&nbsp;» (l'échantillon lui-même n'est pas modifié), et utilise l'outil [Bouclage automatique](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) sur «&nbsp;Flute 90&nbsp;».
Une nouvelle analyse confirme le résultat&nbsp;: plus de touche muette, plus de note fausse, le même volume partout et une boucle dont les formes d'onde se raccordent désormais au point de bouclage.
La même gamme après les corrections&nbsp;:

![](files/ai-assistant-flute-after.mp3)

L'assistant explique ce qu'il fait, et il doit souvent faire des choix&nbsp;: ici, il a atténué une division plutôt que de modifier les données audio de l'échantillon.
Si vous préférez une autre solution, il suffit de le dire — et vous pouvez revoir chaque modification dans l'éditeur, ou l'annuler.


## Exemple&nbsp;: créer un instrument à partir de zéro {#doc_create}


Un assistant comme Claude Code peut aussi exécuter des programmes sur votre ordinateur, par exemple pour calculer des échantillons.
Combiné aux fonctions de Polyphone, cela permet de construire un instrument entier à partir d'une description.
La flûte de l'exemple précédent a été réalisée ainsi, à partir de deux demandes&nbsp;:

*«&nbsp;Peux-tu me faire un son de flûte pour la touche 60&nbsp;?&nbsp;»*

*«&nbsp;Maintenant, fais une flûte complète avec plusieurs niveaux d'intensité, dans une nouvelle banque de sons.&nbsp;»*

Voici ce que l'assistant a fait, étape par étape&nbsp;:

1. il a mesuré une flûte enregistrée dans une banque de sons General MIDI pour connaître le timbre de l'instrument dans chaque registre (harmoniques, bruit de souffle, attaque, vibrato),
2. il a écrit un programme synthétisant 39 échantillons&nbsp;: 13 notes de Do4 à Do7, une toutes les 3 touches, chacune dans trois nuances — *p* est plus sombre et plus soufflé avec une attaque douce, *f* est plus riche en harmoniques avec une attaque plus franche,
3. il a importé les échantillons dans une nouvelle banque de sons, réglé leurs notes de base et leurs boucles, et créé un instrument par nuance avec une relâche de 0,4 s,
4. il a superposé les trois instruments dans un preset (banque 0, preset 73, la flûte de la norme General MIDI) avec les étendues de vélocités 0-49, 50-89 et 90-127, comme décrit dans le tutoriel «&nbsp;[Des instruments réalistes avec plusieurs couches de vélocités](tutorials/realistic-instruments-with-multiple-velocity-layers.md)&nbsp;»,
5. il a fait le rendu de notes des deux côtés de chaque limite de vélocité&nbsp;: le volume ne change que de 0,1 à 0,4 dB, de sorte que seul le timbre change d'une couche à la suivante.

La dernière étape a révélé une faiblesse&nbsp;: dans l'octave la plus aiguë, les trois couches avaient presque le même timbre.
L'assistant a modifié son programme pour que les notes fortes deviennent aussi plus brillantes dans l'aigu, puis a de nouveau généré les échantillons et construit l'instrument.

Écoutez la touche Do4 jouée avec les vélocités 35, 70 et 110 — une note par couche&nbsp;:

![](files/ai-assistant-flute-layers.mp3)

Et une phrase de plus en plus forte, où le timbre suit la nuance&nbsp;:

![](files/ai-assistant-flute-phrase.mp3)

Une flûte synthétisée n'égalera jamais un bon enregistrement&nbsp;: cet exemple montre la méthode plus que le son.
Avec vos propres enregistrements, le même genre de demande s'applique&nbsp;: *«&nbsp;Importe les fichiers wav de ce dossier, trouve leur note de base, boucle-les et construis un instrument avec une couche par nuance.&nbsp;»*


## Autres idées {#doc_ideas}


Voici d'autres demandes qui fonctionnent bien&nbsp;:

* *«&nbsp;Règle une relâche naturelle pour tous les instruments, plus longue dans le grave.&nbsp;»*
* *«&nbsp;Les notes aiguës de cet instrument sont trop brillantes&nbsp;: adoucis-les progressivement à partir de Do6.&nbsp;»*
* *«&nbsp;Crée un jeu de voix céleste à partir de cet instrument d'orgue, avec 3 battements par seconde au Do4.&nbsp;»*
* *«&nbsp;Convertis tous les échantillons stéréo en mono et mets à jour les instruments.&nbsp;»*
* *«&nbsp;Compare ces deux échantillons&nbsp;: pourquoi le second sonne-t-il plus terne&nbsp;?&nbsp;»*
* *«&nbsp;Explique simplement les modulateurs de ce preset.&nbsp;»*
* *«&nbsp;Supprime les échantillons inutilisés et les doublons, puis enregistre la banque de sons au format sf3.&nbsp;»*


## Conseils {#doc_tips}


* **Précisez le périmètre**&nbsp;: «&nbsp;seulement l'instrument “Piano”&nbsp;», «&nbsp;ne modifie pas les échantillons, seulement les instruments&nbsp;»…
* **Demandez des mesures**&nbsp;: «&nbsp;vérifie le résultat&nbsp;» ou «&nbsp;compare avant et après&nbsp;» amène l'assistant à contrôler son travail avec les fonctions d'analyse.
* **Avancez pas à pas** pour les modifications importantes, et écoutez entre les étapes.
* **Gardez une copie** de votre banque de sons, et n'enregistrez que lorsque vous êtes satisfait&nbsp;: demandez à l'assistant d'enregistrer, ou utilisez :kbd:`Ctrl`+:kbd:`S`.
* **Les grandes banques de sons** sont plus longues à analyser&nbsp;: ciblez un instrument ou un preset plutôt que la banque de sons entière.
* **Confidentialité**&nbsp;: l'assistant reçoit les noms, les paramètres, les mesures et des images de formes d'onde, pas l'audio lui-même&nbsp;; mais ces données sont envoyées au service de l'assistant, comme le reste de la conversation.
