在[树状图](manual/soundfont-editor/tree.md)中选中 :guilabel:`通用` 标题时，[工具菜单](manual/soundfont-editor/tools/index.md)提供的工具分为 3 个类别：

* [清理](#doc_cat_clean)，
* [调制器](#doc_cat_mod)，
* [实用工具](#doc_cat_util)。


## 清理 {#doc_cat_clean}


### 移除未使用的元素 {#doc_unused}


所有未被任何乐器使用的采样，以及所有未被任何预设使用的乐器，都会被自动删除。
会有一条消息提示删除了多少个元素。


### 清理重复项 {#doc_duplicates}


相同的采样会被合并：数据、采样率、循环、根音符和校正都必须相同，立体声采样会连同其链接的采样一起比较。
在每组相同的采样中，（按文件中的顺序）保留第一个采样，并在乐器中用它替换其他采样，其他采样则被删除。
会有一条消息提示删除了多少个采样。

可以事先在[采样配置概览](manual/soundfont-editor/configuration-summaries.md#doc_sample)中查看重复项。


![采样配置概览中的重复项](images/tool_clean_duplicates.png "采样配置概览中的重复项")



## 调制器 {#doc_cat_mod}


### 移除所有调制器 {#doc_removemods}


所有[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)和[预设](manual/soundfont-editor/editing-pages/preset-editor.md)中的所有[调制器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)都会被移除。


## 实用工具 {#doc_cat_util}


### 导出预设列表 {#doc_export}


使用此工具，可以将所有[预设](manual/soundfont-editor/editing-pages/preset-editor.md)的列表连同其库编号和预设编号一起写成文本。
该文本可以设为 csv 格式以便导入电子表格，或设为 html 表格以便嵌入网站。
有一个按钮可以将文本复制到剪贴板。


![预设列表](images/en_preset_list.png "预设列表")