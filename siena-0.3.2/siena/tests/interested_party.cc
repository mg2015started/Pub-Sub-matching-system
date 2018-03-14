// -*- C++ -*-
//
//  This file is part of Siena, a wide-area event notification system.
//  See http://www.cs.colorado.edu/serl/dot/siena.html
//
//  Author: Antonio Carzaniga <carzanig@cs.colorado.edu>
//  See the file AUTHORS for full details. 
//
//  Copyright (C) 1998-1999 University of Colorado
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
// $Id: interested_party.cc,v 1.2 2002/11/22 17:52:35 carzanig Exp $
//

//
// this is an example of an interested party, that is, a consumer of
// notifications
//

#include <iostream>
#include <exception>

using namespace std;
//
// this gives you access to the Siena headers (and to the Siena namespace)
//
#include <siena.h>

SimpleSiena * S;

//
// here's a simple interested party 
//
class TemperatureMonitor : public Notifiable {
public:
    virtual void	notify(Event &);
};

void TemperatureMonitor::notify(Event &e)
{
    //
    // gets the (int) value of the "temperature" attribute warning:
    // this might generate an exception in case that attribute doesn't
    // exist or if it is of a different type.
    //
    int temp = e["temperature"];

    //
    // does something with it
    //
    if (temp < 75) {
	cout << "temperature is ok: " << temp << endl;
    } else if (temp > 100) {
	cout << temp << " is way too hot!" << endl;
	//
	// this will cause the main loop to terminate
	//
	S->exit_loop();
    } else {
	cout << temp << " is hot!" << endl;
    }
}

void print_usage(const char *programname) 
{
    cerr << "Usage: " << programname << " [options...]\n\
options:\n\
\t-port number\n\
\t-connect URI\n\
" << endl;

    exit(-1);
}

int main (int argc, char *argv[])
{
    string		server	= "senp://localhost:1969";
    unsigned short	port	= 0;	// Any port

    for (int i=1; i< argc; ++i) {	// parses command-line parameters
	if (strcmp(argv[i], "-port")==0) {
	    if (++i < argc) {
		port = atoi(argv[i]);
	    } else {
		print_usage(argv[0]);
	    }
	} else if (strcmp(argv[i], "-connect")==0) {
	    if (++i < argc) {
		server = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} 
	else {
	    print_usage(argv[0]);
	}
    }

    S = new SimpleSiena(server, port);
    //                    ^       ^
    //                   /         \ 
    //          Siena server     local port (to receive notifications)
    //
    
    // 
    // creates a filter for the subscription
    //
    Filter f; 
    f.add_constraint("temperature", Siena_gt, 0);
    //
    // you could also input a filter from any input stream, e.g.:
    // istrstream is("filter { temperature > 0 }");
    // is >> f;
    //

    TemperatureMonitor T;

    try {
	S->subscribe(f, &T);	// subscribes T
	S->main_loop();		// enter the loop... waiting for notifications
	S->disconnect(&T);	// this unsubscribes T
    } catch (exception &ex) {
	cerr << "error: " << ex.what() << endl;
    }
    delete(S);
}
