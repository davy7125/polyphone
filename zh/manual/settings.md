可以从[主屏幕](manual/index.md)或[菜单](manual/menu.md#doc_shortcuts)（快捷键 :kbd:`Ctrl`+:kbd:`,`）打开软件设置。
设置分为六个部分：

* [通用](#doc_general)
* [界面](#doc_interface)
* [音频](#doc_sound)
* [键盘](#doc_keyboard)
* [AI助手](#doc_ai_assistant)
* [在线仓库](#doc_repository)

所有修改都会立即保存。


## 通用 {#doc_general}


:guilabel:`通用` 部分可以修改以下参数：

音频后端
: 指定用于播放声音的音频系统和设备。
  可用的系统取决于操作系统（例如 Windows 上的 WASAPI 和 ASIO，Linux 上的 ALSA 和 JACK，macOS 上的 Core Audio）。
  选择“-”会关闭音频输出。

音频输入
: 指定[录音机](manual/soundfont-editor/toolbar.md#doc_recorder)在 :guilabel:`麦克风 → wav` 模式下录制的设备，仅在录音机以此模式显示时才打开。
  ASIO 不能用于输入。

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

采样导入：移除开头空白
: 导入采样时，位于起音开始之前的所有空白区域都会被自动移除（效果与自动检测背景噪声的“[裁剪](manual/soundfont-editor/tools/sample-tools.md#doc_trim)”工具相同）。

采样导入：移除末尾空白
: 导入采样时，位于声音末尾的空白区域会被自动移除，并替换为短淡出（效果与自动检测背景噪声的“[裁剪](manual/soundfont-editor/tools/sample-tools.md#doc_trim)”工具相同）。


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

恢复标签页
: 启动时按相同顺序重新打开关闭 Polyphone 时已打开的音色库和目录，并保持显示[主屏幕](manual/index.md#doc_home)。在此期间被删除、重命名或移动的项目将被跳过。

通知
: 在[主屏幕](manual/index.md#doc_home)上显示网站发布的[通知](manual/index.md#doc_notifications)（警报、新版本、公告、支持呼吁）。禁用此选项后，不会下载任何内容。

启动视图
: 选择打开音色库时显示的视图，可选项如下：:guilabel:`通用`（默认）、:guilabel:`采样`、:guilabel:`采样 - 第一个采样`、:guilabel:`乐器`、:guilabel:`乐器 - 第一个乐器 - 表格`、:guilabel:`乐器 - 第一个乐器 - 范围`、:guilabel:`乐器 - 第一个乐器 - 包络`、:guilabel:`乐器 - 第一个乐器 - 音色`、:guilabel:`预设`、:guilabel:`预设 - 第一个预设 - 表格`、:guilabel:`预设 - 第一个预设 - 范围`。对于第一个乐器或预设，最后一部分指定显示的页面：[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)、[范围编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_range)、[包络编辑器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope)或[音色视图](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_timbre)。

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
  没有边框时，拖动虚拟键盘、录音机和 MIDI 播放器窗口的横幅（其中显示窗口标题和关闭按钮）即可移动窗口，拖动窗口边缘即可调整其大小。

装饰
: 可以关闭某些列表中的背景装饰，以提高可读性。

对语言、音符名格式、样式、颜色、窗口边框或装饰的修改，会在下次启动软件时生效。


![设置：界面](images/settings_interface.png "设置：界面")


## 音频 {#doc_sound}


:guilabel:`音频` 部分可以修改合成器的混响（电平、房间大小、阻尼和宽度）和合唱（电平、深度和频率），以及总音量。
如果连接了外部键盘，可以用音量 MIDI 控制器改变总音量。
:guilabel:`混响` 和 :guilabel:`合唱` 左侧的指示灯用于开启或关闭各个效果，且不会丢失其设置。
也可以一边演奏一边在[虚拟键盘](manual/soundfont-editor/toolbar.md#doc_keyboard_effects)窗口中更改这些设置：更改是渐进的，不会打断正在演奏的音符。

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


## AI助手 {#doc_ai_assistant}


在 :guilabel:`AI助手` 部分中，可以允许支持 Model Context Protocol（MCP）的 AI 助手（例如 Claude）浏览、编辑和分析在 Polyphone 中打开的音色库。
助手所做的修改会立即显示在编辑器中，并且可以像其他修改一样撤销。

允许 AI 助手访问已打开的音色库
: 启动 Polyphone 的 MCP 服务器（默认关闭）。
  下方的一行显示服务器的状态和地址，例如“服务器已启动，地址：http://127.0.0.1:8642/mcp”。
  服务器只接受来自本机的连接。

端口
: 服务器的端口，默认为 8642。
  如果该端口已被其他软件占用，可以选择另一个端口。

将 Polyphone 添加到 Claude Code 的命令
: 在终端中运行此命令后，Claude Code 即可使用 Polyphone。
  :guilabel:`复制` 按钮可将其复制到剪贴板。

AI 可用的功能
: 助手可以调用的功能列表，分为四类：:guilabel:`浏览`、:guilabel:`编辑`、:guilabel:`工具` 和 :guilabel:`声音分析`。
  这些描述是供助手阅读的，使用英文编写。

教程“[用 Claude 编辑音色库](tutorials/edit-soundfonts-with-claude.md)”介绍了如何将 Claude 连接到 Polyphone，并给出了请求示例。


![设置：AI助手](images/settings_ai_assistant.png "设置：AI助手")


## 在线仓库 {#doc_repository}


可以在这里输入账户的电子邮件地址和密码，将 Polyphone 与该账户关联；[高级账户](premium)可以解锁整个[在线仓库](manual/online-repository.md)。
连接成功后，Polyphone 每次启动时都会自动连接到该账户，直到单击 :guilabel:`退出账户` 按钮为止。

从[在线仓库](manual/online-repository.md)下载音色库时所用的目录也可以在这里修改。


![设置：仓库](images/settings_repository.png "设置：仓库")



*[JACK]: JACK Audio Connection Kit
*[ASIO]: Audio Stream Input/Output
*[MIDI]: Musical Instrument Digital Interface