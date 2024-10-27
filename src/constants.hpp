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

static const char* const programName = "npdfr";
static const u32 majorVersion = 0;
static const u32 minorVersion = 2;
static const u32 patchVersion = 1;

static const size_t maxSearchLength = 1024;
static const size_t maxPageNumberLength = 16;

static const u32 blockVerticalSpacer = 1;
static const u32 blockHorizontalSpacer = 4;

#define STR(x) #x
#define XSTR(x) STR(x)

static const char* const localExtractorPath = "./tools/extract.py";
static const char* const localExtractorMarkerPath = "./tools/npdfr-tools-marker";
static const char* const globalExtractorPath = XSTR(NPDFR_INSTALL_PREFIX)"/share/npdfr/tools/extract.py";
static const char* const globalExtractorMarkerPath = XSTR(NPDFR_INSTALL_PREFIX)"/share/npdfr/tools/npdfr-tools-marker";

#undef STR
#undef XSTR
