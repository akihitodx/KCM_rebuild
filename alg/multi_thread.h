#ifndef KCM_REBUILD_MULTI_THREAD_H
#define KCM_REBUILD_MULTI_THREAD_H
#include "alg.h"
#include <thread>

void part_join_thread(unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,vector<unordered_map<unsigned_key,unsigned_key>> &matches);

void thread_join(pair<unsigned_key,unsigned_key> &match,unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<unordered_map<unsigned_key,unsigned_key>> &matches,int level,unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &level_index);

void do_thread(unordered_set<vector<int>,VectorHash> &aaa,unordered_set<vector<int>,VectorHash> &bbb,unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &level_index,vector<int> &connect,vector<unordered_map<unsigned_key,unsigned_key>> &matches,int level,unsigned_key new_key);
#endif //KCM_REBUILD_MULTI_THREAD_H
