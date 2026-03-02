#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <vector>

using namespace std;
using vvb = vector<vector<bool>>;
struct Color { float r, g, b; };
using vvc = vector<vector<Color>>;

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
//basically same as load_ppm 
vvc load_image(const string& path){
    ifstream f(path, ios::binary);
    if(!f){
        cerr << "Error: cannot open file " << path << "\n";
        exit(1);
    }
    string format;
    int width, height, maxval;
    f >> format >> width >> height >> maxval;
    f.ignore(1);
    int channels = (format == "P5") ? 1 : 3;
    vvc img(height, vector<Color>(width));
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            unsigned char rgb[3] = {0,0,0};
            f.read(reinterpret_cast<char*>(rgb), channels);
            img[y][x] = {rgb[0] / (float)maxval, rgb[1] / (float)maxval, rgb[2] / (float)maxval};
        }
    }
    return img;
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

Color mode(const string& mode, int row, int col, int size){
    if(mode == "rainbow"){
        float hue = (float)(row + col) / (2.0f * (size - 1));
        if(hue < 0.5f){
            float s = hue * 2.0f;
            return {1.0f - s, s, 0.0f};
        } else{
            float s = (hue - 0.5f) * 2.0f;
            return {0.0f, 1.0f - s, s};
        }
    } else if(mode == "random"){
        int pick = (row * 31 + col * 17) % 3;
        if(pick == 0) return {1.0f, 0.0f, 0.0f}; //red
        if(pick == 1) return {0.0f, 1.0f, 0.0f}; //green
        return {0.0f, 0.0f, 1.0f}; //blue
    } else if (mode == "hstripe") {
        if (row % 2 == 0) return {0.0f, 0.3f, 0.8f}; //dark blue
        else return {0.4f, 0.7f, 1.0f}; //light blue
    } else if (mode == "vstripe") {
        if (col % 2 == 0) return {0.8f, 0.0f, 0.0f}; //dark red
        else return {1.0f, 0.4f, 0.4f}; //light red
    } else if (mode == "checker") {
        if ((row + col) % 2 == 0){
            return {1.0f, 0.0f, 0.0f};  // red
        } else {
            return {0.0f, 0.0f, 1.0f}; // blue
        }
    }
    return {0.0f, 0.0f, 0.0f}; //black
}

void make_jgraph(const vvb& qr, float mod_size, const string& colormode, const vvc* img = nullptr){
    int size = qr.size();
    float total_size = size * mod_size;

    printf("newgraph\n");
    printf("xaxis min 0 max %.2f size 4 nodraw\n", total_size);
    printf("yaxis min 0 max %.2f size 4 nodraw\n", total_size);
    printf("newline poly pcfill 1 1 1\n");
    printf("pts 0 0  %.2f 0  %.2f %.2f  0 %.2f\n", total_size, total_size, total_size, total_size);

    for(int row = 0; row < size; row++){
        for(int col = 0; col < size; col++){
            if(!qr[row][col]) continue; //skip light mods
            float x0 = col * mod_size;
            float y0 = (size - 1 - row) * mod_size; //flip y axis for jgraph
            float x1 = x0 + mod_size;
            float y1 = y0 + mod_size;

            Color c;
            if(colormode == "image" && img){
                c = (*img)[row][col]; //use pic colors
            } else {
                c = mode(colormode, row, col, size);
            }
            printf("newline poly pcfill %.3f %.3f %.3f\n", c.r, c.g, c.b);
            printf("pts %.2f %.2f  %.2f %.2f  %.2f %.2f  %.2f %.2f\n", x0, y0, x1, y0, x1, y1, x0, y1);
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2){
        cout << "Usage: happy_qr input.ppm [rainbow\random]\n";
        return 1;
    }
    //cout << "happy_qr: got file " << argv[1] << "\n";
    //color mode
    string mode = (argc >= 3) ? argv[2] : "normal";

    vvb pixels = load_ppm(argv[1]);
    vvb qr = grid(pixels);

    if(mode == "image"){
        vvc img = load_image(argv[3]);
        make_jgraph(qr, 1.0f, mode, &img);
    } else {
        make_jgraph(qr, 1.0f, mode);
    }
    
    return 0;
}
