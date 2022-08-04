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

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Basicmath.h"
#include <map>

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
private:

	HINSTANCE m_hInstance = nullptr;
	HWND      m_hWindow = nullptr;
    Microsoft::WRL::ComPtr<IWICBitmapSource> m_wicSource; // Generated from WIC.
    Microsoft::WRL::ComPtr<ID2D1ImageSourceFromWic> m_d2dSource; // Generated from D2D.
    Microsoft::WRL::ComPtr<ID2D1Effect> m_d2dEffect; // Generated from D2D.
    std::wstring m_effectShaderFilename; // Empty means no shader t.
    GUID m_effectClsid;
    bool m_imageIsValid;
    bool m_effectIsValid; // false means effect file is missing or invalid.

public:

    Game(PWSTR appTitle);
    ~Game();
    // Initialization and management
    void Initialize(HWND window, int width, int height, LPWSTR mediaFileName);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);


    // Properties
    void GetDefaultSize(int& width, int& height) const;


    void SetPattern(bool ispattern) { m_isPattern = ispattern; }
private:

    void ConstructorInternal();
    void Update(DX::StepTimer const& timer);
    void UpdateDxgiColorimetryInfo();

    void Render();
 	void DrawPattern(ID2D1DeviceContext2* ctx);				
    void DrawImageCommon(ID2D1DeviceContext2* ctx);

    // Common rendering subroutines.
    void Clear();
    void CreateDeviceIndependentResources();
    void CreateDeviceDependentResources();
    void LoadImageResources();
    void LoadEffectResources();

	
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    DXGI_OUTPUT_DESC1                                       m_outputDesc;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    float                                   m_totalTime;

    PWSTR                                   m_appTitle;
    LPWSTR   m_FileName = nullptr;
    bool     m_isPattern;
};