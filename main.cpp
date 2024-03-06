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

using namespace std;

int main(int argc, char* argv[]) {

//    string query_path = "../test/demo1";
//    string query_path = "../test/demo2";
//    string query_path = "../test/data";
//    string query_path = "../test/query";
//    string data_path = "../test/data";
    string query_path = "../test/y4_6.graph";  //���������
//    string query_path = "../test/y_8.graph";
    string data_path ="../test/yeast.graph";

//    string query_path = "../test/human/query_graph/query_dense_4_3.graph";
//    string data_path ="../test/human/data_graph/human_temp.graph";

//        string query_path = "../test/data";
//        string data_path = "../test/data";

    auto* query = new Graph();
    query->read_graph(query_path);

    auto* data = new Graph();
    data->read_graph(data_path);

    // ��ʼ��ʱ
    auto startTime = std::chrono::high_resolution_clock::now();

    //query
    unordered_set<int> kernel;
    unordered_map<int,unordered_set<int>> comm;
    //special ���Ľڵ�֮��ĵ���
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

//    //���߳�
//    part_join(index,match_order);

//    //���߳� ����ȫ���߳�
//    do_thread(index,match_order);

    //���߳� ������ֲ������߳�
 //   do_thread_level(index,match_order_level);

//    index.clear();
//    index[0].insert({342,1138,769,818});

//    single_check(index,single,*data);

    // ������ʱ
    auto endTime = std::chrono::high_resolution_clock::now();
    // ��������ʱ��
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);


    //out

    ofstream out("../output.txt");
    if(out.is_open()){
        out<< "��������ʱ�䣺" << duration.count() <<" microseconds"<< std::endl;

        //wirte complete match_table
        int count = 0;
        for(const auto& ll: index[final_key]){
            for(auto l : ll.second){
                ++count;
                for(auto i: l){
                    out<<i<<" ";
                }
                out<<endl;
            }
        }
        out<<"match count: "<<count<<endl;
        out.close();

        cout<<"find match count: "<<count<<endl;
        cout<<"finished work"<<endl;
    }else{
        cerr<<"unable to open the file"<<endl;
    }


    return 0;

}
