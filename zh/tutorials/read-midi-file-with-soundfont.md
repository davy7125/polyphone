MIDI 文件包含 MIDI 消息，组合起来就构成一段旋律。它们可以与使用音色库的合成器配合使用，本教程介绍如何用 MIDI 文件来测试音色库的效果。


## 一点背景知识


<div class="too_complicated">
<p>在合成器键盘上演奏时，会发出 <a href="https://zh.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a> 信号，其中包含所演奏的音符及其力度（表示按键力度大小的术语）等信息。简单来说，MIDI 文件就是按时间顺序记录了这样一个键盘所发出的信号。到这一步，还没有产生任何声音。</p>

<p>要让扬声器发出声音，还需要一个转换步骤，这就是合成器的作用：它根据收到的 MIDI 信号触发声音。合成器有多种类型，这里我们讨论的是基于采样的合成器，更具体地说，是使用 SoundFont 格式音色库的合成器。</p>

<p>一个 MIDI 文件最多可以控制 16 个不同的通道，从而可以同时使用 16 种不同的乐器。除了指明要演奏哪些音符外，MIDI 文件还包含一些指令，告诉合成器在通道 Z 上接下来的音符要使用库 Y 中的乐器 X。为了确保乐器选择正确，合成器所使用的音色库必须符合 <a href="https://zh.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> 标准，该标准为每个 X 和 Y 编号指定了特定的乐器。例如，在音色库中，乐器 <strong>001</strong> 必须对应钢琴。</p>

<p>总结一下，这里要讨论的是：播放一个 MIDI 文件，由它驱动一个合成器，合成器再触发一个符合 General MIDI 标准的音色库中的声音。接下来的内容就简单多了！</p>
</div>


## 准备工作


要用下文介绍的方法播放 MIDI 文件，你需要：

- 最新版本的 Polyphone，可在这个[页面](software)获取，
- 一个 MIDI 文件，例如<a href="files/ff7choco.mid" download>这个</a>，
- 一个你想测试、且符合 <a href="https://zh.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> 标准的音色库（[仓库](soundfonts/midi-standard/gm)中有相应的筛选条件）。

不符合该标准的音色库也可以测试，只需[自己选择它的预设](#doc_presets)。


## 播放 MIDI 文件


在 Polyphone 中打开你的音色库，然后单击[工具栏](manual/soundfont-editor/toolbar.md)右侧的 ![MIDI](images/toolbar_midi.png "MIDI"){.inline} 图标，显示[MIDI 播放器](manual/soundfont-editor/toolbar.md#doc_midi_player)。

单击窗口左上角的文件夹按钮，选择你的 MIDI 文件，然后单击播放按钮（或按 :kbd:`Space`），就可以开始了。

![Polyphone 的 MIDI 播放器](images/midi_player.png "Polyphone 的 MIDI 播放器")

文件中每个包含音符的通道在表格中各占一行。:guilabel:`预设` 列显示文件请求的预设（它可能在乐曲中途改变），每当有音符开始时，:guilabel:`音量` 列中的指示灯就会亮起。文件名下方的滑块可用于跳转到乐曲的其他段落。

以[合成器模式](tutorials/use-polyphone-as-a-synthesizer.md)打开 Polyphone 时，也可以使用 MIDI 播放器。


## 测试音色库


音色库按其在编辑器中的状态演奏，包括尚未保存的修改：你可以在乐曲播放时编辑乐器，并立即听到结果，例如在调整其音量、包络或滤波器时。

播放器还可以帮助你听清细节：

- 通道的 :guilabel:`S`（独奏）和 :guilabel:`M`（静音）按钮可以单独播放某个乐器或将其从混音中去掉，:guilabel:`音量` 列中的滑块则用于平衡它与其他通道的音量；
- :guilabel:`预设` 列中的警告图标表示音色库中没有文件请求的预设：该通道的音符不会被演奏，这说明该音色库没有完全符合 General MIDI 标准；
- 通道的 MIDI 按钮会列出文件当前使用的控制器（调制轮、表情、混响……），便于检查音色库的[调制器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)；
- 横幅右侧的[播放设置](manual/soundfont-editor/toolbar.md#doc_midi_player_settings)可以循环播放乐曲、放慢速度以便听清快速段落，或者移调以测试其他音。


## 选择音色库和预设 {#doc_presets}


每个通道都可以演奏不同的音色库：在 Polyphone 中打开多个音色库，然后双击 :guilabel:`音色库` 列中的单元格，选择该通道演奏的音色库。例如，你可以在同一声部上比较两个音色库的钢琴，或者在你自己的音色库演奏旋律时，用一个 General MIDI 音色库演奏伴奏。

默认情况下，各通道设为 :guilabel:`选择`：它们演奏当前标签页的音色库，或在[音色库浏览器](manual/soundfont-explorer.md#doc_play)中选中的文件。要比较一个目录中的所有音色库，请在 Polyphone 中打开该目录，并在乐曲播放时依次选中其中的文件：每个音色库都会在后台加载，准备就绪后即可听到，无需打开标签页。

![选择通道的音色库](images/midi_player_soundfonts.png "选择通道的音色库")

同样，双击 :guilabel:`预设` 列中的单元格，可以强制使用音色库中的某个预设，而不使用文件请求的预设。这样就可以测试不符合 General MIDI 标准的音色库，例如只包含一个乐器的音色库：为旋律所在的通道选择它的预设，该通道就会忽略文件中的程序变更。


## 导出结果


对结果满意后，播放设置中的 :guilabel:`导出音频...` 会按播放器中听到的效果将乐曲写入 wav 文件，音色库、预设、音量、速度和音高均与播放器相同。
也可以通过[命令行](manual/annexes/command-line.md#doc_render)将 MIDI 文件转换为 wav 文件，例如在脚本中使用。
