/*
 * Copyright (C) 2018 Codership Oy <info@codership.com>
 *
 * This file is part of wsrep-lib.
 *
 * Wsrep-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Wsrep-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wsrep-lib.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "wsrep/id.hpp"
#include "uuid.hpp"

#include <cctype>
#include <sstream>
#include <algorithm>

const wsrep::id wsrep::id::undefined_ = wsrep::id();

wsrep::id::id(const std::string& str)
    :  data_()
{
    wsrep::uuid_t wsrep_uuid;

    if (str.size() == WSREP_LIB_UUID_STR_LEN &&
        wsrep::uuid_scan(str.c_str(), str.size(), &wsrep_uuid) ==
        WSREP_LIB_UUID_STR_LEN)
    {
        std::memcpy(data_.buf, wsrep_uuid.data, sizeof(data_.buf));
    }
    else if (str.size() <= 16)
    {
        std::memcpy(data_.buf, str.c_str(), str.size());
    }
    else
    {
        std::ostringstream os;
        os << "String '" << str
           << "' does not contain UUID or is longer thatn 16 bytes";
        throw wsrep::runtime_error(os.str());
    }
}

std::string wsrep::id::to_string() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}


/*
 * If the buffer pointed by ptr contains only alphanumeric chars followed by
 * one or more null terminators, consider it a valid alphanumeric string.
 *
 * A string starting with null is not an alphanumeric string.
 */
static bool is_alphanumeric_string(const char* ptr, size_t size)
{
    if (size == 0)
        return false;
    if (ptr[0] == '\0')
        return false;
    const char* first_not_alphanumeric = std::find_if_not(
        ptr, ptr + size, [](char c) { return ::isalnum(c); });
    if (static_cast<size_t>(std::distance(ptr, first_not_alphanumeric)) == size)
        return false;
    return std::all_of(first_not_alphanumeric, ptr + size,
                       [](char c) { return (c == '\0'); });
}

std::ostream& wsrep::operator<<(std::ostream& os, const wsrep::id& id)
{
    const char* ptr(static_cast<const char*>(id.data()));
    size_t size(id.size());
    /* If the buffer pointed by ptr contains only alphanumeric chars followed by
     * one or more null terminators, return the string. */
    if (is_alphanumeric_string(ptr, size))
    {
        return (os << std::string(ptr, ::strnlen(ptr, size)));
    }
    else
    {
        char uuid_str[WSREP_LIB_UUID_STR_LEN + 1];
        wsrep::uuid_t uuid;
        std::memcpy(uuid.data, ptr, sizeof(uuid.data));
        if (wsrep::uuid_print(&uuid, uuid_str, sizeof(uuid_str)) < 0)
        {
            throw wsrep::runtime_error("Could not print uuid");
        }
        uuid_str[WSREP_LIB_UUID_STR_LEN] = '\0';
        return (os << uuid_str);
    }
}

std::istream& wsrep::operator>>(std::istream& is, wsrep::id& id)
{
    std::string id_str;
    std::getline(is, id_str);
    id = wsrep::id(id_str);
    return is;
}
