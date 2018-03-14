//
// Created by mg2015started on 18-1-25.
//

#include "include/EventServer.h"
using namespace std;

const string EventServer::Version = "0.3.2";

void EventServer::publish(Request &r, Pub &pub)
{
    //LOGENTRY("publish r=" << r << " pub=" << pub << endl);
    //if (ev.empty()) return;
/*
    URI sender;
    try {
        Request::const_iterator ri = r.find(SENP::From);
        if (ri != r.end() && (*ri).second.type() == Siena_string)
            sender = (*ri).second.string_value();
    } catch (BadURI &bu) {
        //
        // I just ignore this exception
        //
    }
*/
    rein.match(pub, myuri);
    /*
    iostream *s;
    s = subscriber.open_connection();
    Request r2;
    r2[SENP::Method] = SENP::PUB;
    //if (myuri != NULL) {
    r2[SENP::Publisher] = myuri;
    //}
    *s << r2 << pub << endl << flush;
    //string reply;
    //*s >> reply;
    //delete(s);
    //rein.match(pub, myuri);
     */
}

void EventServer::subscribe(Request &r, IntervalSub &sub)
{
    //LOGENTRY("subscribe r=" << r << " sub=" << sub << endl);
    rein.insert(sub);

    //if (f.empty()) return;
    //URI U(r[SENP::Subscriber].string_value());
    //rein.insert(sub, myuri);
}

void EventServer::initialize(const string & mu, int _valDom, int k, int buck_num)
{
    //
    // initializes the server without neighbor servers
    //
    myuri = mu;
    rein.initialize(_valDom,k,buck_num);
}

/*
void connect_peer(Request &r)
{
    LOGENTRY("connect_peer r=" << r << endl);

    set<URI>::iterator new_uri
            = neighbors.insert(URI(r[SENP::Peer].string_value())).first;

    r[SENP::Method]=SENP::SUB;
    r[SENP::Subscriber]=myuri;
    set<Node*>::iterator yi;
    for(yi = root.postset.begin(); yi != root.postset.end(); ++yi)
        write_to_siena((*new_uri), r, );
    db_save_neighbors();
}

void forward_peer(const URI & u, const Filter &f)
{
    for (int i=0; i<neighbors.size(); ++i)
        neighbors[i]->subscribe(const URI & u, const Filter &f);
}
*/
