CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra

happy_qr: src/happy_qr.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f happy_qr