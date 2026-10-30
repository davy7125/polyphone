可以通过命令行运行 Polyphone，将音色库转换为 [sf2](#doc_sf2)、[sf3](#doc_sf3)、[sfz](#doc_sfz)、[csv](#doc_csv) 或 [dls](#doc_dls) 格式。转换支持的文件格式有 [sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)、[sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) 和 [organ](manual/annexes/the-different-soundfont-formats.md#doc_organ)。

还可以使用一个选项以[合成器模式](#doc_synth)打开 Polyphone。

也可以将文件和目录作为参数传入，以便在图形界面中打开它们，例如 :command:`polyphone file1.sf2 file2.sf3 /path/to/directory`（目录会在[音色库浏览器](manual/soundfont-explorer.md)中打开）。
另外还有一些[其他选项](#doc_other)，用于重置配置或显示帮助。

转换时，加载和写入过程会记录在控制台中。
如果转换成功，返回码为 0；如果失败（输入文件不存在、文件无法读取或写入、sf2、sf3 和 dls 格式的输出文件已存在等），则返回其他值。


## 转换为 sf2 {#doc_sf2}


### 命令行


```
polyphone -1 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。


### 参数


-1
: 模式“1”表示“转换为 sf2”。

-d
: 输入文件转换后的输出目录。
  此参数是可选的，默认与输入文件位于同一目录。

-o
: 转换后文件的输出名称，不含扩展名（会自动添加扩展名“.sf2”）。
  此参数是可选的，默认与输入文件同名。


### 示例


```
polyphone -1 -i /path/to/file.sfArk
```


## 转换为 sf3 {#doc_sf3}


### 命令行


```
polyphone -2 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。


### 参数


-2
: 模式“2”表示“转换为 sf3”。

-d
: 输入文件转换后的输出目录。
  此参数是可选的，默认与输入文件位于同一目录。

-o
: 转换后文件的输出名称，不含扩展名（会自动添加扩展名“.sf3”）。
  此参数是可选的，默认与输入文件同名。

-c
: 转换配置。
  可以指定压缩质量：“0”为低，“1”为中，“2”为高。
  此参数是可选的，默认为“1”（中等质量）。


### 示例


```
polyphone -2 -i /path/to/file.sf2 -c 2
```


## 转换为 sfz {#doc_sfz}


### 命令行


```
polyphone -3 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。


### 参数


-3
: 模式“3”表示“转换为 sfz”。

-d
: 输入文件转换后的输出目录。
  此参数是可选的，默认与输入文件位于同一目录。

-o
: 转换后文件的输出名称，不含扩展名（会自动添加扩展名“.sfz”）。
  此参数是可选的，默认与输入文件同名。

-c
: 转换配置，由 3 个以字符“|”分隔的值组成。此参数是可选的，默认为“0|0|0”。
  
  * 如果每个预设都要以其预设编号作为前缀，第一个值为“1”，否则为“0”。
  * 如果要为每个库创建一个目录，第二个值为“1”，否则为“0”。
  * 如果要使用 General MIDI 分类来整理预设，第三个值为“1”，否则为“0”。


### 示例


```
polyphone -3 -i /path/to/file.sf3 -c '0|1|1'
```

转换时会创建一个目录（以输出名称命名），其中包含 sfz 文件和一个 :file:`samples` 子目录。如果该目录已存在，会在其名称后加上一个数字。


## 转换为 .csv {#doc_csv}


### 命令行


```
polyphone -4 -i <file/to/convert> -d <output/directory> -o <output_file_name> -c <configuration>
```

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。


### 参数


-4
: 模式“4”表示“转换为 .csv”。

-d
: 输入文件转换后的输出目录。
  此参数是可选的，默认与输入文件位于同一目录。

-o
: 转换后文件的输出名称，不含扩展名（会自动添加扩展名“.csv”）。
  此参数是可选的，默认与输入文件同名。

-c
: 转换配置。
  可以使用选项“raw”导出原始值（即其在音色库文件中的存储形式），而不是带有可测量物理单位的值。例如，导出的释音时间为 0.491（秒），其原始形式则为 -1229。默认导出带物理单位的值。


### 示例


```
polyphone -4 -i /path/to/file.sf2 -c 'raw'
```

转换时会创建一个目录（以输出名称命名），其中包含 :file:`samples.csv` 文件，以及每个乐器和每个预设各一个文件。如果该目录已存在，会在其名称后加上一个数字。


## 转换为 dls {#doc_dls}


### 命令行


```
polyphone -6 -i <file/to/convert> -d <output/directory> -o <output_file_name>
```

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。


### 参数


-6
: 模式“6”表示“转换为 dls”。

-d
: 输入文件转换后的输出目录。
  此参数是可选的，默认与输入文件位于同一目录。

-o
: 转换后文件的输出名称，不含扩展名（会自动添加扩展名“.dls”）。
  此参数是可选的，默认与输入文件同名。


### 示例


```
polyphone -6 -i /path/to/file.sf2
```

文件以 DLS 2 级格式写入，转换的限制见[这里](manual/annexes/the-different-soundfont-formats.md#doc_dls)。


## 合成器模式 {#doc_synth}


### 命令行


```
polyphone -s -i <file/to/read> -c <configuration>
```

**注意：**在 Windows 上，需要写出 Polyphone 的完整路径，而不只是 :command:`polyphone`，例如 :command:`C:/Program files/Polyphone/polyphone.exe`。

![以合成器模式选项打开的 Polyphone](images/synth_mode.png "以合成器模式选项打开的 Polyphone")


### 参数


-s
: 模式“s”表示“以合成器模式打开 Polyphone”。

-c
: 合成器配置，由 3 个以字符“|”分隔的值组成。此参数是可选的，默认为“0|off|off”。之后可以通过图形界面修改这些参数。
  
  * 第一个值是演奏时要监听的 MIDI 通道编号（1 到 16 之间）。值为 0 或“all”表示使用所有通道，并将其作为一个通道处理。值为 -1 或“each”表示分别监听每个通道，例如这是[播放 MIDI 文件](tutorials/read-midi-file-with-soundfont.md)的前提条件。
  * 第二个值为“off”或“on”，用于禁用或启用同时选择多个预设进行演奏。
  * 第三个值为“off”、“on”或“toggle”，用于禁用或启用通过与预设编号对应的琴键来选择预设（例如，琴键 0 激活预设 0）。


### 示例


```
polyphone -s -i /path/to/file.sf2 -c '1|on|toggle'
```


## 其他选项 {#doc_other}


-h
: 显示简短的帮助信息。

-r
: 重置 Polyphone 的配置（设置、窗口位置和大小、历史记录等）。
  出现问题时，这可能会有帮助。

```
polyphone -r
```
