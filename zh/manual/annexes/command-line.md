可以通过命令行运行 Polyphone，将音色库转换为 [sf2](#doc_sf2)、[sf3](#doc_sf3)、[sfz](#doc_sfz)、[csv](#doc_csv) 或 [dls](#doc_dls) 格式。转换支持的文件格式有 [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)、[sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) 和 [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ)。

还可以用音色库[播放 MIDI 文件](#doc_render)，并将结果写入 wav 文件。
还可以使用选项以[合成器模式](#doc_synth)打开 Polyphone，或在启动 [MCP 服务器](#doc_mcp)的情况下打开 Polyphone。

也可以将文件和目录作为参数传入，以便在图形界面中打开它们，例如 :command:`polyphone file1.sf2 file2.sf3 /path/to/directory`（目录会在[音色库浏览器](manual/soundfont-explorer.md)中打开）。
如果 Polyphone 已经打开，文件会在正在运行的实例中打开。
另外还有一些[其他选项](#doc_other)，用于重置配置或显示帮助和版本。

使用 :command:`polyphone --help` 可以显示所有选项的列表。
参数的顺序无关紧要。
仍然接受[以前版本](#doc_legacy)的选项。

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。


## 转换 {#doc_conversion}


### 命令行


```
polyphone --convert <格式> <要转换的/文件> -d <输出/目录> -o <输出/文件> -f
```


### 参数


--convert
: 转换格式：“sf2”、“sf3”、“sfz”、“csv”或“dls”，各格式特有的选项见下文。
  如果给出了带扩展名的输出文件，则此参数可以省略，例如 :command:`polyphone file.sf2 -o file.sf3`。

-d, --output-dir
: 输出目录，该目录必须存在。
  此参数是可选的，默认为每个输入文件所在的目录。

-o, --output
: 输出文件，可以带或不带扩展名（会自动添加格式的扩展名）。
  此参数是可选的，默认输出文件与输入文件同名。
  仅有名称时写入输出目录，相对路径从当前目录开始（如果使用了 -d，则从输出目录开始）。
  转换多个文件时不能使用此参数。

-f, --force
: 替换已存在的输出文件。
  如果没有此参数，当输出文件已存在时转换会失败。
  输入文件可以被其转换结果替换，例如 :command:`polyphone --convert sf2 file.sf2 -f` 会重写 :file:`file.sf2`。

一条命令可以转换多个文件，每个输出文件与其输入文件同名。
即使某个文件转换失败，其他文件仍会被转换。


### 示例


```
polyphone --convert sf2 /path/to/file.sfArk
polyphone /path/to/file.sf2 -o /path/to/converted.sf3
polyphone --convert sf3 *.sf2 -d /path/to/directory
```


## 转换为 sf2 {#doc_sf2}


此转换没有特定的选项。

```
polyphone --convert sf2 /path/to/file.dls
```


## 转换为 sf3 {#doc_sf3}


--quality
: 压缩质量：“low”（低）、“medium”（中）或“high”（高）。
  此参数是可选的，默认为“medium”。

```
polyphone --convert sf3 /path/to/file.sf2 --quality high
```


## 转换为 sfz {#doc_sfz}


--preset-prefix
: 在每个预设前加上其预设编号。

--bank-dir
: 为每个库创建一个目录。

--gm-sort
: 使用通用 MIDI 分类对预设排序。

```
polyphone --convert sfz /path/to/file.sf3 --bank-dir --gm-sort
```

转换会创建一个以输出文件命名的目录，其中包含 sfz 文件和子目录 :file:`samples`。
如果该目录已存在，会在其名称后添加一个数字：即使使用 -f，也不会替换任何内容。


## 转换为 .csv {#doc_csv}


--raw
: 导出原始值（即音色库文件中存储的值），而不是带有可测量物理单位的值。
  例如，导出的释音时间将是 0.491（秒），原始形式则为 -1229。
  默认导出带物理单位的值。

```
polyphone --convert csv /path/to/file.sf2 --raw
```

转换会创建一个以输出文件命名的目录，其中包含文件 :file:`samples.csv`，以及每个乐器和每个预设各一个文件。
如果该目录已存在，会在其名称后添加一个数字：即使使用 -f，也不会替换任何内容。


## 转换为 dls {#doc_dls}


此转换没有特定的选项。

```
polyphone --convert dls /path/to/file.sf2
```

文件以 DLS 2 级格式写入，转换的限制见[此处](manual/annexes/the-different-soundfont-formats.md#doc_dls)。


## 渲染 MIDI 文件 {#doc_render}


### 命令行


```
polyphone --render <MIDI/文件> <音色库> -d <输出/目录> -o <输出/文件> -f
```


### 参数


--render
: 要播放的 MIDI 文件的路径（扩展名为 .mid、.midi、.kar 或 .rmi）：使用音色库播放该 MIDI 文件，并将结果写入 wav 文件。

-d, --output-dir
: 写入 wav 文件的输出目录。
  此参数是可选的，默认与 MIDI 文件位于同一目录。

-o, --output
: 输出的 wav 文件，可以带或不带扩展名。
  此参数是可选的，默认 wav 文件与 MIDI 文件同名。

-f, --force
: 替换已存在的 wav 文件。


### 示例


```
polyphone --render /path/to/song.mid /path/to/file.sf2
```

MIDI 文件的所有通道都演奏该音色库，预设由文件中的程序变更（program change）选择（打击乐使用通道 10 上的库 128）。
使用合成器的设置：[音频](manual/settings.md#doc_sound)（混响、合唱、总音量）、[复音数](manual/settings.md#doc_general)和[调音](manual/settings.md#doc_keyboard)。
结果是 48,000 Hz 的立体声 32 位浮点 wav 文件，在最后几个音符的声音消失后结束。
音色库中没有的预设，其音符不会被演奏，并会针对该预设显示警告。
在图形界面中，也可以从[MIDI 播放器](manual/soundfont-editor/toolbar.md#doc_midi_player_settings)导出 MIDI 文件，此时每个通道可以演奏不同的音色库或预设。


## 合成器模式 {#doc_synth}


### 命令行


```
polyphone --synth <要读取的/文件> --channel <通道> --multiple-selection --key-selection <模式>
```

![以合成器模式选项打开的 Polyphone](images/synth_mode.png "以合成器模式选项打开的 Polyphone")


### 参数


-s, --synth
: 以合成器模式打开 Polyphone。

--channel
: 演奏时要监听的 MIDI 通道编号（1 到 16 之间）。
  值“all”表示使用所有通道，并将其作为一个通道处理（默认值）。
  值“each”表示分别监听每个通道，例如这是播放音序器 MIDI 输出的前提条件（也可以直接用[MIDI 播放器](manual/soundfont-editor/toolbar.md#doc_midi_player)播放 MIDI 文件）。

--multiple-selection
: 可以同时选择多个预设进行演奏。

--key-selection
: 值为“on”时，通过与预设编号对应的琴键选择预设（例如，琴键 0 激活预设 0）。
  值为“toggle”时，琴键还可以取消选择其预设。

这些参数是可选的，之后可以通过图形界面修改这些设置。


### 示例


```
polyphone --synth /path/to/file.sf2 --channel 1 --multiple-selection --key-selection toggle
```


## MCP 服务器 {#doc_mcp}


### 命令行


```
polyphone --mcp <要打开的/文件> --mcp-port <端口>
```


### 参数


--mcp
: 即使在[设置](manual/settings.md)中已禁用，也为本次会话启动 MCP 服务器，使 AI 助手能够浏览、编辑和分析已打开的音色库。
  服务器地址会显示在控制台中。
  即使 Polyphone 已经打开，也会打开一个新的 Polyphone 实例。

--mcp-port
: MCP 服务器的端口（此参数隐含 --mcp）。
  此参数是可选的，默认为设置中的端口（8642）。

在图形界面中更改 MCP 服务器的设置会取代这些选项。


### 示例


```
polyphone --mcp-port 8700 /path/to/file.sf2
```


## 其他选项 {#doc_other}


-h, --help
: 显示所有选项的列表。

-v, --version
: 显示 Polyphone 的版本。

-r, --reset-config
: 重置 Polyphone 的配置（设置、窗口布局、历史记录等）。
  出现问题时，这可能会有帮助。

```
polyphone --reset-config
```


## 消息和返回码 {#doc_exit}


加载和写入过程会记录在控制台中，错误会写入错误输出。
消息和帮助使用[设置](manual/settings.md)中选择的语言。

返回码如下：

0
: 成功。

1
: 输入文件或输出目录不存在，或输出文件已存在（未使用 -f）。

2
: 参数无效：未知选项、错误的值、选项不适用于所选模式、不支持的输入文件格式等。

3
: 无法读取输入文件（音色库、MIDI 文件）。

4
: 无法写入输出。

转换多个文件时，返回码为第一次失败的返回码。


## 以前版本的选项 {#doc_legacy}


仍然接受 Polyphone 以前版本的选项：

-1, -2, -3, -4, -6
: 转换为 sf2、sf3、sfz、csv 和 dls，与 --convert 相同。

-5
: 渲染 MIDI 文件，其路径用 -c 给出。

-i
: 输入文件，也可以不使用此选项直接写出。

-c
: 配置，由以字符“|”分隔的值组成：
  
  * 转换为 sf3：压缩质量，“0”为低，“1”为中，“2”为高；
  * 转换为 sfz：3 个值，“1”或“0”，分别对应 --preset-prefix、--bank-dir 和 --gm-sort，例如“0|1|1”；
  * 转换为 csv：“raw”对应 --raw；
  * 渲染 MIDI 文件（-5）：MIDI 文件的路径；
  * 合成器模式：通道（“all”、“each”或 1 到 16，0 等同于“all”，-1 等同于“each”）、多选（“on”或“off”）以及按键选择（“on”、“off”或“toggle”），例如“all|on|toggle”。

```
polyphone -2 -i /path/to/file.sf2 -c 2
polyphone -s -i /path/to/file.sf2 -c 'all|on|toggle'
```
