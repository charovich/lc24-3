
U8* env_namespaces[] = {"struct::", "union::", "enum::"};


Env* global_env() {
    Env* a = 0;
    Variable* v;
    // Builtin namespace
    for (U8 i = 0; i < 3; i++) {
        v = new_var(&a, env_namespaces[i], EnvVar, 0);
        v->decl = 0;
    }


    // Variable* v = new_var(&a, "U0", TypeVar, 0);
    // Type* decl = new(Type);
    // decl.type = IntType;
    // decl.size = 0;
    // v.decl = decl;

    v = new_var(&a, "U8", TypeVar, 0);
    Type* TU8 = new(Type);
    TU8->type = IntType;
    TU8->size = 1;
    v->decl = TU8;

    v = new_var(&a, "U16", TypeVar, 0);
    Type* decl = new(Type);
    decl->type = IntType;
    decl->size = 2;
    v->decl = decl;

    v = new_var(&a, "U24", TypeVar, 0);
    Type* TUSZ = new(Type);
    TUSZ->type = IntType;
    TUSZ->size = 3;
    v->decl = TUSZ;

    v = new_var(&a, "USZ", TypeVar, 0);
    v->decl = TUSZ;

    v = new_var(&a, "F24", TypeVar, 0);
    decl = new(Type);
    decl->type = FltType;
    decl->size = 3;
    v->decl = decl;

    v = new_var(&a, "<string literal>", TypeVar, 0);
    decl = new(Type);
    decl->type = PtrType;
    decl->item = TU8;
    v->decl = decl;

    return a;
}

typedef struct {
    TypeType type;
    union {
        U32 size;
        Field* args;
    };
} TypeMachineEntry;

ERR stat_zvezdoy(U8 i, TypeMachineEntry* data, U8* log_pc) {
    while (i--) {
        if (*log_pc == 16) {
            return Error;
        }
        data[*log_pc].type = PtrType;
        (*log_pc)++;
    }
    return Ok;
}
ERR get_type(Env** env, Tokenizer* tok, Type** out_type, U8* out_nbuf);

ERR parkovka(Env** env, Tokenizer* tok, Type** out_type) {
    *out_type = 0;
    switch (tok->cur.type) {
        case StructKeywordToken:
        case UnionKeywordToken:
        case EnumKeywordToken: {
            TokenType sue_me = tok->cur.type;
            tokenizer_next(tok);
            Env** ns = (Env**)&(resolve_env(*env, env_namespaces[sue_me - StructKeywordToken])->decl);
            U8 new_name[NAME_LIMIT+1];
            new_name[0] = 0;

            if (tok->cur.type == NameToken) {
                Variable* var = resolve_env(*ns, tok->cur.str);
                if (var && var->type == TypeVar) {
                    tokenizer_next(tok);
                    *out_type = var->decl;
                    return Ok;
                }
                strcpy(new_name, tok->cur.str);
                tokenizer_next(tok);
            }
            if (tok->cur.type != LeftBrace) {
                tkerr(tok, "Expected {");
                return Error;
            }
            tokenizer_next(tok);
            if (sue_me == EnumKeywordToken) {
                U32 curval = 0;
                *out_type = resolve_env(*env, "U24")->decl;
                while (tok->cur.type != RightBrace) {
                    if (tok->cur.type != NameToken) {
                        tkerr(tok, "Bad name in enum");
                        return Error;
                    }
                    Variable* item = new_var(env, tok->cur.str, LiteralVar, 0);
                    item->decl = *out_type;
                    tokenizer_next(tok);
                    if (tok->cur.type == Assign) {
                        tokenizer_next(tok);
                        if (tok->cur.type != NumberToken) {
                            tkerr(tok, "Expected number");
                            return Error;
                        }
                        curval = tok->cur.number;
                        tokenizer_next(tok);
                    }
                    item->literal = curval++;
                    if (tok->cur.type == RightBrace) break;
                    if (tok->cur.type != Comma) {
                        tkerr(tok, "Expected , or }");
                        return Error;
                    }
                    tokenizer_next(tok);
                }
                tokenizer_next(tok);
            } else {
                U32 cursize = 0;
                Field* next;
                Field** tail = &next;
                while (tok->cur.type != RightBrace) {
                    Type* entry;
                    U8 name[NAME_LIMIT + 1];
                    if (get_type(env, tok, &entry, name) != Ok) return Error;
                    if (!entry || !name[0]) {
                        tkerr(tok, "Invalid entry definition");
                        return Error;
                    }
                    Field* new_fld = new(Field);
                    new_fld->name = strdup(name);
                    new_fld->offset = cursize;
                    new_fld->type = entry;
                    *tail = new_fld;
                    tail = &(new_fld->next);
                    if (sue_me == StructKeywordToken) cursize += entry->size;
                    else if (entry->size < cursize) cursize = entry->size;
                    if (tok->cur.type == RightBrace) break;
                    if (tok->cur.type != Semi) {
                        tkerr(tok, "Expected ; or }");
                        return Error;
                    }
                    tokenizer_next(tok);
                }
                tokenizer_next(tok);
                *out_type = new(Type);
                (*out_type)->type = FieldType;
                (*out_type)->size = cursize;
                (*out_type)->field = next;
            }
            if (new_name[0]) {
                Variable* v = new_var(ns, new_name, TypeVar, 0);
                v->decl = *out_type;
            }
            return Ok;
        }
        case NameToken: {
            Variable* var = resolve_env(*env, tok->cur.str);
            if (!var || var->type != TypeVar) {
                return Ok;
            }
            tokenizer_next(tok);
            *out_type = var->decl;
            return Ok;
        }
        default:
            return Ok;
    }
}
ERR get_type(Env** env, Tokenizer* tok, Type** out_type, U8* out_nbuf) {
    /* Initial */
    out_nbuf && (*out_nbuf = 0);
    Variable* var;
    Type* src;
    if (parkovka(env, tok, &src) != Ok) return Error;
    if (!src) { *out_type = 0; return Ok; }

    TypeMachineEntry log[16];
    U8 log_pc = 0;

    U8 ukaz[8]; // zvezdochki
    ukaz[0] = 0;
    U8 ukazi = 0;

    U8 post = 0;

    while (1) {
        switch (tok->cur.type) {
            case Star:
                if (post) {
                    tkerr(tok, "Bad postfix operator: *");
                    return Error;
                }
                ukaz[ukazi] += 1;
                tokenizer_next(tok);
                break;
            case NameToken: {
                if (post) {
                    tkerr(tok, "Bad postfix operator: name");
                    return Error;
                }
                if (!out_nbuf) {
                    tkerr(tok, "Unable to give it a name");
                    return Error;
                }
                strcpy(out_nbuf, tok->cur.str);
                post = 1;
                tokenizer_next(tok);
                break;
            }
            case LeftBracket: {
                post = 1;
                tokenizer_next(tok);
                if (tok->cur.type != NumberToken) {
                    tkerr(tok, "Expected number");
                    return Error;
                }
                if (log_pc == 16) {
                    tkerr(tok, "Type is too complex");
                    return Error;
                }
                log[log_pc].type = ArrType;
                log[log_pc].size = tok->cur.number;
                log_pc++;
                tokenizer_next(tok);
                if (tok->cur.type != RightBracket) {
                    tkerr(tok, "Expected ]");
                    return Error;
                }
                tokenizer_next(tok);
                break;
            }
            case LeftParen: {
                tokenizer_next(tok);
                if (tok->cur.type == RightParen) {
                    /* Empty function! */
                    tokenizer_next(tok);
                    if (log_pc == 16) {
                        tkerr(tok, "Type is too complex");
                        return Error;
                    }
                    log[log_pc].type = CodeType;
                    log[log_pc].args = 0;
                    log_pc++;
                    break;
                }
                Type* farg;
                U8 param_name[NAME_LIMIT+1];
                if(get_type(env, tok, &farg, param_name)!=Ok) {
                    return Error;
                }
                if (farg) {
                    U8 offset = 0;

                    Field* fid = new(Field);
                    fid->name = strdup(param_name);
                    fid->type = farg;
                    fid->offset = offset * TGTWORD;
                    fid->next = 0;

                    if (log_pc == 16) {
                        tkerr(tok, "Type is too complex");
                        return Error;
                    }
                    log[log_pc].type = CodeType;
                    log[log_pc].args = fid;
                    log_pc++;

                    while (tok->cur.type != RightParen) {
                        if (!fid) {
                            tkerr(tok, "Can't accept arguments after ...");
                            return Error;
                        }
                        if (tok->cur.type != Comma) {
                            tkerr(tok, "Expected , or )");
                            return Error;
                        }
                        tokenizer_next(tok);
                        if (tok->cur.type == TripleDot) {
                            tokenizer_next(tok);
                            Field* tmp = new(Field);
                            tmp->type = 0;
                            tmp->name = 0;
                            tmp->offset = offset ++* TGTWORD;
                            tmp->next = 0;

                            fid->next = tmp;
                            fid = 0;
                            continue;
                        }

                        if(get_type(env, tok, &farg, param_name)!=Ok) {
                            return Error;
                        }
                        if(!farg) {
                            tkerr(tok, "Expected type");
                            return Error;
                        }
                        Field* tmp = new(Field);
                        tmp->name = param_name[0] ? strdup(param_name) : 0;
                        tmp->type = farg;
                        tmp->offset = offset ++* TGTWORD;
                        tmp->next = 0;
                        fid->next = tmp;
                        fid = tmp;
                    }
                    tokenizer_next(tok);
                    break;
                }
                if (post == 1) {
                    tkerr(tok, "Type not recognized");
                    return Error;
                }
                if (ukazi == 7) {
                    tkerr(tok, "Too many parens");
                    return Error;
                }
                ukazi += 1;
                ukaz[ukazi] = 0;
                break;
            }
            case RightParen: {
                if (ukazi) {
                    if(stat_zvezdoy(ukaz[ukazi], log, &log_pc)!=Ok) {
                        tkerr(tok, "Too many starts");
                        return Error;
                    }
                    ukazi -= 1;
                    break;
                }
                /* Underflow is not checked because it would prevent
                 * manual type conversion: &(U32*)0
                 */
            }
            default: {
                if (ukazi != 0) {
                    tkerr(tok, "Unclosed paren(s)");
                    return Error;
                }
                if(stat_zvezdoy(ukaz[ukazi], log, &log_pc)!=Ok) {
                    tkerr(tok, "Too many stars");
                    return Error;
                }

                // Now reverse!
                Type* tmp;
                while(log_pc--) {
                    switch(log[log_pc].type) {
                        case ArrType: {
                            if (src->type == CodeType) {
                                tkerr(tok, "Please wrap the function in a pointer yourself...");
                                return Error;
                            }
                            tmp = new(Type);
                            tmp->type = ArrType;
                            tmp->size = src->size * log[log_pc].size;
                            tmp->item = src;
                            src = tmp;
                            break;
                        }
                        case PtrType: {
                            tmp = new(Type);
                            tmp->type = PtrType;
                            tmp->size = TGTWORD;
                            tmp->item = src;
                            src = tmp;
                            break;
                        }
                        case CodeType: {
                            tmp = new(Type);
                            tmp->type = CodeType;
                            tmp->size = 0x7FFFFFFF;
                            tmp->item = src;
                            tmp->field = log[log_pc].args;
                            src = tmp;
                            break;
                        }
                        default:
                            break;
                    }
                }
                *out_type = src;
                return Ok;
            }

        }
    }
}
