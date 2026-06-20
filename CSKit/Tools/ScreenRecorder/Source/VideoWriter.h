#pragma once
#include "pch.h"
#include <iostream>

// ScreenRecorder.cpp で定義されている Logger クラスへのアクセスを簡略化
// （循環参照を避けるため、ここでは標準出力への出力に留めるか、
//   ScreenRecorder.cpp 側でグローバルな LOG マクロが有効な環境でコンパイルされることを期待する）
// 確実に log.txt にも出したい場合は、外部宣言するか関数を介する。
#ifdef LOG
#define VW_LOG LOG
#else
#define VW_LOG std::wcerr
#endif

class VideoWriter
{
public:
    VideoWriter(const std::wstring& filename, uint32_t width, uint32_t height, uint32_t fps)
        : m_width(width), m_height(height), m_fps(fps), m_frameDuration(10000000 / fps), m_startTime(0), m_lastTimestamp(-1)
    {
        HRESULT hr = MFCreateSinkWriterFromURL(filename.c_str(), nullptr, nullptr, m_writer.put());
        if (FAILED(hr))
        {
            VW_LOG << L"Failed to create sink writer for: " << filename << L" (HRESULT: 0x" << std::hex << hr << L")" << std::endl;
            if (hr == 0x80070020) // ERROR_SHARING_VIOLATION
            {
                VW_LOG << L"The file is being used by another process." << std::endl;
            }
            winrt::throw_hresult(hr);
        }

        // 出力メディアタイプの設定 (H.264)
        winrt::com_ptr<IMFMediaType> outType;
        winrt::check_hresult(MFCreateMediaType(outType.put()));
        winrt::check_hresult(outType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
        winrt::check_hresult(outType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264));

        // ビットレートの設定 (解像度とFPSに応じた計算)
        uint32_t bitrate = static_cast<uint32_t>(width * height * fps * 0.15); 
        if (bitrate < 15000000) bitrate = 15000000; // 最低 15Mbps
        winrt::check_hresult(outType->SetUINT32(MF_MT_AVG_BITRATE, bitrate)); 
        
        winrt::check_hresult(outType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
        winrt::check_hresult(MFSetAttributeSize(outType.get(), MF_MT_FRAME_SIZE, width, height));
        winrt::check_hresult(MFSetAttributeRatio(outType.get(), MF_MT_FRAME_RATE, fps, 1));
        winrt::check_hresult(MFSetAttributeRatio(outType.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1));
        winrt::check_hresult(m_writer->AddStream(outType.get(), &m_streamIndex));

        // 入力メディアタイプの設定 (RGB32)
        winrt::com_ptr<IMFMediaType> inType;
        winrt::check_hresult(MFCreateMediaType(inType.put()));
        winrt::check_hresult(inType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
        winrt::check_hresult(inType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
        winrt::check_hresult(inType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive));
        winrt::check_hresult(MFSetAttributeSize(inType.get(), MF_MT_FRAME_SIZE, width, height));
        winrt::check_hresult(MFSetAttributeRatio(inType.get(), MF_MT_FRAME_RATE, fps, 1));
        winrt::check_hresult(MFSetAttributeRatio(inType.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1));
        winrt::check_hresult(m_writer->SetInputMediaType(m_streamIndex, inType.get(), nullptr));

        // エンコーダのパラメータ調整（必要に応じて）
        winrt::com_ptr<ICodecAPI> codecApi;
        if (SUCCEEDED(m_writer->GetServiceForStream(m_streamIndex, GUID_NULL, IID_PPV_ARGS(codecApi.put()))))
        {
            // 低遅延モードの設定（リアルタイムキャプチャに有用）
            VARIANT var = {};
            var.vt = VT_BOOL;
            var.boolVal = VARIANT_TRUE;
            // 一部のエンコーダや環境ではサポートされていない場合があるため、失敗しても続行する
            codecApi->SetValue(&CODECAPI_AVLowLatencyMode, &var);
        }

        // 書き込み開始
        winrt::check_hresult(m_writer->BeginWriting());
    }

    void WriteFrame(const std::vector<uint8_t>& pixelData, LONGLONG timestamp100ns)
    {
        if (m_startTime == 0) m_startTime = timestamp100ns;
        LONGLONG relativeTimestamp = timestamp100ns - m_startTime;

        // タイムスタンプが逆転または重複しないように調整（Media Foundation の要件）
        if (relativeTimestamp <= m_lastTimestamp)
        {
            relativeTimestamp = m_lastTimestamp + 1;
        }
        m_lastTimestamp = relativeTimestamp;

        const DWORD cbBuffer = static_cast<DWORD>(pixelData.size());
        winrt::com_ptr<IMFMediaBuffer> buffer;
        winrt::check_hresult(MFCreateMemoryBuffer(cbBuffer, buffer.put()));

        BYTE* pData = nullptr;
        winrt::check_hresult(buffer->Lock(&pData, nullptr, nullptr));
        memcpy(pData, pixelData.data(), cbBuffer);
        winrt::check_hresult(buffer->Unlock());
        winrt::check_hresult(buffer->SetCurrentLength(cbBuffer));

        winrt::com_ptr<IMFSample> sample;
        winrt::check_hresult(MFCreateSample(sample.put()));
        winrt::check_hresult(sample->AddBuffer(buffer.get()));
        
        winrt::check_hresult(sample->SetSampleTime(relativeTimestamp));
        winrt::check_hresult(sample->SetSampleDuration(m_frameDuration));

        winrt::check_hresult(m_writer->WriteSample(m_streamIndex, sample.get()));
    }

    void Close()
    {
        if (m_writer)
        {
            HRESULT hr = m_writer->Finalize();
            if (FAILED(hr))
            {
                std::wcerr << L"Warning: Sink Writer Finalize failed (0x" << std::hex << hr << L")" << std::endl;
            }
            m_writer = nullptr;
        }
    }

    ~VideoWriter()
    {
        try
        {
            Close();
        }
        catch (...)
        {
            // デストラクタ内での例外は抑制する
        }
    }

private:
    winrt::com_ptr<IMFSinkWriter> m_writer;
    DWORD m_streamIndex = 0;
    uint32_t m_width, m_height, m_fps;
    LONGLONG m_frameDuration;
    LONGLONG m_startTime;
    LONGLONG m_lastTimestamp;
};
