#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <string>
#include <iomanip>

namespace fs = std::filesystem;
using namespace std;
using namespace chrono;

struct Item {
    int value;
    int weight;
    double ratio;

    Item(int v, int w) : value(v), weight(w) {
        ratio = (weight > 0) ? static_cast<double>(value)/weight : 0.0;
    }
};

struct Solution {
    int total_value;
    int total_weight;
    vector<bool> selected;
};

struct Result {
    string filename;
    int max_value;
    int total_weight;
    int num_selected;
    double exec_time;
};

// Compare items by value/weight ratio in descending order
bool compareItems(const Item& a, const Item& b) {
    return a.ratio > b.ratio;
}

Solution greedyKnapsack(int capacity, vector<Item>& items) {
    sort(items.begin(), items.end(), compareItems);

    vector<bool> selected(items.size(), false);
    int current_weight = 0;
    int total_value = 0;

    for(size_t i = 0; i < items.size(); ++i) {
        if(current_weight + items[i].weight <= capacity) {
            selected[i] = true;
            current_weight += items[i].weight;
            total_value += items[i].value;
        }
    }

    return {total_value, current_weight, selected};
}

vector<Item> readItems(const string& filename, int& capacity) {
    ifstream file(filename);
    vector<Item> items;

    if (!file.is_open()) return items;

    int n;
    file >> n >> capacity;

    for(int i = 0; i < n; ++i) {
        int v, w;
        file >> v >> w;
        items.emplace_back(v, w);
    }

    return items;
}

Result processFile(const string& filename) {
    auto start = high_resolution_clock::now();
    Result result;
    result.filename = filename;

    int capacity;
    vector<Item> items = readItems(filename, capacity);

    if(items.empty()) {
        result.max_value = 0;
        result.total_weight = 0;
        result.num_selected = 0;
        result.exec_time = 0.0;
        return result;
    }

    Solution solution = greedyKnapsack(capacity, items);
    auto end = high_resolution_clock::now();

    result.max_value = solution.total_value;
    result.total_weight = solution.total_weight;
    result.num_selected = count(solution.selected.begin(),
                               solution.selected.end(), true);
    result.exec_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

    return result;
}

void saveCSV(const vector<Result>& results, const string& filename) {
    ofstream file(filename);
    file << "Filename,Max Value,Total Weight,Items Selected,Execution Time (s)\n";

    for(const auto& res : results) {
        file << res.filename << ","
             << res.max_value << ","
             << res.total_weight << ","
             << res.num_selected << ","
             << fixed << setprecision(3) << res.exec_time << "\n";
    }
}

void printSummary(const vector<Result>& results) {
    if(results.empty()) {
        cout << "No results to display\n";
        return;
    }

    double total_time = 0.0;
    int total_value = 0;

    for(const auto& res : results) {
        cout << res.filename << "\t"
             << res.max_value << "\t"
             << res.total_weight << "\t"
             << res.num_selected << "\t"
             << fixed << setprecision(3) << res.exec_time << endl;

        total_time += res.exec_time;
        total_value += res.max_value;
    }

    cout
         << "Average time: " << total_time / results.size() << "s\n"
         << "Total value: " << total_value << "\n"
         << "Processed files: " << results.size() << endl;
}

int main() {
    vector<Result> results;
    string data_dir = "/Users/phonkyponky/CLionProjects/untitled/data2";

    try {
        for(const auto& entry : fs::directory_iterator(data_dir)) {
            if(entry.is_regular_file()) {
                Result res = processFile(entry.path().string());
                results.push_back(res);
            }
        }
    } catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    sort(results.begin(), results.end(),
        [](const Result& a, const Result& b) { return a.filename < b.filename; });

    printSummary(results);
    saveCSV(results, "greedy_results.csv");

    return 0;
}
