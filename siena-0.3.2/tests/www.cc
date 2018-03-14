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
// $Id: www.cc,v 1.2 2002/11/22 17:52:36 carzanig Exp $
//
#include <string>
#include <exception>
#include <iostream>
#include <cstdlib>

using namespace std;

#include <siena/Socket.h>

int main(int argc, char *argv[])
{
    try {
	const char * host;
	INAddress::port_type p = 80;
	if (argc > 1) {
	    host = argv[1];
	    if (argc > 2) {
		p = atoi(argv[2]);
	    }
	} else {
	    host = "localhost";
	}
	
	string l;
	INAddress a(host, p);
	TCPSocket s(a);
	s << "GET / HTTP/1.0\r\n\r\n" << flush;
	while (getline(s, l))
	    cout << l << endl;
    } catch (exception &ex) {
	cerr << ex.what() << endl;
	return -1;
    }
    return 0;
}
