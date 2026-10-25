要创建[音色库](manual/annexes/the-different-soundfont-formats.md#doc_sf2)，请在[主屏幕](manual/index.md#doc_left)上单击 :guilabel:`新音色库`。
音色库采用 3 层结构，因此创建新音色库需要三个基本步骤：

* [准备采样](#doc_sample)，
* [创建乐器](#doc_instrument)，乐器由采样组成，
* [创建预设](#doc_preset)，预设由乐器组成。


## 准备采样 {#doc_sample}


### 载入采样


准备采样首先要**载入**音频文件（例如 .wav 文件）。
为此，请单击[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)中的 :guilabel:`添加采样`，然后选择一个或多个音频文件（也可以将文件拖放到[树状图](manual/soundfont-editor/tree.md#doc_dragdrop)上）。
采样可以通过以下途径获得：

* 从互联网获取，
* 录制真实乐器，
* 使用专门的软件进行合成。


### 为采样设置循环


接下来，如果希望采样发声的时间比其原本长度更长，可能需要为采样设置**循环**。
例如，你只有一个 1 秒长的长笛采样，却希望它能无限地持续发声。
为此，可以在采样范围内手动或自动设置循环点（起点 + 终点），使循环区域可以重复播放，从而延长采样的发声时间。
虽然可以手动设置循环点，但使用“[自动循环](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)”功能自动设置通常效果更好，而且快得多。

手动设置循环点的步骤：

1. 在[树状图](manual/soundfont-editor/tree.md)中单击要设置循环的采样，
2. 在[采样编辑器](manual/soundfont-editor/editing-pages/sample-editor.md)中，在[波形图](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph)（WAV 显示区域）上随意放置循环的起点和终点，除非已经定义了循环。
  单击左键设置起点，单击右键设置终点。
  应先设置右侧的点（因为右侧的点默认位于位置 0，而左侧的点不能放在右侧的点之后）。
3. 启用 :guilabel:`循环` 选项后，按 :guilabel:`播放` 按钮，
4. 在采样播放的同时调整循环的起点和 / 或终点，直到两个位置之间的过渡尽可能平滑。


**注意：**要在乐器中听到循环，必须在[参数表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)的 :guilabel:`全局` 列或各个音符列中，在 :guilabel:`循环播放` 参数行选择 ![循环开启](images/loop_on.png "循环开启"){.inline}。![循环开启](images/loop_on.png "循环开启"){.inline} 表示开启，![循环关闭](images/loop_off.png "循环关闭"){.inline} 或留空表示关闭。


![循环示意图](images/loop_illustration.png "循环示意图")


### 为采样调音


最后，还需要进行**调音**。
为此，[采样](manual/soundfont-editor/editing-pages/sample-editor.md#doc_player)编辑页面中提供了一个校准工具（正弦）。
对每个采样，方法如下：

1. 开始播放（按 :guilabel:`播放` 按钮），尽量启用 :guilabel:`循环` 选项，
2. 启用 :guilabel:`正弦` 选项，
3. 调整音量滑块，使两个声音都能听得尽可能清楚，
4. 修改 :guilabel:`根音符`，直到两个声音最为接近，
5. 调整 :guilabel:`校正`（以音分为单位），使采样与根音符对准。
   为此，请注意出现的拍频，并使其尽可能慢。

[频率](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq)分析可以作为开始调音时的一个很好的参考。


### 更进一步


有多种工具可以用来编辑采样，例如：

* [移除开头空白](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)，
* 对声音进行[均衡](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq)，
* [移调](manual/soundfont-editor/tools/sample-tools.md#doc_transpose)，
* 对音量进行[归一化](manual/soundfont-editor/tools/sample-tools.md#doc_volume)，
* 为立体声采样[调整平衡](manual/soundfont-editor/tools/sample-tools.md#doc_balance)。


## 创建乐器 {#doc_instrument}


在这一步，假定已经有一个或多个采样可用于创建乐器。


### 创建乐器并添加采样


首先，单击[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)中的 :guilabel:`添加乐器`。
此时需要输入一个名称（如果在树状图中选中了采样，可以直接将它们链接到新乐器）。

否则，可以通过“[拖放](manual/soundfont-editor/tree.md#doc_dragdrop)”将采样添加到乐器中。
在树状图中，你会看到乐器下出现了分区。

**注意：**创建分区时，采样并不会被复制，而是被链接。分区的数量没有限制。


### 分配分区


将采样添加到乐器后，它们会以分区（列）的形式出现在[乐器编辑器的表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)中。
然后需要在表格中修改 :guilabel:`音符范围`，将每个分区放到键盘上的相应位置。
一般来说，最好让分区的范围包含它所代表采样的根音符。
必须覆盖整个键盘（例如，对于一台常见的合成器键盘，从音符 36 到音符 96）。

“[采样自动定位](manual/soundfont-editor/tools/instrument-tools.md#doc_position)”工具可以自动将采样分配到整个键盘上。


### 设置分区


要在包含循环采样的乐器中启用循环播放模式，请在参数表格的 :guilabel:`全局` 列或各个音符列中，在 :guilabel:`循环播放` 参数行选择 ![循环开启](images/loop_on.png "循环开启"){.inline}。

* ![循环开启](images/loop_on.png "循环开启"){.inline} 表示开启，
* ![循环关闭](images/loop_off.png "循环关闭"){.inline} 或留空表示关闭。
* ![循环开启并播放到结尾](images/loop_on_end.png "循环开启并播放到结尾"){.inline} 表示开启循环，并在松开琴键后读取采样的剩余部分。

顾名思义，全局分区可以为整个乐器设置参数。
全局参数只应用于没有指定相同参数的分区。
也就是说，普通分区的参数优先于全局分区的设置。

在这个简单教程中，乐器无需编辑其他参数即可正常工作。
现在应该已经可以用[虚拟键盘](manual/soundfont-editor/toolbar.md#doc_keyboard)演奏了。

不过，为了改进乐器，建议在 :guilabel:`音量包络释音(秒)` 行中输入一个值，以免松开琴键时声音戛然而止。
其他参数在[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)的说明中有介绍。


## 创建预设 {#doc_preset}


在这一步，假定已经有一个或多个乐器可用于创建预设。


### 什么是预设？


预设相当于硬件合成器中的音色（patch）名称。
它从音色库外部可见，由库编号和预设编号标识。
一个音色库可以包含一个或多个预设。
每个预设包含一个或多个乐器，就像乐器包含一个或多个采样一样。


### 创建预设并添加乐器


首先，单击[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)中的 :guilabel:`添加预设`。
此时需要输入一个名称（如果在树状图中选中了乐器，可以直接将它们链接到新预设）。

否则，可以通过“[拖放](manual/soundfont-editor/tree.md#doc_dragdrop)”将乐器添加到预设中。
在树状图中，你会看到预设下出现了一个或多个分区。
通常可能只有一个分区，例如预设只包含一个乐器的情况。

可以创建与乐器数量相同（甚至更多）的预设，每个预设可以包含一个或多个乐器。


### 设置预设


可能并不需要修改预设的参数，因为：

* 创建预设时，库编号和预设编号会自动分配，
* 将乐器添加到预设时，音符范围会根据乐器自动计算。

就本教程而言，音色库已经创建完成！
