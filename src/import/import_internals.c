#include "gwion_util.h"
#include "gwion_ast.h"
#include "gwion_env.h"
#include "vm.h"
#include "traverse.h"
#include "instr.h"
#include "gwion.h"
#include "object.h"
#include "operator.h"
#include "import.h"
#include "gwi.h"

void gwi_body(const Gwi gwi, const Section *section) {
  const Class_Def cdef = gwi->gwion->env->class_def->info->cdef;
  if (!cdef->body) {
    cdef->body = new_mp_vector(gwi->gwion->mp, Section, 1);
    mp_vector_set(cdef->body, Section, 0, *section);
  } else {
    mp_vector_add(gwi->gwion->mp, &cdef->body, Section, (*section));
  }
}

ANN void gwi_reset(const Gwi gwi) {
  if (gwi->ck) {
    ck_clean(gwi);
    mp_free2(gwi->gwion->mp, sizeof(ImportCK), gwi->ck);
    gwi->ck = NULL;
  }
  env_reset(gwi->gwion->env);
}

ANN static bool run_with_doc(const Gwi gwi, bool (*f)(const Gwi)) {
  struct GwfmtState ls     = {.builtin = true, .nindent = 4};
  gwfmt_state_init(&ls);
  text_init(&ls.text, gwi->gwion->mp);
  Gwfmt gwfmter = {.mp = gwi->gwion->mp, .ls = &ls, .st = gwi->gwion->st };
  gwfmt_indent(&gwfmter);
  gwfmt_util(&gwfmter, "{-}#!+ %s{0}\n", gwi->gwion->env->name);
  gwi->gwfmt = &gwfmter;
  const bool ret = f(gwi);
  fprintf(stdout, "%s", ls.text.str);
  free_mstr(gwi->gwion->mp, ls.text.str);
  return ret;
}

ANN bool gwi_run(const Gwion gwion, bool (*f)(const Gwi)) {
  OperCK       oper = {};
  struct Gwi_  gwi  = {.gwion = gwion, .oper = &oper};
  const bool ret  = !gwion->data->cdoc ? f(&gwi) : run_with_doc(&gwi, f);
  if (!ret) gwi_reset(&gwi);
  return ret;
}

