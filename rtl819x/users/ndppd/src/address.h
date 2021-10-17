// ndppd - NDP Proxy Daemon
// Copyright (C) 2011  Daniel Adolfsson <daniel@priv.nu>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#pragma once

#include <string>
#include <netinet/ip6.h>

#include "ndppd.h"

NDPPD_NS_BEGIN

class iface;

class address {
public:
    address();
    address(const address& addr);
    address(const std::string& str);
    address(const char* str);
    address(const in6_addr& addr);
    address(const in6_addr& addr, const in6_addr& mask);
    address(const in6_addr& addr, int prefix);

    struct in6_addr& addr();

    const struct in6_addr& const_addr() const;

    struct in6_addr& mask();

    // Compare _a/_m against a._a.
    bool operator==(const address& addr) const;

    bool operator!=(const address& addr) const;

    void reset();

    const std::string to_string() const;

    bool parse_string(const std::string& str);

    int prefix() const;

    void prefix(int n);

    bool is_unicast() const;

    bool is_multicast() const;

    operator std::string() const;

private:
    struct in6_addr _addr, _mask;
};

NDPPD_NS_END
