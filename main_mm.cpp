#include <iostream>
#include "alg/alg.h"
#include "alg/multi_Thread.h"
#include "structure/Graph.h"
#include <unordered_set>
#include <map>
#include <bitset>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <cmath>

pair<long,int>  do_func(const string &query_path, const string &data_path);

using namespace std;
int main(int argc, char* argv[]) {

    string query_path,data_path;
    ifstream inputFile("../path");
    ofstream out("../ret");
    if(inputFile.is_open() && out.is_open()){
        getline(inputFile,data_path);
        if (!data_path.empty() && data_path[data_path.length() - 1] == '\r') {
            data_path.erase(data_path.length() - 1);
        }
        while(getline(inputFile,query_path)){
            if (!query_path.empty() && query_path[query_path.length() - 1] == '\r') {
                query_path.erase(query_path.length() - 1);
            }
            auto ret = do_func(query_path,data_path);
            out<<query_path<<" :"<<ret.first<<" ,"<<ret.second<<" ,mis "<<ret.second *4 <<endl;
        }
        inputFile.close();
        out.close();
    }else{
        cout<<"open path or ret file error"<<endl;
    }

    return 0;

}

pair<long,int> do_func(const string &query_path, const string &data_path) {
    auto* query = new Graph();
    query->read_graph(query_path);

    auto* data = new Graph();
    data->read_graph(data_path);

    // 开始计时
    auto startTime = std::chrono::high_resolution_clock::now();

    //query
    unordered_set<int> kernel;
    unordered_map<int,unordered_set<int>> comm;
    //special 核心节点之间的单边
    set<pair<int,int>> special;
    unordered_map<int,unordered_set<int>> others;
    get_info(*query,kernel,comm,special,others);

    //data
    unordered_map<int,unordered_set<int>> kernel_index;
    unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> comm_index;
    unordered_map<int,unordered_map<int,unordered_map<int,unordered_set<int>>>> other_cand;
    pro_nodes(*query,*data,kernel,comm,kernel_index,comm_index,other_cand,others);

    unordered_map<int,unordered_map<int,vector<vector<int>>>> others_table;
    get_others_table(other_cand,others_table,(*query).count_v);

    vector<vector<pair<unsigned_key,unsigned_key>>> match_order_level;
    vector<unordered_map<unsigned_key,unsigned_key>> matches;
    auto final_key = pre_match_order_level(match_order_level,matches,comm,special);

    int cc = 0;
    for(auto i :match_order_level){
        cout<<"level "<<cc<<endl;
        for(auto j : i){
            cout<<j.first<<"+"<<j.second<<"-->"<<(j.first|j.second) <<" "<<(j.first&j.second)<<endl;
        }
        ++cc;
    }
    unordered_map<unsigned_key,unordered_map<string,unordered_set<vector<int>,VectorHash>>> index;
    init_index(query->count_v,*data,kernel_index,comm_index,special,others_table,index,matches);

//    do_thread_level(index,match_order_level);
    part_join(index,match_order_level,matches);

    //join and check

//    //单线程
//    part_join(index,match_order);

//    //多线程 创建全部线程
//    do_thread(index,match_order);

    //多线程 按级别分布创建线程
//   do_thread_level(index,match_order_level);

//    index.clear();
//    index[0].insert({342,1138,769,818});

//    single_check(index,single,*data);

    // 结束计时
    auto endTime = std::chrono::high_resolution_clock::now();
    // 计算运行时间
    auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);

    //out
//    ofstream out("../output.txt");
//    if(out.is_open()){
//        out<< "程序运行时间：" << duration.count() <<" microseconds"<< endl;
//
//        //wirte complete match_table
//        int count = 0;
//        for(const auto& ll: index[final_key]){
//            for(auto l : ll.second){
//                ++count;
//                for(auto i: l){
//                    out<<i<<" ";
//                }
//                out<<endl;
//            }
//        }
//        out<<"match count: "<<count<<endl;
//        out.close();
//        cout<<"=============="<<endl;
//        cout<<"find match count: "<<count<<endl;
//        cout<<"time: "<<duration.count()<<" microseconds"<<endl;
//        cout<<"finished work"<<endl;
//    }else{
//        cerr<<"unable to open the file"<<endl;
//    }

    //test
//    int ccc = 0;
//    for(const auto& ll: index[final_key]){
//        for(auto l : ll.second){
//            unordered_set<int> set;
//            for(int i = 0; i< l.size(); ++i){
//                if(set.count(l[i])>0){
//                    cout<<"error"<<endl;
//                    return ccc;
//                }else{
//                    set.insert(l[i]);
//                }
//                if(data->label[l[i]] != query->label[i]){
//                    cout<<"error"<<endl;
//                    return 222;
//                }
//                for(auto nei: query->adj[i]){
//                    if(query->label[nei] != data->label[l[nei]]){
//                        cout<<"error"<<endl;
//                        return 333;
//                    }
//                }
//            }
//            ++ccc;
//
//        }
//    }
    int c_ret = 0;
    for(auto i: index[final_key]){
        c_ret += i.second.size();
    }
    return {(long)duration.count(),c_ret};
}
