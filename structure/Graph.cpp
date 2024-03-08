//
// Created by DELL on 2024/1/3.
//

#include <string>
#include <iostream>
#include <fstream>
#include "Graph.h"

void Graph::read_graph(const string& path) {
    ifstream ifs;
    ifs.open(path);
    if(!ifs.is_open()){
        cout <<path <<" open failed"<<endl;
        return;
    }
    vector<int> nei;
    char type;
    int a ,b ,c;
    ifs>>type;
    while(type=='v'){
        ifs>>a>>b>>c;
        label.push_back(b);
        ++count_v;
        ifs>>type;
    }
    this->degree.resize(this->count_v,0);
    this->adj.resize(this->count_v);
    this->nei_label.resize(this->count_v);
    while(!ifs.eof()){
        ifs>>a>>b>>c;
        this->adj[a].insert(b);
        this->adj[b].insert(a);
        this->nei_label[a].insert(label[b]);
        this->nei_label[b].insert(label[a]);
        ++count_e;
        ifs>>type;
    }
    for(int i = 0 ;i<adj.size(); ++i){
        degree[i] = (int)adj[i].size();
    }
}

