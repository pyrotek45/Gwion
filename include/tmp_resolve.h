#ifndef __TMPL_RESOLVE
#define __TMPL_RESOLVE
ANN Func find_template_match(const Env env, const Value value,
                             Exp_Call *const exp);
ANN Func find_func_match(const Env env, const Func up, Exp_Call *const exp);
ANN2(1,2) bool check_tmpl(const Env env, const TmplArg_List tl, const Specialized_List sl, const loc_t loc, const bool is_spread);
#endif
