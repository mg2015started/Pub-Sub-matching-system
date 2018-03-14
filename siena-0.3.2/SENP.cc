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
// $Id: SENP.cc,v 1.2 2002/11/22 17:52:34 carzanig Exp $
// 

#include <string>
#include <sstream>

#include <siena/SENP.h>
#include <siena/URI.h>

using namespace std;

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

#ifdef NO_INLINE
#define inline
#include <siena/SENP.icc>
#endif

const string	SENP::Senp		= "senp";
const string	SENP::Version		= "0.1";

const string	SENP::From		= "from";
const string	SENP::To		= "to";
const string	SENP::Method		= "method";
const string	SENP::Subscriber	= "subscriber";
const string	SENP::Publisher		= "publisher";
const string	SENP::Peer		= "peer";
const string	SENP::Date		= "date";
const string	SENP::Ttl		= "ttl";

const string	SENP::PUB		= "PUB";
const string	SENP::SUB		= "SUB";
const string	SENP::UNS		= "UNS";
const string	SENP::ADV		= "ADV";
const string	SENP::UNA		= "UNA";

const string	SENP::HLO		= "HLO";
const string	SENP::BYE		= "BYE";

const string	SENP::SUS		= "SUS";
const string	SENP::RES		= "RES";

const string	SENP::RE_Ok		= "200";
const string	SENP::RE_Ignored	= "201";

const string	SENP::RE_Error		= "400";
const string	SENP::RE_BadRequest	= "401";
const string	SENP::RE_NotImplemented	= "402";
const string	SENP::RE_UnknownObject	= "403";

const string	SENP::DBG		= "DBG";

SENPUri::SENPUri(string::const_iterator curs, string::const_iterator end)
{
    //
    // parses and construct a SENP URI:
    // //host[:port][/path...]
    //
    string			hname;
    INAddress::port_type	port = DefaultPort;
    
    if (curs == end || *curs++ != '/') throw BadURI();
    if (curs == end || *curs++ != '/') throw BadURI();
    //
    // the hostname may contain letters, digits, 
    // dots (domain separators), and hyphens (see RFC1034)
    //
    while(curs != end && (isalnum(*curs) || *curs == '.' || *curs == '-'))
	hname += *curs++;
  
    if (curs != end && *curs == ':') {
	++curs;
	port = 0;
	while(curs != end && isdigit(*curs))
	    port = port*10 + *curs++ - '0';
    }
    if (curs != end) {
	if (*curs == '/') {
	    object.assign(++curs, end);
	} else {
	    throw BadURI();
	}
    }
    address.set_host_port(hname.c_str(), port);
}

SENPUri::~SENPUri() {};

string SENPUri::to_string() const 
{
    ostringstream ost;

    ost << SENP::Senp << "://" << address.hostname() << ':' << address.port();
    if (object.size() > 0)
	ost << '/' << object;

    return ost.str();
}

iostream * SENPUri::open_connection() const 
{
    return new TCPSocket(address);
}

const char * SENPException::what() const throw()
{
    return reply.c_str();
}

#ifdef HAVE_CXX_NAMESPACE
}; // namespace Siena
#endif

