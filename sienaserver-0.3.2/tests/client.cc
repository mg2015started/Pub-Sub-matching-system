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
// $Id: client.cc,v 1.2 2002/11/22 20:57:46 carzanig Exp $
//
#include <siena.h>

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <exception>

using namespace std;

#include <siena/Socket.h>

//
// quick hack here!
//
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	256
#endif

TCPPort *		Port = NULL;

#ifdef HAVE_CXX_NAMESPACE
using namespace Siena;
#endif

ostream * output = &cout;

static void read_messages(TCPPort *port)
{
    TCPSocket s;
    try {
	for(;;) {
	    s = port->accept();
        Pub pub;
        IntervalSub sub;
        Request r;
	    s >> r;
	    if (r[SENP::Method]=="QUIT") return;
	    *output << r << endl;
	    if (r[SENP::Method]==SENP::PUB) {
		s >> pub;
		*output << "here" <<  pub << endl;
	    } else {
		s >> sub;
		*output << sub << endl;
	    }
	    s << SENP::RE_Ok << endl;
	    s.close();
	}
    } catch (exception &ex) {
	cerr <<  ex.what() << endl;
    }
};

void print_usage(const char *programname) 
{
    cerr << "Usage: " << programname << " <URI> [options...]\n\
options:\n\t-port number\n\t-o filename\n\t-detach\n" << endl;
    exit(-1);
}

int main (int argc, char *argv[])
{
    unsigned short port = 0;
    bool detach = false;
    const char * outfile = NULL;

    for (int i=1; i< argc; i++) {		// parses command-line parameters
	if (strcmp(argv[i], "-port")==0) {
	    if (++i < argc) {
		port = atoi(argv[i]);
	    } else {
		print_usage(argv[0]);
	    }
	} else if (strcmp(argv[i], "-o")==0) {
	    if (++i < argc) {
		outfile = argv[i];
	    } else {
		print_usage(argv[0]);
	    }
	} else if (strcmp(argv[i], "-detach")==0) {
	    detach = true;
	} else {
	    print_usage(argv[0]);
	}
    }

    ofstream o;
    if (outfile != NULL) {
	o.open(outfile);
	if (!o) {
	    cerr << "can't open output file " << outfile << endl;
	    exit(1);
	}
    }

    try {
	Port = new TCPPort(port);

	if (detach && outfile != NULL) {
	    pid_t pid;
	    switch((pid = fork())) {
	    case -1:
		cerr << "fork() failed!" << endl;
		exit(-1);
	    case 0: {
		break;
	    }
	    default:
		cout << pid << endl;
		exit(0);
	    }
	}
	if (outfile != NULL) {
	    output = & o;
	}

	char hostname[MAXHOSTNAMELEN];
	gethostname(hostname, MAXHOSTNAMELEN -1);

	read_messages(Port);
	delete(Port);
	return 0;
    } catch (exception &ex) {
	cerr << ex.what() << endl;
	exit(-1);
    }
}
