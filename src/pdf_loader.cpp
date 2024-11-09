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
#include "pdf_loader.hpp"

#include <mupdf/fitz.h>

static vector<Outline> walkOutline(fz_context* ctx, fz_document* fzDocument, const fz_outline* fzOutline)
{
    vector<Outline> outlines;

    while (fzOutline)
    {
        Outline outline(
            fzOutline->title,
            fz_page_number_from_location(ctx, fzDocument, fzOutline->page)
        );

        for (const Outline& subOutline : walkOutline(ctx, fzDocument, fzOutline->down))
        {
            outline.add(subOutline);
        }

        outlines.push_back(outline);

        fzOutline = fzOutline->next;
    }

    return outlines;
}

static string extractCharText(const fz_stext_char* fzChar)
{
    string s;
    int c = fzChar->c;

    if (c <= 0x7f)
    {
        s += static_cast<char>(c);
    }
    else if (c <= 0x7ff)
    {
        s += 0xc0 | (c >> 6);
        s += 0x80 | (c & 0x3f);
    }
    else if (c <= 0xffff)
    {
        s += 0xe0 | (c >> 12);
        s += 0x80 | ((c >> 6) & 0x3f);
        s += 0x80 | (c & 0x3f);
    }
    else if (c <= 0x10ffff)
    {
        s += 0xf0 | (c >> 18);
        s += 0x80 | ((c >> 12) & 0x3f);
        s += 0x80 | ((c >> 6) & 0x3f);
        s += 0x80 | (c & 0x3f);
    }

    return s;
}

static string extractLineText(const fz_stext_line* fzLine)
{
    string text;

    const fz_stext_char* fzChar = fzLine->first_char;

    while (fzChar)
    {
        text += extractCharText(fzChar);

        fzChar = fzChar->next;
    }

    return text;
}

static string extractBlockText(const fz_stext_block* fzBlock)
{
    string text;

    const fz_stext_line* fzLine = fzBlock->u.t.first_line;

    while (fzLine)
    {
        text += extractLineText(fzLine) + "\n";

        fzLine = fzLine->next;
    }

    return text;
}

Document loadPDF(const filesystem::path& path)
{
    Document result;

    fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);

    fz_register_document_handlers(ctx);

    fz_document* fzDocument = fz_open_document(ctx, path.string().c_str());

    fz_outline* fzOutline = fz_load_outline(ctx, fzDocument);

    result.setOutline(walkOutline(ctx, fzDocument, fzOutline));

    fz_drop_outline(ctx, fzOutline);

    int pageCount = fz_count_pages(ctx, fzDocument);

    for (int i = 0; i < pageCount; i++)
    {
        fz_page* fzPage = fz_load_page(ctx, fzDocument, i);

        fz_rect bounds = fz_bound_page(ctx, fzPage);

        Page page(bounds.x1 - bounds.x0, bounds.y1 - bounds.y0);

        fz_stext_options options{};
        options.scale = 1;

        fz_stext_page* fzStextPage = fz_new_stext_page_from_page(ctx, fzPage, &options);

        const fz_stext_block* fzBlock = fzStextPage->first_block;

        while (fzBlock)
        {
            if (fzBlock->type == FZ_STEXT_BLOCK_TEXT)
            {
                Block block(
                    fzBlock->bbox.x0,
                    fzBlock->bbox.x1,
                    fzBlock->bbox.y0,
                    fzBlock->bbox.y1,
                    extractBlockText(fzBlock)
                );

                page.add(block);
            }

            fzBlock = fzBlock->next;
        }

        fz_drop_stext_page(ctx, fzStextPage);

        result.add(page);

        fz_drop_page(ctx, fzPage);
    }

    fz_drop_document(ctx, fzDocument);
    fz_drop_context(ctx);

    return result;
}
