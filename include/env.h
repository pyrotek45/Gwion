#ifndef __ENV
#define __ENV

#define SCOPE(a) { ++env->scope; a ;--env->scope; }
#define NSPC(a) { nspc_push_value(env->curr); SCOPE(a); nspc_pop_value(env->curr); }

struct Env_Scope_ {
  struct Vector_    nspc_stack;
  struct Vector_    class_stack;
  struct Vector_    breaks;
  struct Vector_    conts;
  struct Vector_    known_ctx;
  struct Match_ *match;
  size_t depth;
  size_t type_xid;
};

typedef struct Env_       * Env;
struct Env_ {
  m_str name;
  Nspc curr;
  Nspc global_nspc;
  struct Context_ *context;
  Type      class_def;
  Func      func;
  struct Gwion_ *gwion;
  struct Env_Scope_* scope;
};

ANEW Env new_env(MemPool);
ANN void env_reset(const Env);
ANN void free_env(Env);
ANN2(1,3) m_uint env_push(const Env, const Type, const Nspc);
ANN static inline m_uint env_push_global(const Env env) { return env_push(env, NULL, env->global_nspc); }
ANN void env_pop(const Env, const m_uint);
ANN Map env_label(const Env);
ANN Type scan_type(const Env, const Type, const Type_Decl*);
ANN Type type_decl_resolve(const Env, const Type_Decl*);
// tl2str returns a mp_alloced string
ANEW ANN m_str tl2str(const Env, const Type_List); // in type_decl.c
ANN m_bool compat_func(const __restrict__ Func_Def, const __restrict__ Func_Def);
ANN Type known_type(const Env env, const Type_Decl*);
ANN Type type_nonnull(const Env env, const Type base);
ANN Type prim_ref(const Env env, const Type t, const Type_Decl* td);
ANN m_bool env_access(const Env env, const ae_flag flag, const loc_t pos);
ANN m_bool env_storage(const Env env, ae_flag flag, const loc_t pos);
ANN void env_add_type(const Env, const Type);
ANN Type find_type(const Env, ID_List);
ANN m_bool already_defined(const Env env, const Symbol s, const loc_t pos);
ANN m_bool type_engine_check_prog(const Env, const Ast);
ANN m_bool traverse_func_template(const Env, const Func_Def);
ANN ID_List str2list(const Env, const m_str path, m_uint* array_depth, const loc_t);
ANN2(1,3) void env_err(const Env, const struct YYLTYPE *pos, const m_str fmt, ...);
#endif
