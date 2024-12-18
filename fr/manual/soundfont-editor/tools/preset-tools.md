Lorsqu'un ensemble ou l'une de ses divisions est sélectionné dans l'[arborescence](manual/soundfont-editor/tree.md), la [boîte à outils](manual/soundfont-editor/tools/index.md) contient différents éléments répartis dans 4 catégories&nbsp;:

* [analyse](#doc_cat_analyze),
* [édition rapide](#doc_cat_fast),
* [modulateurs](#doc_cat_mod),
* [transformation](#doc_cat_transformation).

L'utilisation d'outils simultanément sur plusieurs ensembles est possible (avec une sélection multiple), sauf pour visualiser les paramètres.
Tous les outils sont aussi disponibles pour les [instruments](manual/soundfont-editor/tools/instrument-tools.md).


## Analyse {#doc_cat_analyze}


### Visualiser les paramètres {#doc_display}

Même outil que «&nbsp;[Visualiser les paramètres](manual/soundfont-editor/tools/instrument-tools.md#doc_display)&nbsp;» pour les instruments.


## Édition rapide {#doc_cat_fast}


### Modifier les atténuations {#doc_attenuation}

Même outil que «&nbsp;[Modifier les atténuations](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)&nbsp;» pour les instruments.


### Paramétrer en fonction des notes {#doc_global}

Même outil que «&nbsp;[Paramétrer en fonction des notes](manual/soundfont-editor/tools/instrument-tools.md#doc_global)&nbsp;» pour les instruments.


### Spatialiser le son {#doc_spatialization}

Même outil que «&nbsp;[Spatialiser le son](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)&nbsp;» pour les instruments.


## Modulateurs {#doc_cat_mod}


### Enlever les modulateurs {#doc_remove_mod}

Même outil que «&nbsp;[Enlever les modulateurs](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)&nbsp;» pour les instruments.


## Transformation {#doc_cat_transformation}


### Dupliquer les divisions {#doc_division}

Même outil que «&nbsp;[Dupliquer les divisions](manual/soundfont-editor/tools/instrument-tools.md#doc_division)&nbsp;» pour les instruments.


### Fusionner les échantillons {#doc_mergesamples}
 
Dans le cas où un ensemble appelle simultanément plusieurs instruments et échantillons lorsqu'une touche est appuyée, cet outil fusionnera, pour chaque note, tous les échantillons joués. Le résultat est un nouvel ensemble qui ne joue qu'un seul échantillon par touche, réduisant ainsi la charge CPU nécessaire à l'exécution de l'ensemble.

Les options définies dans l'image ci-dessous feront que l'outil va simuler toutes les 3 notes un appui de 5 secondes sur une touche. L'enregistrement audio se fera en stéréo et la relâche ne sera pas enregistrée. Les échantillons ainsi créés seront ensuite bouclés grâce à l'outil de [bouclage automatique](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop) et l'instrument recevant tous ces échantillons sera nommé «&nbsp;Cymbale III&nbsp;».

**Note&nbsp;:** l'utilisation de l'option de bouclage automatique supprimera la relâche et diminuera légèrement la durée de la partie appuyée de chaque échantillon.

![Fusionner les échantillons](images/tool_merge_samples.png "Fusionner les échantillons")
