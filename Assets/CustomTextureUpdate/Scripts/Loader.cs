using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Profiling;
using System.Collections;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace CustomTextureUpdate 
{

public class Loader : MonoBehaviour
{
    [SerializeField]
    string path = "hecomi.png";

    CommandBuffer command_;
    uint id_ = uint.MaxValue;
    Texture2D texture_;

    void Start()
    {
        command_ = new CommandBuffer();
        command_.name = "CustomTextureUpdeate";

        StartCoroutine(LoadImage());
    }

    void OnDestroy()
    {
        if (id_ != uint.MaxValue)
        {
            Lib.DestroyLoader(id_);
        }

        command_.Dispose();
        Destroy(texture_);
    }

    IEnumerator LoadImage()
    {
        var url = System.IO.Path.Combine(Application.streamingAssetsPath, path);
#if UNITY_EDITOR
        url = "file://" + url;
#endif

        using (var www = new WWW(url)) 
        {
            yield return www;
            if (string.IsNullOrEmpty(www.error))
            {
                OnDataLoaded(www.bytes);
            }
            else
            {
                Debug.LogError(www.error);
            }
        }
    }

    async void OnDataLoaded(byte[] data)
    {
        var handle = GCHandle.Alloc(data, GCHandleType.Pinned);
        var pointer = handle.AddrOfPinnedObject();
        await Task.Run(() => 
        {
            id_ = Lib.CreateLoader();
            Lib.Load(id_, pointer, data.Length);
        });
        handle.Free();

        if (!Lib.HasLoaded(id_))
        {
            Debug.LogErrorFormat("failed to load {0}.", path);
            return;
        }

        var width = Lib.GetWidth(id_);
        var height = Lib.GetHeight(id_);
        // TODO: check texture format
        texture_ = new Texture2D(width, height, TextureFormat.RGBA32, false);

        var renderer = GetComponent<Renderer>();
        renderer.material.mainTexture = texture_;

        Profiler.BeginSample("Loader.cs - CustomTextureUpdate");
        {
            var callback = Lib.GetTextureUpdateCallback();
            command_.IssuePluginCustomTextureUpdate(callback, texture_, id_);
            Graphics.ExecuteCommandBuffer(command_); 
            command_.Clear();
        }
        Profiler.EndSample();
    }
}

}