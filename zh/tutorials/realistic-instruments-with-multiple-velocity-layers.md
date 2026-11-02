许多音色库每个音符只播放一个采样：无论以多大的力度按下琴键，听到的都是同一段录音，只是音量更大或更小。
原声乐器并不是这样。
轻轻按下的钢琴琴键发出圆润柔和的声音，而用力按下同一个琴键时，声音更明亮、泛音更丰富，起音也更有打击感。

力度层为音色库带来了这种丰富性：每个音符以多种力度录制，并根据音符的力度选择要播放的录音。
本教程以钢琴为例，但这种方法适用于所有音色随演奏力度变化的乐器：吉他、鼓、铜管乐器、弦乐器……
最后还会介绍一些让乐器更加逼真的其他细节。


## 为什么一个采样不够 {#doc_why}


请听 C4 琴键由弱到强弹奏四次（力度 30、60、90 和 120）：

* 使用以中等力度录制的<a href="files/velocity-layers-one-layer.mp3" download>单个采样</a>，

![](files/velocity-layers-one-layer.mp3)

* 使用<a href="files/velocity-layers-four-layers.mp3" download>四个力度层</a>，根据力度选择录音。

![](files/velocity-layers-four-layers.mp3)

使用单个采样时，弱音听起来像是从远处听到的强音，而最强的音符缺少光彩。
合成器会为弱音略微降低滤波器的截止频率（这是默认调制器之一），但这与真实钢琴的音色变化相去甚远。
使用四个力度层时，音色会随演奏力度而变化。

在音乐中，差别同样明显，这里用的是越来越强的琶音：

* <a href="files/velocity-layers-phrase-one-layer.mp3" download>单个采样</a>，

![](files/velocity-layers-phrase-one-layer.mp3)

* <a href="files/velocity-layers-phrase-four-layers.mp3" download>四个力度层</a>。

![](files/velocity-layers-phrase-four-layers.mp3)


## 准备采样 {#doc_samples}


### 录制各个力度层


每个音符都需要以多种力度录制，例如 *pp*（极弱）、*mp*（中弱）、*mf*（中强）和 *ff*（极强）。
使用 3 或 4 个力度层时效果已经很明显，而最好的钢琴音源库会使用 8 到 16 个力度层，使过渡更加平滑。
所有录音都必须在相同条件下进行（话筒、距离、增益）：只有演奏力度可以改变。

本教程的示例使用 4 个力度层，在钢琴的整个音域内每 3 个琴键录制一个采样。


### 导入


用音符和力度为文件命名，例如 :file:`C4 pp.wav`、:file:`C4 mp.wav`、:file:`C4 mf.wav` 和 :file:`C4 ff.wav`。
如果音频文件中没有保存根音符，只要文件名中包含英文写法的音名（C4 为琴键 60）或琴键编号，Polyphone 就能从文件名中找到根音符。
名称中写明的力度会在下一步中派上用场。

使用[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)中的 :guilabel:`添加采样`，或通过[拖放](manual/soundfont-editor/tree.md#doc_dragdrop)到树状图中，导入所有文件。
一个立体声文件会生成两个采样，名称分别以“L”和“R”结尾。
然后按照教程“[如何准备采样](tutorials/how-to-prepare-a-sample.md#doc_tuning)”中的说明，检查每个采样的调音。
钢琴采样不需要循环：声音会自然衰减。


## 每个力度层一个乐器 {#doc_instruments}


每个力度层都成为一个乐器，只包含同一种力度的采样。
对每种力度：

1. 在树状图的[搜索栏](manual/soundfont-editor/tree.md#doc_search)中输入它的名称（例如“pp”），使树状图只显示这一力度层的采样，
2. 选中所有这些采样：单击第一个，然后按住 :kbd:`Shift` 键单击最后一个，
3. 单击[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)中的 :guilabel:`添加乐器`，为乐器命名（例如“Piano pp”），并保持 :guilabel:`链接选定采样` 选项为勾选状态。

立体声采样会自动分别声像到左侧和右侧，音符范围会根据根音符自动分布到键盘上（参见工具“[采样自动定位](manual/soundfont-editor/tools/instrument-tools.md#doc_position)”）。
现在已经可以用[虚拟键盘](manual/soundfont-editor/toolbar.md#doc_keyboard)单独弹奏每个乐器。

所有力度层共用的参数可以一次性编辑：在树状图中选中这四个乐器后，[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)会并排显示它们的全局分区，选中某一行的四个单元格后输入的值会写入所有这些单元格。
在示例中，:guilabel:`音量包络释音(秒)` 参数设为 1 秒，这样松开琴键时声音不会突然中断。


## 在预设中叠加力度层 {#doc_preset}


在树状图中选中这四个乐器，在勾选 :guilabel:`链接选定乐器` 选项的情况下单击 :guilabel:`添加预设`。
此时预设包含四个分区，每个分区都覆盖整个键盘。
在[预设编辑器](manual/soundfont-editor/editing-pages/preset-editor.md)中为预设的每个分区设置不同的 :guilabel:`力度范围`，即可叠加力度层：

* 力度层 *pp* 为 0-39，
* 力度层 *mp* 为 40-69，
* 力度层 *mf* 为 70-99，
* 力度层 *ff* 为 100-127。

这些范围必须覆盖 0 到 127 的所有力度，既不能有空隙（某些力度会没有声音），也不能有重叠（两个力度层会同时发声）。
边界不一定要均匀：可以根据录音进行调整。
:guilabel:`衰减(分贝)` 行中的数值将在下一节中说明。


![预设表格中的力度层](images/velocity_layers_table.png "预设表格中的力度层")


在[范围编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)中，每个力度层都是一条水平带：选中的分区（这里是 *mp*）以斜线填充显示。
在这个视图中，空隙或重叠一目了然。


![范围编辑器中的力度层](images/velocity_layers_ranges.png "范围编辑器中的力度层")


**注意：**力度范围也可以在一个包含所有采样的乐器中定义，每个分区都有自己的力度范围。
这时，在[设置](manual/settings.md#doc_interface)的 :guilabel:`分区排序` 选项中选择 :guilabel:`按力度范围`，可以让同一力度层的分区排列在一起。


## 平衡各个力度层 {#doc_balance}


在一个力度层内部，音量已经会随力度变化：这是由 sf2 格式定义的另一个默认调制器完成的。
但在每个边界处，由于上一层的录音更响，音量会突然增大：在示例中约为 5 到 7 分贝。
目标是从一个力度层过渡到下一个力度层时只有音色发生变化，而音量平滑地增加。

使用[虚拟键盘](manual/soundfont-editor/toolbar.md#doc_keyboard)比较边界两侧的声音：音符的力度取决于单击的垂直位置，方向键 :kbd:`↑` 和 :kbd:`↓` 会以增加或减少 1 的力度重新弹奏最后一个琴键。
然后，从下往上对每个边界进行以下操作：

1. 先以某一力度层的最后一个力度（例如 39）弹奏一个琴键，再以下一力度层的第一个力度（40）弹奏，
2. 在预设表格中增加上一层的衰减，直到两个音符的音量大致相同，
3. 继续处理下一个边界：衰减会逐层累加，每个力度层的衰减至少与其下方的力度层相同。

在示例中，衰减分别为 0、6、11 和 16 分贝，如上面的表格所示。
请在整个键盘上检查多个琴键：如果某个音区仍然不平衡，可以在该力度层的乐器中细调相关分区的衰减，例如使用工具“[基于音符的配置](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”。

这样，乐器从最弱到最强的整体动态范围就由默认调制器决定。
如果弱音太弱或不够弱，可以使用工具“[覆盖默认调制器](manual/soundfont-editor/tools/instrument-tools.md#doc_default_mod)”修改这个调制器。

最后，测试效果的最好方法是真正的乐曲：用 MIDI 键盘或 [MIDI 播放器](manual/soundfont-editor/toolbar.md#doc_midi_player)演奏音色库（参见教程“[用音色库播放 MIDI 文件](tutorials/read-midi-file-with-soundfont.md)”）。


## 更多细节，更加逼真 {#doc_details}


力度层是一大进步，但真实的乐器还有其他音色库能够再现的特性。
下面介绍钢琴的其中两个。


### 松键噪声


松开琴键时，制音器会落回琴弦上，击弦机也会发出轻微的噪声。
有些采样库提供了这些噪声：示例中使用的采样库每个琴键都有一个。

把它们放在第五个乐器“Piano release”中，并以完整的力度范围添加到预设中：

* 在 :guilabel:`循环播放` 行中选择播放模式 ![释音](images/sample_mode_release.png "释音"){.inline}（释音），使采样在松开琴键时播放，
* 使用较长的 :guilabel:`音量包络释音(秒)`（示例中为 5 秒），使噪声能够完整播放，
* 大幅衰减该乐器（示例中为 40 分贝）：噪声必须保持低调。

借助将力度与音量联系起来的默认调制器，音符弹得越重，噪声也越大。

**注意：**“释音”播放模式并未在 sf2 格式中定义，其他播放器可能不支持。
教程“[在乐器中使用自定义释音](tutorials/using-custom-releases-in-an-instrument.md)”介绍了一种适用于任何播放器的方法。


### 没有制音器的琴弦


钢琴最高的琴弦没有制音器（大约是最后一个半八度）：松开琴键后它们仍会继续发声。
因此，在每个力度层的乐器中，为这些琴键（示例中从琴键 89 的 F6 开始）的分区设置更长的 :guilabel:`音量包络释音(秒)`：3 秒而不是 1 秒。

请听键盘中部的短音，然后是最高八度的短音，最后是低音区的一个和弦：

* <a href="files/velocity-layers-without-details.mp3" download>没有这些细节</a>，

![](files/velocity-layers-without-details.mp3)

* <a href="files/velocity-layers-with-details.mp3" download>加入松键噪声和没有制音器的琴弦</a>。

![](files/velocity-layers-with-details.mp3)


### 其他想法


还有一些改进值得尝试：

* 录制每一个琴键，而不是每三个琴键录一个：钢琴的每个琴键都有自己的琴弦，移调后的采样永远不会完全等同于相邻的音符，
* 使用立体声录音，让声音更有宽度，
* 使用工具“[自然释音](manual/soundfont-editor/tools/instrument-tools.md#doc_release)”计算随键盘位置变化的释音时间，
* 使用工具“[基于音符的配置](manual/soundfont-editor/tools/instrument-tools.md#doc_global)”按音区调整音量或滤波器。

同样的方法也适用于其他乐器：轻拨或重拨的吉他弦、在鬼音和重音之间变化的军鼓、从圆润转为明亮铜管音色的小号……
从真实乐器中借鉴的每一个细节，都会让音色库更加生动。
