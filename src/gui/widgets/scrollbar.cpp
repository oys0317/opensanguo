/* $Id: scrollbar.cpp 39094 2009-10-04 06:10:46Z mordante $ */
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

#include "gui/widgets/scrollbar.hpp"

#include "foreach.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/widgets/event_handler.hpp"
#include "gui/widgets/window.hpp" // Needed for invalidate_layout()

#include <boost/bind.hpp>

namespace gui2 {

tscrollbar_::tscrollbar_()
	: tcontrol(COUNT)
	, state_(ENABLED)
	, item_count_(0)
	, item_position_(0)
	, visible_items_(1)
	, step_size_(1)
	, pixels_per_step_(0.0)
	, mouse_(0, 0)
	, positioner_offset_(0)
	, positioner_length_(0)
	, callback_positioner_move_(0)
{
	connect_signal<event::MOUSE_ENTER>(boost::bind(
				&tscrollbar_::signal_handler_mouse_enter, this, _2, _3));
	connect_signal<event::MOUSE_MOTION>(boost::bind(
				&tscrollbar_::signal_handler_mouse_motion, this, _2, _3, _5));
	connect_signal<event::MOUSE_LEAVE>(boost::bind(
				&tscrollbar_::signal_handler_mouse_leave, this, _2, _3));
	connect_signal<event::LEFT_BUTTON_DOWN>(boost::bind(
				&tscrollbar_::signal_handler_left_button_down, this, _2, _3));
	connect_signal<event::LEFT_BUTTON_UP>(boost::bind(
				&tscrollbar_::signal_handler_left_button_up, this, _2, _3));
}

void tscrollbar_::scroll(const tscroll scroll)
{
	switch(scroll) {
		case BEGIN :
			set_item_position(0);
			break;

		case ITEM_BACKWARDS :
			if(item_position_) {
				set_item_position(item_position_ - 1);
			}
			break;

		case HALF_JUMP_BACKWARDS :
			set_item_position(item_position_ > (visible_items_ / 2) ?
				item_position_ - (visible_items_ / 2) : 0);
			break;

		case JUMP_BACKWARDS :
			set_item_position(item_position_ > visible_items_ ?
				item_position_ - visible_items_  : 0);
			break;

		case END :
			set_item_position(item_count_ - 1);
			break;

		case ITEM_FORWARD :
			set_item_position(item_position_ + 1);
			break;

		case HALF_JUMP_FORWARD :
			set_item_position(item_position_ +  (visible_items_ / 2));
			break;

		case JUMP_FORWARD :
			set_item_position(item_position_ +  visible_items_ );
			break;

		default :
			assert(false);
		}
}

void tscrollbar_::set_size(const tpoint& origin, const tpoint& size)
{
	// Inherited.
	tcontrol::set_size(origin, size);

	recalculate();
}

#ifdef GUI2_OLD_EVENT_HANDLING
void tscrollbar_::mouse_move(tevent_handler& event)
{
	tpoint mouse = event.get_mouse();
	mouse.x -= get_x();
	mouse.y -= get_y();

	DBG_GUI_E << "Scrollbar: mouse move at " << mouse << ".\n";

	switch(state_) {
		case ENABLED :
			if(on_positioner(mouse)) {
				set_state(FOCUSSED);
			}

			break;
		case DISABLED :
			// do nothing
			break;

		case PRESSED : {
				const int distance = get_length_difference(mouse_, mouse);
				mouse_ = mouse;
				move_positioner(distance);
			}
			break;

		case FOCUSSED :
			if(!on_positioner(mouse)) {
				set_state(ENABLED);
			}
			break;

		default :
			assert(false);
	}
}

void tscrollbar_::mouse_leave(tevent_handler&)
{
	DBG_GUI_E << "Scrollbar: mouse leave.\n";

	if(state_ == FOCUSSED) {
		set_state(ENABLED);
	}
}

void tscrollbar_::mouse_left_button_down(tevent_handler& event)
{
	tpoint mouse = event.get_mouse();
	mouse.x -= get_x();
	mouse.y -= get_y();

	DBG_GUI_E << "Scrollbar: mouse down at " << mouse << ".\n";

	if(on_positioner(mouse)) {
		mouse_ = mouse;
		event.mouse_capture();
		set_state(PRESSED);
	}

	const int bar = on_bar(mouse);

	if(bar == -1) {
		scroll(HALF_JUMP_BACKWARDS);
		if(callback_positioner_move_) {
			callback_positioner_move_(this);
		}
	} else if(bar == 1) {
		scroll(HALF_JUMP_FORWARD);
		if(callback_positioner_move_) {
			callback_positioner_move_(this);
		}
	} else {
		assert(bar == 0);
	}
}

void tscrollbar_::mouse_left_button_up(tevent_handler& event)
{
	tpoint mouse = event.get_mouse();
	mouse.x -= get_x();
	mouse.y -= get_y();

	DBG_GUI_E << "Scrollbar: mouse up at " << mouse << ".\n";

	if(state_ != PRESSED) {
		return;
	}

	event.mouse_capture(false);

	if(on_positioner(mouse)) {
		set_state(FOCUSSED);
	} else {
		set_state(ENABLED);
	}
}
#endif

void tscrollbar_::set_item_position(const unsigned item_position)
{
	// Set the value always execute since we update a part of the state.
	item_position_ = item_position + visible_items_ > item_count_ ?
		item_count_ - visible_items_ : item_position;

	item_position_ = (item_position_ + step_size_ - 1) / step_size_;

	// Determine the pixel offset of the item position.
	positioner_offset_ = static_cast<unsigned>(item_position_ * pixels_per_step_);

	update_canvas();
}

void tscrollbar_::update_canvas() {

	foreach(tcanvas& tmp, canvas()) {
		tmp.set_variable("positioner_offset", variant(positioner_offset_));
		tmp.set_variable("positioner_length", variant(positioner_length_));
	}
	set_dirty();
}

void tscrollbar_::set_state(const tstate state)
{
	if(state != state_) {
		state_ = state;
		set_dirty(true);
	}
}

void tscrollbar_::recalculate()
{
	// We can be called before the size has been set up in that case we can't do
	// the proper recalcultion so stop before we die with an assert.
	if(!get_length()) {
		return;
	}

	// Get the available size for the slider to move.
	const int available_length =
		get_length() - offset_before() - offset_after();

	assert(available_length > 0);

	// All visible.
	if(item_count_ <= visible_items_) {
		positioner_offset_ = offset_before();
		positioner_length_ = available_length;
		recalculate_positioner();
		item_position_ = 0;
		update_canvas();
		return;
	}

	/**
	 * @todo In the MP lobby it can happen that a listbox has first zero items,
	 * then gets filled and since there are no visible items the second assert
	 * after this block will be triggered. Use this ugly hack to avoid that
	 * case. (This hack also added the gui/widgets/window.hpp include.)
	 */
	if(!visible_items_) {
		twindow* window = get_window();
		assert(window);
		window->invalidate_layout();
		ERR_GUI_G << "Scrollbar: Can't recalculate size, force a Window layout phase.\n";
		return;
	}

	assert(step_size_);
	assert(visible_items_);

	const unsigned steps = (item_count_ - visible_items_ + step_size_ - 1) / step_size_;

	positioner_length_ = available_length * visible_items_ / item_count_;
	recalculate_positioner();

	// Make sure we can also show the last step, so add one more step.
	pixels_per_step_ =
		(available_length - positioner_length_)
		/ static_cast<float>(steps + 1);

	set_item_position(item_position_);
#if 0
	std::cerr << "Scrollbar recalculate overview:\n"
		<< "item_count_ " << item_count_
		<< " visible_items_ " << visible_items_
		<< " step_size_ " << step_size_
		<< " steps " << steps
		<< "\n"
		<< "minimum_positioner_length() " << minimum_positioner_length()
		<< " maximum_positioner_length() " << maximum_positioner_length()
		<< "\n"
		<< " positioner_length_ " << positioner_length_
		<< " positioner_offset_ " << positioner_offset_
		<< "\n"
		<< "available_length " << available_length
		<< " pixels_per_step_ " << pixels_per_step_
		<< ".\n\n";
#endif
}

void tscrollbar_::recalculate_positioner()
{
	const unsigned minimum = minimum_positioner_length();
	const unsigned maximum = maximum_positioner_length();

	if(minimum == maximum) {
		positioner_length_ = maximum;
	} else if(maximum != 0 && positioner_length_ > maximum) {
		positioner_length_ = maximum;
	} else if(positioner_length_ < minimum) {
		positioner_length_ = minimum;
	}
}

void tscrollbar_::move_positioner(const int distance)
{
	if(distance < 0 && -distance > static_cast<int>(positioner_offset_)) {
		positioner_offset_ = 0;
	} else {
		positioner_offset_ += distance;
	}

	const unsigned length = get_length() - offset_before() - offset_after() - positioner_length_;

	if(positioner_offset_ > length) {
		positioner_offset_ = length;
	}

	unsigned position =
		static_cast<unsigned>(positioner_offset_ / pixels_per_step_);

	// Note due to floating point rounding the position might be outside the
	// available positions so set it back.
	if(position > item_count_ - visible_items_) {
		position = item_count_ - visible_items_;
	}

	if(position != item_position_) {
		item_position_ = position;

		child_callback_positioner_moved();

		if(callback_positioner_move_) {
			callback_positioner_move_(this);
		}
	}
#if 0
	std::cerr << "Scrollbar move overview:\n"
		<< "item_count_ " << item_count_
		<< " visible_items_ " << visible_items_
		<< " step_size_ " << step_size_
		<< "\n"
		<< "minimum_positioner_length() " << minimum_positioner_length()
		<< " maximum_positioner_length() " << maximum_positioner_length()
		<< "\n"
		<< " positioner_length_ " << positioner_length_
		<< " positioner_offset_ " << positioner_offset_
		<< "\n"
		<< " pixels_per_step_ " << pixels_per_step_
		<< " item_position_ " << item_position_
		<< ".\n\n";
#endif
	update_canvas();
}

void tscrollbar_::load_config_extra()
{
	// These values won't change so set them here.
	foreach(tcanvas& tmp, canvas()) {
		tmp.set_variable("offset_before", variant(offset_before()));
		tmp.set_variable("offset_after", variant(offset_after()));
	}
}

void tscrollbar_::signal_handler_mouse_enter(
		const event::tevent event, bool& handled)
{
	DBG_GUI_E << get_control_type() << "[" << id() << "]: " << event << ".\n";

	// Send the motion under our event id to make debugging easier.
	signal_handler_mouse_motion(event, handled, get_mouse_position());
}

void tscrollbar_::signal_handler_mouse_motion(
		const event::tevent event, bool& handled, const tpoint& coordinate)
{
	DBG_GUI_E << get_control_type()
			<< "[" << id() << "]: " << event << " at " << coordinate << ".\n";

	tpoint mouse = coordinate;
	mouse.x -= get_x();
	mouse.y -= get_y();

	switch(state_) {
		case ENABLED :
			if(on_positioner(mouse)) {
				set_state(FOCUSSED);
			}

			break;

		case PRESSED : {
				const int distance = get_length_difference(mouse_, mouse);
				mouse_ = mouse;
				move_positioner(distance);
			}
			break;

		case FOCUSSED :
			if(!on_positioner(mouse)) {
				set_state(ENABLED);
			}
			break;

		case DISABLED :
			// Shouldn't be possible.

			/* FALL DOWN */
		default :
			assert(false);
	}
	handled = true;
}

void tscrollbar_::signal_handler_mouse_leave(
		const event::tevent event, bool& handled)
{
	DBG_GUI_E << get_control_type() << "[" << id() << "]: " << event << ".\n";

	if(state_ == FOCUSSED) {
		set_state(ENABLED);
	}
	handled = true;
}


void tscrollbar_::signal_handler_left_button_down(
		const event::tevent event, bool& handled)
{
	DBG_GUI_E << get_control_type() << "[" << id() << "]: " << event << ".\n";

	tpoint mouse = get_mouse_position();
	mouse.x -= get_x();
	mouse.y -= get_y();

	if(on_positioner(mouse)) {
		assert(get_window());
		mouse_ = mouse;
		get_window()->mouse_capture();
		set_state(PRESSED);
	}

	const int bar = on_bar(mouse);

	if(bar == -1) {
		scroll(HALF_JUMP_BACKWARDS);
		if(callback_positioner_move_) {
			callback_positioner_move_(this);
		}
	} else if(bar == 1) {
		scroll(HALF_JUMP_FORWARD);
		if(callback_positioner_move_) {
			callback_positioner_move_(this);
		}
	} else {
		assert(bar == 0);
	}

	handled = true;
}

void tscrollbar_::signal_handler_left_button_up(
		const event::tevent event, bool& handled)
{
	DBG_GUI_E << get_control_type() << "[" << id() << "]: " << event << ".\n";

	tpoint mouse = get_mouse_position();
	mouse.x -= get_x();
	mouse.y -= get_y();

	if(state_ != PRESSED) {
		return;
	}

	assert(get_window());
	get_window()->mouse_capture(false);

	if(on_positioner(mouse)) {
		set_state(FOCUSSED);
	} else {
		set_state(ENABLED);
	}

	handled = true;
}

} // namespace gui2
