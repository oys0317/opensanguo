/* $Id: stage.cpp 38055 2009-08-24 00:24:46Z crab $ */
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
 * Stage of a composite AI
 * @file ai/composite/stage.cpp
 */

#include "ai.hpp"
#include "stage.hpp"
#include "../contexts.hpp"
#include "../../foreach.hpp"
#include "../../log.hpp"
#include <map>
#include <string>

namespace ai {

static lg::log_domain log_ai_composite_stage("ai/composite/stage");
#define DBG_AI_COMPOSITE_STAGE LOG_STREAM(debug, log_ai_composite_stage)
#define LOG_AI_COMPOSITE_STAGE LOG_STREAM(info, log_ai_composite_stage)
#define ERR_AI_COMPOSITE_STAGE LOG_STREAM(err, log_ai_composite_stage)

// =======================================================================
// COMPOSITE AI STAGE
// =======================================================================

stage::stage( ai_context &context, const config &cfg )
	: recursion_counter_(context.get_recursion_count()), cfg_(cfg)
{
	init_ai_context_proxy(context);
}

void stage::on_create()
{
	LOG_AI_COMPOSITE_STAGE << "side "<< get_side() << " : "<<" created stage with name=["<<cfg_["name"]<<"]"<<std::endl;
}

stage::~stage()
{
}

bool stage::play_stage()
{
	return do_play_stage();
}

int stage::get_recursion_count() const
{
	return recursion_counter_.get_count();
}

config stage::to_config() const
{
	config cfg;
	cfg["engine"] = cfg_["engine"];
	cfg["name"] = cfg_["name"];
	cfg["id"] = cfg_["id"];
	return cfg;
}


const std::string& stage::get_id() const
{
	return cfg_["id"];
}

// =======================================================================
// COMPOSITE AI IDLE STAGE
// =======================================================================


idle_stage::idle_stage( ai_context &context, const config &cfg )
	: stage(context,cfg)
{
}

idle_stage::~idle_stage()
{
}

bool idle_stage::do_play_stage(){
	LOG_AI_COMPOSITE_STAGE << "Turn " << get_info().tod_manager_.turn() << ": playing idle stage for side: "<< get_side() << std::endl;
	return false;
}

// =======================================================================
// COMPOSITE AI MINISTAGE
// =======================================================================

ministage::ministage(const config &cfg)
	: cfg_(cfg),stage_()
{
}

ministage::~ministage()
{
}


stage_ptr ministage::get_stage_ptr(ai_context &context)
{
	if (stage_) {
		return stage_;
	}

	std::vector<stage_ptr> stages;
	engine::parse_stage_from_config(context,cfg_,std::back_inserter(stages));
	if (stages.empty()) {
		return stage_ptr();
	}
	stage_ = stages.front();
	return stage_;
}

config ministage::to_config() const
{
	if (!stage_) {
		return cfg_;
	}
	return stage_->to_config();
}



} //end of namespace ai