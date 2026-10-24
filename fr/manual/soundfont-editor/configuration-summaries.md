Les résumés de configuration fournissent un aperçu de tous les éléments contenus dans une banque de sons.
Ils sont accessibles par le biais des entêtes :guilabel:`Échantillons`, :guilabel:`Instruments` et :guilabel:`Ensembles` dans l'[arborescence](manual/soundfont-editor/tree.md), et permettent une visualisation de leurs éléments correspondants.

Les informations résumées dans ces pages peuvent être utilisées pour des inspections dans les grandes banques de sons.

Chaque élément est affiché sur une ligne d'un tableau.
Un clic sur l'en-tête d'une colonne trie le tableau selon cette colonne, et un double-clic sur une ligne ouvre l'élément correspondant dans son [éditeur](manual/soundfont-editor/editing-pages/index.md).
Les valeurs qui méritent de l'attention sont écrites en gras, en jaune (avertissement) ou en rouge (erreur).


## Échantillons {#doc_sample}


En cliquant sur l'entête :guilabel:`Échantillons` d'une banque de sons dans l'[arborescence](manual/soundfont-editor/tree.md), le résumé de configuration des échantillons apparaît.


![Résumé de configuration des échantillons](images/conf_summary_sample.png "Résumé de configuration des échantillons")


Pour chaque échantillon, les colonnes sont&nbsp;: :guilabel:`Utilisé`, :guilabel:`Durée totale`, :guilabel:`Durée de la boucle`, :guilabel:`Note de base`, :guilabel:`Correction`, :guilabel:`Type d'échantillon`, :guilabel:`Échantillon lié`, :guilabel:`Taux d'échantillonnage` et :guilabel:`Compression`.
Ce résumé permet par exemple d'effectuer les vérifications suivantes&nbsp;:

* Est-il utilisé&nbsp;?
* Sa durée totale (et éventuellement sa durée de boucle) est-elle suffisante&nbsp;?
* La boucle est-elle correcte&nbsp;? (une boucle susceptible de produire un clic est mise en évidence)
* L'accordage a-t-il été effectué&nbsp;?
* Est-il correctement lié en stéréo&nbsp;? (les deux échantillons d'une paire stéréo devraient avoir la même note de base)
* Le taux d'échantillonnage est-il suffisant pour une bonne qualité audio&nbsp;?
* A-t-il été compressé&nbsp;?


## Instruments {#doc_instrument}


En cliquant sur l'entête :guilabel:`Instruments` d'une banque de sons dans l'[arborescence](manual/soundfont-editor/tree.md), le résumé de configuration des instruments apparaît.


![Résumé de configuration des instruments](images/conf_summary_instrument.png "Résumé de configuration des instruments")


Pour chaque instrument, les colonnes sont&nbsp;: :guilabel:`Utilisé`, :guilabel:`Nombre d'échantillons`, :guilabel:`Nombre de paramètres`, :guilabel:`Nombre de modulateurs`, :guilabel:`Étendue totale (notes)`, :guilabel:`Étendue totale (vélocités)`, :guilabel:`Atténuation`, :guilabel:`Lecture en boucle`, :guilabel:`Chorus` et :guilabel:`Réverbération`.
Ce résumé permet par exemple d'effectuer les vérifications suivantes&nbsp;:

* Est-il utilisé&nbsp;?
* Les nombres d'échantillons liés, de paramètres et de modulateurs sont-ils cohérents&nbsp;?
* Les étendues de notes et de vélocités sont-elles correctes&nbsp;?
* Les échantillons ont-ils été correctement atténués&nbsp;?
* La lecture en boucle est-elle bien activée&nbsp;?
* Les paramètres de chorus et de réverbération n'ont-ils pas été oubliés&nbsp;?


## Ensembles {#doc_preset}


En cliquant sur l'entête :guilabel:`Ensembles` d'une banque de sons dans l'[arborescence](manual/soundfont-editor/tree.md), le résumé de configuration des ensembles apparaît.


![Résumé de configuration des ensembles](images/conf_summary_preset.png "Résumé de configuration des ensembles")


Pour chaque ensemble, les colonnes sont&nbsp;: :guilabel:`Banque - Ensemble`, :guilabel:`Nombre d'instruments`, :guilabel:`Nombre de paramètres`, :guilabel:`Nombre de modulateurs`, :guilabel:`Étendue totale (notes)`, :guilabel:`Étendue totale (vélocités)`, :guilabel:`Atténuation`, :guilabel:`Chorus` et :guilabel:`Réverbération`.
Ce résumé permet par exemple d'effectuer les vérifications suivantes&nbsp;:

* Les numéros de banque et d'ensemble ont-ils été affectés correctement&nbsp;?
* Les nombres d'instruments liés, de paramètres et de modulateurs sont-ils cohérents&nbsp;?
* Les étendues de notes et de vélocités sont-elles correctes&nbsp;?
* Les instruments ont-ils été correctement atténués&nbsp;?
* Les paramètres de chorus et de réverbération (en tant qu'offsets) n'ont-ils pas été oubliés&nbsp;?