#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include <vector>
#include <tuple>
#include <filesystem>

#include "gesamtlib/gsmt_aligner.h"
#include "protein_distance.h"

namespace py = pybind11;
namespace fs = std::filesystem;
using namespace pybind11::literals;


std::vector<std::tuple<std::string, int>> save_chains(const std::string &input_file, const std::string &output_dir, const std::string &output_name) {
    gsmt::Structure structure;
    std::vector<std::tuple<std::string, int>> ids;
    int chain_no = 0;

    while (true) {
        auto rc = structure.getStructure(input_file.c_str(), nullptr, chain_no, false);
        if (rc) {
            break;
        }
        mmdb::PPAtom atom;
        int n_atoms;
        structure.getCalphas(atom, n_atoms);

        if (n_atoms >= seg_length_default) {
            std::string chain_id = atom[0]->GetChainID();
            ids.emplace_back(chain_id, n_atoms);

            mmdb::io::File file;
            std::string path = std::string(output_dir) + "/" + output_name + ":" + chain_id + ".bin";
            file.assign(path.c_str());
            file.rewrite();
            structure.write(file);
            file.shut();
        }

        chain_no++;
    }
    return ids;
}


int prepare_aligned_PDBs(const std::string &id1, const std::string &id2, const std::string &archive_dir,
                         const std::string &output_dir) {
    auto s1 = load_single_structure(id1, archive_dir, false);
    auto s2 = load_single_structure(id2, archive_dir, false);

    const double threshold = 0.0;

    auto Aligner = std::make_unique<gsmt::Aligner>();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    int matchNo;
    gsmt::PSuperposition SD;
    Aligner->Align(s1.get(), s2.get(), false);
    Aligner->getBestMatch(SD, matchNo);

    if (not SD) {
        return 1;
    }

    auto M1 = s1->getSelectedStructure(mmdb::STYPE_CHAIN);
    auto M2 = s2->getSelectedStructure(mmdb::STYPE_CHAIN);
    M2->ApplyTransform(SD->T);

    std::string query_pdb = output_dir + "/query.pdf";
    M1->WritePDBASCII(query_pdb.c_str());

    std::string other_pdb = output_dir + "/" + id2 + ".aligned.pdb";
    M2->WritePDBASCII(other_pdb.c_str());

    delete M1;
    delete M2;

    return 0;
}


std::tuple<enum status, float, float, float, int>
get_results(const std::string &id1, const std::string &id2, const std::string &archive_dir, double size_threshold) {

    auto s1 = load_single_structure(id1, archive_dir, true);
    auto s2 = load_single_structure(id2, archive_dir, true);

    auto size1 = s1->getNCalphas();
    auto size2 = s2->getNCalphas();

    if (size2 < size_threshold * size1 or size1 < size_threshold * size2) {
        return std::make_tuple(RESULT_DISSIMILAR, 0, 0, 0, 0);
    }

    auto Aligner = std::make_unique<gsmt::Aligner>();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(0.0, 0.0);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    int matchNo;
    gsmt::PSuperposition SD;
    Aligner->Align(s1.get(), s2.get(), false);
    Aligner->getBestMatch(SD, matchNo);

    if (not SD) {
        return std::make_tuple(RESULT_DISSIMILAR, 0, 0, 0, 0);
    }

    return std::make_tuple(RESULT_OK, SD->Q, SD->rmsd, SD->seqId, SD->Nalgn);
}


PYBIND11_MODULE(python_distance, m) {
    m.doc() = "Python binding to GESAMT library";

    py::enum_<status>(m, "Status")
            .value("OK", status::RESULT_OK)
            .value("DISSIMILAR", status::RESULT_DISSIMILAR)
            .value("TIMEOUT", status::RESULT_TIMEOUT);

    m.def("save_chains", &save_chains, "input_file"_a, "output_dir"_a, "output_name"_a, "Save chains from query protein");
    m.def("init_library", &init_library, "archive_directory"_a, "preload_list"_a, "binary_archive"_a,
          "approximation_threshold"_a, "cache_size"_a, "Initialize the library");
    m.def("get_distance", &get_distance, "id1"_a, "id2"_a, "time_threshold"_a, "Compute distance between two objects");
    m.def("get_results", &get_results, "id1"_a, "id2"_a, "archive_dir"_a, "size_threshold"_a, "Get the Qscore and other metrics between two objects");
    m.def("prepare_aligned_PDBs", &prepare_aligned_PDBs, "id1"_a, "id2"_a, "archive_dir"_a, "output_dir"_a, "Prepare aligned PDBs to display");
}
