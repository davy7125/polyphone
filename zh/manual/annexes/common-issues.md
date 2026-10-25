本页列出音色库中常见的缺陷（这些缺陷可能会在评价中被指出），并针对每一种缺陷给出修正的方法和相关文档链接：

* [采样质量差或准备不当](#doc_sample)，
* [音准不正确](#doc_tuning)，
* [ADSR 包络不合适](#doc_adsr)，
* [循环缺失或不准确](#doc_loop)，
* [不符合 MIDI 标准](#doc_standard)。


## 采样质量差或准备不当 {#doc_sample}


有时，一个音色库中的乐器听起来不够真实，如果原因在于采样质量差，唯一的办法就是替换这些采样：

* 在网上寻找其他采样，
* 使用质量更好的麦克风录音（或借助外部信号处理工具），
* 通过其他方式制作采样。

有时，只需更好地准备或使用现有的采样，就能解决问题。

### 音质差，尤其是在高音区

通常，单个音频采样不足以制作出高质量的乐器。一个采样有其自身的音高（无固定音高的打击乐除外），在很宽的音符范围内使用它会使声音失真，尤其是在较高的音符上。要解决这个问题，应该使用多个采样；如果只有一个采样，可以：

* 为该采样创建副本，
* 例如用这个[工具](manual/soundfont-editor/tools/sample-tools.md#doc_transpose)将副本移调一个八度，
* 用这个[工具](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq)或[那个工具](manual/soundfont-editor/tools/sample-tools.md#doc_filter)滤除高频谐波。

然后应将这些采样分配到播放时不会改变原有音色的音符范围上。

### 声音中有规律的“咔嗒”声

问题很可能来自位置不当的循环点，这在另一[节](#doc_loop)中有介绍。

### 采样裁剪不当

采样往往只有一部分被用到，删除多余部分可以减小文件大小：

* 采样中有用的部分有时位于最终循环点之前：此时可以删除循环终点之后的部分，用这个[工具](manual/soundfont-editor/tools/sample-tools.md#doc_trimloop)只需点几下即可完成；
* 采样开头可能有一段静音，应将其删除，以减少触发音符时的延迟（也有一个[工具](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)可以快速完成）；
* 最后，有些采样可能根本没有被使用：这个[配置概览](manual/soundfont-editor/configuration-summaries.md#doc_sample)会列出它们，而这个[工具](manual/soundfont-editor/tools/global-tools.md#doc_unused)会自动移除它们。


## 音准不正确 {#doc_tuning}


为虚拟乐器调音包括多个步骤，按顺序完成所有步骤非常重要。

### 采样层级的调音

这是最重要的调音步骤，详见这个[页面](tutorials/how-to-prepare-a-sample.md#doc_tuning)。

### 乐器层级的调音

这个[页面](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)中介绍的 4 个参数，用于定义采样播放时的移调：**根音符**、**调音（半音）**、**调音（音分）** 和 **音阶调音**。在演奏乐器时，这些值可能会被[调制器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_modulator)修改，因此也应仔细检查调制器。

如果与某个采样链接的乐器分区中没有定义任何参数或调制器，务必检查乐器的全局分区中是否定义了参数或调制器（该分区为其他所有分区设定默认配置）。

### 预设层级的调音

在预设层级，有 3 个参数会影响采样的移调：**调音（半音）**、**调音（音分）** 和 **调音（音阶）**。这些值会叠加到预设分区所用乐器的配置上。

与乐器一样，预设也可以包含调制器，而且其全局分区相当于默认配置，不应忽视。


## ADSR 包络不合适 {#doc_adsr}


ADSR 包络表示演奏一个音符时音量如何变化，也表示松开该音符时音量如何变化。ADSR 这个词中的四个字母各对应一个特定阶段，而音色库中使用的音量包络实际上包含 6 个阶段：

* 延迟（delay），
* 起音（attack，A），
* 保持（hold），
* 衰减（decay，D），
* 延音（sustain，S），
* 释音（release，R）。

这些阶段在这个[页面](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_envelope)中有更详细的介绍，下面列出一些与包络有关的可能问题。

### 声音开始得太突然

要让音符的开始更柔和，请在[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)或[预设](manual/soundfont-editor/editing-pages/preset-editor.md)层级设置或增加音符的**起音时长**。

### 声音开始得太柔和

与上一种情况相反，请减少或去掉音符的**起音时长**。

### 声音开始前有明显的延迟

有三种可能且可以互补的解决办法：

* 如果在[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)或[预设](manual/soundfont-editor/editing-pages/preset-editor.md)层级设置了**延迟时长**，将其去掉，
* 去掉音频采样开头的静音（参见这个[页面](manual/soundfont-editor/editing-pages/sample-editor.md#doc_graph)中的采样裁剪，或[这里](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)的静音移除工具），
* 最后，感觉到的延迟可能是由你电脑上使用的音频服务器造成的（这种情况下并不是音色库本身的缺陷，可以在[设置](manual/settings.md#doc_general)中更改音频输出）。

### 声音突然中断

如果在按住音符时本应持续的声音突然中断，请参阅关于[循环](#doc_loop)的一节。

如果松开音符时声音戛然而止，请在[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md)或[预设](manual/soundfont-editor/editing-pages/preset-editor.md)层级设置或增加**释音时长**。用这个[工具](manual/soundfont-editor/tools/instrument-tools.md#doc_release)可以配置该时长，使其在从低音到高音的整个音域内听起来都很自然。


## 循环缺失或不准确 {#doc_loop}


循环可以用固定长度的采样产生无限时长的声音，而耳朵察觉不到重复。对于长笛等某些类型的乐器，循环必不可少，而对于打击乐等其他乐器则不需要。在钢琴采样中也可以使用循环，用较短的采样模拟延音踏板的效果。

### 松开音符之前声音就停止了

如果声音在本应继续播放时停止了，最可能的原因是：

* 采样的循环没有定义——这种情况下，可以参考这个[页面](tutorials/how-to-prepare-a-sample.md#doc_loop)来设置循环，
* 没有启用循环播放——这种情况下，需要在[乐器](manual/soundfont-editor/editing-pages/instrument-editor.md#doc_table)层级修改**循环播放**参数。

### 声音中有规律的“咔嗒”声

很可能是循环不正确，这个[页面](tutorials/how-to-prepare-a-sample.md#doc_loop)介绍了如何制作合适的循环。


## 不符合 MIDI 标准 {#doc_standard}


MIDI 标准规定了音色库中所含乐器的特定编号方式。这样，遵循同一标准的不同音色库就可以互换使用，而所演奏的音色不会有明显差异：例如，钢琴和长笛会以相同的方式被调用。如果评价指出音色库不符合某个标准，有两种可能的解决办法：

* 删除声明该音色库符合某个标准的信息，
* 或者检查音色库中的所有预设是否都正确地分配给了相应种类的乐器（乐器列表见这个[页面](manual/annexes/midi-standards.md)），并且标准要求的所有乐器都已具备。
