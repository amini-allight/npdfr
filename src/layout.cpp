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
#include "layout.hpp"
#include "constants.hpp"

static bool verticalAlignment(const Block& a, const Block& b)
{
    return !(a.left() > b.right() || a.right() < b.left());
}

static bool horizontalAlignment(const Block& a, const Block& b)
{
    return !(a.top() > b.bottom() || a.bottom() < b.top());
}

static bool overlap(const Block& a, const Block& b)
{
    return !(
        a.left() > b.right() ||
        b.left() > a.right() ||
        a.top() > b.bottom() ||
        b.top() > a.bottom()
    );
}

bool overlapVerticalBefore(const Block& block, const Block& candidate)
{
    return overlap(block, candidate) && candidate.top() < block.top();
}

bool overlapHorizontalBefore(const Block& block, const Block& candidate)
{
    if (overlap(block, candidate) && candidate.left() < block.left())
    {
        /*
         * This prevents a loop where two overlapping blocks can exist where one
         * is leftmost and the other is topmost, causing infinite recursion
         * because they depend on each other for positioning, this condition
         * causes the topmost one to take precedence
         */
        if (candidate.top() > block.top())
        {
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

tuple<i32, i32> recursiveLocate(const vector<Block>& blocks, size_t index)
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
            verticalAlignment(block, candidate) &&
            (candidate.bottom() <= top || overlapVerticalBefore(block, candidate)) &&
            (!nearestTop || candidate.bottom() > nearestTop->bottom())
        )
        {
            nearestTopIndex = i;
            nearestTop = candidate;
        }

        if (
            horizontalAlignment(block, candidate) &&
            (candidate.right() <= left || overlapHorizontalBefore(block, candidate)) &&
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
