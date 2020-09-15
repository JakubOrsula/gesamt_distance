//
// Created by krab1k on 21.01.20.
//

#pragma once

#include <string>
#include <memory>

#include "gesamtlib/gsmt_aligner.h"
#undef DefineClass // JNI conflicts with GESAMT macro, so undef it

enum status {
    RESULT_OK,
    RESULT_DISSIMILAR,
    RESULT_TIMEOUT
};

std::shared_ptr<gsmt::Structure> load_single_structure(const std::string &id, const std::string &directory, bool binary);


void init_library(const std::string &archive_directory, const std::string &preload_list_filename, bool binary_archive,
                  double approximation_threshold, int cache_size);

void close_library();

enum status run_computation(const std::string &id1, const std::string &id2, float time_threshold, std::unique_ptr<gsmt::Superposition> &SD);

float get_distance(const std::string& id1, const std::string &id2, float time_threshold);

std::shared_ptr<gsmt::Structure> get_structure(const std::string &id);
