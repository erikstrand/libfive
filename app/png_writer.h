#include <png.h>
#include <cstdint>

class PngWriter {
public:
    PngWriter();
    ~PngWriter();

    void Free();
    void Allocate(int32_t width, int32_t height);
    png_bytep* row_pointers() { return row_pointers_; }
    void SetPixel(int32_t x, int32_t y, uint8_t value = 255);
    void SetAllPixelsBlack();
    void Write(char const* filename);

private:
    uint32_t row_size() { return 3 * width_; }

    int32_t width_;
    int32_t height_;
    png_bytep* row_pointers_;
};

