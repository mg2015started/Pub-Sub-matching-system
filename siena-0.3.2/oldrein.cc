#include "oldrein.h"
/*
void oldRein::insert(Sub sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        Cnt cnt = sub.constraints[i];
        Combo c;
        c.val = cnt.value;
        c.uri = sub.uri;
        if (cnt.op == 0)        // ==
        {
            data[cnt.att][0][c.val / buckStep].push_back(c);
            data[cnt.att][1][c.val / buckStep].push_back(c);
        }
        else if (cnt.op == 1)   // >=
            data[cnt.att][0][c.val / buckStep].push_back(c);
        else                    // >=
            data[cnt.att][1][c.val / buckStep].push_back(c);
    }
}
 */

void oldRein::insert(IntervalSub &sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCnt cnt = sub.constraints[i];
        Combo c;
        c.val = cnt.lowValue;
        c.uri = sub.uri;
        data[cnt.att][0][c.val / buckStep].push_back(c);
        c.val = cnt.highValue;
        data[cnt.att][1][c.val / buckStep].push_back(c);
    }
}

void oldRein::match(const Pub &pub, string myuri)
{
    for (map<string,bool>::iterator it=bitset.begin(); it!=bitset.end(); ++it)
        it->second = 0;
    for (int i = 0; i < pub.size; i++)
    {
        //Timer t0;
        int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
        vector<Combo> &data_0 = data[att][0][buck];
        //pretime += t0.elapsed_nano()/1000000.0;

        //Timer t1;
        int data_0_size = data_0.size();
        for (int k = 0; k < data_0_size; k++)
            if (data_0[k].val > value)
                bitset[data_0[k].uri] = true;
        //searchtime += t1.elapsed_nano()/1000000.0;

        //Timer t2;
        for (int j = buck + 1; j < bucks; j++){
            vector<Combo> &data_1 = data[att][0][j];
            int data_1_size = data_1.size();
            for (int k = 0; k < data_1_size; k++)
                bitset[data_1[k].uri] = true;
        }
        //travesetime += t2.elapsed_nano()/1000000.0;

        //Timer t3;
        vector<Combo> &data_2 = data[att][1][buck];
        int data_2_size = data_2.size();
        for (int k = 0; k < data_2_size; k++)
            if (data_2[k].val < value)
                bitset[data_2[k].uri] = true;
        //searchtime += t3.elapsed_nano()/1000000.0;

        //Timer t4;
        for (int j = buck - 1; j >= 0; j--)
        {
            vector<Combo> &data_3 = data[att][1][j];
            int data_3_size = data_3.size();
            for (int k = 0; k < data_3_size; k++)
                bitset[data_3[k].uri] = true;
        }
        //travesetime += t4.elapsed_nano()/1000000.0;

    }

    //Timer t5;
    for (map<string,bool>::iterator it=bitset.begin(); it!=bitset.end(); ++it)
        if (!(it->second)){
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
/*
    f0 << pretime << "\n";
    f1 << searchtime << "\n";
    f2 << travesetime << "\n";
    f3 << tagtime << "\n";
    f0.close();
    f1.close();
    f2.close();
*/
}
