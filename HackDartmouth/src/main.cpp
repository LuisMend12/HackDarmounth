// main.cpp
#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include <vector>
#include <string>
#include <fstream>

#include <d3d11.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>

using namespace ImPlot;

// declare d3d11 globals
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
HWND g_hWnd = nullptr;

//rendering helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

int main() {
  WNDCLASSEX wc = {
  sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0L, 0L,
  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
  _T("wndclss"), NULL };


  RegisterClassEx(&wc);

  //register window
  g_hWnd = ::CreateWindowEx(WS_EX_TRANSPARENT, wc.lpszClassName, _T("BrainFlux"), WS_OVERLAPPEDWINDOW, 100, 100, 1350, 640, NULL, NULL, wc.hInstance, NULL);
  SetLayeredWindowAttributes(g_hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
  
  if (!CreateDeviceD3D(g_hWnd)) {
    CleanupDeviceD3D();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  UpdateWindow(g_hWnd);
  ShowWindow(g_hWnd, SW_SHOW);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.Fonts->AddFontFromFileTTF("imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
  ImGui::GetStyle().WindowBorderSize = 0.0f;
  ImGui::GetStyle().AntiAliasedLines = true;
  ImGui::StyleColorsDark();

  
  ImVec4 titlebar_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); 
  ImVec4 input_color = ImVec4(0.2f, 0.0f, 0.3f, 1.0f);
  ImVec4 grab_color = ImVec4(0.4f, 0.2f, 0.5f, 1.0f);
  ImVec4 hover_color = ImVec4(0.6f, 0.2f, 0.5f, 1.0f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.Colors[ImGuiCol_TitleBg] = titlebar_color;
  style.Colors[ImGuiCol_TitleBgActive] = titlebar_color;
  style.Colors[ImGuiCol_TitleBgCollapsed] = titlebar_color;
  style.Colors[ImGuiCol_FrameBg] = input_color;
  style.Colors[ImGuiCol_SliderGrab] = grab_color;
  style.Colors[ImGuiCol_FrameBgHovered] = hover_color;

  ImGui_ImplWin32_Init(g_hWnd);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  static std::vector<float> x1, y1;
  std::vector<float> x2, y2;
  std::vector<float> x3, y3;
  std::vector<float> x4, y4;


  //this sucks
  std::ifstream infile("data/compressed_data_vheavy.txt");
  std::string line;
  if (!infile) {
    std::cerr << "failed to open file.\n";
    return 1;
  }

  size_t index = 0;
  while (std::getline(infile, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    y1.push_back(static_cast<float>(std::stoi(line)));
    x1.push_back(static_cast<float>(index++));
  }

  std::ifstream infile2("data/compressed_data_heavy.txt");
  std::string line1;
  if (!infile2) {
    std::cerr << "Failed to open file.\n";
    return 1;
  }

  index = 0;
  while (std::getline(infile2, line1)) {
    if (line1.empty() || line1[0] == '#') {
      continue;
    }
    y2.push_back(static_cast<float>(std::stoi(line1)));
    x2.push_back(static_cast<float>(index++));
  }


  std::ifstream infile3("data/compressed_data_medium.txt");
  std::string line2;
  if (!infile3) {
    std::cerr << "Failed to open file.\n";
    return 1;
  }

  index = 0;
  while (std::getline(infile3, line2)) {
    if (line2.empty() || line2[0] == '#') {
      continue;
    }
    y3.push_back(static_cast<float>(std::stoi(line2)));
    x3.push_back(static_cast<float>(index++));
  }

  std::ifstream infile4("data/compressed_data_light.txt");
  std::string line3;
  if (!infile4) {
    std::cerr << "Failed to open file.\n";
    return 1;
  }

  index = 0;
  while (std::getline(infile4, line3)) {
    if (line3.empty() || line3[0] == '#') {
      continue;
    }
    y4.push_back(static_cast<float>(std::stoi(line3)));
    x4.push_back(static_cast<float>(index++));
  }

  infile.close();

  ImGuiWindowFlags wnd_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

  float tick = 0.0f;
  float time = 500.0f;

  float max_y_scale =  5000.0f;
  float min_y_scale = -5000.0f;

  static bool l1, l2, l3, l4;
  l1 = l2 = l4 = false;
  l3 = true;

  float tick_sz = 0.5f;

  // main loop
  MSG msg = {};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      continue;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10)); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(800, 600));
    ImGui::Begin("Monitor", NULL, wnd_flags);
    tick += tick_sz;
    
    ImPlot::PushStyleColor(ImPlotCol_FrameBg, ImVec4(0.0f, 0.0f, 0.09f, 1.0f));
    ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0.5f, 0.5f, 0.5f, 0.3f));
    if (ImPlot::BeginPlot("Neural Activity Monitor", ImVec2(-1, -1), ImPlotFlags_NoBoxSelect | ImPlotLegendFlags_NoHighlightAxis |ImPlotFlags_NoInputs)) { 

      ImPlot::SetupAxesLimits(tick + time, tick, min_y_scale, max_y_scale, ImPlotCond_Always);
      
      ImPlot::SetNextLineStyle(ImVec4(0, 200, 255, 255), 1.0f);

      if (l1) {
        ImPlot::PlotLine("Plot 1  (V. Heavy)", x1.data(), y1.data(), (int)y1.size());
      }
      if (l2) {
        ImPlot::PlotLine("Plot 2 (Heavy)", x2.data(), y2.data(), (int)y2.size());
      }
      if (l3) {
        ImPlot::PlotLine("Plot 3 (Med)", x3.data(), y3.data(), (int)y3.size());
      }
      if (l4){
        ImPlot::PlotLine("Plot 4 (Light", x4.data(), y4.data(), (int)y4.size());
      }

      ImPlot::EndPlot();
    }
    ImPlot::PopStyleColor();
    ImGui::End();
    ImGui::PopStyleVar(); 
    ImPlot::PopStyleColor();

    ImGui::SetNextWindowPos(ImVec2(800, 0));
    ImGui::SetNextWindowSize(ImVec2(550, 600));
    ImGui::Begin("Control Panel", NULL, wnd_flags);

    ImGui::Text("Settings:");
    ImGui::Dummy(ImVec2(0, 5)); 

    ImGui::PushItemWidth(150);
    ImGui::GetStyle().FramePadding = ImVec2(4, 8);
    ImGui::GetStyle().FrameRounding = 8.0f; 
    ImGui::GetStyle().GrabRounding = 8.0f;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Speed:");
    ImGui::SameLine(75);
    ImGui::SliderFloat("##speed", &tick_sz, 0.001f, 5.00f);
    ImGui::SameLine(250);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Max (Y):");
    ImGui::SameLine(320);
    ImGui::SliderFloat("##max y", &max_y_scale, 0.f, 5000.0f);

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Time(s)");
    ImGui::SameLine(75);
    ImGui::SliderFloat("##time", &time, 5.0f, 1000.0f);
 
    ImGui::SameLine(250);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Min (Y):");
    ImGui::SameLine(320);
    ImGui::SliderFloat("##min y", &min_y_scale, -5000.0f, 0.f);

    ImGui::Dummy(ImVec2(0, 15));

    ImGui::Separator();
    ImGui::Separator();
    
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("Compression settings:");
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::SetCursorPosX(25); 
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Very heavy");
    ImGui::SameLine();
    ImGui::Checkbox("##l1", &l1);

    ImGui::SameLine(150);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Heavy");
    ImGui::SameLine();
    ImGui::Checkbox("##l2", &l2);

    ImGui::SameLine(260);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Medium");
    ImGui::SameLine();
    ImGui::Checkbox("##l3", &l3);

    ImGui::SameLine(375);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Light");
    ImGui::SameLine();
    ImGui::Checkbox("##l4", &l4);

    ImGui::PopItemWidth();
    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::Render();
    
    const float clear_color[4] = { 0.95f, 0.95f, 0.97f, 1.0f };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pSwapChain->Present(1, 0);
  }

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  DestroyWindow(g_hWnd);
  UnregisterClass(wc.lpszClassName, wc.hInstance);

  return 0;
}


bool CreateDeviceD3D(HWND hWnd) {
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 2;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0 };
  if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
    featureLevelArray, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
    &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

void CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
  if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
  if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
  ID3D11Texture2D* pBackBuffer = nullptr;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
  pBackBuffer->Release();
}

void CleanupRenderTarget() {
  if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  switch (msg)
  {
  case WM_SYSCOMMAND:
    if ((w_param & 0xfff0) == SC_KEYMENU)
      return 0;
    break;
  case WM_NCHITTEST:
    if (ImGui::GetMousePos().y < 800 && ImGui::GetMousePos().x < 600)
    {
      // allow menu to be draggable
      LRESULT hit{ DefWindowProc(hwnd, msg, w_param, l_param) };
      if (hit == HTCLIENT)
        hit = HTCAPTION;

      return hit;
    }
    break;
  case WM_CLOSE:
    DestroyWindow(hwnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param);
    return DefWindowProc(hwnd, msg, w_param, l_param);
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}
