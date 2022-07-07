#pragma once
// This file should only ever be included in typedefs.h

#include <span>
#include <optional>
#include "il2cpp-utils-methods.hpp"

template<class T, class Ptr = List<T>*>
struct ListWrapper {
    static_assert(sizeof(Ptr) == sizeof(void*), "Size of Ptr type must be the same as a void*!");
    
    // TODO: Consider requirementally constexpr-ifying this call
    // TODO: Apply these il2cpp conversion changes to ArrayW as well, to permit ArrayW to hold wrapper types and not pure pointers
    constexpr ListWrapper(Ptr const& p) noexcept : ptr(p) {}
    
    template<class V = void>
    requires (std::is_pointer_v<Ptr> && !il2cpp_utils::has_il2cpp_conversion<Ptr>)
    constexpr ListWrapper(void* alterInit) noexcept : ptr(reinterpret_cast<Ptr>(alterInit)) {}

    constexpr ListWrapper(std::span<T> const p) : ptr(il2cpp_utils::NewSpecific<Ptr>(p.size())) {
        std::copy(p.begin(), p.end(), begin());
    }

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;

    using iterator = pointer;
    using const_iterator = const_pointer;

    [[nodiscard]] constexpr int size() const {
        return ptr->size;
    }
    T& operator[](size_t i) {
        return ptr->items->values[i];
    }
    const T& operator[](size_t i) const {
        return ptr->items->values[i];
    }

    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The reference to the item.
    T& get(size_t i) {
        THROW_UNLESS(i < size() && i >= 0);
        return ptr->items->values[i];
    }
    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The const reference to the item.
    const T& get(size_t i) const {
        THROW_UNLESS(i < size() && i >= 0);
        return ptr->items->values[i];
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<T> to the item, mostly considered to be a T&.
    std::optional<WrapperRef<T>> try_get(size_t i) {
        if (i >= size() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(ptr->items->values[i]);
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<const T> to the item, mostly considered to be a const T&.
    std::optional<WrapperRef<const T>> try_get(size_t i) const {
        if (i >= size() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(ptr->items->values[i]);
    }

    iterator begin() {
        return ptr->items->values;
    }
    iterator end() {
        return &ptr->items->values[size()];
    }
    const_iterator begin() const {
        return ptr->items->values;
    }
    const_iterator end() const {
        return &ptr->items->values[size()];
    }

    operator std::span<T const> const() const {
        return std::span<T const>(this->begin(), this->end());
    }

    operator std::span<T>() {
        return std::span<T>(this->begin(), this->end());
    }

    operator Ptr() noexcept {
        return ptr;
    };

    constexpr void* convert() const noexcept {
        if constexpr (std::is_pointer_v<Ptr>) {
            return ptr;
        } else if constexpr (il2cpp_utils::has_il2cpp_conversion<Ptr>) {
            return ptr.convert();
        }
    }

    Ptr operator ->() noexcept {
        return ptr;
    }

    Ptr const operator ->() const noexcept {
        return ptr;
    }

    private:
    Ptr ptr;
};

// ListW for the win, just implicitly
template<class T, class Ptr = List<T>*>
using ListW = ListWrapper<T, Ptr>;

static_assert(il2cpp_utils::has_il2cpp_conversion<ListWrapper<int, List<int>*>>);
template<class T, class Ptr>
struct ::il2cpp_utils::il2cpp_type_check::need_box<ListWrapper<T, Ptr>> {
    constexpr static bool value = false;
};