#include "Image.h"

int main()
{
    Image test("pic.png");

    Image image = test;
    image.grayscale_avg_parallel();
    image.write("gray.png");
    // test.write("test.jpg");
    return 0;
}
