#pragma once

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using EscapeC = std::map<char, std::string>;
using Number = std::variant<int, double>;
using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node: public Value {
public:
    // Огромное спасибо, за подсказку, так намного лучше и еще лучше разобрался в наследовании!
    using Value::variant;

    const Node& GetValue() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    double AsDouble() const;
    bool AsBool() const;
    const std::string& AsString() const;

    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

void PrintValue(std::nullptr_t, std::ostream& out);
void PrintValue(bool stmt, std::ostream& out);
void PrintValue(const Array& array, std::ostream& out);
void PrintValue(const Dict& dict, std::ostream& out);
void PrintValue(const std::string& s, std::ostream& out);
void PrintValue(const int& i, std::ostream& out);
void PrintValue(const double& d, std::ostream& out);
void PrintNode(const Node& node, std::ostream& out);

std::string LoadS(std::istream& input);
Number LoadNumber(std::istream& input);
}  // namespace json