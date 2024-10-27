/*
Copyright 2024 Amini Allight

This file is part of npdfr.

npdfr is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

npdfr is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with npdfr. If not, see <https://www.gnu.org/licenses/>.
*/
#include "outline.hpp"
#include "charwise.hpp"
#include "whitespace.hpp"
#include "constants.hpp"

Outline::Outline(const string& title, i32 page)
    : _title(trimWhitespace(title))
    , _page(page)
{

}

void Outline::add(const Outline& outline)
{
    _outline.push_back(outline);
}

const string& Outline::title() const
{
    return _title;
}

i32 Outline::page() const
{
    return _page;
}

const vector<Outline>& Outline::outline() const
{
    return _outline;
}

i32 Outline::pageIndexAt(i32 selectIndex, i32* y) const
{
    if (selectIndex == *y)
    {
        return _page;
    }

    (*y)++;

    for (const Outline& outline : _outline)
    {
        i32 pageIndex = outline.pageIndexAt(selectIndex, y);

        if (pageIndex >= 0)
        {
            return pageIndex;
        }
    }

    return -1;
}

i32 Outline::width() const
{
    i32 width = charwiseSize(_title + " (" + to_string(_page + 1) + ")");

    for (const Outline& outline : _outline)
    {
        width = max<i32>(width, blockHorizontalSpacer + outline.width());
    }

    return width;
}

i32 Outline::height() const
{
    i32 height = 1;

    for (const Outline& outline : _outline)
    {
        height += outline.height();
    }

    return height;
}
