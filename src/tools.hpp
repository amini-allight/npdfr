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

string randomTemporaryPath()
{
    static const char alphabet[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    random_device device;
    mt19937 engine(device());
    uniform_int_distribution<size_t> distribution(0, (sizeof(alphabet) / sizeof(char)) - 1);
    string s;

    for (size_t i = 0; i < 16; i++)
    {
        s += alphabet[distribution(engine)];
    }

    return format("/tmp/{}", s);
}

string getFile(const string& path)
{
    ifstream file(path);

    return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}
