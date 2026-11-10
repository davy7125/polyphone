工具栏位于编辑器顶部，其中的图标用于：

* 常用的[编辑功能](#doc_edit)，
* 切换编辑器中的[视图](#doc_view)，
* 显示[录音机](#doc_recorder)，
* 显示[虚拟键盘](#doc_keyboard)，
* 显示[MIDI 播放器](#doc_midi_player)。


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

选择其他乐器或预设时，所选视图保持不变。打开音色库时第一个乐器或预设显示的视图可以通过[设置](manual/settings.md#doc_interface)中的 :guilabel:`启动视图` 选项选择。


## 录音机 {#doc_recorder}


单击 ![录音机](images/toolbar_recorder.png "录音机"){.inline} 图标会打开一个显示录音机的窗口，它可以将 Polyphone 播放的内容、音频输入或 MIDI 信号录制到文件中。窗口中会显示已录制的时间。


![录音机](images/recorder.png "录音机")


横幅中的列表用于选择录音模式：

Polyphone → wav
: 将 Polyphone 的输出录制为 :file:`.wav` 文件。

麦克风 → wav
: 将[设置](manual/settings.md#doc_general)中选择的音频输入录制为 :file:`.wav` 文件，根据设备为立体声或单声道。
  输入不会发送到音频输出。

MIDI 信号
: 将使用 MIDI 键盘或[虚拟键盘](#doc_keyboard)演奏的内容录制为 :file:`.mid` 文件。

左侧按钮有两种状态（快捷键 :kbd:`Ctrl`+:kbd:`R`，在所有窗口中均可使用，必要时会打开录音机）：

录音
: 如果当前没有在录制，则开始新的录音。
  文件创建在上次录音所在的文件夹中；如果已存在同名文件，则会在文件名后添加编号。

停止
: 停止当前的录制。
  关闭窗口时，经确认后录制也会停止。

右侧按钮同样有两种状态（仅在录制进行中时可用，快捷键 :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`R`）：

暂停
: 暂停当前的录制。

继续
: 继续当前的录制。

这些按钮下方的电平表显示各声道的电平，即使没有录音时也会显示，因此可以在录音前调整音量。其右侧的数值是自录音开始以来达到的最高电平；如果达到了最大电平，该数值将显示在红色背景上，此时声音已被削波。单击电平表可重置此数值。在 :guilabel:`MIDI 信号` 模式下，电平表显示所演奏音符的力度，以确认已收到信号。

窗口底部根据模式列出最近 5 次音频或 MIDI 录音，以及它们的大小或音符数。不再存在的文件会在打开窗口时从列表中移除。单击可选中一行，该行随后显示四个按钮：

试听
: 在 Polyphone 中通过音频输出播放音频文件（再次单击可停止；播放期间选择另一行会改为播放该行的文件），或在 [MIDI 播放器](#doc_midi_player)中打开 MIDI 文件。

文件夹
: 打开文件所在的文件夹。

删除
: 经确认后将文件移到回收站。

导入
: 将音频文件作为采样导入当前显示的音色库。
  仅在显示音色库编辑器时此按钮才可用。

横幅右侧的菜单包含以下选项：

自动开始
: 录音按钮会等待第一个声音或 MIDI 信号，然后开始录音。
  自动停止后，将以同样的方式等待下一次录音，直到单击停止按钮。

静音后停止
: 没有声音或 MIDI 信号时停止录音的秒数，从 1 到 10（:guilabel:`已禁用` 表示不自动停止）。
  静音部分会从文件中移除，仍按住的琴键不算静音。

声音电平
: 仅用于音频输入：输入被视为声音的电平，高于其背景噪声。
  对于 Polyphone 的输出，此电平固定为 -90 dB。

询问文件名
: 在录音开始时通过对话框询问文件名。

更改文件夹...
: 选择之后录音的文件夹，音频文件和 MIDI 文件共用。

如果录音无法继续（磁盘已满或速度太慢、文件达到 4 GB、音频输入或音频输出的采样率发生变化），将显示一条消息，此前录制的内容会保留在文件中。


## 虚拟键盘 {#doc_keyboard}


单击 ![钢琴](images/toolbar_piano.png "钢琴"){.inline} 图标会打开一个显示虚拟键盘的窗口，它用于测试采样、乐器和预设。
该窗口会发出 MIDI 信号：

* 通过钢琴键盘发出 **note on** / **note off** / **aftertouch**（触后），
* 展开 **MIDI 控制器部分**后，还可发出其他 MIDI 信号。

窗口顶部的横幅中有一个列表，用于选择键盘类型（5 个八度、6 个八度、88 键或 128 键），横幅中还会显示正在演奏的音符的音符编号、力度和触后。
横幅右侧的菜单用于显示或隐藏 [MIDI 控制器](#doc_keyboard_controllers)和[音频效果](#doc_keyboard_effects)，它们都显示在键盘上方。

窗口会保持键盘的比例：改变宽度时高度随之改变。
显示或隐藏某个区域只会改变窗口的高度，窗口从顶部伸长或缩短，使键盘保持在屏幕上的同一位置（在 Linux 的 Wayland 下无法做到）。

也可以在[音色库浏览器](manual/soundfont-explorer.md#doc_play)中显示虚拟键盘，无需打开文件即可演奏目录中各文件的元素。


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
  只要虚拟键盘窗口处于活动状态，即使焦点位于该窗口的其他元素上（旋钮、下拉列表等），这些按键也能演奏。

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
  只要虚拟键盘窗口处于活动状态，这些快捷键同样有效。

:kbd:`→` / :kbd:`←`
: 按右方向键或左方向键，会以相同的力度演奏比上一次所弹琴键高或低一个半音的音。
  配合 :kbd:`Shift` 键时，会跳过 6 个琴键，而不是 1 个。

:kbd:`↑` / :kbd:`↓`
: 按上方向键或下方向键，会以更强或更弱的力度演奏与上一次所弹琴键相同的音高。
  配合 :kbd:`Shift` 键时，力度每次变化 10，而不是 1。

只有当钢琴键盘拥有焦点时，方向键才会演奏音符；否则它们作用于拥有焦点的元素，例如旋钮。


### MIDI 控制器 {#doc_keyboard_controllers}


在横幅右侧的菜单中勾选 :guilabel:`MIDI控制器` 选项后，键盘上方会显示 MIDI 控制器区域。
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


### 音频效果 {#doc_keyboard_effects}


在横幅右侧的菜单中勾选 :guilabel:`效果` 选项后，窗口顶部会显示一个区域，可以一边演奏一边调整合成器的主要设置：

混响
: 一个指示灯用于开启或关闭混响，4 个旋钮分别改变其电平、房间大小、宽度和阻尼。
  鼠标悬停在旋钮上时会显示其名称。

合唱
: 一个指示灯用于开启或关闭合唱，3 个旋钮分别改变其电平、深度和频率。

音量
: 滑块用于改变总音量。

这些设置与[音频设置](manual/settings.md#doc_sound)中的相同：在一处所做的更改会同步显示在另一处。
更改是渐进的，不会打断正在演奏的音符。


## MIDI 播放器 {#doc_midi_player}


单击 ![MIDI](images/toolbar_midi.png "MIDI"){.inline} 图标会打开一个显示 MIDI 播放器的窗口，它可以用已打开的音色库或在[音色库浏览器](manual/soundfont-explorer.md#doc_play)中选中的文件播放 MIDI 文件（:file:`.mid`、:file:`.midi`、:file:`.kar` 或 :file:`.rmi`），以便在乐曲中测试这些音色库。
音色库按其在编辑器中的状态演奏，包括尚未保存的修改。
在[合成器模式](tutorials/use-polyphone-as-a-synthesizer.md)下也可以使用 MIDI 播放器。


![MIDI 播放器](images/midi_player.png "MIDI 播放器")


窗口顶部的横幅从左到右包含：

* 用于打开 MIDI 文件的按钮（打开窗口时，如果上次打开的文件仍然存在，则会重新加载它），
* 文件名、已播放时间和总时长，下方是一个滑块，用于跳转到乐曲的其他位置，
* 用于开始或暂停播放的按钮，也可以使用 :kbd:`Space`，
* 用于停止播放并回到开头的按钮，
* 用于打开[播放设置](#doc_midi_player_settings)的按钮。

如果文件无法读取，则会在通道的位置显示原因。
关闭窗口时播放会停止。

播放期间，仍然可以使用外部 MIDI 键盘或[虚拟键盘](#doc_keyboard)配合文件演奏编辑器中选中的元素（文件中的音符不会显示在虚拟键盘上），并可以用[录音机](#doc_recorder)录制结果。


### 通道


表格列出文件中包含音符的通道，每个通道都有各自的设置：

通道
: 通道编号，其音轨名称显示在提示框中。
  :guilabel:`M`（静音）和 :guilabel:`S`（独奏）按钮用于将该通道静音，或只播放处于独奏状态的通道。
  MIDI 按钮显示该通道当前与默认值不同的 MIDI 值：控制器、弯音轮（以半音为单位）、弯音轮灵敏度和通道压力。

音色库
: 该通道演奏的音色库：:guilabel:`选择`、标签页中已打开的某个音色库，或选择 :guilabel:`无` 以静音。
  打开文件时所有通道都设为 :guilabel:`选择`，它演奏主窗口中选中的内容：当前标签页的音色库，或在[音色库浏览器](manual/soundfont-explorer.md#doc_play)中选中的文件，其名称显示在括号中。
  其他标签页（主屏幕、设置、在线仓库……）会保留正在演奏的音色库。
  选中内容改变时，在音色库加载期间名称前会有一个加载图标旋转，同时继续演奏之前的音色库；之后这些通道的预设会恢复为 :guilabel:`自动`。

预设
: 选择 :guilabel:`自动` 时，预设由文件中的程序变更选择（打击乐使用通道 10 上的库 128），当前预设显示在括号中。
  否则，无论文件请求什么，都会演奏在列表中选择的预设。
  如果音色库中没有文件请求的预设，则会显示警告图标，并且该通道的音符不会被演奏。

音量
: 音符开始时指示灯会亮起，滑块可在 -40 到 +10 dB 之间改变该通道的音量（双击可恢复为 0 dB）。

双击单元格，或单击已选中的单元格，即可打开音色库或预设列表。
打开其他文件时，这些设置会被重置。


### 播放设置 {#doc_midi_player_settings}


横幅右侧的按钮会打开一个菜单，其中包含以下设置，打开其他文件时这些设置会保留：

循环播放
: 乐曲结束后从头重新开始播放。

速度
: 播放速度，为文件速度的 25% 到 200%（保留文件中的速度变化）。
  文件的速度显示在提示框中，横幅中显示的已播放时间和总时长会随播放速度变化。

音高
: 在 -12 到 +12 个半音之间移调，打击乐（库 128）除外。

双击滑块可将其恢复为默认值（100% 和 0）。


![播放设置](images/midi_player_settings.png "播放设置")


菜单的最后一项是 :guilabel:`导出音频...`，它会按播放器中听到的效果将乐曲写入 :file:`.wav` 文件：音色库、预设、音量、静音和独奏、速度以及音高均与播放器相同。
导出期间播放会暂停，导出会在最后几个音符的声音消失后结束。
如果取消导出，未完成的文件会被删除。
也可以通过[命令行](manual/annexes/command-line.md#doc_render)将 MIDI 文件渲染为 wav 文件。

*[MIDI]: Musical Instrument Digital Interface