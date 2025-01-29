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

(suite bientôt disponible)