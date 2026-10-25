在[树状图](manual/soundfont-editor/tree.md)中单击一个或多个预设时，会出现预设编辑页面。
预设是一个或多个乐器的组合，也是从[音色库](manual/annexes/the-different-soundfont-formats.md#doc_sf2)外部可见的元素。

与[乐器编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md)类似，该页面包含以下几个部分：

* [表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)，
* [范围编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)，在单击[工具栏](manual/soundfont-editor/toolbar.md#doc_view)中的 :guilabel:`范围` 按钮时显示，
* [调制器部分](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)。


![预设页面](images/edit_preset.png "预设页面")


如果只选中一个预设，该页面可以编辑预设的全局参数以及其所有分区的参数。
如果选中了多个预设，则可以编辑每个预设的全局参数。


## 与乐器的区别 {#doc_diff}


这里编辑的参数称为相对参数；而乐器的参数除特殊情况外，称为绝对参数。
预设的参数值会与组成它的乐器的参数相加或相乘。
因此，在预设层级，衰减（或任何其他参数）可以为负值，而在乐器层级则不行。
以下参数在预设层级不可编辑：

* 循环播放，
* 根音符，
* 独占类，
* 固定音符，
* 固定力度，
* 四个偏移量。

预设由预设编号和库编号标识，这一组合是唯一的，可以在表格下方调整。
每个库包含 128 个预设（编号从 0 到 127）。

最后一个库（128）通常用于打击乐套件。