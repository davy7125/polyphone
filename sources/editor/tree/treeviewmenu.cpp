#include "treeviewmenu.h"
#include "soundfontmanager.h"
#include "dialog_list.h"
#include <QMessageBox>
#include <QDebug>

TreeViewMenu::TreeViewMenu(QWidget * parent) : QMenu(parent),
    _dialogList(new DialogList(parent))
{   
    // Associate
    _associateAction = new QAction(trUtf8("Associer à..."), this);
    connect(_associateAction, SIGNAL(triggered()), this, SLOT(associate()));
    this->addAction(_associateAction);

    // Replace
    _replaceAction = new QAction(trUtf8("Remplacer par..."), this);
    connect(_replaceAction, SIGNAL(triggered()), this, SLOT(replace()));
    this->addAction(_replaceAction);
    this->addSeparator();

    connect(_dialogList, SIGNAL(elementSelected(EltID,bool)), this, SLOT(itemSelectedFromList(EltID,bool)));

    // Copy
    _copyAction = new QAction(trUtf8("Copier"), this);
    _copyAction->setShortcut(QString("Ctrl+C"));
    connect(_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    this->addAction(_copyAction);

    // Paste
    _pasteAction = new QAction(trUtf8("Coller"), this);
    _pasteAction->setShortcut(QString("Ctrl+V"));
    connect(_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    this->addAction(_pasteAction);

    // Delete
    _removeAction = new QAction(trUtf8("Supprimer"), this);
    _removeAction->setShortcut(QString("Del"));
    connect(_removeAction, SIGNAL(triggered()), this, SLOT(remove()));
    this->addAction(_removeAction);
    this->addSeparator();

    // Rename
    _renameAction = new QAction(trUtf8("Renommer..."), this);
    _renameAction->setShortcut(Qt::Key_F2);
    connect(_renameAction, SIGNAL(triggered()), this, SLOT(rename()));
    this->addAction(_renameAction);
}

TreeViewMenu::~TreeViewMenu()
{
    _dialogList->deleteLater();
}

void TreeViewMenu::initialize(IdList ids)
{
    _currentIds = ids;

    // Associate
    bool associate = true;
    foreach (EltID id, ids)
    {
        if (id.typeElement != elementSmpl && id.typeElement != elementInst)
        {
            associate = false;
            break;
        }
    }
    _associateAction->setEnabled(associate);

    // Replace
    _replaceAction->setEnabled(ids.count() == 1 && (ids[0].typeElement == elementInstSmpl || ids[0].typeElement == elementPrstInst));

    // Rename
    bool rename = true;
    foreach (EltID id, ids)
    {
        if (id.typeElement != elementSmpl && id.typeElement != elementInst && id.typeElement != elementPrst)
        {
            rename = false;
            break;
        }
    }
    if (rename)
    {
        _renameAction->setEnabled(true);
        _renameAction->setText(ids.count() == 1 ? trUtf8("Renommer...") : trUtf8("&Renommer en masse..."));
    }
    else
    {
        _renameAction->setText(trUtf8("Renommer..."));
        _renameAction->setEnabled(false);
    }
}

void TreeViewMenu::associate()
{
    if (!_currentIds.empty())
        _dialogList->showDialog(_currentIds[0], true);
}

void TreeViewMenu::replace()
{
    if (_currentIds.count() == 1)
        _dialogList->showDialog(_currentIds[0], false);
}

void TreeViewMenu::remove()
{
    // Remove all the selected elements
    int message = 1;
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID id, _currentIds)
    {
        if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementInstSmpl ||
                id.typeElement == elementPrst || id.typeElement == elementPrstInst)
            sm->remove(id, &message);
    }

    if (message % 2 == 0)
        QMessageBox::warning((QWidget*)this->parent(), trUtf8("Attention"),
                             trUtf8("Impossible de supprimer un échantillon s'il est utilisé par un instrument."));
    if (message % 3 == 0)
        QMessageBox::warning((QWidget*)this->parent(), trUtf8("Attention"),
                             trUtf8("Impossible de supprimer un instrument s'il est utilisé par un preset."));

    sm->endEditing("tree:remove");
}

void TreeViewMenu::itemSelectedFromList(EltID id, bool isAssociation)
{
    if (isAssociation)
    {
        if (!_currentIds.empty())
            associate(_currentIds, id);
    }
    else
    {
        if (_currentIds.count() == 1)
            replace(id, _currentIds[0]);
    }
}

void TreeViewMenu::associate(IdList ids, EltID idDest)
{
    // Type of the element(s) that will be created
    AttributeType champ;
    if (idDest.typeElement == elementInst)
    {
        idDest.typeElement = elementInstSmpl;
        champ = champ_sampleID;
    }
    else
    {
        idDest.typeElement = elementPrstInst;
        champ = champ_instrument;
    }
    AttributeValue val;

    // For each element to associate
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID idSrc, ids)
    {
        // Create a division
        idDest.indexElt2 = sm->add(idDest);

        // Association of idSrc in idDest
        val.wValue = idSrc.indexElt;
        sm->set(idDest, champ, val);
        if (champ == champ_sampleID)
        {
            // Pan
            if (sm->get(idSrc, champ_sfSampleType).sfLinkValue == rightSample ||
                    sm->get(idSrc, champ_sfSampleType).sfLinkValue == RomRightSample)
                val.shValue = 500;
            else if (sm->get(idSrc, champ_sfSampleType).sfLinkValue == leftSample ||
                     sm->get(idSrc, champ_sfSampleType).sfLinkValue == RomLeftSample)
                val.shValue = -500;
            else
                val.shValue = 0;
            sm->set(idDest, champ_pan, val);
        }
        else
        {
            // Key range
            int keyMin = 127;
            int keyMax = 0;
            EltID idLinked = idSrc;
            idLinked.typeElement = elementInstSmpl;
            foreach (int i, sm->getSiblings(idLinked))
            {
                idLinked.indexElt2 = i;
                if (sm->isSet(idLinked, champ_keyRange))
                {
                    keyMin = qMin(keyMin, (int)sm->get(idLinked, champ_keyRange).rValue.byLo);
                    keyMax = qMax(keyMax, (int)sm->get(idLinked, champ_keyRange).rValue.byHi);
                }
            }
            AttributeValue value;
            if (keyMin < keyMax)
            {
                value.rValue.byLo = keyMin;
                value.rValue.byHi = keyMax;
            }
            else
            {
                value.rValue.byLo = 0;
                value.rValue.byHi = 127;
            }
            sm->set(idDest, champ_keyRange, value);
        }
    }

    sm->endEditing("command:associate");
}

void TreeViewMenu::replace(EltID idSrc, EltID idDest)
{
    // Checks
    if (idDest.typeElement != elementInstSmpl && idDest.typeElement != elementPrstInst)
        return;
    if (idSrc.typeElement != elementSmpl && idSrc.typeElement != elementInst)
        return;

    // Type of value to change
    AttributeType champ;
    if (idSrc.typeElement == elementSmpl)
        champ = champ_sampleID;
    else
        champ = champ_instrument;

    // Replace the link of a division
    AttributeValue val;
    val.wValue = idSrc.indexElt;
    SoundfontManager::getInstance()->set(idDest, champ, val);
    SoundfontManager::getInstance()->endEditing("command:replace");
}

void TreeViewMenu::rename()
{
    qDebug() << "rename";
}

void TreeViewMenu::copy()
{
    qDebug() << "copy";
}

void TreeViewMenu::paste()
{
    qDebug() << "paste";
}
