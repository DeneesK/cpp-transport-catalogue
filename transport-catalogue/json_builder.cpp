#include "json_builder.h"

namespace json {
    DictItemContext KeyItemContext::Value(json::Node&& value) {
        auto& builder = Get();
        builder.Value(std::move(value));
        return DictItemContext{builder};
    }

    ArrayItemContext ItemContext::StartArray() {
         auto& builder = Get();
         builder.StartArray();
         return ArrayItemContext{builder};       
    }

    DictItemContext ItemContext::StartDict() {
         auto& builder = Get();
         builder.StartDict();
         return DictItemContext{builder};       
    }

    ArrayItemContext ArrayItemContext::Value(json::Node&& value) {
        auto& builder = Get();
        builder.Value(std::move(value));
        return ArrayItemContext{builder};           
    }

    Builder& ItemContext::EndArray() {
         auto& builder = Get();
         builder.EndArray();
         return builder;              
    }

    Builder& ItemContext::EndDict() {
         auto& builder = Get();
         builder.EndDict();
         return builder;             
    }
    KeyItemContext ItemContext::Key(std::string&& value) {
         auto& builder = Get();
         builder.Key(std::move(value));
         return KeyItemContext{builder};   
    }

    KeyItemContext Builder::Key(const std::string& key) {
        if(nodes_stack_.empty() || key_.has_value() || !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("unable to add a key");
        }
        key_ = std::move(key);
        return KeyItemContext{*this};
    }
    Builder& Builder::Value(const json::Node& value) {
        if(!key_.has_value() && !nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {
            throw std::logic_error("unable to add a value");
        }
        if(nodes_stack_.empty() && !root_.IsNull()) {
            throw std::logic_error("unable to add a value");
        }
        if(nodes_stack_.empty() && root_.IsNull()) {
            root_ = std::move(value);
        } else if(key_.has_value()) {
            auto& dict = const_cast<json::Dict&>(nodes_stack_.back()->AsDict());
            dict.emplace(key_.value(), value);
            key_ = std::nullopt;
        }
        if(!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            auto& arr = const_cast<json::Array&>(nodes_stack_.back()->AsArray());
            arr.emplace_back(value);
        }
        return *this;
    }
    DictItemContext Builder::StartDict() {
        if(nodes_stack_.empty() && root_.IsNull()) {
            root_ = json::Node{json::Dict{}};
            nodes_stack_.emplace_back(&root_);
        } else if (!nodes_stack_.empty() && (nodes_stack_.back()->IsArray())) {
            auto& arr = const_cast<json::Array&>(nodes_stack_.back()->AsArray());
            arr.emplace_back(json::Dict{});
            nodes_stack_.emplace_back(&arr.back());
        } else if(key_.has_value()) {
            auto& dict = const_cast<json::Dict&>(nodes_stack_.back()->AsDict());
            dict.emplace(key_.value(), json::Dict{});
            nodes_stack_.emplace_back(&dict.at(key_.value()));
            key_ = std::nullopt;
        } else {
            throw std::logic_error("unable to start a dict");
        }
        return DictItemContext{*this};
    }
    Builder& Builder::EndDict() {
        if(nodes_stack_.empty() || !nodes_stack_.back()->IsDict() || key_.has_value()) {
            throw std::logic_error("unable to end a dict");
        }
        nodes_stack_.pop_back();
        return *this;
    }
    ArrayItemContext Builder::StartArray() {
        if(nodes_stack_.empty() && root_ == nullptr) {
            root_ = json::Node{json::Array{}};
            nodes_stack_.emplace_back(&root_);
        } else if(key_.has_value()) {
            auto& dict = const_cast<json::Dict&>(nodes_stack_.back()->AsDict());
            dict.emplace(key_.value(), json::Array{});
            nodes_stack_.emplace_back(&dict.at(key_.value()));
            key_ = std::nullopt;           
        } else if(!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
            auto& arr = const_cast<json::Array&>(nodes_stack_.back()->AsArray());
            arr.emplace_back(json::Array{});
            nodes_stack_.emplace_back(&arr.back());
        } else {
            throw std::logic_error("unable to start an array");
        }
        return ArrayItemContext{*this};
    }
    Builder& Builder::EndArray() {
        if(nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
            throw std::logic_error("unable to end a array");
        }
        nodes_stack_.pop_back();
        return *this;
    }
    json::Node Builder::Build() {
        if(!nodes_stack_.empty()) {
            throw std::logic_error("Builder is not ready to build");
        } 
        if(root_.IsNull()) {
            throw std::logic_error("Builder is not ready to build");
        }
        return root_;
    }
}