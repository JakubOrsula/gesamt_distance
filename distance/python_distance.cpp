#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>
#include <vector>
#include <tuple>

#include "gesamtlib/gsmt_utils.h"
#include "protein_distance.h"

namespace py = pybind11;
using namespace pybind11::literals;


std::vector<std::string> save_chains(const std::string &input_file, const std::string &output_dir) {

    gsmt::Structure s;
    auto ret = s.getStructure(input_file.c_str(), "", -1, false);
    if (ret) {
        std::stringstream ss;
        ss << "Cannot open file: " << input_file;
        throw std::runtime_error(ss.str());
    }
    auto M = s.getMMDBManager();
    auto model = M->GetFirstDefinedModel();
    mmdb::PPChain chain;
    int nc;

    model->GetChainTable (chain,nc);

    std::vector<std::string> ids;
    for (int i  = 0; i < nc; i++) {
        auto chainid = chain[i]->GetChainID();
        if (chainid[0] != '\0') {
            ids.emplace_back(chainid);
            auto M1 = new mmdb::Manager();

            M1->Copy(M, mmdb::COPY_MASK::MMDBFCM_All);
            gsmt::Structure s2;
            s2.getStructure(M1, chainid, -1, false);
            mmdb::io::File file;
            std::stringstream ss;
            ss << output_dir << "/" << "query" << ":" << chainid << ".bin";
            file.assign(ss.str().c_str());
            file.rewrite();
            s2.write(file);
            file.shut();
        }
    }

    return ids;
}


std::tuple<enum status, float, float, float, int> computation_details(const std::string &id1, const std::string &id2, float time_threshold, const std::string &output_dir) {
    auto SD = std::make_unique<gsmt::Superposition>();

    auto status = run_computation(id1, id2, time_threshold, SD);

    if (status != RESULT_OK) {
        return std::make_tuple(status, 0, 0, 0, 0);
    }

    auto s1 = get_structure(id1);
    auto s2 = get_structure(id2);

    auto M1 = s1->getSelectedStructure (mmdb::STYPE_CHAIN);
    auto M2 = s2->getSelectedStructure (mmdb::STYPE_CHAIN);
    M2->ApplyTransform(SD->T);

    std::stringstream ss;
    ss << output_dir << "/" << "query.pdb";

    M1->WritePDBASCII(ss.str().c_str());

    ss.str("");
    ss.clear();
    ss << output_dir << "/" << id2 << ".aligned.pdb";

    M2->WritePDBASCII(ss.str().c_str());

    delete M1;
    delete M2;

    return std::make_tuple(status, SD->Q, SD->rmsd, SD->seqId, SD->Nalgn);
}


PYBIND11_MODULE(python_distance, m) {
    m.doc() = "Python binding to GESAMT library";

    py::enum_<status>(m, "Status")
            .value("OK", status::RESULT_OK)
            .value("DISSIMILAR", status::RESULT_DISSIMILAR)
            .value("TIMEOUT", status::RESULT_TIMEOUT);

    m.def("save_chains", &save_chains, "input_file"_a, "output_dir"_a, "Save chains from query protein");
    m.def("init_library", &init_library, "archive_directory"_a, "preload_list"_a, "binary_archive"_a,
          "approximation_threshold"_a, "cache_size"_a, "Initialize the library");
    m.def("get_distance", &get_distance, "id1"_a, "id2"_a, "time_threshold"_a, "Compute distance between two objects");
    m.def("computation_details", &computation_details, "id1"_a, "id2"_a, "time_threshold"_a, "output_dir"_a, "Get details about distance computation");

}
