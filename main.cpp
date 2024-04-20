#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <list>

class Mesh {
    __attribute__((unused)) int data{};
};

const uint32_t MAX_KEY_LENGTH = 80;
const uint32_t MAX_ENTRIES_PER = 100;
const uint32_t MAX_TEST_ITERATIONS = 21;
const uint32_t INITIAL_SEED = 89;

//typedef std::basic_string<unsigned char> std::string;

//static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

bool testBenjamenMeyersCode(std::unordered_map<std::string, std::vector<Mesh *> *> &bfxm_hash_table, std::string& hash_name, Mesh * thus) {
    std::vector<Mesh *> *hashers = bfxm_hash_table.at(hash_name);
    std::vector<Mesh *>::iterator finder;
    if (hashers) {
        try {
            // the following loop has several tricks to it:
            // 1. `std::vector::erase()` can take an iterator and remove it from the vector; but invalidates
            //      the iterator in the process
            // 2. To overcome the invalid iterator issue, the next previous iterator is cached
            // 3. In the case that the previous iterator would be invalid (e.g it's at the start) then it needs
            //      to restart the loop without the loop conditions, therefore a simple GOTO is used instead to
            //      avoid the incrementing the iterator so that values are not skipped
            // A reverse iterator could kind of help this; however, `std::vector::erase` unfortunately
            // does not work on reverse iterators.
            for (auto hashItem = hashers->begin(); hashItem != hashers->end(); ++hashItem) {
                retryEraseItem:
                if (*hashItem == thus) {
                    bool resetIter = false;
                    auto cachedHashItem = hashers->begin();
                    if (hashItem != hashers->begin()) {
                        cachedHashItem = hashItem - 1;
                    } else {
                        resetIter = true;
                        cachedHashItem = hashItem + 1;
                    }
                    hashers->erase(hashItem);
                    if (hashers->empty()) {
                        bfxm_hash_table.erase(hash_name);
                        delete hashers;
                        hashers = nullptr;
                        break;
                    }

                    if (resetIter) {
                        hashItem = hashers->begin();
                        // a necessary use of Goto as we do not want to use ++hashItem
                        goto retryEraseItem;
                    } else {
                        hashItem = cachedHashItem;
                    }
                }
            }
        } catch (std::exception & ex) {
            std::cerr << "Exception occurred in test of Benjamen Meyer's code: " << ex.what() << std::endl;
            return false;
        }

        return true;
    }

    return false;
}

int main() {
    constexpr uint32_t input{INITIAL_SEED};
    constexpr uint32_t output_size{1};

    std::unordered_map<std::string, std::vector<Mesh *> *> starting_bfxm_hash_table;

    std::seed_seq seq{input};
    std::vector<std::uint32_t> seeds(output_size);
    seq.generate(seeds.begin(), seeds.end());
    for (const std::uint32_t n : seeds) {
        std::cout << "Seed: " << n << '\n';
    }

    std::mt19937 random_value_generator(seeds[0]);
    uint32_t how_many_bfxm_hash_table_keys = random_value_generator() % MAX_ENTRIES_PER;
    std::vector<std::string> keys_to_insert;
    keys_to_insert.reserve(how_many_bfxm_hash_table_keys);
    for (uint32_t i = 0; i < how_many_bfxm_hash_table_keys; ++i) {
        uint32_t key_len = random_value_generator() % MAX_KEY_LENGTH;
        char key_buffer[MAX_KEY_LENGTH + 1]{};
        for (uint32_t j1 = 0; j1 < key_len; ++j1) {
            key_buffer[j1] = random_value_generator() % ('z' - ' ') + ' ';
        }
        key_buffer[key_len] = '\0';
        std::string new_key_value{const_cast<const char *>(key_buffer)};
        std::cout << key_buffer << '\n';
        keys_to_insert.push_back(new_key_value);
    }

    uint32_t how_many_bfxm_value_vectors = random_value_generator() % MAX_ENTRIES_PER;
    std::vector<std::vector<Mesh *> *> vector_pointers_to_add_to_bfxm;
    vector_pointers_to_add_to_bfxm.reserve(how_many_bfxm_value_vectors);
    for (uint32_t j = 0; j < how_many_bfxm_value_vectors; ++j) {
        uint32_t how_many_Meshes = random_value_generator() % MAX_ENTRIES_PER;
        auto * child_vector = new std::vector<Mesh *>();
        child_vector->reserve(how_many_Meshes);
        for (uint32_t k = 0; k < how_many_Meshes; ++k) {
            Mesh *new_Mesh = new Mesh();
            child_vector->push_back(new_Mesh);
        }
        vector_pointers_to_add_to_bfxm.push_back(child_vector);
    }

    for (uint32_t l = 0; l < how_many_bfxm_hash_table_keys; ++l) {
        uint32_t index_of_vector_to_add = random_value_generator() % how_many_bfxm_value_vectors;
        starting_bfxm_hash_table[keys_to_insert[l]] = vector_pointers_to_add_to_bfxm[index_of_vector_to_add];
    }

    uint32_t number_of_iterations = random_value_generator() % MAX_TEST_ITERATIONS;
    for (uint32_t m = 0; m < number_of_iterations; ++m) {
        uint32_t key_index = random_value_generator() % how_many_bfxm_hash_table_keys;
        std::string key_for_this_iteration = keys_to_insert.at(key_index);
        std::vector<Mesh *> *vector_to_pull_mesh_from = starting_bfxm_hash_table[key_for_this_iteration];
        uint32_t mesh_index = random_value_generator() % vector_to_pull_mesh_from->size();
        Mesh *mesh_for_this_iteration = vector_to_pull_mesh_from->at(mesh_index);
        bool test_successful = testBenjamenMeyersCode(starting_bfxm_hash_table, key_for_this_iteration, mesh_for_this_iteration);
        if (!test_successful) {
            std::cerr << "Unsuccessful test of Benjamen Meyer's code" << std::endl;
            return 1;
        }
    }

    std::cout << "All tests successful!" << std::endl;
    return 0;
}