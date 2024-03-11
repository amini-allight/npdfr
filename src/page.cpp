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
#include "page.hpp"
#include "constants.hpp"

Page::Page(f64 width, f64 height)
    : _width(width)
    , _height(height)
{

}

void Page::add(const Block& block)
{
    _blocks.push_back(block);
}

vector<SearchResultLocation> Page::search(const string& search) const
{
    vector<SearchResultLocation> results;

    if (search.empty())
    {
        return results;
    }

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        const Block& block = _blocks.at(i);

        vector<SearchResultLocation> blockResults = block.search(search);

        for (SearchResultLocation& blockResult : blockResults)
        {
            blockResult.blockIndex = i;
        }

        results.insert(results.end(), blockResults.begin(), blockResults.end());
    }

    return results;
}

i32 Page::width() const
{
    i32 width = 0;

    for (const string& line : lines())
    {
        width = max<i32>(width, line.size());
    }

    return width;
}

i32 Page::height() const
{
    return lines().size();
}

const vector<Block>& Page::blocks() const
{
    return _blocks;
}

static bool verticalOverlap(const Block& a, const Block& b)
{
    return !(a.left() > b.right() || a.right() < b.left());
}

static bool horizontalOverlap(const Block& a, const Block& b)
{
    return !(a.top() > b.bottom() || a.bottom() < b.top());
}

static tuple<i32, i32> recursiveLocate(const vector<Block>& blocks, size_t index)
{
    const Block& block = blocks.at(index);

    f64 top = block.top();
    f64 left = block.left();

    size_t nearestTopIndex = 0;
    optional<Block> nearestTop;
    size_t nearestLeftIndex = 0;
    optional<Block> nearestLeft;

    for (size_t i = 0; i < blocks.size(); i++)
    {
        if (i == index)
        {
            continue;
        }

        const Block& candidate = blocks.at(i);

        if (
            verticalOverlap(block, candidate) &&
            candidate.bottom() <= top &&
            (!nearestTop || candidate.bottom() > nearestTop->bottom())
        )
        {
            nearestTopIndex = i;
            nearestTop = candidate;
        }

        if (
            horizontalOverlap(block, candidate) &&
            candidate.right() <= left &&
            (!nearestLeft || candidate.right() > nearestLeft->right())
        )
        {
            nearestLeftIndex = i;
            nearestLeft = candidate;
        }
    }

    i32 x = 0;

    if (nearestLeft)
    {
        x = get<0>(recursiveLocate(blocks, nearestLeftIndex)) + nearestLeft->width() + blockHorizontalSpacer;
    }

    i32 y = 0;

    if (nearestTop)
    {
        y = get<1>(recursiveLocate(blocks, nearestTopIndex)) + nearestTop->height() + blockVerticalSpacer;
    }

    return { x, y };
}

vector<string> Page::lines() const
{
    vector<tuple<i32, i32>> offsets(_blocks.size());

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        offsets.at(i) = recursiveLocate(_blocks, i);
    }

    i32 width = 0;
    i32 height = 0;

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        const Block& block = _blocks.at(i);
        const auto [ x, y ] = offsets.at(i);

        width = max(width, x + block.width());
        height = max(height, y + block.height());
    }

    vector<string> lines(height, string(width, ' '));

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        const Block& block = _blocks.at(i);
        const auto [ offsetX, offsetY ] = offsets.at(i);

        vector<string> blockLines = block.lines();

        for (i32 y = 0; y < blockLines.size(); y++)
        {
            for (i32 x = 0; x < blockLines.front().size(); x++)
            {
                lines.at(offsetY + y).at(offsetX + x) = blockLines.at(y).at(x);
            }
        }
    }

    return lines;
}
