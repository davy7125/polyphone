在[树状图](manual/soundfont-editor/tree.md)中选中 :guilabel:`通用` 标题时，会显示一般信息编辑页面。
该页面由三类元素组成：

* [可编辑字段](#doc_edit)，
* 不可编辑的[一般信息](#doc_info)，
* 关于音色库的[统计信息](#doc_stats)。

[工具栏](manual/soundfont-editor/toolbar.md#doc_view)中的 :guilabel:`默认调制器` 按钮显示音色库的[默认调制器](#doc_default_mod)，:guilabel:`通用` 按钮则重新显示本页面。


![一般信息页面](images/edit_general.png "一般信息页面")


## 可编辑字段 {#doc_edit}


一般信息编辑页面提供以下属性的修改字段：

标题
: 音色库的内部名称，不一定与文件名相同。

作者
: 音色库作者的名称。

日期
: 文件的创建日期（可借助日历选择）。

产品
: 音色库所针对的产品。

版权
: 如果文件受版权保护，应在此填写相关声明。

备注
: 不属于上述任何类别的信息（文本可以包含任何 UTF-8 字符）。

位深
: 表示采样以 16 位还是 24 位分辨率存储（如果选择 24 位，则不允许压缩）。
  请注意，某些合成器可能不支持 24 位采样。

压缩
: 启用采样压缩（此时位深会设为 16 位）。启用此选项后，[采样编辑器](manual/soundfont-editor/editing-pages/sample-editor.md)中会显示一个压缩按钮。
  压缩是 [sf3 格式](manual/annexes/the-different-soundfont-formats.md#doc_sf3)提供的功能，并非所有合成器都支持。
  启用或禁用此选项决定了保存时的文件格式，必要时 Polyphone 会将文件扩展名改为 .sf2 / .sf3。


## 一般信息 {#doc_info}


一般信息编辑页面提供以下不可编辑的信息：

SoundFont 格式
: 文件所使用的 [sf2 格式](manual/annexes/the-different-soundfont-formats.md#doc_sf2)版本。

ROM 名称和版本
: 如果使用了采样 ROM，则显示其名称和版本。

音频引擎
: 音频引擎的名称。

编辑软件
: 创建该音色库所用的编辑软件。

将鼠标悬停在信息图标上（位于显示作者和日期的那一行末尾）时，这些信息会以提示框的形式显示。

文件名和大小
: 文件的路径和名称，以及它在磁盘上的大小，显示在窗口底部（大小为文件上次保存时的大小）。


## 统计信息 {#doc_stats}


会统计以下数量：

* [采样](manual/soundfont-editor/editing-pages/sample-editor.md)数量、未使用的采样数量以及重复的采样数量，
* [乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)数量以及未使用的乐器数量，
* [预设](manual/soundfont-editor/editing-pages/preset-editor.md)数量，
* 乐器中已编辑的参数和调制器数量，
* 预设中已编辑的参数和调制器数量。

采样、乐器和预设旁边的 :guilabel:`详细信息` 按钮会显示相应的[配置概览](manual/soundfont-editor/configuration-summaries.md)。

未使用的采样和乐器可以用“[移除未使用的元素](manual/soundfont-editor/tools/global-tools.md#doc_unused)”工具轻松移除。
重复的采样可以用“[清理重复项](manual/soundfont-editor/tools/global-tools.md#doc_duplicates)”工具移除。

**注意：**乐器和预设中的参数数量不应超过 65536 个。
某些合成器可能无法全部读取。



## 默认调制器 {#doc_default_mod}


默认调制器应用于音色库的每个乐器，并先于乐器自身的[调制器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)：乐器中源、目标和变换都相同的调制器会取代默认调制器（标签 :guilabel:`覆盖 默认调制器`）。

没有自己的列表时，音色库使用 SoundFont 格式的默认调制器，它们会显示出来但不能编辑：力度到衰减和到滤波器截止频率、通道压力和调制轮到颤音、音量、声像、表情、混响、合唱以及弯音轮。
力度到滤波器截止频率的调制器取决于[音频设置](manual/settings.md#doc_sound)。

:guilabel:`自定义` 按钮为音色库创建自己的列表，列表从这些调制器开始，之后可以像乐器的调制器一样编辑（添加、删除、复制、粘贴、修改）。
该列表取代 SoundFont 格式的默认调制器：从列表中删除的调制器不再应用于任何乐器，添加的调制器则应用于所有乐器。
:guilabel:`恢复为 SoundFont 默认值` 按钮删除该列表，恢复格式的默认调制器。

两个按钮可以在乐器和默认调制器之间移动调制器，且不改变声音：

* :guilabel:`自定义` 旁边的 :guilabel:`从乐器中合并`：所有乐器的全局分区中数值相同的调制器成为音色库的默认调制器，并从乐器中移除（调制量为 0 的调制器会删除对应的默认调制器）；
* :guilabel:`恢复为 SoundFont 默认值` 旁边的 :guilabel:`移至乐器`：音色库的默认调制器被移至所有乐器的全局分区，并重新应用 SoundFont 格式的默认调制器，这样在不读取 DMOD 块的合成器中文件的声音也相同。

该列表保存在 sf2 和 sf3 文件的 DMOD 块中，这是一项[格式扩展](manual/annexes/the-different-soundfont-formats.md#doc_sf2)，FluidSynth 2.5 及以上版本、SpessaSynth 以及 Polyphone 2.7 及以上版本（包括[插件](manual/plugin.md)）可以读取。
其他合成器会忽略它，并应用 SoundFont 格式的默认调制器。

将乐器复制到默认调制器不同的另一个音色库时，会为该乐器添加全局调制器，使其声音保持不变。
[DLS 导出](manual/annexes/the-different-soundfont-formats.md#doc_dls)会转换音色库的默认调制器；sfz 导出只保留力度对音量和滤波器截止频率的作用。