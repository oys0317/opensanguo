/* $Id: scroll_label.cpp 37882 2009-08-16 20:29:09Z mordante $ */
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

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/auxiliary/window_builder/scroll_label.hpp"

#include "config.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/widgets/scroll_label.hpp"

namespace gui2 {

namespace implementation {

tbuilder_scroll_label::tbuilder_scroll_label(const config& cfg)
	: implementation::tbuilder_control(cfg)
	, auto_hide_scrollbar_(
		utils::string_bool(cfg["auto_hide_scrollbar"], true))
{
}

twidget* tbuilder_scroll_label::build() const
{
	tscroll_label* widget = new tscroll_label();

	init_control(widget);

	boost::intrusive_ptr<const tscroll_label_definition::tresolution> conf =
		boost::dynamic_pointer_cast
		<const tscroll_label_definition::tresolution>(widget->config());
	assert(conf);

	widget->init_grid(conf->grid);
	widget->finalize_setup();

	DBG_GUI_G << "Window builder: placed scroll label '"
			<< id << "' with defintion '"
			<< definition << "'.\n";

	return widget;
}

} // namespace implementation

} // namespace gui2

/*WIKI_MACRO
 * @start_macro = scroll_label_description
 *
 *        A scroll label is a label that wraps its text and also has a
 *        vertical scrollbar. This way a text can't be too long to be shown
 *        for this widget.
 * @end_macro
 */

/*WIKI
 * @page = GUIWidgetInstanceWML
 * @order = 2_scroll_label
 *
 * == Scroll label ==
 *
 * @macro = scroll_label_description
 *
 * A scroll label has no special fields.
 *
 * @start_table = config
 *     auto_hide_scrollbar (bool = true)
 *                                     Automatically hide the scrollbar when
 *                                     the text can be shown without the
 *                                     scrollbar.
 * @end_table
 */

