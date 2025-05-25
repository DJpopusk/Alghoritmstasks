#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
using namespace std;

struct Item {
    int value;
    int weight;
    double ratio; 

    Item(int v, int w) : value(v), weight(w) {
        ratio = (w > 0) ? (double)v / w : 0.0;
    }

    Item() : value(0), weight(0), ratio(0.0) {}
};

struct Element {
    int price, weight;
    double heuristics;

    Element(int p, int w) : price(p), weight(w) {
        heuristics = (w > 0) ? (double)p / w : 0.0;
    }

    Element() : price(0), weight(0), heuristics(0.0) {}
};

class Node {
public:
    int ind, price, weight;
    double profit;
    vector<bool> solution; // для отслеживания выбранных предметов

    Node(int i, int pric, int w, double prof) {
        ind = i;
        price = pric;
        weight = w;
        profit = prof;
    }

    Node() : ind(-1), price(0), weight(0), profit(0.0) {}
};

struct Result {
    string filename;
    int maxValue;
    int totalWeight;
    int numItems;
    double executionTime;
    vector<bool> solution;
};

// Функция для определения прибыльности пути от этой вершины
double calculateProfit(int ind, int price, int weight, int N, int W, const vector<Element> &items) {
    if (weight >= W) return 0.0;

    double total_profit = price;
    int i = ind + 1;
    int total_weight = weight;

    // Жадно добавляем предметы, пока можем
    while (i < N && total_weight + items[i].weight <= W) {
        total_profit += items[i].price;
        total_weight += items[i].weight;
        i++;
    }

    // Добавляем дробную часть последнего предмета для оценки верхней границы
    if (i < N) {
        double remaining_capacity = W - total_weight;
        total_profit += remaining_capacity * items[i].heuristics;
    }

    return total_profit;
}

vector<int> branchAndBound(int N, int W, vector<Element> &items, vector<bool> &bestSolution) {
    // Сортируем предметы по убыванию отношения цена/вес
    auto cmp = [](const Element &a, const Element &b) {
        return a.heuristics > b.heuristics;
    };
    sort(items.begin(), items.end(), cmp);

    // Очередь с приоритетом для узлов
    auto cmp_bound = [](const Node &a, const Node &b) {
        return a.profit < b.profit;
    };
    priority_queue<Node, vector<Node>, decltype(cmp_bound)> PQ(cmp_bound);

    // Начальный узел
    Node root(-1, 0, 0, calculateProfit(-1, 0, 0, N, W, items));
    root.solution.resize(N, false);
    PQ.push(root);

    int maxProfit = 0;
    int bestWeight = 0;
    bestSolution.resize(N, false);

    while (!PQ.empty()) {
        Node current = PQ.top();
        PQ.pop();

        // Если это листовой узел или оценка не лучше текущего максимума
        if (current.ind == N - 1 || current.profit <= maxProfit) {
            continue;
        }

        int nextInd = current.ind + 1;

        // Ветвь 1: включаем следующий предмет
        if (current.weight + items[nextInd].weight <= W) {
            Node include(nextInd,
                        current.price + items[nextInd].price,
                        current.weight + items[nextInd].weight,
                        0.0);
            include.solution = current.solution;
            include.solution[nextInd] = true;
            include.profit = calculateProfit(include.ind, include.price, include.weight, N, W, items);

            if (include.price > maxProfit) {
                maxProfit = include.price;
                bestWeight = include.weight;
                bestSolution = include.solution;
            }

            if (include.profit > maxProfit) {
                PQ.push(include);
            }
        }

        // Ветвь 2: не включаем следующий предмет
        Node exclude(nextInd, current.price, current.weight, 0.0);
        exclude.solution = current.solution;
        exclude.solution[nextInd] = false;
        exclude.profit = calculateProfit(exclude.ind, exclude.price, exclude.weight, N, W, items);

        if (exclude.profit > maxProfit) {
            PQ.push(exclude);
        }
    }

    return {maxProfit, bestWeight};
}

vector<Item> readKnapsackFile(const string& filename, int& capacity) {
    ifstream file(filename);
    vector<Item> items;

    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return items;
    }

    int numItems;
    file >> numItems >> capacity;

    items.reserve(numItems);
    for (int i = 0; i < numItems; i++) {
        int value, weight;
        file >> value >> weight;
        items.emplace_back(value, weight);
    }

    file.close();
    return items;
}

Result solveBranchAndBound(const string& filename) {
    Result result;
    result.filename = filename;

    auto start = chrono::high_resolution_clock::now();

    int capacity;
    vector<Item> items = readKnapsackFile(filename, capacity);

    if (items.empty()) {
        result.maxValue = 0;
        result.totalWeight = 0;
        result.numItems = 0;
        result.executionTime = 0.0;
        return result;
    }

    // Конвертируем Item в Element для совместимости с алгоритмом
    vector<Element> elements;
    elements.reserve(items.size());
    for (const auto& item : items) {
        elements.emplace_back(item.value, item.weight);
    }

    vector<bool> bestSolution;
    vector<int> solution = branchAndBound(items.size(), capacity, elements, bestSolution);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    result.maxValue = solution[0];
    result.totalWeight = solution[1];
    result.numItems = count(bestSolution.begin(), bestSolution.end(), true);
    result.executionTime = duration.count() / 1000.0;
    result.solution = bestSolution;

    return result;
}

void saveResultsToCSV(const vector<Result>& results, const string& filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Cannot create CSV file " << filename << endl;
        return;
    }

    // Заголовок CSV
    file << "Filename,Max Value,Total Weight,Num Items,Execution Time (s)\n";

    // Данные
    for (const auto& result : results) {
        file << result.filename << ","
             << result.maxValue << ","
             << result.totalWeight << ","
             << result.numItems << ","
             << fixed << setprecision(3) << result.executionTime << "\n";
    }

    file.close();
    cout << "Results saved to " << filename << endl;
}

void printStatistics(const vector<Result>& results) {
    if (results.empty()) {
        cout << "No results to display." << endl;
        return;
    }

    double totalTime = 0.0;
    int totalValue = 0;

    cout << "\n=== Branch and Bound Results ===" << endl;
    cout << "Filename\t\tMax Value\tWeight\t\tItems\t\tTime(s)" << endl;
    cout << "--------------------------------------------------------------------" << endl;

    for (const auto& result : results) {
        cout << result.filename << "\t\t"
             << result.maxValue << "\t\t"
             << result.totalWeight << "\t\t"
             << result.numItems << "\t\t"
             << fixed << setprecision(3) << result.executionTime << endl;

        totalTime += result.executionTime;
        totalValue += result.maxValue;
    }

    cout << "--------------------------------------------------------------------" << endl;
    cout << "Average execution time: " << fixed << setprecision(3)
         << totalTime / results.size() << " seconds" << endl;
    cout << "Total value across all instances: " << totalValue << endl;
    cout << "Number of instances solved: " << results.size() << endl;
}

int main() {
    string directory = "/Users/phonkyponky/CLionProjects/untitled/data2";

    if (!fs::exists(directory)) {
        cerr << "Error: Directory '" << directory << "' does not exist." << endl;
        cerr << "Please create the directory and place knapsack instance files there." << endl;
        return 1;
    }

    vector<Result> results;

    cout << "Processing knapsack instances using Branch and Bound algorithm..." << endl;

    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().string();
                cout << "Processing: " << filename << "..." << endl;

                Result result = solveBranchAndBound(filename);
                if (result.maxValue > 0 || result.executionTime > 0) {
                    results.push_back(result);
                    cout << "Completed: Max value = " << result.maxValue
                         << ", Time = " << fixed << setprecision(3)
                         << result.executionTime << "s" << endl;
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Error accessing directory: " << e.what() << endl;
        return 1;
    }

    if (results.empty()) {
        cout << "No valid knapsack files found or processed." << endl;
        return 1;
    }

    sort(results.begin(), results.end(),
         [](const Result& a, const Result& b) {
             return a.filename < b.filename;
         });

    printStatistics(results);

    saveResultsToCSV(results, "branch_and_bound_results.csv");

    return 0;
}
