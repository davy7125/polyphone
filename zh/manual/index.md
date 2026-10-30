欢迎阅读 Polyphone **用户手册**！想快速了解什么是音色库以及如何用 Polyphone 编辑音色库，可以先看[教程](tutorials/index.md)，或者观看 <a href="https://www.youtube.com/results?search_query=polyphone+sf2" target="_blank">YouTube</a> 上的视频（特别感谢 Polyphone 用户制作的实用视频）。如果想全面了解软件的各项功能，请继续阅读。

如果需要更多帮助，或者阅读这些页面让你望而却步，可以到[论坛](forum)寻求帮助。


## 主屏幕 {#doc_home}


Polyphone 的第一个屏幕基本上由 3 部分组成：

* [左侧部分](#doc_left)，包含已打开文件和目录的历史记录以及快捷方式；
* [右侧部分](#doc_right)，专门用于音色库仓库；
* [顶部区域](#doc_top)，包含标签页和一个菜单。


![Polyphone 主屏幕](images/page_home.png "Polyphone 主屏幕")


## 左侧部分 {#doc_left}


### 打开音色库


打开音色库有多种方式：

* 单击 :guilabel:`打开音色库` 会打开文件浏览器，可选择扩展名为 :file:`.sf2`、:file:`.sf3`、:file:`.sfArk`、:file:`.sfz`、:file:`.dls` 或 :file:`.organ` 的文件（不同格式的介绍见[这里](manual/annexes/the-different-soundfont-formats.md)）。可以一次选择多个文件；
* 单击 :guilabel:`新音色库` 会打开一个空白音色库，一切都需要从零开始制作；
* 单击 :guilabel:`打开目录` 会打开[音色库浏览器](manual/soundfont-explorer.md)，它会扫描整个目录，便于打开所需的文件；
* 双击历史记录中的音色库可以重新打开它（选中的条目也可以按 :kbd:`Enter` 打开，按 :kbd:`Del` 从历史记录中移除）；
* 将文件或目录拖放到历史记录区域，或拖放到[标签栏](#doc_top)（无论当前处于哪个标签页），即可将其打开。

打开音色库后，会出现一个新的标签页，其中包含一个[音色库编辑器](manual/soundfont-editor/index.md)。


### 快捷方式


左侧部分的底部有三个按钮：

* :guilabel:`文档`：打开网页浏览器阅读 Polyphone [文档](documentation)；
* :guilabel:`论坛`：打开网页浏览器访问 Polyphone [论坛](forum)；
* :guilabel:`设置`：在新标签页中打开软件[设置](manual/settings.md)（也可按 :kbd:`Ctrl`+:kbd:`,` 打开）。


## 右侧部分 {#doc_right}


### 在线仓库


单击 :guilabel:`在线仓库` 按钮会打开一个标签页，显示[在线音色库仓库](manual/online-repository.md)的内容。


### 每日音色库


这里每天随机显示 5 个音色库，鼓励大家发现新的音色库。
双击其中一个会打开新标签页，显示相应的[音色库描述](manual/online-repository.md#doc_description)。
单击作者名称或音色库的某个属性，会以相应的筛选条件打开[在线仓库](manual/online-repository.md)。


### 搜索


可以在这里输入关键词，在在线仓库中搜索特定的音色库，结果显示在[专用标签页](manual/online-repository.md)中。


## 顶部区域 {#doc_top}


### 标签页


通过标签页可以快速切换到：

* [主屏幕](#doc_home)（第一个标签页，始终打开），
* 已打开音色库的[编辑器](manual/soundfont-editor/index.md)，
* [音色库浏览器](manual/soundfont-explorer.md)（每个打开的目录一个），
* 软件[设置](manual/settings.md)，
* [在线仓库](manual/online-repository.md)，
* 在线仓库中某个音色库的[描述](manual/online-repository.md#doc_description)。

可以使用键盘快捷键或鼠标滚轮在标签页之间切换：

* :kbd:`Ctrl`+:kbd:`Tab`、:kbd:`Ctrl`+:kbd:`PageDown` 或在标签栏上按住 :kbd:`Shift` 向下滚动鼠标滚轮：转到下一个标签页（不包括主屏幕标签页），
* :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`Tab`、:kbd:`Ctrl`+:kbd:`PageUp` 或在标签栏上按住 :kbd:`Shift` 向上滚动鼠标滚轮：转到上一个标签页（不包括主屏幕标签页），
* :kbd:`Ctrl`+:kbd:`Home` 和 :kbd:`Ctrl`+:kbd:`End`：转到第一个和最后一个标签页（不包括主屏幕标签页），
* :kbd:`Ctrl`+:kbd:`W`：关闭当前标签页，
* :kbd:`Ctrl`+:kbd:`H`：转到主屏幕。

快捷键 :kbd:`Ctrl`+:kbd:`F` 会将焦点移到当前标签页的搜索框：主屏幕的搜索框、[在线仓库](manual/online-repository.md)的搜索框、音色库编辑器中的[树状图搜索栏](manual/soundfont-editor/tree.md#doc_search)，或[音色库浏览器](manual/soundfont-explorer.md)的搜索框。


### 菜单


菜单位于右上角，包含各种通用功能，具体介绍见[这里](manual/menu.md)。
