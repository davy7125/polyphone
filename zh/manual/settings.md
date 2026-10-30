可以从[主屏幕](manual/index.md)或[菜单](manual/menu.md#doc_shortcuts)（快捷键 :kbd:`Ctrl`+:kbd:`,`）打开软件设置。
设置分为五个部分：

* [通用](#doc_general)
* [界面](#doc_interface)
* [音频](#doc_sound)
* [键盘](#doc_keyboard)
* [在线仓库](#doc_repository)

所有修改都会立即保存。


## 通用 {#doc_general}


:guilabel:`通用` 部分可以修改以下参数：

音频后端
: 指定用于播放声音的音频系统和设备。
  可用的系统取决于操作系统（例如 Windows 上的 WASAPI 和 ASIO，Linux 上的 ALSA 和 JACK，macOS 上的 Core Audio）。
  选择“-”会关闭音频输出。

缓冲大小
: 指定音频缓冲区的大小。
  缓冲区小则延迟低，但声音可能会出现爆音。
  选择 :guilabel:`默认` 值时，大小取决于音频系统。

复音数
: 同时播放的复音的最大数量，范围为 64 到 4096（默认为 1024）。
  一个复音即一个正在播放的采样：一个音符可以使用多个复音，例如立体声采样使用两个。
  超出此数量时，音符会淡出：首先是已松开的音符，然后是最轻的音符。
  刚开始的音符以及按住的音符中最高和最低的音符会尽可能长时间地保留。

处理器负载
: 计算复音可占用的最大处理器时间比例，范围为 20% 到 100%（默认为 60%）。
  超出此比例时，音符会以同样的方式淡出。
  值越高，可同时演奏的音符越多，但声音可能会出现爆音。

MIDI 输入
: 指定控制合成器和虚拟键盘的 MIDI 输入。

此外，还可以勾选以下选项：

立体声编辑：修改链接的采样
: 编辑立体声采样的一侧时，另一侧会自动进行相同的编辑。
  编辑立体声乐器分区时也是如此：对应另一声道的分区（具有相同的音符范围和力度范围）会进行相同的编辑。

采样导入：裁剪至循环终点
: 导入采样时，位于循环终点之后的数据会被自动移除（效果与“[裁剪至循环终点](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)”工具相同）。

采样导入：移除开头空白
: 导入采样时，位于起音开始之前的所有空白区域都会被自动移除（效果与“[移除开头空白](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)”工具相同）。


![设置：通用](images/settings_general.png "设置：通用")


## 界面 {#doc_interface}


此部分提供与软件界面相关的选项：

语言
: 可以在这里更改语言。

音符名格式
: 音符可以按编号命名（遵循 MIDI 规范），也可以根据中央 C 的名称（C3、C4 或 C5）命名。
  音符名称可以使用升号或降号。

单一实例
: 在同一个窗口中打开所有文件。

启动视图
: 选择打开音色库时显示的视图，可选项如下：:guilabel:`通用`（默认）、:guilabel:`采样`、:guilabel:`采样 - 第一个采样`、:guilabel:`乐器`、:guilabel:`乐器 - 第一个乐器 - 表格`、:guilabel:`乐器 - 第一个乐器 - 范围`、:guilabel:`乐器 - 第一个乐器 - 包络`、:guilabel:`预设`、:guilabel:`预设 - 第一个预设 - 表格`、:guilabel:`预设 - 第一个预设 - 范围`。对于第一个乐器或预设，最后一部分指定显示的页面：[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)、[范围编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)或[包络编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope)。

分区排序
: 可以在这里更改乐器或预设中分区的排列顺序：:guilabel:`按音符范围`、:guilabel:`按力度范围`、:guilabel:`按字母顺序` 或 :guilabel:`无`（按创建顺序）。

样式
: 界面元素的样式。
  :guilabel:`Fusion` 样式为默认样式，根据操作系统还可能提供其他样式（Adwaita、Windows、macOS……）。

颜色
: 提供多种预设配色方案，用于自定义 Polyphone 的外观。
  也可以手动选择文本、窗口、按钮、选择以及列表的颜色。
  只有在使用 :guilabel:`Fusion` 和 :guilabel:`Windows` 样式时才能更改颜色。

窗口边框
: 显示或隐藏窗口边框，以节省屏幕空间（macOS 上不可用）。

装饰
: 可以关闭某些列表中的背景装饰，以提高可读性。

对语言、音符名格式、样式、颜色、窗口边框或装饰的修改，会在下次启动软件时生效。


![设置：界面](images/settings_interface.png "设置：界面")


## 音频 {#doc_sound}


:guilabel:`音频` 部分可以修改合成器的混响（电平、房间大小、阻尼和宽度）和合唱（电平、深度和频率），以及总音量。
如果连接了外部键盘，可以用音量 MIDI 控制器改变总音量。

由于修改滤波器截止频率的默认调制器在 sf2 规范的 2.01 版和 2.04 版之间发生了变化，可以在这里定义使用哪个版本：

* :guilabel:`SoundFont 2.01`：使用“当力度小于 64 时，根据力度降低滤波器频率”这一规则（在力度 63 和 64 之间可以听到明显且有问题的跳变）；
* :guilabel:`SoundFont 2.04`（默认）：使用“根据力度降低滤波器频率”这一规则（不再有跳变）；
* :guilabel:`已禁用`：不设置该默认调制器。


![设置：音频](images/settings_sound.png "设置：音频")


## 键盘 {#doc_keyboard}


用于控制[虚拟键盘](manual/soundfont-editor/toolbar.md#doc_keyboard)的电脑键盘按键在 :guilabel:`键盘` 部分中定义。
可以通过 :guilabel:`起始 C` 设置逐个八度地升高或降低音高。

对于更专业的音乐人，可以在这里修改调音：

参考音高
: 中央 C 上方的 A（音符 69）的频率，范围为 390 到 470 Hz（默认为 440 Hz）。

律制
: 演奏时可以使用平均律以外的律制。
  选择某种律制后，还可以选择作为该律制参考的音（其主音）。

参考音高和律制也可以通过 MIDI RPN 消息（master fine tuning、tuning program select 和 tuning bank select）进行修改。


![设置：键盘](images/settings_keyboard.png "设置：键盘")


## 在线仓库 {#doc_repository}


可以在这里输入账户的电子邮件地址和密码，将 Polyphone 与该账户关联；[高级账户](premium)可以解锁整个[在线仓库](manual/online-repository.md)。
连接成功后，Polyphone 每次启动时都会自动连接到该账户，直到单击 :guilabel:`退出账户` 按钮为止。

从[在线仓库](manual/online-repository.md)下载音色库时所用的目录也可以在这里修改。


![设置：仓库](images/settings_repository.png "设置：仓库")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface