typedef enum {
    IntType,
    FltType,
    PtrType,
    // Degradable types
    FieldType,
    ArrType,
    CodeType
} TypeType;

typedef struct Type {
    TypeType type;
    U32 size;
    struct Type* item; // In functions this is return value
    void* field;
} Type;

typedef struct Field {
    U8* name;
    U32 offset;
    Type* type;
    struct Field* next;
} Field;

U0 print_type(Type* type) {
    /* Погоди, это реально? */
    switch (type->type) {
        case IntType:
            printf("U%d", type->size * 8);
            break;
        case FltType:
            printf("F%d", type->size * 8);
            break;
        case PtrType:
            printf("*");
            print_type(type->item);
            break;
        case ArrType:
            printf("[%d]", type->size / type->item->size);
            print_type(type->item);
            break;
        case CodeType:
            printf("(...)=>");
            print_type(type->item);
            break;
    }
}

U8 cmp_types(Type*, Type*);

U8 cmp_fields(Field* a, Field* b) {
    while (1) {
        // check if it's end
        if (!(a && b)) return a == b;
        if (!cmp_types(a->type, b->type)) return 0;
        a = a->next;
        b = b->next;
    }
}

U8 cmp_types(Type* a, Type* b) {
    if (a->type != b->type) {
        return 0;
    }
    switch(a->type) {
        case IntType:
            return a->size == b->size;
        case FltType:
            return a->size == b->size;
        case PtrType:
            return cmp_types(a->item, b->item);
        case ArrType:
            return (a->size == b->size) && cmp_types(a->item, b->item);
        case CodeType:
            return cmp_types(a->item, b->item) && cmp_fields(a->field, b->field);
    }
}
