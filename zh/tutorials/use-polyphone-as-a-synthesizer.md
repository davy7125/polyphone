Polyphone 不仅是一个音色库编辑器，还是一个功能完备的合成器，可以配合 MIDI 键盘演奏。
本教程介绍如何以合成器模式启动 Polyphone。


## 准备工作

首先需要一个**主控键盘**或其他 MIDI 控制器，并通过 **MIDI 转 USB 适配器**将其连接到电脑。

在[这里](software)下载最新版本的 Polyphone，安装后打开程序。在[设置](manual/settings.md#doc_general)中，应该能在 MIDI 输入里找到你的 MIDI 设备。如果找不到，很可能是电脑上没有安装 MIDI 适配器的驱动程序（请先解决这个问题再继续）。

同样在设置中，检查音频输出是否正确。

## 以合成器模式打开 Polyphone


### Windows

为 :file:`polyphone.exe` 创建一个带参数 :command:`-s` 的快捷方式，具体方法见下文[Polyphone 启动选项](#doc_launch)一节。

### Linux

打开终端，输入以下命令行：

```
polyphone -s
```

### Mac

打开终端，输入以下命令行：

```
/Applications/Polyphone.app/Contents/MacOS/Polyphone -s
```

**注意：**如果你把 Polyphone 应用程序移到了 :file:`Applications` 以外的文件夹，需要相应修改上面的路径。


## 演奏乐器


要演奏乐器，只需打开一个音色库（例如可以从[仓库](soundfonts)下载），然后选择该音色库中的某个乐器。乐器按库排列：选择一个库，下方就会显示乐器列表。

![以合成器模式选项打开的 Polyphone](images/synth_mode.png "以合成器模式选项打开的 Polyphone")

选择乐器后，它的音符范围会显示在虚拟键盘上，你可以用 MIDI 控制器上的相应琴键演奏它。
:guilabel:`MIDI紧急复位` 按钮会停止当前正在播放的所有声音（在出现卡音时很有用），:guilabel:`录音机` 按钮会打开[录音机](manual/soundfont-editor/toolbar.md#doc_recorder)。


## 选项


### 同时演奏多个乐器

要在按下键盘上的一个琴键时演奏多个乐器，只需将 :guilabel:`多选` 选项设为 :guilabel:`开`。这样就可以在乐器列表中选择多个乐器。

### 分别演奏多个乐器

如果电脑上连接了多个 MIDI 控制器，可以打开多个音色库，并为每个打开的音色库选择特定的通道。这样，例如第一个键盘可以用来演奏一种乐器，第二个键盘则用来演奏另一种乐器。同一个音色库可以打开多次，以便同时演奏其中的多个乐器。

通过修改 :guilabel:`通道` 的值来分配通道。例如，第一个打开的标签页监听通道 1，第二个则监听通道 2。
值 :guilabel:`所有`（默认）会监听所有通道，值 :guilabel:`检测...` 会选择你在键盘上按下的下一个琴键所在的通道。

![双通道合成器配置](images/synth_2_channels.png "双通道合成器配置")

**注意：**有些键盘可以分成两部分，这样一个键盘就可以向 2 个不同的通道发送音符。

### 用琴键切换乐器

切换乐器通常是通过“Program change”类型的 MIDI 消息完成的，Polyphone 能够读取这类消息。

不过，也可以使用另一种机制：使用编程为发送“Note ON”/“Note OFF”消息的琴键，其音符编号与乐器编号相对应。当音色库只有少数几个乐器，并且琴键可以编程为发送特定音符时（例如主控键盘上的打击垫），这种机制会很有用。

如果 :guilabel:`按键选择` 选项设为 :guilabel:`开`：

* 收到“Note ON”MIDI 消息时，激活一个乐器，
* 收到“Note OFF”MIDI 消息时，停用一个乐器。

当主控键盘上的打击垫被配置为切换模式（按一下激活音符，再按一下停用）时，可以使用此选项。

如果选择 :guilabel:`切换`：

* 收到“Note ON”MIDI 消息时，激活一个乐器，
* 再次收到“Note ON”MIDI 消息时，停用一个乐器。

这次，切换是在 Polyphone 中完成的。


## Polyphone 启动选项 {#doc_launch}


上面介绍的所有选项，都可以在打开 Polyphone 时通过在命令行中添加参数来调用。还可以指定要打开的音色库。

在 **Linux** 或 **macOS** 上，只需补全命令行即可。在 **Windows** 上，右键单击可执行文件 :file:`polyphone.exe`，然后选择 :guilabel:`发送到` → :guilabel:`桌面快捷方式`（在 Windows 11 上需先在右键菜单中选择 :guilabel:`显示更多选项`）来创建快捷方式。然后，打开该快捷方式的属性，就可以在目标中添加参数，例如：

```
"C:\Program Files\Polyphone\polyphone.exe" -s -c "all|on|toggle"
```

各个选项的说明见这个[页面](manual/annexes/command-line.md#doc_synth)。


## 合成器的用途


既然你已经知道如何把 Polyphone 变成合成器，何不[用树莓派制作一件乐器](tutorials/create-a-synthesizer-with-raspberry-pi.md)？

Polyphone 的合成器模式还可以用来[播放 MIDI 文件](tutorials/read-midi-file-with-soundfont.md)。
