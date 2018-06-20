#include <memory>
#include <png.h>

namespace CustomTextureUpdate
{

class PngLoader
{
public:
    void Load(const void *pData, size_t size);
    bool HasLoaded() const { return m_hasLoaded; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetColorType() const { return m_type; }
    unsigned char * GetData() const { return m_data.get(); }

private:
    std::unique_ptr<unsigned char[]> m_data;
    bool m_hasLoaded = false;
	png_uint_32 m_width = 0;
    png_uint_32 m_height = 0;
    png_byte m_type = PNG_COLOR_TYPE_RGBA;
};

}
