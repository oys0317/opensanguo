/* $Id: formula_fwd.hpp 37680 2009-08-11 20:15:19Z crab $ */
/*
   Copyright (C) 2008 - 2009 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef FORMULA_FWD_HPP_INCLUDED
#define FORMULA_FWD_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

namespace game_logic {

class formula_debugger;

class formula;
typedef boost::shared_ptr<formula> formula_ptr;
typedef boost::shared_ptr<const formula> const_formula_ptr;

}

#endif
