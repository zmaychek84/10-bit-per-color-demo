//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#include "pch.h"
#include "winioctl.h"
#include "ntddvdeo.h"

#include "Game.h"
#include "GrayscaleEffect.h"

#include <winrt\Windows.Devices.Display.h>
#include <winrt\Windows.Devices.Enumeration.h>


#include "atlbase.h"
#include "atlstr.h"

void OutputDebugPrintf(const char* strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);

	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugString(CA2W(strBuffer));
}



//using namespace concurrency;

using namespace winrt::Windows::Devices;
using namespace winrt::Windows::Devices::Display;
using namespace winrt::Windows::Devices::Enumeration;

extern void ExitGame();

using namespace DirectX;
using Microsoft::WRL::ComPtr;


Game::Game(PWSTR appTitle)
{
    m_appTitle = appTitle;
	CoInitialize(NULL);

    ConstructorInternal();
}
Game::~Game()
{
    //OutputDebugPrintf("%s \n", __FUNCTION__);
    if(m_d2dEffect)
    m_d2dEffect.Reset();
    if (m_d2dSource)
        m_d2dSource.Reset();
    if (m_deviceResources)
        m_deviceResources.reset();
}
void Game::ConstructorInternal()
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_effectShaderFilename = std::wstring(L"GrayscaleEffect.cso");
    m_effectClsid = CLSID_CustomGrayscaleEffect;

    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height, LPWSTR mediaFileName)
{
    m_FileName = mediaFileName;

    m_deviceResources->SetWindow(window, width, height);
    m_deviceResources->CreateDeviceResources();
    m_deviceResources->SetDpi(96.0f);     // TODO: using default 96 DPI for now
    m_deviceResources->CreateWindowSizeDependentResources();

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    m_deviceResources->PIXBeginEvent(L"Update");

    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    m_deviceResources->PIXEndEvent();
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

void Game::UpdateDxgiColorimetryInfo()
{
    // Output information is cached on the DXGI Factory. If it is stale we need to create
    // a new factory and re-enumerate the displays.
    auto d3dDevice = m_deviceResources->GetD3DDevice();

    ComPtr<IDXGIDevice3> dxgiDevice;
    DX::ThrowIfFailed(d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice)));

    ComPtr<IDXGIAdapter> dxgiAdapter;
    DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

    ComPtr<IDXGIFactory4> dxgiFactory;
    DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

//    if (!dxgiFactory->IsCurrent())
    {
        DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
    }

    // Get information about the display we are presenting to.
    ComPtr<IDXGIOutput> output;
    auto sc = m_deviceResources->GetSwapChain();
    DX::ThrowIfFailed(sc->GetContainingOutput(&output));

    ComPtr<IDXGIOutput6> output6;
    output.As(&output6);

    DX::ThrowIfFailed(output6->GetDesc1(&m_outputDesc));

}

void Game::DrawPattern(ID2D1DeviceContext2 * ctx) //******************* 7.
{
    auto rect = m_deviceResources->GetOutputSize();

    if (!m_effectIsValid)
    {
   
       MessageBox(m_hWindow, L"ERROR: GrayscaleEffect.cso is missing\n",NULL, MB_OK);
    }
    else
    {
        DX::ThrowIfFailed(m_d2dEffect->SetValueByName(
            L"OutputSize",
            D2D1::Point2F((float)rect.right - rect.left, (float)rect.bottom - rect.top)));

        ctx->DrawImage(m_d2dEffect.Get());
    }

}

// Common method to render an image test pattern to the screen.
void Game::DrawImageCommon(ID2D1DeviceContext2* ctx)
{
    if (m_imageIsValid == true)
    {
        // Center the image, draw at 1.0x (pixel) zoom.
        // TODO: Currently we force all D2D rendering to 96 DPI regardless of display DPI.
        // Therefore DIPs = pixels.
        auto targetSize = m_deviceResources->GetOutputSize();
        unsigned int width, height;
        DX::ThrowIfFailed(m_wicSource->GetSize(&width, &height));

        float dX = (targetSize.right - targetSize.left - static_cast<float>(width)) / 2.0f;
        float dY = (targetSize.bottom - targetSize.top - static_cast<float>(height)) / 2.0f;

        ctx->DrawImage(m_d2dSource.Get(), D2D1::Point2F(dX, dY));
    }

}

// Draws the scene.
void Game::Render()
{
    m_deviceResources->PIXBeginEvent(L"Render");

    Clear();

    auto ctx = m_deviceResources->GetD2DDeviceContext();

    ctx->BeginDraw();

	if(m_isPattern)
        DrawPattern(ctx);
    else
        DrawImageCommon(ctx);

    HRESULT hr = ctx->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    // Window size changed also corresponds to switching monitors.
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1920;
    height = 1080;
}


#pragma region Direct3D Resources
void Game::CreateDeviceIndependentResources()
{
    auto dwFactory = m_deviceResources->GetDWriteFactory();
   DX::ThrowIfFailed(GrayscaleEffect::Register(m_deviceResources->GetD2DFactory()));
}


// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto ctx = m_deviceResources->GetD2DDeviceContext();

    if (m_isPattern)
        LoadEffectResources(); 
    else
        LoadImageResources();

    UpdateDxgiColorimetryInfo();

}

void Game::LoadImageResources()
{
    auto wicFactory = m_deviceResources->GetWicImagingFactory();
    auto ctx = m_deviceResources->GetD2DDeviceContext();

    // This test involves an image file.
    if (m_FileName != 0)
    {
        // First, ensure that there is a WIC source (device independent).
        if (m_wicSource == nullptr)
        {
            ComPtr<IWICBitmapDecoder> decoder;
            HRESULT hr = wicFactory->CreateDecoderFromFilename(
                m_FileName,
                nullptr,
                GENERIC_READ,
                WICDecodeMetadataCacheOnDemand,
                &decoder);

            if FAILED(hr)
            {
                if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
                {
                    m_imageIsValid = false;
                    return;
                }
                else
                {
                    DX::ThrowIfFailed(hr);
                }
            }

            ComPtr<IWICBitmapFrameDecode> frame;
            DX::ThrowIfFailed(decoder->GetFrame(0, &frame));

            // Always convert to FP16 for JXR support. We ignore color profiles in this tool.
            WICPixelFormatGUID outFmt = GUID_WICPixelFormat64bppPRGBAHalf;

            ComPtr<IWICFormatConverter> converter;
            DX::ThrowIfFailed(wicFactory->CreateFormatConverter(&converter));
            DX::ThrowIfFailed(converter->Initialize(
                frame.Get(),
                outFmt,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0f,
                WICBitmapPaletteTypeCustom));

            DX::ThrowIfFailed(converter.As(&m_wicSource));
        }

        // Next, ensure that there is a D2D source (device dependent).
        if (m_d2dSource == nullptr)
        {
            assert(m_wicSource != nullptr);

            DX::ThrowIfFailed(ctx->CreateImageSourceFromWic(m_wicSource.Get(), &m_d2dSource));

            // The image is only valid if both WIC and D2D resources are ready.
            m_imageIsValid = true;
        }
    }
}

void Game::LoadEffectResources()
{
    auto ctx = m_deviceResources->GetD2DDeviceContext();
    // This test involves a shader file.
    if (m_effectShaderFilename.compare(L"") != 0)
    {
        assert(m_effectClsid != GUID{});

        try
        {
            DX::ThrowIfFailed(ctx->CreateEffect(m_effectClsid, &m_d2dEffect));
            m_effectIsValid = true;
        }
        catch (std::exception)
        {
            // Most likely caused by a missing cso file. Continue on.
            m_effectIsValid = false;
        }
    }
}

void Game::OnDeviceLost()
{
    
}

void Game::OnDeviceRestored()
{
   // CreateDeviceDependentResources();

}
#pragma endregion
