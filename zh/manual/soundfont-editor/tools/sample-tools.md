在[树状图](manual/soundfont-editor/tree.md)中选中一个或多个采样时，[工具菜单](manual/soundfont-editor/tools/index.md)中包含若干工具，分为 5 个类别：

* [音频处理](#doc_cat_processing)，
* [快速编辑](#doc_cat_fast_editing)，
* [文件](#doc_cat_files)，
* [循环 / 播放](#doc_cat_loop_playback)，
* [立体声采样](#doc_cat_stereo)。

可以通过多选，同时对多个采样使用这些工具。


## 音频处理 {#doc_cat_processing}


### 音量调整 {#doc_volume}

可以通过三种不同的方式改变采样的音量：

* 增加一定的分贝数（正值或负值），
* 将信号乘以一个系数，
* 对信号进行归一化，使其最大值达到最大强度的某个百分比。

![音量调整工具](images/tool_change_volume.png "音量调整工具")


### 外部命令 {#doc_externalcommand}

此工具可以使用外部命令或程序来编辑采样。
不再需要先导出采样、再重新导入 Polyphone，一切都会自动完成：

1. 下载一个 .wav 文件编辑软件，例如 :program:`Audacity` 或 :program:`Sweep`（Linux），
2. 对一个或多个采样使用 :guilabel:`外部命令` 工具，
3. 在 :guilabel:`命令` 字段中填写调用外部软件的命令行（可能需要阅读该软件的文档），其中参数 :command:`{wav}` 是必需的，因为它会被替换为导出文件的路径（例如 :command:`sweep {wav}`），
4. 用外部工具编辑采样，
5. 用外部工具保存修改后的 wav 文件并关闭该工具，
6. 采样会被自动载入音色库。

字段右侧的按钮可以帮助选择可执行文件，:guilabel:`历史` 列表会记住最近使用过的命令。
有两个选项可用：

授权立体声编辑
: 处理立体声采样时，它会与其链接的采样一起导出为一个立体声文件，重新导入文件时两个采样都会更新。

如果可能，替换采样信息
: 用外部软件返回的文件中存储的循环、根音符和校正值替换采样原有的值。
  否则保留现有信息。

![外部命令工具](images/tool_external_command.png "外部命令工具")

**注意：**在此过程中，Polyphone 会导出一个 .wav 文件，并在外部命令运行结束后重新导入同一个文件。如果外部命令创建了其他文件，或将结果另存为其他文件，这些文件会被忽略，Polyphone 会重新导入最初导出的那个文件，而它很可能并未被修改。


### 过滤频率 {#doc_filter}

使用此工具时，会显示一个窗口，其中显示所选采样中包含的频率。
然后可以用鼠标画出一个红色区域。
该区域内的所有内容都会被移除：也就是说，可以手动削弱或移除某些频率。

![过滤频率工具](images/tool_filter_frequencies.png "过滤频率工具")

**注意：**这种滤波器并不常见，因为它会生硬地切除频率。
此工具可用于设计“砖墙”滤波器，但在大多数情况下，你可能更想使用[均衡器](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq)。


### 移调 {#doc_transpose}

该工具会要求输入以半音为单位的移位量，然后对声音进行重采样以改变音高。

![移调工具](images/tool_transpose_smpl.png "移调工具")


## 快速编辑 {#doc_cat_fast_editing}


### 从乐器加载参数 {#doc_load_from_instruments}

乐器层级的参数可以覆盖或偏移采样层级定义的参数。
使用此工具，可以根据使用这些采样的乐器中的定义，修正或补全采样参数：

* 用乐器层级定义的覆盖值修改采样的根音符，
* 用“调音(半音)”参数偏移采样的根音符，
* 用“调音(音分)”参数偏移采样的音高校正值，
* 用循环偏移参数设定采样的循环起点和终点。

![从乐器加载参数的工具](images/tool_load_from_instruments.png "从乐器加载参数的工具")


### 移动参数 {#doc_shift_parameter}

此工具可以对所有选中的采样同时编辑以下某个参数（加上一个值或乘以一个值）：

* 根音符，
* 校正，
* 循环起点，
* 循环终点。

![快速编辑工具](images/tool_shift_parameter.png "快速编辑工具")


## 文件 {#doc_cat_files}


### WAV 导出 {#doc_wavexport}

选择目标目录后，所有选中的采样都会导出为 .wav 文件。


## 循环 / 播放 {#doc_cat_loop_playback}


### 自动循环 {#doc_autoloop}

软件会在采样中寻找两个相距较远、适合创建循环的位置。
如有必要，会通过交叉淡入淡出复制一小段声音，使衔接处的过渡尽可能平滑。
对于立体声采样，两侧会创建相同的循环。

如果失败，会弹出一条警告消息，提示找不到任何循环（采样可能太短或太不稳定）。

**警告：**此工具会修改原始采样。
可以通过“[撤销](manual/soundfont-editor/toolbar.md#doc_edit)”功能撤销修改，但如果保存了文件并关闭了 Polyphone，就无法恢复了。

**注意：**默认情况下，乐器中的循环是关闭的。
要听到循环，必须在[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)的 :guilabel:`全局` 列或各个音符列中，在 :guilabel:`循环播放` 参数行选择 ![循环开启](images/loop_on.png "循环开启"){.inline}。
![循环开启](images/loop_on.png "循环开启"){.inline} 表示开启，![循环关闭](images/loop_off.png "循环关闭"){.inline} 表示关闭。
![循环开启并播放到结尾](images/loop_on_end.png "循环开启并播放到结尾"){.inline} 表示开启循环，并在松开琴键后将采样播放到结尾。


### 交叉淡入淡出循环 {#doc_crossfadeloop}

交叉淡入淡出工具可以看作[自动循环工具](#doc_autoloop)的手动版本。首先，需要在[波形图](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph)中将循环的起点和终点放在一个稳定区域附近。然后，此工具会从这些位置出发，通过一个时长（以毫秒为单位）可配置的交叉淡入淡出来创建循环。因此一切都是可配置的，但在寻找循环的起点和终点时需要注意：

* 在两个位置上，声音的谐波必须一致，以免在过渡时听到类似黑胶唱片划痕的效果，
* 各个谐波的相位也必须一致，以免在交叉淡入淡出期间出现频率抵消，这会严重改变乐器的音色，
* 尽可能让循环终点远离起点，以避免声音过于“合成感”。

一个技巧是关注波形穿过 x 轴（值为 0）的位置。

![交叉淡入淡出循环工具](images/tool_crossfade_loop.png "交叉淡入淡出循环工具")


### 移除开头空白 {#doc_removeblank}

采样开始之前的所有空白区域都会被删除，从而缩短听到声音之前的时间。
对于立体声采样，两侧会删除相同的长度。

![移除开头空白](images/remove_blank.png "移除开头空白")

**注意：**此工具可以在每次导入采样时自动应用，相应选项位于软件[设置](manual/settings.md#doc_general)中。


### 裁剪至循环终点 {#doc_trimloop}

如果所选采样定义了循环，软件会剪掉循环终点之后的声音（在末尾保留一小段余量）。

![裁剪至循环终点](images/trim_to_loop.png "裁剪至循环终点")

**注意：**根据软件[设置](manual/settings.md#doc_general)，此工具可以在每次导入采样时自动应用。


## 立体声采样 {#doc_cat_stereo}


### 平衡调整 {#doc_balance}

如果采样与另一个采样相链接（立体声），软件可以使两个采样的平均强度相等，从而调整平衡。
这样在[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)中精确设置采样的声像就会更容易。
此工具不能用于单声道采样。


### 查找链接 {#doc_link}

在选中的采样中，名称只有最后一个字母不同（L&nbsp;/&nbsp;R）的采样会被自动链接（名称中的空格以及字符 ``-``、``_``、``(``、``)``、``[``、``]``、``{``、``}`` 和 ``|`` 会被忽略）。
例如，如果音色库中有两个采样分别名为 ``bassoon45L`` 和 ``bassoon45R``，那么 ``bassoon45L`` 会被识别为 ``bassoon45R`` 的左声道部分，``bassoon45R`` 会被识别为 ``bassoon45L`` 的右声道部分。

有了这些信息，将这些采样添加到乐器中时就会使用正确的声像。
如果在[设置](manual/settings.md#doc_general)中启用了立体声编辑，还可以在乐器[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)中同时编辑左右两部分。

**警告：**如果名称不符合上述规则，此工具可能会破坏已有的链接。
会有一条消息列出未找到匹配项的采样。


### 取消链接 {#doc_unlink}

所有选中的已链接（立体声）采样都会被取消链接。

这样，所有采样都会被当作单声道声音处理，添加到乐器中时它们的声像都会相同。
如果在[设置](manual/settings.md#doc_general)中启用了立体声编辑，它将不再适用。
