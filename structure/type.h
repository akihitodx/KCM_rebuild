#ifndef KCM_REBUILD_TYPE_H
#define KCM_REBUILD_TYPE_H

#include <bitset>

//根据querySize 调整数据类型
typedef unsigned int unsigned_key;


struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};

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

#endif //KCM_REBUILD_TYPE_H
