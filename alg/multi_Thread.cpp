#include "multi_Thread.h"

void do_thread(unordered_map<unsigned_key, set<vector<int>>> &index,vector<pair<unsigned_key,unsigned_key>> &match_order){
//    vector<mutex> mm(index.size());
    unordered_map<unsigned_key, mutex* > mm;
    for(auto i: index){
        mm[i.first] = new mutex;
    }
    vector<thread> threads(match_order.size());
    for(int i = 0; i< threads.size(); i++){
        threads[i] = thread(partial_join,match_order[i].first,match_order[i].second, ref(index), ref(mm));
    }
    for(auto& th : threads){
        th.join();
    }

    for(auto i: mm){
        delete i.second;
    }

}
void do_thread_level(unordered_map<unsigned_key,unordered_map<string,vector<vector<int>>>> &index,vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level){
    for(auto level : match_order_level){
        vector<thread> threads(level.size());
        for(int i = 0; i< threads.size(); i++){
            threads[i] = thread(partial_join_level,level[i].first,level[i].second,  ref(index));
        }
        for(auto& th : threads){
            th.join();
        }
    }
}

void partial_join_level(unsigned_key first, unsigned_key second ,unordered_map<unsigned_key,unordered_map<string,vector<vector<int>>>> &index){
    auto index_a = index[first];
    auto index_b = index[second];
    auto new_key  = first | second;
//    auto con_key  = first ^ second;
    vector<int> connect;
    unsigned_key mask = 1;
    for(int i = 0; i<sizeof(unsigned_key) * 8; ++i){
        //first 该位存在值 且second 该位不存在值
        if(first & mask && !(second & mask)){
            connect.push_back(i);
        }
        mask <<= 1;
    }

    for(const auto& ele_a: index_a){
        if(index_b.count(ele_a.first)>0){
            //可以GPU实现

            //CPU
            unordered_map<string,vector<vector<int>>> temp_map;
            for(const auto& a_table: ele_a.second){
                for(const auto& b_table: index_b[ele_a.first]){
                    auto temp = b_table;
                    for(auto i : connect){
                        temp[i] = a_table[i];
                    }
                    auto temp_index = to_key_index(new_key,temp);
                    temp_map[temp_index].push_back(temp);
                }
            }
            index[new_key].insert(temp_map.begin(), temp_map.end());
        }
    }
}


void partial_join(unsigned_key first, unsigned_key second,unordered_map<unsigned_key, set<vector<int>>> &index,unordered_map<unsigned_key, mutex*> &mm){
    unsigned_key res = first | second;
    while(true){
        if(!index[first].empty()){
            mm[first]->lock();
            break;
        }
    }
    while(true){
        if(!index[second].empty()){
            mm[second]->lock();
            break;
        }
    }
    mm[res]->lock();

    auto index_a = index[first];
    auto index_b = index[second];
    unsigned_key new_match = first | second;
    index[new_match]={};
    //get same pos
    unsigned_key match = first & second;
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
//        index.erase(first);
//        index.erase(second);
    index[first].clear();
    index[second].clear();

    mm[res]->unlock();
    mm[second]->unlock();
    mm[first]->unlock();
}

