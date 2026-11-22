Polyphone 还提供插件版本，可用于数字音频工作站（DAW）以及其他音频插件宿主。
该插件使用 Polyphone 的合成器演奏 sf2、sf3、sfz 和 dls 格式的音色库：同一个音色库在宿主中和在编辑器中听起来完全一样。
插件只演奏音色库，不编辑音色库：修改需要在 Polyphone 中进行，然后在插件中重新打开该音色库。

插件在 Linux、Windows 和 macOS 上提供 VST3、CLAP 和 LV2 格式，在 macOS 上还提供 Audio Unit（AU）格式。


![Polyphone 插件](images/plugin.png "Polyphone 插件")


## 安装 {#doc_install}


插件按操作系统分别打包为压缩包，可在“[Software](software)”部分获取。
将宿主所用格式的插件复制到宿主查找插件的文件夹中，然后在宿主中重新扫描插件。

Linux
: :file:`Polyphone.vst3` 复制到 :file:`~/.vst3/`，\
  :file:`Polyphone.clap` 复制到 :file:`~/.clap/`，\
  :file:`Polyphone.lv2` 复制到 :file:`~/.lv2/`。

macOS
: :file:`Polyphone.vst3` 复制到 :file:`~/Library/Audio/Plug-Ins/VST3/`，\
  :file:`Polyphone.clap` 复制到 :file:`~/Library/Audio/Plug-Ins/CLAP/`，\
  :file:`Polyphone.component` 复制到 :file:`~/Library/Audio/Plug-Ins/Components/`（Audio Unit 格式，供 Logic Pro 和 GarageBand 使用），\
  :file:`Polyphone.lv2` 复制到 :file:`~/Library/Audio/Plug-Ins/LV2/`。

Windows
: :file:`Polyphone.vst3` 复制到 :file:`C:\Program Files\Common Files\VST3`，\
  :file:`Polyphone.clap` 复制到 :file:`C:\Program Files\Common Files\CLAP`，\
  :file:`Polyphone.lv2` 复制到 :file:`%APPDATA%\LV2`。

在 macOS 上，这些插件未经过公证（notarization）：如果宿主拒绝加载它们，请移除已下载文件的隔离标记，例如：

```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3
```


## 音色库 {#doc_soundfont}


窗口顶部显示音色库的名称及其路径。
可以通过以下方式打开音色库：

* 使用 :guilabel:`打开...` 按钮，
* 使用其右侧的箭头，其中列出最近打开的 10 个音色库，
* 将文件拖放到窗口上。

sfz 和 dls 文件会像 Polyphone 导入它们时一样进行转换，sfz 文件的采样就是它所引用的音频文件。
打开音色库时，会立即读取其结构：乐器会列出并可以演奏。
随后在后台读取其采样，期间名称前会显示一个加载图标（其工具提示显示进度）：采样尚未读取的音符不会发声。
已演奏的音符和所选乐器的采样会优先读取，sf3 文件的采样会被解压。
全部读取完成后，演奏期间不会从磁盘读取任何内容；离线导出会等待所有采样读取完成。
当插件的多个实例演奏同一个文件时（例如每条音轨一个实例），该文件只会被读取一次并在内存中只保留一份。
如果文件发生了变化，例如在 Polyphone 中重新保存后，它会被重新读取。


## 演奏模式 {#doc_modes}


乐器（音色库的预设）列表上方的三个按钮用于选择乐器的演奏方式：

:guilabel:`单一乐器`
: 在列表中选中的乐器会在全部 16 个 MIDI 通道上演奏。
  乐器会连同其库编号和程序编号一起显示，例如“000:024 Guitar”。

:guilabel:`16 通道`
: 每个 MIDI 通道演奏各自的乐器，在该通道所在的行中选择。
  选择 :guilabel:`程序变更` 时，该通道演奏由其接收到的库选择和程序变更消息所选定的乐器，就像 General MIDI 合成器一样：此时通道 10 使用库 128，用于鼓组。
  正在演奏的乐器显示在每一行的右侧。
  这种模式便于演奏为多种乐器编排的 MIDI 文件。

:guilabel:`MPE`
: 选中的乐器由 MPE 控制器（MIDI Polyphonic Expression）演奏：每个音符都有自己的通道，因此也有各自的弯音、压力和音色（CC 74）。
  在控制器发送其配置（RPN 6）之前，使用一个由 15 个通道组成的下部区域。
  区域的主通道作用于该区域的所有音符：它的弯音会叠加到每个音符的弯音上，它的控制器会应用于所有音符。
  弯音范围对音符为 48 个半音，对主通道为 2 个半音，可由控制器更改（RPN 0）。


![“16 通道”模式下的插件](images/plugin_channels.png "“16 通道”模式下的插件")


## 参数 {#doc_parameters}


这些参数就是 Polyphone 合成器的参数，在 Polyphone 的[音频](manual/settings.md#doc_sound)和[键盘](manual/settings.md#doc_keyboard)设置中也可以找到。
:guilabel:`增益`（-24 到 +24 dB）位于标题栏，其他参数归组显示在右侧：

:guilabel:`音符`
: 第一行列表中的演奏模式（参见[演奏模式](manual/plugin.md#doc_play_modes)）：:guilabel:`复音`（默认）、:guilabel:`单音` 或 :guilabel:`连奏`，连奏的滑音在其右侧设置。
  :guilabel:`参考音高`：从 A#3 到 A4 的一个音及其频率（A4 为 400 到 480 Hz，默认为 440 Hz），无论律制如何，该音都以此频率发声。
  :guilabel:`移调`：-24 到 +24 个半音，鼓组不移调。

:guilabel:`力度`
: 设置对音符力度的响应，每项设置都以图形显示（力度从左到右为 0 到 127）：
  
  * :guilabel:`滤波器截止`：根据力度将滤波器截止频率降低最多 2 个八度的默认调制器，与 Polyphone 的[音频](manual/settings.md#doc_sound)设置中相同。SoundFont 格式的 2.04 版本（默认）在力度越小时降低得越多。2.01 版本在力度 64 处有一个跳变，这是 2.04 版本修正的错误：它是为补偿该跳变而制作的旧音色库的兼容模式。选择 :guilabel:`禁用` 时，截止频率不随力度变化。对于有自己的[默认调制器](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_default_mod)的音色库，此设置无效。
  * :guilabel:`曲线`：应用于所接收音符力度的曲线，中间为线性（默认，双击可恢复），向左为凹形（力度更柔和：需要更重的触键才能达到高力度），向右为凸形（力度迅速升高）。

:guilabel:`律制与音阶`
: Polyphone 的律制与音阶（按类别排列），其后是自定义律制，并移调到所选的 :guilabel:`调性`。
  右侧的键盘以音分显示每个音的微调，编辑方式与 Polyphone 的[键盘设置](manual/settings.md#doc_keyboard)相同：鼠标滚轮或上下拖动，双击恢复为 0，右键菜单可切换音是否使用或将调性移到该音上。编辑过的律制会成为自定义律制。
  音阶不使用的音不会发声，除非勾选 :guilabel:`全部演奏`。

:guilabel:`混响`
: :guilabel:`电平`、:guilabel:`大小`、:guilabel:`宽度` 和 :guilabel:`阻尼`。

:guilabel:`合唱`
: :guilabel:`电平`、:guilabel:`深度` 和 :guilabel:`频率`。

:guilabel:`律制与音阶`、:guilabel:`混响` 和 :guilabel:`合唱` 分组的标题右侧有一个开关：关闭时，该分组会折叠，其律制或效果不会生效（使用全部音的平均律，无效果），但其设置会保留。
窗口的高度会随展开的分组而变化。

宿主可以自动化控制所有这些参数，但参考音高所在的音（自动化的是 A4 的频率）和自定义律制的微调除外，它们会随工程一起保存。


## 演奏模式 {#doc_play_modes}


:guilabel:`音符` 组第一行的列表选择每个 MIDI 通道中音符的演奏方式：

:guilabel:`复音`
: 多个音同时演奏（默认）。

:guilabel:`单音`
: 一次只演奏一个音：每个音都从起音开始，前一个音迅速停止。

:guilabel:`连奏`
: 一次只演奏一个音：在松开前一个音之前弹奏的音会延续前一个音，不重新起音。
  如果演奏前一个音的分区也包含新的键，其声部会继续使用同一个采样，只改变音高。
  新键的其他采样从其循环的起点开始（没有循环时从采样起点开始），并进行交叉淡化。
  列表右侧的滑条设置滑音：:guilabel:`关`（默认，音高立即改变），或从前一个音开始的音高滑动时长，最长 5 秒。
  在松开前一个音之后弹奏的音从起音开始，与 :guilabel:`单音` 模式相同。

括号中是按住多个键时发声的键：最后按下的键、最高的键或最低的键。
松开正在发声的键而其他键仍被按住时，优先的键的音会再次发声：按住 D 并重复弹 E 即可演奏颤音，在连奏模式下带有滑音。
鼓（音色库 128）和 :guilabel:`MPE` 模式始终为复音。


## 键盘和状态 {#doc_keyboard}


窗口底部的键盘会显示接收到的音符，也可以用鼠标演奏。
键盘下方显示正在发声的复音数量以及插件占用的处理器比例。

:guilabel:`紧急停止` 按钮会停止所有声音并重置控制器，在某个音符卡住时很有用。


## 设置 {#doc_settings}


:guilabel:`设置` 按钮会打开一个菜单，其中包含以下选项：

:guilabel:`复音数`
: 复音的最大数量，从 64 到 4096（默认为 1024）。
  超出此数量时，最轻的音符会淡出。

:guilabel:`键盘`
: 用窗口中的键盘演奏的音符所使用的 MIDI 通道。

:guilabel:`语言`
: 插件的语言，默认为系统语言。
  该选择由插件的所有实例共用。
  提供给宿主的参数名称和输出名称使用加载插件时的语言（使用 LV2 时始终为英语）。

:guilabel:`固定延迟（音符在其准确位置播放）`
: 禁用此选项时（默认），一个音频块中的 MIDI 消息会被一起提前，使该块的第一个音符从块的开头开始：现场演奏的音符平均提前半个缓冲时长发声，但其延迟随它们在宿主缓冲中的位置而变化。
  启用后，每条消息都在其准确位置处理：现场演奏音符的延迟保持不变。
  在离线导出期间，消息始终在其准确位置处理。

:guilabel:`削波到 [-1; 1]`
: 输出被限制在 [-1; 1] 区间内（默认），与 Polyphone 相同。
  不削波时，更大的值会传给宿主，由宿主以浮点数混音：这样可以提高增益而不会饱和，电平在宿主的输出之前调整。

:guilabel:`将音色库存储在工程中`
: 音色库会保存在宿主的工程中，这样即使没有该文件，也可以在另一台电脑上打开该工程。
  sfz 文件无法这样做，因为它的采样是其他文件。

:guilabel:`通道和效果使用各自的输出`
: 除主输出外，输出“通道 1”到“通道 16”分别接收各自通道中不带效果的音符，输出“效果”接收发送到混响和合唱的部分。
  如果某个通道的输出未在宿主中启用，该通道仍保留在主输出中。
  禁用此选项时（默认），所有声音都混合到主输出中。


## 在工程中使用 {#doc_project}


宿主的工程会保存参数、所演奏的乐器、复音数以及音色库的路径（如果音色库存储在工程中，则保存其内容）。
打开工程时，只会先读取音色库的结构，再恢复其余内容：宿主无需等待采样，离线导出也可以立即开始（它会等待尚未读取的采样）。

在离线导出期间以及在实时状态下使用[固定延迟](manual/plugin.md#doc_settings)时，无论宿主的缓冲大小如何，音符都会准确地在其 MIDI 消息所在的位置开始。
在离线导出期间，复音数永远不会因处理器负载而受到限制。
