/*
 * globals.h: global parser state
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

#define STACKSIZE 16

#define BASE_APP_C \
  "/*\n" \
  " * app.c: Built with tml\n" \
  " */\n" \
  "#include \"tuibox.h\"\n" \
  "#include \"tml_events.h\"\n" \
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
  "  ui_draw(&u);\n" \
  "\n" \
  "  ui_loop(&u){\n" \
  "    ui_update(&u);\n" \
  "  }\n" \
  "\n" \
  "  return 0;\n" \
  "}"

#define BASE_EVENTS_H \
  "/*\n" \
  " * tml_events.h: Built with tml\n" \
  " */\n" \
  "#ifndef __EVENTS_H\n" \
  "#define __EVENTS_H\n\n"
#define END_EVENTS_H "#endif"

typedef void (*func)();
struct tml_tag_t {
  char *name;
  func init;
  func write;
  char *parent;
};
struct tml_attr_t {
  char *name;
  func assign;
};
struct tml_node_t {
  char *id;
  int x, y, w, h;
  char data[256];
  int click;
  int hover;
  struct tml_tag_t *tag;
};
struct tml_color_t {
  char *name;
  char *ansi;
};

int g_screen = -1;
int g_sind = 0;
char *g_expected = NULL;
struct tml_node_t *g_stack[STACKSIZE];

void _writebox(char *out_c, char *out_e, struct tml_node_t *node){
  int len = 0;
  char tmp_click[64],
       tmp_hover[64];

  if(node->w == 1 && node->h == 1){
    printf("Warning: Box with id \"%s\" has no width or height property and will use the default value of 1, this probably is unintended.\n", node->id);
  }

  sprintf(tmp_click, "box_%s_click", node->id);
  sprintf(tmp_hover, "box_%s_hover", node->id);

  sprintf(
    out_c,
    "  ui_add(\n" \
    "    %i, %i,\n" \
    "    %i, %i,\n" \
    "    %i,\n" \
    "    NULL, 0,\n" \
    "    box_%s_draw,\n" \
    "    %s,\n" \
    "    %s,\n" \
    "    NULL, NULL,\n" \
    "    &u\n" \
    "  );\n\n",
    node->x, node->y,
    node->w, node->h,
    g_screen,
    node->id,
    node->click ? tmp_click : "NULL",
    node->hover ? tmp_hover : "NULL"
  );
  if(node->data[0] == '\0'){
    len = sprintf(
      out_e,
      "/* ========== Box ID: %s ========== */\n" \
      "void box_%s_draw(ui_box_t *b, char *out){\n" \
      "  /* TODO: This snippet was generated by tml */\n" \
      "}\n\n",
      node->id,
      node->id
    );
  } else {
    len = sprintf(
      out_e,
      "/* ========== Box ID: %s ========== */\n" \
      "void box_%s_draw(ui_box_t *b, char *out){\n" \
      "  int x, y;\n" \
      "  strcpy(out, \"%s\");\n" \
      "  for(y=0;y<b->h;y++){\n" \
      "    for(x=0;x<b->w;x++){\n" \
      "      strcat(out, \" \");\n" \
      "    }\n" \
      "    strcat(out, \"\\n\");\n" \
      "  }\n" \
      "  strcat(out, \"\\x1b[0m\");\n" \
      "}\n\n",
      node->id,
      node->id,
      node->data
    );
  }
  if(node->click){
    len += sprintf(
      out_e + len,
      "void %s(ui_box_t *b, int x, int y){\n" \
      "  /* TODO: This snippet was generated by tml */\n" \
      "}\n\n",
      tmp_click
    );
  }
  if(node->hover){
    sprintf(
      out_e + len,
      "void %s(ui_box_t *b, int x, int y, int down){\n" \
      "  /* TODO: This snippet was generated by tml */\n" \
      "}\n\n",
      tmp_hover
    );
  }
}

void _newtext(struct tml_node_t *node, char *start, int len){
  strncpy(node->data, start, len);
  node->data[len - 1] = '\0';
}
void _writetext(char *out_c, char *out_e, struct tml_node_t *node){
  int len = 0;
  char tmp_click[64],
       tmp_hover[64];

  sprintf(tmp_click, "text_%s_click", node->id);
  sprintf(tmp_hover, "text_%s_hover", node->id);

  sprintf(
    out_c,
    "  ui_text(\n" \
    "    %i, %i,\n" \
    "    \"%s\",\n" \
    "    %i,\n" \
    "    %s, %s,\n" \
    "    &u\n" \
    "  );\n\n",
    node->x, node->y,
    node->data,
    g_screen,
    node->click ? tmp_click : "NULL",
    node->hover ? tmp_hover : "NULL"
  );
  if(node->click || node->hover){
    len = sprintf(
      out_e,
      "/* ========== Text ID: %s ========== */\n",
      node->id
    );
  }
  if(node->click){
    len += sprintf(
      out_e + len,
      "void %s(ui_box_t *b, int x, int y){\n" \
      "  /* TODO: This snippet was generated by tml */\n" \
      "}\n\n",
      tmp_click
    );
  }
  if(node->hover){
    sprintf(
      out_e + len,
      "void %s(ui_box_t *b, int x, int y, int down){\n" \
      "  /* TODO: This snippet was generated by tml */\n" \
      "}\n\n",
      tmp_hover
    );
  }
}

void _setscreen(){
  g_screen++;
}

void _newcenter(struct tml_node_t *node, char *start, int len){
  node->x = -1;
  node->y = -1;
}

struct tml_tag_t tags[] = {
  { "ui",     NULL,       NULL,       NULL },
  { "screen", _setscreen, NULL,       "ui" },
  { "box",    NULL,       _writebox,  "screen" },
  { "text",   _newtext,   _writetext, "screen" },
  { "center", _newcenter,  _writebox,  "screen" }
};

struct tml_color_t colors[] = {
  { "red", "255;0;0m" },
  { "green", "0;255;0m" },
  { "blue", "0;0;255m" },
};

void _id(struct tml_node_t *node, char *val){
  int len = strlen(val);

  free(node->id);
  node->id = malloc(len);
  strncpy(node->id, val, len);
}
void _clickable(struct tml_node_t *node, char *val){
  node->click = 1;
}
void _hoverable(struct tml_node_t *node, char *val){
  node->hover = 1;
}
void _x(struct tml_node_t *node, char *val){
  node->x = atoi(val);
}
void _y(struct tml_node_t *node, char *val){
  node->y = atoi(val);
}
void _width(struct tml_node_t *node, char *val){
  node->w = atoi(val);
}
void _height(struct tml_node_t *node, char *val){
  node->h = atoi(val);
}
void _copycolor(struct tml_node_t *node, char *val, char ground){
  int i, len;
  char tmp[64];

  for(i=0;i<LENGTH(colors);i++){
    if(strcmp(val, colors[i].name) == 0){
      len = sprintf(tmp, "\\x1b[%c8;2;%s", ground, colors[i].ansi);
      memmove(node->data + len, node->data, strlen(node->data) + 1);
      memcpy(node->data, tmp, len);
      return;
    }
  }
  printf("Warning: Unrecognized color name \"%s\".\n", val);
}
void _background(struct tml_node_t *node, char *val){
  _copycolor(node, val, '4');
}
void _color(struct tml_node_t *node, char *val){
  _copycolor(node, val, '3');
}
void _center(struct tml_node_t *node, char *val){
  node->x = -1;
  node->y = -1;
}

struct tml_attr_t attrs[] = {
  { "id", _id },
  { "clickable", _clickable },
  { "hoverable", _hoverable },
  { "x", _x },
  { "y", _y },
  { "width", _width },
  { "height", _height },
  { "background", _background },
  { "color", _color },
  { "center", _center }
};

#endif
