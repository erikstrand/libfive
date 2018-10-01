#include "png_writer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <iostream>

PngWriter::PngWriter(): width_(0), height_(0), row_pointers_(nullptr) {}

PngWriter::~PngWriter() {
    if (row_pointers_ != nullptr) {
        Free();
    }
}

void PngWriter::Free() {
    for (int32_t y = 0; y < height_; y++) {
        free(row_pointers_[y]);
    }
    free(row_pointers_);
    width_ = 0;
    height_ = 0;
}

void PngWriter::Allocate(int32_t width, int32_t height) {
    width_ = width;
    height_ = height;
    if (row_pointers_ != nullptr) {
        Free();
    }
    row_pointers_ = (png_bytep*)malloc(sizeof(png_bytep) * height_);
    for (int y = 0; y < height_; y++) {
        row_pointers_[y] = (png_byte*)malloc(row_size());
    }
}

void PngWriter::Clear() {
    for (int y = 0; y < height_; y++) {
        std::memset(row_pointers_[y], 0, row_size());
    }
}

void PngWriter::SetPixel(int32_t x, int32_t y) {
    png_bytep row = row_pointers_[y];
    png_bytep px = &(row[x * 3]);
    px[0] = 255;
    px[1] = 255;
    px[2] = 255;
    //for(int x = 0; x < width; x++) {
    //    png_bytep px = &(row[x * 3]);
    //    px[0] = 0;
    //    px[1] = 0;
    //    px[2] = 0;
    //}
}

void PngWriter::Write(char const* filename) {
    auto fp = fopen(filename, "wb");
    if (!fp) {
        std::cout << "Couldn't make file\n";
        abort();
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cout << "Couldn't make png_structp\n";
        abort();
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cout << "Couldn't make png_structp\n";
        abort();
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cout << "Couldn't set jump\n";
        abort();
    }

    png_init_io(png, fp);
    // Output is 8bit depth, RGB format.
    png_set_IHDR(png,
                 info,
                 width_,
                 height_,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    if (png_get_rowbytes(png, info) != row_size()) {
        std::cout << "Allocated bad amount of memory\n";
        abort();
    }

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png_, 0, PNG_FILLER_AFTER);

    png_write_image(png, row_pointers_);
    png_write_end(png, NULL);

    if (png && info) {
        png_destroy_write_struct(&png, &info);
    }

    fclose(fp);


/*
  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  width_ = 10;
  height_ = 10;
  png_set_IHDR(
    png,
    info,
    width_, height_,
    8,
    //PNG_COLOR_TYPE_RGBA,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // create an image
    png_bytep *row_pointers;
  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height_);
  for(int y = 0; y < height_; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }
    //std::cout << "png_bytep size " << sizeof(png_bytep) << '\n';
    //std::cout << "png row bytes " << png_get_rowbytes(png, info) << '\n';
  for(int y = 0; y < height_; y++) {
    png_bytep row = row_pointers[y];
    for(int x = 0; x < width_; x++) {
      png_bytep px = &(row[x * 3]);
      px[0] = 0;
      px[1] = 0;
      px[2] = 0;
      //px[3] = 255;
      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
    }
  }
  row_pointers[0][0] = 255;
  row_pointers[0][1] = 255;
  row_pointers[0][2] = 255;

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < height_; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  if (png && info)
    png_destroy_write_struct(&png, &info);
    */
}

