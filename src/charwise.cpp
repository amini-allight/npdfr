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
#include "charwise.hpp"
#include <cassert>

bool isPrimaryByte(char c)
{
    return (c & 0xc0) != 0x80;
}

vector<string> splitUTF8(const string& s)
{
    vector<string> chars;

    for (char c : s)
    {
        if (isPrimaryByte(c))
        {
            chars.push_back(string() + c);
        }
        else
        {
            chars.back() += c;
        }
    }

    return chars;
}

string joinUTF8(const vector<string>& chars)
{
    string s;

    for (const string& c : chars)
    {
        s += c;
    }

    return s;
}

size_t charwiseSize(const string& s)
{
    return splitUTF8(s).size();
}

string charwiseSubstring(const string& s, size_t offset, size_t size)
{
    vector<string> chars = splitUTF8(s);

    return joinUTF8({ chars.begin() + offset, chars.begin() + offset + size });
}

size_t charwiseFind(const string& s, const string& search, size_t offset)
{
    size_t index = s.find(search, charwiseSubstring(s, 0, offset).size());

    if (index == string::npos)
    {
        return index;
    }

    return charwiseSize(s.substr(0, index));
}
