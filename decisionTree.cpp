// copyright Luca Istrate, Andrei Medar

#include "./decisionTree.h"  // NOLINT(build/include)
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>

using std::string;
using std::pair;
using std::vector;
using std::unordered_map;
using std::make_shared;

// structura unui nod din decision tree
// splitIndex = dimensiunea in functie de care se imparte
// split_value = valoarea in functie de care se imparte
// is_leaf si result sunt pentru cazul in care avem un nod frunza
Node::Node() {
    is_leaf = false;
    left = nullptr;
    right = nullptr;
}

void Node::make_decision_node(const int index, const int val) {
    split_index = index;
    split_value = val;
}

void Node::make_leaf(const vector<vector<int>> &samples,
                     const bool is_single_class) {
    // TODO(you)
    // Seteaza nodul ca fiind de tip frunza (modificati is_leaf si result)
    // is_single_class = true -> toate testele au aceeasi clasa (acela e result)
    // is_single_class = false -> se alege clasa care apare cel mai des
    // verificam daca toate testele au aceeasi clasa
    if (is_single_class) {
        // setam nodu ca fiind frunza
        is_leaf = true;
        result = samples[0][0];
    } else {
        // vector de aparitii pentru fiecare clasa
        vector<int> res(10, 0);
        for (unsigned int i = 0; i < samples.size(); i++) {
            res[samples[i][0]]++;
        }
        // cautam clasa majoranta
        int class_majorant, max = -1;
        for (unsigned int i = 0; i < res.size(); i++) {
            if (res[i] > max) {
                max = res[i];
                class_majorant = i;
            }
        }
        // setam nodul ca fiind frunza
        is_leaf = true;
        result = class_majorant;
    }
}

pair<int, int> find_best_split(const vector<vector<int>> &samples,
                               const vector<int> &dimensions) {
    // TODO(you)
    // Intoarce cea mai buna dimensiune si valoare de split dintre testele
    // primite. Prin cel mai bun split (dimensiune si valoare)
    // ne referim la split-ul care maximizeaza IG
    // pair-ul intors este format din (split_index, split_value)

    int splitIndex = -1, splitValue = -1;
    float max = 0;
    // for-ul merge pentru fiecare IG, daca exista
    for (unsigned int i = 0; i < dimensions.size(); i++) {
        // calculam media armonica de pe coloana data de fiecare
        // element din vectorul dimensions
    	float S = 0;
    	for (unsigned int j = 0; j < samples.size(); j++) {
    		S += 1/(float)samples[j][dimensions[i]];
    	}
        // vaoarea de split va fi raportul dintre numarul de samples si S
    	int splitValue_aux = floor(samples.size() / S);
        // indexul de split va fi elementul actual din dimensions
    	int splitIndex_aux = dimensions[i];

    	pair<vector<int>, vector<int>> best_split;
        // fac splitul pentru valoare de split si splitul de index alese
    	best_split = get_split_as_indexes(samples, splitIndex_aux, splitValue_aux);

        // daca din split rezulta un copil fara niciun element, for-ul merge
        // mai departe
    	if (best_split.first.size() == 0 || best_split.second.size() == 0) {
    		continue;
    	}
        // initializam information gain
    	float  IG = 0.0f;
        // aplicam formula pentru infromation gain din enunt
    	IG = get_entropy(samples) - (best_split.first.size()*
    		get_entropy_by_indexes(samples, best_split.first) +
    		best_split.second.size()*get_entropy_by_indexes(samples,
    		best_split.second))/ (best_split.first.size() + best_split.second.size());
        // setam IG-ul maxim, retinan valoarea de split si indexul de split
    	if (IG > max) {
    		max = IG;
    		splitIndex = splitIndex_aux;
    		splitValue = splitValue_aux;
    	}
    }
    return pair<int, int>(splitIndex, splitValue);
}

void Node::train(const vector<vector<int>> &samples) {
    // TODO(you)
    // Antreneaza nodul curent si copii sai, daca e nevoie
    // 1) verifica daca toate testele primite au aceeasi clasa (raspuns)
    // Daca da, acest nod devine frunza, altfel continua algoritmul.
    // 2) Daca nu exista niciun split valid, acest nod devine frunza. Altfel,
    // ia cel mai bun split si continua recursiv
    // shared_ptr<Node> n = make_shared<Node>();
    // verifica daca toate testele primite au aceeasi clasa
    if (same_class(samples)) {
        // face nodul frunza
    	make_leaf(samples, true);
    } else {
        // vector care retine srqt(785) dimensiuni random
    	vector<int> dim = random_dimensions(785);
        // cauta cel mai bun split
    	pair<int, int> best_split = find_best_split(samples, dim);
        // daca nu a gasit un cel mai bun split atunci face nodul frunza
    	if (best_split.first == -1 || best_split.second == -1) {
    		make_leaf(samples, false);
    	} else {
            // daca s-a gasit cel mai bun split cream subarborele drept
            // si cel stang
    		left = make_shared<Node>();
    		right = make_shared<Node>();
            // facem split pe baza valorii de split si indexului de split
            // rezultat din alegerea celui mai bun split
    		pair<vector<vector<int>>, vector<vector<int>>>
    		split_pair = split(samples, best_split.first, best_split.second);
            // daca rezulta un copil fara elemente, face nodul frunza
    		if (split_pair.first.size() == 0 || split_pair.second.size() == 0) {
    			make_leaf(samples, false);
    		} else {
                // daca are rezulta 2 copii cu element, face nod de decizie
    			make_decision_node(best_split.first, best_split.second);
                // merge recursiv pe subarborele stang, iar apoi pe cel drept
    			left->train(split_pair.first);
    			right->train(split_pair.second);
			}
		}
    }
}

int Node::predict(const vector<int> &image) const {
    // TODO(you)
    // Intoarce rezultatul prezis de catre decision tree
    // verifica daca nodul e frunza si atunci valoarea din nod
    if (is_leaf) {
    	return result;
    } else {
        // verificam partea arborelui in care va merge recursiv programul
        if (image[split_index - 1] <= split_value) {
    	   left->predict(image);
    	} else {
    		right->predict(image);
    	}
    }
}

bool same_class(const vector<vector<int>> &samples) {
    // TODO(you)
    // Verifica daca testele primite ca argument au toate aceeasi
    // clasa(rezultat). Este folosit in train pentru a determina daca
    // mai are rost sa caute split-uri
    // variabila a retine clasa din primul vector din samples
    int a = samples[0][0];
    // verificam daca ceilalti vectori au aceeasi clasa ca primul vector
    for (int i = 1; i < samples.size(); ++i) {
    	if (samples[i][0] != a) {
    		return false;
    	}
    }
    return true;
}

float get_entropy(const vector<vector<int>> &samples) {
    // Intoarce entropia testelor primite
    assert(!samples.empty());
    vector<int> indexes;

    int size = samples.size();
    for (int i = 0; i < size; i++) indexes.push_back(i);

    return get_entropy_by_indexes(samples, indexes);
}

float get_entropy_by_indexes(const vector<vector<int>> &samples,
                             const vector<int> &index) {
    // TODO(you)
    // Intoarce entropia subsetului din setul de teste total(samples)
    // Cu conditia ca subsetul sa contina testele ale caror indecsi se gasesc in
    // vectorul index (Se considera doar liniile din vectorul index)
    int nr = index.size();
    // v - vector de aparitii pentru fiecare clasa
    std::vector<int> v(10, 0);
    for (int i = 0; i < nr; ++i) {
        v[samples[index[i]][0]]++;
    }
    // initializam entropia
    float entropy = 0.0f;
    for (int i = 0; i < 10; ++i) {
        // aplicam formula din enunt pentru entropie si verifica si situatia
        // in care ar fi log din 0
        float pi = (float)v[i] / (float)nr;
        if (pi > 0) {
        	entropy -= pi * log2(pi);
        }
    }
    return entropy;
}

vector<int> compute_unique(const vector<vector<int>> &samples, const int col) {
    // TODO(you)
    // Intoarce toate valorile (se elimina duplicatele)
    // care apar in setul de teste, pe coloana col
    // vector care retine valorile unice de pe o coloana col
    vector<int> uniqueValues;
    vector<int> v;
    for (int i = 0; i < samples.size(); ++i) {
        // adaugam fiecare element de pe coala respectiva
    	uniqueValues.push_back(samples[i][col]);
    }
    // sortam valorile din vector
    std::sort(uniqueValues.begin(), uniqueValues.end());
    // eliminam duplicatele din vector
    auto it = std::unique(uniqueValues.begin(), uniqueValues.end());
    uniqueValues.resize(std::distance(uniqueValues.begin(), it));
    return uniqueValues;
}

pair<vector<vector<int>>, vector<vector<int>>> split(
    const vector<vector<int>> &samples, const int split_index,
    const int split_value) {
    // Intoarce cele 2 subseturi de teste obtinute in urma separarii
    // In functie de split_index si split_value
    vector<vector<int>> left, right;

    auto p = get_split_as_indexes(samples, split_index, split_value);
    for (const auto &i : p.first) left.push_back(samples[i]);
    for (const auto &i : p.second) right.push_back(samples[i]);

    return pair<vector<vector<int>>, vector<vector<int>>>(left, right);
}

pair<vector<int>, vector<int>> get_split_as_indexes(
    const vector<vector<int>> &samples, const int split_index,
    const int split_value) {
    // TODO(you)
    // Intoarce indecsii sample-urilor din cele 2 subseturi obtinute in urma
    // separarii in functie de split_index si split_value
    // vectori care retin indecsii fiecaror samples, care apartin ori
    // subarborelui stang sau drept
    vector<int> left, right;
    for (int i = 0; i < samples.size(); ++i) {
        // verificam pentru fiecare vector din samples daca va fi adaugat in
        // subarborele stang sau drept
    	if (samples[i][split_index] <= split_value) {
    		left.push_back(i);
    	} else {
    		right.push_back(i);
    	}
    }
    return make_pair(left, right);
}

vector<int> random_dimensions(const int size) {
    // TODO(you)
    // Intoarce sqrt(size) dimensiuni diferite pe care sa caute splitul maxim
    // Precizare: Dimensiunile gasite sunt > 0 si < size
    // vector care retine toate dimensiunile alese random
	vector<int> rez;
	std::random_device rand_r;
	bool ok;  // retina daca o clasa a fost sau nu aleasa
	int random_size;
    // cat timp nu s-a gasit numarul de dimensiuni random
    while (rez.size() < floor(sqrt(size))) {
    	ok = true;
    	random_size = rand_r() % size;
        // in caz ca dimensiunea random va fi 0, va fi setata automat
        // ca fiind 1, astfel incat sa apartina intervalului (0, size)
    	if (random_size == 0) {
    		random_size = 1;
        }
        // verificam daca dimensiunea random a mai fost aleasa
    	for (int i = 0; i < rez.size(); ++i) {
    		if (random_size == rez[i]){
    			ok = false;
    			break;
    		}
    	}
        // daca dimensiunea nu a fost aleasa, o adaugam
    	if (ok) {
    		rez.push_back(random_size);
    	}
    }
    return rez;
}
