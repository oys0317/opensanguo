/* $Id: formula_tokenizer.cpp 37742 2009-08-14 12:35:36Z dragonking $ */
/*
   Copyright (C) 2007 - 2009 by David White <dave.net>
   Part of the Silver Tree Project

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 or later.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include <sstream>

#include <boost/regex.hpp>

#include "foreach.hpp"
#include "formula_tokenizer.hpp"

namespace formula_tokenizer
{

namespace {

using boost::regex;

struct token_type {
	regex re;
	TOKEN_TYPE type;
};

//create the array with list of possible tokens
token_type token_types[] = {	{ regex("^(not\\b|and\\b|or\\b|d(?=[^a-zA-Z])|where\\b)"), TOKEN_OPERATOR },
				{ regex("^\\n"),               TOKEN_EOL },
				{ regex("^([ ]|\\t|\\r|\\f)"), TOKEN_WHITESPACE },
				{ regex("^\\("),               TOKEN_LPARENS },
				{ regex("^\\)"),               TOKEN_RPARENS },
				{ regex("^\\["),               TOKEN_LSQUARE },
				{ regex("^\\]"),               TOKEN_RSQUARE },
				{ regex("^->"),                TOKEN_POINTER },
				{ regex("^'[^']*'"),           TOKEN_STRING_LITERAL },
				{ regex("^fai\\b"),            TOKEN_KEYWORD },
				{ regex("^faiend\\b"),         TOKEN_KEYWORD },
				{ regex("^functions\\b"),      TOKEN_KEYWORD },
				{ regex("^def\\b"),            TOKEN_KEYWORD },
				{ regex("^[a-zA-Z_]+"),        TOKEN_IDENTIFIER },
				{ regex("^\\d+\\.\\d+"),       TOKEN_DECIMAL },
				{ regex("^\\d+"),              TOKEN_INTEGER },
{ regex("^(\\*|\\+|-(?=[^>])|\\^|%|/|<=|>=|<|>|!=|=|\\.\\+|\\.-|\\.\\*|\\./|\\.)"), TOKEN_OPERATOR },
				{ regex("^,"),                 TOKEN_COMMA },
				{ regex("^;"),                 TOKEN_SEMICOLON },
				{ regex("^#.*?#"),             TOKEN_COMMENT }
};

}

token get_token(iterator& i1, iterator i2) {
	foreach(const token_type& t, token_types) {
		boost::smatch match;
		if(boost::regex_search(i1, i2, match, t.re, boost::match_single_line)) {
			token res;
			res.type = t.type;
			res.begin = i1;
			i1 = res.end = i1 + match.length();

			return res;
		}
	}

	std::ostringstream expr;
	while( (i1 != i2) && (*i1 != '\n') ) {
		if( (*i1 != '\t') )
			expr << *i1;
		++i1;
	}

	throw token_error("Unrecognized token", expr.str() );
}

}

#ifdef UNIT_TEST_TOKENIZER

int main()
{
	using namespace formula_tokenizer;
	std::string test = "(abc + 4 * (5+3))^2";
	std::string::const_iterator i1 = test.begin();
	std::string::const_iterator i2 = test.end();
	TOKEN_TYPE types[] = {TOKEN_LPARENS, TOKEN_IDENTIFIER,
	                      TOKEN_WHITESPACE, TOKEN_OPERATOR,
						  TOKEN_WHITESPACE, TOKEN_INTEGER,
						  TOKEN_WHITESPACE, TOKEN_OPERATOR,
						  TOKEN_WHITESPACE, TOKEN_LPARENS,
						  TOKEN_INTEGER, TOKEN_OPERATOR,
						  TOKEN_INTEGER, TOKEN_RPARENS,
						  TOKEN_RPARENS, TOKEN_KEYWORD,
	                      TOKEN_OPERATOR, TOKEN_INTEGER};
	std::string tokens[] = {"(", "abc", " ", "+", " ", "4", " ",
	                        "*", " ", "(", "5", "+", "3", ")", ")", "functions"};
	for(int n = 0; n != sizeof(types)/sizeof(*types); ++n) {
		token t = get_token(i1,i2);
		assert(std::string(t.begin,t.end) == tokens[n]);
		assert(t.type == types[n]);

	}
}

#endif
