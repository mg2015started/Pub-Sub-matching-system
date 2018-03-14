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
// $Id: Monitor.cc,v 1.2 2002/06/12 00:45:07 carzanig Exp $
//
#include <string>
#include <exception>

#include <siena/Monitor.h>
#include <siena/Socket.h>

const string SimpleMonitor::SienaIcon = "siena";
const string SimpleMonitor::ThinclientIcon = "thinclient";
const string SimpleMonitor::ObjectIcon = "object";
const string SimpleMonitor::AntoIcon = "anto";
const string SimpleMonitor::AdaptIcon = "adapt";
const string SimpleMonitor::UpdateIcon = "update";
const string SimpleMonitor::ReconfigureIcon = "reconfigure";
const string SimpleMonitor::InventoryIcon = "inventory";
const string SimpleMonitor::RemoveIcon = "remove";
const string SimpleMonitor::ConstrainIcon = "constrain";
const string SimpleMonitor::RickIcon = "rick";
const string SimpleMonitor::NotifIcon = "notif";
const string SimpleMonitor::PubIcon = "pub";
const string SimpleMonitor::SubIcon = "sub";
const string SimpleMonitor::UnsubIcon = "unsub";
const string SimpleMonitor::AdvIcon = "adv";
const string SimpleMonitor::UnadvIcon = "unadv";
 
SimpleMonitor::SimpleMonitor(const char *host, unsigned short port)
    : addr(host,port) {};

void SimpleMonitor::notify(const string & sender, const string & receiver) {
    string cmd = "notify";
    cmd += Sep;
    cmd += sender;
    cmd += Sep;
    cmd += receiver;
    send(cmd);
}

void SimpleMonitor::subscribe(const string & sender, const string & receiver) {
    string cmd = "subscribe";
    cmd += Sep;
    cmd += sender;
    cmd += Sep;
    cmd += receiver;
    send(cmd);
}

void SimpleMonitor::unsubscribe(const string & sender, const string & receiver) {
    string cmd = "unsubscribe";
    cmd += Sep;
    cmd += sender;
    cmd += Sep;
    cmd += receiver;
    send(cmd);
}

void SimpleMonitor::connect(const string & n1, const string & n2) {
    string cmd = "connect";
    cmd += Sep;
    cmd += n1;
    cmd += Sep;
    cmd += n2;
    send(cmd);
}

void SimpleMonitor::disconnect(const string & n1, const string & n2) {
    string cmd = "disconnect";
    cmd += Sep;
    cmd += n1;
    cmd += Sep;
    cmd += n2;
    send(cmd);
}

void SimpleMonitor::add_node(const string & node, const string & ntype) {
    string cmd = "add_node";
    cmd += Sep;
    cmd += node;
    cmd += Sep;
    cmd += ntype;
    send(cmd);
}

void SimpleMonitor::add_node(const string & node) {
    string cmd = "add_node";
    cmd += Sep;
    cmd += node;
    send(cmd);
}

void SimpleMonitor::remove_node(const string & node) {
    string cmd = "remove_node";
    cmd += Sep;
    cmd += node;
    send(cmd);
}

void SimpleMonitor::send(const string &cmd)
{
    try {
	s.sendto(addr, cmd.data(), cmd.size());
	//
	// I should verify the result here ...work in progress...
	//
    } catch (exception &ex) {
	//
	// I purposely ignore exceptions here. Right now the monitoring
	// facility must be simple and not intrusive for the application
	//
    }
}
