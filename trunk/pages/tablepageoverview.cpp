#include "tablepageoverview.h"
#include <QHeaderView>

TablePageOverview::TablePageOverview(QWidget *parent) : QTableWidget(parent)
{
    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onSort(int)));

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    // Horizontal header in bold
    this->horizontalHeader()->setHighlightSections(false);
    QFont font = this->font();
    font.setBold(true);
    this->horizontalHeader()->setFont(font);
}

void TablePageOverview::onSort(int column)
{
    Q_UNUSED(column)

    // Restore colors
    colorRows();
}

void TablePageOverview::colorRows()
{
    for (int row = 0; row < this->rowCount(); row++)
    {
        QColor color = (row % 2) ? QColor(255, 255, 200) : QColor(255, 255, 255);
        for (int col = 0; col < this->columnCount(); col++)
            this->item(row, col)->setBackgroundColor(color);
    }
}
