#include "gwion_util.h"
#include "gwion_ast.h"
#include "oo.h"
#include "vm.h"
#include "env.h"
#include "type.h"
#include "object.h"
#include "instr.h"
#include "gwion.h"
#include "value.h"
#include "operator.h"
#include "import.h"

GWION_IMPORT(enum_test) {
  GWI_BB(gwi_enum_ini(gwi, NULL))
    GWI_BB(gwi_enum_add(gwi, "ENUM0", 0))
    GWI_BB(gwi_enum_add(gwi, "ENUM1", 1))
    GWI_BB(gwi_enum_add(gwi, "ENUM2", 2))
    GWI_BB(gwi_enum_add(gwi, "ENUM3", 3))
    GWI_BB(gwi_enum_add(gwi, "ENUM4", 4))
    GWI_BB(gwi_enum_add(gwi, "ENUM5", 5))
    GWI_BB(gwi_enum_add(gwi, "ENUM6", 6))
    GWI_BB(gwi_enum_add(gwi, "ENUM7", 7))
    GWI_BB(gwi_enum_add(gwi, "ENUM8", 8))
    GWI_BB(gwi_enum_add(gwi, "ENUM9", 9))
  GWI_OB(gwi_enum_end(gwi))

  GWI_BB(gwi_enum_ini(gwi, "test"))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM0", 0))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM1", 1))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM2", 2))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM3", 3))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM4", 4))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM5", 5))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM6", 6))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM7", 7))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM8", 8))
    GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM9", 9))
  GWI_OB(gwi_enum_end(gwi))

  Type t_enum;
  GWI_OB((t_enum = gwi_mk_type(gwi, "Enum", 0, NULL)))
  GWI_BB(gwi_class_ini(gwi, t_enum, NULL, NULL))
    GWI_BB(gwi_enum_ini(gwi, 0))
      GWI_BB(gwi_enum_add(gwi, "ENUM0", 0))
      GWI_BB(gwi_enum_add(gwi, "ENUM1", 1))
      GWI_BB(gwi_enum_add(gwi, "ENUM2", 2))
      GWI_BB(gwi_enum_add(gwi, "ENUM3", 3))
      GWI_BB(gwi_enum_add(gwi, "ENUM4", 4))
      GWI_BB(gwi_enum_add(gwi, "ENUM5", 5))
      GWI_BB(gwi_enum_add(gwi, "ENUM6", 6))
      GWI_BB(gwi_enum_add(gwi, "ENUM7", 7))
      GWI_BB(gwi_enum_add(gwi, "ENUM8", 8))
      GWI_BB(gwi_enum_add(gwi, "ENUM9", 9))
    GWI_OB(gwi_enum_end(gwi))

    GWI_BB(gwi_enum_ini(gwi, "Enumtest"))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM0", 0))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM1", 1))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM2", 2))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM3", 3))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM4", 4))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM5", 5))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM6", 6))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM7", 7))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM8", 8))
      GWI_BB(gwi_enum_add(gwi, "TYPED_ENUM9", 9))
    GWI_OB(gwi_enum_end(gwi))
  GWI_OB(gwi_class_end(gwi))

  return GW_OK;
}
