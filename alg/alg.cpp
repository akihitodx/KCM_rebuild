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
            if(count_kernel.size() == 1 && count_comm.size()== 0){
                others[*count_kernel.begin()].insert(i);
            }else if(count_comm.size()>0){
                kernel.insert(i);
                for(auto c: count_comm){
                    comm[c].insert(i);
                }
            }
        }
    }
    //check kernel to comm
    unordered_map<int,unordered_set<int>> kernel_nei_kernel;
    int loc = -1;
    int max_count = 0;
    for(auto i: kernel){
        int count  = 0;
        for(auto nei: graph.adj[i]){
            if(kernel.count(nei) > 0 ){
                kernel_nei_kernel[i].insert(nei);
            }
        }
    }
    while(!kernel_nei_kernel.empty()){
        int max = kernel_nei_kernel.begin()->first;
        for(auto i: kernel_nei_kernel){
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

void pre_match_order_level(vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level,unordered_map<unsigned_key,unsigned_key> &matches,unordered_map<int,unordered_set<int>> &comm){
    unordered_set<unsigned_key> keys;
    for(auto const &com: comm){
        auto &kernels = com.second;
        for (auto it1 = kernels.begin(); it1 != kernels.end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != kernels.end(); ++it2) {
                keys.insert(1<< *it1 | 1 << *it2 | 1<<com.first);
            }
        }
    }

    unordered_set<unsigned_key> new_keys;
    for(auto & level : match_order_level){
        while(keys.size()>1){
            auto xx = *keys.begin();
            unsigned_key yy;
            keys.erase(xx);
            for(auto i: keys){
                if(xx & i){
                    new_keys.insert(xx | i);
                    level.emplace_back(xx,i);
                    matches[xx] = xx & i;
                    matches[i] = xx & i;
                    yy = i;
                    break;
                }
            }
            keys.erase(yy);
        }
        keys.insert(new_keys.begin(), new_keys.end());
        new_keys.clear();
    }

}

void init_index(int querySize,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table, unordered_map<unsigned_key,unordered_map<string,vector<vector<int>>>> &index,unordered_map<unsigned_key,unsigned_key> &matches){
    for(auto key: matches){
        //对于每一个簇 进行插入操作 但在插入的时候 要根据matches.second值 组合创建索引
    }
}
//这效率要死了
void init_index(int query_graph_length,unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &comm_index,unordered_map<unsigned_key ,set<vector<int>>> &index,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table){
    for(const auto& query_com: comm_index){
        for(auto data_com = query_com.second.begin(); data_com!=query_com.second.end(); ++data_com){
            for(auto a_pair = data_com->second.begin(); a_pair != data_com->second.end(); ++a_pair){
                auto temp_it = a_pair;
                for(auto b_pair = ++temp_it; b_pair != data_com->second.end(); ++b_pair){
                    for(auto i: a_pair->second){
                        for(auto j: b_pair->second){
                            auto aa = *(a_pair);
                            auto bb = *(b_pair);
                            unsigned_key key =  (1<<query_com.first) | (1<<a_pair->first) | (1<<b_pair->first);

                            if(others_table.count(a_pair->first)>0 && others_table.count(b_pair->first)==0){
                                //a in others && b not in others
                                for(auto tt:others_table[a_pair->first]){
                                    if(tt.first != i) continue;
                                    for(auto temp: tt.second){
                                        temp[b_pair->first] = j;
                                        temp[query_com.first] = data_com->first;
                                        index[key].insert(temp);
                                    }
                                }
                            }else if(others_table.count(a_pair->first)==0 && others_table.count(b_pair->first)>0){
                                //b in others && a not in others
                                for(auto tt:others_table[b_pair->first]){
                                    if(tt.first != j) continue;
                                    for(auto temp: tt.second){
                                        temp[a_pair->first] = i;
                                        temp[query_com.first] = data_com->first;
                                        index[key].insert(temp);
                                    }
                                }
                            }else if(others_table.count(a_pair->first)==0 && others_table.count(b_pair->first)==0){
                                //a & b all not in others
                                vector<int> temp(query_graph_length,-1);
                                temp[a_pair->first] = i;
                                temp[b_pair->first] = j;
                                temp[query_com.first] = data_com->first;
                                index[key].insert(temp);
                            }else{
                                //a & b all in others
                                for(auto aaa: others_table[a_pair->first]){
                                    if(aaa.first != i) continue;
                                    for(auto a_temp:aaa.second){
                                        for(auto bbb:others_table[b_pair->first]){
                                            if(bbb.first != j) continue;
                                            for(auto b_temp:bbb.second){
                                                for(int loc = 0;loc<b_temp.size();++loc){
                                                    if(a_temp[loc] != -1){
                                                        b_temp[loc] = a_temp[loc];
                                                    }
                                                }
                                                b_temp[query_com.first] = data_com->first;
                                                index[key].insert(b_temp);
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



//此步存在重复问题 但后续会有功能可以去重
void init_index_special(int query_graph_length, unordered_map<int,unordered_set<int>> &kernel_index, set<pair<int,int>> special,unordered_map<unsigned_key ,set<vector<int>>> &index, Graph &data,unordered_map<int,unordered_map<int,vector<vector<int>>>> &others_table){
    for(auto spe: special){
        auto spe_a_index = kernel_index[spe.first];
        auto spe_b_index = kernel_index[spe.second];
        unsigned_key key = (1<<spe.first) | (1<<spe.second);
        index[key] = {};
        for(auto node: spe_a_index){
            for(auto nei: data.adj[node]){
                if(spe_b_index.count(nei) > 0){
                    if(others_table.count(spe.first) > 0 && others_table.count(spe.second) == 0){
                        //node in  & nei not in
                        for(auto table : others_table[spe.first]){
                            if(table.first != node) continue;
                            for(auto temp: table.second){
                                temp[spe.second] = nei;
                                index[key].insert(temp);
                            }
                        }
                    }else if(others_table.count(spe.first) == 0 && others_table.count(spe.second) > 0){
                        //node not in & nei in
                        for(auto table : others_table[spe.second]){
                            if(table.first != nei) continue;
                            for(auto temp: table.second){
                                temp[spe.first] = node;
                                index[key].insert(temp);
                            }
                        }
                    }else if(others_table.count(spe.first) == 0 && others_table.count(spe.second) == 0){
                        //node not in & nei not in
                        vector<int> temp(query_graph_length,-1);
                        temp[spe.first] = node;
                        temp[spe.second] = nei;
                        index[key].insert(temp);
                    }else{
                        // node in and nei in
                        for(auto aaa: others_table[spe.first]){
                            if(aaa.first != node) continue;
                            for(auto a_temp:aaa.second){
                                for(auto bbb:others_table[spe.second]){
                                    if(bbb.first != nei) continue;
                                    for(auto b_temp:bbb.second){
                                        for(int loc = 0;loc<b_temp.size();++loc){
                                            if(a_temp[loc] != -1){
                                                b_temp[loc] = a_temp[loc];
                                            }
                                        }
                                        index[key].insert(b_temp);
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

struct CompareByBitCount {
    bool operator()(unsigned_key a,unsigned_key b) const {
        // 计算a和b中的1的位数
        int countA = std::bitset<sizeof(unsigned_key) * 8>(a).count();
        int countB = std::bitset<sizeof(unsigned_key) * 8>(b).count();

        // 比较元素中的1的位数
        if (countA != countB) {
            return countA < countB;
        } else {
            return a < b;
        }
    }
};
unsigned_key init_match_order(unordered_map<unsigned_key, set<vector<int>>> &index,vector<pair<unsigned_key,unsigned_key>> &match_order){
    set<unsigned_key,CompareByBitCount> keys;
    for(auto i : index){
        keys.insert(i.first);
    }
    while(keys.size() > 1){
        unordered_set<unsigned_key> add;
        unordered_set<unsigned_key> temp;
        for(auto i : keys){
            if(temp.empty()){
                temp.insert(i);
                continue;
            }
            bool flag = false;
            unsigned_key tt;
            for(auto t: temp){
                if(t & i){
                    add.insert(t | i);
                    index[t | i] = { };
                    tt = t;
                    match_order.emplace_back(t,i);
                    flag = true;
                    break;
                }
            }
            if(!flag){
                temp.insert(i);
            }else{
                temp.erase(tt);
            }
        }
        keys.clear();
        keys.insert(temp.begin(),temp.end());
        keys.insert(add.begin(),add.end());
    }
    return *keys.begin();
//    match_order.emplace_back(*keys.begin(),*(keys.begin()++));

}

void init_match_order_level(unordered_map<unsigned_key, set<vector<int>>> &index, vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level){
    unordered_set<unsigned_key> keys;
    for(auto i : index){
        keys.insert(i.first);
    }
    unordered_set<unsigned_key> new_keys;
    for(int level = 0; level< match_order_level.size(); ++level){
        while(keys.size()>1){
            auto xx = *keys.begin();
            unsigned_key yy;
            keys.erase(xx);
            for(auto i: keys){
                if(xx & i){
                    new_keys.insert(xx | i);
                    match_order_level[level].emplace_back(xx,i);
                    yy = i;
                    break;
                }
            }
            keys.erase(yy);
        }
        keys.insert(new_keys.begin(), new_keys.end());
        new_keys.clear();
    }

}

void part_join(unordered_map<unsigned_key, set<vector<int>>> &index,vector<pair<unsigned_key,unsigned_key>> &match_order){
    for(auto order: match_order){
        auto index_a = index[order.first];
        auto index_b = index[order.second];
        unsigned_key new_match = order.first | order.second;
        index[new_match]={};
        //get same pos
        unsigned_key match = order.first & order.second;
        vector<int> poss;
        int pos = 0;
        while(match>0){
            if(match & 1){
                poss.push_back(pos);
            }
            match>>=1;
            ++pos;
        }
        //iter
        int x= 0,y= 0;
        for(auto aa: index_a){
            ++x;
            for(auto bb: index_b){
                ++y;
                cout<<x<<" "<<y<<endl;
                bool flag = true;
                for(auto loc: poss){
                    if(aa[loc] != bb[loc]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    vector<int> temp = aa;
                    for(int i = 0;i < bb.size(); ++i){
                        if(bb[i] != -1){
                            temp[i] = bb[i];
                        }
                    }
                    //unique value
                    unordered_set<int> in;
                    bool in_flag = true;
                    for(auto i : temp){
                        if(i != -1){
                            if(in.count(i) < 1){
                                in.insert(i);
                            }else{
                                in_flag = false;
                                break;
                            }
                        }
                    }
                    if(in_flag){
                        index[new_match].insert(temp);
                    }
                }
            }
        }
//        index.erase(order.first);
//        index.erase(order.second);
        index[order.first].clear();
        index[order.second].clear();
        cout<<"====="<<endl;
    }
}

void single_check(unordered_map<unsigned_key, set<vector<int>>> &index,set<pair<int,int>> &single,Graph &data){
    auto &el = index.begin()->second;
    for(auto it = el.begin(); it != el.end() ;){
        bool flag = true;
        for(auto spe: single){
            auto v_a = (*it)[spe.first];
            auto v_b = (*it)[spe.second];
            if(data.adj[v_a].count(v_b) == 0){
                flag = false;
                break;
            }
        }
        if(flag){
            ++it;
        }else{
            it = el.erase(it);
        }
    }
}

void get_other_cand(unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> &other_cand,Graph &data, Graph &query,unordered_map<int,unordered_set<int>> &kernel_index,unordered_map<int,unordered_set<int>> &others){
    for(auto kernel: kernel_index){
        if(others.count(kernel.first)>0){
            for(auto node: kernel.second){
                for(auto nei: data.adj[node]){
                    for(auto other: others[kernel.first]){
                        if(data.label[nei] == query.label[other]){
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
    for(auto query_kernel: other_cand){
        for(auto other: query_kernel.second){
            vector<int> temp(length,-1);
            temp[query_kernel.first] = other.first;
            vector<vector<int>> table;
            generateCombinations(other.second,temp,table,other.second.begin());
            others_table[query_kernel.first][other.first] = table;
        }
    }

}