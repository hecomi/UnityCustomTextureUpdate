#include <map>
#include "png_loader.h"
#include "Unity/IUnityRenderingExtensions.h"

using namespace CustomTextureUpdate;


namespace
{
    std::map<unsigned int, PngLoader*> g_loaders;
    unsigned int g_loaderId = 0;

    PngLoader * GetLoader(unsigned int id)
    {
        auto it = g_loaders.find(id);
        return (it != g_loaders.end()) ? it->second : nullptr;
    }
}


extern "C"
{

UNITY_INTERFACE_EXPORT unsigned int UNITY_INTERFACE_API CreateLoader()
{
    auto *pLoader = new PngLoader();
    const auto id = g_loaderId++;
    g_loaders.emplace(id, pLoader);
    return id;
}

UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API DestroyLoader(unsigned int id)
{
    auto it = g_loaders.find(id);
    if (it != g_loaders.end())
    {
        delete it->second;
        g_loaders.erase(it);
    }
}

UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API Load(unsigned int id, const void *pData, size_t dataSize)
{
    if (auto *pLoader = GetLoader(id))
    {
        pLoader->Load(pData, dataSize);
    }
}

UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API GetWidth(unsigned int id)
{
    auto *pLoader = GetLoader(id);
    return pLoader ? pLoader->GetWidth() : 0;
}

UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API GetHeight(unsigned int id)
{
    auto *pLoader = GetLoader(id);
    return pLoader ? pLoader->GetHeight() : 0;
}
    
UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API GetColorType(unsigned int id)
{
    auto *pLoader = GetLoader(id);
    return pLoader ? pLoader->GetColorType() : 0;
}
    
UNITY_INTERFACE_EXPORT bool UNITY_INTERFACE_API HasLoaded(unsigned int id)
{
    auto *pLoader = GetLoader(id);
    return pLoader && pLoader->HasLoaded();
}

void OnTextureUpdate(int eventId, void *pData)
{
    const auto event = static_cast<UnityRenderingExtEventType>(eventId);

    if (event == kUnityRenderingExtEventUpdateTextureBegin)
    {
        auto *pParams = reinterpret_cast<UnityRenderingExtTextureUpdateParams*>(pData);
        const auto id = pParams->userData;
        if (auto *pLoader = GetLoader(id))
        {
            if (pLoader->GetWidth() == pParams->width &&
                pLoader->GetHeight() == pParams->height)
            {
                pParams->texData = pLoader->GetData();
            }
            else
            {
                // Error
            }
        }
    }
    else if (event == kUnityRenderingExtEventUpdateTextureEnd)
    {
        auto *pParams = reinterpret_cast<UnityRenderingExtTextureUpdateParams*>(pData);
        pParams->texData = nullptr;
    }
}

UNITY_INTERFACE_EXPORT UnityRenderingEventAndData UNITY_INTERFACE_API GetTextureUpdateCallback()
{
    return OnTextureUpdate;
}

}
