/* $Id: event_handler.hpp 39245 2009-10-11 13:36:37Z mordante $ */
/*
   Copyright (C) 2007 - 2009 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file event_handler.hpp
 * Contains the information with an event.
 */

#ifndef GUI_WIDGETS_EVENT_INFO_HPP_INCLUDED
#define GUI_WIDGETS_EVENT_INFO_HPP_INCLUDED

#ifdef GUI2_OLD_EVENT_DISPATCHER

#ifdef GUI2_OLD_EVENT_HANDLING
#include "events.hpp"
#endif
#include "gui/widgets/event_executor.hpp"
#include "gui/auxiliary/event/handler.hpp"

#include <string>
#include <SDL.h>

class t_string;

namespace gui2{

struct tpoint;
class twidget;
class twindow;

/** The event handler class for the widget library. */
class tevent_handler
#ifdef GUI2_OLD_EVENT_HANDLING
	: public events::handler
#endif
{
public:
	tevent_handler();

#ifndef GUI2_OLD_EVENT_HANDLING
	virtual ~tevent_handler() {}
#else
	~tevent_handler() { leave(); }
#endif

#ifdef GUI2_OLD_EVENT_HANDLING
	/** Inherited from events::handler. */
	void process_events() { events::pump(); }

	/** Inherited from events::handler. */
	void handle_event(const SDL_Event& event);
#endif

	/** Returns the main window. */
	virtual twindow& get_window() = 0;

	/** Returns the main window. */
	virtual const twindow& get_window() const = 0;

	/** See twidget::find_at() for the description. */
	virtual twidget* find_at(const tpoint& coordinate,
			const bool must_be_active) = 0;

	/** The const version of find_at. */
	virtual const twidget* find_at(const tpoint& coordinate,
		const bool must_be_active) const = 0;

	/**
	 * Captures the mouse input.
	 *
	 * When capturing the widget that has the mouse focus does the capturing.
	 *
	 * @param capture             Set or release the capturing.
	 */
	void mouse_capture(const bool capture = true);

	/**
	 * Captures the keyboard input.
	 *
	 * @param widget              The widget which should capture the keyboard.
	 *                            Sending NULL releases the capturing.
	 */
	void keyboard_capture(twidget* widget);

	/**
	 * Adds the widget to the keyboard chain.
	 *
	 * @param widget              The widget to add to the chain. The widget
	 *                            should be valid widget, which hasn't been
	 *                            added to the chain yet.
	 */
	void add_to_keyboard_chain(twidget* widget);

	/**
	 * Remove the widget from the keyborad chain.
	 *
	 * @parameter widget          The widget to be removed from the chain.
	 */
	void remove_from_keyboard_chain(twidget* widget);

	/** Return the current mouse position. */
	tpoint get_mouse() const;

	/**
	 * Initializes the location of the mouse.
	 *
	 * After a layout of the window the mouse location needs to be updated to
	 * test whether it entered or left a widget.
	 */
	void init_mouse_location();

	/**
	 * Shows a tooltip.
	 *
	 * A tooltip is a small shortly visible item which is meant to show the user
	 * extra information. It shows after a short time hovering over a widget and
	 * automatically disappears again after a while. Only one tooltip or help
	 * message can be active at a time.
	 *
	 * @param message             The message to show.
	 * @param timeout             The time the tooltip is shown, 0 means
	 *                            forever.
	 */
	void show_tooltip(const t_string& message, const unsigned timeout);

	/** Removes the currently shown tooltip. */
	void remove_tooltip();

	/**
	 * Shows a help message.
	 *
	 * A help message is like a tooltip, but in general contains more info and
	 * the user needs to trigger it (most of the time with the F1 button).
	 *
	 * @param message             The message to show.
	 * @param timeout             The time the help message is shown, 0 means
	 *                            forever.
	 */
	void show_help_popup(const t_string& message, const unsigned timeout);

	/** Removes the currently show tooltip. */
	void remove_help_popup();

	/**
	 * Handles the removal of a widget.
	 *
	 * When a widget is destroyed in a window, it _must_ call this function
	 * so all pointers stored here are also removed.
	 *
	 * @param widget              The widget being destroyed.
	 */
	void remove_widget(const twidget* widget);

private:

	/**
	 * A mouse button.
	 *
	 * The class tracks the state of the mouse button and which functions to
	 * invoke upon state changes.
	 * */
	struct tmouse_button {

		tmouse_button(
				  const std::string& name
#ifdef GUI2_OLD_EVENT_HANDLING
				, void (tevent_executor::*down) (tevent_handler&)
				, void (tevent_executor::*up) (tevent_handler&)
				, void (tevent_executor::*click) (tevent_handler&)
				, void (tevent_executor::*double_click) (tevent_handler&)
#endif
				, bool (tevent_executor::*wants_double_click) () const
				, const event::tevent sdl_button_down
				, const event::tevent sdl_button_up
				, const event::tevent button_down
				, const event::tevent button_up
				, const event::tevent button_click
				, const event::tevent button_double_click)
			: last_click_stamp(0)
			, last_clicked_widget(NULL)
			, focus(0)
			, name(name)
#ifdef GUI2_OLD_EVENT_HANDLING
			, down(down)
			, up(up)
			, click(click)
			, double_click(double_click)
#endif
			, wants_double_click(wants_double_click)
			, is_down(false)
			, sdl_button_down_(sdl_button_down)
			, sdl_button_up_(sdl_button_up)
			, button_down_(button_down)
			, button_up_(button_up)
			, button_click_(button_click)
			, button_double_click_(button_double_click)
		{
		}

		/** The time of the last click used for double clicking. */
		Uint32 last_click_stamp;

		/** The widget the last click was on, used for double clicking. */
		twidget* last_clicked_widget;

		/**
		 * If the mouse isn't captured we need to verify the up is on the same
		 * widget as the down so we send a proper click, also needed to send the
		 * up to the right widget.
		 */
		twidget* focus;

		/** used for debug messages. */
		const std::string name;

		/**
		 * Pointers to member functions, this way we can call the proper
		 * function indirect without writing a case for which button to
		 * use.
		 */
#ifdef GUI2_OLD_EVENT_HANDLING
		void (tevent_executor::*down) (tevent_handler&);
		void (tevent_executor::*up) (tevent_handler&);
		void (tevent_executor::*click) (tevent_handler&);
		void (tevent_executor::*double_click) (tevent_handler&);
#endif
		bool (tevent_executor::*wants_double_click) () const;

		/** Is the button down? */
		bool is_down;

		/** Events to fire. */
		const event::tevent sdl_button_down_;
		const event::tevent sdl_button_up_;
		const event::tevent button_down_;
		const event::tevent button_up_;
		const event::tevent button_click_;
		const event::tevent button_double_click_;
	};

#ifdef GUI2_OLD_EVENT_HANDLING
	/**
	 * We create a new event context so we're always modal. Maybe this has to
	 * change, but not sure yet.
	 */
	events::event_context event_context_;
#endif

	int mouse_x_;                      /**< The current mouse x. */
	int mouse_y_;                      /**< The current mouse y. */

	tmouse_button left_;               /**< The left mouse button. */
	tmouse_button middle_;             /**< The middle mouse button. */
	tmouse_button right_;              /**< The right mouse button. */

	bool hover_pending_;			   /**< Is there a hover event pending? */
	unsigned hover_id_;                /**< Id of the pending hover event. */
	SDL_Rect hover_box_;               /**< The area the mouse can move in,
										*   moving outside invalidates the
										*   pending hover event.
										*/

	bool had_hover_;                   /**< A widget only gets one hover event
	                                    *   per enter cycle.
										*/

	/** The widget of the currently active tooltip. */
	twidget* tooltip_;

	/** The widget of the currently active help popup. */
	twidget* help_popup_;

    /** The widget that currently has the moue focus. */
	twidget* mouse_focus_;

	/** Did the current widget capture the focus? */
	bool mouse_captured_;

	/** The widget that holds the keyboard focus. */
	twidget* keyboard_focus_;

	/**
	 * Fall back keyboard focus items.
	 *
	 * When the focussed widget didn't handle the keyboard event (or no handler
	 * for the keyboard focus) it is send all widgets in this vector. The order
	 * is from rbegin() to rend().  If the keyboard_focus_ is in the vector it
	 * won't get the event twice. The first item added to the vector should be
	 * the window, so it will be the last handler and can dispatch the hotkeys
	 * registered.
	 */
	std::vector<twidget*> keyboard_focus_chain_;

	/**
	 * Set of functions that handle certain events and sends them to the proper
	 * widget. These functions are called by the SDL event handling functions.
	 */

#ifdef GUI2_OLD_EVENT_HANDLING
	/**
	 * Called when the mouse enters a widget.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param mouse_over          The widget that should receive the event.
	 */
	void mouse_enter(const SDL_Event& event, twidget* mouse_over);
#endif
private:
	/**
	 * Called when the mouse enters a widget.
	 *
	 * @param mouse_over          The widget that should receive the event.
	 */
	void mouse_enter(twidget* mouse_over);
public:
#ifdef GUI2_OLD_EVENT_HANDLING
	/**
	 * Called when the mouse moves over a widget.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param mouse_over          The widget that should receive the event.
	 */
	void mouse_move(const SDL_Event& event, twidget* mouse_over);
#endif
private:
	/**
	 * Called when the mouse moves over a widget.
	 *
	 * @param mouse_over          The widget that should receive the event.
	 * @param coordinate          The current screen coordinate of the mouse.
	 */
	void mouse_motion(twidget* mouse_over, const tpoint& coordinate);
public:
#ifdef GUI2_OLD_EVENT_HANDLING
	/**
	 * Called when a widget should raises a hover event.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param mouse_over          The widget that should receive the event.
	 */
	void mouse_hover(const SDL_Event& event, twidget* mouse_over);

	/**
	 * Called when the mouse leaves a widget.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param mouse_over          The widget that should receive the event.
	 */
	void mouse_leave(const SDL_Event& event, twidget* mouse_over);
#endif
private:
	/** Called when the mouse leaves the current widget. */
	void mouse_leave();
public:
#ifdef GUI2_OLD_EVENT_HANDLING
	/**
	 * Called when a mouse button is pressed on a widget.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param mouse_over          The widget that should receive the event. This
	 *                            widget can be NULL and capturing the mouse
	 *                            can send the event to another widget.
	 * @param button              The button which was used to generate the event.
	 */
	void mouse_button_down(
		const SDL_Event& event, twidget* mouse_over, tmouse_button& button);

	/**
	 * Called when a mouse button is released.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param mouse_over          The widget that should receive the event. This
	 *                            widget can be NULL and capturing the mouse
	 *                            can send the event to another widget.
	 * @param button              The button which was used to generate the event.
	 */
	void mouse_button_up(
		const SDL_Event& event, twidget* mouse_over, tmouse_button& button);

	/**
	 * Called when a mouse click is generated.
	 *
	 * Note if the widget wants a double click a double click might be send or
	 * the click might be delayed to wait for a double click.
	 *
	 * @param widget              The widget that should receive the event.
	 * @param button              The button which was used to generate the event.
	 */
	void mouse_click(twidget* widget, tmouse_button& button);

	/**
	 * Called when a scroll wheel is scrolled.
	 *
	 * The wheel event is always send to the widget underneath the mouse and
	 * ignores the focus. If a widget doesn't handle the event it's send to
	 * its parent. The event won't escape the current window. The event moves
	 * up through inactive widgets.
	 *
	 * @todo document this event in the event handling document.
	 *
	 * @param event               The SDL_Event which was triggered.
	 * @param widget              The widget the mouse is over, this widget
	 *                            might not be active and be NULL so the code
	 *                            needs to test for it.
	 */
	void mouse_wheel(const SDL_Event& event, twidget* widget);

	/**
	 * Called when a mouse button is pressed.
	 *
	 * When a mouse button is pressed it's direct parent container is
	 * notified. If the parent container is the parent window this function
	 * doesn't notify the parent. The funcion focus_parent_window will send
	 * the event.
	 *
	 * @param widget              The widget that generated the event.
	 */
	 void focus_parent_container(twidget* widget);

	/**
	 * Called when a mouse button is pressed.
	 *
	 * When a mouse button is pressed it's direct parent window is send the
	 * focus event.
	 *
	 * @param widget              The widget that generated the event.
	 */
	 void focus_parent_window(twidget* widget);
#endif

	/**
	 * Raises a hover request.
	 *
	 * @param test_on_widget      Do we need to test whether we're on a widget.
	 */
	void set_hover(const bool test_on_widget = false);

#ifdef GUI2_OLD_EVENT_HANDLING
	/**
	 * A key has been pressed.
	 *
	 * @param event               The SDL_Event which was triggered.
	 */
	void key_down(const SDL_Event& event);
#endif

	/**
	 * The function to do the real job of showing the tooltip.
	 *
	 * @param location            The location in the window where to show the
	 *                            tooltip.
	 * @param tooltip             The message to show.
	 */
	virtual void do_show_tooltip(
		const tpoint& location, const t_string& tooltip) = 0;

	/** Function to do the real removal of the tooltip. */
	virtual void do_remove_tooltip() = 0;

	/**
	 * The function to do the real job of showing the help popup.
	 *
	 * @param location            The location in the window where to show the
	 *                            help popup.
	 * @param help_popup          The message to show.
	 */
	virtual void do_show_help_popup(
		const tpoint& location, const t_string& help_popup) = 0;

	/** Function to do the real removal of the help popup. */
	virtual void do_remove_help_popup() = 0;

	/**
	 * Handler for the click dismiss functionallity.
	 *
	 * @returns                   True if the click dismiss action is performed,
	 *                            false otherwise.
	 */
	virtual bool click_dismiss() = 0;


public:
	/**
	 * Connects the internal signals for the handler.
	 *
	 * @param widget              The widget to connect the signals to.
	 */
	void connect_signals(twidget& w);

private:

	void sdl_mouse_motion(const event::tevent event, const tpoint& coordinate);
	void sdl_left_button_down(const tpoint& coordinate);
	void sdl_left_button_up(const tpoint& coordinate);
	void sdl_middle_button_down(const tpoint& coordinate);
	void sdl_middle_button_up(const tpoint& coordinate);
	void sdl_right_button_down(const tpoint& coordinate);
	void sdl_right_button_up(const tpoint& coordinate);

	void button_down(twidget* widget
			, tmouse_button& button
			, const tpoint& coordinate);
	void button_up(twidget* widget
			, tmouse_button& button
			, const tpoint& coordinate);
	void mouse_button_click(twidget* widget, tmouse_button& button);

	void sdl_key_down(const SDLKey key
			, const SDLMod modifier
			, const Uint16 unicode);

	void sdl_wheel(const event::tevent event);
};

} // namespace gui2

#endif
#endif
