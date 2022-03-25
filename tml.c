/*
 * tml.c: terminal markup language
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

enum parser_errors {
  ERR_START = -1,
  ERR_END = -2,
  ERR_NAME = -3,
  ERR_PARENT = -4
};

int tag(char *start){
  int i, j;

  for(i=0;i<LENGTH(tags);i++){
    if(strncmp(start, tags[i].name, strlen(tags[i].name)) == 0){
      for(j=0;j<=g_sind;j++){
        if((g_stack[j] == NULL && tags[i].parent == NULL) ||
           (g_stack[j] != NULL && strcmp(g_stack[j]->name, tags[i].parent) == 0)){
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
    default:
      printf("Error: Failed to parse file.\n");
      break;
  }
  exit(cause);
}

int main(int argc, char **argv){
  FILE *fp, *fpe;
  char *buf, *c, *d, *e, out[256];
  size_t l;
  int tmp;

  if(argc < 2){
    printf("Usage: tml [markup.tml]\n");
    return 0;
  }

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

  d = buf;
  for(;;){
    /* Find next start of tag */
    c = strchr(d, '<');
    if(!c){
      if(g_sind == 0) goto write;
      else fail(ERR_START);
    }

    /* Determine if it is an end tag */
    if(*(c + 1) == '/'){
      if(g_sind == 0) fail(ERR_START);

      g_expected = g_stack[g_sind-1]->name;
      c++;
    }
    
    /* Find corresponding end */
    d = strchr(c, '>');
    if(!d) fail(ERR_END);

    /* Find next space to isolate name */
    e = strchr(c, ' ');

    /* Copy tag name to stack */
    if(g_expected != NULL){
      if(strncmp(c + 1, g_expected, strlen(g_expected)) == 0){
        g_expected = NULL;
        g_sind--;
      } else fail(ERR_END);
    } else {
      tmp = tag(c + 1);
      if(tmp < 0) fail(tmp);
      
      g_stack[g_sind++] = &tags[tmp];
      if(tags[tmp].write != NULL){
        tags[tmp].write(out, tags[tmp]);
        fwrite(
          out,
          strlen(out),
          1,
          fp
        );
      }
      if(tags[tmp].onload != NULL){
        tags[tmp].onload();
      }
    }

    /* Parse attributes */
    /* TODO */
  }

write:;
  fwrite(END_APP_C, strlen(END_APP_C), 1, fp);
  fwrite(END_EVENTS_H, strlen(END_EVENTS_H), 1, fpe);

  fclose(fp);
  fclose(fpe);

  printf("Successfully parsed \"%s\".\n  Please edit \"tml_events.h\" to add event handlers.\n", argv[1]);

  return 0;
}
