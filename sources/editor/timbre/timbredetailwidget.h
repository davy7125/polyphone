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
class QHBoxLayout;
class QVBoxLayout;
class QTreeWidgetItem;
class TimbreDetailTree;

// Details of the key selected in the timbre view: a fixed title with the label of its root key and the notices, then a
// tree on the background of the lists whose columns align the names (and the names of the measures, as level and
// timbre, of a line written on several rows), the values, the units and the icons of the labels, put on the rows they
// come from and named by their tooltip. The sections can be folded (state kept in the configuration), a folded section
// being summed up after its title and showing the icons of its rows.
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

    // Number and its unit, in two aligned columns
    struct Value
    {
        Value() {}
        explicit Value(QString text, QString unit = QString()) : text(text), unit(unit) {}
        QString text;
        QString unit;
    };

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
    // The summary is written after the title when the section is folded
    void beginSection(Section section, QString title, QString summary = QString());
    // Badge shown as an icon, its text and tooltip in the tooltip of the icon
    void addRow(QString label, const Value &value, QString tooltip = QString(), const Badge &badge = Badge());
    // Row of a measure (sublabel) of a line, the label being only given on its first row
    void addRow(QString label, QString sublabel, const Value &value, QString tooltip = QString(), const Badge &badge = Badge());
    void addSubtitle(QString text); // Group of rows in a section
    void addNote(QString text);
    void endUpdate();

private slots:
    void onItemClicked(QTreeWidgetItem * item, int column);
    void onExpansionChanged(QTreeWidgetItem * item);

private:
    QWidget * createBadge(const Badge &badge, QWidget * parent);
    void setBadge(QHBoxLayout * layout, QWidget *&current, const Badge &badge);
    QTreeWidgetItem * createItem(int kind);
    void setBadge(QTreeWidgetItem * item, const Badge &badge);
    void updateSectionBadges();
    void updateColumns();
    void updateWidth();

    TimbrePalette _windowPalette, _listPalette;
    QColor _labelColor, _secondaryColor;
    QWidget * _header;
    QLabel * _title, * _subtitle;
    QWidget * _subtitleLine;
    QHBoxLayout * _titleLayout, * _subtitleLayout;
    QWidget * _titleBadge, * _subtitleBadge;
    QVBoxLayout * _noticeLayout;
    QList<QLabel *> _notices;
    TimbreDetailTree * _tree;
    QTreeWidgetItem * _currentSection; // Parent of the next rows
    QTreeWidgetItem * _currentGroup; // Subtitle of the next rows, if any
    bool _expanded[SECTION_COUNT];
    bool _building;
    int _neededWidth;
};

#endif // TIMBREDETAILWIDGET_H
