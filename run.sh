g++ -o main main.cpp -g -O3 `llvm-config --cxxflags --ldflags --system-libs --libs core`
./main