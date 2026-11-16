/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/


#ifndef TIMBREDETAILWIDGET_H
#define TIMBREDETAILWIDGET_H

#include <QWidget>
#include "timbrepalette.h"
class QLabel;
class QScrollArea;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;

// Details of the key selected in the timbre view: a fixed title with the label of its root key, then notices and
// sections that can be folded (state kept in the configuration), in a vertical scroll area
class TimbreDetailWidget : public QWidget
{
    Q_OBJECT

public:
    // Sections, in their display order
    enum Section
    {
        SECTION_DEVIATION,
        SECTION_JUNCTIONS,
        SECTION_RENDERING,
        SECTION_REMOVAL,
        SECTION_SAMPLES,
        SECTION_MEASURES,
        SECTION_COUNT
    };

    // Shape and colored text, like the marks of the map
    struct Badge
    {
        // Constructor instead of member initializers: the struct is a default argument in the enclosing class
        Badge() : valid(false), shape(TimbrePalette::SHAPE_CHECK) {}
        bool valid;
        TimbrePalette::Shape shape;
        QColor color;
        QString text;
        QString tooltip;
    };
    static Badge makeBadge(TimbrePalette::Shape shape, QColor color, QString text, QString tooltip = QString());
    static Badge shapeOnly(const Badge &badge); // Same badge without its text

    explicit TimbreDetailWidget(QWidget * parent = nullptr);

    // Width without horizontal scroll bar, which only grows while the same instrument is displayed (otherwise the graph
    // next to it would change its width at each click): resetWidth() when the instrument changes
    QSize sizeHint() const override;
    void resetWidth();

    // Content rebuilt between beginUpdate() and endUpdate(), the sections not filled in between being hidden
    void beginUpdate();
    void setTitle(QString text, const Badge &badge = Badge()); // No title: no header
    void setSubtitle(QString text, const Badge &badge = Badge());
    void addNotice(QString text);
    // The summary is shown when the section is folded, the badge always
    void beginSection(Section section, QString title, QString summary = QString(), const Badge &badge = Badge());
    void addColumnHeaders(const QStringList &headers); // Above the values of the next rows
    void addRow(QString label, const QStringList &values, QString tooltip = QString(), const Badge &badge = Badge());
    void addSubtitle(QString text); // Group of rows in a section
    void addNote(QString text);
    void endUpdate();

private:
    struct SectionWidgets
    {
        QWidget * container = nullptr;
        QWidget * header = nullptr; // Clickable, for folding
        QHBoxLayout * headerLayout = nullptr;
        QLabel * title = nullptr;
        QWidget * badge = nullptr;
        QLabel * summary = nullptr;
        QWidget * body = nullptr; // Recreated at each update
        QGridLayout * grid = nullptr;
        int row = 0;
        bool used = false;
    };

    void createSection(Section section);
    void setExpanded(Section section, bool expanded);
    void updateSection(Section section);
    QWidget * createBadge(const Badge &badge, QWidget * parent);
    void setBadge(QHBoxLayout * layout, QWidget *&current, const Badge &badge);
    QLabel * createLabel(QString text, QString objectName, QWidget * parent);
    void updateWidth();

    TimbrePalette _palette;
    QWidget * _header;
    QLabel * _title, * _subtitle;
    QWidget * _subtitleLine;
    QHBoxLayout * _titleLayout, * _subtitleLayout;
    QWidget * _titleBadge, * _subtitleBadge;
    QScrollArea * _scroll;
    QWidget * _content;
    QVBoxLayout * _contentLayout;
    QList<QLabel *> _notices;
    SectionWidgets _sections[SECTION_COUNT];
    bool _expanded[SECTION_COUNT];
    Section _currentSection;
    int _indent;
    int _neededWidth;
};

#endif // TIMBREDETAILWIDGET_H
