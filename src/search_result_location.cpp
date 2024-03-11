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
#include "search_result_location.hpp"

SearchResultLocation::SearchResultLocation(
    const string& documentName,
    i32 pageIndex,
    i32 blockIndex,
    i32 characterIndex
)
    : documentName(documentName)
    , pageIndex(pageIndex)
    , blockIndex(blockIndex)
    , characterIndex(characterIndex)
{

}

bool SearchResultLocation::operator==(const SearchResultLocation& rhs) const
{
    return documentName == rhs.documentName &&
        pageIndex == rhs.pageIndex &&
        blockIndex == rhs.blockIndex &&
        characterIndex == rhs.characterIndex;
}

bool SearchResultLocation::operator!=(const SearchResultLocation& rhs) const
{
    return !(*this == rhs);
}

bool SearchResultLocation::operator>(const SearchResultLocation& rhs) const
{
    if (pageIndex > rhs.pageIndex)
    {
        return true;
    }
    else if (pageIndex < rhs.pageIndex)
    {
        return false;
    }
    else
    {
        if (blockIndex > rhs.blockIndex)
        {
            return true;
        }
        else if (blockIndex < rhs.blockIndex)
        {
            return false;
        }
        else
        {
            if (characterIndex > rhs.characterIndex)
            {
                return true;
            }
            else if (characterIndex < rhs.characterIndex)
            {
                return false;
            }
            else
            {
                return false;
            }
        }
    }
}

bool SearchResultLocation::operator<(const SearchResultLocation& rhs) const
{
    if (pageIndex < rhs.pageIndex)
    {
        return true;
    }
    else if (pageIndex > rhs.pageIndex)
    {
        return false;
    }
    else
    {
        if (blockIndex < rhs.blockIndex)
        {
            return true;
        }
        else if (blockIndex > rhs.blockIndex)
        {
            return false;
        }
        else
        {
            if (characterIndex < rhs.characterIndex)
            {
                return true;
            }
            else if (characterIndex > rhs.characterIndex)
            {
                return false;
            }
            else
            {
                return false;
            }
        }
    }
}

bool SearchResultLocation::operator>=(const SearchResultLocation& rhs) const
{
    return *this > rhs || *this == rhs;
}

bool SearchResultLocation::operator<=(const SearchResultLocation& rhs) const
{
    return *this < rhs || *this == rhs;
}

strong_ordering SearchResultLocation::operator<=>(const SearchResultLocation& rhs) const
{
    if (*this == rhs)
    {
        return strong_ordering::equal;
    }
    else if (*this < rhs)
    {
        return strong_ordering::less;
    }
    else
    {
        return strong_ordering::greater;
    }
}
