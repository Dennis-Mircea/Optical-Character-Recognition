// copyright Luca Istrate, Andrei Medar
#include "randomForest.h"
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include "decisionTree.h"

using std::vector;
using std::pair;
using std::string;
using std::mt19937;

vector<vector<int>> get_random_samples(const vector<vector<int>> &samples,
                                       int num_to_return) {
    // TODO(you)
    // Intoarce un vector de marime num_to_return cu elemente random,
    // diferite din samples
    vector<vector<int>> ret;
    std::random_device rand_r;
    int n = samples.size(), index_random;
    // cautam un nr random de la 0 la n, care va reprezenta linia din vectorul
    // samples
    index_random = rand_r() % n;

    // cat timp nu s-au gasit numarul total de samples random
    while (ret.size() < num_to_return) {
        index_random = rand_r() % n;
        // verificam daca linia random din samples a mai fost ales deja
        auto it = std::find(ret.begin(), ret.end(), samples[index_random]);
        // daca nu l-a gasit(vectorul din samples), il adauga in vectorul ret
        if (it == ret.end()) {
            ret.push_back(samples[index_random]);
        }
    }
    return ret;
}

RandomForest::RandomForest(int num_trees, const vector<vector<int>> &samples)
    : num_trees(num_trees), images(samples) {}

void RandomForest::build() {
    // Aloca pentru fiecare Tree cate n / num_trees
    // Unde n e numarul total de teste de training
    // Apoi antreneaza fiecare tree cu testele alese
    assert(!images.empty());
    vector<vector<int>> random_samples;

    int data_size = images.size() / num_trees;

    for (int i = 0; i < num_trees; i++) {
        // cout << "Creating Tree nr: " << i << endl;
        random_samples = get_random_samples(images, data_size);

        // Construieste un Tree nou si il antreneaza
        trees.push_back(Node());
        trees[trees.size() - 1].train(random_samples);
    }
}

int RandomForest::predict(const vector<int> &image) {
    // TODO(you)
    // Va intoarce cea mai probabila prezicere pentru testul din argument
    // se va interoga fiecare Tree si se va considera raspunsul final ca
    // fiind cel majoritar
    // vector care contine numarul de aparitii al fiecarei clase
    vector<int> result(10, 0);
    for (int i = 0; i < num_trees; i++) {
        // verficam daca functia e valida
        if (trees[i].predict(image) != -1) {
            // incrementam numarul de aparitii
            result[trees[i].predict(image)]++;
        }
    }
    // cautam clasa majoritara in vectorul de aparitii
    int max = -1, majoritar;
    for (int i = 0; i < 10; ++i) {
        if (result[i] > max) {
            max = result[i];
            majoritar = i;
        }
    }
    return majoritar;
}
