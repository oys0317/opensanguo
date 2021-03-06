/* $Id: scroll_label.hpp 36576 2009-07-04 13:38:53Z mordante $ */
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

#ifndef GUI_AUXILIARY_WINDOW_BUILDER_SCROLL_LABEL_HPP_INCLUDED
#define GUI_AUXILIARY_WINDOW_BUILDER_SCROLL_LABEL_HPP_INCLUDED

#include "gui/auxiliary/window_builder/control.hpp"

namespace gui2 {

namespace implementation {

struct tbuilder_scroll_label
	: public tbuilder_control
{
	tbuilder_scroll_label(const config& cfg);

	twidget* build () const;

private:

	bool auto_hide_scrollbar_;
};

} // namespace implementation

} // namespace gui2

#endif

