#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
#include "ARIMAModel.h"
#include <vector>
#include <stdio.h>
#include <time.h>
#include <string.h>


using namespace std;

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename);

vector<vector<int> > DP_Bag(int phy_mem, int phy_CPU, vector<int> V_mem, vector<int> V_CPU, vector<int> V_num, bool memOrCPU);

vector<vector<int> > Greedy_Bag(int phy_mem, int phy_CPU, vector<int> V_mem, vector<int> V_CPU, vector<int> V_num, bool memOrCPU);

#endif
