#pragma once

#include <optional>
#include <utility>

using std::string_view;

class Spliterator {
protected: 
    string_view const m_src;
    char m_split;
    bool m_is_terminated;
    size_t m_seek_pos = 0;
    
public:
    Spliterator(string_view const src, char split, bool is_terminated)
        : m_src(src), m_split(split), m_is_terminated(is_terminated)
    {}
    
    std::optional<string_view> next() {
        if (m_seek_pos == string_view::npos) {
            return std::nullopt;
        }
        auto next_pos = m_src.find(m_split, m_seek_pos);
        // split character found
        if (next_pos != string_view::npos) {
            auto substr = m_src.substr(m_seek_pos, next_pos-m_seek_pos);
            m_seek_pos = next_pos + 1;
            return substr;
        }
        // split character not found, last element is not terminated
        else if (!m_is_terminated) {
            auto substr = m_src.substr(m_seek_pos);
            m_seek_pos = string_view::npos;
            return substr;
        }
        // split character not found, last element is terminated
        else {
            return std::nullopt;
        }
    }
    
    std::optional<std::pair<string_view, string_view>> next_pair() {
        auto first = next();
        if (!first.has_value()) {
            return std::nullopt;
        }
        auto second = next();
        if (!second.has_value()) {
            return std::nullopt;
        }
        return std::make_pair(*first, *second);
    }
};
