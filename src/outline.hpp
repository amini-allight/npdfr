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

class Outline
{
public:
    Outline(const string& title, i32 page);

    void add(const Outline& outline);

    const string& title() const;
    i32 page() const;
    const vector<Outline>& outline() const;

    i32 pageIndexAt(i32 selectIndex, i32* y) const;
    i32 width() const;
    i32 height() const;

private:
    string _title;
    i32 _page;
    vector<Outline> _outline;
};
