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
// $Id: IO.h,v 1.1 2002/06/10 19:57:52 carzanig Exp $
//
// This is going to be my ultimate Socket C++ library
//
#ifndef _IO_h
#define _IO_h

#include <sys/types.h>

#include <string>
#include <exception>

class IOError;
class RWError;

//
// this should probably be somewhere else...
//
class IOError: public virtual exception {
public:
    int				errno_value;
    
				IOError(int);

    virtual const char *	what()				const _GLIBCXX_USE_NOEXCEPT;
};

class RWError: public virtual IOError {
public:
    size_t			chars_done;
    
				RWError(size_t cdone, int errn);

};

//
// signal-transparent I/O
//
size_t		do_write(int fildes, const char * buf, size_t nbyte);
size_t		do_read(int fildes, char * buf, size_t nbyte);
void		do_close(int fildes);

// 
// inline stuff
//
#ifndef OUTLINE
#include <siena/IO.icc>
#endif

#endif
