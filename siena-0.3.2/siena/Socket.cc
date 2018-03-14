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
// $Id: Socket.cc,v 1.3 2002/11/22 17:52:34 carzanig Exp $
//
#include <sienaconf.h>

#include <cstring>
#include <cctype>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <exception>
#include <new>

#include <siena/Socket.h>

//
// inline stuff
//
#ifdef NO_INLINE
#define inline
#include <siena/Socket.icc>
#undef inline
#endif


const INAddress & INAddress::this_host() 
{
    static INAddress addr;
    char hostname[MAXHOSTNAMELEN];
    ::gethostname(hostname, MAXHOSTNAMELEN -1);
    addr = INAddress(hostname, 0);
    return addr;
}

void INAddress::set_host_port(const char * host, 
				  INAddress::port_type port)
{
    set_host(host);
    sin_port = ntohs(port);
}

void INAddress::set_host(const char* host_name)
{
    if ((sin_addr.s_addr = inet_addr(host_name)) == (unsigned long)-1) {
	hostent* hp = ::gethostbyname(host_name);
	if (hp == 0) throw SocketError(SocketError::GetHostName);
	memcpy(&sin_addr, hp->h_addr, hp->h_length);
	sin_family = hp->h_addrtype;
    } else
	sin_family = AF_INET;
}

SocketRep::SocketRep(int type): ref_count(0) 
{
    fd = ::socket(PF_INET, type, 0);
    if (fd < 0)	throw SocketError(SocketError::Create, errno);
}

SocketRep::~SocketRep() {
    if (fd >= 0) ::close(fd);
}

INAddress Socket::local_address() const 
{
    INAddress a;
    INAddress::size_type len = a.size();
    if (::getsockname(filedescr(), a.addr(), &len) == -1)
	throw SocketError(SocketError::GetSockName, errno);

    return a;
}

INAddress Socket::peer_address() const
{
    INAddress a;
    INAddress::size_type len = a.size();
    errno = 0;
    if (::getpeername(filedescr(), a.addr(), &len) == -1)
	throw SocketError(SocketError::GetSockName, errno);

    return a;
}

void TCPSocket::connect(const INAddress & addr)
{
    if (rep == NULL) {
	TCPSocketRep * new_rep = new TCPSocketRep();
	Socket::assign(new_rep);
	ios::init(&new_rep->buf);
    }
    errno = 0;
    if (::connect(filedescr(), (struct sockaddr *)addr.addr(), addr.size()))
	throw SocketError(SocketError::Connect, errno);
}

ssize_t UDPSocket::sendto(const INAddress & addr, const void *msg, size_t len)
{
    ssize_t res = -1;
    if (rep == NULL) {
	Socket::assign(new UDPSocketRep());
    }
    errno = 0;
    res = ::sendto(filedescr(), msg, len, 0, 
		   (struct sockaddr *)addr.addr(), addr.size());
    if (res < 0)
	throw SocketError(SocketError::SendTo, errno);
    return res;
}

int TCPPort::accept(INAddress &addr) const
{
    int nfd;
    INAddress::size_type len = addr.size();
    errno = 0;
    nfd = ::accept(filedescr(), addr.addr(), &len);   
    if (nfd < 0) throw SocketError(SocketError::Accept, errno);

    return nfd;
}

void TCPPort::bind(INAddress::port_type port, bool reuse, int maxconn)
{
    if (rep == NULL)
	Socket::assign(new TCPPortRep());
    address_reuse(reuse);
    INAddress a(port);
    errno = 0;
    if (::bind(filedescr(), a.addr(), a.size()) < 0)
	throw SocketError(SocketError::Bind, errno);
    errno = 0;
    if (::listen(filedescr(), SOMAXCONN) < 0)
	throw SocketError(SocketError::Listen, errno);
}

void Socket::address_reuse(bool reuse)
{
    int value = reuse ? 1 : 0;
    errno = 0;
    if (::setsockopt(filedescr(), SOL_SOCKET, SO_REUSEADDR, 
		     (char *)&value, sizeof(value)) < 0)
	throw SocketError(SocketError::SetAddressReuse, errno);
}

bool Socket::address_reuse() const
{
    int value;
    INAddress::size_type len = sizeof(value);
    errno = 0;
    if (::getsockopt(filedescr(), SOL_SOCKET, SO_REUSEADDR, 
		     (char *)&value, &len) < 0)
	throw SocketError(SocketError::GetAddressReuse, errno);
    return (value != 0);
}

const char * SocketError::what() const throw() {
  static const char * _descr[] = { 
    "unknown socket exception", 
    "couldn't create socket",
    "couldn't bind socket",
    "couldn't listen to socket",
    "couldn't get hostname",
    "couldn't connect",
    "couldn't accept connections",
    "socket not connected",
    "couldn't get local address",
    "couldn't get peer address",
    "couldn't get address reuse",
    "couldn't set address reuse",
    "couldn't send message"
  };

  return _descr[type];
}

//
// Implementation of a TCP Socket Buffer
//
// SocketBuffer is essentially a pair of buffers. A GET buffer and a
// PUT buffer. The application reads from the GET buffer and writes to
// the PUT buffer. SocketBuffer takes care of flushing the content of
// the PUT buffer to the socket (fd) and re-filling the GET buffer
// with bytes read from the socket.
//
// PUT buffer:
//  pbuf                                       pbuf + buffer_size
// v      un-flushed       avail for write    v
// ######################--------------------- 
// ^                     ^                    ^
//  pbase()               pptr()               epptr()
//
//
//
// GET buffer:
//  gbuf                                       gbuf + buffer_size
// v                    avail for read        v
// -----------------######################----
// ^                ^                     ^
//  gback()          gptr()                egptr()
//

int SocketBuffer::sync()
{
    //
    // this method flushes (or tries to flush) unwritten data onto the
    // socket
    //
    ssize_t res = 0;
    bool error = false;
    int error_code = 0;

    if (pptr() != NULL && pbase() < pptr() && pptr() <= epptr()) {
        // 
        // #######################----------------  PUT buffer
        // ^                      ^               ^
        //  pbase()                pptr()          epptr()
        //
	res = ::write(fd, pbase(), pptr() - pbase());
        if (res > 0) {
            char * pto = pbase();
            char * pfrom = pbase() + res;
            int len = pptr () - pbase () - res;
            //
            // <-----res-----><-len-->
            // wwwwwwwwwwwwwww########----------------  PUT buffer
            // ^              ^       ^               ^
            //  pto=pbase()    pfrom   pptr()          epptr()
            //
            while (pfrom < pptr()) *pto++ = *pfrom++;
            setp(pbase(), pbuf + buffer_size);
            pbump(len);
	    return 0;
        }
    }
    return res;
}

long int SocketBuffer::showmanyc()
{   
    //                  <-----showmanyc------>
    // -----------------######################  GET buffer
    // ^                ^                     ^
    //  gback()          gptr()                egptr()
    //
    if (gptr() != NULL && gptr() < egptr())
        return egptr() - gptr();
    return 0;
}

int SocketBuffer::underflow()
{
    if (gptr() == NULL) return EOF;
    //
    // if there is still something in the buffer... 
    //
    if (gptr() < egptr()) return (unsigned char) *gptr();
    //
    // otherwise (GET buffer is empty) reads more stuff from the
    // socket
    //
    ssize_t rlen = ::read(fd, gbuf, buffer_size);

    if (rlen <= 0) return EOF;
    setg(gbuf, gbuf, gbuf + rlen);
    return (unsigned char) *gptr();
}

int SocketBuffer::uflow()
{
    int ret = underflow();

    if (ret != EOF) gbump(1);

    return ret;
}

streamsize SocketBuffer::xsgetn(char * s, streamsize n)
{
    int avail;
    streamsize tot = 0;

    while ((avail = showmanyc()) < n) {
        if (avail > 0) {
            memcpy(s, gptr(), avail);
            gbump(avail);
            n -= avail;
            tot += avail; 
            s += avail;
        }
        if (underflow() == EOF)
            return tot;
    }
    if (n > 0) {
        memcpy(s, gptr(), n);
        gbump(n);
        tot += n;
    }
    
    return tot;
}

int SocketBuffer::overflow(int c)
{
    if (pbase() == NULL) return EOF;
    //
    // flush written if the application writes c == EOF
    //
    if (c == EOF) return sync();
    //
    // or if PUT buffer is full
    //
    if (pptr() == epptr()) sync();

    *pptr() = (char)c;
    pbump(1);

    return c;
}

streamsize SocketBuffer::xsputn(const char* s, streamsize n)
{
    int avail; 
    streamsize tot = 0;

    while ((avail = epptr() - pptr()) < n) {
        if (avail > 0) {
            memcpy(pptr(), s, avail);
            pbump(avail);
            n -= avail;
            tot += avail;
            s += avail;
        }
	if (sync() < 0) return -1;
    }
    if (n > 0) {
        memcpy(pptr(), s, n);
        pbump(n);
        tot += n;
    }

    return tot;
}

