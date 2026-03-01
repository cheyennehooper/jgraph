#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2){
        cout << "Usage: happy_qr input.ppm\n";
        return 1;
    }
    cout << "happy_qr: got file " << argv[1] << "\n";
    return 0;
}