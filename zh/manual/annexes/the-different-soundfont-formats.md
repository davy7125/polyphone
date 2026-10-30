Polyphone 支持多种音色库格式：

* [sf2](#doc_sf2)，
* [sf3](#doc_sf3)，
* [sfz](#doc_sfz)，
* [dls](#doc_dls)，
* [sfArk](#doc_sfark)，
* [organ](#doc_organ)。


## sf2 音色库 {#doc_sf2}


sf2 音色库是 Polyphone 编辑的主要文件类型。
每个 sf2 文件包含一个或多个虚拟乐器，由音频采样和大量参数组成。
这些参数定义采样在整个键盘上应如何播放，还可能受到预定义信号（[调制器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)）的调制。
sf2 文件采用 3 层结构：

* [采样](manual/soundfont-editor/editing-pages/sample-editor.md)，例如来自小号或钢琴，
* [乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)，由采样组成，
* [预设](manual/soundfont-editor/editing-pages/preset-editor.md)，由乐器组成。

除了这些元素的定义和设置之外，sf2 文件还包含[一般信息](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md)（例如作者、版权或编辑时间）。

音色库由软件合成器使用，例如由 MIDI 信号驱动的 <a href="http://www.fluidsynth.org/" target="_blank">FluidSynth</a>。
也可以使用 sf2 音色库，通过 <a href="https://sourceforge.net/projects/timidity" target="_blank">TiMidity</a>、<a href="https://www.mindwerks.net/projects/wildmidi" target="_blank">WildMIDI</a> 或 QuickTime 收听 MIDI 文件，或通过 <a href="https://musescore.org/" target="_blank">MuseScore</a> 试听乐谱。
采样器、声卡甚至合成器等硬件也可能支持 .sf2 格式。

关于音色库的更多信息，可以参考例如 <a href="https://zh.wikipedia.org/wiki/SoundFont" target="_blank">维基百科</a>。
sf2 规范的 2.01 版可在[这里](files/sf_specifications_v2.01.pdf)获取，2.04 版可在[那里](files/sf_specifications_v2.04.pdf)获取。


## sf3 音色库 {#doc_sf3}


sf3 格式由 <a href="https://musescore.org/" target="_blank">MuseScore</a> 开发，各方面都与 sf2 格式相似，区别在于[采样](manual/soundfont-editor/editing-pages/sample-editor.md)可以以 OGG 格式（类似 MP3 格式，但是开源的）存储，而不是以原始数据存储。
因此，在质量相当的情况下，sf3 格式的体积约为 sf2 格式的十分之一。
在 Polyphone 中，可以在音色库的[一般信息](manual/soundfont-editor/editing-pages/editing-of-the-general-information.md#doc_edit)中启用压缩，然后在[采样编辑器](manual/soundfont-editor/editing-pages/sample-editor.md#doc_info)中压缩每个采样（可以选择压缩质量）。

以这种格式导出的音色库中的压缩采样不适合之后再编辑，因为反复压缩会导致音质下降。
这种格式应视为最终产品，对所有音色库的最终用户都非常有价值，因为：

* 更便于在互联网上下载，
* 可以节省电脑的存储空间，
* MIDI + sf3 的组合为播放配乐（电子游戏、移动应用……）提供了一种轻量而强大的解决方案。

与解决同类问题的 <a href="http://www.fmjsoft.com/fmt/sf2pack.htm" target="_blank">sf2pack</a> 格式不同，sf3 格式完全开源。
其源代码是公开的，从而保证了它的长期可用性（让我们避免重蹈 sfArk 或 sfPack 的覆辙！）。


## sfz 音色库 {#doc_sfz}


sfz 格式与 sf2 格式的目标相同：通过在键盘上安排和配置采样来创建乐器。
主要区别在于，sf2 格式是一个包含所有内容的单一文件，而 sfz 格式是一个文本文件，附带一组 .wav 采样。
由于它的设计初衷是可以由人直接编辑，主要优势在于无需复杂的编辑器就能编辑该文件。
但对于大型乐器，仍然强烈建议使用编辑器，因为参数数量很快就会多得令人望而生畏。
sfz 格式的定义也不如 sf2 格式严格：在编辑和播放 sfz 乐器的方式上可能会出现差异。不过与此同时，sfz 乐器的能力相比 sf2 标准也有所扩展。


### sfz 导入的限制


* 只导入在[表格](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)中有对应项的操作码（opcode）。
* 由于 sf2 文件中作用于音高和滤波器的包络是同一个（Mod env），因此无法为音高和滤波器导入不同的包络（``fileg_*`` 和 ``pitcheg_*`` 操作码）。
* 由于 sf2 文件中作用于音量和滤波器的 LFO 是同一个（Mod LFO），因此无法为音量和滤波器导入不同的 LFO（``amplfo_*`` 和 ``fillfo_*`` 操作码）。
* 唯一可以导入的滤波器是二阶低通滤波器（``fil_type=lpf_2p``）。
* 要正确导入，同一组内的 ``group`` 和 ``off_by`` 操作码必须包含相同的值。

**警告：**``width`` 和 ``position`` 操作码可能无法被正确解释。


### sfz 导出的限制


* 目前不导出[调制器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)。
* 由于 sfz 格式（版本 1）只允许一种音高调制，因此 :guilabel:`Vib LFO → 音高 (c)` 和 :guilabel:`Mod LFO → 音高 (c)` 这两个参数无法同时导出。
* 0-1 的循环可以导出，但大多数 sfz 播放器会忽略它。
* 某些合成器可能不支持 24 位采样，请参阅它们的规格说明书。


### 一般说明


sfz / sf2 转换过程中声级（dB）的匹配已使用 Sforzando 软件进行过测试。
不过仍可能存在一些差异。

此外，参数随音符变化的方式（通过 :guilabel:`音符 → 音量包络保持 / 衰减` 和 :guilabel:`音符 → Mod 包络保持 / 衰减`）没有完全对应的方式。
sf2 格式使用指数规律来定义衰减和保持时长随音符的变化，而 sfz 格式使用线性规律。


## DLS 文件 {#doc_dls}


DLS 格式（DownLoadable Sounds）由 MIDI Manufacturers Association 发布，其目的与 sf2 格式相同：一个文件中包含采样以及将其作为乐器演奏所需的参数。
它有两个级别：DLS 1 级（1997 年）和 DLS 2 级（最新版本为 2.2）。
DLS 文件曾被 Windows 的软件合成器（文件 :file:`gm.dls`）、DirectMusic、电子游戏和手机所使用。

其结构与 sf2 格式不同：DLS 文件包含波形（即采样）和乐器，每个乐器都像预设一样具有库编号和程序编号。
一个乐器由若干区域组成，每个区域在一定的音符和力度范围内播放一个波形。
参数（包络、LFO、滤波器、调制等）以连接的形式写入，连接由一个源（例如 MIDI 控制器或 LFO）、一个可选的控制和一个目标组成。
参数既可以为整个乐器定义，也可以为单个区域定义，此时区域的参数会取代乐器的参数。

Polyphone 可以导入两个级别的 DLS 文件（仅限 .dls 文件，不支持嵌入在 .rmi 或 .xmf 文件中的 DLS 库），并能将音色库[导出](manual/menu.md#doc_save)为 DLS 2.2 级格式。
保存音色库时，导入的 DLS 文件不会被修改：音色库会以 sf2 格式保存。


### DLS 导入的限制


* 部分参数在 sf2 格式中没有对应项，因此会被忽略：音量包络的关断时间、立体声声像以外的输出（中置、后置和 LFE 声道）、作用于音量或滤波器的颤音 LFO，以及作用于音量的调制包络。
* 在 DLS 合成器中，再次弹奏同一个音符会停止前一个音符，而在 sf2 合成器中两个音符都能听到。
* 只保留库编号的 MSB（CC0），若 MSB 为 0 则保留库编号的 LSB（CC32）。如果多个乐器具有相同的库编号和程序编号，其中一个乐器的程序编号会被更改。


### DLS 导出的限制


* DLS 1 级合成器可以播放采样，但会忽略包络、LFO、滤波器和调制。
* 24 位采样会被舍入为 16 位。
* 以下内容不会导出：
  参数 :guilabel:`固定音符` 和 :guilabel:`固定力度`，
  使用[循环播放](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)“释音”模式的分区，
  变换为“绝对值”或与其他调制器链接的调制器，
  作用于 DLS 格式中不存在的参数的调制器（采样偏移、音符到包络保持或衰减等），
  用 2 个源改变 LFO 或调制包络深度的调制器。
* 源和目标相同但曲线不同的两个调制器会被合并。
* 每个预设最多只能导出 15 个独占类。
* :guilabel:`音阶调音` 以及来自 CC1、CC7、CC10、CC11、CC91 和 CC93 以外的 MIDI 控制器的调制，可能会被某些 DLS 合成器忽略。


### 一般说明


sf2 和 DLS 格式的默认调制不同（力度到滤波器截止频率、调制轮和通道压力控制的颤音、混响和合唱的电平）：转换时会添加调制器，使声音保持不变。

转换已通过比较原始文件与转换后文件的渲染结果进行测试，并使用可读写 DLS 文件的 <a href="https://github.com/spessasus/spessasynth_core" target="_blank">SpessaSynth</a> 进行了验证。
由于支持 DLS 2 级格式的合成器很少，某些参数的播放方式可能因合成器而异。


## sfArk 压缩文档 {#doc_sfark}


与 sf3 格式一样，sfArk 压缩文档也是一种压缩格式。
这意味着声音质量会略有下降（但通常难以察觉）。
这种格式正逐渐过时，因此 Polyphone 只支持导入 sfArk 压缩文档（不支持导出）。


## GrandOrgue 配置 {#doc_organ}


GrandOrgue 是一款主要用于模拟管风琴等乐器的程序。一架管风琴由一系列 wav 文件（每个文件包含一根音管的声音）以及一个扩展名为“.organ”的文件定义，后者包含基于所有 wav 文件构建管风琴所需的全部参数。基本上，所有采样都与特定音符关联，并按音栓分组，演奏管风琴时可以开启或关闭这些音栓。Polyphone 能够根据这种映射创建音色库，但不支持将音色库转换为 GrandOrgue 格式。

### 导入限制

目前不支持移调。


*[MIDI]: Musical Instrument Digital Interface
