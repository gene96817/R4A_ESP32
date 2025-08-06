
#ifndef FOUR_GENES_LINE_FOLLOWING_H
#define FOUR_GENES_LINE_FOLLOWING_H

extern float inchderate;
// derate factor to the speeds in the state table

// State action structure
typedef struct _StateAction {
    int speed;
    int direction;
    const char* comment;
} StateAction;

#endif // FOUR_GENES_LINE_FOLLOWING_H