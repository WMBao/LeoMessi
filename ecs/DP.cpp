#include "predict.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>

using namespace std;

#define GET_ARRAY_LEN(array,len) {len = (sizeof(array) / sizeof(array[0]));}
bool cmp_by_value(const pair<int,int>& lhs, const pair<int, int>& rhs) {
	return lhs.second > rhs.second;
}


vector<vector<int> > Greedy_Bag(int phy_mem, int phy_CPU, vector<int> V_mem, vector<int> V_CPU, vector<int> V_num, bool memOrCPU)
{

	vector<vector<int> > final_result;


	vector<int> V_flaver;
	for (int i = 0; i < V_num.size(); i++)
	{
		for (int j = 0; j < V_num[i]; j++)
		{
			V_flaver.push_back(i);
		}
	}

	multimap<int, int> V_map;

	for (int i = 0; i < V_flaver.size(); i++)
	{
		if (memOrCPU)
		{
			V_map.insert(pair<int, int>(V_flaver[i], V_mem[V_flaver[i]]));
		}
		else
		{
			V_map.insert(pair<int, int>(V_flaver[i], V_CPU[V_flaver[i]]));
		}

	}
	vector<pair<int, int>> map_vec(V_map.begin(), V_map.end());
	sort(map_vec.begin(), map_vec.end(), cmp_by_value);

	int V_count = V_flaver.size();

	while (V_count > 0)
	{
		int cur_mem = phy_mem;
		int cur_CPU = phy_CPU;
		vector<int> one_phy_res;
		for (int i = 0; i < V_count; i++)
		{
			if (cur_mem - V_mem[map_vec[i].first] >= 0 && cur_CPU - V_CPU[map_vec[i].first] >= 0)
			{
				one_phy_res.push_back(map_vec[i].first);
				cur_mem -= V_mem[map_vec[i].first];
				cur_CPU -= V_CPU[map_vec[i].first];
			}
		}
		final_result.push_back(one_phy_res);
		for (int i = 0; i < one_phy_res.size(); i++)
		{
			V_num[one_phy_res[i]] -= 1;
		}

		V_flaver.clear();
		for (int i = 0; i < V_num.size(); i++)
		{
			for (int j = 0; j < V_num[i]; j++)
			{
				V_flaver.push_back(i);
			}
		}

		V_map.clear();
		for (int i = 0; i < V_flaver.size(); i++)
		{
			if (memOrCPU)
			{
				V_map.insert(pair<int, int>(V_flaver[i], V_mem[V_flaver[i]]));
			}
			else
			{
				V_map.insert(pair<int, int>(V_flaver[i], V_CPU[V_flaver[i]]));
			}

		}

		map_vec.clear();
		map_vec.insert(map_vec.begin(), V_map.begin(), V_map.end());
		sort(map_vec.begin(), map_vec.end(), cmp_by_value);

		V_count = V_flaver.size();
	}
	return final_result;
}


vector<vector<int> > DP_Bag(int phy_mem, int phy_CPU, vector<int> V_mem, vector<int> V_CPU, vector<int> V_num, bool memOrCPU)
{
	
	vector<vector<int> > final_result;
	phy_mem += 1;
	phy_CPU += 1;


	vector<int> V_flaver;
	for (int i = 0; i < V_num.size(); i++)
	{
		for (int j = 0; j < V_num[i]; j++)
		{
			V_flaver.push_back(i);
		}
	}
	int V_count = V_flaver.size() + 1;

	while (V_count > 1)
	{

		int ***table = new int**[V_count];
		for (int i = 0; i < V_count; i++)
		{
			table[i] = new int*[phy_CPU];
			for (int j = 0; j < phy_CPU; j++)
				table[i][j] = new int[phy_mem];
		}

		vector<vector<vector<vector<int> > > > result (V_count, vector<vector<vector<int> > >(phy_CPU, vector<vector<int> >(phy_mem)));
		

		for (int i = 0; i < V_count; i++)
		{
			for (int j = 0; j < phy_CPU; j++)
			{
				for (int k = 0; k < phy_mem; k++)
				{
					table[i][j][k] = i == 0 ? 0 : table[i - 1][j][k];
					if (i != 0)
						result[i][j][k].insert(result[i][j][k].end(), result[i - 1][j][k].begin(), result[i - 1][j][k].end());
					if (i > 0 && j >= V_CPU[V_flaver[i - 1]] && k >= V_mem[V_flaver[i - 1]])
					{
						if (memOrCPU)
						{
							if (table[i][j][k] < table[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]] + V_mem[V_flaver[i - 1]])
							{
								table[i][j][k] = table[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]] + V_mem[V_flaver[i - 1]];
								result[i][j][k].clear();
								result[i][j][k].insert(result[i][j][k].end(), result[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]].begin(), result[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]].end());
								result[i][j][k].push_back(V_flaver[i - 1]);
							}
						}
						else
						{
							if (table[i][j][k] < table[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]] + V_CPU[V_flaver[i - 1]])
							{
								table[i][j][k] = table[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]] + V_CPU[V_flaver[i - 1]];
								result[i][j][k].clear();
								result[i][j][k].insert(result[i][j][k].end(), result[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]].begin(), result[i - 1][j - V_CPU[V_flaver[i - 1]]][k - V_mem[V_flaver[i - 1]]].end());
								result[i][j][k].push_back(V_flaver[i - 1]);
							}
						}
					}

				
				}
			}
		}
		vector<int> one_phy_res;
		one_phy_res.insert(one_phy_res.end(), result[V_count - 1][phy_CPU - 1][phy_mem - 1].begin(), result[V_count - 1][phy_CPU - 1][phy_mem - 1].end());


		vector<int> V_num_ = V_num;

		for (int i = 0; i < one_phy_res.size(); i++)
		{
			V_num_[one_phy_res[i]] -= 1;
		}

		bool ok = true;
		while (ok)
		{
			ok = true;
			for (int i = 0; i < V_num_.size(); i++)
			{
				if (V_num_[i] < 0)
				{
					ok = false;
					break;
				}
			}
			if (ok == true)
			{
				final_result.push_back(one_phy_res);
				V_num = V_num_;
				for (int i = 0; i < one_phy_res.size(); i++)
				{
					V_num_[one_phy_res[i]] -= 1;
				}
			}
		}
		V_flaver.clear();
		for (int i = 0; i < V_num.size(); i++)
		{
			for (int j = 0; j < V_num[i]; j++)
			{
				V_flaver.push_back(i);
			}
		}
		V_count = V_flaver.size() + 1;

	}

	return	final_result;
}




//int main()
//{
//	int phy_mem = 5;
//	int phy_CPU = 5;
//
//	vector<int> V_mem;
//	V_mem.push_back(2);
//	V_mem.push_back(1);
//
//	vector<int> V_CPU;
//	V_CPU.push_back(2);
//	V_CPU.push_back(1);
//
//	vector<int> V_num;
//	V_num.push_back(2);
//	V_num.push_back(2);
//
//	bool memOrCPU = true;
//
//	vector<vector<int> > result = DP_Bag(phy_mem, phy_CPU, V_mem, V_CPU, V_num, memOrCPU);
//	for (int num_phy_m = 0; num_phy_m < result.size(); num_phy_m++)
//	{
//		cout << num_phy_m << '\n';
//		for (int num_flav = 0; num_flav < result[num_phy_m].size(); num_flav++)
//		{
//			cout << '\t' << result[num_phy_m][num_flav] << '\n';
//		}
//	}
//	return 0;
//}

