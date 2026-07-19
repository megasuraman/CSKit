#include "pch.h"
#include "VideoWriter.h"
#include "Logger.h"

#include <atomic>
#include <chrono>
#include <conio.h>
#include <csignal>
#include <dwmapi.h>
#include <filesystem>
#include <iostream>
#include <limits>
#include <shobjidl.h>
#include <sstream>
#include <type_traits> 
#include <vector>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <winrt/Windows.UI.Composition.h>

using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;


std::ofstream Logger::m_file;
#define LOG Logger::Instance()
#pragma comment(lib, "dwmapi.lib")

std::atomic<bool> g_stopRequested(false);
BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		std::cout << "\nInterrupt signal received. Stopping recording safely..." << std::endl;
		g_stopRequested = true;
		return TRUE;
	}
	return FALSE;
}

winrt::com_ptr<ID3D11Device> CreateD3D11Device()
{
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	winrt::com_ptr<ID3D11Device> device;
	winrt::check_hresult(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		device.put(),
		nullptr,
		nullptr
	));

	return device;
}

struct WindowInfo
{
	HWND hwnd;
	std::wstring title;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (!IsWindowVisible(hwnd)) return TRUE;

	int length = GetWindowTextLength(hwnd);
	if (length == 0) return TRUE;

	std::wstring title(length, L'\0');
	GetWindowText(hwnd, &title[0], length + 1);

	// DWM のクローク状態を確認（Windows 10/11 の UWP アプリなどの判定）
	BOOL cloaked = FALSE;
	DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
	if (cloaked) return TRUE;

	auto& windows = *reinterpret_cast<std::vector<WindowInfo>*>(lParam);
	windows.push_back({ hwnd, title });

	return TRUE;
}

winrt::Windows::Graphics::Capture::GraphicsCaptureItem CreateCaptureItemForWindow(HWND hwnd)
{
	auto interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem item{ nullptr };
	winrt::check_hresult(interop_factory->CreateForWindow(hwnd, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), winrt::put_abi(item)));
	return item;
}

IDirect3DDevice CreateDirect3DDevice(ID3D11Device* d3d11Device)
{
	winrt::com_ptr<IDXGIDevice> dxgiDevice;
	winrt::check_hresult(d3d11Device->QueryInterface(IID_PPV_ARGS(dxgiDevice.put())));
	winrt::com_ptr<::IInspectable> inspectable;
	winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put()));
	return inspectable.as<IDirect3DDevice>();
}

// デバッグ用にピクセルデータを.bmpで出力
void SavePixelsToBmp(const std::vector<uint8_t>& pixelData, uint32_t width, uint32_t height, const std::wstring& filename)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file) return;

	BITMAPFILEHEADER fileHeader = {};
	fileHeader.bfType = 0x4D42; // "BM"
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (DWORD)pixelData.size();
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER infoHeader = {};
	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biWidth = (LONG)width;
	infoHeader.biHeight = (LONG)height; // 下から上 (ボトムアップ)
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 32;
	infoHeader.biCompression = BI_RGB;

	file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
	file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
	file.write(reinterpret_cast<const char*>(pixelData.data()), pixelData.size());
}

int main(int argc, char* argv[])
{
    // 実行ファイルのパスを取得して、同じフォルダに log.txt を作成する
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::filesystem::path basePath(exePath);
    std::filesystem::path logPath = basePath;
    logPath.replace_filename(L"log.txt");

    Logger::Init(logPath.string());
    LOG << "--- Session Start ---" << std::endl;

    // .ini ファイルから設定を読み込む
    std::filesystem::path iniPath = basePath;
    if (argc > 1)
    {
        // 第1引数があればそれをINIファイルパスとして使用
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, NULL, 0);
        std::wstring customIniPath;
        customIniPath.resize(size_needed - 1);
        MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, &customIniPath[0], size_needed);
        iniPath = customIniPath;
    }
    else
    {
        // 引数がなければデフォルトの config.ini
        iniPath.replace_filename(L"config.ini");
    }
    
    std::wstring filename = L"output.mp4";
    std::wstring targetWindowTitle = L"";
    std::wstring watchFilePath = L"";
    int recordSeconds = -1;

    if (std::filesystem::exists(iniPath))
    {
        wchar_t buf[1024];
        GetPrivateProfileStringW(L"Capture", L"OutputFile", filename.c_str(), buf, 1024, iniPath.wstring().c_str());
        filename = buf;
        
        GetPrivateProfileStringW(L"Capture", L"TargetWindowTitle", targetWindowTitle.c_str(), buf, 1024, iniPath.wstring().c_str());
        targetWindowTitle = buf;

        GetPrivateProfileStringW(L"Capture", L"WatchFilePath", watchFilePath.c_str(), buf, 1024, iniPath.wstring().c_str());
        watchFilePath = buf;

        recordSeconds = GetPrivateProfileIntW(L"Capture", L"RecordSeconds", recordSeconds, iniPath.wstring().c_str());
        
        LOG << L"Loaded settings from INI: " << iniPath.wstring() << std::endl;
        LOG << L"  OutputFile: " << filename << std::endl;
        LOG << L"  TargetWindowTitle: " << targetWindowTitle << std::endl;
        LOG << L"  WatchFilePath: " << watchFilePath << std::endl;
        LOG << L"  RecordSeconds: " << recordSeconds << std::endl;
    }
    else if (argc > 1)
    {
        LOG << L"Specified INI file not found: " << iniPath.wstring() << std::endl;
    }

    winrt::init_apartment();
    winrt::check_hresult(MFStartup(MF_VERSION));

    if (!std::filesystem::exists(iniPath))
    {
        LOG << "Usage: SimpleCapture.exe [ini_file_path]" << std::endl;
        LOG << "Using default output: output.mp4" << std::endl;
    }

	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	int exitCode = 0;
	try
	{
		// 出力フォルダが存在しない場合は作成
		std::filesystem::path filePath(filename);
		if (filePath.has_parent_path())
		{
			std::error_code ec;
			if (std::filesystem::create_directories(filePath.parent_path(), ec))
			{
				LOG << L"Created directory: " << filePath.parent_path().wstring() << std::endl;
			}
		}

		// 既存のファイルを削除してロックを回避
		DeleteFile(filename.c_str());

		GraphicsCaptureItem item{ nullptr };
		HWND targetHwnd = NULL;

		if (!targetWindowTitle.empty())
		{
			std::vector<std::wstring> filters;
			std::wstringstream ss(targetWindowTitle);
			std::wstring segment;
			while (std::getline(ss, segment, L','))
			{
				if (!segment.empty())
				{
					filters.push_back(segment);
				}
			}

			std::vector<WindowInfo> windows;
			EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

			for (const auto& filter : filters)
			{
				for (const auto& info : windows)
				{
					if (info.title.find(filter) != std::wstring::npos)
					{
						targetHwnd = info.hwnd;
						LOG << L"Found matching window for filter \"" << filter << L"\": " << info.title << L" (HWND: " << (void*)targetHwnd << L")" << std::endl;
						break;
					}
				}
				if (targetHwnd) break;
			}

			if (targetHwnd)
			{
				item = CreateCaptureItemForWindow(targetHwnd);
			}
			else
			{
				LOG << L"No window found matching any of the filters: " << targetWindowTitle << std::endl;
				LOG << "Falling back to picker..." << std::endl;
			}
		}

		if (item == nullptr)
		{
			// キャプチャ対象の選択
			GraphicsCapturePicker picker;
			auto initializeWithWindow = picker.as<IInitializeWithWindow>();
			winrt::check_hresult(initializeWithWindow->Initialize(GetConsoleWindow()));

			LOG << "Select a window or monitor to capture..." << std::endl;
			item = picker.PickSingleItemAsync().get();
		}

		if (item == nullptr)
		{
			LOG << "Capture selection cancelled." << std::endl;
			return 0;
		}

		LOG << L"Selected: " << item.DisplayName().c_str() << std::endl;

		// D3D11 デバイスと VideoWriter の準備
		auto d3d11Device = CreateD3D11Device();
		auto direct3DDevice = CreateDirect3DDevice(d3d11Device.get());

		winrt::com_ptr<ID3D11DeviceContext> context;
		d3d11Device->GetImmediateContext(context.put());

		auto itemSize = item.Size();
		uint32_t width = static_cast<uint32_t>(itemSize.Width);
		uint32_t height = static_cast<uint32_t>(itemSize.Height);

		// H.264 エンコーダ用に切り捨てて偶数にする
		width &= ~1;
		height &= ~1;

		uint32_t fps = 30;

		std::unique_ptr<VideoWriter> writer;

		// ステージングテクスチャの準備（GPUからCPUへの読み取り用）
		D3D11_TEXTURE2D_DESC stagingDesc = {};
		stagingDesc.Width = width;
		stagingDesc.Height = height;
		stagingDesc.MipLevels = 1;
		stagingDesc.ArraySize = 1;
		stagingDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		stagingDesc.SampleDesc.Count = 1;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		winrt::com_ptr<ID3D11Texture2D> stagingTexture;
		winrt::check_hresult(d3d11Device->CreateTexture2D(&stagingDesc, nullptr, stagingTexture.put()));

		using namespace winrt::Windows::System;
		auto controller = DispatcherQueueController::CreateOnDedicatedThread();
		auto queue = controller.DispatcherQueue();

		// キャプチャセッションの構成
		winrt::Windows::Graphics::SizeInt32 captureSize = { static_cast<int32_t>(width), static_cast<int32_t>(height) };

		// FramePool は DispatcherQueue が存在するスレッドで作成する必要がある
		Direct3D11CaptureFramePool framePool{ nullptr };

		// DispatcherQueue.RunSynchronously が使えない場合は、単純なイベント待ち合わせなどで代用
		winrt::handle creationEvent{ CreateEvent(nullptr, FALSE, FALSE, nullptr) };
		queue.TryEnqueue([&]()
			{
				framePool = Direct3D11CaptureFramePool::Create(
					direct3DDevice,
					DirectXPixelFormat::B8G8R8A8UIntNormalized,
					2,
					captureSize);
				SetEvent(creationEvent.get());
			});
		WaitForSingleObject(creationEvent.get(), INFINITE);

		GraphicsCaptureSession session = framePool.CreateCaptureSession(item);

		bool isRecording = false;
		bool debugFrameSaved = false;
		std::vector<uint8_t> pixelData(width * height * 4);

		auto frameArrivedRevoker = framePool.FrameArrived(winrt::auto_revoke, [&](Direct3D11CaptureFramePool const& sender, winrt::Windows::Foundation::IInspectable const&)
			{
				// イベントが来ていることを確認するためのログ
				static bool firstFrameLog = false;
				if (!firstFrameLog)
				{
					std::cout << "First FrameArrived event received." << std::endl;
					firstFrameLog = true;
				}

				if (!isRecording)
				{
					return;
				}

				auto frame = sender.TryGetNextFrame();
				if (!frame) return;

				// タイムスタンプの取得 (100ナノ秒単位)
				// SystemRelativeTime() は TimeSpan を返し、TimeSpan.Duration は 100ナノ秒単位の整数
				auto timestamp = frame.SystemRelativeTime().count();

				// フレームからテクスチャを取得
				auto surface = frame.Surface();
				auto access = surface.as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
				winrt::com_ptr<ID3D11Texture2D> frameTexture;
				winrt::check_hresult(access->GetInterface(IID_PPV_ARGS(frameTexture.put())));

				// ステージングテクスチャにコピー
				context->CopyResource(stagingTexture.get(), frameTexture.get());

				// CPU メモリにマップしてピクセルデータを取得
				D3D11_MAPPED_SUBRESOURCE mapped;
				winrt::check_hresult(context->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mapped));

				uint8_t* pSrc = static_cast<uint8_t*>(mapped.pData);
				for (uint32_t y = 0; y < height; ++y)
				{
					// 動画が上下逆になるのを防ぐため、上下反転させてコピーする
					// MFVideoFormat_RGB32 はボトムアップ形式として扱われることが多いため
					memcpy(pixelData.data() + (height - 1 - y) * width * 4, pSrc + y * mapped.RowPitch, width * 4);
				}
				context->Unmap(stagingTexture.get(), 0);

#if 0			// デバッグ用: 最初の1フレームを画像として保存
				if (!debugFrameSaved)
				{
					debugFrameSaved = true;
					SavePixelsToBmp(pixelData, width, height, L"debug_frame.bmp");
					std::cout << "Debug frame saved to debug_frame.bmp" << std::endl;
				}
#endif

				// 動画ファイルに書き込み
				if (writer)
				{
					//std::cout << "Writing frame with timestamp: " << timestamp << std::endl;
					writer->WriteFrame(pixelData, timestamp);
				}
			});

		if (targetWindowTitle.empty() && watchFilePath.empty())
		{
			std::cout << "Press Enter to START recording..." << std::endl;
			// 外部パイプからの入力を待つ可能性も考慮
			if (std::cin.good())
			{
				std::cin.get();
			}
		}
		else
		{
			if (!targetWindowTitle.empty())
			{
				std::cout << "Target window specified. Starting recording automatically..." << std::endl;
			}
			if (!watchFilePath.empty())
			{
				std::cout << "Watch file specified. Starting recording automatically..." << std::endl;
			}
		}

		std::cout << "Recording started. Press Enter to STOP recording..." << std::endl;
		if (targetHwnd)
		{
			std::cout << "Target window monitoring active. Capture will stop if window is closed." << std::endl;
		}
		if (!watchFilePath.empty())
		{
			std::cout << "Watch file monitoring active. Capture will stop if file is deleted." << std::endl;
		}

		writer = std::make_unique<VideoWriter>(filename, width, height, fps);
		session.StartCapture();
		isRecording = true;
		auto startTime = std::chrono::steady_clock::now();

		// Enterキー待ち、またはウィンドウが閉じられるのを待つ
		if (targetWindowTitle.empty())
		{
			// cin.get() で Enter を待った後の残りをクリア
			// ただし、既に EOF の場合は何もしない
			if (std::cin.good())
			{
				// std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
		}

		while (isRecording)
		{
			// シグナルハンドラからの停止要求をチェック
			if (g_stopRequested)
			{
				break;
			}

			// Enterキーが押されたかチェック (コンソール入力がある場合)
			if (_kbhit())
			{
				int ch = _getch();
				if (ch == '\r' || ch == '\n' || ch == 'q')
				{
					LOG << "Stop requested by user keyboard input." << std::endl;
					break;
				}
			}

			// 標準入力の状態をチェック
			if (std::cin.eof())
			{
				LOG << "Standard input reached EOF. Stopping capture..." << std::endl;
				break;
			}

			// パイプが切断されたかどうか検知
			HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
			if (hStdIn != INVALID_HANDLE_VALUE)
			{
				DWORD dwMode;
				if (GetConsoleMode(hStdIn, &dwMode))
				{
					// コンソール入力モード
				}
				else
				{
					// パイプまたはファイル入力
					DWORD dwAvailable = 0;
					if (!PeekNamedPipe(hStdIn, NULL, 0, NULL, &dwAvailable, NULL))
					{
						// パイプが壊れているか閉じられている
						LOG << "Input pipe closed. Stopping capture..." << std::endl;
						break;
					}

					// パイプから 'q' などの終了コマンドが送られてきていないかチェック
					if (dwAvailable > 0)
					{
						char buffer[1024];
						DWORD dwRead = 0;
						if (ReadFile(hStdIn, buffer, min(sizeof(buffer) - 1, dwAvailable), &dwRead, NULL))
						{
							for (DWORD i = 0; i < dwRead; ++i)
							{
								if (buffer[i] == 'q' || buffer[i] == '\r' || buffer[i] == '\n')
								{
									LOG << "Stop requested via input stream." << std::endl;
									isRecording = false;
									break;
								}
							}
							if (!isRecording) break;
						}
					}
				}
			}

			// ターゲットウィンドウがまだ存在するかチェック
			if (targetHwnd && !IsWindow(targetHwnd))
			{
				LOG << "Target window closed. Stopping capture..." << std::endl;
				break;
			}

			// 監視ファイルが存在するかチェック
			if (!watchFilePath.empty() && !std::filesystem::exists(watchFilePath))
			{
				LOG << L"Watch file \"" << watchFilePath << L"\" no longer exists. Stopping capture..." << std::endl;
				break;
			}

			// 指定秒数が経過したかチェック
			if (recordSeconds > 0)
			{
				auto now = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
				if (elapsed >= recordSeconds)
				{
					LOG << L"Specified duration (" << recordSeconds << L"s) reached. Stopping capture..." << std::endl;
					break;
				}
			}

			Sleep(100);
		}

		isRecording = false;
		session.Close();
		frameArrivedRevoker.revoke();
		framePool.Close();

		controller.ShutdownQueueAsync().get();

		if (writer)
		{
			writer->Close();
			writer.reset();
		}

		// キャプチャ停止後に MFShutdown を呼ぶ前に少し待機（非同期処理の完了を待つ）
		Sleep(500);

		LOG << L"Recording stopped. File saved as " << filename << std::endl;
	}
	catch (winrt::hresult_error const& ex)
	{
		LOG << L"Error: " << ex.message().c_str() << L" (0x" << std::hex << ex.code().value << L")" << std::endl;
		exitCode = 1;
	}

	MFShutdown();
	return exitCode;
}