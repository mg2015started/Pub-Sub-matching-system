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
// $Id: SimpleSiena.cc,v 1.3 2002/11/22 17:52:34 carzanig Exp $
// 
#include <sienaconf.h>

#include <sstream>
#include <iostream>

#include <siena/Siena.h>
#include <siena/SienaIO.h>
#include <siena/SimpleSiena.h>
#include <siena/SENP.h>
#include <siena/URI.h>
#include <siena/Socket.h>

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

//
// A simple client-side Siena service
//
SimpleSiena::SimpleSiena(const string &siena_uri, INAddress::port_type p):
    server(siena_uri), port_number(p), port(NULL) {};

SimpleSiena::~SimpleSiena() 
{
    if (myuri != NULL) delete(myuri);
    if (port != NULL) delete(port);
}

void SimpleSiena::publish(const Pub &pub)
{
    iostream *s;
    s = server.open_connection();
    Request r;
    r[SENP::Method] = SENP::PUB;
    if (myuri != NULL) {
	r[SENP::Publisher] = *myuri;
    }
    *s << r << pub << flush;
    string reply;
    *s >> reply;
    delete(s);
	if (reply != SENP::RE_Ok) throw (SENPException(reply));
    //
    // throw exception if reply != SENP::RE_Ok
    //
}

Notifiable * get_party_from_string(const string &s)
{
    //
    // dirty hack! I have to do this because unfortunately, my
    // implementation of the C++ stream library doesn't have
    // istream::operator>>(void * &) (which is part of the standard!)
    // Anyway it is rather safe to assume that a pointer can be stored
    // in an unsigned long long
    //
    union {
	unsigned long long ul;
	Notifiable * n;
    };
    istringstream is(s);
    is >> ul;
    return n;
}

string put_party_into_string(Notifiable *party)
{
    //
    // dirty hack! See explanation above.
    //
    union {
	unsigned long long ul;
	Notifiable * n;
    };
    n = party;
    ostringstream os;
    os << ul;
    return os.str();
}

void SimpleSiena::sub_uns(const string & method, 
			  const IntervalSub & f, Notifiable *party)
{
    if (myuri == NULL) return;
    iostream *s;
    s = server.open_connection();

    Request r;

    r[SENP::Method] = method;
    string sub = *myuri;
    sub += '/';
    sub += put_party_into_string(party);
    r[SENP::Subscriber] = sub;


    *s << r << f << flush;
    string reply;
    *s >> reply;
    delete(s);
    if (reply != SENP::RE_Ok) throw (SENPException(reply));
}


void SimpleSiena::subscribe(const IntervalSub &f, Notifiable *party)
{
    if (myuri == NULL) {
	port = new TCPPort(port_number);
	ostringstream ostr;
	ostr << "senp://" << INAddress::this_host().hostname() 
	     << ":" << port->local_address().port();
	myuri = new string(ostr.str());
    }

    sub_uns(SENP::SUB, f, party);
}


void SimpleSiena::exit_loop()
{
    in_loop=false;
}

void SimpleSiena::main_loop()
{
    TCPSocket * s;
    Pub pub;
    Request r;

    if (myuri == NULL) {
	port = new TCPPort(port_number);
	ostringstream ostr;
	ostr << "senp://" << INAddress::this_host().hostname() 
	     << ":" << port->local_address().port();
	myuri = new string(ostr.str());
    }

    in_loop = true;
    while(in_loop) {
	s = new TCPSocket(port->accept());
	*s >> r;
	Request::iterator method_i = r.find(SENP::Method);
	if (method_i == r.end() || (*method_i).second.type()!=Siena_string) {
	    *s << SENP::RE_BadRequest << flush;
	    delete(s); s = NULL;
	} else if ((*method_i).second.string_value() == SENP::PUB) {
	    *s >> pub;
	    *s << SENP::RE_Ok << endl;
	    Request::iterator to_i = r.find(SENP::To);
	    if (to_i != r.end() && (*to_i).second.type()==Siena_string) {
		URI u((*to_i).second.string_value());
		if (u.schema() != SENP_SCHEMA) {
		    *s << SENP::RE_UnknownObject << flush;
		    delete(s); s = NULL;
		} else {
		    SENPUri * suri = (SENPUri *)u.representation();
		    void *party;
		    party = get_party_from_string(suri->get_object());
		    if (party == (Notifiable *)0) {
			*s << SENP::RE_UnknownObject << flush;
			delete(s); s = NULL;
		    } else {
			*s << SENP::RE_Ok << flush;
			delete(s); s = NULL;
			((Notifiable *)party)->notify(pub);
		    }
		}
	    } else {
		*s << SENP::RE_UnknownObject << flush;
		delete(s); s = NULL;
	    }
	} else {
	    *s << SENP::RE_NotImplemented <<  flush;
	    delete(s); s = NULL;
	}
    }
}

#ifdef HAVE_CXX_NAMESPACE
}; // namespace Siena
#endif


