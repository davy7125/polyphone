#ifndef SOUNDFONTCOMMENTCELL_H
#define SOUNDFONTCOMMENTCELL_H

#include <QWidget>
class SoundfontCommentData;

namespace Ui {
class SoundfontCommentCell;
}

class SoundfontCommentCell : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontCommentCell(QWidget *parent = 0);
    ~SoundfontCommentCell();

    void initialize(SoundfontCommentData * data, int level);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::SoundfontCommentCell *ui;
};

#endif // SOUNDFONTCOMMENTCELL_H
