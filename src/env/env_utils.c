#include "gwion_util.h"
#include "gwion_ast.h"
#include "gwion_env.h"
#include "traverse.h"
#include "vm.h"
#include "parse.h"

#define GET(a, b) ((a) & (b)) == (b)
ANN bool env_access(const Env env, const ae_flag flag, const loc_t loc) {
  if (env->scope->depth) {
    if (GET(flag, ae_flag_global))
      ERR_b(loc, _("`{G}global{0}` can only be used at %s scope."),
            GET(flag, ae_flag_global) && !env->class_def ? "file" : "class")
  }
  if ((GET(flag, ae_flag_static) || GET(flag, ae_flag_private) ||
       GET(flag, ae_flag_protect)) &&
      (!env->class_def || env->scope->depth))
    ERR_b(loc, _("`{G}static/private/protect{0}` can only be used at class scope."))
  return true;
}

ANN bool env_storage(const Env env, ae_flag flag, const loc_t loc) {
  CHECK_B(env_access(env, flag, loc));
  if(env->class_def && GET(flag, ae_flag_global))
    ERR_b(loc, _("`{G}global{0}` at class scope only valid for function pointers"));
  return true;
}
#undef GET

#define RETURN_TYPE(a)                                                         \
  do {                                                                         \
    const Type t = (a);                                                        \
    if (t) return t;                                                           \
  } while (0)

ANN static Type find_in_parent(const Type type, const Symbol xid) {
  Type base = type;
  while (base && base->nspc) {
    RETURN_TYPE(nspc_lookup_type1(base->nspc, xid));
    base = base->info->parent;
  }
  return NULL;
}

ANN Type find_initial(const Env env, const Symbol xid) {
  if (env->class_def) RETURN_TYPE(find_in_parent(env->class_def, xid));
  RETURN_TYPE(nspc_lookup_type1(env->curr, xid));
  const Vector v = &env->scope->nspc_stack;
  for (m_uint i = vector_size(v) + 1; --i;) {
    const Nspc nspc = (Nspc)vector_at(v, i - 1);
    RETURN_TYPE(nspc_lookup_type1(nspc, xid));
  }
  return NULL;
}
#undef RETURN_TYPE

ANN Type find_type(const Env env, Type_Decl *td) {
  DECL_OO(Type, type, = find_initial(env, td->tag.sym));
  while ((td = td->next) && type && type->nspc) {
    const Nspc nspc  = type->nspc;
    if(!(type = find_in_parent(type, td->tag.sym)))
      ERR_O(td->tag.loc, _("...(cannot find class '%s' in nspc '%s')"),
            s_name(td->tag.sym), nspc->name)
  }
  return type;
}

ANN bool can_define(const Env env, const Symbol s, const loc_t loc) {
  const Value v = nspc_lookup_value0(env->curr, s);
  if (!v || is_class(env->gwion, v->type)) return true;
  gwerr_basic(_("already declared as variable"), NULL, NULL, env->name, loc, 0);
  declared_here(v);
  env_error_footer(env);
  return false;
}

ANN static Type class_type(const Env env, const Type base) {
  const Type t_class = env->gwion->type[et_class];
  const Type t       = type_copy(env->gwion->mp, t_class);
  t->info->parent    = t_class;
  t->info->base_type = base;
  set_tflag(t, tflag_infer);
  return t;
}

ANN Value mk_class(const Env env, const Type base, const loc_t loc) {
  const Type   t   = class_type(env, base);
  const Symbol sym = insert_symbol(base->name);
  const Value  v   = new_value(env, t, MK_TAG(sym, loc));
  valuefrom(env, v->from);
  SET_FLAG(v, const);
  set_vflag(v, vflag_valid);
  nspc_add_value_front(env->curr, sym, v);
  t->info->value = base->info->value = v;
  return v;
}

ANN Value global_string(const Env env, const m_str str, const loc_t loc) {
  char c[strlen(str) + 8];
  sprintf(c, "%s:string", str);
  const Symbol sym = insert_symbol(c);
  const Value  v   = nspc_lookup_value0(env->global_nspc, sym);
  if (v) return v;
  const Value value =
      new_value(env, env->gwion->type[et_string], MK_TAG(sym, loc));
  _nspc_add_value_front(env->global_nspc, sym, value);
  return value;
}

ANN bool not_reserved(const Env env, const Tag tag) {
  const Map map = &env->gwion->data->id;
  for (m_uint i = 0; i < map_size(map); i++) {
    if (tag.sym == (Symbol)VKEY(map, i))
      ERR_b(tag.loc, _("%s is reserved."), s_name(tag.sym));
  }
  return true;
}
