start:
    mov %ac 6           ; CMD_DISABLE
    mov %bs $1221       ; LC3D_AUTO_FLUSH
    int $45

    mov %ac $C          ; CMD_MATRIX_MODE
    mov %bs 1           ; PROJECTION
    int $45

    mov %ac $15         ; CMD_LOAD_IDENTITY
    int $45

    mov %ac $14         ; CMD_ORTHO
    mov %bs $BF8000     ; -1.0f
    mov %cn $3F8000     ; 1.0f
    mov %dc $BF8000
    mov %dt $3F8000
    mov %di $BF8000
    mov %ex $3F8000
    int $45

    mov %ac $C          ; CMD_MATRIX_MODE
    mov %bs 0           ; MODELVIEW
    int $45

    mov %ac 5           ; CMD_ENABLE
    mov %bs $0B71       ; DEPTH_TEST
    int $45

    mov %ac $F          ; CMD_CLEAR_COLOR
    mov %bs 0
    mov %cn 0
    mov %dc 0
    int $45

    mov %ac 16          ; CMD_GEN_LISTS
    mov %bs 1
    int $45
    mov %fx %ac         ; %fx = list_id

    mov %ac 17          ; CMD_NEW_LIST
    mov %bs %fx
    int $45

    mov %ac 1           ; CMD_BEGIN
    mov %bs 3           ; MODE_TRIANGLES
    int $45
    
    ; Vertex 1 (RED)
    mov %ac 4
    mov %bs 255
    mov %cn 0
    mov %dc 0
    mov %dt 255
    int $45
    mov %ac 3
    mov %bs 0           ; X = 0.0
    mov %cn $3E8000     ; Y = 0.25
    mov %dc 0
    int $45

    ; Vertex 2 (GREEN)
    mov %ac 4
    mov %bs 0
    mov %cn 255
    mov %dc 0
    mov %dt 255
    int $45
    mov %ac 3
    mov %bs $BE1555     ; X = -0.21
    mov %cn $BDE666     ; Y = -0.125
    mov %dc 0
    int $45

    ; Vertex 3 (BLUE)
    mov %ac 4
    mov %bs 0
    mov %cn 0
    mov %dc 255
    mov %dt 255
    int $45
    mov %ac 3
    mov %bs $3E1555     ; X = 0.21
    mov %cn $BDE666     ; Y = -0.125
    mov %dc 0
    int $45
    
    mov %ac 2           ; CMD_END
    int $45

    mov %ac 19          ; CMD_END_LIST
    int $45

    mov %lx 0

_loop:
    mov %ac $E          ; CMD_CLEAR
    mov %bs $4100
    int $45

    mov %ac $15         ; CMD_LOAD_IDENTITY
    int $45

    mov %ac 22          ; CMD_ROTATEI
    mov %bs %lx         ; angle
    mov %cn 0
    mov %dc 1           ; Y axis
    mov %dt 0
    int $45

    mov %ac 8           ; CMD_SCALEF
    mov %bs $3FC000     ; 1.5f
    mov %cn $3FC000
    mov %dc $3F8000     ; 1.0f
    int $45

    mov %ac 18          ; CMD_CALL_LIST
    mov %bs %fx
    int $45

    
    ; Triangle 1:
    mov %ac $15         ; LOAD_IDENTITY
    int $45
    
    mov %ac 22          ; ROTATEI
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7           ; TRANSLATEF
    mov %bs $3F4CCC     ; X = 0.8
    mov %cn 0
    mov %dc 0
    int $45
    
    mov %ac 18          ; CALL_LIST
    mov %bs %fx
    int $45

    ; Triangle 2:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs $3F1999     ; X = 0.6
    mov %cn $3F1999     ; Y = 0.6
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    ; Triangle 3:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs 0
    mov %cn $3F4CCC     ; Y = 0.8
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    ; Triangle 4:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs $BF1999     ; X = -0.6
    mov %cn $3F1999     ; Y = 0.6
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    ; Triangle 5:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs $BF4CCC     ; X = -0.8
    mov %cn 0
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    ; Triangle 6:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs $BF1999     ; X = -0.6
    mov %cn $BF1999     ; Y = -0.6
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    ; Triangle 7:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs 0
    mov %cn $BF4CCC     ; Y = -0.8
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    ; Triangle 8:
    mov %ac $15
    int $45
    
    mov %ac 22
    mov %bs %lx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45
    
    mov %ac 7
    mov %bs $3F1999     ; X = 0.6
    mov %cn $BF1999     ; Y = -0.6
    mov %dc 0
    int $45
    
    mov %ac 18
    mov %bs %fx
    int $45

    mov %ac 23          ; CMD_FLUSH
    int $45

    mov %ac 5           ; +5
    add %lx %ac
    
    mov %ac 360
    cmp %lx %ac
    jl _no_reset
    mov %lx 0
_no_reset:
    
    mov %dc 16
    int $22
    
    jmp _loop