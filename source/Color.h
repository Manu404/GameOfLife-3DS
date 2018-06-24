#ifndef COLOR_H
#define COLOR_H
#include <3ds/services/am.h>
#include <stdlib.h>
#include <cmath>

class HSV
{
public:
    float saturation;
    float hue;
    float value;
};

class Color
{
    int satModifier = 1;

    void GenerateRandomColor();

    // RGB2HSV and HSV2RGB are based on Color Match Remix [http://color.twysted.net/]
    // which is based on or copied from ColorMatch 5K [http://colormatch.dk/]
    HSV* RGB2HSV(Color* color);
    Color* HSV2RGB(HSV* hsv);

    int HueShift(int h, int s);
    int max3(int a, int b, int c);
    int min3(int a, int b, int c);

public:

    float R,G,B;

    Color()
    {
    }

    Color(u8 r, u8 g, u8 b)
    {
        this->R = r;
        this->G = g;
        this->B = b;
    }

    void NextGradientColor(int speed);   
    void ComputeComplementFromColor(Color* color);
};

#endif