#include "soundfontcomment.h"
#include "ui_soundfontcomment.h"
#include "soundfontcommentdata.h"
#include "soundfontcommentcell.h"
#include <QLabel>

SoundfontComment::SoundfontComment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontComment)
{
    ui->setupUi(this);
}

SoundfontComment::~SoundfontComment()
{
    delete ui;
}

bool sortRecentFirst(const SoundfontCommentData *s1, const SoundfontCommentData *s2)
{
    return s1->getCreationTime() > s2->getCreationTime();
}

bool sortLastFirst(const SoundfontCommentData *s1, const SoundfontCommentData *s2)
{
    return s1->getCreationTime() < s2->getCreationTime();
}

void SoundfontComment::display(QList<SoundfontCommentData *> data, int level)
{
    // Order the list
    if (level == 0)
        qSort(data.begin(), data.end(), sortRecentFirst);
    else
        qSort(data.begin(), data.end(), sortLastFirst);

    // Add each comment
    for (int i = 0; i < data.count(); i++)
    {
        SoundfontCommentCell * cell = new SoundfontCommentCell(this);
        cell->initialize(data[i], level);
        ui->verticalLayout->addWidget(cell);

        // Display the children
        this->display(data[i]->getChildren(), level + 1);
    }

    if (level == 0 && data.isEmpty())
    {
        QLabel * label = new QLabel(this);
        label->setText(trUtf8("Pas de commentaire."));
        ui->verticalLayout->addWidget(label);
    }
}
