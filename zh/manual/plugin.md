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
打开音色库时会读取整个音色库，sf3 文件的采样会被解压：演奏期间不会从磁盘读取任何内容。
当插件的多个实例演奏同一个文件时（例如每条音轨一个实例），该文件只会被读取一次并在内存中只保留一份。
如果文件发生了变化，例如在 Polyphone 中重新保存后，它会被重新读取。


## 演奏模式 {#doc_modes}


预设列表上方的三个按钮用于选择预设的演奏方式：

:guilabel:`单一预设`
: 在列表中选中的预设会在全部 16 个 MIDI 通道上演奏。
  预设会连同其库编号和预设编号一起显示，例如“000:024 Guitar”。

:guilabel:`16 通道`
: 每个 MIDI 通道演奏各自的预设，在该通道所在的行中选择。
  选择 :guilabel:`程序变更` 时，该通道演奏由其接收到的库选择和程序变更消息所选定的预设，就像 General MIDI 合成器一样：此时通道 10 使用库 128，用于鼓组。
  正在演奏的预设显示在每一行的右侧。
  这种模式便于演奏为多种乐器编排的 MIDI 文件。

:guilabel:`MPE`
: 选中的预设由 MPE 控制器（MIDI Polyphonic Expression）演奏：每个音符都有自己的通道，因此也有各自的弯音、压力和音色（CC 74）。
  在控制器发送其配置（RPN 6）之前，使用一个由 15 个通道组成的下部区域。
  区域的主通道作用于该区域的所有音符：它的弯音会叠加到每个音符的弯音上，它的控制器会应用于所有音符。
  弯音范围对音符为 48 个半音，对主通道为 2 个半音，可由控制器更改（RPN 0）。


![“16 通道”模式下的插件](images/plugin_channels.png "“16 通道”模式下的插件")


## 参数 {#doc_parameters}


右侧的参数就是 Polyphone 合成器的参数，在 Polyphone 的[音频](manual/settings.md#doc_sound)和[键盘](manual/settings.md#doc_keyboard)设置中也可以找到：

:guilabel:`合成器`
: :guilabel:`增益`（-50 到 +50 dB）、:guilabel:`移调`（-24 到 +24 个半音，鼓组不移调）、:guilabel:`调音`（A4 的参考音高，400 到 480 Hz）和 :guilabel:`律制`，律制的微调相对于在列表下方选择的调。

:guilabel:`混响`
: :guilabel:`电平`、:guilabel:`大小`、:guilabel:`宽度` 和 :guilabel:`阻尼`。

:guilabel:`合唱`
: :guilabel:`电平`、:guilabel:`深度` 和 :guilabel:`频率`。

所有这些参数都可以由宿主进行自动化控制。


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

:guilabel:`将音色库存储在工程中`
: 音色库会保存在宿主的工程中，这样即使没有该文件，也可以在另一台电脑上打开该工程。
  sfz 文件无法这样做，因为它的采样是其他文件。

:guilabel:`通道和效果使用各自的输出`
: 除主输出外，输出“通道 1”到“通道 16”分别接收各自通道中不带效果的音符，输出“效果”接收发送到混响和合唱的部分。
  如果某个通道的输出未在宿主中启用，该通道仍保留在主输出中。
  禁用此选项时（默认），所有声音都混合到主输出中。


## 在工程中使用 {#doc_project}


宿主的工程会保存参数、所演奏的预设、复音数以及音色库的路径（如果音色库存储在工程中，则保存其内容）。
打开工程时，会先读取音色库，再恢复其余内容，以便离线导出可以立即开始。

无论宿主的缓冲大小如何，音符都会准确地在其 MIDI 消息所在的位置开始。
在离线导出期间，复音数永远不会因处理器负载而受到限制。
