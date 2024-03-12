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

bool isPrimaryByte(char c);
vector<string> splitUTF8(const string& s);
string joinUTF8(const vector<string>& chars);
size_t charwiseSize(const string& s);
string charwiseSubstring(const string& s, size_t offset, size_t size);
size_t charwiseFind(const string& s, const string& search, size_t offset);
