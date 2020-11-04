#ifndef UNARCHIVER_H
#define UNARCHIVER_h

#include <stdio.h>
#include <stdlib.h>
#include "haffman_tree_restorer.h"

int haffman_unarchivate(FILE *in, FILE *out, int offset, int size);

#endif