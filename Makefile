#this was actually super annoying bc i dont think ive ever set up 
#a makefile to this degree before but it was v satisfying in the end
#I hope only 2 commands is satisfying for you too ((: 
CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra
JGRAPH   = ~jplank/src/jgraph/work/jgraph
QRLIB    = QR-Code-generator/cpp

PDFS = default.pdf rainbow.pdf random.pdf vstripe.pdf hstripe.pdf plank.pdf

all: happy_qr make_qr_code $(PDFS)

happy_qr: src/happy_qr.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

QR-Code-generator:
	git clone https://github.com/nayuki/QR-Code-generator.git

make_qr_code: src/make_qr_code.cpp QR-Code-generator
	$(CXX) $(CXXFLAGS) -o $@ src/make_qr_code.cpp $(QRLIB)/qrcodegen.cpp

plank_qr.ppm: make_qr_code
	echo "Jim Plank" | ./make_qr_code > $@

default_qr.ppm: make_qr_code
	echo "I love coding" | ./make_qr_code > $@

rainbow_qr.ppm: make_qr_code
	echo "CS494 is awesome" | ./make_qr_code > $@

random_qr.ppm: make_qr_code
	echo "I cant believe the US is entering war with Iran!" | ./make_qr_code > $@

vstripe_qr.ppm: make_qr_code
	echo "Does this lab make me look cool?" | ./make_qr_code > $@

hstripe_qr.ppm: make_qr_code
	echo "I cant wait to have a beer this weekend" | ./make_qr_code > $@

plank_scaled.ppm: james-plank.jpg plank_qr.ppm
	convert james-plank.jpg -resize 21x21! $@

default.jgr: default_qr.ppm happy_qr
	./happy_qr $< > $@

rainbow.jgr: rainbow_qr.ppm happy_qr
	./happy_qr $< rainbow > $@

random.jgr: random_qr.ppm happy_qr
	./happy_qr $< random > $@

vstripe.jgr: vstripe_qr.ppm happy_qr
	./happy_qr $< vstripe > $@

hstripe.jgr: hstripe_qr.ppm happy_qr
	./happy_qr $< hstripe > $@

plank.jgr: plank_qr.ppm plank_scaled.ppm happy_qr
	./happy_qr plank_qr.ppm image plank_scaled.ppm > $@

%.ps: %.jgr
	$(JGRAPH) -P $< > $@

%.pdf: %.ps
	ps2pdf $< $@

clean:
	rm -f happy_qr make_qr_code *.ppm *.jgr *.ps *.pdf
	rm -rf QR-Code-generator