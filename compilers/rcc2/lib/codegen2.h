typedef enum {
    NullBlock, // Since NULL is for reporting errors
    ENABlock, // EvalNode arg
    LinkBlock,
    IfBlock,
    WhileBlock,
    RetBlock
} BlockType;

typedef struct Block {
    BlockType type;
    union {
        EvalNode* en;
        struct {
            struct Block* prev;
            struct Block* last;
        };
        struct {
            EvalNode* cond;
            struct Block* body;
            struct Block* alt_body;
        };
    };
} Block;

typedef struct {
    // U8* name;
    Env* locals; // Chained to global env
    Block* body;
    U32 stack_len;
    U8 has_args;
} Func;

typedef struct {
    ERR (*crt_start)(FILE* out);
    ERR (*asm_start)(FILE* out, U8* name);
    ERR (*asm_emit)(FILE* out, U8* data);
    U16 (*arg_start)(U16 stack_len, U8 argc);
    ERR (*emit_fn)(FILE* out, U8* name, Func* fn);
    ERR (*emit_var)(FILE* out, U8* name, Type* var);
} Target;

Block* block(Env** env, U32* max_stack, U32* stack_len, Tokenizer* tok) {
    if (tok->cur.type == LeftBrace) {
        tokenizer_next(tok);
        Block* cur = new(Block);
        cur->type = NullBlock;
        Env* nenv = *env;
        U32 curlen = *stack_len;
        while (tok->cur.type != RightBrace) {
            Block* ret = block(&nenv, max_stack, &curlen, tok);
            if(!ret) return 0;
            if (ret->type == NullBlock) {
                free(ret);
            } else if (cur->type != NullBlock) {
                Block* tmp = new(Block);
                tmp->type = LinkBlock;
                tmp->prev = cur;
                tmp->last = ret;
                cur = tmp;
            } else {
                free(cur);
                cur = ret;
            }
        }
        if (curlen > *max_stack) *max_stack = curlen;
        tokenizer_next(tok);
        return cur;
    }
    if (tok->cur.type == IfKeywordToken || tok->cur.type == WhileKeywordToken) {
        U8 looping = tok->cur.type == WhileKeywordToken;
        tokenizer_next(tok);
        if (tok->cur.type != LeftParen) {
            tkerr(tok, "Expected (");
            return 0;
        }
        tokenizer_next(tok);

        Block* cur = new(Block);
        cur->type = looping ? WhileBlock : IfBlock;
        cur->cond = expr(tok, env);
        if (!cur->cond) {
            return 0;
        }
        if (tok->cur.type != RightParen) {
            tkerr(tok, "Expected )");
            return 0;
        }
        tokenizer_next(tok);
        cur->body = block(env, max_stack, stack_len, tok);
        if (!cur->body) {
            return 0;
        }
        if (!looping && tok->cur.type == ElseKeywordToken) {
            tokenizer_next(tok);
            cur->alt_body = block(env, max_stack, stack_len, tok);
            if (!cur->alt_body) return 0;
        } else {
            cur->alt_body = 0;
        }
        return cur;
    }
    if (tok->cur.type == ReturnKeywordToken) {
        tokenizer_next(tok);
        EvalNode* node;
        if (tok->cur.type == Semi) {
            node = new(EvalNode);
            node->op = NumberToken;
            node->type = resolve_env(*env, "USZ")->decl;
            node->lvl = Comptime;
            node->number = 0;
        } else {
            node =expr(tok, env);
            if(!node) return 0;
        }
        if (tok->cur.type != Semi) {
            tkerr(tok, "Expected ;");
            return 0;
        }
        tokenizer_next(tok);
        Block* tmp = new(Block);
        tmp->type = RetBlock;
        tmp->en = node;
        return tmp;
    }
    U8 name[NAME_LIMIT+1];
    Type* type;
    if (get_type(env, tok, &type, name) != Ok) return 0;
    if (!type) {
        EvalNode* node = expr(tok, env);
        if(!node) return 0;
        if (tok->cur.type != Semi) {
            tkerr(tok, "Expected ;");
            return 0;
        }
        tokenizer_next(tok);
        Block* tmp = new(Block);
        tmp->type = ENABlock;
        tmp->en = node;
        return tmp;
    }
    Variable* v = new_var(env, name, LocalVar, 1);//TODO remove 1
    if (!v) {
        tkerr(tok, "Double declaration of %s (btw block scoping is WIP)", name);
        return 0;
    }
    v->decl = type;
    v->stack_loc = *stack_len;
    *stack_len += type->size;
    if (tok->cur.type == Semi) {
        tokenizer_next(tok);
    } else if (tok->cur.type == Assign) {
        tokenizer_next(tok);
        /* Create the variable node */
        EvalNode* vn = new(EvalNode);
        vn->op = NameToken;
        vn->lvl = NoSideEffects;
        vn->type = v->decl;
        vn->var = v;
        /* Create the assignment node */
        EvalNode* an = new(EvalNode);
        an->op = Assign;
        an->nodes[0] = vn;
        an->nodes[1] = iif(tok, env);
        if(!an->nodes[1]) return 0;
        if(adjust_node(an) != Ok) {
            tkerr(tok, "Type mismatch ;( [OP=%02X]", an->op);
            return 0;
        }
        if (tok->cur.type != Semi) {
            tkerr(tok, "Expected ;");
            return 0;
        }
        tokenizer_next(tok);
        Block* tmp = new(Block);
        tmp->type = ENABlock;
        tmp->en = an;
        return tmp;
    } else {
        tkerr(tok, "Expected ; or =");
        return 0;
    }

    Block* tmp = new(Block);
    tmp->type = NullBlock;
    return tmp;
}
ERR holyc_parody(FILE* out, Target* tgt, Tokenizer* tok) {
    Env* env = global_env();
    tgt->crt_start(out);

    U8 name[NAME_LIMIT+1];
    Type* type;

    while (tok->cur.type != EOFToken) {
        if (tok->cur.type == TypedefKeywordToken) {
            tokenizer_next(tok);

            if(get_type(&env, tok, &type, name) != Ok) return Error;
            if (!type) {
                tkerr(tok, "no type provided for typedef");
                return Error;
            }
            if (tok->cur.type != Semi) {
                tkerr(tok, "Expected ;");
                return Error;
            }
            tokenizer_next(tok);

            Variable* v = new_var(&env, name, TypeVar, 0);
            v->decl = type;
            continue;
        }
        if (get_type(&env, tok, &type, name) != Ok) return Error;
        if (!type) {
            tkerr(tok, "no type");
            return Error;
        }
        if (type->type == CodeType) {
            // Function

            Variable* v = new_var(&env, name, GlobalVar, 0);
            v->decl = type;
            switch(tok->cur.type) {
                case LeftBrace: {
                    /* Arguments */
                    Env* locals = env;
                    U8 argc = 0;
                    for(Field* cur = type->field; cur; cur = cur->next) {
                        Variable* v = new_var(&locals, cur->name, LocalVar, 0);
                        if (!v) {
                            tkerr(tok, "Double declaration of argument %s", cur->name);
                            return 0;
                        }
                        v->decl = cur->type;
                        argc++;
                    }
                    Env* arg_start = locals;

                    /* Initialize struct */
                    Func fn;
                    fn.has_args = !!type->field;
                    fn.stack_len = 0;
                    U32 curlen = 0;

                    fn.body = block(&locals, &fn.stack_len, &curlen, tok);
                    if(!fn.body) return Error;
                    /* Fix argument location */
                    U16 stack_len = tgt->arg_start(fn.stack_len, argc);
                    for (; arg_start != env; arg_start = arg_start->next) {
                        arg_start->var.stack_loc = stack_len;
                        stack_len -= TGTWORD;
                    }
                    /* do it™ */
                    if(tgt->emit_fn(out, name, &fn) != Ok) return Error;
                    break;
                }
                case Semi:
                    tokenizer_next(tok);
                    break;
                case Assign: {
                    tgt->asm_start(out, name);
                    tokenizer_next(tok);
                    U8 flag = 1;
                    while (flag) {
                        switch (tok->cur.type) {
                            case StrToken:
                                tgt->asm_emit(out, tok->cur.ptr);
                                tokenizer_next(tok);
                                break;
                            case Semi:
                                flag = 0;
                                tokenizer_next(tok);
                                break;
                            default:
                                tkerr(tok, "In ASM functions, only strings are supported");
                                return Error;
                        }
                    }
                    break;
                }
                default:
                    tkerr(tok, "Expected ; or {");
                    return Error;
            }

        } else {
            if (tok->cur.type != Semi) {
                tkerr(tok, "Expected ;");
                return Error;
            }
            tokenizer_next(tok);
            tgt->emit_var(out, name, type);

            Variable* v = new_var(&env, name, GlobalVar, 0);
            if (!v) {
                tkerr(tok, "Double declaration of %s", name);
                return 0;
            }
            v->decl = type;
        }
    }
    return Ok;
}
