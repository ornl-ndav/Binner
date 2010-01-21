#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vcbutils.h"
#include "vcbtrimesh.h"
#include "vcblinalg.h"
#include "dmem.h"
#include "vcbmcube.h"
/*   z
 *   ^  y
 *   |/
 *   ---> x
 *              7 --- 8              
 *            / |   / |
 *           3 --- 4  5    :the hidden vertex is 6
 *           |/    |/
 *           2 --- 1
 *
 *  corresponding to the mc-cases:
 *    0      1       2      3      5
 *    8      12      13     9      11
 *    4      6       7      10     14
 *
 *    15/`5  16/`6   17/`2  18/`7
 *    19/`3  20/`4   21/`1  22/`0
 */
 
/**
 * mcube_table is the table that contains the actual data values for the triangulator in the Marching Cubes algorthim.
 */
static struct MCubeTable mcube_table [256] = {
{0, {1, 2, 3, 4, 5, 6, 7, 8}, 0},
{1, {1, 2, 3, 4, 5, 6, 7, 8}, 25},
{1, {2, 3, 4, 1, 6, 7, 8, 5}, 26},
{2, {1, 2, 3, 4, 5, 6, 7, 8}, 27},
{1, {3, 4, 1, 2, 7, 8, 5, 6}, 22},
{3, {1, 2, 3, 4, 5, 6, 7, 8}, 31},
{2, {2, 3, 4, 1, 6, 7, 8, 5}, 30},
{5, {4, 3, 7, 8, 1, 2, 6, 5}, 31},
{1, {4, 1, 2, 3, 8, 5, 6, 7}, 21},
{2, {4, 1, 2, 3, 8, 5, 6, 7}, 29},
{3, {2, 3, 4, 1, 6, 7, 8, 5}, 31},
{5, {3, 2, 6, 7, 4, 1, 5, 8}, 31},
{2, {3, 4, 1, 2, 7, 8, 5, 6}, 23},
{5, {2, 1, 5, 6, 3, 4, 8, 7}, 31},
{5, {1, 4, 8, 5, 2, 3, 7, 6}, 31},
{8, {3, 2, 6, 7, 4, 1, 5, 8}, 15},
{1, {5, 8, 7, 6, 1, 4, 3, 2}, 41},
{2, {5, 1, 4, 8, 6, 2, 3, 7}, 57},
{3, {5, 6, 2, 1, 8, 7, 3, 4}, 59},
{5, {6, 5, 8, 7, 2, 1, 4, 3}, 59},
{4, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{6, {5, 1, 4, 8, 6, 2, 3, 7}, 63},
{6, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{14, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{3, {5, 1, 4, 8, 6, 2, 3, 7}, 61},
{5, {8, 4, 3, 7, 5, 1, 2, 6}, 61},
{7, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{9, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{6, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{11, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{12, {1, 4, 8, 5, 2, 3, 7, 6}, 63},
{15, {5, 6, 2, 1, 8, 7, 3, 4}, 47},
{1, {6, 7, 3, 2, 5, 8, 4, 1}, 42},
{3, {6, 2, 1, 5, 7, 3, 4, 8}, 59},
{2, {2, 6, 7, 3, 1, 5, 8, 4}, 58},
{5, {5, 1, 4, 8, 6, 2, 3, 7}, 59},
{3, {6, 7, 3, 2, 5, 8, 4, 1}, 62},
{7, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{5, {7, 6, 5, 8, 3, 2, 1, 4}, 62},
{9, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{4, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{6, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{6, {6, 2, 1, 5, 7, 3, 4, 8}, 63},
{11, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{6, {4, 3, 7, 8, 1, 2, 6, 5}, 63},
{12, {2, 1, 5, 6, 3, 4, 8, 7}, 63},
{14, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{15, {6, 7, 3, 2, 5, 8, 4, 1}, 47},
{2, {6, 5, 8, 7, 2, 1, 4, 3}, 43},
{5, {2, 6, 7, 3, 1, 5, 8, 4}, 59},
{5, {1, 2, 3, 4, 5, 6, 7, 8}, 59},
{8, {1, 2, 3, 4, 5, 6, 7, 8}, 51},
{6, {5, 6, 2, 1, 8, 7, 3, 4}, 63},
{12, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{11, {5, 1, 4, 8, 6, 2, 3, 7}, 63},
{15, {3, 4, 1, 2, 7, 8, 5, 6}, 55},
{6, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{14, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{12, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{15, {4, 8, 5, 1, 3, 7, 6, 2}, 55},
{10, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{16, {8, 7, 6, 5, 4, 3, 2, 1}, 63},
{16, {7, 8, 4, 3, 6, 5, 1, 2}, 63},
{17, {8, 7, 6, 5, 4, 3, 2, 1}, 39},
{1, {7, 3, 2, 6, 8, 4, 1, 5}, 38},
{4, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{3, {2, 6, 7, 3, 1, 5, 8, 4}, 62},
{6, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{2, {7, 3, 2, 6, 8, 4, 1, 5}, 54},
{6, {7, 3, 2, 6, 8, 4, 1, 5}, 63},
{5, {6, 2, 1, 5, 7, 3, 4, 8}, 62},
{11, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{3, {7, 8, 4, 3, 6, 5, 1, 2}, 55},
{6, {1, 4, 8, 5, 2, 3, 7, 6}, 63},
{7, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{12, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{5, {8, 7, 6, 5, 4, 3, 2, 1}, 55},
{14, {8, 7, 6, 5, 4, 3, 2, 1}, 63},
{9, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{15, {7, 8, 4, 3, 6, 5, 1, 2}, 47},
{3, {5, 8, 7, 6, 1, 4, 3, 2}, 47},
{6, {1, 5, 6, 2, 4, 8, 7, 3}, 63},
{7, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{12, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{6, {3, 7, 8, 4, 2, 6, 5, 1}, 63},
{10, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{12, {6, 2, 1, 5, 7, 3, 4, 8}, 63},
{16, {4, 8, 5, 1, 3, 7, 6, 2}, 63},
{7, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{12, {8, 4, 3, 7, 5, 1, 2, 6}, 63},
{13, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{18, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{12, {8, 7, 6, 5, 4, 3, 2, 1}, 63},
{16, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{18, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{19, {6, 5, 8, 7, 2, 1, 4, 3}, 47},
{2, {6, 7, 3, 2, 5, 8, 4, 1}, 46},
{6, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{5, {3, 7, 8, 4, 2, 6, 5, 1}, 62},
{14, {5, 1, 4, 8, 6, 2, 3, 7}, 63},
{5, {2, 3, 4, 1, 6, 7, 8, 5}, 62},
{12, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{8, {2, 3, 4, 1, 6, 7, 8, 5}, 60},
{15, {1, 5, 6, 2, 4, 8, 7, 3}, 61},
{6, {6, 7, 3, 2, 5, 8, 4, 1}, 63},
{10, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{12, {3, 7, 8, 4, 2, 6, 5, 1}, 63},
{16, {5, 8, 7, 6, 1, 4, 3, 2}, 63},
{11, {4, 8, 5, 1, 3, 7, 6, 2}, 63},
{16, {8, 5, 1, 4, 7, 6, 2, 3}, 63},
{15, {4, 1, 2, 3, 8, 5, 6, 7}, 61},
{17, {5, 8, 7, 6, 1, 4, 3, 2}, 45},
{5, {8, 5, 1, 4, 7, 6, 2, 3}, 47},
{11, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{9, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{15, {7, 3, 2, 6, 8, 4, 1, 5}, 55},
{14, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{16, {8, 4, 3, 7, 5, 1, 2, 6}, 63},
{15, {5, 8, 7, 6, 1, 4, 3, 2}, 61},
{17, {4, 8, 5, 1, 3, 7, 6, 2}, 53},
{12, {8, 5, 1, 4, 7, 6, 2, 3}, 63},
{16, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{18, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{19, {8, 4, 3, 7, 5, 1, 2, 6}, 55},
{16, {2, 1, 5, 6, 3, 4, 8, 7}, 63},
{20, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{19, {1, 4, 8, 5, 2, 3, 7, 6}, 61},
{21, {8, 7, 6, 5, 4, 3, 2, 1}, 37},
{1, {8, 7, 6, 5, 4, 3, 2, 1}, 37},
{3, {1, 4, 8, 5, 2, 3, 7, 6}, 61},
{4, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{6, {2, 1, 5, 6, 3, 4, 8, 7}, 63},
{3, {8, 4, 3, 7, 5, 1, 2, 6}, 55},
{7, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{6, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{12, {4, 3, 7, 8, 1, 2, 6, 5}, 63},
{2, {4, 8, 5, 1, 3, 7, 6, 2}, 53},
{5, {5, 8, 7, 6, 1, 4, 3, 2}, 61},
{6, {8, 4, 3, 7, 5, 1, 2, 6}, 63},
{14, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{5, {7, 3, 2, 6, 8, 4, 1, 5}, 55},
{9, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{11, {8, 7, 6, 5, 4, 3, 2, 1}, 63},
{15, {8, 5, 1, 4, 7, 6, 2, 3}, 47},
{2, {5, 8, 7, 6, 1, 4, 3, 2}, 45},
{5, {4, 1, 2, 3, 8, 5, 6, 7}, 61},
{6, {8, 5, 1, 4, 7, 6, 2, 3}, 63},
{11, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{6, {5, 8, 7, 6, 1, 4, 3, 2}, 63},
{12, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{10, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{16, {6, 7, 3, 2, 5, 8, 4, 1}, 63},
{5, {1, 5, 6, 2, 4, 8, 7, 3}, 61},
{8, {4, 1, 2, 3, 8, 5, 6, 7}, 60},
{12, {1, 5, 6, 2, 4, 8, 7, 3}, 63},
{15, {2, 3, 4, 1, 6, 7, 8, 5}, 62},
{14, {7, 3, 2, 6, 8, 4, 1, 5}, 63},
{15, {3, 7, 8, 4, 2, 6, 5, 1}, 62},
{16, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{17, {6, 7, 3, 2, 5, 8, 4, 1}, 46},
{3, {6, 5, 8, 7, 2, 1, 4, 3}, 47},
{7, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{6, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{12, {5, 1, 4, 8, 6, 2, 3, 7}, 63},
{7, {2, 6, 7, 3, 1, 5, 8, 4}, 63},
{13, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{12, {7, 6, 5, 8, 3, 2, 1, 4}, 63},
{18, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{6, {4, 8, 5, 1, 3, 7, 6, 2}, 63},
{12, {5, 8, 7, 6, 1, 4, 3, 2}, 63},
{10, {6, 7, 3, 2, 5, 8, 4, 1}, 63},
{16, {3, 7, 8, 4, 2, 6, 5, 1}, 63},
{12, {7, 3, 2, 6, 8, 4, 1, 5}, 63},
{18, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{16, {1, 5, 6, 2, 4, 8, 7, 3}, 63},
{19, {5, 8, 7, 6, 1, 4, 3, 2}, 47},
{5, {7, 8, 4, 3, 6, 5, 1, 2}, 47},
{9, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{14, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{15, {8, 7, 6, 5, 4, 3, 2, 1}, 55},
{12, {7, 8, 4, 3, 6, 5, 1, 2}, 63},
{18, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{16, {1, 4, 8, 5, 2, 3, 7, 6}, 63},
{19, {7, 8, 4, 3, 6, 5, 1, 2}, 55},
{11, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{15, {6, 2, 1, 5, 7, 3, 4, 8}, 62},
{16, {7, 3, 2, 6, 8, 4, 1, 5}, 63},
{17, {7, 3, 2, 6, 8, 4, 1, 5}, 54},
{16, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{19, {2, 6, 7, 3, 1, 5, 8, 4}, 62},
{20, {1, 2, 3, 4, 5, 6, 7, 8}, 63},
{21, {7, 3, 2, 6, 8, 4, 1, 5}, 38},
{2, {8, 7, 6, 5, 4, 3, 2, 1}, 39},
{6, {7, 8, 4, 3, 6, 5, 1, 2}, 63},
{6, {8, 7, 6, 5, 4, 3, 2, 1}, 63},
{10, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{5, {4, 8, 5, 1, 3, 7, 6, 2}, 55},
{12, {4, 8, 5, 1, 3, 7, 6, 2}, 63},
{14, {4, 8, 5, 1, 3, 7, 6, 2}, 63},
{16, {6, 5, 8, 7, 2, 1, 4, 3}, 63},
{5, {3, 4, 1, 2, 7, 8, 5, 6}, 55},
{11, {7, 3, 2, 6, 8, 4, 1, 5}, 63},
{12, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{16, {5, 6, 2, 1, 8, 7, 3, 4}, 63},
{8, {3, 4, 1, 2, 7, 8, 5, 6}, 51},
{15, {1, 2, 3, 4, 5, 6, 7, 8}, 59},
{15, {2, 6, 7, 3, 1, 5, 8, 4}, 59},
{17, {6, 5, 8, 7, 2, 1, 4, 3}, 43},
{5, {6, 7, 3, 2, 5, 8, 4, 1}, 47},
{14, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{12, {6, 7, 3, 2, 5, 8, 4, 1}, 63},
{16, {4, 3, 7, 8, 1, 2, 6, 5}, 63},
{11, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{16, {6, 2, 1, 5, 7, 3, 4, 8}, 63},
{16, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{20, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{9, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{15, {7, 6, 5, 8, 3, 2, 1, 4}, 62},
{18, {4, 1, 2, 3, 8, 5, 6, 7}, 63},
{19, {6, 7, 3, 2, 5, 8, 4, 1}, 62},
{15, {5, 1, 4, 8, 6, 2, 3, 7}, 59},
{17, {2, 6, 7, 3, 1, 5, 8, 4}, 58},
{19, {6, 2, 1, 5, 7, 3, 4, 8}, 59},
{21, {6, 7, 3, 2, 5, 8, 4, 1}, 42},
{5, {5, 6, 2, 1, 8, 7, 3, 4}, 47},
{12, {5, 6, 2, 1, 8, 7, 3, 4}, 63},
{11, {2, 3, 4, 1, 6, 7, 8, 5}, 63},
{16, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{9, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{18, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{15, {8, 4, 3, 7, 5, 1, 2, 6}, 61},
{19, {5, 1, 4, 8, 6, 2, 3, 7}, 61},
{14, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{16, {3, 2, 6, 7, 4, 1, 5, 8}, 63},
{16, {5, 1, 4, 8, 6, 2, 3, 7}, 63},
{20, {3, 4, 1, 2, 7, 8, 5, 6}, 63},
{15, {6, 5, 8, 7, 2, 1, 4, 3}, 59},
{19, {5, 6, 2, 1, 8, 7, 3, 4}, 59},
{17, {5, 1, 4, 8, 6, 2, 3, 7}, 57},
{21, {5, 8, 7, 6, 1, 4, 3, 2}, 41},
{8, {6, 7, 3, 2, 5, 8, 4, 1}, 15},
{15, {1, 4, 8, 5, 2, 3, 7, 6}, 31},
{15, {2, 1, 5, 6, 3, 4, 8, 7}, 31},
{17, {3, 4, 1, 2, 7, 8, 5, 6}, 23},
{15, {3, 2, 6, 7, 4, 1, 5, 8}, 31},
{19, {2, 3, 4, 1, 6, 7, 8, 5}, 31},
{17, {4, 1, 2, 3, 8, 5, 6, 7}, 29},
{21, {4, 1, 2, 3, 8, 5, 6, 7}, 21},
{15, {4, 3, 7, 8, 1, 2, 6, 5}, 31},
{17, {2, 3, 4, 1, 6, 7, 8, 5}, 30},
{19, {1, 2, 3, 4, 5, 6, 7, 8}, 31},
{21, {3, 4, 1, 2, 7, 8, 5, 6}, 22},
{17, {1, 2, 3, 4, 5, 6, 7, 8}, 27},
{21, {2, 3, 4, 1, 6, 7, 8, 5}, 26},
{21, {1, 2, 3, 4, 5, 6, 7, 8}, 25},
{22, {1, 2, 3, 4, 5, 6, 7, 8}, 0},
};

static int        vcb_off[9], xdim, ydim, zdim;
static void *     block;
static vcbdatatype vt_dtype;
static float      vt_isoval;
static float      oval[9*4]; /* nn only uses the first 9 values, wn use all 36 */
static void       (*graboval) (int);

void graboval_ub(int idx)
{
	static unsigned char * ubptr;
	int i;
	ubptr = (unsigned char *)block;

	for (i = 1; i <= 8; i ++)
		oval[i] = ubptr[idx+vcb_off[i]] - vt_isoval;
}

void graboval_s(int idx)
{
	static short * sptr;
	int i;
	sptr = (short *)block;

	for (i = 1; i <= 8; i ++)
		oval[i] = sptr[idx+vcb_off[i]] - vt_isoval;
}

void graboval_i(int idx)
{
	static int * iptr;
	int i;
	iptr = (int *) block;

	for (i = 1; i <= 8; i ++)
		oval[i] = iptr[idx+vcb_off[i]] - vt_isoval;
}

void graboval_f(int idx)
{
	static float * fptr;
	int i;
	fptr = (float *) block;

	for (i = 1; i <= 8; i ++)
		oval[i] = fptr[idx+vcb_off[i]] - vt_isoval;
}

void grabovaln_ub(int idx)
{
	static unsigned char * ubptr;
	static char * bptr;
	int i;
	ubptr = (unsigned char *)block;
	bptr = (char *) block;

	for (i = 1; i <= 8; i ++) {
		oval[i*4+0] = ubptr[(idx+vcb_off[i])*4] - vt_isoval;
		oval[i*4+1] = (float)bptr[(idx+vcb_off[i])*4+1];
		oval[i*4+2] = (float)bptr[(idx+vcb_off[i])*4+2];
		oval[i*4+3] = (float)bptr[(idx+vcb_off[i])*4+3];
	}
}

void grabovaln_s(int idx)
{
	static short * sptr;
	int i;
	sptr = (short *) block;

	for (i = 1; i <= 8; i ++) {
		oval[i*4+0] = sptr[(idx+vcb_off[i])*4] - vt_isoval;
		oval[i*4+1] = (float)sptr[(idx+vcb_off[i])*4+1];
		oval[i*4+2] = (float)sptr[(idx+vcb_off[i])*4+2];
		oval[i*4+3] = (float)sptr[(idx+vcb_off[i])*4+3];
	}
}

void grabovaln_i(int idx)
{
	static int * iptr;
	int i;
	iptr = (int *) block;

	for (i = 1; i <= 8; i ++) {
		oval[i*4+0] = iptr[(idx+vcb_off[i])*4] - vt_isoval;
		oval[i*4+1] = (float)iptr[(idx+vcb_off[i])*4+1];
		oval[i*4+2] = (float)iptr[(idx+vcb_off[i])*4+2];
		oval[i*4+3] = (float)iptr[(idx+vcb_off[i])*4+3];
	}
}

void grabovaln_f(int idx)
{
	static float * fptr;
	int i;
	fptr = (float *) block;

	for (i = 1; i <= 8; i ++) {
		oval[i*4+0] = fptr[(idx+vcb_off[i])*4] - vt_isoval;
		oval[i*4+1] = fptr[(idx+vcb_off[i])*4+1];
		oval[i*4+2] = fptr[(idx+vcb_off[i])*4+2];
		oval[i*4+3] = fptr[(idx+vcb_off[i])*4+3];
	}
}

void vcbMCVertexBegin(vcbdatatype datatype, void * data, int *dsizes, float isoval, int wnormal)
{
	vt_dtype  = datatype;
	block     = data;
	vt_isoval = isoval;

	xdim      = dsizes[0];
	ydim      = dsizes[1];
	zdim      = dsizes[2];

    vcb_off[1] = 0;                   /* [i,   j,   k]   */
	vcb_off[2] = 1;                   /* [i,   j,   k+1] */
	vcb_off[3] = zdim+1;              /* [i,   j+1, k+1] */
	vcb_off[4] = zdim;                /* [i,   j+1, k]   */
	vcb_off[5] = ydim*zdim;           /* [i+1, j,   k]   */
	vcb_off[6] = vcb_off[5]+vcb_off[2]; /* [i+1, j,   k+1] */
	vcb_off[7] = vcb_off[5]+vcb_off[3]; /* [i+1, j+1, k+1] */
	vcb_off[8] = vcb_off[5]+vcb_off[4]; /* [i+1, j+1, k]   */

	switch (datatype) {
		case VCB_UNSIGNEDBYTE: 
		case VCB_BYTE :
			graboval = (wnormal == VCB_ONLYVERTEX) ? graboval_ub : grabovaln_ub;
			break;
		case VCB_UNSIGNEDSHORT:
		case VCB_SHORT:
			graboval = (wnormal == VCB_ONLYVERTEX) ? graboval_s : grabovaln_s;
			break;
		case VCB_UNSIGNEDINT:
		case VCB_INT:
		case VCB_BITFIELD:
			graboval = (wnormal == VCB_ONLYVERTEX) ? graboval_i : grabovaln_i;
			break;
		case VCB_FLOAT:
		case VCB_DOUBLE:
			graboval = (wnormal == VCB_ONLYVERTEX) ? graboval_f : grabovaln_f;
			break;
	}
}

void vcbMCVertexEnd()
{
	/* haven't thought of what to do here yet */
}

#ifndef vcb_tabulate
#define vcb_tabulate(ov,x,y,z) \
	ov[1][0] = x;     ov[1][1] = y;     ov[1][2] = z;     \
	ov[2][0] = x;     ov[2][1] = y;     ov[2][2] = z+1.f; \
	ov[3][0] = x;     ov[3][1] = y+1.f; ov[3][2] = z+1.f; \
	ov[4][0] = x;     ov[4][1] = y+1.f; ov[4][2] = z;     \
	ov[5][0] = x+1.f; ov[5][1] = y;     ov[5][2] = z;     \
	ov[6][0] = x+1.f; ov[6][1] = y;     ov[6][2] = z+1.f; \
	ov[7][0] = x+1.f; ov[7][1] = y+1.f; ov[7][2] = z+1.f; \
	ov[8][0] = x+1.f; ov[8][1] = y+1.f; ov[8][2] = z
#endif

#ifndef vcb_cal_f
#define vcb_cal_f(i,j,k) \
			weight = val[i]/(val[i]-val[j]); onemwe = 1.f - weight; \
			f[k*3+0] = v[i][0] * onemwe + v[j][0] * weight;             \
			f[k*3+1] = v[i][1] * onemwe + v[j][1] * weight;             \
			f[k*3+2] = v[i][2] * onemwe + v[j][2] * weight
#endif

#ifndef vcb_ass_f
#define vcb_ass_f(i,j) \
	        f[j*3 + 0] = f[i*3 + 0];  \
	        f[j*3 + 1] = f[i*3 + 1];  \
	        f[j*3 + 2] = f[i*3 + 2]
#endif

int  vcbMCVNoNormals(int x, int y, int z, float * f)
/*
 * marching cube isosurface extraction, no normal is generated 
 * return value: number of triangles extracted
 *
 * f must pointer to at least 12*3 floats
 */
{
	static float ov[9][3], val[9], v[9][3];
	float weight, onemwe, fx, fy, fz;
	int i, indx, mask, wcase, no_faces;
	vcbMCT tab;

	indx = (x * ydim + y)*zdim + z;
	graboval(indx);

	for(i=1, mask = 0; i<=8; i++)
		if(oval[i]<0) mask |= 1<<(i-1);

	tab = & mcube_table[mask];
	wcase=tab->which_case;

	if ((wcase<1)||(wcase>22)) return 0;

	fx = (float)x; fy = (float)y; fz = (float)z;

	vcb_tabulate(ov,fx,fy,fz);

	/* begin extraction */
	for(indx = 1; indx <= 8; indx ++ ) {
		val[indx]= oval[tab->permute[indx-1]];

		for (i = 0; i < 3; i ++)
			v[indx][i] = ov[tab->permute[indx-1]][i];
	}

	switch(wcase){
		case 0:
			no_faces = 0;
			break; /* no triangles */
		case 1:

			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(1,5,1);
			vcb_cal_f(1,4,2);
			no_faces = 1; 
			break;
		case 2:
			/* 1, 2, 3 */
			vcb_cal_f(1,5,0);
			vcb_cal_f(2,6,1);
			vcb_cal_f(2,3,2);
			/* 3, 4, 1*/
			vcb_ass_f(2,3);
			vcb_cal_f(1,4,4);
			vcb_ass_f(0,5);

			no_faces = 2; 
			break;
		case 3:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(1,5,1);
			vcb_cal_f(1,4,2);
			/* 4, 5, 6 */
			vcb_cal_f(3,2,3);
			vcb_cal_f(3,7,4);
			vcb_cal_f(3,4,5);

			no_faces = 2; 
			break;
		case 4:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(1,5,1);
			vcb_cal_f(1,4,2);
			/* 4, 5, 6 */
			vcb_cal_f(7,3,3);
			vcb_cal_f(7,8,4);
			vcb_cal_f(7,6,5);

			no_faces = 2; 
			break;
		case 5:
			/* 1, 2, 3 */
			vcb_cal_f(2,1,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(5,1,2);
			/* 4, 3, 2 */
			vcb_cal_f(5,8,3);
			vcb_ass_f(2,4);
			vcb_ass_f(1,5);
			/* 5, 4, 2 */
			vcb_cal_f(6,7,6);
			vcb_ass_f(3,7);
			vcb_ass_f(1,8);

			no_faces = 3; 
			break;
		case 6:
			/* 1, 2, 3 */
			vcb_cal_f(1,5,0);
			vcb_cal_f(2,6,1);
			vcb_cal_f(2,3,2);
			/* 3, 4, 1 */
			vcb_ass_f(2,3);
			vcb_cal_f(1,4,4);
			vcb_ass_f(0,5);
			/* 5, 6, 7 */
			vcb_cal_f(7,3,6);
			vcb_cal_f(7,8,7);
			vcb_cal_f(7,6,8);

			no_faces = 3; 
			break;
		case 7:
			/* 1, 2, 3 */
			vcb_cal_f(2,1,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(2,6,2);
			/* 4, 5, 6 */
			vcb_cal_f(4,1,3);
			vcb_cal_f(4,3,4);
			vcb_cal_f(4,8,5);
			/* 7, 8, 9 */
			vcb_cal_f(7,8,6);
			vcb_cal_f(7,6,7);
			vcb_cal_f(7,3,8);

			no_faces = 3; 
			break;
		case 8:
			/* 1, 2, 3 */
			vcb_cal_f(1,4,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(6,7,2);
			/* 4, 1, 3 */
			vcb_cal_f(5,8,3);
			vcb_ass_f(0,4);
			vcb_ass_f(2,5);

			no_faces = 2; 
			break;
		case 9:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(6,7,1);
			vcb_cal_f(6,2,2);
			/* 1, 3, 4 */
			vcb_ass_f(0,3);
			vcb_cal_f(8,7,4);
			vcb_ass_f(1,5);
			/* 1, 4, 5 */
			vcb_ass_f(0,6);
			vcb_cal_f(1,4,7);
			vcb_ass_f(4,8);
			/* 5, 4, 6*/
			vcb_ass_f(7,9);
			vcb_cal_f(8,4,10);
			vcb_ass_f(4,11);
		
			no_faces = 4; 
			break;
		case 10:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(4,3,1);
			vcb_cal_f(1,5,2);
			/* 2, 4, 3 */
			vcb_ass_f(1,3);
			vcb_cal_f(4,8,4);
			vcb_ass_f(2,5);
			/* 5, 6, 7 */
			vcb_cal_f(6,2,6);
			vcb_cal_f(6,5,7);
			vcb_cal_f(7,3,8);
			/* 8, 7, 9*/
			vcb_ass_f(8,9);
			vcb_ass_f(7,10);
			vcb_cal_f(7,8,11);
			
			no_faces = 4; 
			break;
		case 11:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(6,2,1);
			vcb_cal_f(7,3,2);
			/* 1, 3, 4 */
			vcb_ass_f(0,3);
			vcb_ass_f(2,4);
			vcb_cal_f(5,8,5);
			/* 1, 4, 5 */
			vcb_ass_f(0,6);
			vcb_ass_f(5,7);
			vcb_cal_f(1,4,8);
			/* 4, 3, 6*/
			vcb_ass_f(5,9);
			vcb_ass_f(2,10);
			vcb_cal_f(7,8,11);

			no_faces = 4; 
			break;
		case 12:
			/* 1, 2, 3 */
			vcb_cal_f(2,1,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(5,1,2);
			/* 3, 2, 4 */
			vcb_ass_f(2,3);
			vcb_ass_f(1,4);
			vcb_cal_f(5,8,5);
			/* 4, 2, 5 */
			vcb_ass_f(5,6);
			vcb_ass_f(1,7);
			vcb_cal_f(6,7,8);
			/* 6, 7, 8*/
			vcb_cal_f(4,1,9);
			vcb_cal_f(4,3,10);
			vcb_cal_f(4,8,11);

			no_faces = 4; 
			break;
		case 13:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(1,5,1);
			vcb_cal_f(1,4,2);
			/* 4, 5, 6 */
			vcb_cal_f(3,2,3);
			vcb_cal_f(3,4,5);
			vcb_cal_f(3,7,4);
			/* 7, 8, 9 */
			vcb_cal_f(6,2,6);
			vcb_cal_f(6,7,7);
			vcb_cal_f(6,5,8);
			/* 10, 11, 12*/
			vcb_cal_f(8,5,9);
			vcb_cal_f(8,7,10);
			vcb_cal_f(8,4,11);

			no_faces = 4; 
			break;
		case 14:
			/* 1, 2, 3 */
			vcb_cal_f(2,1,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(6,7,2);
			/* 1, 3, 4 */
			vcb_ass_f(0,3);
			vcb_ass_f(2,4);
			vcb_cal_f(8,4,5);
			/* 1, 4, 5 */
			vcb_ass_f(0,6);
			vcb_ass_f(5,7);
			vcb_cal_f(5,1,8);
			/* 3, 6, 4*/
			vcb_ass_f(2,9);
			vcb_cal_f(8,7,10);
			vcb_ass_f(5,11);

			no_faces = 4; 
			break;
		
		case 15:
			/* 1, 2, 3 */
			vcb_cal_f(2,1,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(5,1,2);
			/* 4, 3, 2 */
			vcb_cal_f(5,8,3);
			vcb_ass_f(2,4);
			vcb_ass_f(1,5);
			/* 5, 4, 2 */
			vcb_cal_f(6,7,6);
			vcb_ass_f(3,7);
			vcb_ass_f(1,8);

			no_faces = 3; 
			break;

		case 16:
			/* 1, 2, 3 */
			vcb_cal_f(1,5,0);
			vcb_cal_f(2,6,1);
			vcb_cal_f(1,4,2);
			/* 1, 4, 2*/
			vcb_ass_f(2,3);
			vcb_ass_f(1,4);
			vcb_cal_f(7,8,5);
			/* 6, 2, 4*/
			vcb_ass_f(2,6);
			vcb_ass_f(5,7);
			vcb_cal_f(2,3,8);
			/* 2, 1, 11 */
			vcb_ass_f(5,9);
			vcb_cal_f(7,3,10);
			vcb_ass_f(8,11);
			/* 4, 11, 12 */
			vcb_ass_f(5,12);
			vcb_ass_f(1,13);
			vcb_cal_f(6,7,14);

			no_faces = 5;
			break;
		case 17:
			/* 1, 2, 3 */
			vcb_cal_f(1,5,0);
			vcb_cal_f(2,6,1);
			vcb_cal_f(2,3,2);
			/* 3, 4, 1*/
			vcb_ass_f(2,3);
			vcb_cal_f(1,4,4);
			vcb_ass_f(0,5);

			no_faces = 2; 
			break;

		case 18:
			/* 1, 2, 3 */
			vcb_cal_f(1,4,0);
			vcb_cal_f(1,2,1);
			vcb_cal_f(4,8,2);
			/* 3, 2, 5 */
			vcb_ass_f(1,3);
			vcb_cal_f(2,6,4);
			vcb_ass_f(2,5);
			/* 5, 4, 8 */
			vcb_ass_f(4,6);
			vcb_cal_f(7,8,7);
			vcb_ass_f(5,8);
			/* 8, 7, 11 */
			vcb_ass_f(7,9);
			vcb_ass_f(4,10);
			vcb_cal_f(6,7,11);
			
			/* 12, 13, 14 */
			vcb_cal_f(2,3,12);
			vcb_cal_f(3,4,13);
			vcb_cal_f(3,7,14);

			no_faces = 5;
			break;
		case 19: 
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(2,3,1);
			vcb_cal_f(1,5,2);

			/* 3, 2, 5 */
			vcb_ass_f(1,3);
			vcb_cal_f(3,7,4);
			vcb_ass_f(2,5);
			/* 5, 4, 8 */
			vcb_ass_f(4,6);
			vcb_cal_f(3,4,7);
			vcb_ass_f(5,8);
			/* 8, 7, 11 */
			vcb_ass_f(7,9);
			vcb_cal_f(1,4,10);
			vcb_ass_f(8,11);

			no_faces = 4;
			break;
		case 20:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(1,4,1);
			vcb_cal_f(1,5,2);
			/* 4, 5, 6 */
			vcb_cal_f(7,3,3);
			vcb_cal_f(7,6,4);
			vcb_cal_f(7,8,5);
			
			no_faces = 2; 
			break;

		case 21:
			/* 1, 2, 3 */
			vcb_cal_f(1,2,0);
			vcb_cal_f(1,4,1);
		    vcb_cal_f(1,5,2);
			no_faces = 1; 
			break;
		case 22:
			no_faces = 0;
			break; /* no triangles */
		default:
			break;
	}
	return(no_faces); 
}

#ifndef vcb_cal_fn
#define vcb_cal_fn(i,j,k) \
			weight = val[i]/(val[i]-val[j]); onemwe = 1.f - weight; \
			f[k*6+0] = v[i][0] * onemwe + v[j][0] * weight;         \
			f[k*6+1] = v[i][1] * onemwe + v[j][1] * weight;         \
			f[k*6+2] = v[i][2] * onemwe + v[j][2] * weight;         \
			f[k*6+3] = v[i][3] * onemwe + v[j][3] * weight;         \
			f[k*6+4] = v[i][4] * onemwe + v[j][4] * weight;         \
			f[k*6+5] = v[i][5] * onemwe + v[j][5] * weight
#endif


#ifndef vcb_ass_fn
#define vcb_ass_fn(i,j) \
	        f[j*6 + 0] = f[i*6 + 0];  \
	        f[j*6 + 1] = f[i*6 + 1];  \
	        f[j*6 + 2] = f[i*6 + 2];  \
	        f[j*6 + 3] = f[i*6 + 3];  \
	        f[j*6 + 4] = f[i*6 + 4];  \
	        f[j*6 + 5] = f[i*6 + 5]
#endif

int  vcbMCVWithNormals(int x, int y, int z, float * f)
{
	static float ov[9][6], val[9], v[9][6];
	float weight, onemwe, fx, fy, fz;
	int i, indx, mask, wcase, no_faces;
	vcbMCT tab;

	indx = (x * ydim + y)*zdim + z;
	graboval(indx);

	for(i=1, mask = 0; i<=8; i++)
		if(oval[i*4]<0) mask |= 1<<(i-1);

	tab = & mcube_table[mask];
	wcase=tab->which_case;

	if ((wcase<1)||(wcase>22)) return 0;

	fx = (float)x; fy = (float)y; fz = (float)z;

	vcb_tabulate(ov,fx,fy,fz);
	for (i = 1; i <= 8; i ++) {
		ov[i][3] = oval[i*4+1];
		ov[i][4] = oval[i*4+2];
		ov[i][5] = oval[i*4+3];
	}

	/* begin extraction */
	for(indx = 1; indx <= 8; indx ++ ) {
		val[indx]= oval[(tab->permute[indx-1])*4];

		for (i = 0; i < 6; i ++)
			v[indx][i] = ov[tab->permute[indx-1]][i];
	}

	switch(wcase){
		case 0:
			no_faces = 0;
			break; /* no triangles */
		case 1:

			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(1,5,1);
			vcb_cal_fn(1,4,2);
			no_faces = 1; 
			break;
		case 2:
			/* 1, 2, 3 */
			vcb_cal_fn(1,5,0);
			vcb_cal_fn(2,6,1);
			vcb_cal_fn(2,3,2);
			/* 3, 4, 1*/
			vcb_ass_fn(2,3);
			vcb_cal_fn(1,4,4);
			vcb_ass_fn(0,5);

			no_faces = 2; 
			break;
		case 3:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(1,5,1);
			vcb_cal_fn(1,4,2);
			/* 4, 5, 6 */
			vcb_cal_fn(3,2,3);
			vcb_cal_fn(3,7,4);
			vcb_cal_fn(3,4,5);

			no_faces = 2; 
			break;
		case 4:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(1,5,1);
			vcb_cal_fn(1,4,2);
			/* 4, 5, 6 */
			vcb_cal_fn(7,3,3);
			vcb_cal_fn(7,8,4);
			vcb_cal_fn(7,6,5);

			no_faces = 2; 
			break;
		case 5:
			/* 1, 2, 3 */
			vcb_cal_fn(2,1,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(5,1,2);
			/* 4, 3, 2 */
			vcb_cal_fn(5,8,3);
			vcb_ass_fn(2,4);
			vcb_ass_fn(1,5);
			/* 5, 4, 2 */
			vcb_cal_fn(6,7,6);
			vcb_ass_fn(3,7);
			vcb_ass_fn(1,8);

			no_faces = 3; 
			break;
		case 6:
			/* 1, 2, 3 */
			vcb_cal_fn(1,5,0);
			vcb_cal_fn(2,6,1);
			vcb_cal_fn(2,3,2);
			/* 3, 4, 1 */
			vcb_ass_fn(2,3);
			vcb_cal_fn(1,4,4);
			vcb_ass_fn(0,5);
			/* 5, 6, 7 */
			vcb_cal_fn(7,3,6);
			vcb_cal_fn(7,8,7);
			vcb_cal_fn(7,6,8);

			no_faces = 3; 
			break;
		case 7:
			/* 1, 2, 3 */
			vcb_cal_fn(2,1,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(2,6,2);
			/* 4, 5, 6 */
			vcb_cal_fn(4,1,3);
			vcb_cal_fn(4,3,4);
			vcb_cal_fn(4,8,5);
			/* 7, 8, 9 */
			vcb_cal_fn(7,8,6);
			vcb_cal_fn(7,6,7);
			vcb_cal_fn(7,3,8);

			no_faces = 3; 
			break;
		case 8:
			/* 1, 2, 3 */
			vcb_cal_fn(1,4,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(6,7,2);
			/* 4, 1, 3 */
			vcb_cal_fn(5,8,3);
			vcb_ass_fn(0,4);
			vcb_ass_fn(2,5);

			no_faces = 2; 
			break;
		case 9:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(6,7,1);
			vcb_cal_fn(6,2,2);
			/* 1, 3, 4 */
			vcb_ass_fn(0,3);
			vcb_cal_fn(8,7,4);
			vcb_ass_fn(1,5);
			/* 1, 4, 5 */
			vcb_ass_fn(0,6);
			vcb_cal_fn(1,4,7);
			vcb_ass_fn(4,8);
			/* 5, 4, 6*/
			vcb_ass_fn(7,9);
			vcb_cal_fn(8,4,10);
			vcb_ass_fn(4,11);
		
			no_faces = 4; 
			break;
		case 10:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(4,3,1);
			vcb_cal_fn(1,5,2);
			/* 2, 4, 3 */
			vcb_ass_fn(1,3);
			vcb_cal_fn(4,8,4);
			vcb_ass_fn(2,5);
			/* 5, 6, 7 */
			vcb_cal_fn(6,2,6);
			vcb_cal_fn(6,5,7);
			vcb_cal_fn(7,3,8);
			/* 8, 7, 9*/
			vcb_ass_fn(8,9);
			vcb_ass_fn(7,10);
			vcb_cal_fn(7,8,11);
			
			no_faces = 4; 
			break;
		case 11:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(6,2,1);
			vcb_cal_fn(7,3,2);
			/* 1, 3, 4 */
			vcb_ass_fn(0,3);
			vcb_ass_fn(2,4);
			vcb_cal_fn(5,8,5);
			/* 1, 4, 5 */
			vcb_ass_fn(0,6);
			vcb_ass_fn(5,7);
			vcb_cal_fn(1,4,8);
			/* 4, 3, 6*/
			vcb_ass_fn(5,9);
			vcb_ass_fn(2,10);
			vcb_cal_fn(7,8,11);

			no_faces = 4; 
			break;
		case 12:
			/* 1, 2, 3 */
			vcb_cal_fn(2,1,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(5,1,2);
			/* 3, 2, 4 */
			vcb_ass_fn(2,3);
			vcb_ass_fn(1,4);
			vcb_cal_fn(5,8,5);
			/* 4, 2, 5 */
			vcb_ass_fn(5,6);
			vcb_ass_fn(1,7);
			vcb_cal_fn(6,7,8);
			/* 6, 7, 8*/
			vcb_cal_fn(4,1,9);
			vcb_cal_fn(4,3,10);
			vcb_cal_fn(4,8,11);

			no_faces = 4; 
			break;
		case 13:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(1,5,1);
			vcb_cal_fn(1,4,2);
			/* 4, 5, 6 */
			vcb_cal_fn(3,2,3);
			vcb_cal_fn(3,4,5);
			vcb_cal_fn(3,7,4);
			/* 7, 8, 9 */
			vcb_cal_fn(6,2,6);
			vcb_cal_fn(6,7,7);
			vcb_cal_fn(6,5,8);
			/* 10, 11, 12*/
			vcb_cal_fn(8,5,9);
			vcb_cal_fn(8,7,10);
			vcb_cal_fn(8,4,11);

			no_faces = 4; 
			break;
		case 14:
			/* 1, 2, 3 */
			vcb_cal_fn(2,1,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(6,7,2);
			/* 1, 3, 4 */
			vcb_ass_fn(0,3);
			vcb_ass_fn(2,4);
			vcb_cal_fn(8,4,5);
			/* 1, 4, 5 */
			vcb_ass_fn(0,6);
			vcb_ass_fn(5,7);
			vcb_cal_fn(5,1,8);
			/* 3, 6, 4*/
			vcb_ass_fn(2,9);
			vcb_cal_fn(8,7,10);
			vcb_ass_fn(5,11);

			no_faces = 4; 
			break;
		
		case 15:
			/* 1, 2, 3 */
			vcb_cal_fn(2,1,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(5,1,2);
			/* 4, 3, 2 */
			vcb_cal_fn(5,8,3);
			vcb_ass_fn(2,4);
			vcb_ass_fn(1,5);
			/* 5, 4, 2 */
			vcb_cal_fn(6,7,6);
			vcb_ass_fn(3,7);
			vcb_ass_fn(1,8);

			no_faces = 3; 
			break;

		case 16:
			/* 1, 2, 3 */
			vcb_cal_fn(1,5,0);
			vcb_cal_fn(2,6,1);
			vcb_cal_fn(1,4,2);
			/* 1, 4, 2*/
			vcb_ass_fn(2,3);
			vcb_ass_fn(1,4);
			vcb_cal_fn(7,8,5);
			/* 6, 2, 4*/
			vcb_ass_fn(2,6);
			vcb_ass_fn(5,7);
			vcb_cal_fn(2,3,8);
			/* 2, 1, 11 */
			vcb_ass_fn(5,9);
			vcb_cal_fn(7,3,10);
			vcb_ass_fn(8,11);
			/* 4, 11, 12 */
			vcb_ass_fn(5,12);
			vcb_ass_fn(1,13);
			vcb_cal_fn(6,7,14);

			no_faces = 5;
			break;
		case 17:
			/* 1, 2, 3 */
			vcb_cal_fn(1,5,0);
			vcb_cal_fn(2,6,1);
			vcb_cal_fn(2,3,2);
			/* 3, 4, 1*/
			vcb_ass_fn(2,3);
			vcb_cal_fn(1,4,4);
			vcb_ass_fn(0,5);

			no_faces = 2; 
			break;

		case 18:
			/* 1, 2, 3 */
			vcb_cal_fn(1,4,0);
			vcb_cal_fn(1,2,1);
			vcb_cal_fn(4,8,2);
			/* 3, 2, 5 */
			vcb_ass_fn(1,3);
			vcb_cal_fn(2,6,4);
			vcb_ass_fn(2,5);
			/* 5, 4, 8 */
			vcb_ass_fn(4,6);
			vcb_cal_fn(7,8,7);
			vcb_ass_fn(5,8);
			/* 8, 7, 11 */
			vcb_ass_fn(7,9);
			vcb_ass_fn(4,10);
			vcb_cal_fn(6,7,11);
			
			/* 12, 13, 14 */
			vcb_cal_fn(2,3,12);
			vcb_cal_fn(3,4,13);
			vcb_cal_fn(3,7,14);

			no_faces = 5;
			break;
		case 19: 
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(2,3,1);
			vcb_cal_fn(1,5,2);

			/* 3, 2, 5 */
			vcb_ass_fn(1,3);
			vcb_cal_fn(3,7,4);
			vcb_ass_fn(2,5);
			/* 5, 4, 8 */
			vcb_ass_fn(4,6);
			vcb_cal_fn(3,4,7);
			vcb_ass_fn(5,8);
			/* 8, 7, 11 */
			vcb_ass_fn(7,9);
			vcb_cal_fn(1,4,10);
			vcb_ass_fn(8,11);

			no_faces = 4;
			break;
		case 20:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(1,4,1);
			vcb_cal_fn(1,5,2);
			/* 4, 5, 6 */
			vcb_cal_fn(7,3,3);
			vcb_cal_fn(7,6,4);
			vcb_cal_fn(7,8,5);
			
			no_faces = 2; 
			break;

		case 21:
			/* 1, 2, 3 */
			vcb_cal_fn(1,2,0);
			vcb_cal_fn(1,4,1);
		    vcb_cal_fn(1,5,2);
			no_faces = 1; 
			break;
		case 22:
			no_faces = 0;
			break; /* no triangles */
		default:
			break;
	}
	return(no_faces); 
}

int mc_hashfunc(void * data, void * param)
/* default hash function used by vcbMarchingCubeBlk for extensible hashing */
{
	unsigned int id = 0;
	float * vert, * coeff;
	int bmask1 = 0x0f0f0f0f;

	vert = (float *) data;
	coeff = (float *) param;

	id += (int)((vert[0] - coeff[0])*coeff[3]); 
	id += (int)((vert[1] - coeff[1])*coeff[4]);
	id += (int)((vert[2] - coeff[2])*coeff[5]);

	return (int) id;
}

int mc_cmp (void * src, void * dst)
/* default compare function used by vcbMarchingCubeBlk for extensible hashing */
{
	int i;
	float diff, tdiff, * vert1, * vert2;
	vert1 = (float *) src;
	vert2 = (float *) dst;

	diff = 0.f;
	for (i = 0; i < 3; i ++) {
		tdiff = vert1[i] - vert2[i];
		diff += ( tdiff >= 0) ? tdiff : (-1.f*tdiff);
	}

	return (diff < 0.00001) ? 1 : -1;
}


int  vcbMarchingCubeBlk(vcbdatatype datatype, void * block, float isoval, int *dsizes, 
				int wn, int * nverts, float ** varray_v, int ** varray_i)
/* return: number of triangles identified */
{
	int i, j, k, nx, ny, nz;
	float f[90], * mc_data;  /* at most 5 tri, 15 vertices with normal. 15x6=90 */
	int ntri, nfacets, totalbytes;
	Dmlist dm;
	int nval, enbytes;
	int  (*vcb_mcv) (int x, int y, int z, float * f);

	nx = dsizes[0] - 1;
	ny = dsizes[1] - 1;
	nz = dsizes[2] - 1;

	nval = (wn == VCB_ONLYVERTEX) ? 3 : 6;
	enbytes = 3* nval * sizeof(float);
	vcb_mcv = (wn == VCB_ONLYVERTEX) ? vcbMCVNoNormals : vcbMCVWithNormals;

	vcbMCVertexBegin(datatype, block, dsizes, isoval, wn);

	dm = new_dmlist();
	nfacets = 0;
	for (i = 0; i < nx; i ++)
		for (j = 0; j < ny; j ++)
			for (k = 0; k < nz; k ++) {
				ntri = vcb_mcv(i, j, k, f);
				if (ntri > 0) {
					dml_append(dm, f, ntri*enbytes);
					nfacets += ntri;
				}
			}
	vcbMCVertexEnd();

	totalbytes = dml_totalbytes(dm);
	mc_data = (float *) malloc (totalbytes);
	dml_cpyall(dm, mc_data);

	for (i = 0; i < nfacets*nval; i++) /* to remove unnecessary precision */
		mc_data[i] = (float)(floor(mc_data[i]*1000.+0.5)/1000.); /* keep 3-digit after dot */

	vcbTrimeshRaw(mc_data, nval, nfacets, nverts, varray_v, varray_i);
	free(mc_data);

	if (wn == VCB_WITHNORMAL)
		for (i = 0; i < (*nverts); i ++)
			la_normalize3(&(*varray_v)[i*6+3]);

	return nfacets;
}

int  vcbMarchingCubeList(vcbdatatype datatype, void * block, float isoval, int *dsizes,
				 int ncells, short * cellids,
				 int wn, int * nverts, float ** varray_v, int ** varray_i)
/* return: number of triangles identified */
{
	int i, j, k, l;
	float f[90], * mc_data;  /* at most 5 tri, 15 vertices with normal, 15x6=90 */
	int ntri, nfacets, totalbytes;
	Dmlist dm;
	int nval, enbytes;
	int  (*vcb_mcv) (int x, int y, int z, float * f);

	nval = (wn == VCB_ONLYVERTEX) ? 3 : 6;
	enbytes = 3* nval * sizeof(float);
	vcb_mcv = (wn == VCB_ONLYVERTEX) ? vcbMCVNoNormals : vcbMCVWithNormals;

	vcbMCVertexBegin(datatype, block, dsizes, isoval, wn);

	dm = new_dmlist();
	nfacets = 0;
	for (l = 0; l < ncells; l ++) {
		i = cellids[l*3];
		j = cellids[l*3+1];
		k = cellids[l*3+2];

		if ((ntri = vcb_mcv(i, j, k, f))> 0) {
			dml_append(dm, f, ntri*enbytes);
			nfacets += ntri;
		}
	}
	vcbMCVertexEnd();

	totalbytes = dml_totalbytes(dm);
	mc_data = (float *) malloc (totalbytes);
	dml_cpyall(dm, mc_data);

	for (i = 0; i < nfacets*nval; i++) /* to remove unnecessary precision */
		mc_data[i] = (float)(floor(mc_data[i]*1000.+0.5)/1000.); /* keep 3-digit after dot */

	vcbTrimeshRaw(mc_data, nval, nfacets, nverts, varray_v, varray_i);
	free(mc_data);

	if (wn == VCB_WITHNORMAL)
		for (i = 0; i < (*nverts); i ++)
			la_normalize3(&(*varray_v)[i*6+3]);

	return nfacets;
}
