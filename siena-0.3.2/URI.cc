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
// $Id: URI.cc,v 1.2 2002/11/22 17:52:34 carzanig Exp $
//
#include <sienaconf.h>

#include <cctype>
#include <sstream>

#include <siena/URI.h>
#include <siena/SENP.h>

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

#ifdef NO_INLINE
#define inline
#include <siena/URI.icc>
#endif

const char * BadURI::what() const throw() { 
    return "malformed URI";
}

URI::URI(const string & uri) 
{
    //
    // the constructor simply parses the URI schema (e.g., `mailto:',
    // `senp:', `http:', etc.) and calls the constructor of URIRep
    // with the corresponding URI type over the rest of the uri
    // string.
    //
    // ok, this is very simple: now, we support only ``senp''.
    // ...work in progress...  I'm going to write some pretty
    // ugly-looking code here
    //
    string::const_iterator ui = uri.begin();

    if (ui != uri.end() && *ui++ == 's' 
	&& ui != uri.end() && *ui++ == 'e' 
	&& ui != uri.end() && *ui++ == 'n' 
	&& ui != uri.end() && *ui++ == 'p' 
	&& ui != uri.end() && *ui++ == ':') {
	rep = new SENPUri(ui, uri.end());
	rep->schema = SENP_SCHEMA;
	rep->ref_count = 1;
    } else {
	throw BadURI();
    }
}

#ifdef HAVE_CXX_NAMESPACE
}; // namespace Siena
#endif

