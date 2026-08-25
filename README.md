GrandShelf is a studying Planning app
Study Planner
A study planning application written in C++, using an SQLite database and a web frontend (HTML + Python server). The project is built with CMake.
Project Structure
study-planner/
├── src/                  # Main C++ source code
│   ├── ai/
│   ├── app/
│   ├── db/
│   ├── models/
│   ├── repository/
│   ├── services/
│   ├── ui/
│   ├── util/
│   └── main.cpp
├── tests/                # Tests
├── third_party/          # External libraries / dependencies
├── config/                # Configuration files
├── frontend/              # Web interface
│   ├── index.html
│   └── server.py
├── build/                 # CMake build output directory
├── build-make/            # Direct Makefile build output directory (if used)
├── CMakeLists.txt         # CMake build configuration
├── Makefile                # Alternative/helper build file
├── study_planner.db        # SQLite database file (created/used at runtime)
├── workspace.json
└── workspacememory.md

Prerequisites
·	C++ compiler with C++17 (or later) support (g++ or clang++)
·	CMake version 3.15 or higher
·	Make (or another generator supported by CMake, e.g. Ninja)
·	Python 3 (only needed to run the frontend server in frontend/server.py)
·	SQLite3 library (if linked system-wide; not required separately if bundled under third_party/)
Installing prerequisites on Ubuntu/Debian:
sudo apt update
sudo apt install build-essential cmake python3 libsqlite3-dev

Installing on macOS:
brew install cmake sqlite3

Check versions:
cmake --version
g++ --version

Method 1: Build with CMake (recommended, primary method)
mkdir -p build
cd build
cmake ..
cmake --build .

Or more concisely:
cmake -S . -B build
cmake --build build

The executable is usually generated inside build/ (or build/src/, depending on CMakeLists.txt). To locate it:
find build -maxdepth 3 -type f -executable

Release build (optimized)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

Method 2: Build with Makefile directly (if build-make/ is used)
If you prefer not to use CMake and the project has a standalone Makefile:
make

Output will be placed in build-make/ (or according to the rules defined in the Makefile).
To clean build files:
make clean

Running the Application
After a successful build, run the executable from the output directory, e.g.:
./build/study-planner

(Check the exact executable name in the add_executable section of CMakeLists.txt.)
The study_planner.db database should be accessible next to the executable or at the path defined in config/; otherwise, the app will likely create it on first run.
Running the Frontend (optional)
If the project includes a web interface:
cd frontend
python3 server.py

Then open the address/port shown in the terminal output or defined in server.py (typically something like http://localhost:8000).
Running Tests
If tests are configured via CMake/CTest:
cd build
ctest

Troubleshooting
·	CMake Error: ... CMAKE_CXX_COMPILER not found → No C++ compiler is installed; see the Prerequisites section.
·	SQLite3 not found error → Install libsqlite3-dev (Linux) or sqlite3 via Homebrew (macOS), or make sure the bundled version under third_party/ is built correctly.
·	Stale/broken build/ directory → Remove and rebuild:
·	rm -rf build
cmake -S . -B build
cmake --build build

·	Conflict between build/ and build-make/ → These are separate outputs from two different build methods; use only one to avoid confusion.

