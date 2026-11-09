Quand l'entête :guilabel:`Général` est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md), la [boîte à outils](manual/soundfont-editor/tools/index.md) propose des outils rangés dans trois catégories&nbsp;:

* [nettoyage](#doc_cat_clean),
* [modulateurs](#doc_cat_mod),
* [utilitaire](#doc_cat_util).


## Nettoyage {#doc_cat_clean}


### Enlever les éléments non utilisés {#doc_unused}


Tous les échantillons qui ne sont utilisés par aucun instrument et tous les instruments qui ne sont utilisés par aucun ensemble sont automatiquement supprimés.
Un message indique combien d'éléments ont été supprimés.


### Nettoyer les doublons {#doc_duplicates}


Les échantillons identiques sont fusionnés&nbsp;: les données, le taux d'échantillonnage, la boucle, la note de base et la correction doivent être les mêmes, et les échantillons stéréo sont comparés avec leur échantillon lié.
Pour chaque groupe d'échantillons identiques, le premier (dans l'ordre du fichier) est conservé et remplace les autres dans les instruments, les autres étant supprimés.
Un message indique combien d'échantillons ont été supprimés.

Les doublons sont visibles au préalable dans le [résumé de configuration des échantillons](manual/soundfont-editor/configuration-summaries.md#doc_sample).


## Modulateurs {#doc_cat_mod}


### Enlever les modulateurs {#doc_removemods}


Tous les [modulateurs](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator) présents dans tous les [instruments](manual/soundfont-editor/editing-pages/instrument-editor.md) et [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md) sont supprimés.


## Utilitaire {#doc_cat_util}


### Exporter la liste des ensembles {#doc_export}


Avec cet outil, la liste de tous les [ensembles](manual/soundfont-editor/editing-pages/preset-editor.md) est écrite sous forme de texte, avec leurs numéros de banque et d'ensemble.
Ce texte peut être formaté en csv, pour être intégré dans un tableur, ou en tableau html, pour être intégré dans un site web.
Un bouton copie le texte dans le presse-papier.


![Liste des ensembles](images/en_preset_list.png "Liste des ensembles")