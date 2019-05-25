#include <cstdio>
#include <png.h>
#include "png_loader.h"
#include "util.h"

using namespace CustomTextureUpdate;


void PngLoader::Load(const void *pData, size_t dataSize)
{
    if (dataSize < 8) return;

    const auto *pHeader = reinterpret_cast<const png_byte*>(pData);
    if (png_sig_cmp(pHeader, 0, 8)) return;

    auto png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING,
        nullptr,
        nullptr,
        nullptr);
    if (!png) return;

    auto info = png_create_info_struct(png);
    if (!info) return;

    ScopeReleaser releaser([&]
    {
        png_destroy_read_struct(&png, &info, nullptr);
    });

    struct Data
    {
        const unsigned char *m_pData;
        size_t m_offset;
    };
    Data data
    {
        static_cast<const unsigned char*>(pData),
        8,
    };

    png_set_read_fn(
        png,
        &data,
        [](png_structp png, png_bytep buf, png_size_t size)
        {
            auto &data = *static_cast<Data*>(png_get_io_ptr(png));
            memcpy(buf, data.m_pData + data.m_offset, size);
            data.m_offset += size;
        });

    png_set_sig_bytes(png, 8);
    png_read_png(
        png,
        info,
        PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
        nullptr);

    m_width = png_get_image_width(png, info);
    m_height = png_get_image_height(png, info);
    m_type = png_get_color_type(png, info);
    
    const size_t rowBytes = png_get_rowbytes(png, info);
    m_data = std::make_unique<unsigned char[]>(rowBytes * m_height);

    const auto rows = png_get_rows(png, info);
    for (unsigned int i = 0; i < m_height; ++i)
    {
        const size_t offset = rowBytes * i;
        memcpy(m_data.get() + offset, rows[i], rowBytes);
    }

    m_hasLoaded = true;
}
