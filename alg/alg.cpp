#include "alg.h"
#include "../structure/Graph.h"
#include <iostream>
#include <queue>
#include <algorithm>
#include <bitset>
#include <unistd.h>

using namespace std;

struct cmps {
    bool operator()(int n1,int n2) {
        return n1>n2;
    }
};

void get_info(Graph &graph,unordered_set<int> &kernel,unordered_map<int,unordered_set<int>> &comm,set<pair<int,int>> &special, unordered_map<int,unordered_set<int>> &others){
    map<int,int> mm;
    vector<int> temp_comm;
    for(int i = 0;i<graph.count_v;++i){
        mm[i] = graph.degree[i];
    }
    //init kernel
    while(!mm.empty()){
        int max_degree_id = mm.begin()->first;
        for(auto i: mm){
            if(i.second> mm[max_degree_id]){
                max_degree_id = i.first;
            }
        }
        if(mm[max_degree_id] == 0) break;
        mm.erase(max_degree_id);
        kernel.insert(max_degree_id);
        for(auto i: graph.adj[max_degree_id]){
            if(mm.count(i)<=0) continue;
            if(--mm[i]<=0){
                mm.erase(i);
                temp_comm.push_back(i);
            }
        }
    }
    // init comm
    for(auto i: temp_comm){
        vector<int> count;
        for(auto nei : graph.adj[i]){
            if(kernel.count(nei)>0){
                count.push_back(nei);
            }
        }
        if(count.size()>1){
            comm.insert(pair<int,unordered_set<int>>(i,unordered_set<int>(count.begin(),count.end())));
        }
    }
    //init others
    for(int i = 0;i<graph.count_v; ++i){
        if(kernel.count(i) == 0 && comm.count(i) == 0 ){
            unordered_set<int> count_kernel;
            unordered_set<int> count_comm;
            for(auto nei: graph.adj[i]){
                if (kernel.count(nei)>0){
                    count_kernel.insert(nei);
                }
                if(comm.count(nei)>0){
                    count_comm.insert(nei);
                }
            }
            if(count_kernel.size() == 1 && count_comm.empty()){
                others[*count_kernel.begin()].insert(i);
            }else if(!count_comm.empty()){
                kernel.insert(i);
                for(auto c: count_comm){
                    comm[c].insert(i);
                }
            }
        }
    }
    //check kernel to comm
    unordered_map<int,unordered_set<int>> kernel_nei_kernel;
//    int loc = -1;
//    int max_count = 0;
    for(auto i: kernel){
//        int count  = 0;
        for(auto nei: graph.adj[i]){
            if(kernel.count(nei) > 0 ){
                kernel_nei_kernel[i].insert(nei);
            }
        }
    }
    while(!kernel_nei_kernel.empty()){
        int max = kernel_nei_kernel.begin()->first;
        for(const auto& i: kernel_nei_kernel){
            if(i.second.size() > kernel_nei_kernel[max].size()){
                max = i.first;
            }
        }

        if(kernel_nei_kernel[max].size() > 1){
            //up to comm
            comm[max] = kernel_nei_kernel[max];
            for(auto i: kernel_nei_kernel[max]){
                if(kernel_nei_kernel[i].size()==1){
                    kernel_nei_kernel.erase(i);
                }else{
                    kernel_nei_kernel[i].erase(max);
                }
            }
            kernel_nei_kernel.erase(max);
        }else{
            //special
            special.insert({max,*kernel_nei_kernel[max].begin()});
            kernel_nei_kernel.erase(*kernel_nei_kernel[max].begin());
            kernel_nei_kernel.erase(max);
        }
    }

}

void printGraph(Graph &graph){
    cout<<"label"<<endl;
    for(auto i: graph.label){
        cout<<i<<" ";
    }
    cout<<endl;
    cout<<"degree"<<endl;
    for(auto i : graph.degree){
        cout<<i<<" ";
    }
    cout<<endl;
    cout<<"adj"<<endl;
    for(int i = 0;i< graph.adj.size(); ++i ){
        cout<<i<<": ";
        for(auto j : graph.adj[i]){
            cout<<j<<" ";
        }
        cout<<endl;
    }
}

bool com_Match(multiset<int> &queryNode,multiset<int> &dataNode){
    // dataNode 包含 querNode
    return includes(dataNode.begin(),dataNode.end(),queryNode.begin(),queryNode.end());
}

void pro_nodes(Graph &query,Graph &data,unordered_set<int> &kernel,unordered_map<int,unordered_set<int>> &comm,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,unordered_map<int,unordered_set<int>> &others){
    //prepro kernel
    unordered_set<int> kernel_set;
    for(int data_id = 0; data_id< data.count_v; ++data_id){
        for(auto kernel_id : kernel){
            if(query.label[kernel_id] == data.label[data_id] && query.degree[kernel_id] <= data.degree[data_id]){
                if(com_Match(query.nei_label[kernel_id],data.nei_label[data_id])){
                    kernel_index[kernel_id].insert(data_id);
                }
            }
        }
    }
    //prepro comm
    //存在效率问题 可优化
    for(int data_id = 0; data_id<data.count_v; ++data_id){
        // com comm的一项 first连通点 second 可连接的核心节点们
        for(auto com: comm){
            if(query.label[com.first] == data.label[data_id] && query.degree[com.first] <= data.degree[data_id]){
                unordered_map<int,unordered_map<int,unordered_set<int>>> temp;
                bool flag = true;
                for(auto n_com: com.second){
                    //n_com 每一个可连接的核心节点
                    for(auto nei: data.adj[data_id]){
                        //数据图节点的每一个邻居
                        if(kernel_index[n_com].count(nei)>0){
                            temp[data_id][n_com].insert(nei);
                        }
                    }
                    if(temp[data_id][n_com].empty()){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    comm_index[com.first][temp.begin()->first] = temp.begin()->second;
                }
            }
        }
    }
    //init others
    get_other_cand(other_cand,data,query,kernel_index,others);

}

unsigned_key pre_match_order_level(vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,vector<unordered_map<unsigned_key,unsigned_key>> &matches,unordered_map<int,unordered_set<int>> &comm,set<pair<int,int>> &special){
    set<unsigned_key, CompareByBitCount> keys;
    unsigned_key mask = 1;
    for(auto const &com: comm){
        auto &kernels = com.second;
        for (auto it1 = kernels.begin(); it1 != kernels.end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != kernels.end(); ++it2) {
                keys.insert(mask<< *it1 | mask << *it2 | mask<<com.first);
            }
        }
    }
    for(auto spe: special){
        keys.insert(mask<<spe.first| mask <<spe.second);
    }

    unordered_set<unsigned_key> new_keys;
    vector<pair<unsigned_key,unsigned_key>> temp;
    unordered_map<unsigned_key,unsigned_key> temp_map;
    while(keys.size()>1){
        while(keys.size()>1){
            auto xx = *keys.begin();
            unsigned_key yy;
            keys.erase(xx);
            for(auto i: keys){
                if(xx & i){
                    new_keys.insert(xx | i);
                    temp.emplace_back(xx,i);
                    temp_map[xx] = xx & i;
                    temp_map[i] = xx & i;
                    yy = i;
                    break;
                }
            }
            keys.erase(yy);
        }
        keys.insert(new_keys.begin(), new_keys.end());
        new_keys.clear();
        match_order_level.push_back(temp);
        temp.clear();
        matches.push_back(temp_map);
        temp_map.clear();
    }
    temp_map[*keys.begin()]  = *keys.begin();
    matches.push_back(temp_map);
    return *keys.begin();
}

string to_key_index(vector<unordered_map<unsigned_key,unsigned_key>> &matches,int level,unsigned_key new_key, const vector<int> &match_table){
    string res;
    unsigned_key key;
    for(int i = level; i<matches.size();++i){
        if(matches[i].count(new_key)>0){
            key = matches[i][new_key];
            break;
        }
    }
    unsigned_key mask =  1;
    for(int i = 0; i<sizeof(unsigned_key)* 8; ++i){
        if(key & mask){
            res += to_string(match_table[i]) +"/";
        }
        mask <<= 1;
    }
    return res;
}
unsigned_key find_key(unsigned_key key, vector<unordered_map<unsigned_key,unsigned_key>> &matches){
    int level = 0;
    for(;level<matches.size();++level){
        if(matches[level].count(key)>0){
            break;
        }
    }
    return matches[level][key];
}
void init_index(int querySize,Graph &data,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,set<pair<int,int>> &special,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table, unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<unordered_map<unsigned_key,unsigned_key>> &matches){
    unsigned_key mask = 1;
    for(const auto& query_com: comm_index){
        for(auto data_com = query_com.second.begin(); data_com!=query_com.second.end(); ++data_com){
            for(auto a_pair = data_com->second.begin(); a_pair != data_com->second.end(); ++a_pair){
                auto temp_it = a_pair;
                for(auto b_pair = ++temp_it; b_pair != data_com->second.end(); ++b_pair){
                    for(auto i: a_pair->second){
                        for(auto j: b_pair->second){
                            unsigned_key key =  (mask<<query_com.first) | (mask<<a_pair->first) | (mask<<b_pair->first);
                            if(others_table.count(a_pair->first)>0 && others_table.count(b_pair->first)==0){
                                //a in others && b not in others
                                for(const auto& tt:others_table[a_pair->first]){
                                    if(tt.first != i) continue;
                                    for(const auto& temp_ori: tt.second){
                                        auto temp = temp_ori;
                                        unordered_set<int> ver(temp_ori.begin(), temp_ori.end());
                                        if(ver.count(j)>0 || ver.count(data_com->first)>0) continue;
                                        temp[b_pair->first] = j;
                                        temp[query_com.first] = data_com->first;

                                        auto key_index = to_key_index(matches,0,key,temp);
                                        index[key][key_index].insert(temp);
                                    }
                                }
                            }else if(others_table.count(a_pair->first)==0 && others_table.count(b_pair->first)>0){
                                //b in others && a not in others
                                for(const auto& tt:others_table[b_pair->first]){
                                    if(tt.first != j) continue;
                                    for(const auto& temp_ori: tt.second){
                                        auto temp = temp_ori;
                                        unordered_set<int> ver(temp_ori.begin(), temp_ori.end());
                                        if(ver.count(i)>0 || ver.count(data_com->first)>0) continue;
                                        temp[a_pair->first] = i;
                                        temp[query_com.first] = data_com->first;
                                        auto key_index = to_key_index(matches,0,key,temp);
                                        index[key][key_index].insert(temp);
                                    }
                                }
                            }else if(others_table.count(a_pair->first)==0 && others_table.count(b_pair->first)==0){
                                //a & b all not in others
                                if(i == j) continue;
                                vector<int> temp(querySize,-1);
                                temp[a_pair->first] = i;
                                temp[b_pair->first] = j;
                                temp[query_com.first] = data_com->first;
                                auto key_index = to_key_index(matches,0,key,temp);
                                index[key][key_index].insert(temp);
                            }else{
                                //a & b all in others
                                for(const auto& aaa: others_table[a_pair->first]){
                                    if(aaa.first != i) continue;
                                    for(const auto& a_temp_ori:aaa.second){
                                        for(const auto& bbb:others_table[b_pair->first]){
                                            if(bbb.first != j) continue;
                                            for(const auto& b_temp_ori:bbb.second){
                                                auto a_temp = a_temp_ori;
                                                auto b_temp = b_temp_ori;
                                                for(int loc = 0;loc<b_temp.size();++loc){
                                                    if(a_temp[loc] != -1){
                                                        b_temp[loc] = a_temp[loc];
                                                    }
                                                }
                                                b_temp[query_com.first] = data_com->first;
                                                unordered_set<int> ver;
                                                bool flag = true;
                                                for(auto el : b_temp){
                                                    if (el != -1 && ver.count(el) == 0){
                                                        ver.insert(el);
                                                    }else{
                                                        flag = false;
                                                        break;
                                                    }
                                                }
                                                if(flag){
                                                    auto key_index = to_key_index(matches,0,key,b_temp);
                                                    index[key][key_index].insert(b_temp);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for(const auto &spe: special){
        auto spe_a_index = kernel_index[spe.first];
        auto spe_b_index = kernel_index[spe.second];
        unsigned_key key = (1<<spe.first) | (1<<spe.second);
        for(auto node: spe_a_index){
            for(auto nei: data.adj[node]){
                if(spe_b_index.count(nei) > 0){
                    if(others_table.count(spe.first) > 0 && others_table.count(spe.second) == 0){
                        //node in  & nei not in
                        for(const auto& table : others_table[spe.first]){
                            if(table.first != node) continue;
                            for(const auto& temp_ori: table.second){
                                auto temp = temp_ori;
                                temp[spe.second] = nei;
                                auto key_index = to_key_index(matches,0,key,temp);
                                index[key][key_index].insert(temp);
                            }
                        }
                    }else if(others_table.count(spe.first) == 0 && others_table.count(spe.second) > 0){
                        //node not in & nei in
                        for(const auto& table : others_table[spe.second]){
                            if(table.first != nei) continue;
                            for(const auto& temp_ori: table.second){
                                auto temp = temp_ori;
                                temp[spe.first] = node;
                                auto key_index = to_key_index(matches,0,key,temp);
                                index[key][key_index].insert(temp);
                            }
                        }
                    }else if(others_table.count(spe.first) == 0 && others_table.count(spe.second) == 0){
                        //node not in & nei not in
                        vector<int> temp(querySize,-1);
                        temp[spe.first] = node;
                        temp[spe.second] = nei;
                        auto key_index = to_key_index(matches,0,key,temp);
                        index[key][key_index].insert(temp);
                    }else{
                        // node in and nei in
                        for(const auto& aaa: others_table[spe.first]){
                            if(aaa.first != node) continue;
                            for(const auto& a_temp_ori:aaa.second){
                                for(const auto& bbb:others_table[spe.second]){
                                    if(bbb.first != nei) continue;
                                    for(const auto& b_temp_ori:bbb.second){
                                        auto a_temp = a_temp_ori;
                                        auto b_temp = b_temp_ori;
                                        for(int loc = 0;loc<b_temp.size();++loc){
                                            if(a_temp[loc] != -1){
                                                b_temp[loc] = a_temp[loc];
                                            }
                                        }
                                        auto key_index = to_key_index(matches,0,key,b_temp);
                                        index[key][key_index].insert(b_temp);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void part_join(unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> &index,vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,vector<unordered_map<unsigned_key,unsigned_key>> &matches){
    for(int level = 0; level < match_order_level.size();++level){
        unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> level_index;
        for(auto match: match_order_level[level]){
            auto first = match.first;
            auto second = match.second;
            auto new_key  = first | second;
            vector<int> connect;
            unsigned_key mask = 1;
            int first_size = 0,second_size = 0;
            vector<int> first_table = *index[first].begin()->second.begin();
            vector<int> second_table= *index[second].begin()->second.begin();
            for(int i = 0; i<first_table.size(); ++i){
                if(first_table[i]!= -1) ++first_size;
                if(second_table[i]!= -1) ++second_size;
            }
            if(first_size < second_size){
                for(int i = 0; i<first_table.size(); ++i){
                    //first 该位存在值 且second 该位不存在值
                    if(first_table[i] != -1 && second_table[i] == -1){
                        connect.push_back(i);
                    }
                }
                for(const auto& ele_a: index[first]){
                    if (index[second].count(ele_a.first) > 0){
                        unordered_map<string,unordered_set<vector<int>,VectorHash>> temp_map;
                        for(const auto& a_table: ele_a.second){
                            for(const auto& b_table: index[second][ele_a.first]){
                                bool flag = true;
                                auto temp = b_table;
                                unordered_set<int> uni(temp.begin(), temp.end());
                                uni.erase(-1);
                                for(auto i : connect){
                                    if(uni.count(a_table[i])==0){
                                        temp[i] = a_table[i];
                                        uni.insert(a_table[i]);
                                    }else{
                                        flag = false;
                                        break;
                                    }
                                }
                                if(flag){
                                    auto temp_index = to_key_index(matches,level+1,new_key,temp);
                                    temp_map[temp_index].insert(temp);
                                }
                            }
                        }
                        level_index[new_key].insert(temp_map.begin(), temp_map.end());
                    }
                }
            }else {
                //first位置为基准
                for(int i = 0; i<second_table.size(); ++i){
                    //first 该位存在值 且second 该位不存在值
                    if(second_table[i] != -1 && first_table[i] == -1){
                        connect.push_back(i);
                    }
                }
                for (const auto &ele_b: index[second]) {
                    if (index[second].count(ele_b.first) > 0) {
                        unordered_map<string, unordered_set<vector<int>, VectorHash>> temp_map;
                        for (const auto &b_table: ele_b.second) {
                            for (const auto &a_table: index[first][ele_b.first]) {
                                bool flag = true;
                                auto temp = a_table;
                                unordered_set<int> uni(temp.begin(), temp.end());
                                uni.erase(-1);
                                for (auto i: connect) {
                                    if (uni.count(b_table[i]) == 0) {
                                        temp[i] = b_table[i];
                                        uni.insert(b_table[i]);
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
                        }
                        level_index[new_key].insert(temp_map.begin(), temp_map.end());
                    }
                }
            }

        }
        for(const auto& i: level_index){
            index.erase(i.first);
            index[i.first] = i.second;
        }

    }
}

void get_other_cand(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,Graph &data, Graph &query,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_set<int>> &others){
    for(const auto& kernel: kernel_index){
        if(others.count(kernel.first)>0){
            for(auto node: kernel.second){
                for(auto nei: data.adj[node]){
                    for(auto other: others[kernel.first]){
                        if(data.label[nei] == query.label[other] ){
                            other_cand[kernel.first][node][other].insert(nei);
                        }
                    }
                }
            }
        }
    }
}

void generateCombinations(const std::unordered_map<int, std::unordered_set<int>>& myMap, std::vector<int>& currentCombination,vector<vector<int>> &table, const std::unordered_map<int, std::unordered_set<int>>::iterator& it) {
    // 到达最后一个键，输出当前组合
    if (it == myMap.end()) {
        table.push_back(currentCombination);
        return;
    }

    // 遍历当前键对应的集合的所有元素
    for (int element : it->second) {
        currentCombination[it->first] = element;
        generateCombinations(myMap, currentCombination,table, std::next(it));  // 递归调用下一个键
        currentCombination[it->first] = -1;  // 移除当前元素，尝试下一个元素
    }
}


void get_others_table(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table,int length){
    for(const auto& query_kernel: other_cand){
        for(auto other: query_kernel.second){
            vector<int> temp(length,-1);
            temp[query_kernel.first] = other.first;
            vector<vector<int>> table;
            generateCombinations(other.second,temp,table,other.second.begin());
            others_table[query_kernel.first][other.first] = table;
        }
    }

}