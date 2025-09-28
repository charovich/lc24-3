typedef enum {
    WithSideEffects,
    NoSideEffects,
    Comptime
} EvalLevel;
typedef struct EvalNode {
    TokenType op;
    // Alternative ( x ^ 0x80 ) opcodes:
    // * => Deref
    // & => Ref
    // + => NOT USED for now...
    // - => Negate

    // ; (No xor 0x80) => To float
    // / => To int (Like // in python)

    EvalLevel lvl;
    Type* type;
    union {
        struct EvalNode* nodes[3];
        U32 number;
        U8* str_name;
        Variable* var;
    };
} EvalNode;
U0 free_en(EvalNode* en) {
    switch (en->op) {
        case IfKeywordToken:
        case NameToken:
        case NumberToken:
            return;
        case StrToken:
            free(en->str_name);
            return;
        default:
            free_en(en->nodes[0]);
            free_en(en->nodes[1]);
    }
}
// Mekesoft™ Access Control (Programming Langwuge Edition)
// __S PMFI
//   | |||\Int(+Int)
//   | ||\Float(+Float)
//   | |\Int+Float
//   | \Ptr(+Int)
//   \ Ptr+Ptr
U8 node_rules[] = {
    0x00, // EOFToken
    0x00, // NameToken
    0x00, // NumberToken
    0x00, // StrToken
    0x0B, // PlusPlus
    0x0B, // MinusMinus
    0x00, // LeftParen
    0x00, // RightParen
    0x00, // LeftBracket
    0x00, // RightBracket
    0x00, // Dot
    0x00, // Arrow
    0x0B, // Not
    0x01, // Tilde
    0x07, // Star [As in mul]
    0x07, // Div
    0x07, // Mod
    0x0F, // Plus
    0x1F, // Minus
    0x05, // LeftShift
    0x05,// RightShift
    0x07, // Less
    0x07, // LessEqual
    0x07, // Greater
    0x07, // GreaterEqual
    0x07, // Equal
    0x07, // NotEqual
    0x09, // And
    0x01, // Caret
    0x01, // Or
    0x0F, // AndAnd
    0x0F, // OrOr
    0x00, // Question
    0x00, // Colon
    0x00, // Assign
    0x07, // StarAssign
    0x07, // DivAssign
    0x07, // ModAssign
    0x0F, // PlusAssign
    0x0F, // MinusAssign
    0x07, // LeftShiftAssign
    0x07, // RightShiftAssign
    0x01, // AndAssign
    0x01, // XorAssign
    0x01, // OrAssign
    0x00, // Comma
    0x00, // Semi
    0x00, // LeftBrace
    0x00, // RightBrace
};
U0 move_node(EvalNode* tgt, EvalNode* src) {
    memcpy(tgt, src, sizeof(EvalNode));
    free(src);
}

EvalNode* conv_type(EvalNode* src, Type* tgt) {
    if (tgt && src->type->type == tgt->type) {
        if (tgt->type == PtrType) {
            // hacky
            src->type = tgt;
        }
        return src;
    }
    switch (src->type->type) {
        case IntType:
            if (!tgt) return src;
            switch (tgt->type) {
                case FltType: {
                    EvalNode* flot = new(EvalNode);
                    flot->op = Semi;
                    flot->type = tgt;
                    flot->nodes[0] = src;
                    return flot;
                }
                case PtrType: {
                    src->type = tgt; // hack 2.0
                    return src;
                }
            }
            return 0;
        case FltType:
            if (!tgt) return src;
            switch (tgt->type) {
                case IntType: {
                    EvalNode* floor = new(EvalNode);
                    floor->op = Div ^ 0x80;
                    floor->type = tgt;
                    floor->nodes[0] = src;
                    return floor;
                }
            }
            return 0;
        case PtrType:
            if (!tgt) return src;
            if (tgt->type == IntType) return src;
            return 0;
        case ArrType:
        case FieldType:
        case CodeType: {
            if (!tgt || tgt->type == PtrType) {
                /* Swap to pointer */
                Type* new_t = new(Type);
                new_t->type = PtrType;
                new_t->size = TGTWORD;
                new_t->item = tgt->type == ArrType ? src->type->item : src->type;

                EvalNode* referrer = new(EvalNode);
                referrer->op = And ^ 0x80;
                referrer->type = new_t;
                referrer->nodes[0] = src;
                return referrer;
            }
        }
        default:
            return 0;
    }
}

ERR adjust_node(EvalNode* o) {
    // specials
    U8 node_cnt = 2;
    switch (o->op) {
        default: {
            // Get nodes
            TypeType x = o->nodes[0]->type->type;
            TypeType y = o->nodes[1]->type->type;
            if (x >= FieldType) {
                o->nodes[0] = conv_type(o->nodes[0], 0);
                x = o->nodes[0]->type->type;
            }
            if (y >= FieldType) {
                o->nodes[1] = conv_type(o->nodes[1], 0);
                y = o->nodes[1]->type->type;
            }
            U8 swp = 0;

            if (x > y) {TypeType zov = y; y = x; x = zov; swp = 1;}

            switch (x) {
                case IntType:
                    switch (y) {
                        case IntType:
                            if ((node_rules[o->op] & 1) == 0) return Error;
                            o->type = o->nodes[0]->type;
                            break;
                        case FltType:
                            if ((node_rules[o->op] & 4) == 0) return Error;
                            EvalNode* flot = new(EvalNode);
                            flot->op = Semi;
                            flot->type = o->nodes[1-swp]->type;
                            flot->nodes[0] = o->nodes[swp];
                            o->nodes[swp] = flot;
                            o->type = o->nodes[1-swp]->type;
                            break;
                        case PtrType:
                            if ((node_rules[o->op] & 8) == 0) return Error;
                            /* Apply GovnConv™ */
                            U32 sz = o->nodes[1-swp]->type->item->size;
                            if (sz != 1) {
                                EvalNode* szrepr = new(EvalNode);
                                szrepr->op = NumberToken;
                                szrepr->number = sz;
                                szrepr->type = o->nodes[swp]->type; // Ростипнадзор запретил resolve_env
                                szrepr->lvl = Comptime;

                                EvalNode* md = new(EvalNode);
                                md->op = Star;
                                md->type = o->nodes[swp]->type;
                                md->lvl = o->nodes[swp]->lvl;
                                md->nodes[0] = o->nodes[swp];
                                md->nodes[1] = szrepr;
                                /* if(adjust_node(md) != Ok) {
                                    fprintf(stderr, "GovnCheck™ found an error with recursion!");
                                    return Error;
                                } // DOOM on GovnCheck™ soon™ */
                                o->nodes[swp] = md;
                            }
                            o->type = o->nodes[1-swp]->type;
                            break;
                        default:
                            return Error;
                    }
                    break;
                case FltType:
                    switch (y) {
                        case FltType:
                            if ((node_rules[o->op] & 2) == 0) return Error;
                            o->type = o->nodes[0]->type;
                            break;
                        default:
                            return Error;
                    }
                    break;
                case PtrType:
                    switch (y) {
                        case PtrType:
                            if ((node_rules[o->op] & 16) == 0) return Error;
                            o->type = o->nodes[0]->type;
                            break;
                        default:
                            return Error;
                    }
                    break;
                default:
                    return Error; // Cant do shit anymore
            }
            break;
        }
        case Assign:
            o->lvl = WithSideEffects;
            /* Fix stuff */
            EvalNode* nt = conv_type(o->nodes[1], o->nodes[0]->type);
            if (!nt) return Error;
            o->nodes[1] = nt;
            o->type = nt->type; // pofig?
            return Ok;

        case LeftParen: // pohren
            o->lvl = WithSideEffects;
            return Ok;

        case Question:
            if (o->nodes[0]->lvl == Comptime) {
                // Comptime - В С Ё
            }
            if (o->nodes[1]->type != o->nodes[2]->type) {
                //return Error; // type mismatch
            }
            o->type = o->nodes[1]->type;
            node_cnt = 3;
            break;
        case Comma:
            if (o->nodes[0]->lvl >= NoSideEffects) {
                free_en(o->nodes[0]);
                move_node(o, o->nodes[1]);
                return Ok;
            }
    }
    EvalLevel lvl = o->op == Assign ? WithSideEffects : Comptime ;
    for (U8 i = 0; i < node_cnt; i++) {
        if (o->nodes[i] && o->nodes[i]->lvl < lvl) {
            lvl = o->nodes[i]->lvl;
        }
    }
    // Comptime В С Ё
    o->lvl = lvl;
    return Ok;
}

U8* type_str(Type* type) {
    switch (type->type) {
        case IntType: return "Int";
        case FltType: return "Flt";
        case PtrType: return "Ptr";
        case ArrType: return "Arr";
        case CodeType: return "Fun";
    }
}
U0 typechk_err(Tokenizer* tok, EvalNode* en) {
    Token t;
    t.type = en->op & 0x7F;
    tkerr(tok, "Type check failed on node %s%s(%s, %s)", token_name(&t), en->op & 0x80 ? "(alt)" : "", type_str(en->nodes[0]->type), type_str(en->nodes[1]->type));
}
U0 print_en(EvalNode*en);
