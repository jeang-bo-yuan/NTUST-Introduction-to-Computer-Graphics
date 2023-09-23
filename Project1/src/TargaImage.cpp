///////////////////////////////////////////////////////////////////////////////
//
//      TargaImage.cpp                          Author:     Stephen Chenney
//                                              Modified:   Eric McDaniel
//                                              Date:       Fall 2004
//
//      Implementation of TargaImage methods.  You must implement the image
//  modification functions.
//
// Reference: https://research.cs.wisc.edu/graphics/Courses/559-f2002/projects/project1/project-1-faq.html
///////////////////////////////////////////////////////////////////////////////

#include "Globals.h"
#include "TargaImage.h"
#include "libtarga.h"
#include "Palette.h"
#include "Filter.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

// constants
const int           RED             = 0;                // red channel
const int           GREEN           = 1;                // green channel
const int           BLUE            = 2;                // blue channel
const unsigned char BACKGROUND[3]   = { 0, 0, 0 };      // background color


// Computes n choose s, efficiently
double Binomial(int n, int s)
{
    double        res;

    res = 1;
    for (int i = 1 ; i <= s ; i++)
        res = (n - i + 1) * res / i ;

    return res;
}// Binomial


///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage() : width(0), height(0), data(NULL)
{}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h) : width(w), height(h)
{
   data = new unsigned char[width * height * 4];
   ClearToBlack();
}// TargaImage



///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables to values given.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h, unsigned char *d)
{
    int i;

    width = w;
    height = h;
    data = new unsigned char[width * height * 4];

    for (i = 0; i < width * height * 4; i++)
        data[i] = d[i];
}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Copy Constructor.  Initialize member to that of input
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(const TargaImage& image) 
{
   width = image.width;
   height = image.height;
   data = NULL; 
   if (image.data != NULL) {
      data = new unsigned char[width * height * 4];
      memcpy(data, image.data, sizeof(unsigned char) * width * height * 4);
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Destructor.  Free image memory.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::~TargaImage()
{
    if (data)
        delete[] data;
}// ~TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Converts an image to RGB form, and returns the rgb pixel data - 24 
//  bits per pixel. The returned space should be deleted when no longer 
//  required.
//
///////////////////////////////////////////////////////////////////////////////
unsigned char* TargaImage::To_RGB(void)
{
    unsigned char   *rgb = new unsigned char[width * height * 3];
    int		    i, j;

    if (! data)
        return NULL;

    // Divide out the alpha
    for (i = 0 ; i < height ; i++)
    {
        int in_offset = i * width * 4;
        int out_offset = i * width * 3;

        for (j = 0 ; j < width ; j++)
        {
            RGBA_To_RGB(data + (in_offset + j*4), rgb + (out_offset + j*3));
        }
    }

    return rgb;
}// TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Save the image to a targa file. Returns 1 on success, 0 on failure.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Save_Image(const char *filename)
{
    TargaImage	*out_image = Reverse_Rows();

    if (! out_image)
        return false;

    if (!tga_write_raw(filename, width, height, out_image->data, TGA_TRUECOLOR_32))
    {
        cout << "TGA Save Error: %s\n", tga_error_string(tga_get_last_error());
        return false;
    }

    delete out_image;

    return true;
}// Save_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Load a targa image from a file.  Return a new TargaImage object which 
//  must be deleted by caller.  Return NULL on failure.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Load_Image(char *filename)
{
    unsigned char   *temp_data;
    TargaImage	    *temp_image;
    TargaImage	    *result;
    int		        width, height;

    if (!filename)
    {
        cout << "No filename given." << endl;
        return NULL;
    }// if

    temp_data = (unsigned char*)tga_load(filename, &width, &height, TGA_TRUECOLOR_32);
    if (!temp_data)
    {
        cout << "TGA Error: %s\n", tga_error_string(tga_get_last_error());
        width = height = 0;
        return NULL;
    }
    temp_image = new TargaImage(width, height, temp_data);
    free(temp_data);

    result = temp_image->Reverse_Rows();

    delete temp_image;

    return result;
}// Load_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::To_Grayscale()
{
    int num_of_pixels = height * width;
    // for each pixel
    for (int i = 0; i < num_of_pixels; ++i) {
        // data[id], data[id + 1], data[id + 2], data[id + 3] are RGBA respectively
        int id = i * TGA_TRUECOLOR_32;

        int Y = 0.3 * data[id] + 0.59 * data[id + 1] + 0.11 * data[id + 2];
        Color::memset(data + id, Color::RGB_t(Y, Y, Y));
    }
    return true;
}// To_Grayscale


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Uniform()
{
    int num_of_pixels = height * width;
    // for each pixel
    for (int i = 0; i < num_of_pixels; ++i) {
        int id = i * TGA_TRUECOLOR_32;

        // 取首3個bit
        data[id] &= 0xE0; // R
        data[id + 1] &= 0xE0; // G
        // 取首兩個bit
        data[id + 2] &= 0xC0; // B
    }
    return true; 
}// Quant_Uniform


///////////////////////////////////////////////////////////////////////////////
//
//      Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Populosity()
{
    // 將15 bit的 RGB 存成一個index
    // Binary: (MSB -> LSB)
    // - RRRRR GGGGG BBBBB
    typedef uint_fast16_t RGB_index_t;
    // 給 RGB 轉成 RGB_index_t
    auto to_index = [](unsigned char R, unsigned char G, unsigned char B) -> RGB_index_t {
        RGB_index_t result = 0;
        result |= (RGB_index_t(B & 0b1111'1000) >> 3);
        result |= (RGB_index_t(G & 0b1111'1000) << 2);
        result |= (RGB_index_t(R & 0b1111'1000) << 7);
        return result;
    };
#define getR(index) (unsigned char)((index & 0b0'11111'00000'00000) >> 7)
#define getG(index) (unsigned char)((index & 0b0'00000'11111'00000) >> 2)
#define getB(index) (unsigned char)((index & 0b0'00000'00000'11111) << 3)
    // 用來儲存某顏色出現次數。
    typedef struct { RGB_index_t index; unsigned count; } Record_t;

    int num_of_pixels = height * width;
    constexpr int num_of_color = 32768; // 32 * 32 * 32
    std::vector<Record_t> usage_histogram; // 32768種顏色，各顏色的使用量

    // 初始化usage_histogram
    usage_histogram.reserve(num_of_color);
    for (int i = 0; i < num_of_color; ++i)
        usage_histogram.emplace_back(Record_t{ (RGB_index_t)i, 0 });

    // Uniform Quantization，將RGB分別轉成5 bit，順便計算出現次數
    for (int i = 0; i < num_of_pixels; ++i) {
        int id = i * TGA_TRUECOLOR_32;
        data[id] &= 0b1111'1000;
        data[id + 1] &= 0b1111'1000;
        data[id + 2] &= 0b1111'1000;
        usage_histogram[to_index(data[id], data[id + 1], data[id + 2])].count++;
    }

    // 排序，次數多 -> 次數少，前256項為被選上的顏色
    std::sort(usage_histogram.begin(), usage_histogram.end(), [](const Record_t& r1, const Record_t& r2) -> bool {
        return r1.count > r2.count;
    });

    // 建立 Palette，內含 usage_histogram 中最常出現的 256 色
    Color::Palette_t palette(256);
    for (int i = 0; i < 256; ++i) {
        if (usage_histogram[i].count > 0) {
            RGB_index_t index = usage_histogram[i].index;
            palette[i] = Color::RGB_t(getR(index), getG(index), getB(index));
        }
        else {
            // 其實原圖沒有256種顏色
            palette.resize(i);
            break;
        }
    }

    // 對每個像素替換
    for (int i = 0; i < num_of_pixels; ++i) {
        int id = i * TGA_TRUECOLOR_32;
        Color::RGB_t closest = palette.find_closest_to(Color::RGB_t(data + id));
        // 換成palette中最近的
        Color::memset(data + id, closest);
    }

    return true;
#undef getR
#undef getG
#undef getB
}// Quant_Populosity


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Threshold()
{
    To_Grayscale();
    float threshold = 0xFF / 2.0;

    int num_of_pixels = height * width;
    for (int i = 0; i < num_of_pixels; ++i) { // for each pixel
        int id = i * TGA_TRUECOLOR_32;

        Color::memset(data + id, (data[id] > threshold ? Color::White : Color::Black));
    }
    return true;
}// Dither_Threshold


///////////////////////////////////////////////////////////////////////////////
//
//      Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Random()
{
    int num_of_pixels = height * width;
    int threshold = 255 / 2;

    To_Grayscale();

    srand(time(NULL));
    for (int i = 0; i < num_of_pixels; ++i) {
        int id = i * TGA_TRUECOLOR_32;
        int gray = data[id];

        gray += 255 * (((float)rand() / RAND_MAX * 0.4f) - 0.2f); // 255 * （[-0.2, 0.2]內隨機一數）
        Color::memset(data + id, (gray > threshold ? Color::White : Color::Black));
    }

    return true;
}// Dither_Random


bool TargaImage::General_Dither_FS(const Color::Palette_t& palette) {
    if (palette.empty()) return false;

    // 讓arr和error的0、1、2項分別相加並存回arr，並處理溢位的問題
    // proportion 為 error 要乘上的係數
    auto add_error = [](unsigned char* arr, int* error, float proportion) {
        for (int i = 0; i < 3; ++i) {
            int result = arr[i] + error[i] * proportion;
            arr[i] = (result < 0 ? 0 : (result > 0xFF ? 0xFF : result));
        }
    };

    // Zig-Zag
    for (int r = 0; r < height; ++r) {
        int startC = 0, deltaC = 1;
        // 若在奇數列，則從右到左
        if (r % 2) {
            startC = width - 1;
            deltaC = -1;
        }

        for (int c = startC; 0 <= c && c < width; c += deltaC) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;
            // error in R, G, B channel respectively
            int error[3] = { data[id], data[id + 1], data[id + 2] };

            Color::RGB_t closest = palette.find_closest_to(Color::RGB_t(data + id));
            Color::memset(data + id, closest);

            // calculate error: OLD - NEW
            error[0] -= closest.R;
            error[1] -= closest.G;
            error[2] -= closest.B;

            // 定義「前方」為「c加上deltaC後變化的方向」
            constexpr int Forward = 0, Back_Down = 1, Down = 2, Forward_Down = 3;
            // 前方、後下、下、前下 要乘上的係數
            constexpr float proportion[] = { 7 / 16.f, 3 / 16.f, 5 / 16.f, 1 / 16.f };
            // id加上這些delta後所得的new_id，分別對應到 前、後下、下、前下
            int delta[] = { TGA_TRUECOLOR_32, (width - 1) * TGA_TRUECOLOR_32, width * TGA_TRUECOLOR_32, (width + 1) * TGA_TRUECOLOR_32 };

            // propagate error
            if (0 <= c + deltaC && c + deltaC < width) { // 前方有像素
                add_error(data + id + delta[Forward], error, proportion[Forward]);
            }
            if (r != height - 1) { // 下面有像素
                add_error(data + id + delta[Down], error, proportion[Down]);
                // 後下有像素
                if (0 <= c - deltaC && c - deltaC < width)
                    add_error(data + id + delta[Back_Down], error, proportion[Back_Down]);
                // 前下有像素
                if (0 <= c + deltaC && c + deltaC < width)
                    add_error(data + id + delta[Forward_Down], error, proportion[Forward_Down]);
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//      Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_FS()
{
    To_Grayscale();
    return General_Dither_FS(Color::Palette_t{ Color::Black, Color::White });
}// Dither_FS


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Bright()
{
    To_Grayscale();
    int num_of_pixels = height * width;
    unsigned sum_of_gray = 0;
    unsigned char* gray_arr = new unsigned char[num_of_pixels];

    // 求和 + 填入gray_arr
    for (int i = 0; i < num_of_pixels; ++i) {
        gray_arr[i] = data[i * TGA_TRUECOLOR_32];
        sum_of_gray += gray_arr[i];
    }

    // 計算亮度：平均灰階 / 255
    // 若亮度為 0.17，代表 dithering 後， 17% 的像素要是白的，其餘為黑的
    float brightness = (float)sum_of_gray / num_of_pixels / 0xFF;
    std::cout << "brightness = " << brightness << std::endl;

    // 求閥值
    std::sort(gray_arr, gray_arr + num_of_pixels);
    unsigned char threshold = gray_arr[unsigned(num_of_pixels * (1 - brightness))];
    std::cout << "threshold = " << (int)threshold << std::endl;

    // dithering
    for (int i = 0; i < num_of_pixels; ++i) {
        int id = i * TGA_TRUECOLOR_32;

        Color::memset(data + id, (data[id] > threshold ? Color::White : Color::Black));
    }

    delete[] gray_arr;
    return true;
}// Dither_Bright


///////////////////////////////////////////////////////////////////////////////
//
//      Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Cluster()
{
    float mask[4][4] = {
        0.7059 , 0.3529 , 0.5882 , 0.2353 ,
        0.0588 , 0.9412 , 0.8235 , 0.4118 ,
        0.4706 , 0.7647 , 0.8824 , 0.1176 ,
        0.1765 , 0.5294 , 0.2941 , 0.6471 };
    for (int id = 0; id < 16; ++id) ((float*)mask)[id] *= 0xFF;

    To_Grayscale();

    // for each pixel
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;
            if (data[id] >= mask[r % 4][c % 4]) {
                Color::memset(data + id, Color::White);
            }
            else {
                Color::memset(data + id, Color::Black);
            }
        }
    }
    return true;
}// Dither_Cluster


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Color()
{
    constexpr uint8_t R_channel[] = { 0, 36, 73, 109, 146, 182, 219, 255 };
    constexpr uint8_t G_channel[] = { 0, 36, 73, 109, 146, 182, 219, 255 };
    constexpr uint8_t B_channel[] = { 0, 85, 170, 255 };
    Color::Palette_t palette;
    palette.reserve(8ULL * 8 * 4);

    for (uint8_t r : R_channel) {
        for (uint8_t g : G_channel) {
            for (uint8_t b : B_channel) {
                palette.emplace_back(r, g, b);
            }
        }
    }

    return General_Dither_FS(palette);
}// Dither_Color


///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Over(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout <<  "Comp_Over: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Over


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_In(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_In: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_In


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Out(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Out: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Out


///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Atop(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Atop: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Atop


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Xor(TargaImage* pImage)
{
    if (width != pImage->width || height != pImage->height)
    {
        cout << "Comp_Xor: Images not the same size\n";
        return false;
    }

    ClearToBlack();
    return false;
}// Comp_Xor


///////////////////////////////////////////////////////////////////////////////
//
//      Calculate the difference bewteen this imag and the given one.  Image 
//  dimensions must be equal.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Difference(TargaImage* pImage)
{
    if (!pImage)
        return false;

    if (width != pImage->width || height != pImage->height)
    {
        cout << "Difference: Images not the same size\n";
        return false;
    }// if

    for (int i = 0 ; i < width * height * 4 ; i += 4)
    {
        unsigned char        rgb1[3];
        unsigned char        rgb2[3];

        RGBA_To_RGB(data + i, rgb1);
        RGBA_To_RGB(pImage->data + i, rgb2);

        data[i] = abs(rgb1[0] - rgb2[0]);
        data[i+1] = abs(rgb1[1] - rgb2[1]);
        data[i+2] = abs(rgb1[2] - rgb2[2]);
        data[i+3] = 255;
    }

    return true;
}// Difference


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Box()
{
    const Filter::ImageInfo_t old_image{ height, width, data };
    const Filter::Filter_t box_filter(5, 5, {
        {0.04, 0.04, 0.04, 0.04, 0.04},
        {0.04, 0.04, 0.04, 0.04, 0.04},
        {0.04, 0.04, 0.04, 0.04, 0.04},
        {0.04, 0.04, 0.04, 0.04, 0.04},
        {0.04, 0.04, 0.04, 0.04, 0.04}
     });

    unsigned char* new_data = new unsigned char[(size_t)height * width * TGA_TRUECOLOR_32];

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;

            // set R G B based on calculation
            Color::memset(new_data + id, box_filter.calculate(r - 2, c - 2, r + 2, c + 2, old_image));
            // set Alpha
            new_data[id + 3] = data[id + 3];
        }
    }

    // move new_data back to data
    memmove(data, new_data, (size_t)height * width * TGA_TRUECOLOR_32);

    delete[] new_data;
    return true;
}// Filter_Box


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Bartlett()
{
    const Filter::ImageInfo_t old_image = { height, width, data };
    const Filter::Filter_t bartlett_filter(5, 5, {
        {1 / 81.f, 2 / 81.f, 3 / 81.f, 2 / 81.f, 1 / 81.f},
        {2 / 81.f, 4 / 81.f, 6 / 81.f, 4 / 81.f, 2 / 81.f},
        {3 / 81.f, 6 / 81.f, 9 / 81.f, 6 / 81.f, 3 / 81.f},
        {2 / 81.f, 4 / 81.f, 6 / 81.f, 4 / 81.f, 2 / 81.f},
        {1 / 81.f, 2 / 81.f, 3 / 81.f, 2 / 81.f, 1 / 81.f},
    });

    unsigned char* new_data = new unsigned char[(size_t)height * width * TGA_TRUECOLOR_32];

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;

            // set R G B
            Color::memset(new_data + id, bartlett_filter.calculate(r - 2, c - 2, r + 2, c + 2, old_image));
            // Alpha channel
            new_data[id + 3] = data[id + 3];
        }
    }

    delete[] data;
    data = new_data;

    return true;
}// Filter_Bartlett


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Gaussian()
{
    const Filter::ImageInfo_t old_image = { height, width, data };
    const Filter::Filter_t gussian_filter(5, 5, {
        { 1 / 256.f, 4 / 256.f,  6 / 256.f,  4 / 256.f,  1 / 256.f },
        { 4 / 256.f, 16 / 256.f, 24 / 256.f, 16 / 256.f, 4 / 256.f },
        { 6 / 256.f, 24 / 256.f, 36 / 256.f, 24 / 256.f, 6 / 256.f },
        { 4 / 256.f, 16 / 256.f, 24 / 256.f, 16 / 256.f, 4 / 256.f },
        { 1 / 256.f, 4 / 256.f,  6 / 256.f,  4 / 256.f,  1 / 256.f }
    });

    unsigned char* new_data = new unsigned char[height * width * TGA_TRUECOLOR_32];

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;

            Color::memset(new_data + id, gussian_filter.calculate(r - 2, c - 2, r + 2, c + 2, old_image));
            new_data[id + 3] = data[id + 3];
        }
    }

    delete[] data;
    data = new_data;

    return true;
}// Filter_Gaussian

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

bool TargaImage::Filter_Gaussian_N( unsigned int N )
{
    // Calculate C(N, M)
    auto combination = [](unsigned N, unsigned M) -> float {
        unsigned K = N - M;
        // result = N! / M! / K! = (M+1) * (M+2) * ... * N / K!
        float result = 1;
        while ((++M) <= N) result *= M;
        while (K) result /= (K--);
        return result;
    };

    const Filter::ImageInfo_t old_image = { height, width, data };
    Filter::Filter_t gaussian_N(N, N); // N * N

    float sum = 0;
    // calculate first row of gaussian_N
    // it will be: C(N-1,0), C(N-1,1), C(N-1,2), ..., C(N-1,N-1)
    for (int c = 0; c < N; ++c) {
        float elem = combination(N - 1, c);
        gaussian_N.at(0, c) = elem;
        sum += elem;
    }
    // copy first row to first column, skip (0,0)
    for (int r = 1; r < N; ++r) {
        float elem = gaussian_N.at(0, r);
        gaussian_N.at(r, 0) = elem;
        sum += elem;
    }
    // calculate remaining elements
    for (int r = 1; r < N; ++r) {
        for (int c = 1; c < N; ++c) {
            float elem = gaussian_N.at(r, 0) * gaussian_N.at(0, c);
            gaussian_N.at(r, c) = elem;
            sum += elem;
        }
    }
    // Let each element divided by `sum`
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            gaussian_N.at(r, c) /= sum;
        }
    }

    unsigned char* new_data = new unsigned char[height * width * TGA_TRUECOLOR_32];

    for (int r = 0, half_N = N / 2; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;

            if (N & 1) // N 是奇數
                Color::memset(new_data + id, gaussian_N.calculate(r - half_N, c - half_N, r + half_N, c + half_N, old_image));
            else // N 是偶數
                Color::memset(new_data + id, gaussian_N.calculate(r - half_N, c - half_N, r + half_N - 1, c + half_N - 1, old_image));
            new_data[id + 3] = 0xFF; // Alpha channel
        }
    }

    delete[] data;
    data = new_data;

    return true;
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Edge()
{
    const Filter::ImageInfo_t old_image = { height, width, data };
    const Filter::Filter_t edge_detecter(5, 5, {
        { -1 / 256.f, -4 / 256.f,  -6 / 256.f,  -4 / 256.f,  -1 / 256.f },
        { -4 / 256.f, -16 / 256.f, -24 / 256.f, -16 / 256.f, -4 / 256.f },
        { -6 / 256.f, -24 / 256.f, 220 / 256.f, -24 / 256.f, -6 / 256.f },
        { -4 / 256.f, -16 / 256.f, -24 / 256.f, -16 / 256.f, -4 / 256.f },
        { -1 / 256.f, -4 / 256.f,  -6 / 256.f,  -4 / 256.f,  -1 / 256.f },
    });

    unsigned char* new_data = new unsigned char[(size_t)height * width * TGA_TRUECOLOR_32];

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;

            Color::memset(new_data + id,
                edge_detecter.calculate(r - 2, c - 2, r + 2, c + 2, old_image));
            new_data[id + 3] = 255;
        }
    }

    delete[] data;
    data = new_data;

    return true;
}// Filter_Edge


///////////////////////////////////////////////////////////////////////////////
//
//      Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Enhance()
{
    const Filter::ImageInfo_t old_image = { height, width, data };
    const Filter::Filter_t edge_enhancer(5, 5, {
        { -1 / 256.f, -4 / 256.f,  -6 / 256.f,  -4 / 256.f,  -1 / 256.f },
        { -4 / 256.f, -16 / 256.f, -24 / 256.f, -16 / 256.f, -4 / 256.f },
        { -6 / 256.f, -24 / 256.f, 476 / 256.f, -24 / 256.f, -6 / 256.f },
        { -4 / 256.f, -16 / 256.f, -24 / 256.f, -16 / 256.f, -4 / 256.f },
        { -1 / 256.f, -4 / 256.f,  -6 / 256.f,  -4 / 256.f,  -1 / 256.f },
        });

    unsigned char* new_data = new unsigned char[(size_t)height * width * TGA_TRUECOLOR_32];

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;

            Color::memset(new_data + id,
                edge_enhancer.calculate(r - 2, c - 2, r + 2, c + 2, old_image));
            new_data[id + 3] = 255;
        }
    }

    delete[] data;
    data = new_data;

    return true;
}// Filter_Enhance


///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::NPR_Paint()
{
    unsigned int stroke_radius[] = { 15, 10, 4, 2 };
    // 畫布
    TargaImage canvas(*this);
    // 讓畫布和原圖的差距儘可能大
    for (int id = canvas.height * canvas.width * TGA_TRUECOLOR_32 - 1; id >= 0; --id) {
        // 忽略alpha channel
        if ((id & 0b11) == 3) continue;
        canvas.data[id] = (canvas.data[id] > 127 ? 0 : 255);
    }

    for (unsigned radius : stroke_radius) {
        std::cout << "Painting layer with radius = " << radius << std::endl;
        TargaImage reference_image(*this); // copy (*this)
        reference_image.Filter_Gaussian_N(radius * 2);
        canvas.Paint_Layer(reference_image, radius);
    }

    std::swap(canvas.data, this->data);

    return true;
}


void TargaImage::Paint_Layer(const TargaImage& reference, unsigned radius) {
    constexpr float factorG = 1; // 值越大，筆刷間隔越大
    constexpr int parameterT = 30; // 值越大，越模糊 0 ~ 441

    std::list<Stroke> S;
    int* D = new int[height * width];

    // calculate difference between "this" and "reference"
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int id = (r * width + c) * TGA_TRUECOLOR_32;
            D[r * width + c] = sqrt(Color::euclidean_distance_square(Color::RGB_t(data + id), Color::RGB_t(reference.data + id)));
        }
    }

    srand(time(NULL));

    int grid = factorG * radius;
    for (int r = 0; r < height; r += grid) {
        for (int c = 0; c < width; c += grid) {
            int areaError = 0;
            int maxError = 0, rMax = 0, cMax = 0;

            // add up areaError and find the largest error point
            for (int i = r - grid / 2; i <= r + grid / 2; ++i) {
                for (int j = c - grid / 2; j <= c + grid / 2; ++j) {
                    // (i列, j欄)在圖外
                    if (i < 0 || j < 0 || i >= height || j >= width) {
                        continue;
                    }
                    else {
                        int error = D[i * width + j];
                        areaError += error;
                        if (error > maxError) {
                            maxError = error;
                            rMax = i;
                            cMax = j;
                        }
                    }
                }
            }

            areaError /= (grid * grid);
            if (areaError > parameterT) {
                int id = (rMax * width + cMax) * TGA_TRUECOLOR_32;
                unsigned R = reference.data[id];
                unsigned G = reference.data[id + 1];
                unsigned B = reference.data[id + 2];
                unsigned A = reference.data[id + 3];
                if (rand() & 1)
                    S.emplace_back(radius, cMax, rMax, R, G, B, A);
                else
                    S.emplace_front(radius, cMax, rMax, R, G, B, A);
            }
        }
    }

    for (const auto& stroke : S) {
        this->Paint_Stroke(stroke);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Half_Size()
{
    const Filter::ImageInfo_t old_image = { height, width, data };
    const Filter::Filter_t bartlett_filter(3, 3, {
        { 1 / 16.f, 1 / 8.f, 1 / 16.f },
        { 1 / 8.f,  1 / 4.f, 1 / 8.f },
        { 1 / 16.f, 1 / 8.f, 1 / 16.f}
    });

    int new_height = height / 2;
    int new_width = width / 2;
    unsigned char* new_data = new unsigned char[new_height * new_width * TGA_TRUECOLOR_32];

    // use backward mapping
    for (int r = 0; r < new_height; ++r) {
        for (int c = 0; c < new_width; ++c) {
            int new_id = (r * new_width + c) * TGA_TRUECOLOR_32; // new_data中的r列c欄

            // set RGB
            Color::memset(new_data + new_id,
                bartlett_filter.calculate(r + r - 1, c + c - 1, r + r + 1, c + c + 1, old_image));
            // Alpha channel
            new_data[new_id + 3] = 255;
        }
    }

    height = new_height;
    width = new_width;
    delete[] data; // delete old image
    data = new_data;

    return true;
}// Half_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Double_Size()
{
    const Filter::ImageInfo_t old_image = { height, width, data };
    const Filter::Filter_t even_even(3, 3, {
        { 1 / 16.f, 1 / 8.f, 1 / 16.f },
        { 1 / 8.f,  1 / 4.f, 1 / 8.f },
        { 1 / 16.f, 1 / 8.f, 1 / 16.f}
    });
    const Filter::Filter_t odd_odd(4, 4, {
        { 1 / 64.f, 3 / 64.f, 3 / 64.f, 1 / 64.f },
        { 3 / 64.f, 9 / 64.f, 9 / 64.f, 3 / 64.f },
        { 3 / 64.f, 9 / 64.f, 9 / 64.f, 3 / 64.f },
        { 1 / 64.f, 3 / 64.f, 3 / 64.f, 1 / 64.f }
    });
    const Filter::Filter_t odd_even(4, 3, {
        { 1 / 32.f, 2 / 32.f, 1 / 32.f },
        { 3 / 32.f, 6 / 32.f, 3 / 32.f },
        { 3 / 32.f, 6 / 32.f, 3 / 32.f },
        { 1 / 32.f, 2 / 32.f, 1 / 32.f }
    });
    const Filter::Filter_t even_odd(3, 4, {
        { 1 / 32.f, 3 / 32.f, 3 / 32.f, 1 / 32.f },
        { 2 / 32.f, 6 / 32.f, 6 / 32.f, 2 / 32.f },
        { 1 / 32.f, 3 / 32.f, 3 / 32.f, 1 / 32.f },
    });

    int new_height = height * 2;
    int new_width = width * 2;
    unsigned char* new_data = new unsigned char[new_height * new_width * TGA_TRUECOLOR_32];

    // use backward mapping
    for (int r = 0; r < new_height; ++r) {
        for (int c = 0; c < new_width; ++c) {
            int new_id = (r * new_width + c) * TGA_TRUECOLOR_32;
            int half_r = r / 2, half_c = c / 2;

            if (!(r & 1) && !(c & 1)) { // even even
                Color::memset(new_data + new_id,
                    even_even.calculate(half_r - 1, half_c - 1, half_r + 1, half_c + 1, old_image));
            }
            else if ((r & 1) && (c & 1)) { // odd odd
                Color::memset(new_data + new_id,
                    odd_odd.calculate(half_r - 1, half_c - 1, half_r + 2, half_c + 2, old_image));
            }
            else if ((r & 1) && !(c & 1)) { // odd even
                Color::memset(new_data + new_id,
                    odd_even.calculate(half_r - 1, half_c - 1, half_r + 2, half_c + 1, old_image));
            }
            else { // even odd
                Color::memset(new_data + new_id,
                    even_odd.calculate(half_r - 1, half_c - 1, half_r + 1, half_c + 2, old_image));
            }

            new_data[new_id + 3] = 255;
        }
    }

    height = new_height;
    width = new_width;
    delete[] data;
    data = new_data;

    return true;
}// Double_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Scale the image dimensions by the given factor.  The given factor is 
//  assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Resize(float scale)
{
    const Filter::ImageInfo_t old_image = { height, width, data };

    int new_height = height * scale;
    int new_width = width * scale;
    unsigned char* new_data = new unsigned char[new_height * new_width * TGA_TRUECOLOR_32];

    for (int r = 0; r < new_height; ++r) {
        for (int c = 0; c < new_width; ++c) {
            int new_id = (r * new_width + c) * TGA_TRUECOLOR_32;
            float old_r = r / scale, old_c = c / scale;
            const Filter::Filter_t bartlett_filter = Filter::Filter_t::bartlett4_4(old_r, old_c);

            old_r = floorf(old_r);
            old_c = floorf(old_c);
            Color::memset(new_data + new_id,
                bartlett_filter.calculate(old_r - 1, old_c - 1, old_r + 2, old_c + 2, old_image));
            new_data[new_id + 3] = 255;
        }
    }

    height = new_height;
    width = new_width;
    delete[] data;
    data = new_data;

    return true;
}// Resize


//////////////////////////////////////////////////////////////////////////////
//
//      Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Rotate(float angleDegrees)
{
    float theta = DegreesToRadians(angleDegrees);
    auto forward_mapping = [theta](int& x, int& y) {
        int _x = cosf(theta) * x - sinf(theta) * y;
        int _y = sinf(theta) * x + cosf(theta) * y;
        x = _x; y = _y;
    };
    auto backward_mapping = [theta](float& x, float& y) {
        float _x = cosf(theta) * x + sinf(theta) * y;
        float _y = -sinf(theta) * x + cosf(theta) * y;
        x = _x; y = _y;
    };

    const Filter::ImageInfo_t old_image = { height, width, data };

    // old image:
    // x1y1 --- x2y2
    //  |        |
    // x4y4 --- x3y3
    int x1 = 0, y1 = 0, x2 = width, y2 = 0;
    int x3 = width, y3 = height, x4 = 0, y4 = height;
    // forward map to new image
    forward_mapping(x1, y1);
    forward_mapping(x2, y2);
    forward_mapping(x3, y3);
    forward_mapping(x4, y4);
    // boundary of new image
    int up_bound = std::max({ y1, y2, y3, y4 });
    int low_bound = std::min({ y1, y2, y3, y4 });
    int left_bound = std::min({ x1, x2, x3,x4 });
    int right_bound = std::max({ x1, x2, x3, x4 });
    // new image
    int new_height = up_bound - low_bound;
    int new_width = right_bound - left_bound;
    unsigned char* new_data = new unsigned char[new_height * new_width * TGA_TRUECOLOR_32];

    // backward_mapping
    for (int r = 0; r < new_height; ++r) {
        for (int c = 0; c < new_width; ++c) {
            // 我現在在處理新圖的第new_id個像素
            int new_id = (r * new_width + c) * TGA_TRUECOLOR_32;
            // 求新圖的xy並backward map到舊圖
            float x = left_bound + c, y = low_bound + r;
            backward_mapping(x, y);

            // 若舊圖沒有xy這點
            if (x < 0 || y < 0 || x >= width || y >= height) {
                memset(new_data + new_id, 0, 4);
            }
            else {
                const Filter::Filter_t bartlett_filter = Filter::Filter_t::bartlett4_4(y, x);
                x = floorf(x);
                y = floorf(y);
                Color::memset(new_data + new_id,
                    bartlett_filter.calculate(y - 1, x - 1, y + 2, x + 2, old_image));
                new_data[new_id + 3] = 255;
            }
        }
    }

    // replace old image with new one
    height = new_height;
    width = new_width;
    delete[] data;
    data = new_data;

    return true;
}// Rotate


//////////////////////////////////////////////////////////////////////////////
//
//      Given a single RGBA pixel return, via the second argument, the RGB
//      equivalent composited with a black background.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::RGBA_To_RGB(unsigned char *rgba, unsigned char *rgb)
{
    const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

    unsigned char  alpha = rgba[3];

    if (alpha == 0)
    {
        rgb[0] = BACKGROUND[0];
        rgb[1] = BACKGROUND[1];
        rgb[2] = BACKGROUND[2];
    }
    else
    {
        float	alpha_scale = (float)255 / (float)alpha;
        int	val;
        int	i;

        for (i = 0 ; i < 3 ; i++)
        {
            val = (int)floor(rgba[i] * alpha_scale);
            if (val < 0)
            rgb[i] = 0;
            else if (val > 255)
            rgb[i] = 255;
            else
            rgb[i] = val;
        }
    }
}// RGA_To_RGB


///////////////////////////////////////////////////////////////////////////////
//
//      Copy this into a new image, reversing the rows as it goes. A pointer
//  to the new image is returned.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Reverse_Rows(void)
{
    unsigned char   *dest = new unsigned char[width * height * 4];
    TargaImage	    *result;
    int 	        i, j;

    if (! data)
        return NULL;

    for (i = 0 ; i < height ; i++)
    {
        int in_offset = (height - i - 1) * width * 4;
        int out_offset = i * width * 4;

        for (j = 0 ; j < width ; j++)
        {
            dest[out_offset + j * 4] = data[in_offset + j * 4];
            dest[out_offset + j * 4 + 1] = data[in_offset + j * 4 + 1];
            dest[out_offset + j * 4 + 2] = data[in_offset + j * 4 + 2];
            dest[out_offset + j * 4 + 3] = data[in_offset + j * 4 + 3];
        }
    }

    result = new TargaImage(width, height, dest);
    delete[] dest;
    return result;
}// Reverse_Rows


///////////////////////////////////////////////////////////////////////////////
//
//      Clear the image to all black.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::ClearToBlack()
{
    memset(data, 0, width * height * 4);
}// ClearToBlack


///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::Paint_Stroke(const Stroke& s) {
   int radius_squared = (int)s.radius * (int)s.radius;
   for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
      for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
         int x_loc = (int)s.x + x_off;
         int y_loc = (int)s.y + y_off;
         // are we inside the circle, and inside the image?
         if ((x_loc >= 0 && x_loc < width && y_loc >= 0 && y_loc < height)) {
            int dist_squared = x_off * x_off + y_off * y_off;
            if (dist_squared <= radius_squared) {
               data[(y_loc * width + x_loc) * 4 + 0] = s.r;
               data[(y_loc * width + x_loc) * 4 + 1] = s.g;
               data[(y_loc * width + x_loc) * 4 + 2] = s.b;
               data[(y_loc * width + x_loc) * 4 + 3] = s.a;
            } else if (dist_squared == radius_squared + 1) {
               data[(y_loc * width + x_loc) * 4 + 0] = 
                  (data[(y_loc * width + x_loc) * 4 + 0] + s.r) / 2;
               data[(y_loc * width + x_loc) * 4 + 1] = 
                  (data[(y_loc * width + x_loc) * 4 + 1] + s.g) / 2;
               data[(y_loc * width + x_loc) * 4 + 2] = 
                  (data[(y_loc * width + x_loc) * 4 + 2] + s.b) / 2;
               data[(y_loc * width + x_loc) * 4 + 3] = 
                  (data[(y_loc * width + x_loc) * 4 + 3] + s.a) / 2;
            }
         }
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
               unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
   radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{
}

