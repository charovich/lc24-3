#ifndef GPU3D_H
#define GPU3D_H 1

#include <stdio.h>
#include <stdlib.h>
#include <cpu24/cpu24h.h>
#include <cpu24/gpu3dh.h>

#define MATRIX_STACK_DEPTH            16
#define MAX_DISPLAY_LISTS             256
#define MAX_LIST_COMMANDS             1024

// --- Consts ---
#define LC3D_MODE_POINTS                0x0
#define LC3D_MODE_LINES                 0x1
#define LC3D_MODE_TRIANGLES             0x3
#define LC3D_MODE_QUADS                 0x5
#define LC3D_MODELVIEW                  0x0
#define LC3D_PROJECTION                 0x1
#define LC3D_DEPTH_TEST                 0x0B71
#define LC3D_COLOR_BUFFER_BIT           0x00004000
#define LC3D_DEPTH_BUFFER_BIT           0x00000100
#define LC3D_LESS                       0x0201
#define LC3D_ALWAYS                     0x0207
#define LC3D_AUTO_FLUSH                 0x1221
#define LC3D_COMPILE                    0x1300
#define LC3D_COMPILE_AND_EXECUTE        0x1301
#define LC3D_NO_ERROR                   0x0000
#define LC3D_INVALID_ENUM               0x0500
#define LC3D_INVALID_VALUE              0x0501
#define LC3D_INVALID_OPERATION          0x0502
#define LC3D_STACK_OVERFLOW             0x0503
#define LC3D_STACK_UNDERFLOW            0x0504
#define LC3D_OUT_OF_MEMORY              0x0505
#define LC3D_VIEWPORT                   0x0BA2
#define LC3D_MODELVIEW_MATRIX           0x0BA6
#define LC3D_PROJECTION_MATRIX          0x0BA7
#define LC3D_MAX_MODELVIEW_STACK_DEPTH  0x0D36
#define LC3D_MAX_PROJECTION_STACK_DEPTH 0x0D38


// command ids
#define CMD_BEGIN           1
#define CMD_END             2
#define CMD_VERTEX3F        3
#define CMD_COLOR4UB        4
#define CMD_ENABLE          5
#define CMD_DISABLE         6
#define CMD_TRANSLATEF      7
#define CMD_SCALEF          8
#define CMD_ROTATEF         9
#define CMD_PUSH_MATRIX     10
#define CMD_POP_MATRIX      11
#define CMD_MATRIX_MODE     12
#define CMD_DEPTH_FUNC      13
#define CMD_CLEAR           14
#define CMD_CLEAR_COLOR     15
#define CMD_GEN_LISTS       16
#define CMD_NEW_LIST        17
#define CMD_CALL_LIST       18
#define CMD_END_LIST        19
#define CMD_ORTHO           20
#define CMD_LOAD_IDENTITY   21
#define CMD_ROTATEI         22
#define CMD_FLUSH           23
#define CMD_VIEWPORT        24
#define CMD_GET_ERROR       25
#define CMD_IS_ENABLED      26

// --- LC3D States ---
typedef struct { int x, y; float r, g, b, a, z; } scanline_vert;
typedef struct { float m[4][4]; } mat4;
typedef struct { float x, y, z, w; } vec4;
typedef struct { int command_id; float args[4]; } lc3d_command;
typedef struct { int is_compiled; lc3d_command commands[MAX_LIST_COMMANDS]; int command_count; lc_vertex3d* compiled_object_verts; int compiled_vert_count;  U8 compiled_mode; } lc3d_display_list;

static lc_3d_pipeline_state pipeline_state;
static int current_matrix_mode = LC3D_MODELVIEW;
static mat4 modelview_matrix;
static mat4 projection_matrix;
static U8 lc3d_clear_color = 0;

static mat4 modelview_stack[MATRIX_STACK_DEPTH];
static int modelview_stack_top = -1;
static mat4 projection_stack[MATRIX_STACK_DEPTH];
static int projection_stack_top = -1;

static float* depth_buffer = NULL;
static int depth_test_enabled = 0;
static int depth_function = LC3D_LESS;

static int auto_flush_enabled = 1;

static lc3d_display_list display_lists[MAX_DISPLAY_LISTS] = {0};
static int recording_list_id = -1;
static int list_compile_mode = LC3D_COMPILE;

static int lc3d_error = LC3D_NO_ERROR;

static int viewport_x = 0;
static int viewport_y = 0;
static int viewport_width = WINW;
static int viewport_height = WINH;

float lerp(float a, float b, float t) { if (t < 0.0f) t = 0.0f; if (t > 1.0f) t = 1.0f; return a + (b - a) * t; }
float u32_to_float(U32 val) {
    union {
        U32 u;
        F32 f;
    } converter;
    
    converter.u = val << 8;
    
    return converter.f;
}

void mat4_identity(mat4* mat) {
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) mat->m[i][j] = (i == j) ? 1.0f : 0.0f;
}

void mat4_multiply(mat4* res, const mat4* a, const mat4* b) {
    mat4 tmp;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tmp.m[i][j] = a->m[i][0] * b->m[0][j] + a->m[i][1] * b->m[1][j] + a->m[i][2] * b->m[2][j] + a->m[i][3] * b->m[3][j];
        }
    }
    *res = tmp;
}

vec4 mat4_multiply_vec4(const mat4* mat, const vec4* v) {
    vec4 res;
    res.x = mat->m[0][0] * v->x + mat->m[1][0] * v->y + mat->m[2][0] * v->z + mat->m[3][0] * v->w;
    res.y = mat->m[0][1] * v->x + mat->m[1][1] * v->y + mat->m[2][1] * v->z + mat->m[3][1] * v->w;
    res.z = mat->m[0][2] * v->x + mat->m[1][2] * v->y + mat->m[2][2] * v->z + mat->m[3][2] * v->w;
    res.w = mat->m[0][3] * v->x + mat->m[1][3] * v->y + mat->m[2][3] * v->z + mat->m[3][3] * v->w;
    return res;
}

void put_pixel(LC* lc, int x, int y, float z, U8 color_index) {
    if (x < 0 || x >= WINW || y < 0 || y >= WINH) return;
    
    int index = y * WINW + x;
    
    if (depth_test_enabled) {
        int pass = 0;
        if (depth_function == LC3D_LESS && z < depth_buffer[index]) pass = 1;
        if (depth_function == LC3D_ALWAYS) pass = 1;

        if (pass) {
            depth_buffer[index] = z;
            lc->mem[0x400000 + index] = color_index;
        }
    } else {
        lc->mem[0x400000 + index] = color_index;
    }
}

U8 find_closest_color(U8 r, U8 g, U8 b, int x, int y) {
    static const int bayer[2][2] = {{0, 2}, {3, 1}};
    int threshold = bayer[y & 1][x & 1];
    
    r = (r + threshold * 8 > 255) ? 255 : r + threshold * 8;
    g = (g + threshold * 8 > 255) ? 255 : g + threshold * 8;
    b = (b + threshold * 8 > 255) ? 255 : b + threshold * 8;
    
    int r_idx = (r * 7) / 255;
    int g_idx = (g * 7) / 255;
    int b_idx = (b * 3) / 255;
    
    return (r_idx << 5) | (g_idx << 2) | b_idx;
}

void lc3D_InitPalette(LC* lc) {
    for (int i = 0; i < 256; i++) {
        U8 r = ((i >> 5) & 0x07) * 255 / 7; // R
        U8 g = ((i >> 2) & 0x07) * 255 / 7; // G
        U8 b = (i & 0x03) * 255 / 3;       //  B
        U16 rgb555 = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        lc->mem[0x4A0000 + i * 2] = rgb555 & 0xFF;
        lc->mem[0x4A0000 + i * 2 + 1] = rgb555 >> 8;
    }
    printf("[GPU3D] RGB 3-3-2 palette initialized.\n");
}

void lc3D_SetError(int error) {
    if (lc3d_error == LC3D_NO_ERROR) {
        lc3d_error = error;
    }
}

void draw_line(LC* lc, int x0, int y0, float z0, int x1, int y1, float z1, U8 color_index) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    int len = (dx > dy) ? dx : dy;
    float dz = (len == 0) ? 0 : (z1 - z0) / len;
    float current_z = z0;

    for(;;){
        put_pixel(lc, x0, y0, current_z, color_index);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
        current_z += dz;
    }
}

void sort_vertices_by_y(int coords[][2], U8 colors[]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (coords[j][1] > coords[j+1][1]) {
                int tx = coords[j][0], ty = coords[j][1];
                coords[j][0] = coords[j+1][0]; coords[j][1] = coords[j+1][1];
                coords[j+1][0] = tx; coords[j+1][1] = ty;
                U8 tc = colors[j]; colors[j] = colors[j+1]; colors[j+1] = tc;
            }
        }
    }
}


void fill_triangle(LC* lc, scanline_vert v[3]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (v[j].y > v[j+1].y) { scanline_vert temp = v[j]; v[j] = v[j+1]; v[j+1] = temp; }
        }
    }
    scanline_vert v0 = v[0], v1 = v[1], v2 = v[2];
    if (v0.y == v2.y) return;

    for (int y = v0.y; y <= v2.y; y++) {
        if (y < 0 || y >= WINH) continue;
        float x_a, r_a, g_a, b_a, z_a;
        float x_b, r_b, g_b, b_b, z_b;

        if (y < v1.y) {
            float t_a = (v1.y == v0.y) ? 1.0f : (float)(y - v0.y) / (v1.y - v0.y);
            x_a = lerp(v0.x, v1.x, t_a);
            r_a = lerp(v0.r, v1.r, t_a); g_a = lerp(v0.g, v1.g, t_a); b_a = lerp(v0.b, v1.b, t_a); z_a = lerp(v0.z, v1.z, t_a);
        } else {
            float t_a = (v2.y == v1.y) ? 1.0f : (float)(y - v1.y) / (v2.y - v1.y);
            x_a = lerp(v1.x, v2.x, t_a);
            r_a = lerp(v1.r, v2.r, t_a); g_a = lerp(v1.g, v2.g, t_a); b_a = lerp(v1.b, v2.b, t_a); z_a = lerp(v1.z, v2.z, t_a);
        }
        
        float t_b = (v2.y == v0.y) ? 1.0f : (float)(y - v0.y) / (v2.y - v0.y);
        x_b = lerp(v0.x, v2.x, t_b);
        r_b = lerp(v0.r, v2.r, t_b); g_b = lerp(v0.g, v2.g, t_b); b_b = lerp(v0.b, v2.b, t_b); z_b = lerp(v0.z, v2.z, t_b);

        if (x_a > x_b) {
            float tmp_x = x_a; x_a = x_b; x_b = tmp_x;
            float tmp_c = r_a; r_a = r_b; r_b = tmp_c; tmp_c = g_a; g_a = g_b; g_b = tmp_c; tmp_c = b_a; b_a = b_b; b_b = tmp_c;
            float tmp_z = z_a; z_a = z_b; z_b = tmp_z;
        }

        int start_x = (int)x_a, end_x = (int)x_b;
        if (end_x > start_x) {
            float inv_dx = 1.0f / (end_x - start_x);
            for (int x = start_x; x <= end_x; x++) {
                float t = (float)(x - start_x) * inv_dx;
                U8 r = (U8)lerp(r_a, r_b, t), g = (U8)lerp(g_a, g_b, t), b = (U8)lerp(b_a, b_b, t);
                float z = lerp(z_a, z_b, t);
                put_pixel(lc, x, y, z, find_closest_color(r, g, b, x, y));
            }
        } else {
             put_pixel(lc, start_x, y, z_a, find_closest_color((U8)r_a, (U8)g_a, (U8)b_a, start_x, z_a));
        }
    }
}

mat4* get_current_matrix() {
    return (current_matrix_mode == LC3D_MODELVIEW) ? &modelview_matrix : &projection_matrix;
}

// --- LC3D API ---

void lc3D_MatrixMode(int mode) {
    if (mode == LC3D_MODELVIEW || mode == LC3D_PROJECTION) {
        current_matrix_mode = mode;
    } else {
        lc3D_SetError(LC3D_INVALID_ENUM);
    }
}

int lc3D_GetError() {
    int error = lc3d_error;
    lc3d_error = LC3D_NO_ERROR;
    return error;
}

int lc3D_IsEnabled(int cap) {
    switch (cap) {
        case LC3D_DEPTH_TEST: return depth_test_enabled;
        case LC3D_AUTO_FLUSH: return auto_flush_enabled;
        default: 
            lc3D_SetError(LC3D_INVALID_ENUM);
            return 0;
    }
}


void lc3D_Viewport(int x, int y, int width, int height) {
    if (width < 0 || height < 0) {
        lc3D_SetError(LC3D_INVALID_VALUE);
        return;
    }
    viewport_x = x;
    viewport_y = y;
    viewport_width = width;
    viewport_height = height;
}

void lc3D_LoadIdentity() {
    if (current_matrix_mode == LC3D_MODELVIEW) {
        mat4_identity(&modelview_matrix);
    } else {
        mat4_identity(&projection_matrix);
    }
}

void lc3D_PushMatrix() {
    if (current_matrix_mode == LC3D_MODELVIEW) {
        if (modelview_stack_top >= MATRIX_STACK_DEPTH - 1) {
            lc3D_SetError(LC3D_STACK_OVERFLOW);
            return;
        }
        modelview_stack_top++;
        modelview_stack[modelview_stack_top] = modelview_matrix;
    } else if (current_matrix_mode == LC3D_PROJECTION) {
        if (projection_stack_top >= MATRIX_STACK_DEPTH - 1) {
            lc3D_SetError(LC3D_STACK_OVERFLOW);
            return;
        }
        projection_stack_top++;
        projection_stack[projection_stack_top] = projection_matrix;
    }
}

void lc3D_PopMatrix() {
    if (current_matrix_mode == LC3D_MODELVIEW) {
        if (modelview_stack_top < 0) {
            lc3D_SetError(LC3D_STACK_UNDERFLOW);
            return;
        }
        modelview_matrix = modelview_stack[modelview_stack_top];
        modelview_stack_top--;
    } else if (current_matrix_mode == LC3D_PROJECTION) {
        if (projection_stack_top < 0) {
            lc3D_SetError(LC3D_STACK_UNDERFLOW);
            return;
        }
        projection_matrix = projection_stack[projection_stack_top];
        projection_stack_top--;
    }
}

void lc3D_ClearColor(U8 r, U8 g, U8 b) {
    lc3d_clear_color = find_closest_color(r, g, b, 0, 0);
}

void lc3D_Clear(LC* lc, int mask) {
    if (mask & LC3D_COLOR_BUFFER_BIT) {
        memset(lc->mem + 0x400000, lc3d_clear_color, WINW * WINH);
    }
    if (mask & LC3D_DEPTH_BUFFER_BIT && depth_buffer) {
        for (int i = 0; i < WINW * WINH; i++) {
            depth_buffer[i] = 1.0f;
        }
    }
}

void lc3D_Translatef(float x, float y, float z) {
    mat4 t, temp;
    mat4_identity(&t);
    t.m[3][0] = x; t.m[3][1] = y; t.m[3][2] = z;
    mat4_multiply(&temp, get_current_matrix(), &t);
    *get_current_matrix() = temp;
}

void lc3D_Scalef(float x, float y, float z) {
    mat4 s, temp;
    mat4_identity(&s);
    s.m[0][0] = x; s.m[1][1] = y; s.m[2][2] = z;
    mat4_multiply(&temp, get_current_matrix(), &s);
    *get_current_matrix() = temp;
}

void lc3D_Rotatef(float angle, float x, float y, float z) {
    float rad = angle * 3.14159265f / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);
    float len = sqrtf(x*x + y*y + z*z);
    if (len < 0.0001f) return;
    
    x /= len;
    y /= len;
    z /= len;
    float one_minus_c = 1.0f - c;
    
    mat4 r;
    mat4_identity(&r);
    
    r.m[0][0] = x*x*one_minus_c + c;
    r.m[0][1] = x*y*one_minus_c + z*s;
    r.m[0][2] = x*z*one_minus_c - y*s;
    
    r.m[1][0] = y*x*one_minus_c - z*s;
    r.m[1][1] = y*y*one_minus_c + c;
    r.m[1][2] = y*z*one_minus_c + x*s;
    
    r.m[2][0] = z*x*one_minus_c + y*s;
    r.m[2][1] = z*y*one_minus_c - x*s;
    r.m[2][2] = z*z*one_minus_c + c;
    
    mat4 temp;
    mat4_multiply(&temp, get_current_matrix(), &r);
    *get_current_matrix() = temp;
}

void lc3D_Rotatei(int angle, int x, int y, int z) {
    float angle_f = (float)angle;
    float x_f = (float)x;
    float y_f = (float)y;
    float z_f = (float)z;
    
    lc3D_Rotatef(angle_f, x_f, y_f, z_f);
}

void lc3D_Ortho(float left, float right, float bottom, float top, float nearVal, float farVal) {
    if (left == right || bottom == top || nearVal == farVal) {
        lc3D_SetError(LC3D_INVALID_VALUE);
        return;
    }
    mat4 ortho, temp;
    mat4_identity(&ortho);
    ortho.m[0][0] = 2.0f / (right - left);
    ortho.m[1][1] = 2.0f / (top - bottom);
    ortho.m[2][2] = -2.0f / (farVal - nearVal);
    ortho.m[3][0] = -(right + left) / (right - left);
    ortho.m[3][1] = -(top + bottom) / (top - bottom);
    ortho.m[3][2] = -(farVal + nearVal) / (farVal - nearVal);
    mat4_multiply(&temp, get_current_matrix(), &ortho);
    *get_current_matrix() = temp;
}

void lc3D_Enable(int cap) {
    if (recording_list_id != -1) {
        lc3d_display_list* list = &display_lists[recording_list_id];
        if (list->command_count < MAX_LIST_COMMANDS) {
            list->commands[list->command_count].command_id = CMD_ENABLE;
            list->commands[list->command_count].args[0] = cap;
            list->command_count++;
        }
        return;
    }
    switch (cap) {
        case LC3D_DEPTH_TEST: depth_test_enabled = 1; break;
        case LC3D_AUTO_FLUSH: auto_flush_enabled = 1; break;
        default: lc3D_SetError(LC3D_INVALID_ENUM); break;
    }
}

void lc3D_Disable(int cap) {
    if (recording_list_id != -1) {
        lc3d_display_list* list = &display_lists[recording_list_id];
        if (list->command_count < MAX_LIST_COMMANDS) {
            list->commands[list->command_count].command_id = CMD_DISABLE;
            list->commands[list->command_count].args[0] = cap;
            list->command_count++;
        }
        return;
    }
    switch (cap) {
        case LC3D_DEPTH_TEST: depth_test_enabled = 0; break;
        case LC3D_AUTO_FLUSH: auto_flush_enabled = 0; break;
        default: lc3D_SetError(LC3D_INVALID_ENUM); break;
    }
}

void lc3D_DepthFunc(int func) {
    if (func != LC3D_LESS && func != LC3D_ALWAYS) {
        lc3D_SetError(LC3D_INVALID_ENUM);
        return;
    }
    depth_function = func;
}

int lc3D_GenLists(int range) {
    if (range < 0) {
        lc3D_SetError(LC3D_INVALID_VALUE);
        return 0;
    }
    for (int i = 1; i < MAX_DISPLAY_LISTS; i++) {
        if (display_lists[i].command_count == 0 && !display_lists[i].is_compiled) 
            return i;
    }
    return 0;
}


void lc3D_NewList(int list_id, int mode) {
    if (list_id <= 0 || list_id >= MAX_DISPLAY_LISTS) {
        lc3D_SetError(LC3D_INVALID_VALUE);
        return;
    }
    if (mode != LC3D_COMPILE && mode != LC3D_COMPILE_AND_EXECUTE) {
        lc3D_SetError(LC3D_INVALID_ENUM);
        return;
    }
    if (recording_list_id != -1) {
        lc3D_SetError(LC3D_INVALID_OPERATION);
        return;
    }
    
    recording_list_id = list_id;
    list_compile_mode = mode;
    
    lc3d_display_list* list = &display_lists[list_id];
    list->command_count = 0;
    list->is_compiled = 0;
    
    if (list->compiled_object_verts) {
        free(list->compiled_object_verts);
        list->compiled_object_verts = NULL;
    }
}

void lc3D_EndList(LC* lc) {
    if (recording_list_id == -1) {
        lc3D_SetError(LC3D_INVALID_OPERATION);
        return;
    }
    
    lc3d_display_list* list = &display_lists[recording_list_id];
    
    int list_id = recording_list_id;
    recording_list_id = -1;
    
    int old_active = pipeline_state.is_active;
    int old_count = pipeline_state.count;
    
    for (int i = 0; i < list->command_count; i++) {
        lc3d_command* cmd = &list->commands[i];
        switch (cmd->command_id) {
            case CMD_BEGIN:    
                lc3D_Begin(lc, (U8)cmd->args[0]); 
                break;
            case CMD_VERTEX3F: 
                lc3D_Vertex3f(lc, cmd->args[0], cmd->args[1], cmd->args[2]); 
                break;
            case CMD_COLOR4UB: 
                lc3D_Color4ub(lc, (U8)cmd->args[0], (U8)cmd->args[1], 
                             (U8)cmd->args[2], (U8)cmd->args[3]); 
                break;
        }
    }
    
    if (pipeline_state.count > 0) {
        list->compiled_object_verts = (lc_vertex3d*)malloc(
            sizeof(lc_vertex3d) * pipeline_state.count
        );
        if (!list->compiled_object_verts) {
            lc3D_SetError(LC3D_OUT_OF_MEMORY);
            pipeline_state.is_active = old_active;
            pipeline_state.count = old_count;
            return;
        }
        
        list->compiled_vert_count = pipeline_state.count;
        list->compiled_mode = pipeline_state.mode;
        list->is_compiled = 1;
        
        memcpy(list->compiled_object_verts, pipeline_state.verts, 
               sizeof(lc_vertex3d) * pipeline_state.count);
    }
    
    pipeline_state.is_active = old_active;
    pipeline_state.count = old_count;
}


void lc3D_Begin(LC* lc, U8 mode) {
    (void)lc;
    
    if (recording_list_id != -1) {
        lc3d_display_list* list = &display_lists[recording_list_id];
        if (list->command_count < MAX_LIST_COMMANDS) {
            list->commands[list->command_count].command_id = CMD_BEGIN;
            list->commands[list->command_count].args[0] = (float)mode;
            list->command_count++;
        }
        return;
    }
    
    if (pipeline_state.is_active) {
        lc3D_SetError(LC3D_INVALID_OPERATION);
        printf("[GPU3D] Error: Nested lc3D_Begin call!\n");
        return;
    }
    
    if (mode != LC3D_MODE_POINTS && mode != LC3D_MODE_LINES && 
        mode != LC3D_MODE_TRIANGLES && mode != LC3D_MODE_QUADS) {
        lc3D_SetError(LC3D_INVALID_ENUM);
        return;
    }
    
    pipeline_state.is_active = 1;
    pipeline_state.mode = mode;
    pipeline_state.count = 0;
    pipeline_state.current_color.r = 255;
    pipeline_state.current_color.g = 255;
    pipeline_state.current_color.b = 255;
    pipeline_state.current_color.a = 255;
}

void lc3D_Color4ub(LC* lc, U8 r, U8 g, U8 b, U8 a) {
    if (recording_list_id != -1) {
        lc3d_display_list* list = &display_lists[recording_list_id];
        if (list->command_count < MAX_LIST_COMMANDS) {
            list->commands[list->command_count].command_id = CMD_COLOR4UB;
            list->commands[list->command_count].args[0] = (float)r;
            list->commands[list->command_count].args[1] = (float)g;
            list->commands[list->command_count].args[2] = (float)b;
            list->commands[list->command_count].args[3] = (float)a;
            list->command_count++;
        }
        return;
    }
    (void)lc;
    pipeline_state.current_color.r = r;
    pipeline_state.current_color.g = g;
    pipeline_state.current_color.b = b;
    pipeline_state.current_color.a = a;
}


void lc3D_Vertex3f(LC* lc, float x, float y, float z) {
    if (recording_list_id != -1) {
        lc3d_display_list* list = &display_lists[recording_list_id];
        if (list->command_count < MAX_LIST_COMMANDS) {
            list->commands[list->command_count].command_id = CMD_VERTEX3F;
            list->commands[list->command_count].args[0] = x;
            list->commands[list->command_count].args[1] = y;
            list->commands[list->command_count].args[2] = z;
            list->command_count++;
        }
        return;
    }

    (void)lc;
    if (!pipeline_state.is_active) {
        lc3D_SetError(LC3D_INVALID_OPERATION);
        return;
    }
    
    if (pipeline_state.count >= VERTEX_BUFFER_SIZE) {
        lc3D_SetError(LC3D_OUT_OF_MEMORY);
        return;
    }

    lc_vertex3d* v = &pipeline_state.verts[pipeline_state.count];
    v->x = x; v->y = y; v->z = z;
    v->r = pipeline_state.current_color.r;
    v->g = pipeline_state.current_color.g;
    v->b = pipeline_state.current_color.b;
    v->a = pipeline_state.current_color.a;
    pipeline_state.count++;
}

void lc3D_End(LC* lc) {
    if (recording_list_id != -1) {
        lc3d_display_list* list = &display_lists[recording_list_id];
        if (list->command_count < MAX_LIST_COMMANDS) {
            list->commands[list->command_count].command_id = CMD_END;
            list->command_count++;
        }
        return;
    }
    
    if (!pipeline_state.is_active) {
        lc3D_SetError(LC3D_INVALID_OPERATION);
        return;
    }
    
    if (pipeline_state.count == 0) { 
        pipeline_state.is_active = 0; 
        return; 
    }
    
    pipeline_state.is_active = 0;

    mat4 mvp_matrix;
    mat4_multiply(&mvp_matrix, &projection_matrix, &modelview_matrix);

    scanline_vert v_data[VERTEX_BUFFER_SIZE];
    
    for (int i = 0; i < pipeline_state.count; i++) {
        lc_vertex3d* v_in = &pipeline_state.verts[i];
        vec4 temp_v = {v_in->x, v_in->y, v_in->z, 1.0f};
        temp_v = mat4_multiply_vec4(&mvp_matrix, &temp_v);

        float ndc_x = temp_v.x / temp_v.w;
        float ndc_y = temp_v.y / temp_v.w;
        float ndc_z = temp_v.z / temp_v.w;
        
        v_data[i].x = viewport_x + (ndc_x + 1.0f) * 0.5f * viewport_width;
        v_data[i].y = viewport_y + (-ndc_y + 1.0f) * 0.5f * viewport_height;
        v_data[i].z = (ndc_z + 1.0f) * 0.5f;
        v_data[i].r = v_in->r;
        v_data[i].g = v_in->g;
        v_data[i].b = v_in->b;
        v_data[i].a = v_in->a;
    }

    switch (pipeline_state.mode) {
        case LC3D_MODE_POINTS:
            for (int i = 0; i < pipeline_state.count; i++)
                put_pixel(lc, v_data[i].x, v_data[i].y, v_data[i].z, 
                         find_closest_color(v_data[i].r, v_data[i].g, v_data[i].b, v_data[i].x, v_data[i].y));
            break;
        case LC3D_MODE_LINES:
            for (int i = 0; i < pipeline_state.count; i += 2)
                if (i + 1 < pipeline_state.count)
                    draw_line(lc, v_data[i].x, v_data[i].y, v_data[i].z, 
                             v_data[i+1].x, v_data[i+1].y, v_data[i+1].z, 
                             find_closest_color(v_data[i].r, v_data[i].g, v_data[i].b, v_data[i].x, v_data[i].y));
            break;
        case LC3D_MODE_TRIANGLES:
            for (int i = 0; i < pipeline_state.count; i += 3)
                if (i + 2 < pipeline_state.count)
                    fill_triangle(lc, &v_data[i]);
            break;
        case LC3D_MODE_QUADS:
            for (int i = 0; i < pipeline_state.count; i += 4) {
                if (i + 3 < pipeline_state.count) {
                    scanline_vert tri1[] = {v_data[i], v_data[i+1], v_data[i+2]};
                    fill_triangle(lc, tri1);
                    scanline_vert tri2[] = {v_data[i], v_data[i+2], v_data[i+3]};
                    fill_triangle(lc, tri2);
                }
            }
            break;
    }
    
    if (auto_flush_enabled) {
        GGpage_RGB555LE(lc);
    }
    pipeline_state.count = 0;
}

void lc3D_Flush(LC* lc) {
    GGpage_RGB555LE(lc);
}

void lc3D_CallList(LC* lc, int list_id) {
    if (list_id <= 0 || list_id >= MAX_DISPLAY_LISTS) {
        lc3D_SetError(LC3D_INVALID_VALUE);
        return;
    }
    
    if (recording_list_id != -1) {
        lc3D_SetError(LC3D_INVALID_OPERATION);
        return;
    }
    
    lc3d_display_list* list = &display_lists[list_id];
    
    if (list->is_compiled) {
        mat4 mvp_matrix;
        mat4_multiply(&mvp_matrix, &projection_matrix, &modelview_matrix);
        
        scanline_vert* screen_verts = (scanline_vert*)malloc(
            sizeof(scanline_vert) * list->compiled_vert_count
        );
        
        if (!screen_verts) {
            lc3D_SetError(LC3D_OUT_OF_MEMORY);
            return;
        }
        
        for (int i = 0; i < list->compiled_vert_count; i++) {
            lc_vertex3d* v_in = &list->compiled_object_verts[i];
            vec4 temp_v = {v_in->x, v_in->y, v_in->z, 1.0f};
            temp_v = mat4_multiply_vec4(&mvp_matrix, &temp_v);
            
            float ndc_x = temp_v.x / temp_v.w;
            float ndc_y = temp_v.y / temp_v.w;
            float ndc_z = temp_v.z / temp_v.w;
            
            screen_verts[i].x = viewport_x + (ndc_x + 1.0f) * 0.5f * viewport_width;
            screen_verts[i].y = viewport_y + (-ndc_y + 1.0f) * 0.5f * viewport_height;
            screen_verts[i].z = (ndc_z + 1.0f) * 0.5f;
            screen_verts[i].r = v_in->r;
            screen_verts[i].g = v_in->g;
            screen_verts[i].b = v_in->b;
            screen_verts[i].a = v_in->a;
        }
        
        switch (list->compiled_mode) {
            case LC3D_MODE_TRIANGLES:
                for (int i = 0; i < list->compiled_vert_count; i += 3) {
                    if (i + 2 < list->compiled_vert_count)
                        fill_triangle(lc, &screen_verts[i]);
                }
                break;
            case LC3D_MODE_QUADS:
                for (int i = 0; i < list->compiled_vert_count; i += 4) {
                    if (i + 3 < list->compiled_vert_count) {
                        scanline_vert tri1[] = {
                            screen_verts[i], 
                            screen_verts[i+1], 
                            screen_verts[i+2]
                        };
                        fill_triangle(lc, tri1);
                        scanline_vert tri2[] = {
                            screen_verts[i], 
                            screen_verts[i+2], 
                            screen_verts[i+3]
                        };
                        fill_triangle(lc, tri2);
                    }
                }
                break;
            case LC3D_MODE_LINES:
                for (int i = 0; i < list->compiled_vert_count; i += 2) {
                    if (i + 1 < list->compiled_vert_count)
                        draw_line(lc, 
                            screen_verts[i].x, screen_verts[i].y, screen_verts[i].z,
                            screen_verts[i+1].x, screen_verts[i+1].y, screen_verts[i+1].z,
                            find_closest_color(screen_verts[i].r, screen_verts[i].g, 
                                             screen_verts[i].b, screen_verts[i].x, 
                                             screen_verts[i].y));
                }
                break;
        }
        
        free(screen_verts);
    } else {
        for (int i = 0; i < list->command_count; i++) {
            lc3d_command* cmd = &list->commands[i];
            switch (cmd->command_id) {
                case CMD_BEGIN:    lc3D_Begin(lc, (U8)cmd->args[0]); break;
                case CMD_END:      lc3D_End(lc); break;
                case CMD_VERTEX3F: lc3D_Vertex3f(lc, cmd->args[0], cmd->args[1], cmd->args[2]); break;
                case CMD_COLOR4UB: lc3D_Color4ub(lc, (U8)cmd->args[0], (U8)cmd->args[1], 
                                                  (U8)cmd->args[2], (U8)cmd->args[3]); break;
            }
        }
    }
}


void lc3D_Init(LC* lc) {
    (void)lc;
    pipeline_state.is_active = 0;
    pipeline_state.count = 0;
    lc3D_MatrixMode(LC3D_PROJECTION);
    lc3D_LoadIdentity();
    lc3D_MatrixMode(LC3D_MODELVIEW);
    lc3D_LoadIdentity();
    lc3D_InitPalette(lc);
    
    if (depth_buffer == NULL) {
        depth_buffer = (float*)malloc(sizeof(float) * WINW * WINH);
        if (!depth_buffer) {
            printf("[GPU3D] FATAL: Failed to allocate depth buffer!\n");
            return;
        }
        for (int i = 0; i < WINW * WINH; i++) {
            depth_buffer[i] = 1.0f;
        }
    }
    viewport_x = 0;
    viewport_y = 0;
    viewport_width = WINW;
    viewport_height = WINH;
}

void lc3D_HandleInterrupt(LC* lc) {
    U32 command = lc->reg[AC].word;

    switch (command) {
        case CMD_BEGIN:         lc3D_Begin(lc, (U8)lc->reg[BS].word); break;
        case CMD_END:           lc3D_End(lc); break;
        case CMD_VERTEX3F:      lc3D_Vertex3f(lc, u32_to_float(lc->reg[BS].word), u32_to_float(lc->reg[CN].word), u32_to_float(lc->reg[DC].word)); break;
        case CMD_COLOR4UB:      lc3D_Color4ub(lc, (U8)lc->reg[BS].word, (U8)lc->reg[CN].word, (U8)lc->reg[DC].word, (U8)lc->reg[DT].word); break;
        case CMD_ENABLE:        lc3D_Enable((int)lc->reg[BS].word); break;
        case CMD_DISABLE:       lc3D_Disable((int)lc->reg[BS].word); break;
        case CMD_TRANSLATEF:    lc3D_Translatef(u32_to_float(lc->reg[BS].word), u32_to_float(lc->reg[CN].word), u32_to_float(lc->reg[DC].word)); break;
        case CMD_ROTATEF:       lc3D_Rotatef(u32_to_float(lc->reg[BS].word), u32_to_float(lc->reg[CN].word), u32_to_float(lc->reg[DC].word), u32_to_float(lc->reg[DT].word)); break;
        case CMD_SCALEF:        lc3D_Scalef(u32_to_float(lc->reg[BS].word), u32_to_float(lc->reg[CN].word), u32_to_float(lc->reg[DC].word)); break;
        case CMD_CLEAR:         lc3D_Clear(lc, lc->reg[BS].word); break;
        case CMD_CLEAR_COLOR:   lc3D_ClearColor(lc->reg[BS].word, lc->reg[CN].word, lc->reg[DC].word); break;
        case CMD_DEPTH_FUNC:    lc3D_DepthFunc(lc->reg[BS].word); break;
        case CMD_LOAD_IDENTITY: lc3D_LoadIdentity(); break;
        case CMD_MATRIX_MODE:   lc3D_MatrixMode(lc->reg[BS].word); break;
        case CMD_PUSH_MATRIX:   lc3D_PushMatrix(); break;
        case CMD_POP_MATRIX:    lc3D_PopMatrix(); break;
        case CMD_GEN_LISTS:     int list_id = lc3D_GenLists(lc->reg[BS].word); lc->reg[AC].word = list_id; break;
        case CMD_NEW_LIST:      lc3D_NewList(lc->reg[BS].word, lc->reg[CN].word); break;
        case CMD_END_LIST:      lc3D_EndList(lc); break;
        case CMD_CALL_LIST:     lc3D_CallList(lc, lc->reg[BS].word); break;
        case CMD_ORTHO:         lc3D_Ortho(u32_to_float(lc->reg[BS].word), u32_to_float(lc->reg[CN].word), u32_to_float(lc->reg[DC].word), u32_to_float(lc->reg[DT].word), u32_to_float(lc->reg[DI].word), u32_to_float(lc->reg[EX].word)); break;
        case CMD_ROTATEI:       lc3D_Rotatei((int)lc->reg[BS].word, (int)lc->reg[CN].word, (int)lc->reg[DC].word, (int)lc->reg[DT].word); break;
        case CMD_FLUSH:         lc3D_Flush(lc); break;
        case CMD_VIEWPORT:      lc3D_Viewport((int)lc->reg[BS].word, (int)lc->reg[CN].word, (int)lc->reg[DC].word, (int)lc->reg[DT].word); break;
        case CMD_GET_ERROR:     lc->reg[AC].word = lc3D_GetError(); break;
        case CMD_IS_ENABLED:    lc->reg[AC].word = lc3D_IsEnabled(lc->reg[BS].word); break;
        default:
            printf("GPU3D: Unknown command code %d\n", command);
            lc3D_SetError(LC3D_INVALID_ENUM);
            break;
    }
}

#endif // GPU3D_H
