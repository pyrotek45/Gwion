#!/usr/bin/bash
mkdir -p embed
rm -f embed/*

#json=$(jq -c '.' "$1")
json=$(cat "$1")

cat  << EOF >> embed/embed_head
#include "gwion_util.h"
#include "gwion_ast.h"
#include "gwion_env.h"
#include "vm.h"
#include "instr.h"
#include "emit.h"
#include "compile.h"
#include "gwion.h"
#include "object.h"
#include "operator.h"
#include "import.h"
#include "gwi.h"

EOF

header() {
  echo "$1" >> embed/embed_head
}

config() {
  echo "$1" >> embed/embed.mk
}

has_func() {
  nm "$2" | grep "${1}_${3}" > /dev/null
}
plugin() {
  has_func "gwimport" "$1" "$2" && {
    header "extern m_bool gwimport_${2}(const Gwi);"
    echo "  plug->plugin = gwimport_${2};"
  }
}
driver() {
  has_func "gwdriver" "$1" "$2" && {
    header "extern void *gwdriver_$2(struct DriverData_*);"
    echo "  plug->driver = gwdriver_$2;"
  }
}
modini() {
  has_func "gwmodini" "$1" "$2" && {
    header "extern m_bool gwmodini_${2}(gwion);"
    echo "  plug->modini = gwmodini_${2};"
  }
}
modend() {
  has_func "gwmodend" "$1" "$2" && {
    header "extern m_bool gwmodend_${2}(gwion);"
    echo "  plug->modend = gwmodend_${2};"
  }
}

handle_lib() {
  cat << EOF >> embed/embed_body
ANN static void embed_${name}(const Gwion gwion) {
  Plug plug = new_plug(gwion->mp);
$(modini "$1" "$2")$(modend "$1" "$2")$(plugin "$1" "$2")$(driver "$1" "$2")
  map_set(&gwion->data->plugs->map, (vtype)strdup("${2}"), (vtype)plug);
}

EOF
}

cat << EOF >> embed/embed_foot
ANN static void compile_script(const Gwion gwion, const m_str filename,
                              const m_str content, const size_t sz)  {
  const m_str str = mp_malloc2(gwion->mp, sz + 1);
  memcpy(str, content, sz);
  str[sz] = 0;
  compile_string(gwion, filename, str);
  mp_free2(gwion->mp, sz + 1, str);
}

EOF

echo "ANN void gwion_embed(const Gwion gwion) {" >> embed/embed_foot
jq -rc '.libraries|.[]' <<< "$json" |
  while read -r lib
  do
    path=$(jq -c '.path' <<< "$lib" | sed -e 's/^"//' -e 's/"$//')
    names=$(jq -c '.names' <<< "$lib")
    config "LDFLAGS += $path"
    if [ "$names" != "null" ]
    then
      jq -c '.[]' <<< "$names" | sed -e 's/^"//' -e 's/"$//' |
      while read -r name
      do
        echo "  embed_${name}(gwion);" >> embed/embed_foot
        handle_lib "$path" "$name"
      done
    fi
  done

handle_script() {
  name="script$2"
  xxd -name "$name" -i "$1" > "embed/${name}.h"
  header "#include \"${name}.h\""
  echo "  compile_script(gwion, \"$name\", ${name}, ${name}_len);"
}

handle_scripts() {
  i=0
  jq -r '.scripts|.[]' <<< "$json"  |
    while read -r name;
    do handle_script "$name" "$i"; i=$((i+1));
    done
}

handle_scripts >> embed/embed_foot
echo "}" >> embed/embed_foot

cat embed/embed_head embed/embed_body embed/embed_foot > embed/embed.c
rm embed/embed_head embed/embed_body embed/embed_foot

