#include "include/generator.h"

using namespace std;

void generator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        //cout << "gen sub" << i << endl;
        Sub sub = GenOneSub(i);

        subList.push_back(sub);
    }

    ofstream fileStream;
    fileStream.open("./Detail/subList.txt");
    for (int i=0; i<subs; ++i)
    {
        Sub &sub = subList[i];
        fileStream << "sub" << sub.uri << "\t" << sub.size << "\t";
        for (int j=0; j<sub.size; ++j)
            fileStream << sub.constraints[j].att << OPR[sub.constraints[j].op] << sub.constraints[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}


Sub generator::GenOneSub(int id)
{
    Sub sub;
    sub.uri = host+to_string(id);
    sub.size = random(k) + 1;
    if (subAttDis == 0)
        GenUniformAtts(sub);
    else if (subAttDis==1)
        GenZipfAtts(sub);
    if (subValDis == 0)
        GenUniformValues(sub);
    else if (subValDis == 1)
        GenZiefValues(sub);
    return sub;
}

void generator::GenUniformAtts(Sub &sub)
{
    vector<int> a;
    for (int i=0; i<sub.size; ++i)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
        Cnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void generator::GenZipfAtts(Sub &sub)
{
    vector<int> a;
    first = true;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(attDom, subAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, subAttalpha);
        a.push_back(x);
        Cnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void generator::GenUniformValues(Sub &sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(valDom);
        sub.constraints[i].value = x;
        double y = rand()/(RAND_MAX + 1.0);
        if (y <equalRatio)
            sub.constraints[i].op = 0;
        else if (y>(1+equalRatio)/2)
            sub.constraints[i].op = 2;
        else
            sub.constraints[i].op = 1;
    }
}

void generator::GenZiefValues(Sub &sub)
{
    first = true;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(valDom, subValalpha);
        sub.constraints[i].value = x;
        double y = rand()/(RAND_MAX + 1.0);
        if (y <equalRatio)
            sub.constraints[i].op = 0;
        else if (y>(1+equalRatio)/2)
            sub.constraints[i].op = 2;
        else
            sub.constraints[i].op = 1;
    }
}


void generator::GenPubList()
{
    for (int i = 0; i < pubs; i++)
    {
        //cout << "gen pub" << i << endl;
        Pub pub = GenOnePub();
        pubList.push_back(pub);
    }
    ofstream fileStream;
    fileStream.open("./Detail/pubList.txt");
    for (int i = 0; i < pubs; i++)
    {
        Pub &pub = pubList[i];
        fileStream << "pub" << i << "\t";
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].att << "=" << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}


Pub generator::GenOnePub()
{
    Pub pub;
    pub.size = m;
    if (pubAttDis == 0)
        GenUniformAtts(pub);
    else if (pubAttDis == 1)
        GenZipfAtts(pub);
    if (pubValDis == 0)
        GenUniformValues(pub);
    else if (pubValDis == 1)
        GenZiefValues(pub);
    return pub;
}

void generator::GenUniformAtts(Pub &pub)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void generator::GenZipfAtts(Pub &pub)
{
    first = true;
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(attDom, pubAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, pubAttalpha);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}


void generator::GenUniformValues(Pub &pub)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(valDom);
        pub.pairs[i].value = x;
    }
}

void generator::GenZiefValues(Pub &pub)
{
    first = true;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(valDom, pubValalpha);
        pub.pairs[i].value = x;
    }
}






//*********************intervalGenerator*******************************



void intervalGenerator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        IntervalSub sub = GenOneSub(i);

        subList.push_back(sub);
    }

    ofstream fileStream;
    fileStream.open("./Detail/subList.txt");
    for (int i=0; i<subs; ++i)
    {
        IntervalSub &sub = subList[i];
        fileStream << "sub" << sub.uri<< "\t" << sub.size << "\t";
        for (int j=0; j<sub.size; ++j)
            fileStream << sub.constraints[j].att << "=" << sub.constraints[j].lowValue << "-" << sub.constraints[j].highValue << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}


IntervalSub intervalGenerator::GenOneSub(int id)
{
    IntervalSub sub;
    sub.uri = host+to_string(id);
    sub.size = random(k) + 1;
    if (subAttDis == 0)
        GenUniformAtts(sub);
    else if (subAttDis==1)
        GenZipfAtts(sub);
    if (subValDis == 0)
        GenUniformValues(sub);
    else if (subValDis == 1)
        GenZiefValues(sub);
    return sub;
}

void intervalGenerator::GenUniformAtts(IntervalSub &sub)
{
    vector<int> a;
    for (int i=0; i<sub.size; ++i)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
        IntervalCnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void intervalGenerator::GenZipfAtts(IntervalSub &sub)
{
    vector<int> a;
    first = true;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(attDom, subAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, subAttalpha);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void intervalGenerator::GenUniformValues(IntervalSub &sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        if (width<1){
            int x = random( int(valDom * (1.0 - width)) );
            int y = x + int(valDom * width);
            sub.constraints[i].lowValue = x;
            sub.constraints[i].highValue = y;
        }
        else {
            int x = random(valDom-1);
            int y = random(valDom-1);
            if (x<y)
            {
                sub.constraints[i].lowValue = x;
                sub.constraints[i].highValue = y;
            }
            else{
                sub.constraints[i].lowValue = y;
                sub.constraints[i].highValue = x;
            }
        }

    }
}

void intervalGenerator::GenZiefValues(IntervalSub &sub)
{
    first = true;
    int Dom = int(valDom * (1.0 - width));
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(Dom, subValalpha);
        int y = x + int(valDom * width);
        sub.constraints[i].lowValue = x;
        sub.constraints[i].highValue = y;
    }
}


void intervalGenerator::GenPubList()
{
    for (int i = 0; i < pubs; i++)
    {
        Pub pub = GenOnePub();
        pubList.push_back(pub);
    }

    ofstream fileStream;
    fileStream.open("./Detail/pubList.txt");
    for (int i = 0; i < pubs; i++)
    {
        Pub &pub = pubList[i];
        fileStream << "pub" << i << "\t";
        for (int j=0; j<pub.size; ++j)
            fileStream << pub.pairs[j].att << "=" << pub.pairs[j].value << "\t";
        fileStream << "\n";
    }
    fileStream.close();
}


Pub intervalGenerator::GenOnePub()
{
    Pub pub;
    pub.size = m;
    if (pubAttDis == 0)
        GenUniformAtts(pub);
    else if (pubAttDis == 1)
        GenZipfAtts(pub);
    if (pubValDis == 0)
        GenUniformValues(pub);
    else if (pubValDis == 1)
        GenZiefValues(pub);
    return pub;
}

void intervalGenerator::GenUniformAtts(Pub &pub)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(attDom);
        while (CheckExist(a,x))
            x = random(attDom);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void intervalGenerator::GenZipfAtts(Pub &pub)
{
    first = true;
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(attDom, pubAttalpha);
        while (CheckExist(a,x))
            x = zipfDistribution(attDom, pubAttalpha);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}


void intervalGenerator::GenUniformValues(Pub &pub)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(valDom);
        pub.pairs[i].value = x;
    }
}

void intervalGenerator::GenZiefValues(Pub &pub)
{
    first = true;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(valDom, pubValalpha);
        pub.pairs[i].value = x;
    }
}


int generator::zipfDistribution(int n, double alpha)
{
    // alpha > 0
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value = 0;            // Computed exponential value to be returned
    int i;                     // Loop counter

    if (first)
    {
        for (i = 1; i <= n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    while (true)
    {
        // Pull a uniform random number (0 < z < 1)
        do
        {
            z = (double) rand() / RAND_MAX;
        } while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i = 1; i <= n; i++)
        {
            sum_prob = sum_prob + c / pow((double) i, alpha);
            if (sum_prob >= z)
            {
                zipf_value = i;
                break;
            }
        }

        if (zipf_value >= 1 && zipf_value <= n)
            return int(zipf_value - 1);
    }
}

bool generator::CheckExist(vector<int> a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

int intervalGenerator::zipfDistribution(int n, double alpha)
{
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value=0;            // Computed exponential value to be returned
    int    i;                     // Loop counter
    if (first)
    {
        for (i=1; i<=n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    while(true)
    {
        // Pull a uniform random number (0 < z < 1)
        do {
            z = (double) rand() / RAND_MAX;
        } while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i = 1; i <= n; i++) {
            sum_prob = sum_prob + c / pow((double) i, alpha);
            if (sum_prob >= z) {
                zipf_value = i;
                break;
            }
        }

        //cout << zipf_value << endl;
        if (zipf_value>=1&&zipf_value<=n)
            return int(zipf_value-1);

    }
}

bool intervalGenerator::CheckExist(vector<int> a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

int generator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}

int intervalGenerator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}

/*
void intervalGenerator::GenzipfPubList() {
    ofstream pubDetail;
    pubDetail.open("pubDetail.txt");
    pubDetail << "pubID" << "\tpair\n";

    //srand(time(NULL));

    vector<int> attsVec;
    vector<int> valuesVec;

    first = true;
    c = 0;

    for (int i = 0; i < pubs; i++) {
        vector<int> a;
        for (int j = 0; j < m; j++) {
            int x = zipfDistribution(atts, alpha);
            while (CheckExist(a, x))
                x = zipfDistribution(atts, alpha);
            a.push_back(x);
            attsVec.push_back(29-x);
        }
    }

    //cout << "check1" << endl;
    first = true;
    c = 0;
    int limit = pubs * m;
    for (int i=0; i<limit; i++){
        valuesVec.push_back(zipfDistribution(valDom, alpha));
    }

    //cout << "check2" << endl;

    int k = 0;

    for (int i = 0; i < pubs; i++) {
        Pub pub;
        pub.size = m;
        for (int j = 0; j < m; j++) {
            Pair tmp;
            tmp.att = attsVec[k];
            tmp.value = (valuesVec[k]+400000)%valDom;
            pub.pairs.push_back(tmp);
            ++k;
        }
        pubList.push_back(pub);
    }

    for (int i = 0; i < pubs; i++)
    {
        pubDetail << i << "\t";
        for (size_t j=0; j< m; ++j)
        {
            pubDetail << pubList[i].pairs[j].att << "=" << pubList[i].pairs[j].value << " ";
        }
        pubDetail << "\n";
    }

    pubDetail.close();
}

void intervalGenerator::GenUniformSubList() {
    ofstream subDetail;
    subDetail.open("subList.txt");
    subDetail << "subID" << "\tcons\n";

    vector<int> attsVec;
    vector<int> valuesVec;

    for (int i = 0; i < subs; i++) {
        vector<int> a;
        for (int j = 0; j < cons; j++) {
            int x = random(atts);
            while (CheckExist(a, x))
                x = random(atts);
            a.push_back(x);
            attsVec.push_back(x);
        }
    }

    int limit = subs * cons;
    for (int i=0; i<limit; i++)
        valuesVec.push_back(random( int(valDom * (1.0 - width)) ));


    int k=0;
    for (int i = 0; i < subs; i++) {
        IntervalSub sub;
        sub.id = i;
        sub.size = cons;
        for (int j = 0; j < cons; j++) {
            IntervalCnt tmp;
            tmp.att = attsVec[k];
            tmp.lowValue = valuesVec[k];
            tmp.highValue = valuesVec[k] + int(valDom * width);
            sub.constraints.push_back(tmp);
            ++k;
        }
        subList.push_back(sub);
    }

    for (int i = 0; i < subs; i++)
    {
        subDetail << subList[i].id << "\t";
        for (size_t j=0; j<subList[i].constraints.size(); ++j)
            subDetail << subList[i].constraints[j].att << ":" << subList[i].constraints[j].lowValue << "-" << subList[i].constraints[j].highValue << " ";
        subDetail << "\n";
    }
    subDetail << flush;
    subDetail.close();

}
*/