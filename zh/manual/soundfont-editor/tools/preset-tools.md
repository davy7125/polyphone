在[树状图](manual/soundfont-editor/tree.md)中选中一个预设或它的某个分区时，[工具菜单](manual/soundfont-editor/tools/index.md)中包含若干工具，分为 4 个类别：

* [分析](#doc_cat_analyze)，
* [快速编辑](#doc_cat_fast)，
* [调制器](#doc_cat_mod)，
* [变换](#doc_cat_transformation)。

除了显示参数之外，其他工具都可以通过多选同时用于多个预设。
所有这些工具也都可用于[乐器](manual/soundfont-editor/tools/instrument-tools.md)。


## 分析 {#doc_cat_analyze}


### 显示参数 {#doc_display}

与乐器的“[显示参数](manual/soundfont-editor/tools/instrument-tools.md#doc_display)”工具相同。


## 快速编辑 {#doc_cat_fast}


### 更改衰减 {#doc_attenuation}

与乐器的“[更改衰减](manual/soundfont-editor/tools/instrument-tools.md#doc_attenuation)”工具相同。


### 基于音符的配置 {#doc_global}

与乐器的“[基于音符的配置](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”工具相同。


### 声音空间化 {#doc_spatialization}

与乐器的“[声音空间化](manual/soundfont-editor/tools/instrument-tools.md#doc_spatialization)”工具相同。


## 调制器 {#doc_cat_mod}


### 移除调制器 {#doc_remove_mod}

与乐器的“[移除调制器](manual/soundfont-editor/tools/instrument-tools.md#doc_remove_mod)”工具相同。


## 变换 {#doc_cat_transformation}


### 分区复制 {#doc_division}

与乐器的“[分区复制](manual/soundfont-editor/tools/instrument-tools.md#doc_division)”工具相同。


### 合并每个音符的采样 {#doc_mergesamples}

当按下一个琴键时，如果预设同时调用多个乐器和采样，此工具会针对每个音符，将所有被播放的采样合并起来。结果是一个每个琴键只播放一个采样的新预设，从而降低运行该预设所需的 CPU 负载。
窗口会要求输入新预设的名称、所录制声音的延音时长和释音时长、所创建采样之间的间隔（每个音符、每 3 个音符或每 6 个音符）、是否自动循环以及声音类型（单声道或立体声）。

按照下图中设定的选项，该工具会每隔 3 个音符模拟一次持续 5 秒的按键。音频将以立体声录制，并且不录制释音。由此创建的采样随后会用[自动循环工具](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)设置循环，包含所有这些采样的乐器以及使用该乐器的新预设都将命名为“Cymbale III”。

**注意：**使用自动循环选项会去掉释音部分，并略微缩短每个采样的延音时长。

![合并每个音符的采样](images/tool_merge_samples.png "合并每个音符的采样")
