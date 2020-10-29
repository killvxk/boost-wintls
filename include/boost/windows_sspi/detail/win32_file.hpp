//
// windows_sspi/detail/win32_file.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Kasper Laudrup (laudrup at stacktrace dot dk)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_WINDOWS_SSPI_DETAIL_WIN32_FILE_HPP
#define BOOST_WINDOWS_SSPI_DETAIL_WIN32_FILE_HPP

#include <boost/winapi/file_management.hpp>
#include <boost/winapi/access_rights.hpp>
#include <boost/winapi/handles.hpp>

#include <string>
#include <vector>

namespace boost {
namespace windows_sspi {
namespace detail {

inline std::vector<char> read_file(const std::string& filename) {
  using namespace boost::system;
  using namespace boost::winapi;
  using file_handle_type = std::unique_ptr<std::remove_pointer<HANDLE_>::type, decltype(&CloseHandle)>;

  // TODO: Support unicode filenames. The proper way to do this is to
  // use boost::filesystem or std::filesystem paths instead of
  // strings, but that would break boost::asio compatibility
  file_handle_type handle{CreateFile(filename.c_str(),
                                     GENERIC_READ_,
                                     FILE_SHARE_READ_,
                                     nullptr,
                                     OPEN_EXISTING_,
                                     FILE_ATTRIBUTE_NORMAL_,
                                     nullptr), CloseHandle};
  if (handle.get() == INVALID_HANDLE_VALUE_) {
    throw system_error(GetLastError(), system_category());
  }

  LARGE_INTEGER_ size;
  if (!GetFileSizeEx(handle.get(), &size)) {
    throw system_error(GetLastError(), system_category());
  }

  std::vector<char> buffer(static_cast<std::size_t>(size.QuadPart));
  DWORD_ read;
  if (!ReadFile(handle.get(), buffer.data(), static_cast<DWORD_>(buffer.size()), &read, nullptr)) {
    throw system_error(GetLastError(), system_category());
  }
  return buffer;
}

} // namespace detail
} // namespace windows_sspi
} // namespace boost

#endif
