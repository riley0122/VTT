#include <iostream>
#include <fstream>
#include <cstring>
#include <CImg.h>
using namespace cimg_library;

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

int main(int argc, char **argv)
{
#ifndef _WIN32
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int AVAIL_ROWS = w.ws_row - 1;
    int AVAIL_COLUMNS = w.ws_col;
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int AVAIL_ROWS = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int AVAIL_COLUMNS = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#endif
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <input_image_path> [output_file_path (optional)] [channel to use for grayscale]" << std::endl;
        return 1;
    }

    const char *fileName = argv[1];
    CImg<unsigned char> image;
    try
    {
        image.load(fileName);
    }
    catch (const CImgException &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (argc >= 4)
    {
        image = image.channel(std::stoi(argv[3]));
    }
    else
    {
        image = image.channel(1);
    }

    std::cout << "Succesfully opened file " << fileName << std::endl;

    image.resize(AVAIL_COLUMNS, AVAIL_ROWS);

    const char *charMap = " .,->|[0";

    int width = image.width();
    int height = image.height();
    int pixelCount = width * height;
    int *pixels = new int[pixelCount];
    std::cout << width << "x" << height << std::endl;
    // Getting each pixels value
    for (int r = 0; r < height; r++)
    {
        for (int c = 0; c < width; c++)
        {
            pixels[r * width + c] = (int)image(c, r, 0, 0);
        }
    }

    // Getting each pixels value but then from the charmap
    char *textPixels = new char[pixelCount];
    for (int i = 0; i < pixelCount; i++)
    {
        int currentPixel = pixels[i] + 1;
        // We divide by the ammount of characters in the charMap then floor it
        float value = currentPixel / 32;
        int finalValue = floor(value);

        textPixels[i] = charMap[finalValue];
    }

    // Showing the charmapped pixels
    for (int i = 1; i < pixelCount; i++)
    {
        std::cout << textPixels[i - 1]; // - 1 because we start at one so that we dont devide by 0 below
        if (i % width == 0)
            std::cout << std::endl;
    }

    std::cout << std::endl;

    if (argc >= 3)
    {
        try
        {
            image.save(argv[2]);
            std::cout << "Image resized and saved as " << argv[2] << " successfully!" << std::endl;
        }
        catch (const CImgException &e)
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    delete[] pixels;
    delete[] textPixels;

    return 0;
}