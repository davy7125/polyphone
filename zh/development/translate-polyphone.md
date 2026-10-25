Polyphone 在设计上支持翻译，以便让更多人能够使用。
每个人都可以参与这项工作：可以从零开始创建一种翻译，也可以完善或改进现有的翻译。
从 2.6 版开始，所有原本不完整的翻译都已借助 AI 补全，这些翻译值得由母语使用者进行审校。


## 前提条件


### 翻译文件


在这个[页面](software/translations)下载与你要翻译的语言对应的翻译包。
如果列表中还没有该语言，请使用[联系](contact)表单提出申请（只需说明你想要的新语言）。
你很快就会收到一个新的翻译包。

翻译文件也可以在 <a href="https://github.com/davy7125/polyphone" target="_blank">GitHub</a> 仓库的 :file:`sources` 目录中找到（每种语言一个 :file:`polyphone_XX.ts` 文件）。


### Qt Linguist


翻译所需的唯一软件是 :program:`Qt Linguist`：一款用于生成翻译文件的工具。
该软件随 Qt 一起安装（参见[开发页面](development/index.md)），Windows 用户也可以在<a href="https://github.com/thurask/Qt-Linguist/releases" target="_blank">这里</a>找到它，Linux 用户则可以在各发行版的软件仓库中找到（Debian 和 Ubuntu 上为 :program:`linguist-qt6` 软件包，Fedora 上为 :program:`qt6-linguist`）。


## 打开翻译文件


解压刚刚下载的翻译包。
你会找到一个 .ts 文件 :file:`polyphone_XX.ts`，其中 :file:`XX` 对应你的语言。
用 :program:`Qt Linguist` 打开 :file:`polyphone_XX.ts`，即可开始翻译。


## 翻译


每个上下文中的每个条目都需要翻译。
你只需逐一查看每个元素并填入你的译文。
根据你对译文的把握程度，可能会有三种情况：

* 如果不知道如何翻译，请放心跳过该元素。
  这没有问题：其他人可以补全它。
* 你认为译文正确，但不太确定：写下你的译文，然后按 :kbd:`Ctrl`+:kbd:`J`。
  该条目不会被标记为已确认，但译文仍会在翻译后的界面中使用。
  这只是告诉其他改进翻译的人，你当时并不确定。
* 你确定译文正确：写下译文并按 :kbd:`Ctrl`+:kbd:`Enter`。
  该条目会被标记为已确认。

翻译需要专注（在这个一切都被设计来<a href="https://medium.com/thrive-global/how-technology-hijacks-peoples-minds-from-a-magician-and-google-s-design-ethicist-56d62ef5edf3" target="_blank">争夺注意力</a>的世界里）。
请注意：

* 标点符号，
* 开头和结尾的空格，
* 大小写，
* 单数和复数形式。

最后一个建议：**不要着急！**
别忘了按 :kbd:`Ctrl`+:kbd:`S` 保存你的工作。


## 测试你的翻译


你可以随时检查自己的翻译。
在 :program:`Qt Linguist` 中，选择 :menuselection:`File --> Release As…`，生成与你所提供的翻译对应的 .qm 文件。
按照翻译包中的说明（:file:`readme.txt` 文件）安装该文件，然后打开 Polyphone。
进入[设置](manual/settings.md#doc_interface)，在语言列表中选择你的翻译，然后重启 Polyphone。
现在就可以审阅你的翻译了。

**注意：**:guilabel:`Release As…` 操作可能只会翻译不含重音符号的文本。如果出现这个问题，请打开终端并使用以下命令：:command:`lrelease polyphone_XX.ts`（根据你的文件进行修改）。
翻译文件 :file:`polyphone_XX.qm` 也可以从名为 :file:`translations` 的目录（位于当前目录或 Polyphone 的配置目录中）加载，而不必内嵌在软件中。


## 提交翻译


完成后（或者累了的时候！），请将你编辑的 .ts 文件（而不是 .qm 文件！）发送到 <contact@polyphone.io>。
这个[页面](software/translations)上的翻译包会随之更新。

如果你熟悉 GitHub，也可以在<a href="https://github.com/davy7125/polyphone" target="_blank">仓库</a>中通过 pull request 提交你的 .ts 文件（参见贡献指南）。