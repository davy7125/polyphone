在 Polyphone 中，可以在[主屏幕](manual/index.md#doc_left)上单击 :guilabel:`打开目录` 按钮来打开一个目录。随后，音色库浏览器会在新标签页中打开，显示该目录中的文件（不扫描其子目录），并为每个音色库提供内容预览。


![音色库浏览器](images/soundfont_explorer.png "音色库浏览器")


## 音色库预览 {#doc_preview}


主列表位于界面左侧，包含文件列表，Polyphone 会按以下方式区分这些文件：

* 可以快速扫描的兼容文件（例如 [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) 和 [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls) 格式）会显示其内容的详细信息，
* 无法快速扫描的兼容文件（尤其是 [.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) 格式）只显示基本信息，
* 不兼容的文件显示为灰色，无法打开。

单击一个可扫描的文件后，界面右侧的列表中会显示它包含的采样、乐器和预设。这些列表中的每个条目还附有一些信息，有助于评估该音色库的精细程度以及制作者投入的工作量。

### 文件层级的详细信息

* 每个文件都会显示大小和最后修改日期，
* 对于可以扫描的音色库，还会显示以下信息：
  * 采样、乐器和预设的数量，
  * 前几个预设的名称。

这些信息在表头下按列对齐。文本框右侧的 :guilabel:`显示` 按钮会打开一个菜单，用于选择显示哪些列（除名称外均可隐藏），以及是否显示 :guilabel:`预设摘要`，即第二行中前几个预设的名称。不显示摘要时，每个文件只占一行，如同表格。

### 每个采样的信息

* 类型（单声道 / 立体声），
* 总时长，
* 循环时长（如果有），
* 采样率。

### 每个乐器或预设的信息

* 分区、参数和生成器的总数，
* 不同音符范围的数量，
* 不同力度范围的数量。


## 排序和搜索功能 {#doc_filter}


上方区域有一个文本框（快捷键 :kbd:`Ctrl`+:kbd:`F` 会将焦点移到该文本框）。填写内容后，浏览器只显示至少满足以下一个条件的文件：

* 文件名包含所搜索的文本，
* 其中某个采样、乐器或预设的名称包含所搜索的文本。

采样、乐器和预设列表也会被筛选，以突出显示匹配的元素。

单击列标题可按名称、大小、日期或采样、乐器、预设的数量对文件排序，再次单击则反转顺序。第一次单击时，名称按 A 到 Z 排序，其他列从最大值或最新的文件开始排序。按元素数量排序时，内容未知的文件始终排在列表末尾。

显示选项、排序方式，以及右侧列表的宽度和展开或折叠状态都会保留到下次使用。


## 文件操作 {#doc_file_operations}


在主列表中单击一个文件时，文件名前面会出现 2 个可单击的图标：

* 铅笔图标，用于重命名文件（也可按 :kbd:`F2`），
* 垃圾桶图标，用于在确认后将文件移到回收站（也可按 :kbd:`Del`）。

利用这些操作，可以快速管理和清理本地的音色库收藏。


## 演奏元素 {#doc_play}


:guilabel:`显示` 按钮右侧的 ![MIDI](images/toolbar_midi.png "MIDI"){.inline} 按钮用于显示或隐藏[MIDI 播放器](manual/soundfont-editor/toolbar.md#doc_midi_player)，旁边的 ![piano](images/toolbar_piano.png "piano"){.inline} 按钮用于显示或隐藏[虚拟键盘](manual/soundfont-editor/toolbar.md#doc_keyboard)，![录音机](images/toolbar_recorder.png "录音机"){.inline} 按钮用于显示或隐藏[录音机](manual/soundfont-editor/toolbar.md#doc_recorder)，与编辑器工具栏中的按钮作用相同。

在右侧列表中选中的采样、乐器或预设可以用虚拟键盘演奏，即使虚拟键盘处于隐藏状态，也可以用外部 MIDI 键盘演奏。与编辑器中一样，虚拟键盘会显示该元素的音域，或采样的根音符。如果采样带有循环，则循环播放；如果勾选了采样[播放器](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player)的 :guilabel:`立体声` 选项，则以立体声播放。

首次选中某个文件中的元素时，音色库会在后台加载，不会打开标签页。之后，只要选中的元素属于同一个文件，音色库就会一直保留；在主列表中选中另一个文件时，它会被释放。

已在标签页中打开的音色库会按照它在编辑器中的状态演奏，包括尚未保存的修改。

设为 :guilabel:`选择` 的 MIDI 播放器通道会演奏主列表中选中的文件，因此无需打开目录中的各个音色库，就能用每个音色库聆听同一个 MIDI 文件。文件会在选中后不久于后台加载（用方向键快速浏览经过的文件不会被加载），准备就绪后即开始演奏。这在[合成器模式](tutorials/use-polyphone-as-a-synthesizer.md)下同样适用，此时 MIDI 播放器是唯一可用的按钮。


## 打开音色库 {#doc_open_soundfont}


双击主列表中的条目会打开该文件（前提是其格式与 Polyphone 兼容），打开后显示的位置由[设置](manual/settings.md#doc_interface)中的 :guilabel:`启动视图` 选项决定（默认显示[一般信息](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)）。

双击某个[采样](manual/soundfont-editor/editing-pages/sample-editor.md)、[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)或[预设](manual/soundfont-editor/editing-pages/preset-editor.md)，会打开该文件并定位到所选的元素。

如果该音色库已为演奏其中的元素或为 MIDI 播放器而加载，标签页会立即打开（合成器模式除外，此时会重新加载文件）。