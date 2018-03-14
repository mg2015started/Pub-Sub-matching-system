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
// $Id: SienaIO.cc,v 1.2 2002/11/22 17:52:34 carzanig Exp $
// 
#include <sienaconf.h>

#include <cctype>
#include <iostream>
#include <string>

using namespace std;

#include <siena/Siena.h>
#include <siena/SienaIO.h>
#include <data_structure.h>

static const char * PE_literal = "expecting literal value";
static const char * PE_operator = "expecting operator";
static const char * PE_lparen = "expecting `{'";
static const char * PE_anamerparen = "expecting attribute name or `}'";
static const char * PE_equal = "excpecting `='";
static const char * PE_pub = "expecting keyword `pub'";
static const char * PE_uri = "expecting keyword `uri'";
static const char * PE_subsize = "expecting keyword `subsize'";
static const char * PE_senp = "expecting keyword `senp'";
static const char * PE_pattern = "expecting keyword `pattern'";
static const char * PE_sub = "expecting keyword `sub' or `}'";
static const char * PE_lowValue = "expecting keyword `lowValue'";
static const char * PE_highValue = "expecting keyword `highValue'";
static const char * PE_subatt = "expecting keyword `subatt'";
static const char * PE_pubatt = "expecting keyword `pubatt'";
static const char * PE_pubValue = "expecting keyword `PE_pubValue'";
static const char * PE_pubsize = "expecting keyword `PE_pubsize'";

static const char * Kwd_senp = "senp";
static const char * Kwd_pub = "pub";
static const char * Kwd_sub = "sub";
static const char * Kwd_any = "any";
static const char * Kwd_null = "null";
static const char * Kwd_true = "true";
static const char * Kwd_false = "false";
static const char * Kwd_lparen = "{";
static const char * Kwd_rparen = "}";

static const char * Kwd_op_sf = "*<";
static const char * Kwd_op_pf = ">*";
static const char * Kwd_op_lt = "<";
static const char * Kwd_op_gt = ">";
static const char * Kwd_op_le = "<=";
static const char * Kwd_op_ge = ">=";
static const char * Kwd_op_eq = "=";
static const char * Kwd_op_ne = "!=";
static const char * Kwd_op_ss = "*";
static const char * Kwd_op_xx = "any";

#ifdef HAVE_CXX_NAMESPACE
namespace Siena {
#endif

//
// byte stuffing function
//
string & encode_string(string &s)
{
    string::iterator si;
    for(si = s.begin(); si != s.end(); ++si) {
	switch(*si) {
	case '"':
	case '\\': s.insert(si, '\\'); break;
	case '\n': *si = 'n'; s.insert(si, '\\'); break;
	case '\0': *si = '0'; s.insert(si, '\\'); break;
	case '\t': *si = 't'; s.insert(si, '\\'); break;
	case '\a': *si = 'a'; s.insert(si, '\\'); break;
	case '\b': *si = 'b'; s.insert(si, '\\'); break;
        default: break;
	}
    }
    s.insert(s.begin(),'"');
    s.insert(s.end(),'"');
    return s;
}

ostream &		operator << (ostream &os, const Pub &pub)
{
	os << Kwd_pub << Kwd_lparen << endl;
	os << pub.size << ' ';
	for (int i=0; i<pub.size; ++i)
		os << pub.pairs[i].att << ' ' << pub.pairs[i].value << ' ';
	os <<  Kwd_rparen;
	return os;
}

ostream &		operator << (ostream &os, const IntervalSub &sub)
{
	os << Kwd_sub << Kwd_lparen << endl;
	os << encode_string(sub.uri) << ' ' << sub.size << ' ';
	for (int i=0; i<sub.size; ++i)
		os << sub.constraints[i].att << ' ' << sub.constraints[i].lowValue << ' ' << sub.constraints[i].highValue << ' ';
	return os << Kwd_rparen;
}

ostream & operator << (ostream &os, const AttributeValue &a)
{
	switch (a.type()) {
		case Siena_null: return os << Kwd_null;
		case Siena_string: {
			string s = a.string_value();
			return os << encode_string(s);
		}
		case Siena_integer:
			return os << a.int_value();
		case Siena_bool:
			return os << (a.bool_value() ? Kwd_true : Kwd_false);
		case Siena_double:
			return os << a.double_value();
		default:
			throw(BadType(a.type()));
	}
}

ostream & write_attribute_set(ostream & os, const AttributeSet &s) {
	for(AttributeSet::const_iterator si = s.begin(); si != s.end(); ++si) {
		os << (*si).first << Kwd_op_eq << (*si).second << endl;
	}
	return os;
}
ostream & operator << (ostream & os, const Request &f)
{
	os << Kwd_senp << Kwd_lparen << endl;
	return write_attribute_set(os, f) << Kwd_rparen;
}


//
// INPUT
//

//
// Tokenizer 
//
enum TokenType {
    TOK_unknown,
    TOK_eof,
    TOK_lparen,
    TOK_rparen,
    TOK_string,
    TOK_int,
    TOK_double,
    TOK_id,
    TOK_operator
};

struct Token {
    TokenType		type;
    union {
	SienaOperator	op;
	int		num;
	string *	str;
	double		dbl;
    };

    Token(): type(TOK_unknown) {};
    ~Token() {
	if (type == TOK_id || type == TOK_string)
	if (str != NULL) delete(str);
    }
};

//
// this is the tokenizer function, I know it's awfully long and it
// might seem complicated, but it's really simple if you look at it as
// a FSM
//
istream & operator >> (istream &is, Token &t)
{
    int c;
    //
    // first clears string possibly memorized in t
    //
    if (t.type == TOK_id || t.type == TOK_string)
	if (t.str != NULL) delete(t.str);
    t.type = TOK_unknown;
    //
    // initial state:
    //
    skip_spaces_and_comments:
    if ((c = is.get()) == EOF) {
	t.type = TOK_eof;
	return is;
    }
    if (isspace(c)) goto skip_spaces_and_comments;
    if (c == '/') {
	if ((c = is.get()) == '/') {
	    while ((c = is.get()) != EOF && c != '\n');
	    goto skip_spaces_and_comments;
	} else {
	    return is;
	}
    }
    //
    // reads a string literal 
    //
    if (c == '"') {
	t.type = TOK_string;
	t.str = new string();
    build_string_literal:
	if ((c = is.get()) == EOF) goto bad_string;
	if (c == '"') return is;
	if (c == '\\') {
	    if ((c = is.get()) == EOF) goto bad_string;
	    switch (c) {
	    case '\\': *t.str += '\\'; break;
	    case '"': *t.str += '"'; break;
	    case 'n': *t.str += '\n'; break;
	    case 't': *t.str += '\t'; break;
	    case '0': *t.str += '\0'; break;
		//
		// to be continued ...work in progress...
		//
	    }
	}
	else {
	    *t.str += c;
	}
	goto build_string_literal;
	//
	// EOF before string is closed by "
	//
	bad_string:
	t.type = TOK_unknown;
	delete(t.str);
    }
    //
    // reads a number (integer or double)
    //
    else if (isdigit(c) || c == '-') {
	bool negative = false;
	if (c == '-') {
	    negative = true;
	    if ((c = is.get()) == EOF || !isdigit(c)) return is;
	}
	t.type = TOK_int;
	t.num = 0;
	unsigned int floatlevel = 1;
	build_num_tok:
	if (t.type == TOK_int) {
	    t.num = t.num * 10 + c - '0';
	} else {
	    t.dbl = t.dbl + (double)(c - '0') / floatlevel;
	    floatlevel *= 10;
	}
	if ((c = is.get()) == EOF) return is;
	if (isdigit(c)) { goto build_num_tok; }
	else if (c == '.' && t.type == TOK_int) {
	    t.type = TOK_double;
	    t.dbl = t.num;
	    floatlevel = 10;
	    if ((c = is.get()) == EOF) return is;
	    if (isdigit(c)) goto build_num_tok;
	}
	is.putback(c);
	if (negative) {
	    if (t.type == TOK_int) { 
		t.num = -t.num; 
	    } else {
		t.dbl = -t.dbl;
	    }
	}
    }
    else if (c == '{') {
	t.type = TOK_lparen;
    }
    else if (c == '}') {
	t.type = TOK_rparen;
    }
    else if (c == '=') {
	t.type = TOK_operator;
	t.op = Siena_eq;
    }
    else if (c == '>') {
	t.type = TOK_operator;
	t.op = Siena_gt;
	if ((c = is.get()) == EOF) return is;
	if (c == '=') {
	    t.op = Siena_ge;
	} else if (c == '*') {
	    t.op = Siena_pf;
	} else {
	    is.putback(c);
	}
    }
    else if (c == '<') {
	t.type = TOK_operator;
	t.op = Siena_lt;
	if ((c = is.get()) == EOF) return is;
	if (c == '=') {
	    t.op = Siena_le;
	} else {
	    is.putback(c);
	}
    }
    else if (c == '*') {
	t.type = TOK_operator;
	t.op = Siena_ss;
	if ((c = is.get()) == EOF) return is;
	if (c == '<') {
	    t.op = Siena_sf;
	} else {
	    is.putback(c);
	}
    }
    else if (c == '!') {
	if ((c = is.get()) == '=') {
	    t.type = TOK_operator;
	    t.op = Siena_ne;
	} 
    }
    //
    // reads an id
    //
    else if (isalpha(c)) {
	t.type = TOK_id;
	t.str = new string();
	
	do {
	    *t.str += c;
	    if ((c = is.get()) == EOF) return is;
	} while(isalpha(c));
	is.putback(c);
    }
    return is;
}


istream &		operator >> (istream &is, Pub &pub)
{
	Token t;
	is >> t;
	if (t.type == TOK_eof) return is;
	if (t.type != TOK_id || *t.str != Kwd_pub)
		throw ParseError(PE_pub);
	is >> t;
	if (t.type != TOK_lparen) throw ParseError(PE_lparen);
	is >> t;
	if (t.type == TOK_int)
		pub.size = t.num;
	else
		throw ParseError(PE_pubsize);
	while (1)
	{
		is >> t;
		if (t.type == TOK_rparen) return is;
		Pair p;
		if (t.type == TOK_int)
			p.att = t.num;
		else
			throw ParseError(PE_pubatt);
		is >> t;
		if (t.type == TOK_int)
			p.value = t.num;
		else
			throw ParseError(PE_pubValue);
		pub.pairs.push_back(p);
	}
}


istream &		operator >> (istream &is, IntervalSub &sub)
{
	Token t;
	is >> t;
	if (t.type != TOK_id || *t.str != Kwd_sub)
		throw ParseError(PE_sub);
	is >> t;
	if (t.type != TOK_lparen) throw ParseError(PE_lparen);

	is >> t;
	//todo
	if (t.type==TOK_string) sub.uri = *(t.str);
	else throw ParseError(PE_uri);
	is >> t;
	if (t.type==TOK_int) sub.size = t.num;
	else throw ParseError(PE_subsize);

	while (1) {
		IntervalCnt c;
		is >> t;
		if (t.type == TOK_rparen) return is;
		if (t.type == TOK_int)
			c.att = t.num;
		else
			throw ParseError(PE_subatt);
		is >> t;

		if (t.type == TOK_int)
			c.lowValue = t.num;
		else
			throw ParseError(PE_lowValue);

		is >> t;

		if (t.type == TOK_int)
			c.highValue = t.num;
		else
			throw ParseError(PE_highValue);

		sub.constraints.push_back(c);
	}
}


istream & read_attribute_set(istream &is, AttributeSet &as)
{
	Token t;

	as.clear();
	while (1) {
		is >> t;
		if (t.type == TOK_rparen) return is;
		if (t.type != TOK_id)
			throw ParseError(PE_anamerparen);
		string name(*t.str);
		is >> t;
		if (t.type != TOK_operator || t.op != Siena_eq)
			throw ParseError(PE_equal);
		is >> as[name];
	}
}

istream & operator >> (istream &is, AttributeValue &a)
{
    Token t;

    is >> t;
    switch (t.type) {
    case TOK_int: a = t.num; break;
    case TOK_string: a = *t.str; break;
    case TOK_double: a = t.dbl; break;
    case TOK_id:
	if (*t.str == Kwd_true) {
	    a = true;
	} else if (*t.str == Kwd_false) {
	    a = false;
	} else if (*t.str == Kwd_null) {
	    a = AttributeValue();
	} else {
	    a = *t.str;
	}
	break;
    case TOK_eof: 
	a = AttributeValue();
	break;
    default: 
	throw ParseError(PE_literal);
    }
    return is;
}


istream & operator >> (istream &is, Request &o)
{
    Token t;
    is >> t;
    if (t.type == TOK_eof) return is;
    if (t.type != TOK_id || *t.str != Kwd_senp) 
	throw ParseError(PE_senp);
    is >> t;
    if (t.type != TOK_lparen) throw ParseError(PE_lparen);
    return read_attribute_set(is,o);
}


#ifdef HAVE_CXX_NAMESPACE
} // namespace Siena
#endif

