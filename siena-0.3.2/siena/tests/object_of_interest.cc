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
// $Id: object_of_interest.cc,v 1.2 2002/11/22 17:52:35 carzanig Exp $
//

//
// this is an example of an object of interest, that is, a producer of
// notifications
//

#include <iostream>
#include <exception>

using namespace std;

//
// this gives you access to all the Siena headers and to the Siena namespace
//
#include <siena.h>

SimpleSiena * S;

void print_usage(const char *programname) 
{
    cerr << "Usage: " << programname << " [options...]\n\
options:\n\
\t-connect URI\n\
" << endl;

    exit(-1);
}

int main (int argc, char *argv[])
{
    string		server	= "senp://localhost:1969";

    for (int i=1; i< argc; ++i) {	// parses command-line parameters
	if (strcmp(argv[i], "-connect")==0) {
	    if (++i < argc) {
		server = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} else {
	    print_usage(argv[0]);
	}
    }

    S = new SimpleSiena(server);
    //                    ^ 
    //                   /  
    //          Siena server
    //
    
    // 
    // creates an event
    //
    Event e;
    int temperature;
    try {
	//
	// reads some numbers from stdin and produces event notifications
	// 
	while(cin >> temperature) {
	    e["temperature"] = temperature;
	    S->publish(e);
	}
    } catch (exception &ex) {
	cerr << "error: " << ex.what() << endl;
    }
    delete(S);
}
