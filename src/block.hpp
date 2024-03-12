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
#include "search_result_location.hpp"

class Block
{
public:
    Block(f64 left, f64 right, f64 top, f64 bottom, const string& text);

    vector<SearchResultLocation> search(const string& search) const;

    i32 width() const;
    i32 height() const;
    f64 left() const;
    f64 right() const;
    f64 top() const;
    f64 bottom() const;
    const string& text() const;

    vector<vector<string>> grid() const;
    tuple<i32, i32> locateSearchInGrid(const SearchResultLocation& location) const;

private:
    f64 _left;
    f64 _right;
    f64 _top;
    f64 _bottom;
    string _text;
};
