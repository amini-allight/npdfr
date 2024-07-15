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
#pragma once

#include "types.hpp"
#include "block.hpp"

class Page
{
public:
    Page(f64 width, f64 height);

    void add(const Block& block);
    void generateGrid();

    vector<SearchResultLocation> search(const string& search) const;

    i32 width() const;
    i32 height() const;
    const vector<Block>& blocks() const;

    const vector<vector<string>>& grid() const;
    tuple<i32, i32> locateSearchInGrid(const SearchResultLocation& location) const;

private:
    f64 _width;
    f64 _height;
    vector<Block> _blocks;
    // Stored to help with locating searches
    vector<tuple<i32, i32>> blockOffsets;
    vector<vector<string>> _grid;
};
