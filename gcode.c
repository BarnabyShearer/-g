/*
 * Simple g-code parser.
 * Don't think you can follow all g-code standards simultainusly; this is targeted at Printrun
 *
 * Copyright 2013 <b@Zi.iS>
 * License GPLv2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "gcode.h"
#include "machine.h"

static int lineno;
static double modal_g;

static void readword(char, char*, double*);

void push_line(char* line) {
    if(*line != 'N') {
        if(strchr(line, '*') != 0) {
            puts("Missing line number");
        } else {
            push_block(line);
            return;
        }
   } else {
       if(strtol((char*)line+1, NULL, 10) == -1) {
           lineno = -1;
       }
       if(strtol((char*)line+1, NULL, 10) != lineno) {
           //HACK: this isn't actuall the spec but seems more sensable.
           puts("Wrong line number");
       } else {
           const char* pstar = strchr(line, '*');
           if(pstar<=0) {
              puts("Missing checksum");
           } else {
              int checksum = 0;
              for(char* i=line; i<pstar; i++) {
                  checksum ^= (int)*i;
              }
              if(strtol(&line[pstar - line + 1], NULL, 10) != checksum) {
                  puts("Invalid checksum");
              } else {
                  const char* pspace = strchr(line, ' ');
                  char cmd[pstar - pspace + 1];
                  strncpy(cmd, pspace, pstar - pspace);
                  cmd[pstar - pspace] = 0;
                  push_block(cmd);
                  lineno++;
                  return;
              }
           }
        }
    }
    printf("Resend:%d\n", lineno);
}

void push_block(char* block) {
    int comment = 0;
    double g = modal_g;
    double m = DBL_MIN;
    double z = DBL_MIN;
    double p = DBL_MIN;
    for ( ; *block; ++block) {
        if(comment) {
            if(*block == ')') {
                comment = 0;
            }
            continue;
        }
        if(*block == '(') {
            comment = 1;
            continue;
        }
        if(*block == '/' || *block == ';') {
            break;
        }
        readword('G', block, &g);
        readword('M', block, &g);
        readword('Z', block, &z);
        readword('P', block, &p);
    }
    if(g == 0 || g == 1 || g == 2 || g == 3 || g == 80 || g == 82 ||
        g == 83 || g == 84 || g == 85 || g == 86 || g == 87 || g == 88 ||
        g == 89) {
        modal_g = g;
    }
    if(g == 0 || g == 1) {
        if(z != DBL_MIN) {
            printf("ok will move: G%d Z%f\n", (int)g, z);
        }
    } else if(g == 4) {
        if(p != DBL_MIN) {
            printf("ok Pause: P%f\n", p);
        }
    } else {
        puts("ok (ignored)");
    }
    if(m == 105) {
        puts("ok");
    } else if (m > 0) {
        puts("ok (ignored)");
    }
};

static void readword(char letter, char* block, double* val) {
    char* endptr = NULL;
    if(*block == letter || *block == letter + 32) {
        double value = strtod(block + 1, &endptr);
        if(block + 1 != endptr) {
            *val = value;
        }
    }
}
