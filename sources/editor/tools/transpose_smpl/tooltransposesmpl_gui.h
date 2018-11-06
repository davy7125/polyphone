#ifndef TOOLTRANSPOSESMPL_GUI_H
#define TOOLTRANSPOSESMPL_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolTransposeSmpl_gui;
}

class ToolTransposeSmpl_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolTransposeSmpl_gui(QWidget *parent = 0);
    ~ToolTransposeSmpl_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolTransposeSmpl_gui *ui;
};

#endif // TOOLTRANSPOSESMPL_GUI_H
