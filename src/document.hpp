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
#include "page.hpp"
#include "outline.hpp"

class Document
{
public:
    Document();
    Document(const string& path);

    vector<SearchResultLocation> search(const string& search) const;

    const vector<Page>& pages() const;
    const vector<Outline>& outline() const;

    i32 outlinePageIndexAt(i32 selectIndex) const;
    i32 outlineWidth() const;
    i32 outlineHeight() const;

private:
    vector<Page> _pages;
    vector<Outline> _outline;
};
