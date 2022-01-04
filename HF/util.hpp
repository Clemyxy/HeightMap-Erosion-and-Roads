#include <string>

#include "image.h"

class Util {
public:
	static auto RGBtoHeight(const Image& img) {
        std::vector<float> imgGray;
	    for (int i = 0; i < img.size(); ++i) {
            imgGray.push_back((img(i).r + img(i).g + img(i).b) / 3 * 255);
        }
        return imgGray;
    }
    static auto RGBtoGray(const Image& img) {
        std::vector<float> imgGray;
	    for (int i = 0; i < img.size(); ++i) {
            imgGray.push_back((img(i).r + img(i).g + img(i).b) / 3);
        }
        return imgGray;
    }
    static auto WriteSF(const SF2& sf, const char* filename, const int& x, const int& y){
        Image img(x,y);
        float min = sf.at(0,0), max = sf.at(0,0);
        float range = sf.Range(min, max);
        for(int i = 0; i < x; ++i)
            for (int j = 0; j < y; ++j) {
                float ij = (sf.at(i, j) - min) / range;
                img(i, j) = Color(ij, ij, ij);
            }
        write_image(img, filename);
    }
};
