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
#include "layout.hpp"
#include "whitespace.hpp"

Page::Page()
{

}

void Page::add(const Block& block)
{
    _blocks.push_back(block);
}

void Page::adjustBlockOffset(float x, float y)
{
    for (Block& block : _blocks)
    {
        block.adjustBlockOffset(x, y);
    }
}

void Page::generateGrid()
{
    blockOffsets = locate(_blocks);

    i32 width = 0;
    i32 height = 0;

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        const Block& block = _blocks.at(i);
        const auto [ x, y ] = blockOffsets.at(i);

        width = max(width, x + block.width());
        height = max(height, y + block.height());
    }

    _grid = vector<vector<string>>(height, vector<string>(width, " "));

    for (size_t i = 0; i < _blocks.size(); i++)
    {
        const Block& block = _blocks.at(i);
        const auto [ offsetX, offsetY ] = blockOffsets.at(i);

        vector<vector<string>> blockGrid = block.grid();

        for (i32 y = 0; y < blockGrid.size(); y++)
        {
            for (i32 x = 0; x < blockGrid.front().size(); x++)
            {
                const string& src = blockGrid.at(y).at(x);;
                string& dst = _grid.at(offsetY + y).at(offsetX + x);

                if (isWhitespace(src) && !isWhitespace(dst))
                {
                    continue;
                }

                dst = src;
            }
        }
    }
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
            tie(blockResult.x, blockResult.y) = locateSearchInGrid(blockResult);
        }

        results.insert(results.end(), blockResults.begin(), blockResults.end());
    }

    // Account for the possibility of multiple overlapping searches from different blocks
    for (size_t i = 0; i < results.size();)
    {
        const SearchResultLocation& result = results[i];

        bool overlap = false;

        for (size_t j = 0; j < results.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            const SearchResultLocation& other = results[j];

            if (result.overlap(other, search))
            {
                overlap = true;
                break;
            }
        }

        if (overlap)
        {
            results.erase(results.begin() + i);
        }
        else
        {
            i++;
        }
    }

    sort(results.begin(), results.end());

    return results;
}

i32 Page::width() const
{
    i32 width = 0;

    for (const vector<string>& line : grid())
    {
        width = max<i32>(width, line.size());
    }

    return width;
}

i32 Page::height() const
{
    return grid().size();
}

const vector<Block>& Page::blocks() const
{
    return _blocks;
}

const vector<vector<string>>& Page::grid() const
{
    return _grid;
}

tuple<i32, i32> Page::locateSearchInGrid(const SearchResultLocation& location) const
{
    auto [ blockX, blockY ] = blockOffsets.at(location.blockIndex);

    auto [ x, y ] = _blocks.at(location.blockIndex).locateSearchInGrid(location);

    return { blockX + x, blockY + y };
}
