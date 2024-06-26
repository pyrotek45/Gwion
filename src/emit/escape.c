#include "gwion_util.h"
#include "gwion_ast.h"
#include "gwion_env.h"
#include "vm.h"
#include "instr.h"
#include "emit.h"
#include "escape.h"

static char escape_table[256] = {
  ['0']  = '0',
  ['\''] = '\'',
  ['"']  = '"',
  ['\\'] = '\\',
  ['a']  = (char)7,  // audible bell
  ['b']  = (char)8,  // back space
  ['f']  = (char)12, // form feed
  ['n']  = (char)10, // new line
  ['r']  = (char)13, // carriage return
  ['t']  = (char)9,  // horizontal tab
  ['v']  = (char)11, // vertical tab
};

static bool get_escape(const Emitter emit, const char c, char *out, const loc_t loc) {
  *out = escape_table[(int)c];
  if(out) return true;
  env_err(emit->env, loc, _("unrecognized escape sequence '\\%c'"), c);
  return false;
}

bool escape_str(const Emitter emit, const m_str base, const loc_t loc) {
  unsigned char *str_lit = (unsigned char *)base;
  m_str          str     = base;
  while (*str_lit) {
    if (*str_lit == '\\') {
      ++str_lit;
      const unsigned char c  = *(str_lit);
      const unsigned char c2 = *(str_lit + 1);
      if (c >= '0' && c <= '7') {
        if (c == '0' && (c2 < '0' || c2 > '7'))
          *str++ = '\0';
        else {
          const unsigned char c3 = *(str_lit + 2);
          if (c2 >= '0' && c2 <= '7' && c3 >= '0' && c3 <= '7') {
            *str++ = (char)((c - '0') * 64 + (c2 - '0') * 8 + (c3 - '0'));
            str_lit += 2;
          } else {
            env_err(emit->env, loc,
                    _("malformed octal escape sequence '\\%c%c%c'"), c, c2, c3);
            return false;
          }
        }
      } else if (c == 'x' || c == 'X' || c == 'u' || c == 'U') {
        ++str_lit;
        const unsigned char c1 = *(str_lit);
        const unsigned char c3 = *(str_lit + 1);
        if (c1 >= '0' && c1 <= 'F' && c3 >= '0' && c3 <= 'F') {
          *str++ = (char)((c1 - '0') * 16 + (c3 - '0'));
          ++str_lit;
        } else {
          env_err(emit->env, loc, _("malformed hex escape sequence '\\%c%c'"),
                  c1, c3);
          return false;
        }
      } else {
        char out;
        CHECK_B(get_escape(emit, (char)c, &out, loc));
        *str++ = out;
      }
    } else
      *str++ = (char)*str_lit;
    ++str_lit;
  }
  *str = '\0';
  return true;
}

ANN bool str2char(const Emitter emit, const m_str c, char *out, const loc_t loc) {
  if(c[0] != '\\') {
    *out = c[0];
    return true;
  }
  return get_escape(emit, c[1], out, loc);
}
