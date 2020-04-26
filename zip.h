
#ifndef ZIP_H
#define ZIP_H

void z_add(void);
void z_sub(void);
void z_call_vs(void);
void z_je(void);
void z_jz(void);
void z_get_prop_len(void);
void z_store(void);
void z_storew(void);
void z_dec_chk(void);
void z_inc_chk(void);
void z_loadb(void);
void z_loadw(void);
void z_put_prop(void);
void z_test_attr(void);
void z_insert_obj(void);
void z_printf(void);
void z_new_line(void);
void z_and(void);
void z_print_num(void);
void z_jump(void);
void z_print_char(void);
void z_ret(void);
void z_rtrue(void);
void z_rfalse(void);
void z_push(void);
void z_pull(void);
void z_set_attr(void);
void z_jin(void);
void z_print_obj(void);
void z_get_parent(void);
void z_get_prop(void);
void z_get_child(void);
void z_get_sibling(void);
void z_inc(void);
void z_jl(void);
void z_ret_popped(void);
void z_read(void);

void returnfromRoutine(short returnValue);
void branchTo(int value);
void storeResult(short value);

#endif /* ZIP_H */