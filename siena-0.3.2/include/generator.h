#ifndef _GENERATOR_H
#define _GENERATOR_H
#include "data_structure.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <cmath>
#include<siena/IO.h>
#include<siena/SENP.h>
#include<siena/SienaIO.h>
#include<siena/Socket.h>
#include<siena/Siena.h>
#include<siena/SimpleSiena.h>
#include<siena/URI.h>
#include<siena.h>

class generator {
    void GenUniformAtts(Sub &sub);

    void GenUniformAtts(Pub &pub);

    void GenZipfAtts(Sub &sub);

    void GenZipfAtts(Pub &pub);

    void GenUniformValues(Sub &sub);

    void GenZiefValues(Sub &sub);

    void GenUniformValues(Pub &pub);

    void GenZiefValues(Pub &pub);

    bool CheckExist(vector<int> a, int x);

    int random(int x);

    int zipfDistribution(int n, double alpha);

    Sub GenOneSub(int id);

    Pub GenOnePub();

    string OPR[3] = {"=", "<=", ">="};

public:
    vector<Sub> subList;
    vector<Pub> pubList;
    int subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom;
    double width, equalRatio, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha;
    int c;
    bool first;
    string host;
    generator(int subs, int pubs, int k, int m, int subAttDis, int subValDis, int pubAttDis, int pubValDis, int attDom, int valDom,
              double subAttalpha, double subValalpha, double pubAttalpha, double pubValalpha, double width, double equalRatio, string host):
            subs(subs), pubs(pubs), k(k), m(m), subAttDis(subAttDis), subValDis(subValDis), pubAttDis(pubAttDis), pubValDis(pubValDis), attDom(attDom), valDom(valDom),
            subAttalpha(subAttalpha) ,subValalpha(subValalpha) ,pubAttalpha(pubAttalpha), pubValalpha(pubValalpha), width(width), equalRatio(equalRatio), host(host)
    {
        srand(time(NULL));
    }


    void GenSubList();

    void GenPubList();
};




class intervalGenerator {
    void GenUniformAtts(IntervalSub &sub);

    void GenUniformAtts(Pub &pub);

    void GenZipfAtts(IntervalSub &sub);

    void GenZipfAtts(Pub &pub);

    void GenUniformValues(IntervalSub &sub);

    void GenZiefValues(IntervalSub &sub);

    void GenUniformValues(Pub &pub);

    void GenZiefValues(Pub &pub);
    bool CheckExist(vector<int> a, int x);

    int random(int x);

    int zipfDistribution(int n, double alpha);

    IntervalSub GenOneSub(int id);

    Pub GenOnePub();

public:
    vector<IntervalSub> subList;
    vector<Pub> pubList;
    int subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom;
    double width, equalRatio, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha;
    int c;
    bool first;
    string host;
    intervalGenerator(int subs, int pubs, int k, int m, int subAttDis, int subValDis, int pubAttDis, int pubValDis, int attDom, int valDom,
              double subAttalpha, double subValalpha, double pubAttalpha, double pubValalpha, double width, double equalRatio, string host):
            subs(subs), pubs(pubs), k(k), m(m), subAttDis(subAttDis), subValDis(subValDis), pubAttDis(pubAttDis), pubValDis(pubValDis), attDom(attDom), valDom(valDom),
            subAttalpha(subAttalpha) ,subValalpha(subValalpha) ,pubAttalpha(pubAttalpha), pubValalpha(pubValalpha), width(width), equalRatio(equalRatio),host(host)
    {
        srand(time(NULL));
    }

    void GenSubList();

    void GenPubList();

    void GenUniformSubList();
    void GenzipfPubList();

};

#endif
