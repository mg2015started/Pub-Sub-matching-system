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
// $Id: URI.h,v 1.2 2002/11/22 17:52:35 carzanig Exp $
//
#ifndef _URI_h
#define _URI_h

#include <exception>
#include <string>
#include <iostream>

using namespace std;

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

class URI;

enum URISchema {
    NULL_SCHEMA		= 0,
    SENP_SCHEMA		= 1
    // MAILTO_SCHEMA	= 2,
    //
    // add other URI types here
    //
};

class URIRep {

    friend class URI;

 public:
    virtual iostream *	open_connection()		const	= 0;
    //
    // there should be a send_message too ...work in progress...
    //
    // virtual void	send_message(const string &)	const	= 0;
    //
    virtual string	to_string()			const	= 0;
    virtual bool	operator < (const URIRep &)	const	= 0;
    virtual bool	operator ==(const URIRep &)	const	= 0;

 private:
    URISchema		schema;
    unsigned int	ref_count;
};

class URI {
 public:
			URI();
			URI(const URI &);
			URI(const string &);
			~URI();

    URI &		operator = (const URI &);

    bool		operator < (const URI &)	const;
    bool		operator ==(const URI &)	const;

    URISchema		schema()			const;
    const URIRep *	representation()		const;

    iostream *		open_connection()		const;
    string 		to_string()			const;

 private:
    URIRep *		rep;
};

//
// exceptions
//
class BadURI: public exception {
public:
    virtual const char * what() const throw();
};

#ifndef NO_INLINE
#include <siena/URI.icc>
#endif

#ifdef HAVE_CXX_NAMESPACE
}; // namespace Siena
#endif

#endif
