#include <Windows.h>
#include <tchar.h>
#include "RenderHelp.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
TCHAR szTitle[MAX_LOADSTRING] = _T("Sample_08_win32");                  // 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING] = _T("sample_08_win32");            // 主窗口类名
RenderHelp *rh = nullptr;

float eyeAtZ = -0.01;
float eyeAtX = 0;
Mat4x4f mat_view = matrix_set_lookat({ 0, 0, eyeAtZ }, { 0,0,1 }, { 0,1,0 });	// 摄像机方位
Mat4x4f mat_proj = matrix_set_perspective(3.1415926f * 0.5f, 800 / 600.0, 1.0, 500.0f);
Mat4x4f mat_mvp = mat_view * mat_proj;	// 综合变换矩阵

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL                DrawPrimitive(HDC hDC, RECT rect, RenderHelp* rh);

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        rh = new RenderHelp(rect.right - rect.left, rect.bottom - rect.top);
    }
        break;
    case WM_ERASEBKGND:
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawPrimitive(hdc, ps.rcPaint, rh);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE:
    {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        rh->Init(width, height);
        {
            mat_view = matrix_set_lookat({ 0, 0, eyeAtZ }, { 0,0,1 }, { 0,1,0 });
            mat_proj = matrix_set_perspective(3.1415926f * 0.5f, width / (float)height, 1.0, 500.0f);
            mat_mvp = mat_view * mat_proj;
        }
    }
        break;
    case WM_MOUSEWHEEL:
    {
        DWORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (fwKeys & MK_CONTROL) {
            eyeAtZ += 0.01 * (zDelta / WHEEL_DELTA);
            mat_view = matrix_set_lookat({ 0, 0, eyeAtZ }, { 0,0,1 }, { 0,1,0 });
            mat_mvp = mat_view * mat_proj;
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        delete rh;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL DrawPrimitive(HDC hdc, RECT rect, RenderHelp* rh)
{
    const int VARYING_COLOR = 0;    // 定义一个 varying 的 key

    rh->Clear();
// 顶点数据，由 VS 读取，如有多个三角形，可每次更新 vs_input 再绘制
    struct { Vec4f pos; Vec4f color; } vs_input[3] = {
        { {  0.0,  1.0, 1.00, 1}, {1, 0, 0, 1} },
        { { -1.0, -1.0, 1.00, 1}, {0, 1, 0, 1} },
        { { +1.0, -1.0, 1.00, 1}, {0, 0, 1, 1} },
    };

    // 顶点着色器，初始化 varying 并返回坐标，
    // 参数 index 是渲染器传入的顶点序号，范围 [0, 2] 用于读取顶点数据
    rh->SetVertexShader([&](int index, ShaderContext& output) -> Vec4f {
        output.varying_vec4f[VARYING_COLOR] = vs_input[index].color;
        Vec4f pos = vs_input[index].pos.xyzw()*mat_mvp;  // 扩充成四维矢量并变换
        return pos;		// 直接返回坐标
    });

    // 像素着色器，返回颜色
    rh->SetPixelShader([&](ShaderContext& input) -> Vec4f {
        return input.varying_vec4f[VARYING_COLOR];
    });

    // 渲染并保存
    rh->DrawPrimitive();

    rh->BitBlt(hdc, &rect);

    return true;
}
