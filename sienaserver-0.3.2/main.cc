// -*- C++ -*-
//
//  This file is part of Siena, a wide-area event notification system.
//  See http://www.cs.colorado.edu/serl/siena/
//
//  Author: Antonio Carzaniga <carzanig@cs.colorado.edu>
//  See the file AUTHORS for full details. 
//
//  Copyright (C) 1998-2002 University of Colorado
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
//  USA, or send email to serl@cs.colorado.edu.
//
//
// $Id: main.cc,v 1.6 2002/11/22 20:57:45 carzanig Exp $
//
#include <sienaserverconf.h>
#include <fstream>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h> // defines MAXHOSTNAMELEN
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>

#include <stdio.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#include <exception>

#include <siena.h>

#include "PThreads.h"
#include "include/EventServer.h"

#define SIENA_SERVER_PORT 1969

class Handler;

//
// server private functions 
//
static void		print_usage(const char *programname);

void print_usage(const char *programname) 
{
    cerr << "Siena server v." << EventServer::Version << endl;
    cerr << "Copyright (c) 1998-2002 University of Colorado at Boulder" << endl;
    cerr << "see http://www.cs.colorado.edu/users/serl/siena/" << endl;
    cerr << "Usage: " << programname << " [options...]\n\
options:\n\
\t-port number\n\
\t-identity URI\n\
\t-connect URI\n\
\t-monitor hostname\n\
\t-monitorport number\n\
\t-log filename\n\
\t-db filename\n\
\t-detach\n\
\t-servers number\n\
\t-strikeouts number\n\
" << endl;

    exit(-1);
}

//
// Event server structure (contains the subscription tables etc.)
//
static EventServer			EventService;
//
// ESLock 
//
static Lock				ESLock;
static Mutex				NCLock;
static Mutex				* LogLock;
static Condition			AcceptCond;
static Condition			ServeCond;

static list<Handler *>			Handlers;

static bool				ServerActive	= true;
//
// NewConnection is the buffer between the acceptor thread and the
// handler threads. The value of NewConnection can be:
// >= 0  --> new connection to handle
//           the acceptor suspends on AcceptCond
//           handlers are signaled, the first one to get the new connection
//	     sets it to -1
// -1    --> acceptor ready to accept
// -2    --> server must shutdown
//
#define NC_TO_ACCEPT	-1
#define NC_TO_SHUTDOWN	-2

static int				NewConnection	= NC_TO_ACCEPT;

static unsigned short			port		= SIENA_SERVER_PORT;
static const char *			peer_server	= NULL;
static const char *			my_uri		= NULL;
static const char *			log_file	= NULL;
static const char *			db_file		= NULL;
static bool				detach		= false;

static const char *			monitor		= NULL;
static unsigned short			monitor_port	= 1996;
static const char *			portstr		= NULL;

static int				handlers	= 3;

static list<Handler *>			HandlersList;

static TCPPort				Serverport;
static ofstream *			Logf		= NULL;
static struct timeval			Logtv; 

#ifdef NO_LOG
#define LOGENTRY(x)
#define LOGBEGIN(x)
#define LOGEND(x)
#else
#define LOGENTRY(x) {if(Logf != NULL) { \
    LogLock->lock(); gettimeofday(&Logtv,NULL); \
    *Logf << ctime(&(Logtv.tv_sec)) << x << endl << flush; LogLock->unlock();}}

#define LOGBEGIN(x) {if(Logf != NULL) { \
    LogLock->lock(); gettimeofday(&Logtv,NULL); \
    *Logf << ctime(&(Logtv.tv_sec)) << x << flush; }}

#define LOGEND(x) {if(Logf != NULL) { \
    *Logf << x << endl << flush; LogLock->unlock();}}
#define LOG(x) {if(Logf != NULL) { *Logf << x << flush; }}
#endif


void quit_main_loop(int) 
{
    ServerActive = false;
}

class Handler : public Thread {
    private:

    virtual void * run();
};

void * Handler::run()
{
    TCPSocket conn;
    bool connected;
    bool locked;
    Request r;

    while (true) {
	NCLock.lock();
	while(NewConnection == NC_TO_ACCEPT) 
	    ServeCond.wait(NCLock);
	
	if (NewConnection == NC_TO_SHUTDOWN) {
	    ServeCond.signal();
	    NCLock.unlock();
	    return 0;
	}

	conn = NewConnection; connected = true;

	NewConnection = NC_TO_ACCEPT;
	AcceptCond.signal();
	NCLock.unlock();
	
	locked = false;

	try {
	    LOGBEGIN("");
	    conn >> r;
	    LOG(r);
	    Request::iterator mi = r.find(SENP::Method);
	    if (mi == r.end()) {
		conn << SENP::RE_BadRequest << " missing method" << endl;
		conn.close(); connected = false; 
		LOGEND(SENP::RE_BadRequest << " missing method");
	    } else if ((*mi).second.type() != Siena_string) {
		conn << SENP::RE_BadRequest << " bad type for method" << endl;
		conn.close(); connected = false;		
		LOGEND(SENP::RE_BadRequest << " bad type for method");
	    } else { 
		const string & method = (*mi).second.string_value();
		if (method == SENP::PUB) {
		    Pub pub;
		    conn >> pub;
		    conn << SENP::RE_Ok << " publication accepted" << endl;
		    conn.close(); connected = false;
		    LOGEND(pub << SENP::RE_Ok);
		    ESLock.rlock(); locked = true;
            //LOG(r);
            //LOG(pub);
		    EventService.publish(r, pub);
		} else if (method == SENP::SUB) {
		    IntervalSub sub;
		    conn >> sub;
		    conn << SENP::RE_Ok << " subscription accepted" << endl;
		    conn.close(); connected = false;
		    LOGEND(sub << SENP::RE_Ok);
		    ESLock.wlock(); locked = true;
            //LOG(r);
            //LOG(sub);
		    EventService.subscribe(r,sub);
		}/* else if (method == SENP::UNS) {
			IntervalSub sub;
			conn >> sub;
		    conn << SENP::RE_Ok << " unsubscription accepted" << endl;
		    conn.close(); connected = false;
		    LOGEND(sub << SENP::RE_Ok);
		    ESLock.wlock(); locked = true;
		    EventService.unsubscribe(r,f);
		} else if (method == SENP::ADV) {
		    Filter f;
		    conn >> f;
		    conn << SENP::RE_Ignored << " advertisement accepted" << endl;
		    conn.close(); connected = false;
		    LOGEND(f << SENP::RE_Ignored);
		    ESLock.wlock(); locked = true;
		    EventService.advertise(r,f);
		} else if (method == SENP::UNA) {
		    Filter f;
		    conn >> f;
		    conn << SENP::RE_Ignored << " unadvertisement accepted" << endl;
		    conn.close(); connected = false;
		    LOGEND(f << SENP::RE_Ignored);
		    ESLock.wlock(); locked = true;
		    EventService.unadvertise(r,f);
		} else if (method == SENP::HLO) {
		    conn << SENP::RE_Ok << " connection accepted" << endl;
		    conn.close(); connected = false;
		    LOGEND(SENP::RE_Ok);
		    ESLock.wlock(); locked = true;
		    EventService.connect_peer(r);
		} else if (method == SENP::BYE) {
		    //
		    // ...work in progress...
		    //
		    conn << SENP::RE_Ok << " goodbye" << endl;
		    conn.close(); connected = false;
		    LOGEND(SENP::RE_Ok);
		    ESLock.wlock(); locked = true;
		    EventService.unsubscribe_all(r);
		} else if (method == SENP::DBG) {
		    conn << SENP::RE_Ok << " server status ok" << endl;
		    ESLock.rlock(); locked = true;
		    EventService.print_poset(conn);
		    conn.close(); connected = false;
		    LOGEND(SENP::RE_Ok);
		} */else {
		    conn << SENP::RE_BadRequest << " can't handle request" << endl;
		    conn.close(); connected = false;
		    LOGEND(SENP::RE_BadRequest);
		}
	    }
	} catch (exception &ex) {
	    if (connected) {
		LOGEND(SENP::RE_Error << ' ' << ex.what());
		conn << SENP::RE_Error << ' ' << ex.what() << endl; 
		conn.close(); connected = false;
	    }
	}
	if (locked) {
	    ESLock.unlock(); 
	    locked = false;
	}
    }
    return 0; // unreachable statement
}

void main_loop()
{
    Thread::unblock_signal(SIGINT);
    Thread::unblock_signal(SIGTERM);

    while (true) {
	NCLock.lock();
	while(NewConnection != NC_TO_SHUTDOWN 
	      && NewConnection != NC_TO_ACCEPT) {
	    AcceptCond.wait(NCLock);
	}
	
	while (NewConnection == NC_TO_ACCEPT && ServerActive) {
	    try {
		NewConnection = Serverport.accept();
	    } catch (SocketError & se) {
		if (se.errno_value != EINTR) {
		    //
		    // pathetic exception handling...
		    //
		    NewConnection = NC_TO_SHUTDOWN;
		}
	    } catch (exception &ex) {
		NewConnection = NC_TO_SHUTDOWN;
	    }
	}
	if (!ServerActive || NewConnection == NC_TO_SHUTDOWN) break;

	ServeCond.signal();
	NCLock.unlock(); 
    }
    NewConnection = NC_TO_SHUTDOWN;
    ServeCond.signal();
    NCLock.unlock();
}

void detach_server()
{
    if (detach) {
	pid_t pid;
	switch((pid = fork())) {
	case -1:
	    cerr << "fork() failed!" << endl;
	    exit(-1);
	case 0: {
	    int i;
	    //
	    // child process ... detach from everything
	    //
	    // for some reason HPUX doesn't define TIOCNOTTY
	    // whatever... no time to look into this now
	    // ...work in progress... 
#ifdef HAVE_SETSID
	    setsid();
#else  /* !HAVE_SETSID */
#ifdef HAVE_GETPGRP
#ifdef GETPGRP_VOID /* POSIX.1 style */
	    /* lose controlling terminal and change process group */
	    setpgrp();
#else /* BSD style */
#ifdef HAVE_TIOCNOTTY
	    if ((i=open("/dev/tty", O_RDWR))>=0) {
		ioctl(i, TIOCNOTTY, 0);
		close(i);
	    }
	    setpgrp(0, getpid());        /* change process group */
#endif
#endif /* SETPGRP_VOID */
#endif /* HAVE_GETPGRP */
#endif /* !HAVE_SETSID */
#ifdef HAVE_GETDTABLESIZE
	    for(i=getdtablesize()-1; i>=0; --i)
		if (i != Serverport.filedescr())
		    close(i);
#endif
	    //
	    //  after closing *all* file descriptors, it is good
	    //  to open the usual stdin/out/err (0,1,2) on a null
	    //  device because many library functions assume these
	    //  fds to be valid.
	    //  Note that UNIX opens fds from 0 in ascending order.
	    //
	    i = open("/dev/null", O_RDWR);	/* 0 - stdin  */
	    dup(i);		
	    dup(i);
	    break;
	}
	default:
	    cout << pid << endl;
	    exit(0);
	}
    }
}

void parse_command_options(int argc, char *argv[])
{
    int i;

    portstr = getenv("MONITORPORT");
    monitor = getenv("MONITOR");

    for (i=1; i< argc; ++i) {		// parses command-line parameters
	if (strcmp(argv[i], "-port")==0) {
	    if (++i < argc) {
		port = atoi(argv[i]);
	    } else {
		print_usage(argv[0]);
	    }
	/*} else if (strcmp(argv[i], "-strikeouts")==0) {
	    if (++i < argc) {
		unsigned short strikeouts = atoi(argv[i]);
		EventService.attempts(strikeouts);
	    } else {
		print_usage(argv[0]);
	    }
	    */
	} else if (strcmp(argv[i], "-servers")==0) {
	    if (++i < argc) {
		handlers = atoi(argv[i]);
	    } else {
		print_usage(argv[0]);
	    }
	} else if (strcmp(argv[i], "-identity")==0) {
	    if (++i < argc) {
		my_uri = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} 
	else if (strcmp(argv[i], "-detach")==0) {
	    detach = true;
	} 
	else if (strcmp(argv[i], "-log")==0) {
	    if (++i < argc) {
		log_file = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	}
		/*
	else if (strcmp(argv[i], "-db")==0) {
	    if (++i < argc) {
		db_file = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	}
		 */
	else if (strcmp(argv[i], "-monitorport")==0) {
	    if (++i < argc) {
		portstr = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} 
 	else if (strcmp(argv[i], "-monitor")==0) {
	    if (++i < argc) {
		monitor = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} 
	else if (strcmp(argv[i], "-connect")==0) {
	    if (++i < argc) {
		peer_server = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} 
	else {
	    print_usage(argv[0]);
	}
    }

    if (portstr != NULL) monitor_port = atoi(portstr);
}

void open_log_file() {
    if (log_file != NULL) {
 	Logf = new ofstream(log_file);
 
 	if (!(*Logf)) {
 	    cerr << "can't open log file " << log_file << endl;
 	    delete(Logf);
 	    Logf = NULL;
  	    exit(1);
  	} else {
	    LogLock = new Mutex();
	}
    }
}

void init_event_service(int valDom,int k, int new_buck_num)
{
    string  myUri;

    //if (Logf != NULL) EventService.set_log(Logf);

    LOGENTRY("BEGIN");

    if (my_uri != NULL) {
	myUri = my_uri;
    } else {
	ostringstream ostr;
	ostr << "senp://" << INAddress::this_host().hostname() 
	     << ":" << Serverport.local_address().port();
	    
	myUri.assign(ostr.str());
    }

    //if (db_file != NULL) EventService.set_db(db_file);
    /*
	if (monitor != NULL) {
	try {
	    EventService.set_monitor(new SimpleMonitor(monitor, monitor_port));
	} catch (exception &ex) {
	    cerr << ex.what() << endl;
	}
    }
     */

    if (peer_server != NULL) {
	iostream *s;
		/*
	URI peer_uri(peer_server);
	cerr << "connecting to peer: " << peer_uri.to_string() << endl;
	try {
	    s = peer_uri.open_connection();
	} catch (SocketError &se) {
	    cerr << "errno: " << se.errno_value << endl;
	    throw;
	}
	Request r;
	r[SENP::Method] = SENP::HLO;
	r[SENP::Peer] = myUri;
	*s << r << endl << flush;
	string reply;
	*s >> reply;
	if (reply == SENP::RE_Ok) {
	    EventService.initialize(myUri, peer_uri.to_string());
	} else {
	    //
	    // should do something here ...work in progress...
	    //
	}
		 */
	delete(s);
    } else {
        LOGENTRY("BEGIN2");
	EventService.initialize(myUri,valDom,k, new_buck_num);  LOGENTRY("End");
    }
}

void create_handlers_pool()
{
    Handler * h;
    for(int i=handlers; i > 0 ; --i) {
	h = new Handler();
	Handlers.push_front(h);
	h->start();
    } 
}

void join_handlers_pool()
{
    list<Handler*>::iterator hli;
    for(hli = Handlers.begin(); hli != Handlers.end(); ++hli) {
	(*hli)->join();
	delete(*hli);
    }
}

int main(int argc, char *argv[])
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int attDom;           // Total number of attributes, i.e. dimensions.
    int k;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
    int subAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int subValDis;         // The distribution of values in subs and pubs. 0:uniform
    int pubAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int pubValDis;         // The distribution of values in subs and pubs. 0:uniform
    double subAttalpha;       // Parameter for Zipf distribution.
    double pubAttalpha;
    double subValalpha;
    double pubValalpha;
    double width;       // Width of a predicate.
    double equalRatio;
    int old_buck_num;
    int new_buck_num;

    ifstream parafile;
    parafile.open(string("paras_1")+string(".txt"));
    parafile >> subs >> pubs >> k >> m >> subAttDis >> subValDis >> pubAttDis >> pubValDis >> attDom >> valDom >> subAttalpha >>subValalpha >>  pubAttalpha >> pubValalpha >> width >> equalRatio >> old_buck_num >> new_buck_num;
    parafile.close();


    try {
	parse_command_options(argc, argv);
    	Serverport.bind(port);
	if (detach) detach_server();
	open_log_file();
        LOGENTRY("111111");
        LOGENTRY(valDom);
        LOGENTRY(k);
        LOGENTRY(new_buck_num);
	init_event_service(valDom,k, new_buck_num);
        LOGENTRY("1");

	create_handlers_pool();
        LOGENTRY("2");
	main_loop();
        LOGENTRY("3");
	join_handlers_pool();

	LOGENTRY("shutdown");
	//EventService.shutdown();
	Serverport.close();
	LOGENTRY("END");
	if (Logf != NULL) { delete(Logf); }
    } catch (exception &ex) {
	cerr << ex.what() << endl;
	return -1;
    }
    return 0;
};
