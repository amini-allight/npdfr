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
#include "tools.hpp"
#include "constants.hpp"

#include <json/json.h>

static vector<Outline> parseOutline(const Json::Value& root)
{
    vector<Outline> outlines;
    outlines.reserve(root.size());

    for (int i = 0; i < root.size(); i++)
    {
        const Json::Value& outlineRoot = root[i];

        Outline outline(outlineRoot["title"].asString(), outlineRoot["page"].asInt());

        for (const Outline& subOutline : parseOutline(outlineRoot["outline"]))
        {
            outline.add(subOutline);
        }

        outlines.push_back(outline);
    }

    return outlines;
}

Document::Document()
{

}

Document::Document(const string& path)
{
    string extractorPath;

    if (filesystem::exists(localExtractorMarkerPath))
    {
        extractorPath = localExtractorPath;
    }
    else if (filesystem::exists(globalExtractorMarkerPath))
    {
        extractorPath = globalExtractorPath;
    }
    else
    {
        throw runtime_error("Could not find extraction tool, npdfr is not correctly installed.");
    }

    string tmpPath = randomTemporaryPath() + ".json";

    system(format("python \"{}\" \"{}\" \"{}\"", extractorPath, path, tmpPath).c_str());

    string serial = getFile(tmpPath);

    Json::Value root;
    Json::Reader reader;
    reader.parse(serial, root);

    for (int i = 0; i < root["pages"].size(); i++)
    {
        const Json::Value& pageRoot = root["pages"][i];

        Page page(pageRoot["width"].asDouble(), pageRoot["height"].asDouble());

        for (int j = 0; j < pageRoot["blocks"].size(); j++)
        {
            const Json::Value& blockRoot = pageRoot["blocks"][j];

            Block block(
                blockRoot["left"].asDouble(),
                blockRoot["right"].asDouble(),
                blockRoot["top"].asDouble(),
                blockRoot["bottom"].asDouble(),
                blockRoot["text"].asString()
            );

            page.add(block);
        }

        _pages.push_back(page);
    }

    _outline = parseOutline(root["outline"]);

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
