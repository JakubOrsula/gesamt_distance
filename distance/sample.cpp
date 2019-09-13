//
// Created by krab1k on 12.09.19.
//

#include <filesystem>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <tuple>
#include <sstream>
#include <iomanip>
#include <omp.h>
#include <iterator>

#include "gesamtlib/gsmt_structure.h"
#include "gesamtlib/gsmt_aligner.h"

using std::string;  using std::vector;   using std::tuple;
using std::cout;    using std::cerr;    using std::endl;
using std::ifstream;
using std::stringstream;
using std::transform;
using std::setw;
using std::fixed;
using std::setprecision;
using std::make_tuple;

namespace fs = std::filesystem;


string to_lower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


void split(const string &str, vector<string> &cont)
{
    auto pos = str.find(' ');
    cont.push_back(str.substr(0, 4));
    cont.push_back(str.substr(5, pos - 5));
    cont.push_back(str.substr(pos + 1, 4));
    cont.push_back(str.substr(pos + 6));
}


void calculate_similarity(gsmt::Structure *s1, gsmt::Structure *s2, double threshold, double &Q, double &time) {
    auto Aligner = new gsmt::Aligner();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    gsmt::PSuperposition SD;
    int matchNo;

    Aligner->Align(s1, s2, true, &time);
    Aligner->getBestMatch(SD, matchNo);
    Q = SD ? SD->Q : -1;

    delete Aligner;
}


vector<tuple<string, string, string, string>> load_names(const string &filename) {
    vector<tuple<string, string, string, string>> data;

    ifstream f(filename);

    string line;
    while (std::getline(f, line)) {
        vector<string> ids;
        split(line, ids);
        data.emplace_back(ids[0], ids[1], ids[2], ids[3]);
    }

    return data;
}


void load_structure(gsmt::Structure *s, const string &directory, const string &pdb_id, const string &chain_id) {
    auto path = fs::path(directory) / (to_lower(pdb_id) + std::string("_updated.cif"));
    s->getStructure(path.c_str(), chain_id.c_str(), -1, false);
    s->prepareStructure(0.0);
}


int main(int argc, char **argv) {


    if (argc <= 3) {
        cerr << "Not enough arguments." << endl;
        return 1;
    }

    string directory(argv[1]);
    string file = argv[2];

    vector<double> thresholds;
    for (int i = 3; i < argc; i++) {
        thresholds.push_back(atof(argv[i]));
    }

    auto data = load_names(file);

    vector<tuple<string, string, int, int, vector<double>, vector<double>>> results;

    results.resize(data.size());

#pragma omp parallel for default(none) shared(data, directory, thresholds, results)
    for (int i = 0; i < data.size(); i++) {
        const auto &[pdb_id1, chain_id1, pdb_id2, chain_id2] = data[i];

        auto s1 = new gsmt::Structure();
        auto s2 = new gsmt::Structure();

        load_structure(s1, directory, pdb_id1, chain_id1);
        load_structure(s2, directory, pdb_id2, chain_id2);

        stringstream ss;
        ss << pdb_id1 << ":" << chain_id1;
        auto id1 = ss.str();
        ss.str(string());
        ss.clear();
        ss << pdb_id2 << ":" << chain_id2;
        auto id2 = ss.str();

        auto size1 = s1->getNCalphas();
        auto size2 = s2->getNCalphas();

        vector<double> Qs;
        vector<double> times;

        for (auto threshold: thresholds) {
            double Q;
            double time;
            calculate_similarity(s1, s2, threshold, Q, time);
            Qs.push_back(Q);
            times.push_back(time);
        }

        results[i] = make_tuple(id1, id2, size1, size2, Qs, times);

        delete s1;
        delete s2;
    }

    for(const auto &[id1, id2, size1, size2, Qs, times]: results) {
        cout << setw(7) << id1 << " " << setw(7) << id2 << " " << setw(4) << size1 << " " << setw(4) << size2 << " ";
        for (const auto Q: Qs) {
            cout << fixed << setprecision(3) << Q << " ";
        }
        for (const auto time: times) {
            cout << fixed << setprecision(4) << time << " ";
        }
        cout << endl;
    }

    return 0;
}
