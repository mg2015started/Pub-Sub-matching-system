//
// Created by mg2015started on 18-1-25.
//

#ifndef SIENASERVER_0_3_2_NEWSERVER_H
#define SIENASERVER_0_3_2_NEWSERVER_H
#include "rein.h"
//#include <oldrein.h>
#include <iostream>
#include <set>
#include <map>

#include "siena/Siena.h"
#include "siena/URI.h"
#include "siena/Monitor.h"
#include "siena/data_structure.h"
#include "siena/SienaIO.h"
#include "siena/SENP.h"

class EventServer {
public:
    static const string Version;
    //
    // This is the interface of the Event Server
    // these functions should not throw exceptions, i.e., they should
    // deal with exceptions themselves.
    //
    void		publish(Request &r, Pub &pub);
    void		subscribe(Request &r, IntervalSub &sub);
    //void		unsubscribe(Request &r, Filter &f);
    //void		connect_peer(Request &);

    void		shutdown();
    //
    // initialize takes the URI of the first peer connection as a
    // parameter, I'm not sure this is the right thing to do. I will
    // probably change this.
    //
    void		initialize(const string &my_uri,
                           const string &peer_uri);
    void		initialize(const string &my_uri, int _valDom, int k, int buck_num);


private:
    Rein rein;
    vector<string> neighbors;
    string myuri;
    //URI subscriber;
};


#endif //SIENASERVER_0_3_2_NEWSERVER_H
