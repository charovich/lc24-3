U8* lc24_registers[] = {
    "%ac",
    "%bs",
    "%dt",
    "%di"
    "%cn",
    // "%dx",
};
U8 lc24_relc = sizeof(lc24_registers) / sizeof(lc24_registers[0]);

typedef struct LC24Str {
    U32 local_id;
    U8* data;
    struct LC24Str* next;
} LC24Str;

typedef struct {
    FILE* out;
    U8 next_ifid;
    U8 next_loopid;
    LC24Str* last_str;
} LC24Fn;

typedef struct {
    LC24Fn* fn;
    U8 next_reg;
} LC24Line;

U32 lc24_append_str(LC24Fn* fn, U8* str) {
    U8 nid = fn->last_str->local_id + 1;
    LC24Str* nstr = new(LC24Str);
    nstr->data = str;
    nstr->local_id = nid;
    nstr->next = 0;
    fn->last_str->next = nstr;
    fn->last_str = nstr;
    return nid;
}
U8* lc24_get_reg(LC24Line* state) {
    return lc24_registers[state->next_reg++];
}
U8* lc24_rem_reg(LC24Line* state) {
    return lc24_registers[--state->next_reg];
}
U8* lc24_last_reg(LC24Line* state) {
    return lc24_registers[state->next_reg-1];
}
U0 lc24_reduce(LC24Line* state, U8** dst, U8** src) {
    *dst = lc24_registers[state->next_reg-2];
    *src = lc24_registers[state->next_reg-1];
    state->next_reg--;
}

/*
ERR lc24_crt_start(FILE* out) {
    fprintf(out,
            "; Compiled by RCC2\n"
            "; Check if this is GovnOS Executable\n"
            "  jsr main int 0\n");
}
*/


ERR lc24_crt_start(FILE* out) {
    fprintf(out,
            "; Compiled by RCC2\n"
            "; Check if this is GovnOS Executable\n"
            "  ldb %%lx\n"
            "  cmp %%bs 0\n"
            "  jmne .isgs\n"
            "  psh .null_cmd\n"
            "  mov %%bs .null_cmd\n"
            "  mov %%lx %%bs\n"
            ".isgs:\n"
            "  psh %%lx\n"
            "; Call entrypoint\n"
            "  jsr main\n"
            "  ldb %%sp\n"
            "  add %%bs 4\n"
            "  mov %%sp %%bs\n"
            "  rts\n"
            ".null_cmd: psh $00 int $00\n");
}

U8 lc24_letter(Type* type) {
    switch (type->type) {
        case IntType:
        case FltType:
        case PtrType:
            switch(type->size) {
                case 1: return 'b';
                case 2: return 'w';
                case 3: return 'h';
            }
        default:
            fprintf(stderr, "Unloadable type with size %d (%d from %p)\n", type->size, type->type, type);
            exit(1);
            return 0;
    }
}
ERR lc24_emit_en(LC24Line* state, EvalNode* en);

ERR lc24_emit_enl(LC24Line* state, EvalNode* en) {
    switch (en->op) {
        case NameToken: {
            switch (en->var->type) {
                case LocalVar: {
                    //U8 letter = lc24_letter(en->var->decl);
                    U8* reg = lc24_get_reg(state);
                    fprintf(state->fn->out,
                            "  mov %s %%bp\n"
                            "  add %s %d ; %s\n",
                            reg,
                            reg,
                            en->var->stack_loc, en->var->name);
                    return Ok;
                }
                case GlobalVar: {
                    fprintf(state->fn->out, "  mov %s %s\n", lc24_get_reg(state), en->var->name);
                    return Ok;
                }
                default:
                    error("Variable %s is not usable here", en->var->name);
                    return Error;
            }
        }
        case Star ^ 0x80: {
            return lc24_emit_en(state, en->nodes[0]);
        }
        default:
            return Error;
    }
}

ERR lc24_emit_farg(LC24Line* state, EvalNode* en, U8* arlc) {
    if (en->op == RightParen) {
        if (lc24_emit_farg(state, en->nodes[1], arlc) != Ok) return Error;
        if (lc24_emit_farg(state, en->nodes[0], arlc) != Ok) return Error;
    } else {
        // if (en->op == NameToken && en->var->type == GlobalRVar)
        (*arlc)++;
        if (en->op == StrToken) {
            U32 sid = lc24_append_str(state->fn, en->str_name);
            fprintf(state->fn->out, "  psh .str%d\n", sid);
            return Ok;
        } else if (en->op == NumberToken) {
            fprintf(state->fn->out, "  psh %d\n", en->number);
            return Ok;
        }
        if (lc24_emit_en(state, en) != Ok) return Error;
        fprintf(state->fn->out, "  psh %s\n", lc24_rem_reg(state));
    }
    return Ok;
}
TokenType lc24_neg_cmp(TokenType cmptype) {
    switch (cmptype) {
        case Less:
            return GreaterEqual;
        case LessEqual:
            return Greater;
        case Greater:
            return LessEqual;
        case GreaterEqual:
            return Less;
        case Equal:
            return NotEqual;
        case NotEqual:
            return Equal;
    }
}
ERR lc24_emit_cmp(FILE* out, U8* r1, U8* r2, TokenType cmptype) {
    switch (cmptype) {
        case Less:
            fprintf(out, "  cmp %s %s\n  xor %s %s\n  jn ", r1, r2, r1, r1);
            return Ok;
        case LessEqual:
            fprintf(out, "  cmp %s %s\n  xor %s %s\n  js ", r2, r1, r1, r1);
            return Ok;
        case Greater:
            fprintf(out, "  cmp %s %s\n  xor %s %s\n  jn ", r2, r1, r1, r1);
            return Ok;
        case GreaterEqual:
            fprintf(out, "  cmp %s %s\n  xor %s %s\n  js ", r1, r2, r1, r1); // осуждаю js
            return Ok;
        case Equal:
            fprintf(out, "  cmp %s %s\n  xor %s %s\n  jme ", r1, r2, r1, r1);
            return Ok;
        case NotEqual:
            fprintf(out, "  cmp %s %s\n  xor %s %s\n  jmne ", r1, r2, r1, r1);
            return Ok;
    }
}
ERR lc24_emit_en(LC24Line* state, EvalNode* en) {
    switch (en->op) {
        case LeftParen: {
            U8 arlc = 0;
            if (en->nodes[1])
                if(lc24_emit_farg(state, en->nodes[1], &arlc) != Ok) return Error;
            // if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            U32 savereg = state->next_reg;
            while (state->next_reg) {
                printf("  psh %s", lc24_rem_reg(state));
            }
            if (en->nodes[0]->op != NameToken) {
                fprintf(stderr, "Attempt to call SHIT");
                return Error;
            }
            fprintf(state->fn->out, "  jsr %s\n", en->nodes[0]->var->name);
            if (arlc) {
                fprintf(state->fn->out, "  mov %%lx %d\n  add %%sp %%lx\n", arlc * TGTWORD);
                // fprintf(state->fn->out, "  mov %%lx 3\n  add %%sp %%lx\n");
            }
            while (savereg--) {
                printf("  pop %s", lc24_get_reg(state));
            }
            if (state->next_reg != 0) {
                fprintf(state->fn->out, "  mov %s %%ac\n", lc24_get_reg(state));
            } else {
                lc24_get_reg(state);
            }
            return Ok;
        }
        case NumberToken:
            fprintf(state->fn->out, "  mov %s %u\n", lc24_get_reg(state), en->number);
            return Ok;
        case NameToken:
            switch (en->var->type) {
                case LocalVar: {
                    U8 letter; if (!(letter = lc24_letter(en->type))) return Error;
                    if (en->var->stack_loc < 32) {
                        fprintf(state->fn->out,
                                "  lob%c %s %d ; %s\n",
                                letter,
                                lc24_get_reg(state),
                                en->var->stack_loc, en->var->name);
                    } else {
                        U8* reg = lc24_get_reg(state);
                        fprintf(state->fn->out,
                                "  mov %s %%bp\n"
                                "  add %s %d ; %s\n"
                                "  l%c %s %s\n",
                                reg, reg, en->var->stack_loc, en->var->name,
                                letter, reg, reg);
                    }
                    return Ok;
                }
                case GlobalVar: {
                    U8 letter; if (!(letter = lc24_letter(en->type))) return Error;
                    U8* reg = lc24_get_reg(state);
                    fprintf(state->fn->out,
                            "  mov %s %s\n"
                            "  l%c %s %s\n",
                            reg, en->var->name,
                            letter,
                            reg, reg);
                    return Ok;
                }
                default:
                    error("Variable %s is not usable here", en->var->name);
                    return Error;
            }
        case Assign: {
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            U8 letter; if (!(letter = lc24_letter(en->nodes[0]->type))) return Error;

            if (en->nodes[0]->op == NameToken && en->nodes[0]->var->type == LocalVar && en->nodes[0]->var->stack_loc < 32) {
                // Save shit
                fprintf(state->fn->out, "  stb%c %s %d ; LOCAL.%s\n", letter, lc24_last_reg(state), en->nodes[0]->var->stack_loc, en->nodes[0]->var->name);
                return Ok;
            }
            if(lc24_emit_enl(state, en->nodes[0])!=Ok)return Error;
            U8 *dst, *src;
            lc24_reduce(state, &dst, &src);
            fprintf(state->fn->out, "  s%c %s %s\n", letter, src, dst);
            return Ok;
        }
        case StrToken: {
            U32 sid = lc24_append_str(state->fn, en->str_name);
            fprintf(state->fn->out, "  mov %s .str%d\n", lc24_get_reg(state), sid);
            return Ok;
        }
        case Plus:
        case Minus:
        case Star:
        case Div: {
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            if (en->nodes[1]->lvl == Comptime && en->nodes[0]->type->type != FltType && en->nodes[1]->op ==NumberToken) {
                if (en->nodes[1]->number == 1 && (en->op == Plus || en->op == Minus)) {
                    fprintf(state->fn->out, "  %s %s\n", en->op == Plus ? "inx" : "dex", lc24_last_reg(state));
                    return Ok;
                }
                fprintf(state->fn->out, "  %s %s %d\n",
                        en->op == Plus ? "add" :
                        en->op == Minus ? "sub" :
                        en->op == Star ? "mul" :
                        en->op == Div ? "div" : 0,
                        lc24_last_reg(state), en->nodes[1]->number);
                return Ok;
            }
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            U8 *dst, *src;
            lc24_reduce(state, &dst, &src);
            U8 *op;
            switch (en->nodes[0]->type->type) {
                case IntType:
                case PtrType:
                    switch (en->op) {
                        case Plus: op = "add"; break;
                        case Minus: op = "sub"; break;
                        case Star: op = "mul"; break;
                        case Div: op = "div"; break;
                    }
                    break;
                case FltType:
                    switch (en->op) {
                        case Plus: op = "addf24"; break;
                        case Minus: op = "subf24"; break;
                        case Star: op = "mulf24"; break;
                        case Div: op = "divf24"; break;
                    }
                    break;
            }
            fprintf(state->fn->out, "  %s %s %s\n", op, dst, src);
            return Ok;
        }
        case LeftShift:
        case RightShift: {
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            if (en->nodes[1]->lvl == Comptime && en->nodes[1]->op == NumberToken) {
                fprintf(state->fn->out, "  sa%c %s %d\n",
                        en->op == LeftShift ? 'l' : 'r',
                        lc24_last_reg(state), en->nodes[1]->number);
                return Ok;
            }
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            U8 *dst, *src;
            lc24_reduce(state, &dst, &src);
            fprintf(state->fn->out,
                    "  mov %%dc 2\n"
                    "  pow %%dc %s\n"
                    "  %s %s %%dc\n",
                        src, en->op == LeftShift ? "mul" : "div", dst);
            return Ok;
        }
        case Mod: {
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            U8 *dst, *src;
            if (en->nodes[1]->lvl == Comptime && en->nodes[0]->type->type != FltType) {
                dst = lc24_last_reg(state);
                fprintf(state->fn->out, "  div %s %d\n  mov %s %%dc", dst, en->nodes[1]->number, dst);
                return Ok;
            }
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            lc24_reduce(state, &dst, &src);
            U8 *op;
            if (en->nodes[0]->type->type == IntType) {
                fprintf(state->fn->out, "  div %s %s\n  mov %s %%dc", dst, src, dst);
            } else {
                fprintf(state->fn->out,
                        "  divf24 %s %s\n"
                        "  mov %%dc %s\n"
                        "  cfi %%dc\n"
                        "  cif %%dc\n"
                        "  subf24 %s %%dc\n", dst, src, dst, dst);
            }
            return Ok;
        }
        case And:
        case Or:
        case Caret: {
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            U8* op;
            switch (en->op) {
                case And: op = "and"; break;
                case Or: op = "ora"; break;
                case Caret: op = "xor"; break;
            }
            U8 *dst, *src;
            lc24_reduce(state, &dst, &src);
            fprintf(state->fn->out, "  %s %s %s\n", op, dst, src);
            return Ok;
        }
        case Less:
        case LessEqual:
        case Greater:
        case GreaterEqual:
        case Equal:
        case NotEqual: {
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            U8 *dst, *src;
            U8 ifid = state->fn->next_ifid++;
            lc24_reduce(state, &dst, &src);
            lc24_emit_cmp(state->fn->out, dst, src, lc24_neg_cmp(en->op));
            fprintf(state->fn->out, ".else%d\n  inx %s\n.else%d:\n", ifid, dst, ifid);
            return Ok;
        }
        case Not: {
            if (lc24_emit_en(state, en->nodes[0])!=Ok) return Error;
            U8* reg = lc24_last_reg(state);
            fprintf(state->fn->out, "  nrm %s\n  xor %s 1\n", reg, reg);
            return Ok;
        }
        case AndAnd:
        case OrOr: {
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            U8 ifid = state->fn->next_ifid++;
            fprintf(state->fn->out,
                    "  cmp %s  0\n"
                    "  %s .known%d\n",
                    lc24_rem_reg(state),
                    en->op == OrOr ? "jmne" : "jme", ifid);
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            fprintf(state->fn->out, ".known%d:\n", ifid);
            return Ok;
        }
        case Question:
            if(lc24_emit_en(state, en->nodes[0])!=Ok)return Error;
            U8 ifid = state->fn->next_ifid++;
            fprintf(state->fn->out,
                    "  cmp %s 0\n"
                    "  jme .else%d\n", lc24_rem_reg(state), ifid);
            if(lc24_emit_en(state, en->nodes[1])!=Ok)return Error;
            fprintf(state->fn->out,
                    "  jmp .ifend%d\n"
                    ".else%d:\n", ifid, ifid);
            // Remove virtual reg
            lc24_rem_reg(state);
            if(lc24_emit_en(state, en->nodes[2])!=Ok)return Error;
            fprintf(state->fn->out, ".ifend%d:\n", ifid);
            return Ok;
        case PlusPlus:
        case MinusMinus:
        case PlusPlus ^ 0x80:
        case MinusMinus ^ 0x80:
        {
            if (lc24_emit_enl(state, en->nodes[0])!=Ok) return Error;
            U8* reg = lc24_last_reg(state);
            U8 letter; if (!(letter = lc24_letter(en->type))) return Error;
            if (en->type->type == FltType) {
                fprintf(stderr, "Float ++/-- is not supported!\n");
                return Error;
            }
            fprintf(state->fn->out,
                    "  l%c %s %%dc\n", letter, reg);
            if (en->type->type == PtrType && en->type->item->size > 1) {
                fprintf(state->fn->out,
                        "  %s %%dc %d\n",
                        ((en->op & 0x7F) == PlusPlus ? "add" : "sub"),
                        en->type->item->size);
            } else {
                fprintf(state->fn->out,
                        "  %s %%dc\n",
                        ((en->op & 0x7F) == PlusPlus ? "inx" : "dex"));
            }
            if (en->type->size > 1) {
                fprintf(state->fn->out,
                        "  sub %s %d\n", reg, en->type->size);
            } else {
                fprintf(state->fn->out,
                        "  dex %s\n", reg);
            }
            fprintf(state->fn->out,
                    "  s%c %s %%dc\n"
                    "  mov %s %%dc\n",
                    letter, reg, reg);
            if (en->op & 0x80) {
                if (en->type->type == PtrType && en->type->item->size > 1) {
                fprintf(state->fn->out,
                        "  %s %s %d\n",
                        ((en->op & 0x7F) == PlusPlus ? "sub" : "add"),
                        reg,
                        en->type->item->size);
            } else {
                fprintf(state->fn->out,
                        "  %s %s\n",
                        ((en->op & 0x7F) == PlusPlus ? "dex" : "inx"),
                        reg);
            }
            }
            return Ok;
        }
        case And ^ 0x80: {
            return lc24_emit_enl(state, en->nodes[0]);
        }
        case Star ^ 0x80: {
            if (lc24_emit_en(state, en->nodes[0])!=Ok) return Error;
            U8* reg = lc24_last_reg(state);
            U8 letter; if (!(letter = lc24_letter(en->type))) return Error;
            fprintf(state->fn->out, "  l%c %s %s\n", letter, reg, reg);
            return Ok;
        }
        case Minus ^ 0x80: {
            if (lc24_emit_en(state, en->nodes[0])!=Ok) return Error;
            U8* reg = lc24_last_reg(state);
            fprintf(state->fn->out, "  not %s inx %s\n", reg, reg);
            return Ok;
        }
        case Div ^ 0x80: {
            if (lc24_emit_en(state, en->nodes[0])!=Ok) return Error;
            fprintf(state->fn->out, "  cfi %s\n", lc24_last_reg(state));
            return Ok;
        }
        case Semi: {
            if (lc24_emit_en(state, en->nodes[0])!=Ok) return Error;
            fprintf(state->fn->out, "  cif %s\n", lc24_last_reg(state));
            return Ok;
        }
        default:
            fprintf(stderr, "Failed to compile op %02X\n", en->op);
            return Error;
    }
}
ERR emit_blk(LC24Fn* fn, Block* blk) {
    switch (blk->type) {
        case ENABlock: {
            LC24Line st;
            st.fn = fn;
            st.next_reg = 0;
            if(lc24_emit_en(&st, blk->en)!=Ok)return Error;
            return Ok;
        }
        case RetBlock: {
            LC24Line st;
            st.fn = fn;
            st.next_reg = 0;
            if(lc24_emit_en(&st, blk->en)!=Ok)return Error;
            fprintf(fn->out, "  jmp .return\n");
            return Ok;
        }
        case LinkBlock:
            if(emit_blk(fn, blk->prev)!=Ok)return Error;
            if(emit_blk(fn, blk->last)!=Ok)return Error;
            return Ok;
        case IfBlock: {
            LC24Line st;
            st.fn = fn;
            st.next_reg = 0;
            if(lc24_emit_en(&st, blk->en)!=Ok)return Error;
            U8 ifid = fn->next_ifid++;
            if (blk->alt_body) {
                fprintf(fn->out,
                        "  cmp %s 0\n"
                        "  jme .else%d\n", lc24_rem_reg(&st), ifid);
                if(emit_blk(fn, blk->body)!=Ok)return Error;
                fprintf(fn->out, "  jmp .ifend%d\n.else%d:\n", ifid, ifid);
                if(emit_blk(fn, blk->alt_body)!=Ok)return Error;
                fprintf(fn->out, ".ifend%d:\n", ifid);
            } else {
                fprintf(fn->out,
                        "  cmp %s 0\n"
                        "  jme .ifend%d\n", lc24_rem_reg(&st), ifid);
                if(emit_blk(fn, blk->body)!=Ok)return Error;
                fprintf(fn->out, ".ifend%d:\n", ifid);
            }
            return Ok;
        }
        case WhileBlock: {
            LC24Line st;
            st.fn = fn;
            st.next_reg = 0;
            U8 loopid = fn->next_loopid++; // бесплатно
            fprintf(fn->out, ".loop%d:\n", loopid);
            if(lc24_emit_en(&st, blk->en)!=Ok)return Error;
            fprintf(fn->out,
                    "  cmp %s 0\n"
                    "  jme .loopend%d\n", lc24_rem_reg(&st), loopid);
            if(emit_blk(fn, blk->body)!=Ok)return Error;
            fprintf(fn->out, "  jmp .loop%d\n.loopend%d:\n", loopid, loopid);
            return Ok;
        }
        case NullBlock:
            //fprintf(out, "[Null!]");
            return Ok;
    }
    return Error;
}
ERR lc24_emit_fn(FILE* out, U8* name, Func* fn) {
    fprintf(out, "%s:\n", name);
    if (fn->stack_len || fn->has_args) {
        fprintf(out,
                "  psh %%bp\n"
                "  sub %%sp %d\n"
                "  mov %%bp %%sp inx %%bp\n", fn->stack_len);
    } else if (fn->has_args) {
        fprintf(out,
                "  psh %%bp\n"
                "  mov %%bp %%sp\n");
    }
    LC24Fn gfn = {};
    gfn.out = out;
    LC24Str dummy;
    dummy.next = 0;
    dummy.local_id = 0;
    gfn.last_str = &dummy;
    if(emit_blk(&gfn, fn->body)!=Ok)
        return Error;
    if (fn->stack_len || fn->has_args) {
        fprintf(out,
                ".return:\n"
                "  add %%sp %d\n"
                "  pop %%bp\n"
                "  rts\n", fn->stack_len);
    } else if (fn->has_args) {
        fprintf(out,
                ".return:\n"
                "  pop %%bp\n"
                "  rts\n");
    } else {
        fprintf(out,
                ".return:\n"
                "  rts\n");
    }

    for(LC24Str* ptr = dummy.next; ptr; ptr = ptr->next) {
        fprintf(out, ".str%d: bytes ", ptr->local_id);
        lostasm_embed(out, ptr->data);
        fprintf(out, "\n");
    }

    fprintf(out, "\n");
    return Ok;
}
ERR lc24_emit_var(FILE* out, U8* name, Type* type) {
    fprintf(out, "%s: reserve %d bytes\n", name, type->size);
}
ERR lc24_asm_start(FILE* out, U8* name) {
    fprintf(out, "%s:\n", name);
    return Ok;
}
ERR lc24_asm_emit(FILE* out, U8* data) {
    if (data[0] == '.')
        fprintf(out, "%s\n", data);
    else
        fprintf(out, "  %s\n", data);
    return Ok;
}
U16 lc24_arg_len(U16 stack_len, U8 arlc) {
    return stack_len + TGTWORD * (arlc - 1 + 2);
    // it works and i dont know why
}
Target lc24 = {
    .crt_start = &lc24_crt_start,
    .arg_start = &lc24_arg_len,
    .emit_fn = &lc24_emit_fn,
    .emit_var = &lc24_emit_var,
    .asm_start = &lc24_asm_start,
    .asm_emit = &lc24_asm_emit,
};
