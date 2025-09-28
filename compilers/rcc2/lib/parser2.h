EvalNode* expr(Tokenizer* tok, Env** env);
EvalNode* assignment(Tokenizer* tok, Env** env);
EvalNode* suffix(Tokenizer* tok, Env** env);

EvalNode* value(Tokenizer* tok, Env** env) {
    switch (tok->cur.type) {
        case NumberToken: {
            EvalNode* node = new(EvalNode);
            node->op = NumberToken;
            node->type = resolve_env(*env, "USZ")->decl;
            node->lvl = Comptime;
            node->number = tok->cur.number;
            tokenizer_next(tok);
            return node;
        }
        case StrToken: {
            EvalNode* node = new(EvalNode);
            node->op = StrToken;
            node->type = resolve_env(*env, "<string literal>")->decl;
            node->lvl = Comptime;
            node->str_name = tok->cur.ptr;
            tokenizer_next(tok);
            return node;
        }
        case LeftParen: {
            tokenizer_next(tok);
            Type* type;
            if (get_type(env, tok, &type, 0) != Ok) return 0;
            if (type) {
                if (tok->cur.type != RightParen) {
                    tkerr(tok, "Expected )");
                    return 0;
                }
                tokenizer_next(tok);
                EvalNode* node = suffix(tok, env);
                if (!node) return 0;
                node = conv_type(node, type);
                if (!node) {
                    tkerr(tok, "Type conversion failed");
                    return 0;
                }
                return node;
            }
            EvalNode* param = expr(tok, env);
            if(!param) {
                return 0;
            }
            if (tok->cur.type != RightParen) {
                tkerr(tok, "Expected )");
                return 0;
            }
            tokenizer_next(tok);
            return param;
        }
        case NameToken: {
            Variable* var = resolve_env(*env, tok->cur.str);
            /*
            if (!var) {
                tkerr(tok, "Can't find %s", tok->cur.str);
                return 0;
            }
            */
            switch (var->type) {
                case LiteralVar: {
                    EvalNode* node = new(EvalNode);
                    node->op = NumberToken;
                    node->lvl = Comptime;
                    node->type = var->decl;
                    node->number = var->literal;
                    tokenizer_next(tok);
                    return node;
                }
                case GlobalVar:
                case LocalVar: {
                    EvalNode* node = new(EvalNode);
                    node->op = NameToken;
                    node->lvl = NoSideEffects;
                    node->type = var->decl;
                    node->var = var;
                    tokenizer_next(tok);
                    return node;
                }
                default:
                    tkerr(tok, "Can't use %s", tok->cur.str);
                    return 0;
            }
        }
        default:
            tkerr(tok, "Unexpected %s", token_name(&(tok->cur)));
            return 0;
    }
}
EvalNode* fn_args(Tokenizer* tok, Env** env, Field* field) {
    EvalNode* cur = assignment(tok, env);
    if(!cur) return 0;
    cur = conv_type(cur, field->type);
    if (!cur) {
        tkerr(tok, "Type conversion failed on argument %s", field->name || "?");
        return 0;
    }
    field = field->next;
    while (tok->cur.type == Comma) {
        if (!field) {
            tkerr(tok, "Too many args");
            return 0;
        }
        EvalNode* tmp = new(EvalNode);
        tmp->op = RightParen;
        tokenizer_next(tok);
        tmp->nodes[0] = cur;
        tmp->nodes[1] = assignment(tok, env);
        if(!tmp->nodes[1]) return 0;
        if (field->type) {
            tmp->nodes[1] = conv_type(tmp->nodes[1], field->type);
            if(!tmp->nodes[1]) {
                tkerr(tok, "Type conversion failed on argument %s", field->name || "?");
                return 0;
            }
            field = field->next;
        }
        /*if(adjust_node(tmp) != Ok) {
            typechk_err(tok, tmp);
            return 0;
        } Нам этот ваш тупочекинг нафиг не нужен!*/
        cur = tmp;
    }
    return cur;
}
EvalNode* suffix(Tokenizer* tok, Env** env) {
    switch (tok->cur.type) {
        case Star: {
            tokenizer_next(tok);
            EvalNode* tmp = new(EvalNode);
            tmp->op = Star ^ 0x80;
            tmp->nodes[0] = suffix(tok, env);
            if (tmp->nodes[0] == 0) {
                return 0;
            }
            if (tmp->nodes[0]->type->type != PtrType) {
                tkerr(tok, "Not a pointer, is %d");
                return 0;
            }
            tmp->type = tmp->nodes[0]->type->item;
            tmp->lvl = tmp->nodes[0]->lvl >= NoSideEffects ? NoSideEffects : WithSideEffects;
            // Готово!
            return tmp;

        }
        case And: {
            tokenizer_next(tok);
            EvalNode* tmp = new(EvalNode);
            tmp->op = And ^ 0x80;
            tmp->nodes[0] = suffix(tok, env);
            if (tmp->nodes[0] == 0) {
                return 0;
            }
            /* Make up a new type */
            Type* new_t = new(Type);
            new_t->type = PtrType;
            new_t->size = TGTWORD;
            new_t->item = tmp->nodes[0]->type;
            tmp->type = new_t;
            tmp->lvl = tmp->nodes[0]->lvl >= NoSideEffects ? NoSideEffects : WithSideEffects;
            // Готово!
            return tmp;

        }
        case Minus: {
            tokenizer_next(tok);
            EvalNode* tmp = new(EvalNode);
            tmp->op = Minus ^ 0x80;
            tmp->nodes[0] = suffix(tok, env);
            if (tmp->nodes[0] == 0) {
                return 0;
            }
            switch (tmp->nodes[0]->type->type) {
                case IntType:
                case FltType:
                    break;
                default:
                    tkerr(tok, "Can't negate");
                    return 0;
            }
            tmp->type = tmp->nodes[0]->type;
            tmp->lvl = tmp->nodes[0]->lvl;
            // Готово!
            return tmp;
        }
        case MinusMinus:
        case PlusPlus: {
            EvalNode* tmp = new(EvalNode);
            tmp->op = tok->cur.type;
            tokenizer_next(tok);
            tmp->nodes[0] = suffix(tok, env);
            if (tmp->nodes[0] == 0) {
                return 0;
            }
            switch (tmp->nodes[0]->type->type) {
                case IntType:
                case FltType:
                case PtrType:
                    break;
                default:
                    tkerr(tok, "Can't increment");
                    return 0;
            }
            tmp->type = tmp->nodes[0]->type;
            tmp->lvl = WithSideEffects;
            // Готово!
            return tmp;
        }
        case Not: {
            tokenizer_next(tok);
            EvalNode* tmp = new(EvalNode);
            tmp->op = Not;
            tmp->nodes[0] = suffix(tok, env);
            if (tmp->nodes[0] == 0) {
                return 0;
            }
            switch (tmp->nodes[0]->type->type) {
                case IntType:
                case FltType:
                case PtrType:
                    break;
                default:
                    tkerr(tok, "Can't negate");
                    return 0;
            }
            tmp->type = tmp->nodes[0]->type;
            tmp->lvl = WithSideEffects;
            // Готово!
            return tmp;
        }

    }
    EvalNode* cur = value(tok, env);
    if(!cur) return 0;
    while (1) {
        switch (tok->cur.type) {
            case LeftParen: {
                tokenizer_next(tok);
                EvalNode* tmp = new(EvalNode);
                tmp->op = LeftParen;
                tmp->nodes[0] = cur;
                if (cur->type->type != CodeType) {
                    tkerr(tok, "Attempt to call NaF");
                    return 0;
                }
                if (tok->cur.type == RightParen) {
                    if (cur->type->field) {
                        tkerr(tok, "No arguments provided");
                        return 0;
                    }
                    tmp->nodes[1] = 0;
                } else {
                    if (!cur->type->field) {
                        tkerr(tok, "Function does not accept arguments");
                        return 0;
                    }
                    tmp->nodes[1] = fn_args(tok, env, cur->type->field);
                    if(!tmp->nodes[1]) return 0;
                }
                if (tok->cur.type != RightParen) {
                    tkerr(tok, "Function call not closed");
                    return 0;
                }
                tokenizer_next(tok);
                tmp->type = tmp->nodes[0]->type->item;

                if(adjust_node(tmp) != Ok) {
                    typechk_err(tok, tmp);
                    return 0;
                }
                cur = tmp;
                break;
            }
            case LeftBracket: {
                tokenizer_next(tok);
                EvalNode* tmp = new(EvalNode);
                tmp->op = Plus;
                tmp->nodes[0] = cur;
                tmp->nodes[1] = expr(tok, env);
                if (!tmp->nodes[1]) return 0;
                if (tok->cur.type != RightBracket) {
                    tkerr(tok, "Array index not closed");
                    return 0;
                }
                tokenizer_next(tok);

                if(adjust_node(tmp) != Ok) {
                    typechk_err(tok, tmp);
                    return 0;
                }
                if (tmp->type->type != PtrType) {
                    tkerr(tok, "Not a pointer");
                    return 0;
                }
                cur = new(EvalNode);
                cur->op = Star ^ 0x80;
                cur->nodes[0] = tmp;
                cur->type = tmp->type->item;
                cur->lvl = tmp->lvl >= NoSideEffects ? NoSideEffects : WithSideEffects;
                // Pointer is OK
                break;
            }
            case Dot: {
                EvalNode* tmp = new(EvalNode);
                tmp->op = And ^ 0x80;
                tmp->nodes[0] = cur;
                /* Make up a new type */
                Type* new_t = new(Type);
                new_t->type = PtrType;
                new_t->size = TGTWORD;
                new_t->item = tmp->nodes[0]->type;
                tmp->type = new_t;
                tmp->lvl = tmp->nodes[0]->lvl >= NoSideEffects ? NoSideEffects : WithSideEffects;
                cur = tmp;
                // tokenizer_next(tok);
                // fall down
            }
            case Arrow: {
                tokenizer_next(tok);
                if (cur->type->type!=PtrType) {
                    tkerr(tok, "Not a pointer");
                    return 0;
                }
                if (cur->type->item->type!=FieldType) {
                    tkerr(tok, "Not a pointer to struct/union");
                    return 0;
                }

                if (tok->cur.type!=NameToken) {
                    tkerr(tok, "Expected name");
                    return 0;
                }
                // find it
                Field* fld = (Field*)cur->type->item->field;
                while (fld) {
                    if (!strcmp(fld->name, tok->cur.str)) break;
                    fld = fld->next;
                }
                if (!fld) {
                    tkerr(tok, "Field %s not found", tok->cur.str);
                    return 0;
                }
                tokenizer_next(tok);
                EvalNode* sen = new(EvalNode);
                sen->op = Star ^ 0x80;
                sen->type = fld->type;
                if (fld->offset) {
                    EvalNode* oen = new(EvalNode);
                    oen->op = NumberToken;
                    oen->type = resolve_env(*env, "USZ")->decl;
                    oen->number = fld->offset;
                    // oen->lvl = Comptime;

                    EvalNode* aen = new(EvalNode);
                    aen->op = Plus;
                    aen->nodes[0] = cur;
                    aen->nodes[1] = oen;
                    sen->nodes[0] = aen;
                } else {
                    sen->nodes[0] = cur;
                }
                cur = sen;
                break;
            }

            case PlusPlus:
            case MinusMinus: {
                EvalNode* tmp = new(EvalNode);
                tmp->op = tok->cur.type ^ 0x80;
                tokenizer_next(tok);
                switch (cur->type->type) {
                    case IntType:
                    //case FltType:
                    case PtrType:
                        break;
                    default:
                        tkerr(tok, "Can't increment");
                        return 0;
                }
                tmp->nodes[0] = cur;
                tmp->type = cur->type;
                tmp->lvl = WithSideEffects;
                cur = tmp;
                break;
            }
            default:
                return cur;
        }
    }
}
U8 infix_data[] = {
    EOFToken, // Star, // 0E
    EOFToken, // Div, // 0F
    EOFToken, // Mod, // 10
    Mod, // Plus, // 11
    Mod, // Minus, // 12
    Minus, // LeftShift, // 13
    Minus, // RightShift, // 14
    RightShift, // Less, // 15
    RightShift, // LessEqual, // 16
    RightShift, // Greater, // 17
    RightShift, // GreaterEqual, // 18
    GreaterEqual, // Equal, // 19
    GreaterEqual, // NotEqual, // 1A
    NotEqual, // And, // 1B
    And, // Caret, // 1C
    Caret, // Or, // 1D
    Or, //  AndAnd, // 1E
    AndAnd // OrOr, // 1F
};

EvalNode* infix(Tokenizer* tok, Env** env, TokenType tceil) {
    Token tmp;
    tmp.type = tceil;
    EvalNode* cur = suffix(tok, env);
    if(!cur) return 0;
    while (tok->cur.type >= Star && tok->cur.type <= tceil) {
        EvalNode* tmp = new(EvalNode);
        tmp->op = tok->cur.type;
        tokenizer_next(tok);
        tmp->nodes[0] = cur;
        TokenType nceil = infix_data[tmp->op - Star];
        tmp->nodes[1] = nceil ? infix(tok, env, nceil) : suffix(tok, env);
        if(!tmp->nodes[1]) return 0;
        if(adjust_node(tmp) != Ok) {
            typechk_err(tok, tmp);
            return 0;
        }
        cur = tmp;
    }
    return cur;
}

EvalNode* iif(Tokenizer* tok, Env** env) {
    EvalNode* cond = infix(tok, env, OrOr);
    if (tok->cur.type != Question) return cond;

    EvalNode* tmp = new(EvalNode);
    tmp->op = tok->cur.type;
    tokenizer_next(tok);
    tmp->nodes[0] = cond;
    tmp->nodes[1] = iif(tok, env);
    if(!tmp->nodes[1]) return 0;
    if (tok->cur.type != Colon) {
        tkerr(tok, "Expected :");
        return 0;
    }
    tokenizer_next(tok);
    tmp->nodes[2] = iif(tok, env);
    if(!tmp->nodes[2]) return 0;
    if(adjust_node(tmp) != Ok) {
        typechk_err(tok, tmp);
        return 0;
    }
    return tmp;
}

EvalNode* assignment(Tokenizer* tok, Env** env) {
    EvalNode* cur = iif(tok, env);
    if(!cur) return 0;
    if (tok->cur.type != Assign) return cur;
    EvalNode* tmp = new(EvalNode);
    tmp->op = tok->cur.type;
    tokenizer_next(tok);
    tmp->nodes[0] = cur;
    tmp->nodes[1] = assignment(tok, env);
    if(!tmp->nodes[1]) return 0;
    if(adjust_node(tmp) != Ok) {
        typechk_err(tok, tmp);
        return 0;
    }
    return tmp;
}

EvalNode* expr(Tokenizer* tok, Env** env) {
    EvalNode* cur = assignment(tok, env);
    if(!cur) return 0;
    while (tok->cur.type == Comma) {
        EvalNode* tmp = new(EvalNode);
        tmp->op = tok->cur.type;
        tokenizer_next(tok);
        tmp->nodes[0] = cur;
        tmp->nodes[1] = assignment(tok, env);
        if(!tmp->nodes[1]) return 0;
        if(adjust_node(tmp) != Ok) {
            typechk_err(tok, tmp);
            return 0;
        }
        cur = tmp;
    }
    return cur;
}

U0 print_en(EvalNode* en) {
    if(!en) {
        printf("[NULL]");
        return;
    }
    switch (en->op) {
        case NumberToken:
            printf("%d", en->number);
            break;
        case NameToken:
            printf("%s", en->var->name);
            break;
        case StrToken:
            printf("....");
            break;
        default:
            printf("{%s ", token_name((Token*)en));
            print_en(en->nodes[0]);
            printf(" ");
            print_en(en->nodes[1]);
            printf("}");
    }
}
