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

void do_thread_level(unordered_map<unsigned_key, set<vector<int>>> &index,vector<vector<pair<unsigned_key,unsigned_key>>> &match_order_level){
    for(auto level : match_order_level){
        vector<thread> threads(level.size());
        for(int i = 0; i< threads.size(); i++){
            threads[i] = thread(partial_join_level,level[i].first,level[i].second, ref(index));
        }
        for(auto& th : threads){
            th.join();
        }
    }
}

void partial_join_level(unsigned_key first, unsigned_key second ,unordered_map<unsigned_key, set<vector<int>>> &index){
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

