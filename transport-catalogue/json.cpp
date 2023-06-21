#include "json.h"

using namespace std;

namespace json {

namespace {

EscapeC escape_c = {{'\r', "\\r"}, {'\n', "\\n"},  {'\"', "\\\""}, {'\\', "\\\\"}};

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    bool fail = true;
    for (char c; input >> c;) {
        if(c == ']') {
            fail = false;
            break;
        }
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));    
    }

    if(fail) {
        throw ParsingError("load array erro"s);
    }    
    return Node(std::move(result));
}

Number LoadNumber(istream& input) {
    using namespace literals;

    string parsed_num;

    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadDigit(istream& input)  {
    auto result = LoadNumber(input);
    if (holds_alternative<int>(result)) {
        return Node(get<int>(result));
    }
    return Node(get<double>(result));
}

string LoadS(istream& input) {
    using namespace literals;
    auto it = istreambuf_iterator<char>(input);
    auto end = istreambuf_iterator<char>();
    string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }
    return s;
}

Node LoadString(istream& input) {
    return Node(std::move(LoadS(input)));
}

Node LoadNull(istream& input) {
    int i = 0;
    string s;
    while(i < 4) {
            s.push_back(input.get());
            ++i;
        }
    if(s != "null") {
            throw ParsingError("load bool error"s);
        }
    return Node(nullptr);
}

Node LoadDict(istream& input) {
    Dict result;
    bool fail = true;
    for (char c; input >> c;) {
        if(c == '}') {
            fail = false;
            break;
        }
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({std::move(key), LoadNode(input)});
    }
    if(fail) {
        throw ParsingError("load dict error"s);
    }    
    return Node(std::move(result));
}

Node LoadBool(istream& input) {
    int i = 0;
    ostringstream out;
    if(input.peek() == 't') {
        while(i < 4) {
            out << char(input.get());
            ++i;
        }
        if(out.str() != "true") {
            throw ParsingError("load bool error"s);
        }
        return Node(true);
    }
    while(i < 5) {
        out << char(input.get());
        ++i;
        }
    if(out.str() != "false") {
        throw ParsingError("load bool error"s);
        }
    return Node(false);
}

Node LoadNode(istream& input) {
    string check = "0123456789-";
    char c;
    input >> c;
    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == 'n') {
        input.putback(c);
        return LoadNull(input);
    } else if (check.find(c) != string::npos) {
        input.putback(c);
        return LoadDigit(input);
    } else if(c != ']' && c != '}') {
        input.putback(c);
        return LoadBool(input);        
    }
    throw ParsingError("Unexpected start of line"s);
}

}  // namespace

Node::Node(Array array): value_(array) {}
Node::Node(): value_(nullptr) {}
Node::Node(nullptr_t): value_(nullptr) {}
Node::Node(Dict map): value_(map) {}
Node::Node(int value): value_(value) {}
Node::Node(double value): value_(value) {}
Node::Node(string value): value_(value) {}
Node::Node(bool value): value_(value) {}

const Node::Value& Node::GetValue() const { 
    return value_; 
}

const Array& Node::AsArray() const {
    if (holds_alternative<Array>(value_)) {
        return get<Array>(value_);
    }
    throw logic_error("wrong value type");
}

const Dict& Node::AsMap() const {
    if (holds_alternative<Dict>(value_)) {
        return get<Dict>(value_);
    }
    throw logic_error("wrong value type");
}

int Node::AsInt() const {
    if (holds_alternative<int>(value_)) {
        return get<int>(value_);
    }
    throw logic_error("wrong value type");
}

double Node::AsDouble() const {
    if (holds_alternative<double>(value_)) {
        return get<double>(value_);
    } else if (holds_alternative<int>(value_)) {
        return static_cast<double>(get<int>(value_));
    }
    throw logic_error("wrong value type");
}

bool Node::AsBool() const {
    if (holds_alternative<bool>(value_)) {
        return get<bool>(value_);
    }
    throw logic_error("wrong value type");
}

const string& Node::AsString() const {
    if (holds_alternative<string>(value_)) {
        return get<string>(value_);
    }
    throw logic_error("wrong value type");
}

bool Node::IsInt() const {
    if (holds_alternative<int>(value_)) {
        return true;
    }
    return false;  
}

bool Node::IsDouble() const {
    if (holds_alternative<double>(value_) || holds_alternative<int>(value_)) {
        return true;
    }
    return false;     
}

bool Node::IsPureDouble() const {
    if (holds_alternative<double>(value_)) {
        return true;
    }
    return false;       
}

bool Node::IsBool() const {
    if (holds_alternative<bool>(value_)) {
        return true;
    }
    return false;   
}

bool Node::IsString() const {
    if (holds_alternative<string>(value_)) {
        return true;
    }
    return false;   
}

bool Node::IsNull() const {
    if (holds_alternative<nullptr_t>(value_)) {
        return true;
    }
    return false;   
}

bool Node::IsArray() const {
    if (holds_alternative<Array>(value_)) {
        return true;
    }
    return false;   
}

bool Node::IsMap() const {
    if (holds_alternative<Dict>(value_)) {
        return true;
    }
    return false;   
}

Document::Document(Node root)
    : root_(std::move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, ostream& output) {
    PrintNode(doc.GetRoot(), output);
}

void PrintValue(nullptr_t, ostream& out) {
    out << "null"sv;
}

// "hello\nworld" 
void PrintValue(const string& s, ostream& out) {
    out << "\"" ;
    for(char c: s) {
        if(escape_c.count(c)) {
            out << escape_c.at(c);
        } else {
            out << c;
        }
    }
    out << "\"";
}

void PrintValue(bool stmt, ostream& out) {
    if(stmt) {
        out << "true";
        return;
    }
    out << "false";
}

void PrintValue(const Array& array, ostream& out) {
    out << "[";
    bool is_first = true;
    for(auto& elem: array) {
        if(is_first) {
            PrintNode(elem, out);
            is_first = false;
        } else {
            out << ", ";
            PrintNode(elem, out);
        }
    }
    out << "]";
}

void PrintValue(const Dict& dict, ostream& out) {
    out << "{";
    bool is_first = true;
    for(auto& [k, v]: dict) {
        if(is_first) {
            PrintValue(k, out);
            out << ": ";
            PrintNode(v, out);
            is_first = false;
        } else {
            out << ", ";
            PrintValue(k, out);
            out << ": ";
            PrintNode(v, out);
        }
    }
    out << "}";
}

void PrintValue(const int& i, ostream& out) {
    out << i;
}
void PrintValue(const double& d, ostream& out) {
    out << d;
}

void PrintNode(const Node& node, ostream& out) {
    visit(
        [&out](const auto& value){ PrintValue(value, out); },
        node.GetValue());
}

bool Node::operator==(const Node& other) const {
    return GetValue() == other.GetValue();
}
bool Node::operator!=(const Node& other) const {
    return GetValue() != other.GetValue();
}
}  // namespace json