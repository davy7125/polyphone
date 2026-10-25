树莓派是一台信用卡大小的电脑，可以在上面安装 Polyphone。把它装进带扬声器的箱体，再搭配一个主控键盘，就能制作出一台真正的合成器！

![自制合成器——正面](images/synth_front.jpg "自制合成器——正面")

![自制合成器——背面](images/synth_back.jpg "自制合成器——背面")

## 所需材料

这份清单可以根据你的项目进行调整：

- 一块树莓派，
- 一张容量为 32 GB 或以上的 micro SD 卡，
- 一个树莓派电源，
- 一个数模转换器（DAC），用于将 USB 输出转换为音频插孔（jack）输出，
- 电脑音箱，其优点是同时包含扬声器和信号放大部分，
- 一个总电源开关，
- 一根嵌入式安装用的公对母双 USB 线，
- 一个用来装下所有部件的箱体，
- 当然还有一个用来演奏的主控键盘。

为了方便地配置树莓派，建议再准备：

- 一台显示器，以及一根连接树莓派用的 micro-HDMI 线，
- 一个键盘，
- 一个鼠标。

本页的示例中使用的是二手的 Edifier R1280T 音箱。它们价格不高，却能较好地还原低音，而且正如下文所示，还可以校正其频率响应的不足。

所用的主控键盘是 Akai MPK Mini Plus，可通过 USB 连接。USB / 插孔转换器是 UGREEN 的入门款，树莓派为第 5 代。

（列出这些细节并不是为了给任何品牌做广告，而是为了介绍一个可行的组合）

## 制作箱体

本教程不会专门介绍某一种制作箱体的方法。像本例这样使用木材，可以展示你的木工手艺，不过用纸板也同样可行。尽情发挥你的创造力吧！总的来说，你需要一个可以打开以便接触内部元件的盖子，以及以下开口：

- 扬声器，
- 电源线，
- 电源开关，
- 音量旋钮以及（如有需要）均衡旋钮，
- 用于低音反射的亥姆霍兹共鸣器，等等。

建议按以下顺序在箱体内安装：

- 首先嵌入安装电源开关、后面板上的 USB 接口以及与扬声器相连的其他按钮，
- 放置电源线，并将其连接到电源开关的输入端，
- 放置与扬声器配套的变压器，并将其连接到电源开关的输出端，
- 放置扬声器的电路板，并将其连接到变压器，
- 固定扬声器，并将其连接到电路板，
- 固定树莓派，
- 将树莓派的电源连接到电源开关的输出端（开关的输出同时为扬声器和树莓派供电），
- 将双 USB 线连接到树莓派，
- 将 USB/插孔转换器连接到另一个 USB 接口，
- 用一根插孔线连接扬声器的电路板。

在下图中，电源和扬声器相关的电路板用橙色圈出，树莓派用蓝色圈出。

![合成器内部](images/synth_inside.jpg "合成器内部")

注意：扬声器的放置必须避免扬声器输出的声音回传到箱体内部，否则会产生共振。**所以不要像本例这样！**有时需要动手尝试才能发现其中的陷阱……

![树莓派](images/synth_raspberry_pi.jpg "树莓派")

共使用 3 个 USB 接口：

- 一个用于获得插孔输出，
- 另外 2 个引到箱体背面，用于连接主控键盘，或者在配置树莓派时连接键盘和鼠标。

## 准备树莓派

### 安装 Debian

拿到树莓派后，首先要做的事情之一，就是在 micro SD 卡上安装 **Debian**。网上很容易找到相关说明，例如<a href="https://www.raspberrypi.com/documentation/computers/getting-started.html#installing-the-operating-system" target="_blank">这里</a>。

准备好 micro SD 卡后，将其插入树莓派的卡槽。然后接上电源、显示器、鼠标和键盘，你的电脑就可以使用了！

### 安装并配置 Polyphone

首先，用以下命令更新系统：

```
sudo apt-get update
sudo apt-get upgrade
```

然后使用这个[页面](software)上提供的 .deb 软件包安装 **Polyphone**。用 USB 将 MIDI 键盘连接到树莓派（如果没有空闲的 USB 接口，请拔下电脑键盘），然后打开 Polyphone，在[设置](manual/settings.md#doc_general)中从 MIDI 输入的下拉列表里选择该 MIDI 键盘。下载一个音色库（例如从[仓库](soundfonts)下载）之后，就已经可以用连接到树莓派的 MIDI 键盘演奏乐器了！

### 自动启动 Polyphone

这一步是为了摆脱对显示器的依赖。可以创建一个包含以下内容的 :file:`synthesizer.desktop` 文件，让树莓派开机时自动启动 Polyphone：

```
[Desktop Entry]
Name=Synthesizer
Exec=polyphone -s /home/pi/Desktop/my-soundfont.sf2
Terminal=false
Type=Application
StartupNotify=true
Icon=/home/pi/Desktop/my-icon.png
Categories=AudioVideo;
```

然后将该文件放到 :file:`/home/pi/.config/autostart` 文件夹中。

可以在命令行中添加选项，用于监听特定的 MIDI 通道、同时演奏多个乐器，或通过琴键激活 / 停用乐器。这个[页面](tutorials/use-polyphone-as-a-synthesizer.md)详细介绍了在合成器模式下使用 Polyphone 的各个选项。

## 更进一步

### 声音均衡

每个扬声器由于其结构不同，对所要发出的各个频率的响应也或多或少有所不同。为了获得平直的频率响应，从而忠实地还原声音，可以制作一个均衡设置来弥补扬声器的不足。<a href="https://github.com/erdewit/HiFiScan" target="_blank">HiFiScan</a> 工具可以自动计算均衡参数，具体流程见该项目的页面。

计算出均衡曲线后，**HiFiScan** 可以导出系数，以便集成到均衡器中。这里我们关注的是可以与 **Calf** 一起使用的 12 段均衡。

### 远程连接

到目前为止，配置树莓派都需要一台显示器来显示桌面。不过，也可以将系统配置为允许通过 **VNC 协议**进行远程访问，网上有很多资料介绍如何设置。
