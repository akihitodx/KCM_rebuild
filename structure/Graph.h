//
// Created by DELL on 2024/1/3.
//

#ifndef KCM_GRAPH_H
#define KCM_GRAPH_H
#include <vector>
#include <unordered_set>
#include <set>
using namespace std;

//class Node{
//public:
//    Node();
//
//    int id;
//    int label;
//    int degree;
//    vector<int> nei;
//    Node(int id, int label ,int degree,vector<int> &nei);
//};

class Graph {
public:
    vector<int> label;
    vector<int> degree;
    vector<unordered_set<int>> adj;
    vector<multiset<int>> nei_label;

    int count_v;
    int count_e;

    void read_graph(const string& path);
};


#endif //KCM_GRAPH_H
