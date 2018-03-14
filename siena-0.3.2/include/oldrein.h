#ifndef OLD_REIN_H
#define OLD_REIN_H
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"
#include <map>
#include<siena/IO.h>
#include<siena/SENP.h>
#include<siena/SienaIO.h>
#include<siena/Socket.h>
#include<siena/Siena.h>
#include<siena/SimpleSiena.h>
#include<siena/URI.h>
#include<siena.h>
using namespace std;

const int OLD_MAX_SUBS = 2000001;
const int OLD_MAX_ATTS = 101;
const int OLD_MAX_BUCKS = 1000;

class oldRein{
    int valDom, buckStep, bucks;
    vector<Combo> data[OLD_MAX_ATTS][2][OLD_MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
    map<string,bool> bitset;
public:
    oldRein(int _valDom, int buck_num){ valDom = _valDom; buckStep = (valDom - 1) / buck_num + 1; bucks = (valDom - 1) / buckStep + 1; }
    void insert(Sub &sub);
    void insert(IntervalSub &sub);
    void match(const Pub &pub, int &matchSubs, const vector<Sub> &subList);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
    void match(const Pub &pub, string myuri);
};

#endif
