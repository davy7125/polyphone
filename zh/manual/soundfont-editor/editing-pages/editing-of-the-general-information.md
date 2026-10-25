在[树状图](manual/soundfont-editor/tree.md)中选中 :guilabel:`通用` 标题时，会显示一般信息编辑页面。
该页面由三类元素组成：

* [可编辑字段](#doc_edit)，
* 不可编辑的[一般信息](#doc_info)，
* 关于音色库的[统计信息](#doc_stats)。


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

* [采样](manual/soundfont-editor/editing-pages/sample-editor.md)数量以及未使用的采样数量，
* [乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)数量以及未使用的乐器数量，
* [预设](manual/soundfont-editor/editing-pages/preset-editor.md)数量，
* 乐器中已编辑的参数和调制器数量，
* 预设中已编辑的参数和调制器数量。

采样、乐器和预设旁边的 :guilabel:`详细信息` 按钮会显示相应的[配置概览](manual/soundfont-editor/configuration-summaries.md)。

未使用的采样和乐器可以用“[移除未使用的元素](manual/soundfont-editor/tools/global-tools.md#doc_unused)”工具轻松移除。

**注意：**乐器和预设中的参数数量不应超过 65536 个。
某些合成器可能无法全部读取。