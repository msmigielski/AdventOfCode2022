# repo_template

To build and run:

cd <repo_path>
cmake -S . -B build
cmake --build build --parallel
cd build
ctest -V