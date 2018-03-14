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
// $Id: SENP.h,v 1.2 2002/11/22 17:52:34 carzanig Exp $
// 
#ifndef _SENP_h
#define _SENP_h

#include <string>

#include <siena/Socket.h>
#include <siena/URI.h>
#include <siena/Siena.h>

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

    //
    // the SENP class embodies most of the messages of the SENP protocol
    // this is a *very minimal* SENP
    // ...work in progress... 
    //

class SENP {
 public:
    //
    // protocol (and schema) name and version
    //
    static const string		Senp;
    static const string		Version;
    //
    // headers of SENP requests (type of value in comment)
    //
    static const string		From;		// string, valid URI
    static const string		To;		// string, valid URI
    static const string		Method;		// string (see below)
    static const string		Subscriber;	// string, valid URI
    static const string		Publisher;	// string, valid URI
    static const string		Peer;		// string, valid URI
    static const string		Date;		// integer (timestamp)
    static const string		Ttl;		// integer >= 0
    //
    // methods: valid values of the Method header above
    // These first ones correspond to the main Siena interface
    //
    static const string		PUB;
    static const string		SUB;
    static const string		UNS;
    static const string		ADV;
    static const string		UNA;
    //
    // methods of the server-to-server interface
    //
    static const string		HLO;
    static const string		BYE;

    static const string		SUS;
    static const string		RES;

    //
    // replies of the protocol. This part is highly incomplete...
    //
    // ok replies
    //
    static const string		RE_Ok;
    static const string		RE_Ignored;
    //
    // error replies
    //
    static const string		RE_Error;
    static const string		RE_BadRequest;
    static const string		RE_NotImplemented;
    static const string		RE_UnknownObject;

    //
    // and a debug request (possibly to be removed in the future)
    //
    static const string		DBG;
};

class SENPUri: public URIRep {
 public:
				SENPUri(string::const_iterator b,
					string::const_iterator e);
    virtual 			~SENPUri();

    static const INAddress::port_type	DefaultPort = 1996;

    virtual bool		operator < (const URIRep &)	const;
    virtual bool		operator ==(const URIRep &)	const;

    virtual iostream *		open_connection()		const; 
    virtual string		to_string()			const;

    const string &		get_object()			const;

 private:
    INAddress			address;
    string			object;
};

class SENPException : public SienaException {
public:
    string			reply;

				SENPException(const string & s): reply(s) {};
    virtual			~SENPException() throw() {};

    virtual const char *	what() const throw();
};

#ifndef NO_INLINE
#include <siena/SENP.icc>
#endif

#ifdef HAVE_CXX_NAMESPACE
}; // namespace Siena
#endif

#endif
