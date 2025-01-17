Polyphone n'est pas qu'un éditeur de soundfonts, c'est aussi un synthétiseur complet permettant de jouer avec un clavier MIDI.
Ce tutoriel indique comment lancer Polyphone en mode «&nbsp;synthé&nbsp;».

## Prérequis

Tout d'abord il vous faut un **clavier maître** ou tout autre contrôleur MIDI, que vous relierez à l'ordinateur par l'intermédiaire d'un **adaptateur MIDI vers USB**.

Téléchargez ensuite la dernière version de Polyphone [ici](software), puis installez et ouvrez le logiciel. Dans les paramètres, vous devriez trouver votre appareil MIDI parmi les [entrées MIDI](manual/settings.md#doc_general). Si ce n'est pas le cas, il est probable que le pilote de l'adaptateur MIDI n'est pas installé sur l'ordinateur (résoudre ce point avant de continuer).

Toujours dans les paramètres, vérifiez que la [sortie audio](manual/settings.md#doc_general) est correcte.

## Ouvrir Polyphone dans le mode «&nbsp;synthé&nbsp;»

### Windows

Après installation de Polyphone, cliquez sur le raccourci :guilabel:`Polyphone (synth mode)` dans le menu démarrer de Windows.

### Linux

Ouvrir un terminal et écrire la commande suivante&nbsp;:

```
polyphone -s
```

### Mac

Ouvrir un terminal et écrire la commande suivante&nbsp;:

```
/Applications/Polyphone.app/Contents/MacOS/polyphone -s
```

**Note&nbsp;:** si vous avez déplacé l'application Polyphone dans un dossier autre que :file:`Applications`, il faudra adapter le chemin ci-dessus.

## Jouer d'un instrument

Pour jouer d'un instrument, il suffit d'ouvrir une banque de sons que vous pouvez télécharger depuis le [dépôt](soundfonts) par exemple, et sélectionner un instrument. Les instruments sont rangés par banque&nbsp;: il faut donc sélectionner une banque pour que la liste des instruments s'affiche en-dessous.

![Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»](images/synth_mode.png "Polyphone ouvert avec le mode «&nbsp;synthé&nbsp;»")

Après avoir sélectionné un instrument, son étendue de notes est affichée sur le clavier virtuel et il vous est possible de jouer de cet instrument avec les touches correspondantes sur votre contrôleur MIDI.

## Options

### Jouer de plusieurs instruments en même temps

Pour jouer de plusieurs instruments lorsqu'une note est appuyée sur le clavier, il suffit de mettre l'option :guilabel:`Sélection multiple` à :guilabel:`on`. Plusieurs instruments pourront être sélectionnés dans la liste des instruments.

### Jouer de plusieurs instruments séparément

Dans le cas où vous disposez de plusieurs contrôleurs MIDI raccordés à l'ordinateur, il est possible d'ouvrir plusieurs banques de sons et de choisir un canal spécifique pour chacune d'elles. De cette manière, un premier clavier sera par exemple utilisé pour jouer d'un instrument et un second clavier sera utilisé pour jouer d'un autre instrument. Il est possible d'ouvrir plusieurs fois la même banque de sons pour jouer plusieurs de ses instruments simultanément.

L'assignation d'un canal se fait en modifiant la valeur de :guilabel:`canal`. Le premier onglet ouvert pourra par exemple écouter le canal 1, alors que le second pourra écouter le canal 2.

![Configuration du synthé avec 2 canaux d'écoute](images/synth_2_channels.png "Configuration du synthé avec 2 canaux d'écoute")

**Note&nbsp;:** certains claviers peuvent se diviser en deux parties et il est ainsi possible avec un seul clavier d'envoyer des notes sur 2 canaux différents.

### Changer d'instrument avec une touche

Un changement d'instrument se fait généralement grâce à un message MIDI de type «&nbsp;Program change&nbsp;» et Polyphone est capable de les lire.

Un autre mécanisme peut toutefois être utilisé&nbsp;: l'usage de touches programmées pour envoyer des messages de type «&nbsp;Note ON&nbsp;» / «&nbsp;Note OFF&nbsp;», le numéro de la note correspondant au numéro de l'instrument. Ce mécanisme peut être utile dans le cas où la banque de sons ne présente que quelques instruments et dans le cas où des touches peuvent être programmées pour envoyer des notes précises (par exemple les pads des claviers maîtres).

Si l'option :guilabel:`Sélection par note` est mise à :guilabel:`on`&nbsp;:
- un instrument est activé lorsqu'un message MIDI de type «&nbsp;Note ON&nbsp;» est reçu,
- un instrument est désactivé lorsqu'un message MIDI de type «&nbsp;Note OFF&nbsp;» est reçu.

Cette option peut être utilisée lorsque le pad d'un clavier maître est configuré pour être une bascule (une pression active la note et une autre pression désactive la note).

Si :guilabel:`basculer` est choisi&nbsp;:
- un instrument est activé lorsqu'un message MIDI de type «&nbsp;Note ON&nbsp;» est reçu,
- un instrument est désactivé lorsqu'un autre message MIDI de type «&nbsp;Note ON&nbsp;» est reçu.

La bascule se fait cette fois au niveau de Polyphone.

## Options du lancement de Polyphone

Toutes les options vues précédemment peuvent être appelées lors de l'ouverture de Polyphone en ajoutant des paramètres dans la ligne de commande. Il est aussi possible de préciser la banque de sons à ouvrir.

Pour **Linux** ou **macOS**, il suffira de compléter la ligne de commande. Pour **Windows**, il faudra créer un raccourci en faisant un clic droit sur le fichier exécutable :file:`polyphone.exe`, puis sélectionner :guilabel:`Envoyer vers le bureau (créer un raccourci)` dans les :guilabel:`Autres options` du menu contextuel. Ensuite, en affichant les propriétés de ce raccourci il est possible d'ajouter les paramètres dans la cible qui pourra par exemple être&nbsp;:

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

Les différentes options sont décrites sur cette [page](manual/annexes/command-line.md#doc_synth).

## Utilisations du synthé

Maintenant que vous savez comment transformer Polyphone en synthétiseur, pourquoi ne pas [utiliser un Raspberry Pi pour créer un instrument de musique](tutorials/create-a-synthesizer-with-raspberry-pi.md) ?

Le mode «&nbsp;synthé&nbsp;» de Polyphone peut également être utilisé pour [lire des fichiers MIDI](tutorials/read-midi-file-with-polyphone.md).
