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
// $Id: Monitor.h,v 1.3 2002/11/22 17:52:34 carzanig Exp $
//
#ifndef _Monitor_h
#define _Monitor_h

#include <string>

#include <siena/Socket.h>

using namespace std;

//
// This is the Siena Monitor interface
//
// It allows to monitor and/or log messages. Every message has a
// `from', a `to', a `method', and some `data'. The interface is very
// simple at this point. Things might change significantly in the
// future. This structure might become a class in itself.
//
class Monitor {
    //
    // the monitor class has a virtual interface
    //
 public:
    virtual void notify(const string & sender, const string & receiver)
	= 0;
    virtual void subscribe(const string & sender, const string & receiver)
	= 0;
    virtual void unsubscribe(const string & sender, const string & receiver)
	= 0;
    virtual void connect(const string & n1, const string & n2)
	= 0;
    virtual void disconnect(const string & n1, const string & n2)
	= 0;
    virtual void add_node(const string & node, const string & ntype)
	= 0;
    virtual void add_node(const string & node)
	= 0;
    virtual void remove_node(const string & node)
	= 0;

 protected:
    static const char Sep = ' ';
};

//
// Here's a simple implementation of the Siena Monitor. This
// implementation uses a simple monitoring protocol implemented on top
// of UDP. This kind of monitor (and perhaps any kind of monitor for
// Siena) is intended for use on a LAN.
//
class SimpleMonitor: public virtual Monitor {
 public:
    SimpleMonitor(const char *, unsigned short);

    virtual void notify(const string & sender, const string & receiver);
    virtual void subscribe(const string & sender, const string & receiver);
    virtual void unsubscribe(const string & sender, const string & receiver);
    virtual void connect(const string & n1, const string & n2);
    virtual void disconnect(const string & n1, const string & n2);
    virtual void add_node(const string & node, const string & ntype);
    virtual void add_node(const string & node);
    virtual void remove_node(const string & node);

    static const string SienaIcon;
    static const string ThinclientIcon;
    static const string ObjectIcon;
    static const string AntoIcon;
    static const string AdaptIcon;
    static const string UpdateIcon;
    static const string ReconfigureIcon;
    static const string InventoryIcon;
    static const string RemoveIcon;
    static const string ConstrainIcon;
    static const string RickIcon;
    static const string NotifIcon;
    static const string PubIcon;
    static const string SubIcon;
    static const string UnsubIcon;
    static const string AdvIcon;
    static const string UnadvIcon;
 
 private:
    INAddress   addr;
    UDPSocket	s;

    void send(const string & msg);
};

#endif 
