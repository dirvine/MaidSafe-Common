/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#ifndef MAIDSAFE_COMMON_ERROR_H_
#define MAIDSAFE_COMMON_ERROR_H_

#ifdef MAIDSAFE_WIN32
#define MAIDSAFE_NOEXCEPT
#else
#define MAIDSAFE_NOEXCEPT noexcept(true)
#endif

#include <system_error>
#include <string>

namespace maidsafe {

enum class maidsafe_error_code {
    bad_string_length = 1,  // 0 would be no error
    no_private_key,
    no_public_key,
    decrypt_failed,
    encrypt_failed,
    // network errors
    no_bootstrap_nodes_found,
    node_not_found,
    listen_failed,
    accept_failed,
    no_transports,
    transports_full,
    no_available_ports,
    address_in_use,
    invalid_message,
    // drive errors
    drive_not_mounted,
    cannot_mount_drive,
    no_filesystem_driver,
    // storage errors
    save_failed,
    get_failed,
    modify_failed,
    delete_failed
};

enum class maidsafe_error_condition {
    common,
    rudp,
    routing,
    drive,
    unknown_error
};

  std::error_code make_error_code(maidsafe_error_code e);
  std::error_condition make_error_condition(maidsafe_error_condition e);

class error_category_impl : public std::error_category {
 public:
    virtual std::string message(int ev) const;
    virtual const char* name() const MAIDSAFE_NOEXCEPT;
    virtual std::error_condition default_error_condition(int ev) const MAIDSAFE_NOEXCEPT;
    // virtual bool equivalent(const std::error_code& code, int condition) const noexcept(true);
};

const std::error_category& error_category();
}  // maidsafe

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
#endif
namespace std {
    template <>
    struct is_error_code_enum<maidsafe::maidsafe_error_code>
      : public true_type {};  //NOLINT (dirvine)

    template <>
    struct is_error_condition_enum<maidsafe::maidsafe_error_condition>
      : public true_type {};  //NOLINT (dirvine)
}

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#endif  // MAIDSAFE_COMMON_ERROR_H_

