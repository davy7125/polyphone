#include "tablewidget.h"
#include "spinboxkey.h"
#include "spinboxrange.h"
#include "config.h"
#include <QApplication>


//////////////////////////////////////////// TABLE WIDGET ////////////////////////////////////////////

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    setItemDelegate(new TableDelegate(this));
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
}
void TableWidget::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
    {
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    }
    this->setColumnCount(0);
}
void TableWidget::addColumn(int column, QString title)
{
    this->insertColumn(column);
    for (int i = 0; i < this->rowCount(); i++)
        this->setItem(i, column, new QTableWidgetItem());
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(title));

    // Ajout d'un élément couleur
    _listColors.insert(column, QColor(0, 0, 0));
}
void TableWidget::setID(EltID id, int colonne)
{
    QString str;
    switch(id.typeElement)
    {
    case elementInstGen: case elementInst: str = "Inst"; break;
    case elementInstSmplGen: case elementInstSmpl: str = "InstSmpl"; break;
    case elementPrstGen: case elementPrst:  str = "Prst"; break;
    case elementPrstInstGen: case elementPrstInst: str = "PrstInst"; break;
    default: break;
    }
    item(0, colonne)->setText(str);
    this->item(1, colonne)->setText(QString::number(id.indexSf2));
    this->item(2, colonne)->setText(QString::number(id.indexElt));
    this->item(3, colonne)->setText(QString::number(id.indexElt2));
}
EltID TableWidget::getID(int colonne)
{
    EltID id(elementUnknown, 0, 0, 0, 0);
    if (this->columnCount() > colonne)
    {
        if (item(0, colonne)->text() == "Inst")
            id.typeElement = elementInst;
        else if (item(0, colonne)->text() == "InstSmpl")
            id.typeElement = elementInstSmpl;
        else if (item(0, colonne)->text() == "Prst")
            id.typeElement = elementPrst;
        else if (item(0, colonne)->text() == "PrstInst")
            id.typeElement = elementPrstInst;
        id.indexSf2 = item(1, colonne)->text().toInt();
        id.indexElt = item(2, colonne)->text().toInt();
        id.indexElt2 = item(3, colonne)->text().toInt();
    }
    return id;
}
void TableWidget::setEnlighted(int colonne, bool isEnlighted)
{
    if (colonne >= this->columnCount())
        return;

    if (isEnlighted)
        _listColors[colonne] = QColor(70, 120, 210);
    else
        _listColors[colonne] = QColor(0, 0, 0);

    _timer->start(30);
}
void TableWidget::updateColors()
{
    int minChange = 40;
    bool toutPareil = true;
    for (int i = 0; i < this->columnCount(); i++)
    {
        if (this->horizontalHeaderItem(i))
        {
            QColor couleur1 = this->horizontalHeaderItem(i)->foreground().color();
            QColor couleur2 = _listColors.at(i);
            if (couleur1 != couleur2)
            {
                int deltaRouge = qMax(-minChange, qMin(minChange, couleur2.red() - couleur1.red()));
                int deltaVert = qMax(-minChange, qMin(minChange, couleur2.green() - couleur1.green()));
                int deltaBleu = qMax(-minChange, qMin(minChange, couleur2.blue() - couleur1.blue()));
                couleur1.setRed(couleur1.red() + deltaRouge);
                couleur1.setGreen(couleur1.green() + deltaVert);
                couleur1.setBlue(couleur1.blue() + deltaBleu);
                this->horizontalHeaderItem(i)->setForeground(couleur1);
            }
            if (couleur1 != couleur2)
                toutPareil = false;
        }
    }
    if (toutPareil)
        _timer->stop();
}
void TableWidget::setColumnCount(int columns)
{
    QTableWidget::setColumnCount(columns);
    _listColors.clear();
    for (int i = 0; i < columns; i++)
        _listColors << QColor(0, 0, 0);
}
void TableWidget::removeColumn(int column)
{
    QTableWidget::removeColumn(column);
    _listColors.removeAt(column);
}

void TableWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
    {
        if (selectedItems().count() == 1)
            QApplication::clipboard()->setText(selectedItems().first()->text());
    }
    else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier))
    {
        if (!selectedItems().isEmpty())
            selectedItems().first()->setText(QApplication::clipboard()->text());
    }
    else if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
        // Touche retour ou suppr (efface la cellule)
        if (!selectedItems().isEmpty())
            selectedItems().first()->setText("");
    }
    QTableWidget::keyPressEvent(event);
}


////////////////////////////////////////////// DELEGATE //////////////////////////////////////////////

QWidget * TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    bool isNumeric, isKey;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row());

    QWidget * widget;
    QColor highlightColor = parent->palette().color(QPalette::Highlight);
    if (isNumeric)
    {
        if (isKey)
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
    if (index.row() == 4 || index.row() == 5)
    {
        SpinBoxRange * spin = (SpinBoxRange *)editor;
        if (index.data().isNull())
            spin->setText("0" + SpinBoxRange::SEPARATOR + "127");
        else
            spin->setText(index.data().toString());
    }
    else if (_table->rowCount() == 50 && (index.row() == 9 || index.row() == 44))
    {
        SpinBoxKey * spin = (SpinBoxKey *)editor;
        if (index.data().isNull())
            spin->setValue(60); // valeur par défaut
        else
            spin->setValue(Config::getInstance()->getKeyNum(index.data().toString()));
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}

void TableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    bool isNumeric, isKey;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row());

    if (nbDecimales > 0 && isNumeric)
    {
        QDoubleSpinBox * spin = (QDoubleSpinBox*)editor;
        model->setData(index, QString::number(spin->value(), 'f', nbDecimales), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

void TableDelegate::getType(bool &isNumeric, bool &isKey, int &nbDecimales, int numRow) const
{
    isNumeric = true;
    isKey = false;
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
