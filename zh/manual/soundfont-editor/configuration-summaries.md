配置概览可以总览音色库中包含的所有元素。
通过[树状图](manual/soundfont-editor/tree.md)中的 :guilabel:`采样`、:guilabel:`乐器` 和 :guilabel:`预设` 标题即可打开，用于查看对应的元素。

这些页面中汇集的信息可以帮助检查大型音色库。

每个元素显示为表格中的一行。
单击某一列的标题会按该列对表格排序，双击某一行会在其[编辑页面](manual/soundfont-editor/editing-pages/index.md)中打开对应的元素。
需要注意的值会以粗体、黄色（警告）或红色（错误）显示。


## 采样 {#doc_sample}


在[树状图](manual/soundfont-editor/tree.md)中单击音色库的 :guilabel:`采样` 标题，会出现采样配置概览。


![采样配置概览](images/conf_summary_sample.png "采样配置概览")


每个采样的列依次为：:guilabel:`已使用`、:guilabel:`总时长`、:guilabel:`循环时长`、:guilabel:`根音符`、:guilabel:`校正`、:guilabel:`采样类型`、:guilabel:`链接的采样`、:guilabel:`采样率` 和 :guilabel:`压缩`。
例如，可以通过此页面进行以下检查：

* 该采样是否被使用？
* 它的总时长（以及循环时长，如果有的话）是否足够长？
* 循环是否正确？（可能产生咔嗒声的循环会被突出显示）
* 是否已经调音？
* 立体声链接是否正确？（立体声对中的两个采样应具有相同的根音符）
* 采样率是否足够高，能保证良好的音质？
* 是否已经压缩？


## 乐器 {#doc_instrument}


在[树状图](manual/soundfont-editor/tree.md)中单击音色库的 :guilabel:`乐器` 标题，会出现乐器配置概览。


![乐器配置概览](images/conf_summary_instrument.png "乐器配置概览")


每个乐器的列依次为：:guilabel:`已使用`、:guilabel:`采样数`、:guilabel:`参数数`、:guilabel:`调制器数`、:guilabel:`最大音符范围`、:guilabel:`最大力度范围`、:guilabel:`衰减`、:guilabel:`循环播放`、:guilabel:`合唱` 和 :guilabel:`混响`。
例如，可以通过此页面进行以下检查：

* 该乐器是否被使用？
* 链接的采样、参数和调制器的数量是否合理？
* 音符范围和力度范围是否正确？
* 采样的衰减是否设置得当？
* 是否启用了循环播放？
* 是否遗漏了合唱和混响设置？


## 预设 {#doc_preset}


在[树状图](manual/soundfont-editor/tree.md)中单击音色库的 :guilabel:`预设` 标题，会出现预设配置概览。


![预设配置概览](images/conf_summary_preset.png "预设配置概览")


每个预设的列依次为：:guilabel:`库 - 预设`、:guilabel:`乐器数量`、:guilabel:`参数数量`、:guilabel:`调制器数量`、:guilabel:`最大音符范围`、:guilabel:`最大力度范围`、:guilabel:`衰减`、:guilabel:`合唱` 和 :guilabel:`混响`。
例如，可以通过此页面进行以下检查：

* 库编号和预设编号是否填写正确？
* 链接的乐器、参数和调制器的数量是否合理？
* 音符范围和力度范围是否正确？
* 乐器的衰减是否设置得准确？
* 是否遗漏了合唱和混响设置（作为偏移量）？