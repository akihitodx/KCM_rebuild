//
// Created by DELL on 2024/1/3.
//

#ifndef KCM_ALG_H
#define KCM_ALG_H

#include <string>
#include "../structure/Graph.h"
#include "../structure/type.h"
#include <map>
#include <unordered_map>
#include <set>
#include <cmath>

void init_query(Graph &graph);

void printGraph(Graph &graph);

void get_info(Graph &graph,unordered_set<int> &kernel,unordered_map<int,unordered_set<int>> &comm,set<pair<int,int>> &special, unordered_map<int,unordered_set<int>> &others);

void pro_nodes(Graph &query,Graph &data,unordered_set<int> &kernel,unordered_map<int,unordered_set<int>> &comm,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,unordered_map<int,unordered_set<int>> &others);

unsigned_key pre_match_order_level(vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,vector<unordered_map<unsigned_key,unsigned_key>> &matches,unordered_map<int,unordered_set<int>> &comm,set<pair<int,int>> &special);

string to_key_index(unsigned_key key, const vector<int> &match_table);
void init_index(int querySize,Graph &data,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,set<pair<int,int>> &special,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table, unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<unordered_map<unsigned_key,unsigned_key>> &matches);


void get_other_cand(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,Graph &data, Graph &query,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_set<int>> &others);
void get_others_table(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table,int length);


void part_join(unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,vector<unordered_map<unsigned_key,unsigned_key>> &matches);
#endif //KCM_ALG_H


