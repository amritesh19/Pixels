#include "Image.h"

int main()
{
    Image test("flowers.jpg");

    Image image = test;
    image.grayscale_avg();
    image.write("flowers_gray.jpg");
    //  test.write("test.jpg");
    return 0;
}

// cd "d:\CppProjects\Pixels\" ; g++ -c Image.cpp -o Image.o
// cd "d:\CppProjects\Pixels\" ; if ($?) { g++ main.cpp Image.o -o main } ; if ($?) { .\main }
