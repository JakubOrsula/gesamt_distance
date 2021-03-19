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


static const int MIN_CHAIN_LENGTH = 10;


std::vector<std::tuple<std::string, int>>
save_chains(const std::string &input_file, const std::string &output_dir, const std::string &output_name) {
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

        if (n_atoms >= MIN_CHAIN_LENGTH) {
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


int prepare_PDB(const std::string &id, const std::string &archive_dir, const std::string &output_dir,
                const std::optional<py::list> &matrix) {
    auto s = load_single_structure(id, archive_dir, false);
    auto M = s->getSelectedStructure(mmdb::STYPE_CHAIN);

    std::string output_file;
    if (matrix.has_value()) {
        double T[4][4];
        for (int i = 0; i < 16; i++) {
            auto val = matrix.value()[i].cast<double>();
            T[i / 4][i % 4] = val;
        }
        M->ApplyTransform(T);
        output_file = output_dir + "/" + id + ".aligned.pdb";
    } else {
        output_file = output_dir + "/query.pdb";
    }

    M->WritePDBASCII(output_file.c_str());
    delete M;
    return 0;
}


std::tuple<enum status, float, float, float, int, std::vector<float>>
get_results(const std::string &id1, const std::string &id2, const std::string &archive_dir, double min_qscore) {

    auto s1 = load_single_structure(id1, archive_dir, true);
    auto s2 = load_single_structure(id2, archive_dir, true);

    auto size1 = s1->getNCalphas();
    auto size2 = s2->getNCalphas();

    if (size2 < min_qscore * size1 or size1 < min_qscore * size2) {
        return std::make_tuple(RESULT_DISSIMILAR, 0, 0, 0, 0, std::vector<float>());
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
        return std::make_tuple(RESULT_DISSIMILAR, 0, 0, 0, 0, std::vector<float>());
    }

    std::vector<float> T;
    for (auto &i : SD->T) {
        for (double j: i) {
            T.push_back(j);
        }
    }

    return std::make_tuple(RESULT_OK, SD->Q, SD->rmsd, SD->seqId, SD->Nalgn, T);
}


PYBIND11_MODULE(python_distance, m) {
    m.doc() = "Python binding to GESAMT library";

    py::enum_<status>(m, "Status")
            .value("OK", status::RESULT_OK)
            .value("DISSIMILAR", status::RESULT_DISSIMILAR);

    m.def("save_chains", &save_chains, "input_file"_a, "output_dir"_a, "output_name"_a,
          "Save chains from query protein");
    m.def("get_results", &get_results, "id1"_a, "id2"_a, "archive_dir"_a, "min_qscore"_a,
          "Get the Q-score and other metrics between two objects");
    m.def("prepare_PDB", &prepare_PDB, "id"_a, "archive_dir"_a, "output_dir"_a, "matrix"_a, "Prepare PDB file");
}
