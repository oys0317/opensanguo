/* $Id: panel.hpp 36392 2009-06-24 19:42:03Z mordante $ */
/*
   Copyright (C) 2008 - 2009 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_AUXILIARY_WINDOW_BUILDER_PANEL_HPP_INCLUDED
#define GUI_AUXILIARY_WINDOW_BUILDER_PANEL_HPP_INCLUDED

#include "gui/auxiliary/window_builder/control.hpp"

namespace gui2 {

namespace implementation {

struct tbuilder_panel
	: public tbuilder_control
{
	tbuilder_panel(const config& cfg);

	twidget* build () const;

	tbuilder_grid_ptr grid;
};

} // namespace implementation

} // namespace gui2

#endif

