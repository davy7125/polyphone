MIDI 文件包含 MIDI 消息，组合起来就构成一段旋律。它们可以与使用音色库的合成器配合使用，本教程介绍如何用 MIDI 文件来测试音色库的效果。


## 一点背景知识


<div class="too_complicated">
<p>在合成器键盘上演奏时，会发出 <a href="https://zh.wikipedia.org/wiki/MIDI" target="_blank">MIDI</a> 信号，其中包含所演奏的音符及其力度（表示按键力度大小的术语）等信息。简单来说，MIDI 文件就是按时间顺序记录了这样一个键盘所发出的信号。到这一步，还没有产生任何声音。</p>

<p>要让扬声器发出声音，还需要一个转换步骤，这就是合成器的作用：它根据收到的 MIDI 信号触发声音。合成器有多种类型，这里我们讨论的是基于采样的合成器，更具体地说，是使用 SoundFont 格式音色库的合成器。</p>

<p>一个 MIDI 文件最多可以控制 16 个不同的通道，从而可以同时使用 16 种不同的乐器。除了指明要演奏哪些音符外，MIDI 文件还包含一些指令，告诉合成器在通道 Z 上接下来的音符要使用库 Y 中的乐器 X。为了确保乐器选择正确，合成器所使用的音色库必须符合 <a href="https://zh.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> 标准，该标准为每个 X 和 Y 编号指定了特定的乐器。例如，在音色库中，乐器 <strong>001</strong> 必须对应钢琴。</p>

<p>总结一下，这里要讨论的是：播放一个 MIDI 文件，由它驱动一个合成器，合成器再触发一个符合 General MIDI 标准的音色库中的声音。接下来的内容就简单多了！</p>
</div>


## 准备工作


要用下文介绍的方法播放 MIDI 文件，你需要：

- 能够上网，
- 一个网页浏览器，
- 一个 MIDI 文件，例如<a href="files/ff7choco.mid" download>这个</a>，
- 可选：一个你想测试、且符合 <a href="https://zh.wikipedia.org/wiki/General_MIDI" target="_blank">General MIDI</a> 标准的音色库（[仓库](soundfonts/midi-standard/gm)中有相应的筛选条件），
- 可选：最新版本的 Polyphone，可在这个[页面](software)获取。


## 播放 MIDI 文件


一个简单的方法是使用 <a href="https://spessasus.github.io/SpessaSynth/" target="_blank">SpessaSynth</a> 合成器，它既是 MIDI 文件播放器，又是音色库合成器。它的优点是可以在线使用，无需任何特殊安装。

单击 :guilabel:`Load MIDI file` 按钮，选择你的 MIDI 文件，就可以开始了。

![SpessaSynth MIDI 播放器和合成器](images/SpessaSynth.jpg "SpessaSynth MIDI 播放器和合成器")


## 选择音色库


要使用你选择的音色库，请单击 :guilabel:`Upload soundfont` 按钮并选择你的文件。


## 将 MIDI 输出连接到 Polyphone


如上所述，**SpessaSynth** 既是 MIDI 文件播放器，又是合成器。也可以只使用它的 MIDI 文件播放功能，把信号发送给 Polyphone，由 Polyphone 负责声音合成。如果音色库太大，不便通过互联网上传，这种方式会很有用。

将 SpessaSynth 连接到 Polyphone 的方法（已在 Linux 上测试）：

- 以[合成器模式](tutorials/use-polyphone-as-a-synthesizer.md)启动 Polyphone，并打开[设置](manual/settings.md#doc_general)，
- 选择“MIDI through”作为 MIDI 输入（记住这个名称），
- 在网页浏览器中打开 SpessaSynth，并打开它的设置，
- 选择 MIDI 输出“MIDI through”（与 Polyphone 的 MIDI 输入同名）。

由于 MIDI 文件可以向 16 个通道发送消息，需要将音色库打开 16 次，并为每个标签页指定 1 到 16 中的不同通道。为方便起见，打开 Polyphone 时可以指定要打开的音色库以及选项“each”，从而一次打开全部 16 个标签页。例如，命令如下：

```
polyphone -s -i "/home/davy/Downloads/gm.sf2" -c "each|off|off"
```

关于命令行的更多信息，请参阅这个[页面](manual/annexes/command-line.md#doc_synth)。
