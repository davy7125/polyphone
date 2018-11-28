#include "tableheaderview.h"
#include "contextmanager.h"
#include <QMouseEvent>
#include <QMenu>
#include "soundfontmanager.h"

TableHeaderView::TableHeaderView(QWidget *parent) : QHeaderView(Qt::Horizontal, parent)
{
    this->setSectionsClickable(true);
    this->setHighlightSections(true);
    this->setDefaultSectionSize(70);

    // Mute icon
    _muteIcon = ContextManager::theme()->getColoredSvg(":/icons/volume-mute.svg", QSize(12, 12), ThemeManager::HIGHLIGHTED_BACKGROUND);

    // Menu
    _menu = new QMenu(this);
    _muteAction = _menu->addAction(trUtf8("mute"));
    _muteAction->setCheckable(true);
    connect(_muteAction, SIGNAL(toggled(bool)), this, SLOT(mute(bool)));

    QAction * action = _menu->addAction(trUtf8("unmute all"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(unmuteAll(bool)));
}

TableHeaderView::~TableHeaderView()
{
    delete _menu;
}

void TableHeaderView::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::RightButton)
    {
        // Id associated to the column
        _currentSection = this->logicalIndexAt(e->pos());
        _currentId = this->model()->headerData(_currentSection, Qt::Horizontal, Qt::UserRole).value<EltID>();

        // Possibly display the menu
        if (_currentId.typeElement == elementInstSmpl || _currentId.typeElement == elementPrstInst)
        {
            _muteAction->blockSignals(true);
            _muteAction->setChecked(SoundfontManager::getInstance()->get(_currentId, champ_mute).bValue > 0);
            _muteAction->blockSignals(false);
            _menu->exec(e->globalPos());
        }
    }
    else
        QHeaderView::mousePressEvent(e);
}

void TableHeaderView::mute(bool isMute)
{
    AttributeValue val;
    val.bValue = (isMute ? 1 : 0);
    SoundfontManager::getInstance()->set(_currentId, champ_mute, val);

    // Update the header
    this->model()->setHeaderData(_currentSection, Qt::Horizontal,
                                 isMute ? QVariant::fromValue(_muteIcon) : QVariant(), Qt::DecorationRole);
}

void TableHeaderView::unmuteAll(bool unused)
{
    Q_UNUSED(unused)

    // Unmute all divisions of all instruments
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;
    val.bValue = 0;
    EltID idInst(elementInst, _currentId.indexSf2);
    foreach (int index, sm->getSiblings(idInst))
    {
        idInst.indexElt = index;
        EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
        foreach (int subIndex, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = subIndex;
            sm->set(idInstSmpl, champ_mute, val);
        }
    }

    // Unmute all divisions of all presets
    EltID idPrst(elementPrst, _currentId.indexSf2);
    foreach (int index, sm->getSiblings(idInst))
    {
        idPrst.indexElt = index;
        EltID idPrstInst(elementPrstInst, idInst.indexSf2, idInst.indexElt);
        foreach (int subIndex, sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = subIndex;
            sm->set(idPrstInst, champ_mute, val);
        }
    }

    // Remove all decorations
    for (int i = 0; i < this->model()->columnCount(); i++)
        this->model()->setHeaderData(i, Qt::Horizontal, QVariant(), Qt::DecorationRole);
}
