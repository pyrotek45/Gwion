#include "gwion_util.h"
#include "gwion_ast.h"
#include "oo.h"
#include "vm.h"
#include "env.h"
#include "type.h"
#include "instr.h"
#include "object.h"
#include "instr.h"
#include "vm.h"
#include "gwion.h"
#include "value.h"
#include "operator.h"
#include "import.h"
#include "gwi.h"

GWION_IMPORT(extend_array_test) {
  GWI_OB(gwi_class_ini(gwi, "ArrayExt", "float[1]"))
  GWI_BB(gwi_class_end(gwi))
  return GW_OK;
}
