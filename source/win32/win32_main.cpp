#include <stdlib.h>
#include <windows.h>

#include "..\image.c"
#include "..\language.h"

global bool32 global_is_running;

global f32 viewport_width = 1;
global f32 viewport_height = 1;
global i32 image_width = 1000;
global i32 image_height = 1000;
global f32 z_plane_distance = 1.0f;

global Image global_image = {1000, 1000, 3};

global Point
viewport_to_pixel(Point point) {
    Point result;

    f32 image_x = (point.x * image_width / (f32)viewport_width);
    f32 image_y = (point.y * image_height / (f32)viewport_height);
    result.x = (image_x + image_width) / 2;
    result.y = (image_y + image_height) / 2;
    return result;
}

global Point
project_vertex_to_pixel(v4 vert) {
    Point result;
    result.x = vert.x * z_plane_distance / (f32)vert.z;
    result.y = vert.y * z_plane_distance / (f32)vert.z;
    return viewport_to_pixel(result);
}

typedef struct Triangle {
    i32 vertex_indices[3];
    Color *color;
} Triangle;

typedef struct Model {
    v3 *vertices;
    i32 vertices_count;

    Triangle *triangles;
    i32 triangles_count;
} Model;

typedef struct Object {
    Model *model;

    v3 scale;
    v3 rotation_axis;
    f32 angle_degrees;
    v3 translation;
} Object;

typedef struct Camera {
    v3 position;
    v3 rotation_axis;
    f32 angle_degrees;
} Camera;

void render_object(Image *image, Object *object, m4 camera_transform) {
    Model *model = object->model;
    v3 scale = object->scale;
    v3 rotation_axis = object->rotation_axis;
    f32 angle_degrees = object->angle_degrees;
    f32 angle_radians = angle_degrees * MY_PI / 180.0f;
    v3 translation = object->translation;

    Point *projected_vertices = (Point *)malloc(sizeof(projected_vertices) * model->vertices_count);
    for (i32 vertex_index = 0; vertex_index < model->vertices_count; vertex_index++) {
        v3 vertex = model->vertices[vertex_index];
        v4 vertex_h = v4(vertex.x, vertex.y, vertex.z, 1);

        m4 transform = m4_init_diagonal(1.0f);
        transform = m4_multiply_m4(transform, m4_scale_m3(scale));
        transform = m4_multiply_m4(transform, m4_rotate(rotation_axis, angle_radians));
        transform = m4_multiply_m4(transform, m4_translate_v3(translation));

        transform = m4_multiply_m4(transform, camera_transform);

        v4 transformed_vertex = v4_multiply_m4(vertex_h, transform);
        projected_vertices[vertex_index] = project_vertex_to_pixel(transformed_vertex);
    }

    for (i32 triangle_index = 0; triangle_index < model->triangles_count; triangle_index++) {
        Triangle triangle = model->triangles[triangle_index];
        Point p1 = projected_vertices[triangle.vertex_indices[0]];
        Point p2 = projected_vertices[triangle.vertex_indices[1]];
        Point p3 = projected_vertices[triangle.vertex_indices[2]];
        image_draw_triangle_wireframe_point(image,
                                            p1,
                                            p2,
                                            p3,
                                            *triangle.color);
    }
    free(projected_vertices);
}

void clear_screen(Image *image) {
    for (i32 y = 0; y < image->height; ++y) {
        for (i32 x = 0; x < image->width; ++x) {
            for (i32 channel = 0; channel < image->channel_number; ++channel) {
                image_set_pixel(image, x, y, color_white);
            }
        }
    }
}

void render_scene(Image *image, Camera camera, Object *objects, i32 objects_count) {
    f32 camera_angle_radians = camera.angle_degrees * MY_PI / 180.0f;
    m4 camera_matrix = m4_init_diagonal(1.0f);
    camera_matrix = m4_multiply_m4(camera_matrix, m4_translate_v3(v3_multiply_scalar(camera.position, -1)));
    camera_matrix = m4_multiply_m4(camera_matrix, m4_transposed(m4_rotate(camera.rotation_axis, camera_angle_radians)));

    for (i32 i = 0; i < objects_count; i++) {
        Object object = objects[i];
        render_object(image, &object, camera_matrix);
    }
}

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

        // TODO: render something to buffer

        Win32_Window_Size window_size = win32_get_window_size(hwnd);
        win32_display_screen_buffer(&global_screen_buffer, device_context, window_size.width, window_size.height);
    }

    return 0;

    // Image *image = &global_image;
    // image->pixels = (u8 *)calloc(image->width * image->height * image->channel_number, sizeof(image->pixels));

    // Model model;
    // v3 vertices[] = {
    //     {1, 1, 1},
    //     {-1, 1, 1},
    //     {-1, 1, -1},
    //     {1, 1, -1},
    //     {1, -1, 1},
    //     {-1, -1, 1},
    //     {-1, -1, -1},
    //     {1, -1, -1}};
    // model.vertices = vertices;
    // model.vertices_count = ArraySize(vertices);

    // Triangle triangles[] = {
    //     {0, 1, 2, &color_red},
    //     {2, 3, 0, &color_red},
    //     {0, 3, 7, &color_green},
    //     {7, 4, 0, &color_green},
    //     {0, 4, 5, &color_blue},
    //     {5, 0, 1, &color_blue},
    //     {1, 5, 2, &color_orange},
    //     {2, 5, 6, &color_orange},
    //     {6, 2, 3, &color_yellow},
    //     {3, 7, 6, &color_yellow},
    //     {6, 5, 7, &color_purple},
    //     {7, 4, 5, &color_purple}};
    // model.triangles = triangles;
    // model.triangles_count = ArraySize(triangles);

    // // Object cube;
    // // cube.model = &model;
    // // cube.scale = v3(2.0f, 2.0f, 2.0f);
    // // cube.rotation_axis = v3(0.0f, 1.0f, 0.0f);
    // // cube.angle_degrees = 30;
    // // cube.translation = v3(-1.5f, 1.0f, 7.0f);

    // // render_object(image, &cube);
    // v3 rotation_axis = v3(0.0f, 0.0f, 1.0f);
    // m4 camera_matrix = m4_rotate(rotation_axis, 0);

    // Object _cube;
    // _cube.model = &model;
    // _cube.scale = v3(1.0f, 1.0f, 1.0f);
    // _cube.rotation_axis = v3(0.0f, 1.0f, 0.0f);
    // _cube.angle_degrees = 45;
    // _cube.translation = v3(0.0f, 0.0f, 4.0f);

    // Camera camera;
    // camera.position = v3(0.0f, 0.0f, 0.0f);
    // camera.rotation_axis = v3(0.0f, 0.0f, 1.0f);
    // camera.angle_degrees = 20;

    // clear_screen(image);
    // render_scene(image, camera, &_cube, 1);
    // image_save(image, "../render.png");

    // free(image->pixels);
    // return 0;
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
        window_width,          // int              DestWidth,
        window_height,         // int              DestHeight,
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