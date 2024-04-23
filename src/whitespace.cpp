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
#include "whitespace.hpp"
#include "charwise.hpp"

bool isWhitespace(const string& c)
{
    return
        c == "\u0009" ||
        c == "\u0020" ||
        c == "\u00a0" ||
        c == "\u00ad" ||
        c == "\u034f" ||
        c == "\u061c" ||
        c == "\u115f" ||
        c == "\u1160" ||
        c == "\u17b4" ||
        c == "\u17b5" ||
        c == "\u180e" ||
        c == "\u2000" ||
        c == "\u2001" ||
        c == "\u2002" ||
        c == "\u2003" ||
        c == "\u2004" ||
        c == "\u2005" ||
        c == "\u2006" ||
        c == "\u2007" ||
        c == "\u2008" ||
        c == "\u2009" ||
        c == "\u200a" ||
        c == "\u200b" ||
        c == "\u200c" ||
        c == "\u200d" ||
        c == "\u200e" ||
        c == "\u200f" ||
        c == "\u202f" ||
        c == "\u205f" ||
        c == "\u2060" ||
        c == "\u2061" ||
        c == "\u2062" ||
        c == "\u2063" ||
        c == "\u2064" ||
        c == "\u206a" ||
        c == "\u206b" ||
        c == "\u206c" ||
        c == "\u206d" ||
        c == "\u206e" ||
        c == "\u206f" ||
        c == "\u3000" ||
        c == "\u2800" ||
        c == "\u3164" ||
        c == "\ufeff" ||
        c == "\uffa0" ||
        c == "\u1d159" ||
        c == "\u1d173" ||
        c == "\u1d174" ||
        c == "\u1d175" ||
        c == "\u1d176" ||
        c == "\u1d177" ||
        c == "\u1d178" ||
        c == "\u1d179" ||
        c == "\u1d17a" ||
        c == "\ue0020";
}

string trimWhitespace(const string& s)
{
    vector<string> parts = splitUTF8(s);

    while (isWhitespace(parts.front()))
    {
        parts.erase(parts.begin());
    }

    while (isWhitespace(parts.back()))
    {
        parts.erase(parts.begin() + (parts.size() - 1));
    }

    return joinUTF8(parts);
}
