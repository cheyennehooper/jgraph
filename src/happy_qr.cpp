#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;
using vvb = vector<vector<bool>>;

vvb load_ppm(const string& path){
    //load ppm file and convert to 2d bool array of pixels
    ifstream f(path, ios::binary);
    if(!f){
        cerr << "Error: cannot open file " << path << "\n";
        exit(1);
    }
    //read header
    string format;
    int width, height, maxval;
    //read format, width, height, maxval from header
    f >> format >> width >> height >> maxval;
    f.ignore(1); //skip newline after header
    if(format != "P5" && format != "P6"){
        cerr << "only P5/6 PPM supported not: " << format << "\n";
        exit(1);
    }
    //pixel data to bool array true == dark false == light
    //channels is 1 for grayscale (P5) and 3 for rgb (P6)
    int channels = (format == "P5") ? 1 : 3; //grayscale or rgb
    vvb pixels(height, vector<bool>(width));
    for(int y = 0; y < height; y++){
        //read pixel data for row y
        for(int x = 0; x < width; x++){
            //read pixel data for pixel (x,y)
            unsigned char rgb[3] = {0,0,0};
            f.read(reinterpret_cast<char*>(rgb), channels);
            int avg = 0;
            //average rgb values for brightness
            for(int c = 0; c < channels; c++){
                avg += rgb[c];
            }
            //dark if below half maxval
            pixels[y][x] = (avg / channels) < (maxval / 2);
        }
    }
    return pixels;
}

void find_start(const vvb& pixels, int& og_x, int& og_y){
    //find first dark pixel in pixels and return its coordinates in x and y
    for(int y = 0; y < (int)pixels.size(); y++){
        for(int x = 0; x < (int)pixels[0].size(); x++){
            if(pixels[y][x]){
                og_x = x;
                og_y = y;
                return;
            }
        }
    }
    cerr << "error: no dark pixels found\n";
    exit(1);
}

int module_size(const vvb& pixels, int og_x, int og_y){
    //count how many dark pixels in row og_y starting from og_x
    int count = 0;
    for(int x = og_x; x < (int)pixels[0].size(); x++){
        if(pixels[og_y][x]){
            count++;
        } else {
            break;
        }
    }
    //each qr module is 7 pixels wide
    return count / 7; 
}

int grid_size(int width, int og_x, int module_size){
    int size = width - (og_x * 2);
    return size / module_size;
}

vvb grid(const vvb& pixels){
    int og_x, og_y;
    find_start(pixels, og_x, og_y);
    
    int mod_size = module_size(pixels, og_x, og_y);
    int size = grid_size(pixels[0].size(), og_x, mod_size);
    int half = mod_size / 2;

    vvb qr(size, vector<bool>(size, false));
    for(int row = 0; row < size; row++){
        for(int col = 0; col < size; col++){
            int x = og_x + col * mod_size + half;
            int y = og_y + row * mod_size + half;
            qr[row][col] = pixels[y][x];
        }
    }
    return qr;
}

int main(int argc, char* argv[]) {
    if (argc < 2){
        cout << "Usage: happy_qr input.ppm\n";
        return 1;
    }
    cout << "happy_qr: got file " << argv[1] << "\n";

    vvb pixels = load_ppm(argv[1]);
    vvb qr = grid(pixels);

    //print qr grid to stdout
    for(const auto& row : qr){
        for(bool cell : row){
            cout << (cell ? "██" : "  ");
        }
        cout << "\n";
    }
    return 0;
}



// //pixel data for ppm and rgb values
// struct PPM{
//     int width, height, maxval;
//     vector<unsigned char> pixels;
// };

// struct qrgrid{
//     int size, pixels, x, y;
//     vector<vector<bool>> dark;
// };

// PPM load_ppm(const string& path){
//     PPM ppm;

//     ifstream f(path, ios::binary);
//     if (!f){
//         cerr << "Error: cannot open file " << path << "\n";
//         exit(1);
//     }

//     string format;
//     f >> format >> ppm.width >> ppm.height >> ppm.maxval;
//     f.ignore(1); //skip newline after header

//     if (format != "P5" && format != "P6"){
//         cerr << "only P5/6 PPM supported not: " << format << "\n";
//         exit(1);
//     }

//     int channels = (format == "P5") ? 1 : 3; //grayscale or rgb
//     vector<unsigned char> raw_data(ppm.width * ppm.height * channels);
//     f.read(reinterpret_cast<char*>(raw_data.data()), raw_data.size());

//     //grayscale or rgb data both converted to rgb
//     ppm.pixels.resize(ppm.width * ppm.height * 3); //always store as rgb
//     for (int i = 0; i < ppm.width * ppm.height; i++) {
//         unsigned char v = raw_data[i * channels]; //grayscale value or red channel
//         ppm.pixels[i * 3 + 0] = v; //red
//         ppm.pixels[i * 3 + 1] = v; //green
//         ppm.pixels[i * 3 + 2] = v; //blue
//     }
//     return ppm;
// }
// int brightness(const PPM& ppm, int x, int y){
//     int base = (y * ppm.width + x) * 3;
//     return (ppm.pixels[base] + ppm.pixels[base+1] + ppm.pixels[base+2]) / 3;
// }

// void first_dark(const PPM& ppm, int& x, int& y){
//     //find first dark pixel in ppm
//     for (y = 0; y < ppm.height; y++){
//         //check each pixel in row y
//         for (x = 0; x < ppm.width; x++){
//             //if pixel is dark return its coordinates
//             if (brightness(ppm, x, y) < ppm.maxval / 2){
//                 return;
//             }
//         }
//     }
//     cerr << "error: no dark pixels found\n";
//     exit(1);
// }

// int count_dark(const PPM& ppm, int x, int y){
//     //count how many dark pixels in row y starting from x
//     int count = 0;
//     for (int i = x; i < ppm.width; i++){
//         if (brightness(ppm, i, y) < ppm.maxval / 2){
//             count++;
//         } else {
//             break;
//         }
//     }
//     return count/7; //each qr module is 7 pixels wide
// }

// int grid_size(const PPM& ppm, int x, int y){
//     //find last dark pixel in each direction 
//     int last_x = x, last_y = y;
//     for(int i = y; i < ppm.height; i++){
//         int dark_count = count_dark(ppm, x, i);
//         if (dark_count == 0){
//             break;
//         }
//         last_y = i;
//     }
// }




// int main(int argc, char* argv[]) {
//     if (argc != 2){
//         cout << "Usage: happy_qr input.ppm\n";
//         return 1;
//     }
//     cout << "happy_qr: got file " << argv[1] << "\n";

//     PPM ppm = load_ppm(argv[1]);
//     cout << "Loaded PPM: " << ppm.width << " x " << ppm.height << " and maxval: " << ppm.maxval << "\n";

//     return 0;
// }

