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
#include "document.hpp"
#include "document_view.hpp"

class Controller
{
public:
    Controller();
    Controller(const Controller& rhs) = delete;
    Controller(Controller&& rhs) = delete;
    ~Controller();

    Controller& operator=(const Controller& rhs) = delete;
    Controller& operator=(Controller&& rhs) = delete;

    void open(const filesystem::path& path);

    void openDisplay();
    void closeDisplay();

    void run();

    bool shouldQuit() const;

private:
    bool displayOpen;

    i32 width;
    i32 height;

    Document emptyDocument;
    Page emptyPage;
    DocumentView emptyView;

    map<string, Document> documents;
    string activeDocumentName;
    map<string, DocumentView> views;

    bool quit;
    string search;
    bool searchForwards;

    void updateSize();
    void drawScreen() const;
    void handleInput();

    void drawPage() const;
    void drawOutline(i32 x, i32* readY, i32* writeY, const vector<Outline>& outline) const;
    void handlePageInput(int ch);
    void handleOutlineInput(int ch);

    void writeToScreen(i32 y, i32 x, const string& s) const;

    void goToStartOfDocument();
    void goToEndOfDocument();
    void goToStartOfPage();
    void goToEndOfPage();
    void goToPage();
    void nextDocument();
    void previousDocument();
    void pageUp();
    void pageDown();
    void scrollUp();
    void scrollDown();
    void panLeft();
    void panRight();
    void toggleOutlineView();
    void nextSearchResult();
    void previousSearchResult();
    void startForwardSearch();
    void startBackwardSearch();

    void goToStartOfOutline();
    void goToEndOfOutline();
    void goToPageOutline();
    void halfPageUpOutline();
    void halfPageDownOutline();
    void pageUpOutline();
    void pageDownOutline();
    void scrollUpOutline();
    void scrollDownOutline();
    void panLeftOutline();
    void panRightOutline();

    SearchResultLocation seedSearchResult() const;
    i32 pages() const;
    i32 maxPageIndex() const;
    i32 maxPan() const;
    i32 maxScroll() const;
    i32 pageWidth() const;
    i32 pageHeight() const;
    i32 maxPanOutline() const;
    i32 maxSelectOutline() const;
    i32 pageWidthOutline() const;
    i32 pageHeightOutline() const;
    const Page& activePage() const;
    const Document& activeDocument() const;
    DocumentView& activeView();
    const DocumentView& activeView() const;
    const vector<SearchResultLocation>& searchResults() const;
    SearchResultLocation activeSearchResult() const;
};
