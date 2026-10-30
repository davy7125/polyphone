菜单位于软件的右上角，包含用于访问各种功能的若干菜单项。


![菜单](images/menu.png "菜单")


## 打开音色库编辑器 {#doc_open}


新建
: 新建一个空白音色库，一切都需要从零开始制作。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`N`。

打开…
: 打开 [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz)、[.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls)、[.sfArk](manual/annexes/the-different-soundfont-formats.md#doc_sfark) 或 [.organ](manual/annexes/the-different-soundfont-formats.md#doc_organ) 文件。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`O`。


## 保存音色库 {#doc_save}


保存
: 保存当前文件。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`S`。

另存为…
: 将当前文件保存为另一个文件。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`Shift`+:kbd:`S`。

导出音色库…
: 将已打开音色库中的一个或多个预设合并，并以 [.sf2](manual/annexes/the-different-soundfont-formats.md#doc_sf2)、[.sf3](manual/annexes/the-different-soundfont-formats.md#doc_sf3)、[.sfz](manual/annexes/the-different-soundfont-formats.md#doc_sfz) 或 [.dls](manual/annexes/the-different-soundfont-formats.md#doc_dls) 格式导出。另外的 .csv 格式会导出所有参数，以便详细分析音色库的内容。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`E`。


![导出窗口](images/export.png "导出窗口")


导出窗口中会列出所有已打开音色库的预设，需要勾选要导出的预设（:guilabel:`全选` 和 :guilabel:`全不选` 按钮可以帮助快速更改选择）。
其他参数包括：

目标
: 写入文件的目录。
  文件永远不会被覆盖：如果已存在同名文件，会在新文件名的末尾加上一个数字。

格式
: :guilabel:`.sf2`、:guilabel:`.sf3`、:guilabel:`.sfz`、:guilabel:`.csv` 或 :guilabel:`.dls` 之一。
  如果预设来自多个音色库，它们会被合并到一个音色库中，每个源音色库成为其中的一个库。
  因此，同一次最多可以导出 127 个音色库，每个音色库最多选择 127 个预设。

质量
: 仅适用于 .sf3 格式，指采样的压缩质量。

每个预设一个文件
: 仅适用于 .sf2、.sf3 和 .dls 格式，为每个选中的预设分别创建一个文件，而不是生成单个文件。

每个库一个目录、预设编号作为前缀、通用MIDI分类
: 仅适用于 .sfz 格式。导出时会创建一个目录，其中包含 sfz 文件和一个 :file:`samples` 子目录。
  sfz 文件可以按库分别放在不同的目录中，文件名可以加上预设编号作为前缀，还可以按 General MIDI 类别分到相应的目录中。

原始值
: 仅适用于 .csv 格式。数值按照其在 sf2 格式中的存储方式写出，而不是转换为常用单位（秒、分贝、音分等）。
  导出时会创建一个目录，其中包含 :file:`samples.csv` 文件，以及每个乐器和每个预设各一个文件。


## 视图选项 {#doc_view}


全屏
: 切换全屏模式。
  也可以使用快捷键 :kbd:`F11`。


## 快捷方式 {#doc_shortcuts}


设置
: 打开软件[设置](manual/settings.md)。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`,`。

在线手册
: 在网页浏览器中显示本用户手册。
  也可以使用快捷键 :kbd:`F1`。

关于 Polyphone…
: 显示软件版本和版权信息。:guilabel:`制作人员` 按钮会显示为项目做出贡献的人员名单。


## 关闭 {#doc_close}


关闭标签页
: 关闭当前标签页（音色库编辑器、音色库浏览器、设置等）。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`W`。

退出
: 退出软件。
  也可以使用快捷键 :kbd:`Ctrl`+:kbd:`Q`。