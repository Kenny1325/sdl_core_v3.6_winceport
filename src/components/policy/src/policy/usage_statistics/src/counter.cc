/*
 Copyright (c) 2013, Ford Motor Company
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the
 distribution.

 Neither the name of the Ford Motor Company nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SRC_COMPONENTS_POLICY_INCLUDE_POLICY_STATISTICS_MANAGER_H_
#define SRC_COMPONENTS_POLICY_INCLUDE_POLICY_STATISTICS_MANAGER_H_
#ifdef MODIFY_FUNCTION_SIGN
#include <global_first.h>
#endif
#include "usage_statistics/counter.h"
#include <cassert>

namespace usage_statistics {

GlobalCounter::GlobalCounter(StatisticsManager* statistics_manager,
                             GlobalCounterId counter_type)
    : counter_type_(counter_type),
      statistics_manager_(statistics_manager) {
}

void GlobalCounter::operator++() const {
  if (statistics_manager_) {
    statistics_manager_->Increment(counter_type_);
  }
}

AppCounter::AppCounter(StatisticsManager* statistics_manager,
                       const std::string& app_id,
                       AppCounterId counter_type)
    : app_id_(app_id),
      counter_type_(counter_type),
      statistics_manager_(statistics_manager) {
}

void AppCounter::operator++() const {
  if (statistics_manager_) {
    statistics_manager_->Increment(app_id_, counter_type_);
  }
}

AppInfo::AppInfo(StatisticsManager* statistics_manager,
                 const std::string& app_id,
                 AppInfoId info_type)
    : app_id_(app_id),
      info_type_(info_type),
      statistics_manager_(statistics_manager) {
}

void AppInfo::Update(const std::string& new_info) const {
  if (statistics_manager_) {
    statistics_manager_->Set(app_id_, info_type_, new_info);
  }
}

AppStopwatch::AppStopwatch(StatisticsManager* statistics_manager,
                           const std::string& app_id)
    : app_id_(app_id),
      stopwatch_type_(SECONDS_HMI_NONE),
      statistics_manager_(statistics_manager),
      start_time_() {
}

AppStopwatch::~AppStopwatch() {
  if (start_time_) {
    Stop();
  }
}

void AppStopwatch::Start(AppStopwatchId stopwatch_type) {
  assert(0 == start_time_);
  stopwatch_type_ = stopwatch_type;
#ifndef OS_WINCE
  start_time_ = time(NULL);
#else
  start_time_=GetTickCount();
#endif
}

void AppStopwatch::Switch(AppStopwatchId stopwatch_type) {
  Stop();
  Start(stopwatch_type);
}

void AppStopwatch::Stop() {
  assert(start_time_ != 0);
#ifdef OS_WINCE
  double difference = difftime(GetTickCount(),start_time_);
#else
  double difference = difftime(time(NULL), start_time_);
#endif
  if (statistics_manager_) {
    statistics_manager_->Add(app_id_, stopwatch_type_, int32_t(difference));
  }
  start_time_ = 0;
}

}  //  namespace usage_statistics

#endif  //  SRC_COMPONENTS_POLICY_INCLUDE_POLICY_STATISTICS_MANAGER_H_