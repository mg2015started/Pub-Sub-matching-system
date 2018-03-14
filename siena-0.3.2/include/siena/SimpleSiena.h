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
// $Id: SimpleSiena.h,v 1.2 2002/11/22 17:52:35 carzanig Exp $
// 
#ifndef _SimpleSiena_h
#define _SimpleSiena_h

#include <iostream>

#include <siena/Siena.h>
#include <siena/SENP.h>
#include <siena/URI.h>
#include <siena/Socket.h>
#include <data_structure.h>


#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

class SimpleSiena: public virtual Siena {
 public:
    /** @memo creates a <code>SimpleSiena</code> that identifies
        itself using the given URI and that receives notifications at
        the given port
	@param siena_uri is the URI that will be used to identify
	<code>this</code> client.
	@param p is the port number that this client uses to receive
	notifications.
    */
			SimpleSiena(const string &siena_uri,
				    INAddress::port_type p = 0);
			~SimpleSiena();

    virtual void	publish(const Pub &);

    virtual void	subscribe(const IntervalSub &, Notifiable *);
    virtual void	unsubscribe(const IntervalSub &, Notifiable *);

    /** enters the main loop waiting for notifications */
    void		main_loop();
    void		exit_loop();

 private:
    URI				server;
    INAddress::port_type	port_number;
    bool			in_loop;
    TCPPort *			port;
    string *			myuri;

    void			sub_uns(const string & method, const IntervalSub & f, Notifiable *party);
};

#ifdef HAVE_CXX_NAMESPACE
}; // namespace Siena
#endif


#endif
