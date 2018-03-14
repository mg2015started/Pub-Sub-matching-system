#include "rein.h"

void Rein::insert(IntervalSub &sub)
{
    int buckStep = LvBuckStep[sub.size-1];
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCnt cnt = sub.constraints[i];
        Combo c;
        c.val = cnt.lowValue;
        c.uri = sub.uri;
        data[sub.size-1][cnt.att][0][c.val / buckStep].push_back(c);
        c.val = cnt.highValue;
        data[sub.size-1][cnt.att][1][c.val / buckStep].push_back(c);
    }
    bitset[sub.size-1].insert(make_pair(sub.uri,0));
}

void Rein::match(const Pub &pub, string  myuri)
{

    for (int siIndex=0; siIndex<Dom && siIndex<pub.size; ++siIndex)
    {
        map<string,bool> &kbitset = bitset[siIndex];
        for (map<string,bool>::iterator it=kbitset.begin(); it!=kbitset.end(); ++it)
            it->second = 0;
        for (int i = 0; i < pub.size; i++)
        {
            //Timer t0;
            int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / LvBuckStep[siIndex];
            int bucks = LvBuck[siIndex];
            vector<Combo> &data_0 = data[siIndex][att][0][buck];
            //pretime += t0.elapsed_nano()/1000000.0;

            //Timer t1;
            int data_0_size = data_0.size();
            for (int k = 0; k < data_0_size; k++)
                if (data_0[k].val > value)
                    kbitset[data_0[k].uri] = true;

            //searchtime += t1.elapsed_nano()/1000000.0;

            //Timer t2;
            for (int j = buck + 1; j < bucks; j++){
                vector<Combo> &data_1 = data[siIndex][att][0][j];
                //if (data_1.empty()) break;
                int data_1_size = data_1.size();
                for (int k = 0; k < data_1_size; k++)
                    kbitset[data_1[k].uri] = true;
            }
            //travesetime += t2.elapsed_nano()/1000000.0;

            //Timer t3;
            vector<Combo> &data_2 = data[siIndex][att][1][buck];
            int data_2_size = data_2.size();
            for (int k = 0; k < data_2_size; k++)
                if (data_2[k].val < value)
                    kbitset[data_2[k].uri] = true;
            //searchtime += t3.elapsed_nano()/1000000.0;

            //Timer t4;
            for (int j = buck - 1; j >= 0; j--){
                vector<Combo> &data_3 = data[siIndex][att][1][j];
                //if (data_3.empty()) break;
                int data_3_size = data_3.size();
                for (int k = 0; k < data_3_size; k++)
                    kbitset[data_3[k].uri] = true;
            }
            //travesetime += t4.elapsed_nano()/1000000.0;
        }

        //Timer t5;
        for (map<string,bool>::iterator it=kbitset.begin(); it!=kbitset.end(); ++it)
            if(!(it->second)){
                URI destination = it->first;
                iostream *s;
                s = destination.open_connection();
                Request r;
                r[SENP::Method] = SENP::PUB;
                r[SENP::Publisher] = myuri;
                *s << r << pub << flush;
                //string reply;
                //*s >> reply;
                //delete(s);
            }
        //tagtime += t5.elapsed_nano()/1000000.0;
    }

}
