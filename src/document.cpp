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

        page.generateGrid();

        _pages.push_back(page);
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
