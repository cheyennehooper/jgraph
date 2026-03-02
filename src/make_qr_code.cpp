//could not for the LIFE of me get the qr generator to 
//work like theres only a demo anymore so i just
//used his library amd some of their code to make
//my own wrapper to out a ppm
#include <iostream>
#include <string>
#include "../QR-Code-generator/cpp/qrcodegen.hpp"

using namespace std;
using namespace qrcodegen;

int main() {
    string text;
    getline(cin, text);

    QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::MEDIUM);
    int size = qr.getSize();
    int border = 4;
    int total = size + border * 2;

    printf("P6\n%d %d\n255\n", total, total);

    for (int y = 0; y < total; y++) {
        for (int x = 0; x < total; x++) {
            int qx = x - border;
            int qy = y - border;
            bool dark = (qx >= 0 && qy >= 0 && qx < size && qy < size) && qr.getModule(qx, qy);
            unsigned char val = dark ? 0 : 255;
            putchar(val);
            putchar(val);
            putchar(val);
        }
    }
    return 0;
}