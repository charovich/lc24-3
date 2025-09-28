typedef enum {
    EnvVar, // c*+ reference???
    TypeVar, // Typedef
    LiteralVar, // Also used in enums etc..
    GlobalVar, // Global integers, pointers, structs
    LocalVar
} VarType;

typedef struct {
    U8 name[NAME_LIMIT+1];
    VarType type;
    Type* decl; // If VarType == TypeVar, specifies the real type. Else it's the type of the variables content
    union {
        U32 literal;
        I32 stack_loc;
    };
} Variable;

#if 1
typedef struct Env {
    Variable var;
    struct Env* next;
} Env;

Variable* resolve_env(Env* env, U8* name) {
    Env* ptr = env;
    while (ptr) {
        if (ptr->var.name && !strcmp(ptr->var.name, name)) {
            return &(ptr->var);
        }
        ptr = ptr->next;
    }
    return 0;
}

Variable* new_var(Env** env, U8* name, VarType type, U8 force) {
    if (!force && resolve_env(*env, name)) {
        return 0;
    }
    Env* new_env = new(Env);
    new_env->next = *env;
    strcpy(new_env->var.name, name);
    new_env->var.type = type;
    *env = new_env;
    return &(new_env->var);
}
#else
typedef struct EnvEntry {
    struct EnvDictEntry* next;
    Variable var;
} EnvEntry;

#define HASH_COUNT 256
typedef EnvDictEntry Env[HASH_COUNT];

typedef struct EnvChain {
    Env* env;
    struct EnvChain* next;
} EnvChain;

U32 fnv1a_hash(U8* str) {
    U32 hash = 0x811c9dc5;

    while(*str) {
        hash = (hash ^ (*str)) * 0x01000193;
    }

    return hash;
}

Variable* resolve_env(Env* env, U8* name) {
    U32 hash = fnv1a_hash(name) % HASH_COUNT;

    EnvEntry* ptr = env[hash];

    while (ptr) {
        if (!strcmp(ptr->var.name, name)) {
            return &(ptr->var);
        }
        ptr = ptr->next;
    }
    return 0;
}
Variable* new_var(Env* a, U8* name, VarType type) {
    if (resolve_env(env, name)) {
        return 0;
    }
    U32 hash = fnv1a_hash(name) % HASH_COUNT;
    EnvEntry* zov = new(EnvEntry);
    zov->next = env[hash];
    strcpy(zov->var.name, name);
    zov->var.type = type;
    env[hash] = zov;
    return &(zov->var);
}
#endif
