音色库中的乐器由采样组成，这些采样例如来自对真实乐器的录音。
本教程介绍如何在音色库中添加并准备采样。


## 导入 {#doc_import}


首先，你的电脑上需要有采样。
网上可以找到大量免费采样，有的是录制自真实乐器（长笛、吉他、大提琴……），有的则是用各种声音合成方式制作的。

本教程提供了这个长笛采样：<a href="files/flute.wav" download>flute.wav</a>。

拿到采样后，用 Polyphone 新建一个音色库（:menuselection:`Menu --> 新建`），然后导入该采样（单击[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)中的 :guilabel:`添加采样`）。


![导入采样](images/import-sample.png "导入采样")


## 循环 {#doc_loop}


### 简介


循环是采样中可以重复播放、从而延长采样时长的一段区域。
假设一个采样只有 0.5 秒长，有了一个好的循环，声音可以延长到 10 秒甚至更长。
这只对长笛、小号、管风琴、小提琴等非打击类乐器的采样有用。
相反，鼓类不需要循环。
钢琴或吉他可能也是如此，这取决于这些声音的合成方式。

因此，只有当你觉得需要延长采样原有的长度时，才需要阅读本节。
否则，请直接跳到下一节“调音”。


### 设置循环


在树状图中，单击要设置循环的采样，并关注循环参数。


![为采样设置循环：循环参数](images/loop-sample-1.png "为采样设置循环：循环参数")


如果运气好，这些值已经包含在 .wav 采样中，相应字段已经填好了。
如果没有，或者设置得不好（这种情况确实会发生……），可以在波形图（WAV 显示区域）上随意单击左键和右键，以确定循环的起点和终点。
通常应在声音的稳定阶段设置循环，而不是在起音或释音阶段。
然后，放大循环起点附近的区域，尽量让绿色（或红色）曲线与蓝色曲线重合。


![为采样设置循环：波形对齐](images/loop-sample-2.png "为采样设置循环：波形对齐")


### 试听效果


设置好循环后，在启用 :guilabel:`循环` 选项的情况下，按播放器上的 :guilabel:`播放` 来检查效果。


![为采样设置循环：检查效果](images/loop-sample-3.png "为采样设置循环：检查效果")


如果在播放到循环终点时听不出过渡，说明这个循环是好的。
否则，请保持播放器运行，并稍微移动循环的终点和 / 或起点，直到找到过渡尽可能平滑的位置。
请听下面的录音，体会糟糕的循环和良好的循环之间的区别：

* <a href="files/bad-loop.mp3" download>糟糕的循环</a>

![](files/bad-loop.mp3)

* <a href="files/good-loop.mp3" download>良好的循环</a>

![](files/good-loop.mp3)


**注意：**如果很难找到合适的循环，可以借助一个工具来“作弊”：[自动循环](manual/soundfont-editor/tools/sample-tools.md#doc_autoloop)工具。
它会通过剪切采样并叠加两个片段来自动创建循环，使片段的结尾与开头完全吻合。
更多信息请参阅相关文档。


## 调音 {#doc_tuning}


如果不想得到一个走音的乐器，就必须为所有采样调音。
在音色库中标明所有导入采样的准确音高后，音色库就能将它们适配到各个琴键上。
有些采样可能已经包含这些信息，这时就不必修改 :guilabel:`根音符` 和 :guilabel:`校正` 中的值。
不过，为谨慎起见，最好还是检查一下。


![为采样调音：音高信息](images/tune-sample-1.png "为采样调音：音高信息")


如果采样有循环，请启用循环，然后按播放器上的 :guilabel:`播放`。
接着启用 :guilabel:`正弦` 校准，并调整音量，使两个声音都能听得尽可能清楚。


![为采样调音：采样与校准信号一起播放](images/tune-sample-2.png "为采样调音：采样与校准信号一起播放")


开始调音时，先修改根音符，直到两个声音最为接近。
然后继续调整校正值。
在这一步，请注意出现的拍频，并使其尽可能慢。
下面是一些包含长笛声音和校准正弦波的示例：

* <a href="files/very-bad-tune.mp3" download>音准很差</a>（根音符不对）

![](files/very-bad-tune.mp3)

* <a href="files/bad-tune.mp3" download>音准较差</a>（校正值不正确，可以听到拍频）

![](files/bad-tune.mp3)

* <a href="files/good-tune.mp3" download>音准良好</a>（两个值都正确）

![](files/good-tune.mp3)


**注意：**[频率分析](manual/soundfont-editor/editing-pages/sample-editor.md#doc_freq)可以作为开始调音时的一个很好的参考。但请记住，没有任何算法能完美地完成这项任务，只有你的耳朵才能找到正确的音准。


![为采样调音：频率分析](images/tune-sample-3.png "为采样调音：频率分析")


## 更进一步 {#doc_further}


[工具栏](manual/soundfont-editor/toolbar.md#doc_edit)的 :guilabel:`工具箱` 菜单中提供了多种工具。
你可以通过以下方式编辑采样：

* [移除开头空白](manual/soundfont-editor/tools/sample-tools.md#doc_removeblank)，
* 对声音进行[均衡](manual/soundfont-editor/editing-pages/sample-editor.md#doc_eq)，
* [移调](manual/soundfont-editor/tools/sample-tools.md#doc_transpose)，
* 对音量进行[归一化](manual/soundfont-editor/tools/sample-tools.md#doc_volume)，
* 为立体声采样[调整平衡](manual/soundfont-editor/tools/sample-tools.md#doc_balance)。


## 关于立体声采样的说明 {#doc_stereo}


立体声采样由两个单声道采样组成。
二者之间的链接信息存储在采样的信息中。


![立体声采样](images/stereo-sample.png "立体声采样")


管理两个单声道采样可能会增加设置循环的难度，因为一个循环可能在一侧听起来很好，而在另一侧却不行。
这种情况下，不妨“作弊”，为左右两侧分别设置不同的循环。
这并不会影响声音质量。
