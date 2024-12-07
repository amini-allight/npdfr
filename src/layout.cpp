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

static bool gridOverlap(
    i32 aX, i32 aY, i32 aWidth, i32 aHeight,
    i32 bX, i32 bY, i32 bWidth, i32 bHeight
)
{
    i32 aLeft = aX;
    i32 aRight = aX + aWidth;
    i32 aTop = aY;
    i32 aBottom = aY + aHeight;

    i32 bLeft = bX;
    i32 bRight = bX + bWidth;
    i32 bTop = bY;
    i32 bBottom = bY + bHeight;

    return !(
        aLeft > bRight ||
        bLeft > aRight ||
        aTop > bBottom ||
        bTop > aBottom
    );
}

static void recursiveLocate(
    const vector<Block>& blocks,
    vector<tuple<i32, i32>>& offsets,
    set<size_t>& traversed,
    size_t index
)
{
    if (get<0>(offsets.at(index)) >= 0 || traversed.contains(index))
    {
        return;
    }

    traversed.insert(index);

    const Block& block = blocks.at(index);

    f64 top = block.top();
    f64 left = block.left();

    i32 x = 0;
    i32 y = 0;

    for (size_t i = 0; i < blocks.size(); i++)
    {
        if (i == index)
        {
            continue;
        }

        const Block& candidate = blocks.at(i);

        if (
            horizontalAlignment(block, candidate) &&
            (candidate.right() <= left || overlapHorizontalBefore(block, candidate))
        )
        {
            recursiveLocate(blocks, offsets, traversed, i);

            x = max(x, get<0>(offsets.at(i)) + candidate.width() + blockHorizontalSpacer);
        }

        if (
            verticalAlignment(block, candidate) &&
            (candidate.bottom() <= top || overlapVerticalBefore(block, candidate))
        )
        {
            recursiveLocate(blocks, offsets, traversed, i);

            y = max(y, get<1>(offsets.at(i)) + candidate.height() + blockVerticalSpacer);
        }
    }

    bool found;
    bool pushLeft = true;

    do
    {
        found = false;

        for (size_t i = 0; i < blocks.size(); i++)
        {
            const auto [ otherX, otherY ] = offsets[i];
            i32 otherWidth = blocks[i].width();
            i32 otherHeight = blocks[i].height();

            if (otherX < 0 || otherY < 0)
            {
                continue;
            }

            if (gridOverlap(
                x, y, block.width(), block.height(),
                otherX, otherY, otherWidth, otherHeight
            ))
            {
                found = true;

                if (pushLeft)
                {
                    x = otherX + otherWidth + blockHorizontalSpacer;
                }
                else
                {
                    y = otherY + otherHeight + blockVerticalSpacer;
                }

                pushLeft = !pushLeft;
                break;
            }
        }
    } while (found);

    offsets.at(index) = { x, y };
}

vector<tuple<i32, i32>> locate(const vector<Block>& blocks)
{
    vector<tuple<i32, i32>> offsets(blocks.size(), { -1, -1 });

    if (blocks.empty())
    {
        return offsets;
    }

    for (size_t i = 0; i < blocks.size(); i++)
    {
        set<size_t> traversed;

        recursiveLocate(blocks, offsets, traversed, i);
    }

    return offsets;
}
