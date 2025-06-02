#ifndef PALETTES_H_INCLUDED
#define PALETTES_H_INCLUDED

struct rgb
{
    float red;
    float green;
    float blue;
};

class palettes{
    std::vector<std::vector<unsigned int>> pletts = {
        {
        0xE6194B, 0x3CB44B, 0xffe119, 0x0082c8, 0xf58231,
        0x911eb4, 0x46f0f0, 0xf032e6, 0xd2f53c, 0xfabebe,
        0x008080, 0xe6beff, 0xaa6e28, 0xfffac8, 0x800000,
        0xaaffc3, 0x808000, 0xffd8b1, 0x000080, 0x808080,
        0xFFFFFF, 0x000000
        },
        {
        0xff0000, 0xf8ff00, 0xa41a1a, 0x10ff00, 0x00ffd9,
        0xaaffc3, 0x808000, 0xffd8b1, 0x000080, 0x808080,
        0xE6194B, 0x3CB44B, 0xffe119, 0x0082c8, 0xf58231,
        0x911eb4, 0x46f0f0, 0xf032e6, 0xd2f53c, 0xfabebe,
        0xFFFFFF, 0x000000
        },
        {
        0x911eb4, 0x46f0f0, 0xf032e6, 0xd2f53c, 0xfabebe,
        0xE6194B, 0x3CB44B, 0xffe119, 0x0082c8, 0xf58231,
        0x008080, 0xe6beff, 0xaa6e28, 0xfffac8, 0x800000,
        0xaaffc3, 0x808000, 0xffd8b1, 0x000080, 0x808080,
        0xFFFFFF, 0x000000
        },
        {
        0xfdff00, 0xff9a00, 0x00ff04, 0x00c5ff, 0x000000,
        0xff00a7, 0x46f0f0, 0xf032e6, 0xd2f53c, 0xf58231,
        0x008080, 0xe6beff, 0xaa6e28, 0xfffac8, 0x800000,
        0xaaffc3, 0x808000, 0xffd8b1, 0x000080, 0x808080,
        0xFFFFFF, 0xfabebe
        }
    };
public:
    palettes(){
    }
    std::vector<unsigned int> getPalette(unsigned int n = 0){
        if (n >= pletts.size()){
            n = 0;
        }
        return pletts[n];
    }
    rgb toRGB(int color, int max = 255)
    {
        rgb result;
        result.red = (0x00FF0000 & color)/0x10000;
        result.red = result.red * max / 0xFF;
        result.green = (0x0000FF00 & color)/0x100;
        result.green = result.green * max / 0xFF;
        result.blue = (0x000000FF & color);
        result.blue = result.blue * max / 0xFF;
        return result;
    }
};

#endif // PALETTES_H_INCLUDED
