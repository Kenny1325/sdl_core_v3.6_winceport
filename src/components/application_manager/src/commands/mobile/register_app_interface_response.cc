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

#ifdef MODIFY_FUNCTION_SIGN
#include <global_first.h>
#endif
#include "application_manager/commands/mobile/register_app_interface_response.h"
#include "interfaces/MOBILE_API.h"
#include "application_manager/application_manager_impl.h"
#include "application_manager/application_impl.h"
#include "application_manager/message_helper.h"

namespace application_manager {

namespace commands {

void RegisterAppInterfaceResponse::Run() {
  LOG4CXX_INFO(logger_, "RegisterAppInterfaceResponse::Run");

  bool success = (*message_)[strings::msg_params][strings::success].asBool();

  bool last_message = !success;
  // Do not close connection in case of APPLICATION_NOT_REGISTERED despite it is an error
  if (!success && (*message_)[strings::msg_params].keyExists(strings::result_code)) {
    mobile_apis::Result::eType result_code =
        mobile_apis::Result::eType((*message_)[strings::msg_params][strings::result_code].asInt());
    if (result_code ==  mobile_apis::Result::APPLICATION_REGISTERED_ALREADY
#ifdef MODIFY_FUNCTION_SIGN
					|| result_code == mobile_api::Result::DUPLICATE_NAME
#endif
		) {
      last_message = false;
    }
  }


  bool close_session = false;
  if (last_message) {
    if (1 < ApplicationManagerImpl::instance()->connection_handler()->GetConnectionSessionsCount(
        (*message_)[strings::params][strings::connection_key].asUInt())) {
      last_message = false;
      close_session = true;
    }
  }

  SendResponse(success, mobile_apis::Result::INVALID_ENUM, last_message);

  if (close_session) {
    ApplicationManagerImpl::instance()->connection_handler()->CloseSession(
        (*message_)[strings::params][strings::connection_key].asUInt());
  }

  if (success) {
    ApplicationSharedPtr application =
        ApplicationManagerImpl::instance()->application(
            (*message_)[strings::params][strings::connection_key].asInt());
    MessageHelper::SendChangeRegistrationRequestToHMI(application);
  }
}

}  // namespace commands
}  // namespace application_manager
