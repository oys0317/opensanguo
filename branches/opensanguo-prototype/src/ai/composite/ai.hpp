/* $Id: ai.hpp 38055 2009-08-24 00:24:46Z crab $ */
/*
   Copyright (C) 2009 by Yurii Chernyi <terraninfo@terraninfo.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file ai/composite/ai.hpp
 * Composite AI with turn sequence which is a vector of stages
 */

#ifndef AI_COMPOSITE_AI_HPP_INCLUDED
#define AI_COMPOSITE_AI_HPP_INCLUDED

#include "../../global.hpp"

#include "aspect.hpp"
#include "contexts.hpp"
#include "engine.hpp"
#include "goal.hpp"
#include "stage.hpp"
#include "../contexts.hpp"
#include "../default/contexts.hpp"
#include "../interface.hpp"
#include "component.hpp"

#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
//silence "inherits via dominance" warnings
#pragma warning(disable:4250)
#endif

//============================================================================
namespace ai {

class ai_composite : public ai_context, public virtual default_ai_context_proxy, public interface, public component {
public:


	/**
	 * Constructor
	 */
	ai_composite( default_ai_context &context, const config &cfg );


	/**
	 * Destructor
	 */
	virtual ~ai_composite();


	/**
	 * Play the turn
	 */
	void play_turn();


	/**
	 * Evaluate command (using fai)
	 */
        virtual std::string evaluate(const std::string& str);

	/**
	 * On new turn
	 */
	virtual void new_turn();


	std::string describe_self();

	/**
	 * serialize
	 */
	virtual config to_config() const;


	int get_recursion_count() const;


	void switch_side(side_number side);


	virtual bool add_goal(int pos, const config &cfg);


	virtual bool add_stage(int pos, const config &cfg);


	void on_create();

	/**
	 * unwrap
	 */
	virtual ai_context& get_ai_context();


	virtual component* get_child(const path_element &child);


	virtual bool add_child(const path_element &child, const config &cfg);


	virtual bool change_child(const path_element &child, const config &cfg);


	virtual bool delete_child(const path_element &child);

protected:

	/**
	 * Config of the AI
	 */
	const config &cfg_;


	/**
	 * Stages of the composite AI
	 */
	std::vector< stage_ptr > stages_;


	/**
	 * Recursion counter
	 */
	recursion_counter recursion_counter_;
};

} //end of namespace ai

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
