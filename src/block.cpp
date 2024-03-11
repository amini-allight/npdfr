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

Block::Block(f64 left, f64 right, f64 top, f64 bottom, const string& text)
    : _left(left)
    , _right(right)
    , _top(top)
    , _bottom(bottom)
    , _text(text)
{

}

optional<SearchResultLocation> Block::find(const string& search, const SearchResultLocation& previous) const
{
    size_t result = _text.find(search, previous.characterIndex + search.size());

    if (result == string::npos)
    {
        return {};
    }

    SearchResultLocation location = previous;
    location.characterIndex = result;

    return location;
}

optional<SearchResultLocation> Block::rfind(const string& search, const SearchResultLocation& previous) const
{
    size_t result = _text.rfind(search, previous.characterIndex);

    if (result == string::npos)
    {
        return {};
    }

    SearchResultLocation location = previous;
    location.characterIndex = result;

    return location;
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

vector<string> Block::lines() const
{
    vector<string> lines(height(), string(width(), ' '));

    i32 x = 0;
    i32 y = 0;

    for (char c : _text)
    {
        if (c == '\n')
        {
            x = 0;
            y++;
        }
        else
        {
            lines.at(y).at(x) = c;
            x++;
        }
    }

    return lines;
}
