#ifndef SOUNDFONTCOMMENT_H
#define SOUNDFONTCOMMENT_H

#include <QWidget>
class SoundfontCommentData;

namespace Ui {
class SoundfontComment;
}

class SoundfontComment : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontComment(QWidget *parent = 0);
    ~SoundfontComment();

    void display(QList<SoundfontCommentData *> data, int level = 0);

private:
    Ui::SoundfontComment *ui;
};

#endif // SOUNDFONTCOMMENT_H
