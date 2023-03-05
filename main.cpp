#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <GL/GL.h>

#include <stdio.h>

// Return 0 if unable to print error, 1 if successful
DWORD printLastError(const char *const prefixStr)
{
    fprintf(stderr, "%s", prefixStr);
    DWORD errorCode = GetLastError();
    LPVOID buffer = NULL;

    if (errorCode == 0)
    {
        fprintf(stderr, "Unable to retrieve last error\n");
        return 0;
    }
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS, // [in] DWORD dwFlags
        NULL,                               // [in, optional] LPCVOID lpSource
        errorCode,                          // [in] DWORD dwMessageId
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // [in] DWORD dwLanguageId

        // Apparently if FORMAT_MESSAGE_ALLOCATE_BUFFER is used, Windows actually
        // treats this like a LPSTR*, even though the declared type is still
        // LPSTR... weird...
        (LPTSTR)&buffer, // [out] LPTSTR lpBuffer
        0,               // [in] DWORD nSize
        NULL             // [in, optional] va_list *Arguments
    );
    fprintf(stderr, "%u: %s\n", errorCode, (LPCTSTR)buffer);
    LocalFree(buffer);
    return errorCode;
}

int main()
{
    HMODULE moduleHandle = GetModuleHandle(NULL);
    if (!moduleHandle)
    {
        return printLastError("Failed get module handle: ");
    }

    WNDCLASS windowClass{
        0,                                   // UINT        style;
        DefWindowProc,                       // WNDPROC     lpfnWndProc;
        0,                                   // int         cbClsExtra;
        0,                                   // int         cbWndExtra;
        moduleHandle,                        // HINSTANCE   hInstance;
        0,                                   // HICON       hIcon;
        0,                                   // HCURSOR     hCursor;
        0,                                   // HBRUSH      hbrBackground;
        0,                                   // LPCWSTR     lpszMenuName;
        TEXT("GetOpenGLVersionWindowClass"), // LPCWSTR     lpszClassName;
    };

    if (!RegisterClass(&windowClass))
    {
        return printLastError("Failed to register window class: ");
    }

    HWND window = CreateWindow(
        TEXT("GetOpenGLVersionWindowClass"), // [in, optional]  lpClassName,
        TEXT("GetOpenGLVersion"),            // [in, optional]  lpWindowName,
        0,                                   // [in]            dwStyle,
        CW_USEDEFAULT,                       // [in]            x,
        CW_USEDEFAULT,                       // [in]            y,
        CW_USEDEFAULT,                       // [in]            nWidth,
        CW_USEDEFAULT,                       // [in]            nHeight,
        NULL,                                // [in, optional]  hWndParent,
        NULL,                                // [in, optional]  hMenu,
        moduleHandle,                        // [in, optional]  hInstance,
        NULL                                 // [in, optional]  lpParam
    );
    if (!window)
    {
        return printLastError("Failed to create window: ");
    }

    HDC deviceContext = GetDC(window);
    if (!deviceContext)
    {
        return printLastError("Failed to get device context: ");
    }

    // This seems to be the minimum of what needs to be set
    PIXELFORMATDESCRIPTOR pixelFormatDesc{
        sizeof(PIXELFORMATDESCRIPTOR),           // WORD  nSize;
        1,                                       // WORD  nVersion;
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL, // DWORD dwFlags;
        0,                                       // BYTE  iPixelType;
        0,                                       // BYTE  cColorBits;
        0,                                       // BYTE  cRedBits;
        0,                                       // BYTE  cRedShift;
        0,                                       // BYTE  cGreenBits;
        0,                                       // BYTE  cGreenShift;
        0,                                       // BYTE  cBlueBits;
        0,                                       // BYTE  cBlueShift;
        0,                                       // BYTE  cAlphaBits;
        0,                                       // BYTE  cAlphaShift;
        0,                                       // BYTE  cAccumBits;
        0,                                       // BYTE  cAccumRedBits;
        0,                                       // BYTE  cAccumGreenBits;
        0,                                       // BYTE  cAccumBlueBits;
        0,                                       // BYTE  cAccumAlphaBits;
        0,                                       // BYTE  cDepthBits;
        0,                                       // BYTE  cStencilBits;
        0,                                       // BYTE  cAuxBuffers;
        0,                                       // BYTE  iLayerType;
        0,                                       // BYTE  bReserved;
        0,                                       // DWORD dwLayerMask;
        0,                                       // DWORD dwVisibleMask;
        0,                                       // DWORD dwDamageMask;
    };

    int chosenPixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
    if (!chosenPixelFormat)
    {
        return printLastError("Failed choose pixel format: ");
    }

    if (!SetPixelFormat(deviceContext, chosenPixelFormat, &pixelFormatDesc))
    {
        return printLastError("Failed set pixel format: ");
    }

    HGLRC glContext = wglCreateContext(deviceContext);
    if (!wglMakeCurrent(deviceContext, glContext))
    {
        return printLastError("Failed to create OpenGL context: ");
    }

    printf("%s\n", glGetString(GL_VERSION));

    wglDeleteContext(glContext);
    return 0;
}

#else // _WIN32

#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Window rootWindow;
    Display *display;
    XVisualInfo *visualInfo;
    GLint glAttrs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    GLXContext glContext;

    if (!(display = XOpenDisplay(NULL)))
    {
        fprintf(stderr, "Could not connect to X server\n");
        return 1;
    }

    rootWindow = DefaultRootWindow(display);
    if (!rootWindow)
    {
        fprintf(stderr, "Could not get root window\n");
        return 1;
    }

    if (!(visualInfo = glXChooseVisual(display, 0, glAttrs)))
    {
        fprintf(stderr, "Could not choose GL visual\n");
        return 1;
    }

    if (!(glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE)))
    {
        fprintf(stderr, "Unable to create GL context\n");
        return 1;
    }
    
    if (!(glXMakeCurrent(display, rootWindow, glContext)))
    {
        fprintf(stderr, "Unable to make GL context current\n");
        return 1;
    }

    printf("%s\n", glGetString(GL_VERSION));
    return 0;
}

#endif // _WIN32