/* Copyright (c) 2009 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "maidsafe/common/log.h"

#ifdef MAIDSAFE_WIN32
#  include <Windows.h>
#endif
#include <chrono>
#include <ctime>
#include <cstdio>

#include "maidsafe/common/utils.h"

namespace fs = boost::filesystem;


namespace maidsafe {

namespace log {

LogMessage::LogMessage(const std::string &file, int line, const std::string &function, int level)
    : kFile_(file),
      kLine_(line),
      kFunction_(function),
      kLevel_(level),
      stream_(),
      log_entry_() {}

LogMessage::~LogMessage() {
  auto itr(kFile_.end()), begin_itr(kFile_.begin());
  std::string project;
  while (itr != begin_itr) {
    if (*(--itr) == "maidsafe") {
      project = (*(++itr)).string();
      break;
    }
  }

  FilterMap filter(Logging::instance().Filter());
  auto filter_itr(filter.find("*"));
  if (filter_itr == filter.end()) {
    filter_itr = filter.find(project);
  }
  if (filter_itr == filter.end())
    return;

  if ((*filter_itr).second > kLevel_)
    return;

  fs::path current_file;
  for (; itr != kFile_.end(); ++itr)
    current_file /= *itr;

  char log_level;
  unsigned short console_colour(0);  // NOLINT (Fraser)
  switch (kLevel_) {
    case kInfo:
      log_level = 'I';
      console_colour = 7U;
      break;
    case kWarning:
      log_level = 'W';
      console_colour = 14U;
      break;
    case kError:
      log_level = 'E';
      console_colour = 12U;
      break;
    case kFatal:
      log_level = 'F';
      console_colour = 12U;
      break;
    default:
      log_level = ' ';
  }

  std::ostringstream oss;
  oss << log_level << " " << std::this_thread::get_id();
#if defined(WIN32)
  oss << '\t';
#endif
//  std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//  std::string time = std::ctime(&t);
//  time.resize(time.size() - 5);
//  std::string ftime = time.substr(12,8);
//  oss << " " << ftime << " ";
  oss << boost::posix_time::microsec_clock().universal_time().time_of_day() << " ";
  oss << current_file.string();
  oss << ":" << kLine_ << "] ";
//  oss << " Function: " << function_ << "] ";

  oss << stream_.str();
  std::string log_entry(oss.str());
  bool colour(Logging::instance().Colour());

  Logging::instance().Send([log_entry, colour, console_colour] {
    if (colour) {
#ifdef MAIDSAFE_WIN32
      CONSOLE_SCREEN_BUFFER_INFO console_info_before;
      HANDLE console_handle(GetStdHandle(STD_OUTPUT_HANDLE));
      if (console_handle != INVALID_HANDLE_VALUE) {
        int got_console_info = GetConsoleScreenBufferInfo(console_handle, &console_info_before);
        SetConsoleTextAttribute(console_handle, console_colour);
        printf("%s\n", log_entry.c_str());
        if (got_console_info != 0)
          SetConsoleTextAttribute(console_handle, console_info_before.wAttributes);
      } else {
        printf("%s\n", log_entry.c_str());
      }
#endif
    } else {
      printf("%s\n", log_entry.c_str());
    }
  });  // message saved
}

Logging::Logging()
    : background_(maidsafe::Active::createActive()),
      log_level_(kFatal),
      filter_(),
      colour_(true) {}

void Logging::Send(functor voidfunction) {
  background_->Send(voidfunction);
}


}  // namespace log
}  // namespace maidsafe
