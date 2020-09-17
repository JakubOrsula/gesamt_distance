//
// Created by krab1k on 17.09.20.
//

#pragma once

#include <algorithm>

std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}
