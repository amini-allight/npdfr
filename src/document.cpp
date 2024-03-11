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

#include <json/json.h>

Document::Document()
{

}

Document::Document(const string& path)
{
    string tmpPath = randomTemporaryPath() + ".json";

    system(format("python ./tools/extract.py \"{}\" \"{}\"", path, tmpPath).c_str());

    string serial = getFile(tmpPath);

    Json::Value root;
    Json::Reader reader;
    reader.parse(serial, root);

    for (int i = 0; i < root["pages"].size(); i++)
    {
        const Json::Value& pageRoot = root["pages"][i];

        Page page(pageRoot["width"].asDouble(), pageRoot["height"].asDouble());

        for (int i = 0; i < pageRoot["blocks"].size(); i++)
        {
            const Json::Value& blockRoot = pageRoot["blocks"][i];

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
}

optional<SearchResultLocation> Document::find(const string& search, const SearchResultLocation& previous) const
{
    for (i32 i = previous.pageIndex; i < _pages.size(); i++)
    {
        const Page& page = _pages.at(i);

        SearchResultLocation next = previous;
        next.pageIndex = i;
        if (i != previous.pageIndex)
        {
            next.blockIndex = 0;
        }

        optional<SearchResultLocation> pageResult = page.find(search, next);

        if (pageResult)
        {
            return pageResult;
        }
    }

    for (i32 i = 0; i <= previous.pageIndex; i++)
    {
        const Page& page = _pages.at(i);

        SearchResultLocation next = previous;
        next.pageIndex = i;
        next.blockIndex = 0;

        optional<SearchResultLocation> pageResult = page.find(search, next);

        if (pageResult)
        {
            return pageResult;
        }
    }

    return {};
}

optional<SearchResultLocation> Document::rfind(const string& search, const SearchResultLocation& previous) const
{
    for (i32 i = previous.pageIndex; i >= 0; i--)
    {
        const Page& page = _pages.at(i);

        SearchResultLocation next = previous;
        next.pageIndex = i;
        if (i != previous.pageIndex)
        {
            next.blockIndex = page.blocks().size() - 1;
        }

        optional<SearchResultLocation> pageResult = page.rfind(search, next);

        if (pageResult)
        {
            return pageResult;
        }
    }

    for (i32 i = _pages.size() - 1; i >= previous.pageIndex; i--)
    {
        const Page& page = _pages.at(i);

        SearchResultLocation next = previous;
        next.pageIndex = i;
        next.blockIndex = page.blocks().size() - 1;

        optional<SearchResultLocation> pageResult = page.rfind(search, next);

        if (pageResult)
        {
            return pageResult;
        }
    }

    return {};
}

const vector<Page>& Document::pages() const
{
    return _pages;
}
