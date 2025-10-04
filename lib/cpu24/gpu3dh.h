#ifndef GPU3DH_H
#define GPU3DH_H 1

#define AC 0x00
#define BS 0x01
#define CN 0x02
#define DC 0x03
#define DT 0x04
#define DI 0x05
#define SP 0x06
#define BP 0x07
#define EX 0x08
#define FX 0x09
#define HX 0x0A
#define LX 0x0B
#define X  0x0C
#define Y  0x0D
#define IX 0x0E
#define IY 0x0F

#define VERTEX_BUFFER_SIZE 1024

void lc3D_Begin(LC* lc, U8 mode);
void lc3D_Color4ub(LC* lc, U8 r, U8 g, U8 b, U8 a);
void lc3D_Vertex3f(LC* lc, float x, float y, float z);
void lc3D_End(LC* lc);

typedef struct {
    float x, y, z;
    U8 r, g, b, a;
} lc_vertex3d;


typedef struct {
    lc_vertex3d verts[VERTEX_BUFFER_SIZE];
    U8 mode;
    U8 count;
    struct { U8 r, g, b, a; } current_color;
    int is_active;
} lc_3d_pipeline_state;

#endif // GPU3DH_H
