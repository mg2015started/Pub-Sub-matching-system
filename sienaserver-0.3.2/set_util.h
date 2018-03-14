// -*- C++ -*-
//
//  This file is part of Siena, a wide-area event notification system.
//  See http://www.cs.colorado.edu/serl/siena/
//
//  Author: Antonio Carzaniga <carzanig@cs.colorado.edu>
//  See the file AUTHORS for full details. 
//
//  Copyright (C) 1998-2002 University of Colorado
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
// $Id: set_util.h,v 1.3 2002/11/22 20:57:46 carzanig Exp $
//
#ifndef _set_util_h
#define _set_util_h

#include <set>

//
// X += Y computes the union of X and Y and assigns it to X
// X and Y being two `compatible' sets 
//
template<class Key, class Compare, class Alloc>
set<Key, Compare, Alloc> & operator += (set<Key, Compare, Alloc> &dest, 
					const set<Key, Compare, Alloc> &src) 
{
    typename set<Key, Compare, Alloc>::iterator di = dest.begin();
    typename set<Key, Compare, Alloc>::const_iterator si = src.begin();
    while(si != src.end()) {
	if (di == dest.end() || (dest.value_comp())(*si, *di)) {
	    dest.insert(di, *si);
	    ++si;
	} else {
	    if (!(dest.value_comp())(*di, *si))
		++si;
	    ++di;
	}
    }
    return dest;
}

//
// X &= Y computes the intersection of X and Y and assigns it to X
// X and Y being two `compatible' sets 
//
template<class Key, class Compare, class Alloc>
set<Key, Compare, Alloc> & operator &= (set<Key, Compare, Alloc> &dest, 
					const set<Key, Compare, Alloc> &src) 
{
    typename set<Key, Compare, Alloc>::iterator di = dest.begin();
    typename set<Key, Compare, Alloc>::const_iterator si = src.begin();
    while(si != src.end() && di != dest.end() ) {
	if ((dest.value_comp())(*si, *di)) {
	    ++si;
	} else if ((dest.value_comp())(*di, *si)) {
	    dest.erase(di++);
	} else {
	    ++si;
	    ++di;
	}
    }
    return dest;
}

//
// X -= Y subtracts every element of Y from X
// X and Y being two `compatible' sets 
//
template<class Key, class Compare, class Alloc>
set<Key, Compare, Alloc> & operator -= (set<Key, Compare, Alloc> &dest, 
					const set<Key, Compare, Alloc> &src) 
{
    typename set<Key, Compare, Alloc>::iterator di = dest.begin();
    typename set<Key, Compare, Alloc>::const_iterator si = src.begin();
    while(si != src.end() && di != dest.end()) {
	if ((dest.value_comp())(*si, *di)) {
	    ++si;
	} else if ((dest.value_comp())(*di, *si)) {
	    ++di;
	} else {
	    ++si;
	    dest.erase(di++);
	}
    }
    return dest;
}

#endif


