/*
 * globals.h: global parser state
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#define STACKSIZE 16

#define BASE_APP_C \
  "/*\n" \
  " * app.c: Built with TML\n" \
  " */\n" \
  "#include \"tuibox.h\"\n" \
  "\n" \
  "ui_t u;\n" \
  "\n" \
  "void stop(){\n" \
  "  ui_free(&u);\n" \
  "  exit(0);\n" \
  "}\n" \
  "\n" \
  "int main(){\n" \
  "  ui_new(0, &u);\n" \
  "  ui_key(\"q\", stop, &u);\n" \
  "\n"
#define END_APP_C \
  "  ui_loop(&u){\n" \
  "    ui_update(&u);\n" \
  "  }\n" \
  "\n" \
  "  return 0;\n" \
  "}"

#define BASE_EVENTS_H \
  "/*\n" \
  " * tml_events.h: Built with TML\n" \
  " */\n" \
  "#ifndef __EVENTS_H\n" \
  "#define __EVENTS_H\n" \
  "\n/* TODO */\n"
#define END_EVENTS_H "\n" \
  "#endif"

int g_screen = -1;
int g_sind = 0;
char *g_expected = NULL;
struct tml_tag_t *g_stack[STACKSIZE];

typedef void (*func)();
struct tml_tag_t {
  char *name;
  func write;
  func onload;
  char *parent;
};

void _writebox(char *out, struct tml_tag_t tag){
  sprintf(
    out,
    "  ui_add(\n" \
    "    %i, %i,\n" \
    "    %i, %i,\n" \
    "    %i,\n" \
    "    NULL, 0,\n" \
    "    %s,\n" \
    "    %s,\n" \
    "    %s,\n" \
    "    NULL, NULL\n" \
    "    &u\n" \
    "  );\n\n",
    1, 1,
    1, 1,
    g_screen,
    "NULL",
    "NULL",
    "NULL"
  );
}
void _writetext(char *out, struct tml_tag_t tag){
  sprintf(
    out,
    "  ui_text(\n" \
    "    %i, %i,\n" \
    "    %i,\n" \
    "    %s, %s,\n" \
    "    &u\n" \
    "  );\n\n",
    1, 1,
    g_screen,
    "NULL",
    "NULL"
  );
}

void _setscreen(){
  g_screen++;
}

struct tml_tag_t tags[] = {
  { "ui", NULL, NULL, NULL },
  { "screen", NULL, _setscreen, "ui" },
  { "box", _writebox, NULL, "screen" },
  { "text", _writetext, NULL, "screen" }
};

#endif
