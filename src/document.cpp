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
#include "document.hpp"
#include "constants.hpp"

Document::Document()
{

}

void Document::add(const Page& page)
{
    _pages.push_back(page);
}

void Document::setOutline(const vector<Outline>& outline)
{
    _outline = outline;
}

void Document::generateGrid()
{
    size_t workerCount = thread::hardware_concurrency();

    vector<thread> workers(workerCount);

    size_t chunkSize = max<size_t>(_pages.size() / workerCount, 1);

    for (size_t workerIndex = 0; workerIndex < workerCount; workerIndex++)
    {
        workers[workerIndex] = thread([this, workerIndex, workerCount, chunkSize]() -> void {
            size_t start = workerIndex * chunkSize;

            if (start >= _pages.size())
            {
                return;
            }

            size_t end = workerIndex + 1 == workerCount
                ? _pages.size()
                : min((workerIndex + 1) * chunkSize, _pages.size());

            for (size_t i = start; i < end; i++)
            {
                _pages[i].generateGrid();
            }
        });
    }

    for (thread& worker : workers)
    {
        worker.join();
    }
}

vector<SearchResultLocation> Document::search(const string& search) const
{
    vector<SearchResultLocation> results;

    if (search.empty())
    {
        return results;
    }

    for (size_t i = 0; i < _pages.size(); i++)
    {
        const Page& page = _pages.at(i);

        vector<SearchResultLocation> pageResults = page.search(search);

        for (SearchResultLocation& pageResult : pageResults)
        {
            pageResult.pageIndex = i;
        }

        results.insert(results.end(), pageResults.begin(), pageResults.end());
    }

    return results;
}

const vector<Page>& Document::pages() const
{
    return _pages;
}

const vector<Outline>& Document::outline() const
{
    return _outline;
}

i32 Document::outlinePageIndexAt(i32 selectIndex) const
{
    i32 y = 0;

    for (const Outline& outline : _outline)
    {
        i32 pageIndex = outline.pageIndexAt(selectIndex, &y);

        if (pageIndex >= 0)
        {
            return pageIndex;
        }
    }

    return -1;
}

i32 Document::outlineWidth() const
{
    i32 width = 0;

    for (const Outline& outline : _outline)
    {
        width = max<i32>(width, blockHorizontalSpacer + outline.width());
    }

    return width;
}

i32 Document::outlineHeight() const
{
    i32 height = 0;

    for (const Outline& outline : _outline)
    {
        height += outline.height();
    }

    return height;
}
