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
// $Id: PThreads.h,v 1.3 2002/06/12 22:32:21 carzanig Exp $
//

//
// this is an initial attempt at providing a thread interface this is
// extremely primitive!  now it is just a simple wrapper for POSIX 1.c
// threads.  The idea is to incorporate win32 threads as well. 
// ...work in progress...
//

#ifndef _PThreads_h
#define _PThreads_h

class Thread;
class Mutex;
class Condition;
class Lock;

#ifndef _REENTRANT
#define _REENTRANT 1
#endif

#include <pthread.h>
#include <signal.h>

//
// this class represents a Thread.  A thread class must implement
// virtual void * run();
//
class Thread {
 public:
    int			start();
    int			join();
    int			kill(int sig);
    virtual void *	run() = 0;

    static int		block_signal(int sig);
    static int		unblock_signal(int sig);
    //
    // lots of stuff to add here ...work in progress...
    //

 protected:
    static void		exit(void * status = NULL);
    static int		yield(void);

 private:
    pthread_t		_t;
    static void	*	_run(void *);
};

class Mutex {
 public: 
			Mutex();
			~Mutex();
    void		lock();
    void		unlock();

 private:
    pthread_mutex_t	m;

    friend class Condition; 
};

class Condition {
 public: 
			Condition();
			~Condition();
    void		wait(Mutex & m);
    void		signal();

 private:
    pthread_cond_t	c;
};

class Lock {
    //
    // this class implements a read/write lock:
    // the idea is that there can be many readers at the same time,
    // but only one writer
    //

 public:
			Lock();
			~Lock();

    void		wlock();
    void		rlock();
    void		unlock();

 private:
#ifdef HAVE_PTHREAD_RWLOCK
    pthread_rwlock_t	l;
#else
    pthread_mutex_t     rl;
    pthread_cond_t      rc;
    int                 status; // -1 (one writer), 0, (no lock), >0 (readers)
#endif
};

#ifndef NO_INLINE
#include "PThreads.icc"
#endif

#endif /* _PThreads_h */
