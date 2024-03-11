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
#include "types.hpp"
#include "constants.hpp"
#include "controller.hpp"

static bool quit = false;

static void onInterrupt(int)
{
    quit = true;
}

int main(int argc, char** argv)
{
    cout << format("{} {}.{}.{}", programName, majorVersion, minorVersion, patchVersion) << endl;
    cout << "Copyright 2024 Amini Allight" << endl << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY; This is free software, and you are welcome to redistribute it under certain conditions. See the included license for further details." << endl << endl;

    if (argc == 1)
    {
        cerr << format("Usage: {} files...", argv[0]) << endl;
        return 1;
    }

    signal(SIGINT, onInterrupt);

    Controller controller;

    for (int i = 1; i < argc; i++)
    {
        controller.open(argv[i]);
    }

    while (!quit && !controller.shouldQuit())
    {
        controller.run();
    }

    return 0;
}
