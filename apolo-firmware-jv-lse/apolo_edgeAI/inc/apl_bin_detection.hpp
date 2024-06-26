#ifndef APL_BIN_DETECTION_H
#define APL_BIN_DETECTION_H

#include <vector>
#include "core/edgelab.h"

typedef struct
{
    int x;
    int y;
    int w;
    int h;
    int target;
    int score;
}box_results_t;

void apl_eval_bin_img(el_img_t *img, std::vector<box_results_t> &result_list);

#endif
