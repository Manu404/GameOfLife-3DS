#include "Color.h"
#include <stdio.h>

int increment = 0;

void Color::NextGradientColor(int speed)
{
    if ((R == 255 && G == 255 && B == 255) || (R == 0 && G == 0 && B == 0))
    {
        GenerateRandomColor();
    }

    HSV* hsv = RGB2HSV(this);
    hsv->hue = HueShift(hsv->hue, 1);
    hsv->saturation += satModifier / 100;
    hsv->value += satModifier / 100;
    Color* newcolor = HSV2RGB(hsv);

    this->R = newcolor->R;
    this->G = newcolor->G;
    this->B = newcolor->B;

    if (hsv->saturation >= 1)
        satModifier = -1;
    else if (hsv->saturation <= 0)
        satModifier = 1;
}

void Color::ComputeComplementFromColor(Color* color)
{
    HSV* hsv = RGB2HSV(color);
    hsv->hue = HueShift(hsv->hue, 180);
    hsv->saturation += satModifier;
    Color* newcolor = HSV2RGB(hsv);

    this->R = newcolor->R;
    this->G = newcolor->G;
    this->B = newcolor->B;
    
    if (satModifier == 255)
        satModifier = satModifier ? -1 : 1;
}


void Color::GenerateRandomColor()
{
    this->R = rand() % 255;
    this->G = rand() % 255;
    this->B = rand() % 255;
}

// RGB2HSV and HSV2RGB are based on Color Match Remix [http://color.twysted.net/]
// which is based on or copied from ColorMatch 5K [http://colormatch.dk/]
HSV* Color::RGB2HSV(Color* color) {
    HSV* hsv = new HSV();
    float max = max3(color->R, color->G, color->B);
    float dif = max - min3(color->R, color->G, color->B);
    hsv->saturation = (max == 0.0) ? 0 : (100 * dif / max);
    if (hsv->saturation == 0) hsv->hue = 0;
    else if (color->R == max) hsv->hue = 60.0*(color->G - color->B) / dif;
    else if (color->G == max) hsv->hue = 120.0 + 60.0*(color->B - color->R) / dif;
    else if (color->B == max) hsv->hue = 240.0 + 60.0*(color->R - color->G) / dif;
    if (hsv->hue<0.0) hsv->hue += 360.0;
    hsv->value = round(max * 100 / 255);
    hsv->hue = round(hsv->hue);
    hsv->saturation = round(hsv->saturation);
    return hsv;
}

Color* Color::HSV2RGB(HSV* hsv) {
    Color* color = new Color();
    if (hsv->saturation == 0) {
        color->R = color->G = color->B = round(hsv->value*2.55);
    }
    else {
        hsv->hue /= 60;
        hsv->saturation /= 100;
        hsv->value /= 100;
        int i = floor(hsv->hue);
        float f = hsv->hue - i;
        float p = hsv->value*(1 - hsv->saturation);
        float q = hsv->value*(1 - hsv->saturation*f);
        float t = hsv->value*(1 - hsv->saturation*(1 - f));
        switch (i) {
        case 0: color->R = hsv->value; color->G = t; color->B = p; break;
        case 1: color->R = q; color->G = hsv->value; color->B = p; break;
        case 2: color->R = p; color->G = hsv->value; color->B = t; break;
        case 3: color->R = p; color->G = q; color->B = hsv->value; break;
        case 4: color->R = t; color->G = p; color->B = hsv->value; break;
        default: color->R = hsv->value; color->G = p; color->B = q;
        }
        color->R = round(color->R * 255);
        color->G = round(color->G * 255);
        color->B = round(color->B * 255);
    }
    return color;
}

int Color::HueShift(int h, int s) {
    return (h + s) % 360;
}

int Color::max3(int a, int b, int c)
{
    if (a >= b && a >= c) return a;
    if (b >= a && b >= c) return b;
    return c;
}

int Color::min3(int a, int b, int c)
{
    if (a <= b && a <= c) return a;
    if (b <= a && b <= c) return b;
    return c;
}