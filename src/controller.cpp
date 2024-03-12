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
#include "controller.hpp"
#include "constants.hpp"
#include "charwise.hpp"

#include <unistd.h>
#include <sys/ioctl.h>
#include <ncurses.h>

Controller::Controller()
    : emptyPage(0, 0)
    , quit(false)
    , searchForwards(true)
    , searchResultLocation("", 0)
{
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    cbreak();
    start_color();
    use_default_colors();
    keypad(stdscr, true);
    curs_set(0);

    init_pair(1, COLOR_BLUE, -1);
    init_pair(2, COLOR_RED, -1);
}

Controller::~Controller()
{
    move(0, 0);
    curs_set(1);
    keypad(stdscr, false);
    echo();
    endwin();
}

void Controller::open(const string& path)
{
    documents.insert_or_assign(path, Document(path));
    views.insert_or_assign(path, DocumentView());

    activeDocumentName = path;
}

void Controller::run()
{
    updateSize();
    drawScreen();
    handleInput();
}

bool Controller::shouldQuit() const
{
    return quit;
}

void Controller::updateSize()
{
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    width = w.ws_col;
    height = w.ws_row;
}

void Controller::drawScreen() const
{
    erase();

    const Page& page = activePage();

    i32 activeSearchX = -1;
    i32 activeSearchY = -1;

    if (
        searchResultLocation.documentName == activeDocumentName &&
        searchResultLocation.pageIndex == activeView().pageIndex
    )
    {
        tie(activeSearchX, activeSearchY) = page.locateSearchInGrid(searchResultLocation);
    }

    vector<vector<string>> grid = page.grid();

    for (i32 screenY = 0; screenY < height - 1; screenY++)
    {
        i32 lineIndex = screenY + activeView().scrollIndex;

        // Scrolled past end
        if (lineIndex >= grid.size())
        {
            break;
        }

        string line = joinUTF8(grid.at(lineIndex));

        // Panned past end
        if (charwiseSize(line) <= activeView().panIndex)
        {
            continue;
        }

        vector<pair<i32, i32>> highlights;
        vector<bool> activeHighlight;

        // Find highlighted regions
        if (search != "")
        {
            size_t offset = 0;

            while (true)
            {
                size_t index = charwiseFind(line, search, offset);

                if (index == string::npos)
                {
                    break;
                }

                offset = index + charwiseSize(search);

                highlights.push_back({ index, offset });
                activeHighlight.push_back(
                    index == activeSearchX &&
                    lineIndex == activeSearchY
                );
            }
        }

        // Draw whole line
        if (highlights.empty())
        {
            string truncatedLine = charwiseSubstring(line, activeView().panIndex, charwiseSize(line) - activeView().panIndex);

            mvaddstr(screenY, 0, truncatedLine.c_str());
        }
        // Draw line in highlighted parts
        else
        {
            vector<string> parts;
            vector<i32> highlighted;

            i32 readHead = activeView().panIndex;

            for (size_t i = 0; i < highlights.size(); i++)
            {
                const auto& [ start, end ] = highlights.at(i);
                bool active = activeHighlight.at(i);

                if (end < activeView().panIndex)
                {
                    continue;
                }
                else if (start < activeView().panIndex)
                {
                    parts.push_back(charwiseSubstring(line, activeView().panIndex, end - activeView().panIndex));
                    highlighted.push_back(active ? 2 : 1);

                    readHead = end;
                }
                else
                {
                    parts.push_back(charwiseSubstring(line, readHead, start - readHead));
                    highlighted.push_back(0);

                    parts.push_back(charwiseSubstring(line, start, end - start));
                    highlighted.push_back(active ? 2 : 1);

                    readHead = end;
                }
            }

            parts.push_back(charwiseSubstring(line, readHead, charwiseSize(line) - readHead));
            highlighted.push_back(0);

            i32 x = 0;

            for (size_t i = 0; i < parts.size(); i++)
            {
                const string& part = parts.at(i);
                i32 highlight = highlighted.at(i);

                switch (highlight)
                {
                case 0 :
                    mvaddstr(screenY, x, part.c_str());
                    break;
                case 1 :
                    attron(COLOR_PAIR(1));
                    attron(A_REVERSE);
                    mvaddstr(screenY, x, part.c_str());
                    attroff(A_REVERSE);
                    attroff(COLOR_PAIR(1));
                    break;
                case 2 :
                    attron(COLOR_PAIR(2));
                    attron(A_REVERSE);
                    mvaddstr(screenY, x, part.c_str());
                    attroff(A_REVERSE);
                    attroff(COLOR_PAIR(2));
                    break;
                }

                x += charwiseSize(part);
            }
        }
    }

    string prompt = format(
        "{} ({}/{}) {}",
        activeDocumentName,
        activeView().pageIndex + 1,
        activeDocument().pages().size(),
        search
    );

    mvaddstr(height - 1, 0, prompt.c_str());
}

void Controller::handleInput()
{
    int ch = getch();

    switch (ch)
    {
    // g, home
    case 'g' :
    case 262 :
        goToStartOfDocument();
        break;
    // G, end
    case 'G' :
    case 360 :
        goToEndOfDocument();
        break;
    // u, U
    case 'u' :
    case 'U' :
        goToStartOfPage();
        break;
    // d, D
    case 'd' :
    case 'D' :
        goToEndOfPage();
        break;
    // tab
    case 9 :
        nextDocument();
        break;
    // shift+tab
    case 353 :
        previousDocument();
        break;
    // b, B, page-up
    case 'b' :
    case 'B' :
    case 339 :
        pageUp();
        break;
    // space, f, F, page-down
    case ' ' :
    case 'f' :
    case 'F' :
    case 338 :
        pageDown();
        break;
    // h, H, left
    case 'h' :
    case 'H' :
    case 260 :
        panLeft();
        break;
    // e, E, j, J, enter, down
    case 'e' :
    case 'E' :
    case 'j' :
    case 'J' :
    case 10 :
    case 258 :
        scrollDown();
        break;
    // y, Y, k, K, up
    case 'y' :
    case 'Y' :
    case 'k' :
    case 'K' :
    case 259 :
        scrollUp();
        break;
    // l, L, right
    case 'l' :
    case 'L' :
    case 261 :
        panRight();
        break;
    // q, Q
    case 'q' :
    case 'Q' :
        quit = true;
        break;
    // n
    case 'n' :
        searchForwards ? nextSearchResult() : previousSearchResult();
        break;
    // N
    case 'N' :
        searchForwards ? previousSearchResult() : nextSearchResult();
        break;
    // /
    case '/' :
        startForwardSearch();
        break;
    // ?
    case '?' :
        startBackwardSearch();
        break;
    }
}

void Controller::goToStartOfDocument()
{
    i32 previousPageIndex = activeView().pageIndex;
    activeView().pageIndex = 0;
    activeView().scrollIndex = 0;

    if (activeView().pageIndex != previousPageIndex)
    {
        activeView().panIndex = 0;
    }
}

void Controller::goToEndOfDocument()
{
    i32 previousPageIndex = activeView().pageIndex;
    activeView().pageIndex = max<i32>(activeDocument().pages().size() - 1, 0);
    activeView().scrollIndex = maxScroll();

    if (activeView().pageIndex != previousPageIndex)
    {
        activeView().panIndex = 0;
    }
}

void Controller::goToStartOfPage()
{
    activeView().scrollIndex = 0;
}

void Controller::goToEndOfPage()
{
    activeView().scrollIndex = maxScroll();
}

void Controller::nextDocument()
{
    bool next = false;

    for (const auto& [ name, document ] : documents)
    {
        if (next)
        {
            activeDocumentName = name;
            return;
        }

        if (name == activeDocumentName)
        {
            next = true;
        } 
    }

    activeDocumentName = documents.begin()->first;
}

void Controller::previousDocument()
{
    string previous;

    for (const auto& [ name, document ] : documents)
    {
        if (name == activeDocumentName)
        {
            if (previous == "")
            {
                activeDocumentName = documents.rbegin()->first;
            }
            else
            {
                activeDocumentName = previous;
            }

            return;
        }

        previous = name;
    }
}

void Controller::pageUp()
{
    i32 previousPageIndex = activeView().pageIndex;
    activeView().pageIndex = clamp<i32>(activeView().pageIndex - 1, 0, activeDocument().pages().size() - 1);

    if (activeView().pageIndex != previousPageIndex)
    {
        activeView().scrollIndex = 0;
        activeView().panIndex = 0;
    }
}

void Controller::pageDown()
{
    i32 previousPageIndex = activeView().pageIndex;
    activeView().pageIndex = clamp<i32>(activeView().pageIndex + 1, 0, activeDocument().pages().size() - 1);

    if (activeView().pageIndex != previousPageIndex)
    {
        activeView().scrollIndex = 0;
        activeView().panIndex = 0;
    }
}

void Controller::scrollUp()
{
    activeView().scrollIndex = clamp(activeView().scrollIndex - 1, 0, maxScroll());
}

void Controller::scrollDown()
{
    activeView().scrollIndex = clamp(activeView().scrollIndex + 1, 0, maxScroll());
}

void Controller::panLeft()
{
    activeView().panIndex = clamp(activeView().panIndex - 1, 0, maxPan());
}

void Controller::panRight()
{
    activeView().panIndex = clamp(activeView().panIndex + 1, 0, maxPan());
}

void Controller::nextSearchResult()
{
    vector<SearchResultLocation> results = activeDocument().search(search);

    if (results.empty())
    {
        return;
    }

    for (SearchResultLocation& result : results)
    {
        result.documentName = activeDocumentName;
    }

    SearchResultLocation currentLocation =
        activeDocumentName == searchResultLocation.documentName
            ? searchResultLocation
            : seedSearchResultLocation();

    searchResultLocation = results.front();
    for (const SearchResultLocation& result : results)
    {
        if (result > currentLocation)
        {
            searchResultLocation = result;
            break;
        }
    }

    i32 previousPageIndex = activeView().pageIndex;
    activeView().pageIndex = searchResultLocation.pageIndex;

    if (activeView().pageIndex != previousPageIndex)
    {
        activeView().scrollIndex = 0;
        activeView().panIndex = 0;
    }
}

void Controller::previousSearchResult()
{
    vector<SearchResultLocation> results = activeDocument().search(search);

    if (results.empty())
    {
        return;
    }

    for (SearchResultLocation& result : results)
    {
        result.documentName = activeDocumentName;
    }

    SearchResultLocation currentLocation =
        activeDocumentName == searchResultLocation.documentName
            ? searchResultLocation
            : seedSearchResultLocation();

    optional<SearchResultLocation> previous;
    for (const SearchResultLocation& result : results)
    {
        if (result >= currentLocation)
        {
            searchResultLocation = previous ? *previous : results.back();
            break;
        }

        previous = result;
    }

    i32 previousPageIndex = activeView().pageIndex;
    activeView().pageIndex = searchResultLocation.pageIndex;

    if (activeView().pageIndex != previousPageIndex)
    {
        activeView().scrollIndex = 0;
        activeView().panIndex = 0;
    }
}

void Controller::startForwardSearch()
{
    string buffer(maxSearchLength, '\0');

    move(height - 1, 0);
    clrtoeol();
    mvaddstr(height - 1, 0, "/");
    echo();
    mvgetnstr(height - 1, 1, buffer.data(), maxSearchLength);
    noecho();

    search = buffer.data();
    searchForwards = true;
    searchResultLocation = seedSearchResultLocation();
    nextSearchResult();
}

void Controller::startBackwardSearch()
{
    string buffer(maxSearchLength, '\0');

    move(height - 1, 0);
    clrtoeol();
    mvaddstr(height - 1, 0, "?");
    echo();
    mvgetnstr(height - 1, 1, buffer.data(), maxSearchLength);
    noecho();

    search = buffer.data();
    searchForwards = false;
    searchResultLocation = seedSearchResultLocation();
    previousSearchResult();
}

SearchResultLocation Controller::seedSearchResultLocation() const
{
    return SearchResultLocation(activeDocumentName, activeView().pageIndex);
}

i32 Controller::maxPan() const
{
    return max(pageWidth() - width, 0);
}

i32 Controller::maxScroll() const
{
    return max(pageHeight() - height, 0);
}

i32 Controller::pageWidth() const
{
    return activePage().width();
}

i32 Controller::pageHeight() const
{
    return activePage().height();
}

const Page& Controller::activePage() const
{
    return activeView().pageIndex < activeDocument().pages().size()
        ? activeDocument().pages().at(activeView().pageIndex)
        : emptyPage;
}

const Document& Controller::activeDocument() const
{
    return documents.contains(activeDocumentName)
        ? documents.at(activeDocumentName)
        : emptyDocument;
}

DocumentView& Controller::activeView()
{
    return views.contains(activeDocumentName)
        ? views.at(activeDocumentName)
        : emptyView;
}

const DocumentView& Controller::activeView() const
{
    return views.contains(activeDocumentName)
        ? views.at(activeDocumentName)
        : emptyView;
}
