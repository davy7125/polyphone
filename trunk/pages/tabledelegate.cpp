#include "tabledelegate.h"
#include "spinboxkey.h"
#include "spinboxrange.h"
#include "comboboxloopmode.h"
#include "config.h"
#include <QTableWidget>
#include <QApplication>

const char * TableDelegate::DECO_PROPERTY = "deco";

QWidget * TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    bool isNumeric, isKey, isLoop;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop);

    QWidget * widget;
    QColor highlightColor = parent->palette().color(QPalette::Highlight);
    if (isNumeric)
    {
        if (isLoop)
        {
            // Remove the icon in the model
            QVariant previousDecoration = index.data(Qt::DecorationRole);
            QAbstractItemModel * model = (QAbstractItemModel *)index.model();
            model->blockSignals(true);
            model->setData(index, QImage(), Qt::DecorationRole);
            model->blockSignals(false);

            QComboBox * combobox = new ComboBoxLoopMode(parent);
            combobox->setStyleSheet("QComboBox{ border: 3px solid " + highlightColor.name() + "; }");
            combobox->setProperty(DECO_PROPERTY, previousDecoration);
            widget = combobox;
        }
        else if (isKey)
        {
            SpinBoxKey * spin = new SpinBoxKey(parent);
            spin->setMinimum(0);
            spin->setMaximum(127);
            spin->setStyleSheet("SpinBoxKey{ border: 3px solid " + highlightColor.name() + "; }"
                                "SpinBoxKey::down-button{width:0px;} SpinBoxKey::up-button{width:0px;} ");
            widget = spin;
        }
        else if (nbDecimales == 0)
        {
            QSpinBox * spin = new QSpinBox(parent);
            spin->setMinimum(-2147483647);
            spin->setMaximum(2147483647);
            spin->setStyleSheet("QSpinBox{ border: 3px solid " + highlightColor.name() + "; }"
                                "QSpinBox::down-button{width:0px;} QSpinBox::up-button{width:0px;} ");
            widget = spin;
        }
        else
        {
            QDoubleSpinBox * spin = new QDoubleSpinBox(parent);
            spin->setMinimum(-1000000);
            spin->setMaximum(1000000);
            spin->setSingleStep(.1);
            spin->setStyleSheet("QDoubleSpinBox{ border: 3px solid " + highlightColor.name() + "; }"
                                "QDoubleSpinBox::down-button{width:0px;} QDoubleSpinBox::up-button{width:0px;} ");
            spin->setDecimals(nbDecimales);
            widget = spin;
        }
    }
    else
    {
        // Etendue
        SpinBoxRange * spin;
        if (isKey)
            spin = new SpinBoxKeyRange(parent);
        else
            spin = new SpinBoxVelocityRange(parent);
        spin->setStyleSheet("SpinBoxRange{ border: 3px solid " + highlightColor.name() + "; }"
                            "SpinBoxRange::down-button{width:0px;} SpinBoxRange::up-button{width:0px;} ");
        widget = spin;
    }

#ifdef Q_OS_MAC
    QFont font = parent->font();
    font.setPixelSize(10);
    widget->setFont(font);
#endif

    return widget;
}

void TableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop);

    if (!isNumeric)
    {
        SpinBoxRange * spin = (SpinBoxRange *)editor;
        if (index.data().isNull())
            spin->setText("0" + SpinBoxRange::SEPARATOR + "127");
        else
            spin->setText(index.data().toString());
    }
    else if (isLoop)
    {
        QComboBox * combobox = (QComboBox *)editor;
        if (index.data(Qt::UserRole).isNull())
            combobox->setCurrentIndex(0);
        else
        {
            switch (index.data(Qt::UserRole).toInt())
            {
            case 1:
                combobox->setCurrentIndex(1);
                break;
            case 3:
                combobox->setCurrentIndex(2);
                break;
            case 0: default:
                combobox->setCurrentIndex(0);
                break;
            }
        }
    }
    else if (isKey)
    {
        SpinBoxKey * spin = (SpinBoxKey *)editor;
        if (index.data().isNull())
            spin->setValue(60); // Default value
        else
            spin->setValue(Config::getInstance()->getKeyNum(index.data().toString()));
    }
    else if (nbDecimales > 0)
    {
        QDoubleSpinBox * spin = (QDoubleSpinBox *)editor;
        if (!index.data().isNull())
            spin->setValue(index.data().toString().replace(",", ".").toDouble());
        else
            spin->setValue(0);
    }
    else
    {
        QSpinBox * spin = (QSpinBox *)editor;
        if (!index.data().isNull())
            spin->setValue(index.data().toString().toInt());
        else
            spin->setValue(0);
    }
}

void TableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop);

    if (isLoop)
    {
        QComboBox * combobox = (QComboBox *)editor;

        int currentData = combobox->itemData(combobox->currentIndex(), Qt::UserRole).toInt();
        model->setData(index, currentData, Qt::UserRole);

        switch (currentData)
        {
        case 0:
            model->setData(index, QImage(":/icones/loop_off.png"), Qt::DecorationRole);
            editor->setProperty(DECO_PROPERTY, QVariant());
            break;
        case 1:
            model->setData(index, QImage(":/icones/loop_on.png"), Qt::DecorationRole);
            editor->setProperty(DECO_PROPERTY, QVariant());
            break;
        case 3:
            model->setData(index, QImage(":/icones/loop_on_end.png"), Qt::DecorationRole);
            editor->setProperty(DECO_PROPERTY, QVariant());
            break;
        }
    }
    else if (nbDecimales > 0 && isNumeric)
    {
        QDoubleSpinBox * spin = (QDoubleSpinBox*)editor;
        model->setData(index, QString::number(spin->value(), 'f', nbDecimales), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void TableDelegate::destroyEditor(QWidget * editor, const QModelIndex & index) const
{
    bool isNumeric, isKey, isLoop;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop);

    if (isLoop && !editor->property(DECO_PROPERTY).isNull())
    {
        // Restore the previous decoration
        QAbstractItemModel * model = (QAbstractItemModel *)index.model();
        model->blockSignals(true);
        model->setData(index, editor->property(DECO_PROPERTY), Qt::DecorationRole);
        model->blockSignals(false);
    }

    QStyledItemDelegate::destroyEditor(editor, index);
}
#endif
void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop);

    if (isLoop)
    {
        // Fond
        QStyleOptionViewItemV4 opt(option);
        initStyleOption(&opt, index);
        painter->fillRect(option.rect, opt.backgroundBrush);

        // Image
        QImage image = index.data(Qt::DecorationRole).value<QImage>();
        painter->drawPixmap(opt.rect.left() + (opt.rect.width() - image.width()) / 2,
                            opt.rect.top() + (opt.rect.height() - image.height()) / 2,
                            image.width(), image.height(), QPixmap::fromImage(image));
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

void TableDelegate::getType(bool &isNumeric, bool &isKey, int &nbDecimales, int numRow, bool &isLoop) const
{
    isNumeric = true;
    isKey = false;
    isLoop = false;
    nbDecimales = 0;

    if (_table->rowCount() == 50)
    {
        // Table instrument
        switch (numRow)
        {
        case 4:
            isNumeric = false;
            isKey = true;
            break;
        case 5:
            isNumeric = false;
            break;
        case 6: case 7: case 14: case 19: case 27: case 37: case 42: case 43:
            nbDecimales = 1;
            break;
        case 15: case 16: case 17: case 18: case 20:
        case 23: case 24: case 25: case 26: case 28:
        case 33: case 34: case 38: case 39:
            nbDecimales = 3;
            break;
        case 9: case 44:
            isKey = true;
            break;
        case 8:
            // Loop mode
            isLoop = true;
            break;
        }
    }
    else
    {
        // Table preset
        switch (numRow)
        {
        case 4:
            isNumeric = false;
            isKey = true;
            break;
        case 5:
            isNumeric = false;
            break;
        case 6: case 7: case 12: case 17: case 25: case 35: case 39: case 40:
            nbDecimales = 1;
            break;
        case 11: case 13: case 14: case 15: case 16: case 18:
        case 21: case 22: case 23: case 24: case 26: case 31:
        case 32: case 36: case 37:
            nbDecimales = 3;
            break;
        }
    }
}
