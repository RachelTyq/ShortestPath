/*
周嘉明
ZY1706133
可执行文件为ubuntu下二进制文件，运行方法 ./route.bin
 */
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <vector>
#include <time.h>

using namespace std;

/*
函数名：readfile
参数：
	filename: txt文件名
	map: 存储文件中二维矩阵的数组的指针
	num: 二维数组长度，长和宽一样
函数功能：读取文件中的内容，并保存在二维数组中
 */
bool readfile(string filename, int** map, int num) {
	ifstream in(filename, ios::in);
	int number;
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			in >> number;
			*((int*)map + i * num + j) = number;
			if (i == j)
				*((int*)map + i * num + j) = 0;
		}
	}
	in.close();
}

/*
函数名：printMatrix
参数：
	map: 二维数组的指针
	num: 二维数组长度，长和宽一样
函数功能：打印二维数组的内容
 */
void printMatrix(int** map, int num) {
	for (int i = 0; i < num; i++) {
		printf("%4d:", i + 1);
		for (int j = 0; j < num; j++) {
			printf("%8d", *((int*)map + i * num + j));
		}
		printf("\n");
	}
}

/*
函数名：printlist
参数：
	l: 存储路径的向量，即从源节点开始经过的节点集合
	map_dis: 二维数组的指针，该二维数组是存储城市间距离的数组
	num: 二维数组长度，长和宽一样
函数功能：打印从源节点开始的路径，并且打印从源节点到各个中间节点的距离
 */
void printlist(vector<int> l, int** map_dis, int num) {
	printf("路径与距离如下：\n");
	for (vector<int>::iterator it = l.begin(); it != l.end(); it++) {
		printf("%4d", *it);
	}
	printf("\n");
	int sum = 0;
	printf("%4d", 0);
	for (int i = 0; i < l.size() - 1; i++) {
		sum += * ((int*)map_dis + l[i] * num + l[i + 1]);
		printf("%4d", sum);
	}
	printf("\n");
}

/*
函数名：floyd
参数：
	map: 二维数组的指针, 指向待计算最短路径的数组
	result: 二维数组的指针，指向存储最短路径的数组
	num: 二维数组长度，长和宽一样
函数功能：图使用二维数组map来表示，利用floyd算法计算图中各个节点对之间的最短路径，并且将结果保存在result中
 */
void floyd(int** map, int** result, int num) {
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; ++j)
		{
			*((int*)result + i * num + j) = *((int*)map + i * num + j);
		}
	}

	for (int k = 0; k < num; ++k)
	{
		for (int i = 0; i < num; ++i)
		{
			for (int j = 0; j < num; ++j)
			{
				if (*((int*)result + i * num + j) >
				        *((int*)result + i * num + k) + * ((int*)result + k * num + j))
				{
					*((int*)result + i * num + j) =
					    *((int*)result + i * num + k) + * ((int*)result + k * num + j);
				}
			}
		}
	}
}

/*
函数名：nearestpath
参数：
	map_dis: 二维数组的指针, 指向待计算最短路径的城市距离数组
	map_cost: 二维数组的指针，指向路费的数组
	num: 二维数组长度，长和宽一样
	maxcost:最大路费限制
	target:目标节点
	min_dis:二维数组的指针，数组记录了每两个城市之间最短路径
	min_cost:二维数组的指针，数组记录了每两个城市之间最小路费
函数功能：在满足最大路费限制的情况下，求源节点到目标节点的最短路径
 */
int nearestpath(int** map_dis, int** map_cost, int num, int maxcost, int target,
                int** min_dis, int** min_cost) {
	//distance数组记录从源节点到其他节点的距离
	int* distance = new int[num];
	for (int i = 0; i < num; i++) {
		distance[i] = INT_MAX;
	}
	distance[0] = 0;

	//cost数组记录从源节点到其他节点的路费
	int* cost = new int[num];
	for (int i = 0; i < num; i++) {
		cost[i] = maxcost;
	}
	cost[0] = 0;

	//visited数组记录节点是否被访问过
	bool* visited = new bool[num];
	memset(visited, 0, sizeof(bool) * num);

	//cities向量记录从源节点开始经过的路径
	vector<int> cities;
	//res_path记录当前满足费用要求的最优路径
	vector<int> res_path;

	cities.push_back(0);
	visited[0] = true;
	//pre为前一个搜索的节点
	int pre = -1;
	//res为当前符合费用要求的最优路径长度
	int res = INT_MAX;

	//利用深度优先穷举从节点0到节点49的所有路径，并根据约束进行剪枝
	while (cities.size()) {
		//city为当前节点，用深度优先进行搜索
		int city = cities.back();
		//如果目标节点，判断此时的路径是否比原本的解更优化
		//如果是，则保存当前解
		if (city == target - 1) {
			pre = city;
			if (distance[city] < res) {
				res = distance[city];
				res_path = cities;
			}
			cities.pop_back();
			visited[pre] = false;
			continue;
		}
		//next为当前节点下一个未被访问的邻节点
		int next = pre + 1;
		while (next < num && (visited[next] || * ((int*)map_dis + city * num + next) == 9999)) {
			next++;
		}

		//如果当前节点的邻节点已经搜索完，则出栈
		if (next == num) {
			pre = city;
			cities.pop_back();
			visited[pre] = false;
		}
		//如果找到当前节点的下一个邻节点，判断剪枝条件
		//剪枝条件1：
		//源节点到当前节点路费 + 当前节点到邻节点路费 + 邻节点到目的节点的最小路费 <= 最大费用要求
		//剪枝条件2：
		//源节点到当前节点距离 + 当前节点到邻节点距离 + 邻节点到目的节点的最短路径 < 当前最优路径
		else if (cost[city] + * ((int*)map_cost + city * num + next) <=
		         maxcost - * ((int*)min_cost + next * num + target - 1)
		         && distance[city] + * ((int*)map_dis + city * num + next) <=
		         res - * ((int*)min_dis + next * num + target - 1)
		        ) {
			//邻节点满足剪枝条件，则入栈继续深度优先搜索
			cities.push_back(next);
			cost[next] = cost[city] + * ((int*)map_cost + city * num + next);
			distance[next] = distance[city] + * ((int*)map_dis + city * num + next);
			visited[next] = true;
			pre = -1;
		}
		else {
			//邻节点不满足不满足剪枝剪枝，则剪枝，停止往下搜索，转而搜索当前节点的下一个邻节点
			pre = next;
		}
	}
	printf("满足费用要求的最优路径结果:\n");
	printf("cost:%d, distance:%d\n", cost[target - 1], distance[target - 1]);
	printlist(res_path, map_dis, num);

	delete[] distance;
	delete[] cost;
	delete[] visited;
	return res;
}

int main() {
	string distance_file = "m1.txt";
	string cost_file = "m2.txt";
	int num = 50;
	int maxcost = 1500;
	printf("输入城市数量（如50）:\n");
	cin >> num;
	printf("输入城市间公路连通情况文件（如m1.txt）:\n");
	cin >> distance_file;
	printf("输入每段公路收取的费用文件（如m2.txt）：\n");
	cin >> cost_file;
	printf("输入最大路费限制（如1500）:\n");
	cin >> maxcost;
	int distance[num][num];

	readfile(distance_file, (int**)distance, num);
	int cost[num][num];

	readfile(cost_file, (int**)cost, num);
	int target = num;
	clock_t start, finish;
	double totaltime;
	start = clock();
	int min_dis[num][num];
	floyd((int**)distance, (int**)min_dis, num);

	int min_cost[num][num];
	floyd((int**)cost, (int**)min_cost, num);

	int res = nearestpath((int**)distance, (int**)cost, num, maxcost, target,
	                      (int**)min_dis, (int**)min_cost);
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("time: %f\n", totaltime);
	// printf("final distance:%d\n", res);

	return 0;
}