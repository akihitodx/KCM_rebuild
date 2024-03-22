#include "multi_thread.h"


void part_join_thread(unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,vector<unordered_map<unsigned_key,unsigned_key>> &matches){
    for(int level = 0; level< match_order_level.size(); ++level){
        unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> level_index;
        vector<thread> threads(match_order_level[level].size());
        for(int i  = 0; i<threads.size();++i){
            threads[i] = thread(thread_join,ref(match_order_level[level][i]), ref(index), ref(matches),level, ref(level_index));
        }
        for(auto &th: threads){
            th.join();
        }
        for(const auto& i: level_index){
            index.erase(i.first);
            index[i.first] = i.second;
        }
    }
}

void thread_join(pair<unsigned_key,unsigned_key> &match,unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<unordered_map<unsigned_key,unsigned_key>> &matches,int level,unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &level_index){
    auto new_key = match.first | match.second;
    int first_size = 0,second_size = 0;
    vector<int> first_table = *index[match.first].begin()->second.begin();
    vector<int> second_table= *index[match.second].begin()->second.begin();
    for(int i = 0; i<first_table.size(); ++i){
        if(first_table[i]!= -1) ++first_size;
        if(second_table[i]!= -1) ++second_size;
    }
    vector<string> need_join;
    for(auto &i: index[match.first]){
        if(index[match.second].count(i.first)>0){
            need_join.push_back(i.first);
        }
    }
    vector<thread> threads(need_join.size());
    vector<int> connect;
    if(first_size < second_size){
        for(int i = 0; i<first_table.size(); ++i){
            //first 该位存在值 且second 该位不存在值
            if(first_table[i] != -1 && second_table[i] == -1){
                connect.push_back(i);
            }
        }
        for(int i = 0; i<need_join.size(); ++i){
            threads[i] = thread(do_thread, ref(index[match.first][need_join[i]]),ref(index[match.second][need_join[i]]),
                                ref(level_index),ref(connect), ref(matches),level,new_key);
        }
    }else{
        for(int i = 0; i<second_table.size(); ++i){
            if(second_table[i] != -1 && first_table[i] == -1){
                connect.push_back(i);
            }
        }
        for(int i = 0; i<need_join.size(); ++i){
            threads[i] = thread(do_thread, ref(index[match.second][need_join[i]]),ref(index[match.first][need_join[i]]),
                                ref(level_index), ref(connect),ref(matches),level,new_key);
        }
    }
    for(auto &th : threads){
        th.join();
    }
}

void do_thread(unordered_set<vector<int>,VectorHash> &aaa,unordered_set<vector<int>,VectorHash> &bbb,unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &level_index,vector<int> &connect,vector<unordered_map<unsigned_key,unsigned_key>> &matches,int level,unsigned_key new_key){
    //aaa短 bbb长 以bbb为基准 将aaa的值复制到bbb
    for(auto const &a_table: aaa){
        unordered_map<string, unordered_set<vector<int>, VectorHash>> temp_map;
        for(auto const &b_table: bbb){
            bool flag = true;
            auto temp = b_table;
            unordered_set<int> uni(temp.begin(), temp.end());
            uni.erase(-1);
            for (auto i: connect) {
                if (uni.count(a_table[i]) == 0) {
                    temp[i] = a_table[i];
                    uni.insert(a_table[i]);
                } else {
                    flag = false;
                    break;
                }

            }
            if (flag) {
                auto temp_index = to_key_index(matches,level+1,new_key,temp);
                temp_map[temp_index].insert(temp);
            }
        }
        for(auto i: temp_map){
            if(level_index[new_key].count(i.first)>0){
                level_index[new_key][i.first].insert(i.second.begin(), i.second.end());
            }else{
                level_index[new_key].insert(temp_map.begin(), temp_map.end());
                break;
            }
        }
    }
}