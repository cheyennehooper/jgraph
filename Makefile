CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra

happy_qr: src/happy_qr.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test.pdf: happy_qr test.ppm
	./happy_qr test.ppm | jgraph -P | ps2pdf - test.pdf

clean:
	rm -f happy_qr