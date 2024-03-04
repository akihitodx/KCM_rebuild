//
// Created by DELL on 2024/1/3.
//

#ifndef KCM_ALG_H
#define KCM_ALG_H

#include <string>
#include "../structure/Graph.h"
#include <map>
#include <unordered_map>
#include <set>
typedef unsigned int unsigned_key;

void init_query(Graph &graph);

void printGraph(Graph &graph);

void get_info(Graph &graph,unordered_set<int> &kernel,unordered_map<int,unordered_set<int>> &comm,set<pair<int,int>> &special, unordered_map<int,unordered_set<int>> &others);

void pro_nodes(Graph &query,Graph &data,unordered_set<int> &kernel,unordered_map<int,unordered_set<int>> &comm,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,unordered_map<int,unordered_set<int>> &others);

void init_index(int query_graph_length,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,unordered_map<unsigned_key ,set<vector<int>>> &index,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table);
void init_index_special(int query_graph_length, unordered_map<int,unordered_set<int>> &kernel_index, set<pair<int,int>> special,unordered_map<unsigned_key ,set<vector<int>>> &index, Graph &data,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table);

unsigned_key init_match_order(unordered_map<unsigned_key, set<vector<int>>> &index,vector<pair<unsigned_key,unsigned_key>> &match_order);

void init_match_order_level(unordered_map<unsigned_key, set<vector<int>>> &index, vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level);

void part_join(unordered_map<unsigned_key, set<vector<int>>> &index,vector<pair<unsigned_key,unsigned_key>> &match_order);

void single_check(unordered_map<unsigned_key, set<vector<int>>> &index,set<pair<int,int>> &single,Graph &data);

void get_other_cand(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,Graph &data, Graph &query,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_set<int>> &others);
void get_others_table(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table,int length);

#endif //KCM_ALG_H


