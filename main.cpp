/*
 * The MIT License (MIT)
 *
 * raw_pointers_safe_deletion_tests
 *
 * Copyright (C) 2024 Stephen G. Tuggy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <regex>
#include <algorithm>
#include <functional>
#include "owner.h"

class Mesh {
    int data{};
};

constexpr uint32_t MAX_KEY_LENGTH = 62;
constexpr uint32_t MAX_ENTRIES_PER = 100;
constexpr uint32_t MAX_TEST_ITERATIONS = 89;
constexpr uint32_t INITIAL_SEED = 1713623895;

std::string char_values_from{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};

template<typename T> void remove_all_references_to(const T *thus, owner<std::vector<T *> *> &vec, bool& collection_was_deleted) {
    if (vec) {
        if (!vec->empty()) {
            auto first_to_remove = std::stable_partition(vec->begin(), vec->end(),
                                                         [thus](const T *pi) { return pi != thus; });
            vec->erase(first_to_remove, vec->end());
        }
        if (vec->empty()) {
            delete vec;
            vec = nullptr;
            collection_was_deleted = true;
        }
    }
}

bool generalized_approach_A(std::unordered_map<std::string, owner<std::vector<Mesh *> *>> &bfxm_hash_table, const std::string& hash_name,
                            owner<Mesh *> &thus, bool &collection_was_deleted) {
    owner<std::vector<Mesh *> *> &hashers = bfxm_hash_table.at(hash_name);
    remove_all_references_to(thus, hashers, collection_was_deleted);
    delete thus;
    thus = nullptr;
    if (collection_was_deleted) {
        std::cout << "collection was deleted" << std::endl;
    }
    return true;
}

bool pr_754_approach(std::unordered_map<std::string, owner<std::vector<Mesh *> *>> &bfxm_hash_table, const std::string& hash_name, owner<Mesh *> & thus, bool &collection_was_deleted)
{
    owner<std::vector<Mesh *> *> &hashers = bfxm_hash_table.at(hash_name);
    if (hashers) {
        try
        {
            if (!hashers->empty()) {
                auto first_to_remove = std::stable_partition(hashers->begin(), hashers->end(),
                            [thus](owner<Mesh *> &pi) { return pi != thus; });
                hashers->erase(first_to_remove, hashers->end());
            }
            if (hashers->empty())
            {
                bfxm_hash_table.erase(hash_name);
                delete hashers;
                hashers = nullptr;
                std::cout << "hashers was deleted" << std::endl;
                collection_was_deleted = true;
                delete thus;
                thus = nullptr;
                return true;
            }
            delete thus;
            thus = nullptr;
        } catch (std::exception & ex) {
            std::cerr << "Exception occurred in test of PR 754 WIP code: " << ex.what() << std::endl;
            return false;
        }
    }
    return true;
}

bool BenjamenMeyersApproach(std::unordered_map<std::string, owner<std::vector<Mesh *> *>> &bfxm_hash_table, const std::string& hash_name, owner<Mesh *> & thus, bool &collection_was_deleted) {
    owner<std::vector<Mesh *> *> &hashers = bfxm_hash_table.at(hash_name);
//    std::vector<Mesh *>::iterator finder;
    if (hashers) {
        // This if block added by SGT 2024-04-20
        if (hashers->empty()) {
            bfxm_hash_table.erase(hash_name);
            delete hashers;
            hashers = nullptr;
            std::cout << "hashers was deleted" << std::endl;
            collection_was_deleted = true;
            delete thus;
            thus = nullptr;
            return true;
        }
        try {
            // the following loop has several tricks to it:
            // 1. `std::vector::erase()` can take an iterator and remove it from the vector; but invalidates
            //      the iterator in the process
            // 2. To overcome the invalid iterator issue, the next previous iterator is cached
            // 3. In the case that the previous iterator would be invalid (e.g. it's at the start) then it needs
            //      to restart the loop without the loop conditions, therefore a simple GOTO is used instead to
            //      avoid the incrementing the iterator so that values are not skipped
            // A reverse iterator could kind of help this; however, `std::vector::erase` unfortunately
            // does not work on reverse iterators.
            for (auto hashItem = hashers->begin(); hashItem != hashers->end(); ++hashItem) {
                retryEraseItem:
                if (*hashItem == thus) {
                    bool resetIter = false;
                    auto cachedHashItem = hashers->begin();
                    if (hashItem != hashers->begin() && hashItem != hashers->end()) {
                        cachedHashItem = hashItem - 1;
                    } else {
                        resetIter = true;
                        cachedHashItem = hashItem + 1;
                    }
                    if (hashers->empty()) {
                        bfxm_hash_table.erase(hash_name);
                        delete hashers;
                        hashers = nullptr;
                        std::cout << "hashers was deleted" << std::endl;
                        collection_was_deleted = true;
                        delete thus;
                        thus = nullptr;
                        return true;
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

            delete thus;
            thus = nullptr;
        } catch (std::exception & ex) {
            std::cerr << "Exception occurred in test of Benjamen Meyer's code: " << ex.what() << std::endl;
            return false;
        }

        return true;
    }

    return false;
}

bool is_integer(const std::string & s){
    const std::regex re("[-+]?[0-9]+");
    return std::regex_match(s, re);
}

bool test_harness(long long input, const uint32_t output_size, bool (*which_approach_to_test)(
                      std::unordered_map<std::string, std::vector<Mesh*>*>& bfxm_hash_table,
                      const std::string& hash_name, owner<Mesh *> & thus, bool &container_was_deleted))
{
    auto *bfxm_hash_table = new std::unordered_map<std::string, std::vector<Mesh *> *>();

    std::seed_seq seq{input};
    std::vector<std::uint32_t> seeds(output_size);
    seq.generate(seeds.begin(), seeds.end());
    for (const std::uint32_t n : seeds) {
        std::cout << "Seed: " << n << '\n';
    }
    // TEMP
    seeds[0] = 1299506555;

    std::mt19937 random_value_generator(seeds[0]);
    std::uniform_int_distribution<uint32_t> uni_max_entries(0, MAX_ENTRIES_PER);
    uint32_t how_many_bfxm_hash_table_keys = uni_max_entries(random_value_generator);
    std::vector<std::string> keys_to_insert;
    keys_to_insert.reserve(how_many_bfxm_hash_table_keys);
    std::uniform_int_distribution<uint32_t> uni_key_length(1, MAX_KEY_LENGTH);
    for (uint32_t i = 0; i < how_many_bfxm_hash_table_keys; ++i) {
        uint32_t key_len = uni_key_length(random_value_generator);
        std::vector<char> key_buffer;
        for (uint32_t char_number = 0; char_number < key_len; ++char_number)
        {
            key_buffer.push_back(char_values_from.at(char_number));
        }
        std::shuffle(key_buffer.begin(), key_buffer.end(), random_value_generator);
        char key_chars[MAX_KEY_LENGTH + 1]{};
        for (uint32_t char_number = 0; char_number < key_len; ++char_number)
        {
            key_chars[char_number] = key_buffer.at(char_number);
        }
        key_chars[key_len] = '\0';
        std::string generated_key{key_chars};
        std::cout << generated_key << std::endl;
        keys_to_insert.push_back(generated_key);
    }

    uint32_t how_many_bfxm_value_vectors = uni_max_entries(random_value_generator);
    std::vector<std::vector<Mesh *> *> vector_pointers_to_add_to_bfxm;
    vector_pointers_to_add_to_bfxm.reserve(how_many_bfxm_value_vectors);
    for (uint32_t j = 0; j < how_many_bfxm_value_vectors; ++j) {
        uint32_t how_many_Meshes = uni_max_entries(random_value_generator);
        auto * child_vector = new std::vector<Mesh *>();
        child_vector->reserve(how_many_Meshes);
        for (uint32_t k = 0; k < how_many_Meshes; ++k) {
            Mesh *new_Mesh = new Mesh();
            child_vector->push_back(new_Mesh);
        }
        vector_pointers_to_add_to_bfxm.push_back(child_vector);
    }

    if (how_many_bfxm_hash_table_keys == 0)
    {
        return true;
    }
    for (uint32_t l = 0; l < how_many_bfxm_hash_table_keys; ++l) {
        std::uniform_int_distribution<uint32_t> uni_key(0, how_many_bfxm_hash_table_keys - 1);
        uint32_t index_of_key_to_add = uni_key(random_value_generator);
        std::uniform_int_distribution<uint32_t> uni_value(0, how_many_bfxm_value_vectors - 1);
        uint32_t index_of_vector_to_add = uni_value(random_value_generator);
        bfxm_hash_table->insert(std::make_pair(keys_to_insert.at(index_of_key_to_add), vector_pointers_to_add_to_bfxm.at(index_of_vector_to_add)));
    }

    std::uniform_int_distribution<uint32_t> uniA(0, MAX_TEST_ITERATIONS);
    uint32_t number_of_iterations = uniA(random_value_generator);
    std::uniform_int_distribution<uint32_t> uni_which_key(0, how_many_bfxm_hash_table_keys - 1);
    for (uint32_t m = 0; m < number_of_iterations; ++m) {
        uint32_t key_index = uni_which_key(random_value_generator);
        std::string key_for_this_iteration = keys_to_insert.at(key_index);
        auto vector_to_pull_mesh_from = bfxm_hash_table->find(key_for_this_iteration);
        if (vector_to_pull_mesh_from == bfxm_hash_table->end())
        {
            continue;
        }
        auto *this_vector = vector_to_pull_mesh_from->second;
        if (this_vector == nullptr)
        {
            continue;
        }
        size_t vector_size = this_vector->size();
        if (vector_size == 0)
        {
            continue;
        }
        std::uniform_int_distribution<size_t> uni(0,vector_size - 1);
        size_t mesh_index = uni(random_value_generator);
        Mesh *mesh_for_this_iteration = this_vector->at(mesh_index);
        bool collection_was_deleted = false;
        bool test_successful = which_approach_to_test(*bfxm_hash_table, key_for_this_iteration, mesh_for_this_iteration, collection_was_deleted);
        if (!test_successful) {
            return false;
        }
        if (collection_was_deleted) {
            return true;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    long long input{time(NULL)};
    if (argc > 1)
    {
        char* s = argv[argc - 1];
        if (is_integer(s))
        {
            input = std::stoll(s);
        }
    }
    constexpr uint32_t output_size{1};

    bool (*whichApproachToTest)(std::unordered_map<std::string, std::vector<Mesh*>*>& bfxm_hash_table,
                                const std::string& hash_name, owner<Mesh *> & thus, bool &collection_was_deleted) = BenjamenMeyersApproach;
//    if (!test_harness(input, output_size, whichApproachToTest))
//    {
//        std::cerr << "Test of Benjamen Meyer's approach failed!" << std::endl;
//        return 1;
//    }

    whichApproachToTest = pr_754_approach;
    if (!test_harness(input, output_size, whichApproachToTest))
    {
        std::cerr << "Test of the PR 754 approach failed!" << std::endl;
        return 1;
    }

    whichApproachToTest = generalized_approach_A;
    if (!test_harness(input, output_size, whichApproachToTest)) {
        std::cerr << "Test of Generalized Approach A failed!" << std::endl;
        return 1;
    }

    std::cout << "All tests successful!" << std::endl;
    return 0;
}
