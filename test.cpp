#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <algorithm>

int main() {
    const int numElements = 100000;
    const int numTests = 100000;
    
    std::vector<int> testValues(numTests);
    for (int i = 0; i < numTests; ++i) {
        testValues[i] = rand() % numElements;
    }

    // Usando std::vector
    std::vector<int> myVector;
    auto startVector = std::chrono::high_resolution_clock::now();
    for (const auto& value : testValues) {
        if (std::find(myVector.begin(), myVector.end(), value) == myVector.end()) {
            myVector.push_back(value);
        }
    }
    auto endVector = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationVector = endVector - startVector;

    // Usando std::unordered_set
    std::unordered_set<int> mySet;
    auto startSet = std::chrono::high_resolution_clock::now();
    for (const auto& value : testValues) {
        mySet.insert(value);
    }
    auto endSet = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationSet = endSet - startSet;

    // Iteração sobre os elementos
    auto startIterVector = std::chrono::high_resolution_clock::now();
    for (const auto& value : myVector) {
        volatile int dummy = value; // Prevent optimization
    }
    auto endIterVector = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationIterVector = endIterVector - startIterVector;

    auto startIterSet = std::chrono::high_resolution_clock::now();
    for (const auto& value : mySet) {
        volatile int dummy = value; // Prevent optimization
    }
    auto endIterSet = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationIterSet = endIterSet - startIterSet;

    // Resultados
    std::cout << "Tempo para verificar e adicionar elementos no vetor: " << durationVector.count() << " segundos\n";
    std::cout << "Tempo para adicionar elementos no unordered_set: " << durationSet.count() << " segundos\n";
    std::cout << "Tempo para iterar sobre o vetor: " << durationIterVector.count() << " segundos\n";
    std::cout << "Tempo para iterar sobre o unordered_set: " << durationIterSet.count() << " segundos\n";

    return 0;
}
