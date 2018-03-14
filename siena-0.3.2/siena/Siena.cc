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
// $Id: Siena.cc,v 1.2 2002/11/22 17:52:34 carzanig Exp $
//
#include <sienaconf.h>

#include <cstdio>

#include <iostream>
#include <string>

using namespace std;

#include <siena/Siena.h>

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

#if defined(NO_INLINE)
#define inline
#include <siena/Siena.icc>
#endif

const char * Siena::Version = SIENAVERSION;

const string SienaTypeDescription[] = {
    "null",
    "string",
    "integer",
    "boolean",
    "data",
    "float",
    "binary"
};

const string SienaOperatorDescription[] = {
    "unknown operator",
    "equal",
    "suffix",
    "prefix",
    "less than",
    "greater than",
    "less equal",
    "greater equal",
    "any value",
    "not equal",
    "contains"
};

AttributeValue & AttributeValue::operator = (const AttributeValue &x)
{
    if (_type == Siena_string && x.str != NULL) delete(str);
    _type = x._type;
    switch(_type) {
    case Siena_null: break;
    case Siena_string:
	if (x.str != NULL)
	    str = new string(*x.str);
	else 
	    str = new string();			// this should never happen
	break;
    case Siena_integer:
	num = x.num;
	break;
    case Siena_bool:
	bln = x.bln;
	break;
    case Siena_double:
	dbl = x.dbl;
	break;
	//
	// to be continued (with other types) ... work in progress ...
	//
    default:
	throw(BadType(_type));
	break;
    }
    return *this;
}

AttributeValue::AttributeValue(const AttributeValue &x): _type(x._type)
{
    switch(_type) {
    case Siena_null: break;
    case Siena_string:
	if (x.str != NULL)
	    str = new string(*x.str);
	else 
	    str = new string();			// this should never happen
	break;
    case Siena_integer:
	num = x.num;
	break;
    case Siena_bool:
	bln = x.bln;
	break;
    case Siena_double:
	dbl = x.dbl;
	break;
	//
	// to be continued (with other types) ... work in progress ...
	//
    default:
	throw(BadType(_type));
	break;
    }
}

AttributeValue::~AttributeValue()
{
    if (_type == Siena_string && str != NULL) delete(str);
}

bool is_suffix(const string &x, const string &y)
{
    // true iff y is a suffix of x, e.g.:
    // x = "pippotto"
    // y = "otto"
    //
    string::const_reverse_iterator xri = x.rbegin();
    string::const_reverse_iterator yri = y.rbegin();
    while(yri != y.rend()) {
	if (xri == x.rend() || *xri != *yri) return false;
	++xri;
	++yri;
    }
    return true;
}

bool is_prefix(const string &x, const string &y)
{
    // true iff y is a prefix of x, e.g.:
    // x = "testolina"
    // y = "test"
    //
    string::const_iterator xi = x.begin();
    string::const_iterator yi = y.begin();
    while(yi != y.end()) {
	if (xi == x.end() || *xi != *yi) return false;
	++xi;
	++yi;
    }
    return true;
}

bool is_substr(const string &x, const string &y)
{
    // true iff y is a substring of x, e.g.:
    // x = "software"
    // y = "war"
    //
    return x.find(y) != x.npos;
}

/** semantics of attribute constraints: applies an operator to two values.

    This function implements some minimal type-conversion (or type
    compatibility). In practice, it only considers integers as
    doubles.  Actually, I'm not even sure the extended semantics would
    make sense, e.g., would it be reasonable to write: <code>int x =
    12; string y = "30"</code>; and then ask <code>x &lt; y</code> ?
    What's the right answer to this?  We still need to specify these
    details */
bool apply_operator(SienaOperator op, 
		    const AttributeValue &x, const AttributeValue &y)
{
    //
    // ...work in progress...
    //
    if (x.type() == Siena_null)
	//
	// I'm not sure what the best semantics would be here
	//
	return y.type() == Siena_null;

    switch(op) {
    case Siena_xx: 
	return true;	
    case Siena_eq: 
	switch(x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && x.string_value() == y.string_value();
	case Siena_bool: return y.type() == Siena_bool 
			     && x.bool_value() == y.bool_value();
	case Siena_integer: return (y.type() == Siena_integer 
				    && x.int_value() == y.int_value())
				|| (y.type() == Siena_double 
				    && x.int_value() == y.double_value());
	case Siena_double: return (y.type() == Siena_integer 
				    && x.double_value() == y.int_value())
				|| (y.type() == Siena_double 
				    && x.double_value() == y.double_value());
	default:					// I should probably
	    return false;				// throw an exception
	}
    case Siena_ne: 
	switch(x.type()) {
	case Siena_string: return y.type() != Siena_string
			       || x.string_value() != y.string_value();
	case Siena_bool: return y.type() != Siena_bool
			     || x.bool_value() != y.bool_value();
	case Siena_integer: 
	    switch(y.type()) {
	    case Siena_integer: return x.int_value() != y.int_value();
	    case Siena_double: return x.int_value() != y.double_value();
	    default: return true;
	    }
	case Siena_double:  
	    switch(y.type()) {
	    case Siena_integer: return x.double_value() != y.int_value();
	    case Siena_double: return x.double_value() != y.double_value();
	    default: return true;
	    }
	default:					// I should probably
	    return false;				// throw an exception
	}
    case Siena_ss:
	switch (x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && is_substr(x.string_value(),
					    y.string_value());
	default: return false;				// I should probably
	}						// throw an exception
    case Siena_sf: 
	switch (x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && is_suffix(x.string_value(), 
					    y.string_value());
	default: return false;				// I should probably
	}						// throw an exception
    case Siena_pf:
	switch (x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && is_prefix(x.string_value(), 
					    y.string_value());
	default: return false;				// I should probably
	}						// throw an exception
    case Siena_lt: 
	switch(x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && x.string_value() < y.string_value();
	case Siena_integer: return (y.type() == Siena_integer 
				    && x.int_value() < y.int_value())
				|| (y.type() == Siena_double 
				    && x.int_value() < y.double_value());
	case Siena_bool: return y.type() == Siena_bool 
			     && x.bool_value() < y.bool_value();
	case Siena_double: return (y.type() == Siena_integer 
				    && x.double_value() < y.int_value())
				|| (y.type() == Siena_double 
				    && x.double_value() < y.double_value());
	default:					// I should probably
	    return false;				// throw an exception
	}
    case Siena_gt: 
	switch(x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && x.string_value() > y.string_value();
	case Siena_integer: return (y.type() == Siena_integer 
				    && x.int_value() > y.int_value())
				|| (y.type() == Siena_double 
				    && x.int_value() > y.double_value());
	case Siena_bool: return y.type() == Siena_bool 
			     && x.bool_value() > y.bool_value();
	case Siena_double: return (y.type() == Siena_integer 
				    && x.double_value() > y.int_value())
				|| (y.type() == Siena_double 
				    && x.double_value() > y.double_value());
	default:					// I should probably
	    return false;				// throw an exception
	}
    case Siena_le: 
	switch(x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && x.string_value() <= y.string_value();
	case Siena_integer: return (y.type() == Siena_integer 
				    && x.int_value() <= y.int_value())
				|| (y.type() == Siena_double 
				    && x.int_value() <= y.double_value());
	case Siena_bool: return y.type() == Siena_bool 
			     && x.bool_value() <= y.bool_value();
	case Siena_double: return (y.type() == Siena_integer 
				    && x.double_value() <= y.int_value())
				|| (y.type() == Siena_double 
				    && x.double_value() <= y.double_value());
	default:					// I should probably
	    return false;				// throw an exception
	}
    case Siena_ge: 
	switch(x.type()) {
	case Siena_string: return y.type() == Siena_string
			       && x.string_value() >= y.string_value();
	case Siena_integer: return (y.type() == Siena_integer 
				    && x.int_value() >= y.int_value())
				|| (y.type() == Siena_double 
				    && x.int_value() >= y.double_value());
	case Siena_bool: return y.type() == Siena_bool 
			     && x.bool_value() >= y.bool_value();
	case Siena_double: return (y.type() == Siena_integer 
				    && x.double_value() >= y.int_value())
				|| (y.type() == Siena_double 
				    && x.double_value() >= y.double_value());
	default:					// I should probably
	    return false;				// throw an exception
	}
    default:
	return false;					// exception ?
    }
    return false;
}

//
// Exceptions
//
const char * SienaException::what() const throw() {
    static const string _descr = "unknown Siena exception";
    return _descr.c_str();
}

const char * EventModelException::what() const throw() {
    static const string _descr = "event model exception";
    return _descr.c_str();
}

const char * BadType::what() const throw() {
    static const string _descr = "bad type";
    return _descr.c_str();
}

ostream & BadType::printout(ostream &os) const {
    return os << what() << ": " << SienaTypeDescription[type];
}

const char * BadOperator::what() const throw() {
    static const string _descr = "bad operator";
    return _descr.c_str();
}

ostream & BadOperator::printout(ostream &os) const {
    return os << what() << ": " << SienaOperatorDescription[op];
}

#ifdef HAVE_CXX_NAMESPACE
} // namespace Siena
#endif

