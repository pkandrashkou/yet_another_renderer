#include <stdlib.h>
#include <windows.h>

// #include "..\image.c"
#include "..\language.h"
#include "render.cpp"

global bool32 global_is_running;

LRESULT CALLBACK win32_main_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
internal void win32_process_pending_messages();

struct Win32_Screen_Buffer {
    void *memory;
    i32 width;
    i32 height;
    i32 pitch;
    i32 bytes_per_pixel;

    BITMAPINFO bitmap_info;
};

global Win32_Screen_Buffer global_screen_buffer;

internal void win32_resize_screen_buffer(Win32_Screen_Buffer *buffer, i32 width, i32 height);
internal void win32_display_screen_buffer(Win32_Screen_Buffer *buffer, HDC device_context, int window_width, int window_height);

struct Win32_Window_Size {
    i32 width;
    i32 height;
};

internal Win32_Window_Size win32_get_window_size(HWND window);

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    win32_resize_screen_buffer(&global_screen_buffer, 1280, 720);

    WNDCLASS wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = win32_main_window_proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RenderWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                                 // Optional window styles.
        wc.lpszClassName,                  // Window class
        "Yet Another Renderer",            // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,  // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    HDC device_context = GetDC(hwnd);

    global_is_running = true;
    while (global_is_running) {
        win32_process_pending_messages();

        Render_Buffer buffer;
        buffer.memory = global_screen_buffer.memory;
        buffer.width = global_screen_buffer.width;
        buffer.height = global_screen_buffer.height;
        buffer.pitch = global_screen_buffer.pitch;
        update_and_render(&buffer);

        Win32_Window_Size window_size = win32_get_window_size(hwnd);
        win32_display_screen_buffer(&global_screen_buffer, device_context, window_size.width, window_size.height);
    }

    return 0;
}

LRESULT CALLBACK win32_main_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            global_is_running = false;
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(hwnd, &paint);
            Win32_Window_Size window_size = win32_get_window_size(hwnd);
            win32_display_screen_buffer(&global_screen_buffer, hdc, window_size.width, window_size.height);
            EndPaint(hwnd, &paint);
        }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

internal void win32_process_pending_messages() {
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT:
                global_is_running = false;
                break;
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 VKCode = (u32)message.wParam;
                bool32 WasDown = (message.lParam & (1 << 30)) != 0;
                bool32 IsDown = (message.lParam & (1 << 31)) == 0;

                if (WasDown != IsDown) {
                    if (VKCode == 'W') {
                    } else if (VKCode == 'A') {
                    } else if (VKCode == 'S') {
                    } else if (VKCode == 'D') {
                    } else if (VKCode == 'Q') {
                    } else if (VKCode == 'E') {
                    } else if (VKCode == VK_UP) {
                    } else if (VKCode == VK_LEFT) {
                    } else if (VKCode == VK_DOWN) {
                    } else if (VKCode == VK_RIGHT) {
                    } else if (VKCode == VK_ESCAPE) {
                        global_is_running = false;
                    } else if (VKCode == VK_SPACE) {
                    }
                }

                bool32 AltKeyWasDown = (message.lParam & (1 << 29)) != 0;
                if ((VKCode == VK_F4) && AltKeyWasDown) {
                    global_is_running = false;
                }

            } break;
            default:
                TranslateMessage(&message);
                DispatchMessageA(&message);
                break;
        }
    }
}

internal Win32_Window_Size win32_get_window_size(HWND window) {
    RECT rect;
    GetClientRect(window, &rect);

    Win32_Window_Size result;
    result.width = rect.right - rect.left;
    result.height = rect.bottom - rect.top;
    return result;
}

internal void win32_resize_screen_buffer(Win32_Screen_Buffer *buffer, i32 width, i32 height) {
    if (buffer->memory) {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;
    buffer->bytes_per_pixel = 4;
    buffer->pitch = buffer->width * buffer->bytes_per_pixel;

    buffer->bitmap_info.bmiHeader.biSize = sizeof(buffer->bitmap_info.bmiHeader);
    buffer->bitmap_info.bmiHeader.biWidth = width;
    buffer->bitmap_info.bmiHeader.biHeight = height;
    buffer->bitmap_info.bmiHeader.biPlanes = 1;
    buffer->bitmap_info.bmiHeader.biBitCount = 32;
    buffer->bitmap_info.bmiHeader.biCompression = BI_RGB;

    i32 memory_size = buffer->width * buffer->height * buffer->bytes_per_pixel;
    buffer->memory = VirtualAlloc(0, memory_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

internal void win32_display_screen_buffer(Win32_Screen_Buffer *buffer, HDC device_context, int window_width, int window_height) {
    StretchDIBits(
        device_context,        //HDC              hdc,
        0,                     // int              xDest,
        0,                     // int              yDest,
        buffer->width,         //window_width,          // int              DestWidth,
        buffer->height,        //window_height,         // int              DestHeight,
        0,                     // int              xSrc,
        0,                     // int              ySrc,
        buffer->width,         // int              SrcWidth,
        buffer->height,        // int              SrcHeight,
        buffer->memory,        // const VOID       *lpBits,
        &buffer->bitmap_info,  // const BITMAPINFO *lpbmi,
        DIB_RGB_COLORS,        // UINT             iUsage,
        SRCCOPY                // DWORD            rop
    );
}