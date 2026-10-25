工具栏位于编辑器顶部，其中的图标用于：

* 常用的[编辑功能](#doc_edit)，
* 切换编辑器中的[视图](#doc_view)，
* 显示[录音机](#doc_recorder)，
* 显示[虚拟键盘](#doc_keyboard)。


![工具栏](images/toolbar.png "工具栏")


## 编辑功能 {#doc_edit}


工具栏左侧的元素用于常用的编辑功能：

![新采样](images/toolbar_sample.png "新采样"){.inline} 添加采样
: 向音色库添加新的[采样](manual/soundfont-editor/editing-pages/sample-editor.md)。
  会弹出一个对话框用于选择音频文件（可导入 :file:`.wav`、:file:`.flac`、:file:`.mp3`、:file:`.ogg`、:file:`.aif`、:file:`.aiff`、:file:`.aifc` 和 :file:`.snd` 文件），可以一次选择多个文件。

![新乐器](images/toolbar_instrument.png "新乐器"){.inline} 添加乐器
: 创建一个新的[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)。
  对话框会要求输入乐器名称，如果在[树状图](manual/soundfont-editor/tree.md)中选中了采样，还会提议将它们链接到新乐器。
  此时，立体声采样会分别放到左右声道，如果所有采样都设有循环则会启用循环，并且当采样的根音符各不相同时，会自动设定音符范围（参见[采样自动定位](manual/soundfont-editor/tools/instrument-tools.md#doc_position)工具）。

![新预设](images/toolbar_preset.png "新预设"){.inline} 添加预设
: 创建一个新的[预设](manual/soundfont-editor/editing-pages/preset-editor.md)。
  对话框会要求输入预设名称，如果在树状图中选中了乐器，还会提议将它们链接到新预设。
  会使用第一个可用的库编号和预设编号。

![工具](images/toolbar_toolbox.png "工具"){.inline} 工具箱
: 单击此按钮会显示一个菜单，其中列出可用于[树状图](manual/soundfont-editor/tree.md)中所选元素的所有[工具](manual/soundfont-editor/tools/index.md)。

![撤销](images/toolbar_undo.png "撤销"){.inline} 撤销
: 撤销上一次修改，也可按 :kbd:`Ctrl`+:kbd:`Z`。

![重做](images/toolbar_redo.png "重做"){.inline} 重做
: 重做上一次被撤销的修改，也可按 :kbd:`Ctrl`+:kbd:`Y` 或 :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Z`。

![保存](images/toolbar_save.png "保存"){.inline} 保存
: 保存音色库，也可按 :kbd:`Ctrl`+:kbd:`S`。


## 切换视图 {#doc_view}


根据当前显示的[编辑器](manual/soundfont-editor/editing-pages/index.md)，工具栏右侧可能会出现一些图标：

* 编辑乐器时，可以在 ![表格](images/toolbar_table.png "表格"){.inline} [表格视图](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)、![范围](images/toolbar_range.png "范围"){.inline} [范围编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)和 ![包络](images/toolbar_adsr.png "包络"){.inline} [包络编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope)之间切换，
* 编辑预设时，可以在 ![表格](images/toolbar_table.png "表格"){.inline} 表格视图和 ![范围](images/toolbar_range.png "范围"){.inline} 范围编辑器之间切换，
* 其他情况下不显示任何图标。


## 录音机 {#doc_recorder}


单击 ![录音机](images/toolbar_recorder.png "录音机"){.inline} 图标会打开一个显示录音机的窗口，它可以将 Polyphone 的输出录制为 :file:`.wav` 文件。窗口中会显示已录制的时间。


![录音机](images/recorder.png "录音机")


左侧按钮有两种状态：

录音
: 如果当前没有在录制，则开始新的录音。
  对话框会要求输入用于保存所录音频信号的文件名。

停止
: 停止当前的录制。
  关闭窗口时录制也会停止。

右侧按钮同样有两种状态（仅在录制进行中时可用）：

暂停
: 暂停当前的录制。

继续
: 继续当前的录制。


## 虚拟键盘 {#doc_keyboard}


单击 ![钢琴](images/toolbar_piano.png "钢琴"){.inline} 图标会打开一个显示虚拟键盘的窗口，它用于测试采样、乐器和预设。
该窗口会发出 MIDI 信号：

* 通过钢琴键盘发出 **note on** / **note off** / **aftertouch**（触后），
* 展开 **MIDI 控制器部分**后，还可发出其他 MIDI 信号。

正在演奏的音符的音符编号、力度和触后会显示在窗口底部，旁边还有一个下拉列表，用于选择键盘大小（5 个八度、6 个八度、88 键或 128 键）。


![虚拟键盘](images/virtual_keyboard.png "虚拟键盘")


### 钢琴键盘


钢琴键盘发出的信号包含音符编号和力度值。
可以通过三种方式控制它：

使用鼠标
: 可以用鼠标单击所需的琴键。
  力度由单击位置的垂直高度决定。
  单击之后，在同一个琴键内继续移动鼠标会发出触后信号。

使用电脑键盘
: 所用的按键在[设置](manual/settings.md#doc_keyboard)中定义。

使用外部 MIDI 键盘
: MIDI 输入在[设置](manual/settings.md#doc_general)中定义。
  如果钢琴键盘可见，它会显示你正在演奏的内容。


#### 标记和颜色


* 选中[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)或[预设](manual/soundfont-editor/editing-pages/preset-editor.md)时，未被分区覆盖的区域会显示为**灰色**。
  这样就可以直观地看到乐器或预设的完整音域。
* 选中乐器或预设的某个分区时，它的范围会以同样的方式显示。
  此外，它的根音符会用**橙色标记**标出。
* 在键盘上选中某个琴键时，它会显示为**蓝色**（或软件[设置](manual/settings.md#doc_interface)中选定的选择颜色）。
  此外，对应分区的范围会显示为**浅蓝色**。
  这只适用于乐器层级。
* 中央 C（音符 60）用**黑色标记**标出。


#### 实用快捷键


:kbd:`Ctrl`+:kbd:`K`
: 随时将焦点移到虚拟键盘（如果键盘处于隐藏状态则会显示出来）。
  这样，例如在编辑表格时，就可以用电脑键盘演奏，而不是编辑单元格。
  虚拟键盘上出现发光效果，表示焦点已在键盘上。

:kbd:`Ctrl`+:kbd:`1`、:kbd:`Ctrl`+:kbd:`2`……:kbd:`Ctrl`+:kbd:`8`
: 这些快捷键用于切换电脑键盘所演奏的八度。
  按 :kbd:`Ctrl`+:kbd:`1` 后可以演奏最低的几个八度。
  按 :kbd:`Ctrl`+:kbd:`8` 可以达到最高的几个八度。

:kbd:`→` / :kbd:`←`
: 按右方向键或左方向键，会以相同的力度演奏比上一次所弹琴键高或低一个半音的音。
  配合 :kbd:`Shift` 键时，会跳过 6 个琴键，而不是 1 个。

:kbd:`↑` / :kbd:`↓`
: 按上方向键或下方向键，会以更强或更弱的力度演奏与上一次所弹琴键相同的音高。
  配合 :kbd:`Shift` 键时，力度每次变化 10，而不是 1。


### MIDI 控制器


单击右下角的箭头可以显示或隐藏 MIDI 控制器区域。
如果你在使用调制器，建议利用这些功能来测试你的音色库。


![带 MIDI 控制器的虚拟键盘](images/virtual_keyboard_2.png "带 MIDI 控制器的虚拟键盘")


该区域由以下控件组成：

弯音轮
: 弯音轮用于升高或降低音高。
  它会自动回到初始位置 0。

灵敏度
: 灵敏度定义弯音轮的调节范围（以半音为单位）。
  默认值为 2（一个全音）。

压感
: 压感表示施加在键盘上的压力大小。
  它与触后类似，不同的是触后针对每个琴键，而压感是作用于所有琴键的全局信号。

旋钮
: 3 个旋钮用于改变 MIDI 控制器的值。
  每个旋钮都可以先指定一个控制器编号，再设定其值。

开 / 关指示灯
: 该指示灯用于将某个 MIDI 控制器的值设为 0 或 127（例如对延音踏板很有用）。
  它与旋钮类似，但只有两个可能的值。

如果在[设置](manual/settings.md#doc_general)中正确设置了 MIDI 输入，所有这些元素都可以与连接到电脑的 MIDI 键盘同步。旋钮支持相对型的 MIDI 控制器值。

*[MIDI]: Musical Instrument Digital Interface