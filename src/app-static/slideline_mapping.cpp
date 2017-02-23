/*
 * Copyright 2014 Andreas Reischuck <https://github.com/arBmind>
 * Copyright 2014 Christian Loose <christian.loose@hamburg.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "slideline_mapping.h"

#include <QRegularExpression>

void SlideLineMapping::build(const QString &code)
{
    static const QRegularExpression re("\n|\r\n|\r");

    _line_to_slide.clear();
    _slide_to_line.clear();
    int horizontal = 0;
    int vertical = 0;
    int lineNumber = 0;

    QStringList lines = code.split(re);
    _slide_to_line.insert(qMakePair(horizontal, vertical), lineNumber+1);
    for (int i = 0; i < lines.count(); ++i) {
        ++lineNumber;
        if (isHorizontalSlideSeparator(lines, i)) {
            _line_to_slide.insert(lineNumber, qMakePair(horizontal, vertical));
            horizontal++;
            vertical = 0;
            _slide_to_line.insert(qMakePair(horizontal, vertical), lineNumber+1);
        }
        if (isVerticalSlideSeparator(lines, i)) {
            _line_to_slide.insert(lineNumber, qMakePair(horizontal, vertical));
            vertical++;
            _slide_to_line.insert(qMakePair(horizontal, vertical), lineNumber+1);
        }
    }
    _line_to_slide.insert(lineNumber, qMakePair(horizontal, vertical));
}

int SlideLineMapping::lineForSlide(const QPair<int, int>& slide) const
{
    QMap<QPair<int, int>, int>::const_iterator it = _slide_to_line.find(slide);
    if (it != _slide_to_line.end()) {
        return it.value();
    }

    return -1;
}

QPair<int, int> SlideLineMapping::slideForLine(int lineNumber) const
{
    QMap<int, QPair<int, int> >::const_iterator it = _line_to_slide.lowerBound(lineNumber);
    if (it != _line_to_slide.end()) {
        return it.value();
    }

    return qMakePair(-1, -1);
}

QMap<int, QPair<int, int> > SlideLineMapping::lineToSlide() const
{
    return _line_to_slide;
}

QMap<QPair<int, int>, int> SlideLineMapping::slideToLine() const
{
    return _slide_to_line;
}

bool SlideLineMapping::isHorizontalSlideSeparator(const QStringList &lines, int lineNumber) const
{
    static const QString horizontalMarker("---");

    return lineNumber > 1 &&
           lineNumber < lines.count()-1 &&
           lines[lineNumber-1].isEmpty() &&
           lines[lineNumber] == horizontalMarker &&
           lines[lineNumber+1].isEmpty();
}

bool SlideLineMapping::isVerticalSlideSeparator(const QStringList &lines, int lineNumber) const
{
    static const QString verticalMarker("--");

    return lineNumber > 1 &&
           lineNumber < lines.count()-1 &&
           lines[lineNumber-1].isEmpty() &&
           lines[lineNumber] == verticalMarker &&
           lines[lineNumber+1].isEmpty();
}
