Un Raspberry Pi est un ordinateur de la taille d'une carte bancaire et dans lequel Polyphone peut être installé. En l'intégrant dans un caisson contenant des haut-parleurs et en associant un clavier maître, il est possible de réaliser un vrai synthétiseur&nbsp;!

![Synthétiseur fait maison - avant](images/synth_front.jpg "Synthétiseur fait maison - avant")

![Synthétiseur fait maison - arrière](images/synth_back.jpg "Synthétiseur fait maison - arrière")

## Matériel nécessaire

Cette liste est adaptable en fonction du projet que vous souhaitez mener&nbsp;:
- un Raspberry Pi,
- une carte micro SD de 32 Go ou plus de capacité,
- une alimentation pour Raspberry Pi,
- un convertisseur analogique / numérique (DAC) pour transformer une sortie USB en prise audio Jack,
- des haut-parleurs d'ordinateur, qui ont l'avantage de contenir à la fois les haut-parleurs et la partie amplification du signal,
- un interrupteur général on / off,
- un câble USB 2x mâle vers femelle encastrable,
- un caisson pour mettre le tout à l'intérieur,
- et bien sûr un clavier maître pour pouvoir jouer.

Pour configurer facilement le Raspberry Pi, il est conseillé d'avoir également&nbsp;:
- un écran avec un câble mini-HDMI pour lui permettre de se raccorder au Raspberry Pi,
- un clavier,
- une souris.

Dans l'exemple de cette page des enceintes Edifier R1280T, achetées d'occasion, ont été utilisées. Pour un prix modeste, elles sont capables de restituer les sons graves relativement bien et comme vous allez le voir plus loin, il sera possible de corriger les défauts de la réponse fréquentielle.

Le clavier maître utilisé est un Akai MPK Mini Plus, raccordable par USB. L'adaptateur USB / jack est un premier prix de la marque UGREEN et le Raspberry Pi est dans sa version 5.

(l'objectif de ces précisions n'est pas de faire de la publicité pour une marque en particulier mais de présenter une combinaison qui fonctionne)

## Préparation du caisson

Ce tutoriel ne présentera pas une manière en particulier de fabriquer un caisson. L'usage du bois pourra mettre en avant vos talents de menuisier comme dans l'exemple actuel mais le carton peut aller tout aussi bien. A vous d'exprimer vos talents créatifs&nbsp;! Globalement, il faut prévoir un couvercle pour accéder aux éléments et des ouvertures pour&nbsp;:
- les enceintes,
- le câble électrique,
- le bouton on / off,
- les boutons pour le volume et - le cas échéant - l'égalisation,
- les résonateurs de Helmholtz pour le bass-reflex, etc.

Un ordre de montage dans le caisson peut être le suivant&nbsp;:
- commencer par encastrer le bouton on / off, les connecteurs USB sur la façade arrière ainsi que tout autre bouton en lien avec les haut-parleurs,
- placer le câble d'alimentation et le connecter à l'entrée du bouton on / off,
- placer le transformateur associé aux haut-parleurs et le raccorder à la sortie du bouton on / off,
- placer la carte électronique des haut-parleurs et la raccorder au transformateur,
- fixer les haut-parleurs et les raccorder à la carte électronique,
- fixer le Raspberry Pi,
- placer l'alimentation du Raspberry Pi et connecter cette alimentation à la sortie du bouton on / off (la sortie du bouton alimente donc à la fois les haut-parleurs et le Raspberry Pi),
- connecter le double câble USB au Raspberry Pi,
- placer le convertisseur USB / jack sur une autre prise USB,
- connecter la carte électronique des haut-parleurs grâce à un cable jack.

Dans l'image qui suit, l'alimentation et la carte électronique liées aux haut-parleurs sont entourées en orange. Le Raspberry Pi est entouré de bleu.

![Intérieur du synthétiseur](images/synth_inside.jpg "Intérieur du synthétiseur")

Note&nbsp;: il faut placer les haut-parleurs de manière à ce qu'il n'y ait pas de retour possible du son entre la sortie du haut-parleur et l'intérieur du caisson, sinon une résonance va apparaître. **Donc pas comme dans cet exemple&nbsp;!** Il faut parfois expérimenter pour découvrir des pièges...

![Le Raspberry Pi](images/synth_raspberry_pi.jpg "Le Raspberry Pi")

3 prises USB sont utilisées&nbsp;:
- l'une pour obtenir une sortie jack,
- les 2 autres sont redirigées vers l'arrière du caisson et serviront pour raccorder le clavier maître ou, lors de la configuration du Raspberry Pi, un clavier et une souris.

## Préparation du Raspberry Pi

### Installation de Debian

L'une des premières étapes à réaliser après la réception d'un Raspberry Pi est l'installation de **Debian** sur une carte micro SD. Vous trouverez des instructions sur Internet très facilement, par exemple <a href="https://www.raspberrypi-france.fr/installer-debian-sur-un-raspberry-pi-guide-complet/" target="_blank">ici</a>.

Une fois la carte micro SD préparée, vous pourrez l'insérer dans la fente prévue à cet effet dans le Raspberry Pi. Connectez ensuite l'alimentation, un écran, une souris et un clavier, et votre ordinateur est prêt !

### Installation et configuration de Polyphone

Mettez tout d'abord à jour le système grâce aux commandes :

```
apt-get update
apt-get upgrade
```

Installez ensuite **Polyphone** grâce au package .deb disponible sur cette [page](software). Connectez le clavier MIDI par USB au Raspberry Pi (déconnectez éventuellement le clavier d'ordinateur si aucun port USB n'est disponible), puis ouvrez Polyphone et sélectionnez le clavier MIDI dans la liste déroulante des entrées MIDI dans les [configurations](manual/settings.md#doc_general). En téléchargeant une soundfont, par exemple depuis le [dépôt](soundfonts), il est déjà possible de jouer d'un instrument avec le clavier MIDI associé au Raspberry Pi !

### Lancement automatique de Polyphone

Cette étape est nécessaire pour ne plus avoir besoin de l'écran. Polyphone peut en effet être lancé automatiquement au démarrage du Raspberry Pi en créant un fichier :file:`synthesizer.desktop`, contenant ce texte :

```
[Desktop Entry]
Name=Synthesizer
Exec=polyphone -s /home/pi/Desktop/my-soundfont.sf2
Terminal=false
Type=Application
StartupNotify=true
Icon=/home/pi/Desktop/my-icon.png
Categories=AudioVideo;
```

Ce fichier sera ensuite placé dans le dossier :file:`/home/pi/.config/autostart`. 

La ligne de commande peut être complétée avec des options pour par exemple écouter un canal MIDI particulier, jouer de plusieurs instruments simultanément ou encore activer / désactiver des instruments grâce à des touches. Cette [page](tutorials/use-polyphone-as-a-synthesizer.md) détaille les options pour utiliser Polyphone en mode synthé.

## Pour aller plus loin

### Égalisation du son

Chaque haut-parleur, de par sa morphologie, répond plus ou moins différemment selon les fréquences que l'on veut lui faire émettre. Pour obtenir une réponse fréquentielle plate et ainsi restituer fidèlement le son, il est possible de créer une égalisation qui compense les défauts des hauts-parleurs. L'outil <a href="https://github.com/erdewit/HiFiScan" target="_blank">HiFiScan</a> permet de calculer automatiquement les paramètres de l'égalisation, le protocole étant décrit sur la page du projet.

Une fois la courbe d'égalisation calculée, **HiFiScan** peut exporter des coefficients à intégrer dans des égaliseurs. Ce qui nous intéresse est l'égalisation 12 bandes qui peut être utilisée avec **Calf**.

### Connexion à distance

La configuration du Raspberry Pi a jusque là nécessité un écran pour permettre l'affichage du bureau. Il est toutefois possible de configurer le système pour permettre un accès à distance via le **protocole VNC** et des ressources existent sur internet pour sa configuration. 
