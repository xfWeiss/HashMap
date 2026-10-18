#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>

template<typename Key, typename Value>
class HashMap {
private:
    using value_storage_type = std::pair<const Key, Value>;

    struct Node {
        value_storage_type data;
        Node* next;

        Node(const Key& key, const Value& value, Node* next_node = nullptr)
            : data(key, value), next(next_node) {}
    };

    static constexpr std::size_t DEFAULT_BUCKET_COUNT = 8;
    static constexpr double MAX_LOAD_FACTOR = 0.75;

    Node** buckets;
    std::size_t bucket_count;
    std::size_t current_size;

    void allocate_buckets(std::size_t count) {
        bucket_count = count == 0 ? DEFAULT_BUCKET_COUNT : count;
        buckets = new Node*[bucket_count]{};
    }

    void ensure_initialized() {
        if (buckets == nullptr || bucket_count == 0) {
            allocate_buckets(DEFAULT_BUCKET_COUNT);
            current_size = 0;
        }
    }

    std::size_t bucket_index(const Key& key) const {
        return std::hash<Key>{}(key) % bucket_count;
    }

    void rehash(std::size_t new_bucket_count) {
        if (new_bucket_count < DEFAULT_BUCKET_COUNT) {
            new_bucket_count = DEFAULT_BUCKET_COUNT;
        }

        Node** new_buckets = new Node*[new_bucket_count]{};

        for (std::size_t i = 0; i < bucket_count; ++i) {
            Node* curr = buckets[i];
            while (curr) {
                Node* next = curr->next;
                const std::size_t new_idx = std::hash<Key>{}(curr->data.first) % new_bucket_count;
                curr->next = new_buckets[new_idx];
                new_buckets[new_idx] = curr;
                curr = next;
            }
        }

        delete[] buckets;
        buckets = new_buckets;
        bucket_count = new_bucket_count;
    }

    void ensure_capacity_for(std::size_t required_size) {
        ensure_initialized();
        while (static_cast<double>(required_size) / static_cast<double>(bucket_count) > MAX_LOAD_FACTOR) {
            rehash(bucket_count * 2);
        }
    }

public:
    explicit HashMap(std::size_t initial = DEFAULT_BUCKET_COUNT)
        : buckets(nullptr), bucket_count(0), current_size(0) {
        allocate_buckets(initial == 0 ? DEFAULT_BUCKET_COUNT : initial);
    }

    ~HashMap() {
        clear();
        delete[] buckets;
    }

    HashMap(const HashMap& other)
        : buckets(nullptr), bucket_count(0), current_size(0) {
        allocate_buckets(other.bucket_count == 0 ? DEFAULT_BUCKET_COUNT : other.bucket_count);
        for (std::size_t i = 0; i < other.bucket_count; ++i) {
            Node* curr = other.buckets[i];
            while (curr) {
                insert(curr->data.first, curr->data.second);
                curr = curr->next;
            }
        }
    }

    HashMap(HashMap&& other) noexcept
        : buckets(other.buckets),
          bucket_count(other.bucket_count),
          current_size(other.current_size) {
        other.buckets = nullptr;
        other.bucket_count = 0;
        other.current_size = 0;
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }

        HashMap tmp(other);
        swap(tmp);
        return *this;
    }

    HashMap& operator=(HashMap&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        clear();
        delete[] buckets;

        buckets = other.buckets;
        bucket_count = other.bucket_count;
        current_size = other.current_size;

        other.buckets = nullptr;
        other.bucket_count = 0;
        other.current_size = 0;

        return *this;
    }

    void swap(HashMap& other) noexcept {
        using std::swap;
        swap(buckets, other.buckets);
        swap(bucket_count, other.bucket_count);
        swap(current_size, other.current_size);
    }

    std::size_t size() const {
        return current_size;
    }

    bool empty() const {
        return current_size == 0;
    }

    void clear() {
        if (buckets == nullptr) {
            current_size = 0;
            bucket_count = 0;
            return;
        }

        for (std::size_t i = 0; i < bucket_count; ++i) {
            Node* curr = buckets[i];
            while (curr) {
                Node* tmp = curr;
                curr = curr->next;
                delete tmp;
            }
            buckets[i] = nullptr;
        }
        current_size = 0;
    }

    bool insert(const Key& key, const Value& value) {
        ensure_initialized();

        std::size_t idx = bucket_index(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->data.first == key) {
                return false;
            }
            curr = curr->next;
        }

        ensure_capacity_for(current_size + 1);
        idx = bucket_index(key);
        buckets[idx] = new Node(key, value, buckets[idx]);
        ++current_size;
        return true;
    }

    bool insert_or_assign(const Key& key, const Value& value) {
        ensure_initialized();

        std::size_t idx = bucket_index(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->data.first == key) {
                curr->data.second = value;
                return false;
            }
            curr = curr->next;
        }

        ensure_capacity_for(current_size + 1);
        idx = bucket_index(key);
        buckets[idx] = new Node(key, value, buckets[idx]);
        ++current_size;
        return true;
    }

    Value& operator[](const Key& key) {
        ensure_initialized();

        std::size_t idx = bucket_index(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->data.first == key) {
                return curr->data.second;
            }
            curr = curr->next;
        }

        ensure_capacity_for(current_size + 1);
        idx = bucket_index(key);
        Node* new_node = new Node(key, Value{}, buckets[idx]);
        buckets[idx] = new_node;
        ++current_size;
        return new_node->data.second;
    }

    Value& at(const Key& key) {
        if (buckets == nullptr || bucket_count == 0) {
            throw std::out_of_range("Key not found");
        }

        Node* curr = buckets[bucket_index(key)];
        while (curr) {
            if (curr->data.first == key) {
                return curr->data.second;
            }
            curr = curr->next;
        }

        throw std::out_of_range("Key not found");
    }

    const Value& at(const Key& key) const {
        if (buckets == nullptr || bucket_count == 0) {
            throw std::out_of_range("Key not found");
        }

        Node* curr = buckets[bucket_index(key)];
        while (curr) {
            if (curr->data.first == key) {
                return curr->data.second;
            }
            curr = curr->next;
        }

        throw std::out_of_range("Key not found");
    }

    bool contains(const Key& key) const {
        if (buckets == nullptr || bucket_count == 0) {
            return false;
        }

        Node* curr = buckets[bucket_index(key)];
        while (curr) {
            if (curr->data.first == key) {
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool erase(const Key& key) {
        if (buckets == nullptr || bucket_count == 0) {
            return false;
        }

        const std::size_t idx = bucket_index(key);
        Node* curr = buckets[idx];
        Node* prev = nullptr;

        while (curr) {
            if (curr->data.first == key) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    buckets[idx] = curr->next;
                }
                delete curr;
                --current_size;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    class const_iterator;

    class iterator {
        friend class const_iterator;

    private:
        Node** buckets_ptr;
        std::size_t bucket_count_value;
        std::size_t index;
        Node* node;

        void advance_to_next_valid_bucket() {
            while (node == nullptr && index < bucket_count_value) {
                node = buckets_ptr[index];
                if (node != nullptr) {
                    return;
                }
                ++index;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const Key, Value>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator(Node** buckets, std::size_t bucket_count, std::size_t start_index, Node* start_node)
            : buckets_ptr(buckets), bucket_count_value(bucket_count), index(start_index), node(start_node) {}

        reference operator*() const {
            return node->data;
        }

        pointer operator->() const {
            return &node->data;
        }

        iterator& operator++() {
            if (node != nullptr) {
                node = node->next;
            }
            if (node == nullptr) {
                ++index;
                advance_to_next_valid_bucket();
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return node == other.node;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    class const_iterator {
    private:
        Node* const* buckets_ptr;
        std::size_t bucket_count_value;
        std::size_t index;
        const Node* node;

        void advance_to_next_valid_bucket() {
            while (node == nullptr && index < bucket_count_value) {
                node = buckets_ptr[index];
                if (node != nullptr) {
                    return;
                }
                ++index;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const Key, Value>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        const_iterator(Node* const* buckets, std::size_t bucket_count, std::size_t start_index, const Node* start_node)
            : buckets_ptr(buckets), bucket_count_value(bucket_count), index(start_index), node(start_node) {}

        const_iterator(const iterator& it)
            : buckets_ptr(it.buckets_ptr), bucket_count_value(it.bucket_count_value), index(it.index), node(it.node) {}

        reference operator*() const {
            return node->data;
        }

        pointer operator->() const {
            return &node->data;
        }

        const_iterator& operator++() {
            if (node != nullptr) {
                node = node->next;
            }
            if (node == nullptr) {
                ++index;
                advance_to_next_valid_bucket();
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return node == other.node;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }
    };

    iterator begin() {
        if (buckets == nullptr || bucket_count == 0) {
            return end();
        }
        for (std::size_t i = 0; i < bucket_count; ++i) {
            if (buckets[i]) {
                return iterator(buckets, bucket_count, i, buckets[i]);
            }
        }
        return end();
    }

    iterator end() {
        return iterator(buckets, bucket_count, bucket_count, nullptr);
    }

    const_iterator begin() const {
        return cbegin();
    }

    const_iterator end() const {
        return cend();
    }

    const_iterator cbegin() const {
        if (buckets == nullptr || bucket_count == 0) {
            return cend();
        }
        for (std::size_t i = 0; i < bucket_count; ++i) {
            if (buckets[i]) {
                return const_iterator(buckets, bucket_count, i, buckets[i]);
            }
        }
        return cend();
    }

    const_iterator cend() const {
        return const_iterator(buckets, bucket_count, bucket_count, nullptr);
    }

    iterator find(const Key& key) {
        if (buckets == nullptr || bucket_count == 0) {
            return end();
        }

        const std::size_t idx = bucket_index(key);
        Node* curr = buckets[idx];
        while (curr) {
            if (curr->data.first == key) {
                return iterator(buckets, bucket_count, idx, curr);
            }
            curr = curr->next;
        }

        return end();
    }

    const_iterator find(const Key& key) const {
        if (buckets == nullptr || bucket_count == 0) {
            return cend();
        }

        const std::size_t idx = bucket_index(key);
        const Node* curr = buckets[idx];
        while (curr) {
            if (curr->data.first == key) {
                return const_iterator(buckets, bucket_count, idx, curr);
            }
            curr = curr->next;
        }

        return cend();
    }
};
