#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>
#include "HashMap.hpp"

void test_basic_operations() {
    std::cout << "Testing basic operations...\n";
    
    HashMap<std::string, int> map;
    assert(map.empty());
    assert(map.size() == 0);
    
    assert(map.insert("one", 1));
    assert(map.insert("two", 2));
    assert(map.insert("three", 3));
    assert(!map.insert("one", 10));
    
    assert(map.size() == 3);
    assert(!map.empty());
    
    assert(map.contains("one"));
    assert(map.contains("two"));
    assert(!map.contains("four"));
    
    assert(map["one"] == 1);
    map["one"] = 10;
    assert(map["one"] == 10);
    
    HashMap<std::string, int> map2;
    map2["hello"] = 42;
    assert(map2["hello"] == 42);
    assert(map2.size() == 1);
    
    assert(map.at("two") == 2);
    map.at("two") = 20;
    assert(map.at("two") == 20);
    
    bool exception_thrown = false;
    try {
        map.at("nonexistent");
    } catch (const std::out_of_range&) {
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    assert(map.erase("two"));
    assert(!map.contains("two"));
    assert(map.size() == 2);
    assert(!map.erase("nonexistent"));
    
    std::cout << "Basic operations test passed!\n";
}

void test_iterator() {
    std::cout << "Testing iterators...\n";
    
    HashMap<int, std::string> map;
    for (int i = 0; i < 10; i++) {
        map.insert(i, "value" + std::to_string(i));
    }
    
    std::vector<std::pair<int, std::string>> elements;
    for (auto it = map.begin(); it != map.end(); ++it) {
        elements.push_back({(*it).first, (*it).second});
    }
    
    assert(elements.size() == 10);
    
    std::sort(elements.begin(), elements.end());
    for (int i = 0; i < 10; i++) {
        assert(elements[i].first == i);
        assert(elements[i].second == "value" + std::to_string(i));
    }
    
    std::vector<std::pair<int, std::string>> elements2;
    for (const auto& pair : map) {
        elements2.push_back({pair.first, pair.second});
    }
    std::sort(elements2.begin(), elements2.end());
    assert(elements == elements2);
    
    auto it = map.find(5);
    assert(it != map.end());
    assert((*it).first == 5);
    assert((*it).second == "value5");
    
    it = map.find(100);
    assert(it == map.end());
    
    auto it2 = map.begin();
    auto old_it = it2++;
    assert(old_it != it2);
    
    std::cout << "Iterator test passed!\n";
}

void test_insert_or_assign() {
    std::cout << "Testing insert_or_assign...\n";
    
    HashMap<std::string, double> map;
    
    bool result = map.insert_or_assign("pi", 3.14);
    assert(result == true);
    assert(map["pi"] == 3.14);
    
    result = map.insert_or_assign("pi", 3.14159);
    assert(result == false);
    assert(map["pi"] == 3.14159);
    
    assert(map.size() == 1);
    
    std::cout << "Insert or assign test passed!\n";
}

void test_rehash_operator_brackets() {
    std::cout << "Testing operator[] after rehash...\n";

    HashMap<int, int> map;
    for (int i = 0; i < 6; i++) {
        map.insert(i, i);
    }

    map[100] = 12345;
    assert(map.at(100) == 12345);
    assert(map.size() == 7);

    for (int i = 0; i < 1000; i++) {
        map[i] = i + 1;
    }

    for (int i = 0; i < 1000; i++) {
        assert(map.at(i) == i + 1);
    }

    std::cout << "operator[] after rehash test passed!\n";
}

void test_const_iteration_and_moved_from_usage() {
    std::cout << "Testing const iteration and moved-from map usage...\n";

    HashMap<int, int> map(0);
    map[1] = 10;
    map[2] = 20;

    const HashMap<int, int>& const_map = map;
    int sum = 0;
    for (const auto& pair : const_map) {
        sum += pair.second;
    }
    assert(sum == 30);
    assert(const_map.find(1) != const_map.end());
    assert(const_map.find(999) == const_map.end());

    HashMap<int, int> moved(std::move(map));
    assert(moved.size() == 2);

    map[42] = 42;
    assert(map.at(42) == 42);
    assert(map.size() == 1);

    std::cout << "Const iteration and moved-from usage test passed!\n";
}

void test_copy_and_move() {
    std::cout << "Testing copy and move operations...\n";
    
    HashMap<int, int> map1;
    for (int i = 0; i < 5; i++) {
        map1.insert(i, i * 10);
    }
    
    HashMap<int, int> map2(map1);
    assert(map2.size() == 5);
    for (int i = 0; i < 5; i++) {
        assert(map2.contains(i));
        assert(map2[i] == i * 10);
    }
    
    map2[0] = 999;
    assert(map1[0] == 0);
    
    HashMap<int, int> map3(std::move(map2));
    assert(map3.size() == 5);
    assert(map2.size() == 0);
    assert(map3[0] == 999);
    
    HashMap<int, int> map4;
    map4 = map1;
    assert(map4.size() == 5);
    for (int i = 0; i < 5; i++) {
        assert(map4[i] == i * 10);
    }
    
    HashMap<int, int> map5;
    map5 = std::move(map4);
    assert(map5.size() == 5);
    assert(map4.size() == 0);
    
    std::cout << "Copy and move test passed!\n";
}

void test_algorithm_compatibility() {
    std::cout << "Testing algorithm compatibility...\n";
    
    HashMap<int, int> map;
    for (int i = 0; i < 10; i++) {
        map.insert(i, i * 2);
    }
    
    int sum = 0;
    std::for_each(map.begin(), map.end(), [&sum](const auto& pair) {
        sum += pair.second;
    });
    assert(sum == 90);
    
    auto it = std::find_if(map.begin(), map.end(), [](const auto& pair) {
        return pair.first == 5;
    });
    assert(it != map.end());
    assert((*it).second == 10);
    
    int count = std::count_if(map.begin(), map.end(), [](const auto& pair) {
        return pair.second > 10;
    });
    assert(count == 4);
    
    std::cout << "Algorithm compatibility test passed!\n";
}

int main() {
    try {
        test_basic_operations();
        test_iterator();
        test_insert_or_assign();
        test_rehash_operator_brackets();
        test_const_iteration_and_moved_from_usage();
        test_copy_and_move();
        test_algorithm_compatibility();
        
        std::cout << "\nAll tests passed successfully!\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
