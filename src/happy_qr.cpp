#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

//pixel data for ppm and rgb values
struct PPM{
    int width, height, maxval;
    vector<unsigned char> pixels;
};
PPM load_ppm(const string& path){
    PPM ppm;

    ifstream f(path, ios::binary);
    if (!f){
        cerr << "Error: cannot open file " << path << "\n";
        exit(1);
    }

    string format;
    f >> format >> ppm.width >> ppm.height >> ppm.maxval;
    f.ignore(1); //skip newline after header

    if (format != "P5" && format != "P6"){
        cerr << "only P5/6 PPM supported not: " << format << "\n";
        exit(1);
    }

    int channels = (format == "P5") ? 1 : 3; //grayscale or rgb
    vector<unsigned char> raw_data(ppm.width * ppm.height * channels);
    f.read(reinterpret_cast<char*>(raw_data.data()), raw_data.size());

    //grayscale or rgb data both converted to rgb
    ppm.pixels.resize(ppm.width * ppm.height * 3); //always store as rgb
    for (int i = 0; i < ppm.width * ppm.height; i++) {
        unsigned char v = raw_data[i * channels]; //grayscale value or red channel
        ppm.pixels[i * 3 + 0] = v; //red
        ppm.pixels[i * 3 + 1] = v; //green
        ppm.pixels[i * 3 + 2] = v; //blue
    }
    return ppm;
}
int brightness(const PPM& ppm, int x, int y){
    int base = (y * ppm.width + x) * 3;
    return (ppm.pixels[base] + ppm.pixels[base+1] + ppm.pixels[base+2]) / 3;
}

int main(int argc, char* argv[]) {
    if (argc != 2){
        cout << "Usage: happy_qr input.ppm\n";
        return 1;
    }
    cout << "happy_qr: got file " << argv[1] << "\n";

    PPM ppm = load_ppm(argv[1]);
    cout << "Loaded PPM: " << ppm.width << " x " << ppm.height << " and maxval: " << ppm.maxval << "\n";

    return 0;
}