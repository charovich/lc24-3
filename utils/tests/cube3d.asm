
start:
    mov %ac $C          ; CMD_MATRIX_MODE
    mov %bs 1           ; PROJECTION
    int $45

    mov %ac $15         ; CMD_LOAD_IDENTITY
    int $45

    mov %ac $14         ; CMD_ORTHO
    mov %bs $C00000     ; -2.0f
    mov %cn $400000     ; 2.0f
    mov %dc $C00000     ; -2.0f
    mov %dt $400000     ; 2.0f
    mov %di $C00000     ; -2.0f
    mov %ex $400000     ; 2.0f
    int $45

    mov %ac $C          ; CMD_MATRIX_MODE
    mov %bs 0           ; MODELVIEW
    int $45

    mov %ac 5           ; CMD_ENABLE
    mov %bs $0B71       ; DEPTH_TEST
    int $45

    mov %ac 6           ; CMD_DISABLE
    mov %bs $1221       ; AUTO_FLUSH
    int $45

    mov %ac $F          ; CMD_CLEAR_COLOR
    mov %bs 0
    mov %cn 0
    mov %dc 0
    int $45

    mov %ac 16          ; CMD_GEN_LISTS
    mov %bs 1
    int $45
    mov %fx %ac         ; %fx = cube_list_id

    mov %ac 17          ; CMD_NEW_LIST
    mov %bs %fx
    int $45

    ; CubeK (12 triangles, 6 faces)
    mov %ac 1           ; CMD_BEGIN
    mov %bs 3           ; MODE_TRIANGLES
    int $45

    ; FRONT FACE (RED)
    ; Triangle 1
    mov %ac 4           ; COLOR4UB
    mov %bs 255
    mov %cn 0
    mov %dc 0
    mov %dt 255
    int $45
    
    mov %ac 3           ; VERTEX3F
    mov %bs $BF8000     ; -1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45

    ; Triangle 2
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45

    ; BACK FACE (GREEN)
    mov %ac 4
    mov %bs 0
    mov %cn 255
    mov %dc 0
    mov %dt 255
    int $45
    
    ; Triangle 1
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45

    ; Triangle 2
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45

    ; TOP FACE (BLUE)
    mov %ac 4
    mov %bs 0
    mov %cn 0
    mov %dc 255
    mov %dt 255
    int $45
    
    ; Triangle 1
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45

    ; Triangle 2
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45

    ; BOTTOM FACE (YELLOW)
    mov %ac 4
    mov %bs 255
    mov %cn 255
    mov %dc 0
    mov %dt 255
    int $45
    
    ; Triangle 1
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45

    ; Triangle 2
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45

    ; RIGHT FACE (CYAN)
    mov %ac 4
    mov %bs 0
    mov %cn 255
    mov %dc 255
    mov %dt 255
    int $45
    
    ; Triangle 1
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45

    ; Triangle 2
    mov %ac 3
    mov %bs $3F8000     ; 1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $3F8000     ; 1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45

    ; LEFT FACE (MAGENTA)
    mov %ac 4
    mov %bs 255
    mov %cn 0
    mov %dc 255
    mov %dt 255
    int $45
    
    ; Triangle 1
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, 1.0
    mov %cn $BF8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45

    ; Triangle 2
    mov %ac 3
    mov %bs $BF8000     ; -1.0, -1.0, -1.0
    mov %cn $BF8000
    mov %dc $BF8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, 1.0
    mov %cn $3F8000
    mov %dc $3F8000
    int $45
    
    mov %ac 3
    mov %bs $BF8000     ; -1.0, 1.0, -1.0
    mov %cn $3F8000
    mov %dc $BF8000
    int $45

    mov %ac 2           ; CMD_END
    int $45

    mov %ac 19          ; CMD_END_LIST
    int $45

    mov %lx 0
    mov %hx 0

_loop:
    mov %ac $E          ; CMD_CLEAR
    mov %bs $4100       ; COLOR | DEPTH
    int $45

    mov %ac $15         ; CMD_LOAD_IDENTITY
    int $45


    mov %ac 22          ; CMD_ROTATEI
    mov %bs %lx         ; angle_x
    mov %cn 1           ; X axis
    mov %dc 0
    mov %dt 0
    int $45

    mov %ac 22          ; CMD_ROTATEI
    mov %bs %hx
    mov %cn 0
    mov %dc 1
    mov %dt 0
    int $45

    mov %ac 18          ; CMD_CALL_LIST
    mov %bs %fx
    int $45

    mov %ac 23          ; CMD_FLUSH
    int $45

    mov %ac 2
    add %lx %ac
    
    mov %ac 3
    add %hx %ac

    mov %ac 360
    cmp %lx %ac
    jl _check_y
    mov %lx 0
_check_y:
    cmp %hx %ac
    jl _wait
    mov %hx 0

_wait:
    mov %dc 0
    int $22
    
    jmp _loop
