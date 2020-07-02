//
// Created by krab1k on 21.01.20.
//

#pragma once

#include <string>


void init_library(const std::string &archive_directory, const std::string &preload_list_filename, bool binary_archive,
                  double approximation_threshold);


float get_distance(const std::string& id1, const std::string &id2, float time_threshold);
