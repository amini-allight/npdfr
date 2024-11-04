#!/usr/bin/env python
# Copyright 2024 Amini Allight
# 
# This file is part of npdfr.
# 
# npdfr is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# npdfr is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with npdfr. If not, see <https://www.gnu.org/licenses/>.
import os
import sys
import fitz
import json

if len(sys.argv) != 3:
    print("Usage: %s in.pdf out.json" % sys.argv[0])
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]

document = fitz.open(input_path)

document_root = {
    "pages": [],
    "outline": []
}

def parse_outline(node):
    outlines = []

    while node != None:
        # Guard against invalid nodes in some PDFs (possibly a PyMuPDF bug?)
        try:
            outline = {
                "title": node.title,
                "page": node.page,
                "outline": []
            }

            outline["outline"] = parse_outline(node.down)

            outlines.append(outline)

            node = node.next
        except:
            break

    return outlines

document_root["outline"] = parse_outline(document.outline)

for page in document:
    blocks = page.get_text("blocks")

    page_root = {
        "width": page.rect.x1 - page.rect.x0,
        "height": page.rect.y1 - page.rect.y0,
        "blocks": []
    }

    for block in blocks:
        left, top, right, bottom, text, _, _ = block

        page_root["blocks"].append({
            "left": left,
            "top": top,
            "right": right,
            "bottom": bottom,
            "text": text
        })

    document_root["pages"].append(page_root)

open(output_path, "w").write(json.dumps(document_root, indent = 4))
