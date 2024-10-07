Polyphone est en anglais par défaut et a été conçu pour être traductible, ce qui augmente ainsi son accessibilité.
Tout le monde peut aider dans une traduction.


## Prérequis


### Fichier de traduction


Téléchargez l'archive de traduction correspondant à la langue que vous souhaitez sur cette [page](software/translations).
Si la langue n'est pas dans la liste, utilisez le formulaire de [contact](contact) pour la demander (vous avez simplement besoin de mentionner la langue désirée).
Une nouvelle archive de traduction vous sera rapidement fournie.


### Qt Linguist


Le seul logiciel dont vous avez besoin est :program:`Qt Linguist`&nbsp;: un outil pour générer des fichiers de traduction.
Ce logiciel peut être trouvé <a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">ici</a> pour les utilisateurs de Windows ou dans les dépôts pour les utilisateurs de Linux (via :program:`Synaptic`, :program:`Yum`, …).


## Ouverture du fichier de traduction


Décompressez l'archive de traduction que vous venez de télécharger.
Vous trouverez à l'intérieur un fichier .ts :file:`polyphone_XX.ts`, :file:`XX` étant en lien avec votre langue.
À l'aide de :program:`Qt Linguist`, ouvrez le fichier :file:`polyphone_XX.ts` et vous serez à même de débuter la traduction.


## Traduction


Chaque élément dans chaque contexte doit être traduit.
Tout ce que vous avez à faire est de passer en revue chaque élément un par un et de remplir la traduction.
Trois cas de figure peuvent apparaître selon votre degré de confiance.

* Si vous ne savez pas comment traduire, n'hésitez pas à sauter l'élément.
  Ce ne sera pas un problème&nbsp;: quelqu'un d'autre pourra compléter.
* Vous pensez avoir la bonne traduction mais avec un doute&nbsp;: écrivez votre proposition et ensuite utilisez le raccourci :kbd:`Ctrl`+:kbd:`J`.
  L'élément ne sera pas validé et la traduction sera toutefois utilisée dans l'interface de Polyphone.
  Cela signifiera aux éventuels autres traducteurs souhaitant améliorer la traduction que vous n'étiez pas sûr.
* Vous avez la bonne traduction&nbsp;: écrivez-la et utilisez le raccourci :kbd:`Ctrl`+:kbd:`Enter`.
  L'élément sera validé.

Une traduction a besoin d'attention (dans un monde où tout est en train d'être fait pour la <a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">solliciter</a>).
Merci de faire attention&nbsp;:

* à la ponctuation,
* aux éventuels espaces avant et après,
* à la casse des caractères (minuscule / majuscule),
* aux singuliers et pluriels.

Un dernier conseil&nbsp;: **prenez votre temps&nbsp;!**
Et n'oubliez pas de sauvegarder votre travail avec :kbd:`Ctrl`+:kbd:`S`.


## Tester la traduction


À n'importe quel moment vous pouvez tester la traduction.
Dans :program:`Qt Linguist` allez dans :menuselection:`File --> Release As…` et créez le fichier .qm correspondant à la traduction que vous souhaitez fournir.
Suivez les indications contenues dans l'archive de traduction pour l'installer (fichier :file:`readme.txt`), ensuite ouvrez Polyphone.
Allez dans les [configurations](manual/settings.md#doc_interface), sélectionnez la traduction et redémarrez Polyphone.
Vous serez maintenant en mesure de passer en revue la traduction.

**Note&nbsp;:** l'opération :guilabel:`Release As…` pourrait ne traduire que les textes ne comprenant aucun accent. Si ce bug apparait, ouvrez un terminal et utilisez la commande suivante&nbsp;: :command:`lrelease polyphone_XX.ts` (adaptez-la au fichier en cours).


## Soumettre la traduction


Lorsque vous avez terminé (ou si vous en avez marre&nbsp;!), envoyez le fichier .ts (pas le fichier compilé .qm&nbsp;!) que vous étiez en train d'éditer à <contact@polyphone.io>.
L'archive de traduction sur cette [page](software/translations) sera mis à jour.