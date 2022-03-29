/*
 * tml.c: terminal markup language
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "globals.h"

enum parser_errors {
  ERR_START = -1,
  ERR_END = -2,
  ERR_NAME = -3,
  ERR_PARENT = -4,
  ERR_ATTR = -5
};

char *id(){
  char *alpha = "abcdefghijklmnopqrstuvwxyz",
       *out = malloc(8 + 1);
  int i;

  for(i=0;i<8;i++){
    out[i] = alpha[rand() % 26];
  }
  out[8] = '\0';

  return out;
}

int tag(char *start){
  int i, j;

  for(i=0;i<LENGTH(tags);i++){
    if(strncmp(start, tags[i].name, strlen(tags[i].name)) == 0){
      for(j=0;j<=g_sind;j++){
        if((g_stack[j] == NULL && tags[i].parent == NULL) ||
           (g_stack[j] != NULL && strcmp(g_stack[j]->tag->name, tags[i].parent) == 0)){
          return i;
        }
      }
      return ERR_PARENT;
    }
  }
  return ERR_NAME;
}

void fail(int cause){
  switch(cause){
    case ERR_START:
      printf("Error: Start of tag expected.\n");
      break;
    case ERR_END:
      printf("Error: End of tag expected.\n");
      break;
    case ERR_NAME:
      printf("Error: Invalid tag name.\n");
      break;
    case ERR_PARENT:
      printf("Error: Invalid tag nesting.\n");
      break;
    case ERR_ATTR:
      printf("Error: Unmatched quote in attribute value.\n");
      break;
    default:
      printf("Error: Failed to parse file.\n");
      break;
  }
  exit(cause);
}

int main(int argc, char **argv){
  FILE *fp, *fpe;
  char *buf,
       *open, *close,
       *attr_o, *attr_v, *attr_c,
       *data_c,
       val[16],
       out_c[512], out_e[512];
  size_t l;
  int tmp, i, off;

  if(argc < 2){
    printf("Usage: tml [markup.tml]\n");
    return 0;
  }

  srand(time(NULL));

  fp = fopen(argv[1], "r");
  if(!fp){
    printf("Error: Failed to open file \"%s\" for reading.\n", argv[1]);
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  buf = malloc((l=ftell(fp)));
  fseek(fp, 0, SEEK_SET);
  fread(buf, l, 1, fp);
  fclose(fp);

  fp = fopen("app.c", "w");
  fpe = fopen("tml_events.h", "w");

  fwrite(BASE_APP_C, strlen(BASE_APP_C), 1, fp);
  fwrite(BASE_EVENTS_H, strlen(BASE_EVENTS_H), 1, fpe);

  close = buf;
  for(;;){
    /* Find next start of tag */
    open = strchr(close, '<');
    if(!open){
      if(g_sind == 0) goto write;
      else fail(ERR_START);
    }

    /* Determine if it is an end tag */
    if(*(open + 1) == '/'){
      if(g_sind == 0) fail(ERR_START);

      g_expected = g_stack[g_sind-1]->tag->name;
      open++;
    }
    
    /* Find corresponding end */
    close = strchr(open, '>');
    if(!close) fail(ERR_END);

    /* Copy tag name to stack */
    if(g_expected != NULL){
      if(strncmp(open + 1, g_expected, strlen(g_expected)) == 0){
        g_expected = NULL;
        g_sind--;
        free(g_stack[g_sind]->id);
        free(g_stack[g_sind]);
      } else fail(ERR_END);
    } else {
      tmp = tag(open + 1);
      if(tmp < 0) fail(tmp);

      g_stack[g_sind] = malloc(sizeof(struct tml_node_t));
      g_stack[g_sind]->id = id();
      /* TODO */
      g_stack[g_sind]->x = g_sind == 0 ? 1 : g_stack[g_sind-1]->x;
      g_stack[g_sind]->y = g_sind == 0 ? 1 : g_stack[g_sind-1]->y;
      g_stack[g_sind]->w = 1;
      g_stack[g_sind]->h = 1;
      g_stack[g_sind]->data[0] = '\0';
      g_stack[g_sind]->click = 0;
      g_stack[g_sind]->hover = 0;
      g_stack[g_sind]->tag = &tags[tmp];

      data_c = strchr(close, '<');
      if(data_c == NULL) fail(ERR_END);
      if(tags[tmp].init != NULL) tags[tmp].init(g_stack[g_sind], close + 1, data_c - close);

      /* Parse attributes */
      attr_o = strchr(open, ' ');
      while(attr_o != NULL && attr_o < close){
        attr_c = strchr(attr_o, ' ');
        attr_v = strchr(attr_o, '=');

        for(i=0;i<LENGTH(attrs);i++){
          if(strncmp(attr_o, attrs[i].name, strlen(attrs[i].name)) == 0){
            if(attr_v == NULL || attr_v > close || (attr_c < close && attr_v > attr_c)){
              val[0] = '\1';
              val[1] = '\0';
            } else {
              if(attr_c == NULL || attr_c > close){
                strncpy(val, attr_v + 1, close - attr_v);
                val[close - attr_v - 1] = '\0';
              } else {
                strncpy(val, attr_v + 1, attr_c - attr_v);
                val[attr_c - attr_v - 1] = '\0';
              }
            }
            if(val[0] == '"'){
              off = strlen(val)-1;
              if(val[off] != '"') fail(ERR_ATTR);
              else val[off] = '\0';

              off = 1;
            } else off = 0;
            attrs[i].assign(g_stack[g_sind], val + off);
          }
        }

        attr_o = attr_c + 1;
      }
      
      if(tags[tmp].write != NULL){
        memset(out_c, 0, sizeof(out_c));
        memset(out_e, 0, sizeof(out_e));
        tags[tmp].write(out_c, out_e, g_stack[g_sind]);
        fwrite(
          out_c,
          strlen(out_c),
          1,
          fp
        );
        fwrite(
          out_e,
          strlen(out_e),
          1,
          fpe
        );
      }
      g_sind++;
    }
  }

write:;
  fwrite(END_APP_C, strlen(END_APP_C), 1, fp);
  fwrite(END_EVENTS_H, strlen(END_EVENTS_H), 1, fpe);

  fclose(fp);
  fclose(fpe);

  free(buf);

  printf("Successfully parsed \"%s\".\n  Please edit \"tml_events.h\" to add event handlers,\n  and copy over tuibox.h in order to compile.\n", argv[1]);

  return 0;
}
