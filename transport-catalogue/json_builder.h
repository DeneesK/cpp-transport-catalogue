#pragma once

#include "json.h"

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace json {
    class ItemContext;
    class DictItemContext;
    class ArrayItemContext;
    class KeyItemContext;

    class Builder {
    public:
        Builder() = default;
        KeyItemContext Key(const std::string& key);
        Builder& Value(const json::Node& value);
        DictItemContext StartDict();
        Builder& EndDict();
        ArrayItemContext StartArray();
        Builder& EndArray();
        json::Node Build();
    private:
        json::Node root_;
        std::vector<Node*> nodes_stack_;
        std::optional<std::string> key_;
    };

    class ItemContext {
    public:
        ItemContext(Builder& builder): builder_(builder) {
        }
    protected:
        Builder& Get() {
            return builder_;
        }
    private:
        Builder& builder_;
    };

    class DictItemContext: public ItemContext {
    public:
        DictItemContext(Builder& builder): ItemContext(builder) {
        }

        Builder& EndDict();
        KeyItemContext Key(std::string&& value);
    };

    class ArrayItemContext : public ItemContext {
    public:
        ArrayItemContext(Builder& builder): ItemContext(builder) {
        }

        ArrayItemContext Value(json::Node&& value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndArray();
    };

    class KeyItemContext : public ItemContext {
    public:
        KeyItemContext(Builder& builder): ItemContext(builder) {
        }

        DictItemContext Value(json::Node&& value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
    };
} 