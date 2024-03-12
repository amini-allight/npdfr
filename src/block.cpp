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
#include "block.hpp"
#include "charwise.hpp"

Block::Block(f64 left, f64 right, f64 top, f64 bottom, const string& text)
    : _left(left)
    , _right(right)
    , _top(top)
    , _bottom(bottom)
    , _text(text)
{

}

vector<SearchResultLocation> Block::search(const string& search) const
{
    vector<SearchResultLocation> results;

    if (search.empty())
    {
        return results;
    }

    size_t offset = 0;

    while (true)
    {
        size_t index = charwiseFind(_text, search, offset);

        if (index == string::npos)
        {
            break;
        }

        offset = index + charwiseSize(search);

        results.push_back(SearchResultLocation("", 0, 0, index));
    }

    return results;
}

i32 Block::width() const
{
    i32 width = 0;

    i32 lineWidth = 0;

    for (char c : _text)
    {
        if (c == '\n')
        {
            width = max(width, lineWidth);
            lineWidth = 0;
        }
        else
        {
            lineWidth++;
        }
    }

    width = max(width, lineWidth);

    return width;
}

i32 Block::height() const
{
    i32 height = 1;

    for (char c : _text)
    {
        if (c == '\n')
        {
            height++;
        }
    }

    return height;
}

f64 Block::left() const
{
    return _left;
}

f64 Block::right() const
{
    return _right;
}

f64 Block::top() const
{
    return _top;
}

f64 Block::bottom() const
{
    return _bottom;
}

const string& Block::text() const
{
    return _text;
}

vector<vector<string>> Block::grid() const
{
    vector<vector<string>> grid(height(), vector<string>(width(), " "));

    i32 x = 0;
    i32 y = 0;

    for (const string& c : splitUTF8(_text))
    {
        if (c == "\n")
        {
            x = 0;
            y++;
        }
        else
        {
            grid.at(y).at(x) = c;
            x++;
        }
    }

    return grid;
}

tuple<i32, i32> Block::locateSearchInGrid(const SearchResultLocation& location) const
{
    vector<string> chars = splitUTF8(_text);

    i32 x = 0;
    i32 y = 0;

    for (size_t i = 0; i < location.characterIndex; i++)
    {
        const string& c = chars.at(i);

        if (c == "\n")
        {
            x = 0;
            y++;
        }
        else
        {
            x++;
        }
    }

    return { x, y };
}
