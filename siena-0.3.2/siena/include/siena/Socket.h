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
// $Id: Socket.h,v 1.2 2002/11/22 17:52:35 carzanig Exp $
//

//
// This is going to be my ultimate Socket C++ library right now, it's
// limited to IP sockets. There are probably numerous libraries on the
// net that do pretty much the same. However I suspect this is going
// to be a crucial piece of library whenever I decide to implement a
// real (robust) Siena, so I want to have `total' control over this
// stuff. Needless to say this is ...work in progress...
//
#ifndef _Socket_h
#define _Socket_h

#include <sienaconf.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <streambuf>
#include <exception>

class INAddress;
class SocketRep;
class TCPSocketRep;
class Socket;
class TCPSocket;
class UDPSocket;	// not yet implemented
class TCPPort;
class UDPPort;		// not yet implemented

using namespace std;
//
// exceptions
//
class SocketError;

class INAddress: public sockaddr_in {
public:
    //
    // all these ifdefs should be replaced by some autoconf macros
    // ...work in progress...
    //
    typedef in_addr_t		ip_address_type;
    typedef socklen_t		size_type;
    typedef in_port_t		port_type;

public:
				INAddress();
				INAddress(const INAddress &);
				INAddress(port_type);
				INAddress(const char *s, port_type);

    void			set_host_port(const char *, port_type);
    void			set_host(const char* host_name);
    void			set_port(port_type);

    INAddress &			operator = (const INAddress &);
  
    bool			operator < (const INAddress &)		const;
    bool			operator == (const INAddress &)		const;

				operator void* ();
    sockaddr_in *		addr_in();
    sockaddr *			addr();
    const sockaddr_in *		addr_in()				const;
    const sockaddr *		addr()					const;

    size_type			size()					const;

    port_type			port()					const;
    ip_address_type		host()					const;
    const char *		hostname()				const;

    static const INAddress &	this_host();
};

class SocketRep {
    friend class Socket;
 protected:
    int				fd;
    unsigned int		ref_count;

 public:
				SocketRep();
				SocketRep(int type);
    virtual			~SocketRep();
};

class TCPPortRep : public SocketRep {
 public:
				TCPPortRep();
				TCPPortRep(int f);
};

class SocketBuffer : public streambuf {
    //
    // This is a stream buffer for sockets. filebuf isn't good enough
    // because it can be in either PUT or GET mode, thus it doesn't
    // implement a two-way communication buffer.
    //
 protected:
    virtual int                 sync();
    virtual long int                 showmanyc();
    virtual streamsize          xsgetn(char * s, streamsize n);
    virtual int                 underflow();
    virtual int                 uflow();
    virtual int                 pbackfail(int c = EOF);
    virtual streamsize          xsputn(const char * s, streamsize n);
    virtual int                 overflow(int c = EOF);

 public:
				SocketBuffer(int & f);
				~SocketBuffer();
    //
    // const buffer size for now...
    //
    static const size_t		buffer_size = 4096;
    
 private:
    int &			fd;
    char                        pbuf[buffer_size];
    char                        gbuf[buffer_size];
};

class TCPSocketRep : public SocketRep {
    friend class		TCPSocket; 
 protected:
				TCPSocketRep();
				TCPSocketRep(int f);

 private:
    SocketBuffer		buf;
};

class Socket {
    //
    // this class is a common placeholder for generic socket
    // operations (such as getsockoption...). The only active
    // functionality provided by this class is the handling of the
    // (shared) socket representation rep. A representation is
    // overwritten by assign and erased by close.
    //
    // Every representation has at least a file descriptor,
    // filedescr() gives access to it.
    //
 public:
				Socket();			// NULL rep
				Socket(SocketRep *r);		// shared rep

    void			assign(SocketRep *r);		// shared rep
    void			close();
    int				filedescr()				const;

				~Socket();
    //
    // soclet operations
    //
    INAddress			peer_address()				const;
    INAddress			local_address()				const;
    //
    bool			address_reuse()				const;
    void			address_reuse(bool b);
    // ...many more to add...
    // ...work in progress...
    //

 protected:
    SocketRep *			rep;
};

class TCPSocket: public Socket, public iostream {
 public:
    TCPSocket();			// null rep
				TCPSocket(int f);		// new rep
				TCPSocket(const INAddress &);	// new rep
				TCPSocket(const TCPSocket &s);	// shared rep

    TCPSocket &			operator = (int f);
    TCPSocket &			operator = (const TCPSocket &s);

    void			connect(const INAddress &);
};

class TCPPort : public Socket {
 public:
				TCPPort();
				TCPPort(int f);
				TCPPort(INAddress::port_type port,
					bool reuse = true,
					int maxconn = SOMAXCONN);
				TCPPort(const TCPPort & p);

    TCPPort &			operator = (int f);
    TCPPort &			operator = (const TCPPort &s);
    //
    // to be continued like TCPSocket...
    //
    void			bind(INAddress::port_type port,
				     bool reuse = true,
				     int maxconn = SOMAXCONN);

    int 			accept()				const;
    int				accept(INAddress &addr)			const;
};

class UDPSocketRep : public SocketRep {
 public:
				UDPSocketRep();
				UDPSocketRep(int f);
};

class UDPSocket: public Socket {
public:
				UDPSocket();
				UDPSocket(int f);		// new rep
				UDPSocket(const UDPSocket &s);	// shared rep

    UDPSocket &			operator = (int f);
    UDPSocket &			operator = (const UDPSocket &s);

    ssize_t			sendto(const INAddress &, 
				       const void *msg, size_t len);
    ssize_t			recvfrom(INAddress &, void *msg, size_t len);
};

class SocketError: public exception {
public:
    enum Type {
	Unknown			= 0,
	Create			= 1,
	Bind			= 2,
	Listen			= 3,
	GetHostName		= 4,
	Connect			= 5,
	Accept			= 6,
	NotConnected		= 7,
	GetSockName		= 8,
	GetPeerName		= 9,
	GetAddressReuse		= 10,
	SetAddressReuse		= 11,
	SendTo			= 12,
	ReceiveFrom		= 13,
	Write			= 14,
	Read			= 15,
    };
    Type			type;
    int				errno_value;
				SocketError(Type t=Unknown, int e = 0);

    virtual const char *	what() const throw();
};

// 
// inline stuff
//
#ifndef NO_INLINE
#include <siena/Socket.icc>
#endif

#endif
