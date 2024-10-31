#include "RyJson.h"
#include <cctype>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ranges>
#include <sstream>
#define USE_EXCEPTION _DEBUG

#pragma warning(disable:4715)
#pragma warning(disable:4267)
#pragma warning(disable:4244)

namespace ry
{
class Json::Parser
{
public:
    explicit Parser(const String& str, std::function<void(int)> parseProgressChanged = {})
        : _str{str}
        , _cursor{0}
        , _parseProgressChanged(std::move(parseProgressChanged))
    { }

    ~Parser() = default;

    Json parse()
    {
        const int progress = float(_cursor + 1) / float(_str.length()) * 1000;
        if (progress != _progress) {
            _progress = progress;
            if (_parseProgressChanged) {
                _parseProgressChanged(_progress);
            }
            // qDebug() << "json parse progress: " << progress / 10;
        }
        const auto ch = getNextToken();
        if ('n' == ch) {
            --_cursor;
            return getNull();
        }
        if (ch == 't' || ch == 'f') {
            --_cursor;
            return getBool();
        }
        if ('-' == ch || std::isdigit(ch)) {
            --_cursor;
            return getNumber();
        }
        if ('[' == ch) {
            return getArray();
        }
        if ('{' == ch) {
            return getObject();
        }
        if ('"' == ch) {
            return getString();
        }
        throw std::logic_error("unexpected token: " + String(ch + "") + "arround: " + std::to_string(_cursor));
    }

private:
    void skipWhiteSpace()
    {
        if (_cursor < _str.length()) {
            auto ch = _str.at(_cursor);
            while (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
                _cursor++;
                ch = _str.at(_cursor);
            }
        }
    }

    char getNextToken()
    {
        skipWhiteSpace();
        if (_cursor == _str.size()) {
            throw std::logic_error("check format\n");
        }
        return _str.at(_cursor++); // std::string [_cursor++] ;
    }

    Json getNull()
    {
        // if (_str == "null") {
        // if (_str.mid(_cursor, 4) == "null") {
        if (_str.compare(_cursor, 4, "null") == 0) {
            _cursor += 4;
            return {};
        }
        throw std::logic_error("parse null error");
    }

    Json getBool()
    {
        if (0 == _str.compare(_cursor, 4, "true")) {
            _cursor += 4;
            return true;
        }
        if (0 == _str.compare(_cursor, 5, "false")) {
            _cursor += 5;
            return false;
        }
        throw std::logic_error(("expected 'true' or 'false' given: " + _str.substr(_cursor, 4)));
    }

    Json getNumber()
    {
        const auto start = _cursor;

        // Check for special floating-point values
        if (_str.compare(_cursor, 3, "inf") == 0 || _str.compare(_cursor, 8, "infinity") == 0) {
            _cursor += (_str[_cursor + 3] == 'i') ? 8 : 3;
            return 0;
        }
        if (_str.compare(_cursor, 3, "nan") == 0 || _str.compare(_cursor, 8, "-nan(ind)") == 0) {
            _cursor += (_str[_cursor + 3] == 'n') ? 8 : 3;
            return 0;
        }

        if (_str[_cursor] == '-') {
            _cursor++;
        }

        // integer part
        if (_str[_cursor] == '0') {
            _cursor++;
        }
        else if (_str[_cursor] >= '1' && _str[_cursor] <= '9') {
            _cursor++;
            while (_str[_cursor] >= '0' && _str[_cursor] <= '9') {
                _cursor++;
            }
        }
        else {
            throw std::logic_error("invalid character in number");
        }
        const bool hasExp = (_str[_cursor] == 'e' || _str[_cursor] == 'E');
        if (_str[_cursor] != '.' && !hasExp) {
            return std::strtoll(_str.c_str() + start, nullptr, 10);
        }
        // decimal part
        if (!hasExp) {
            _cursor++;
            while (std::isdigit(_str[_cursor])) {
                _cursor++;
            }
        }
        parseExponentPart();
        // return _str.mid(start, _cursor - start).toDouble();
        return std::strtod(_str.c_str() + start, nullptr);
    }

    void parseExponentPart()
    {
        if (_str[_cursor] != 'e' && _str[_cursor] != 'E') {
            return;
        }

        _cursor++;
        // Check for an optional sign.
        if (_str[_cursor] == '+' || _str[_cursor] == '-') {
            _cursor++;
        }

        // We must have a digit. Otherwise, we have an error.
        if (!std::isdigit(_str[_cursor])) {
            throw std::logic_error("at least one digit required in fractional part");
        }
        while (!std::isdigit(_str[_cursor])) {
            _cursor++;
        }
    }

    String getString()
    {
        const auto start = _cursor;
        while (true) {
            if (_cursor == _str.size()) {
                throw std::logic_error("unexpected end of input in string");
            }

            auto ch = _str[_cursor++];
            if (ch == '"') {
                break;
            }

            // The usual case: non-escaped characters
            if (ch == '\\') {
                ch = _str[_cursor++];
                switch (ch) {
                    case 'b':
                    case 't':
                    case 'n':
                    case 'f':
                    case 'r':
                    case '"':
                    case '\\': break;
                    case 'u': _cursor += 4; break;
                    default: break;
                }
            }
        }
        return _str.substr(start, _cursor - start - 1);
    }

    Json getArray()
    {
        Json arr(Type::Array);
        auto ch = getNextToken();
        if (ch == ']') {
            return arr;
        }
        _cursor--;
        while (true) {
            arr.push_back(parse());
            ch = getNextToken();
            if (ch == ']') {
                break;
            }
            if (ch != ',') {
                throw std::logic_error("expected ',' in array");
            }
        }
        return arr;
    }

    Json getObject()
    {
        Json obj(Type::Object);
        auto ch = getNextToken();
        if (ch == '}') {
            return obj;
        }
        _cursor--;
        while (true) {
            ch = getNextToken();
            if (ch != '"') {
                throw std::logic_error("expected '\"' in object");
            }
            String key = getString();
            ch         = getNextToken();
            if (ch != ':') {
                throw std::logic_error("expected ':' in object");
            }
            obj[key] = parse();
            ch       = getNextToken();
            if (ch == '}') {
                break;
            }
            if (ch != ',') {
                throw std::logic_error("expected ',' in object");
            }
        }
        return obj;
    }

    // match
    // peek            获取游标处字符
    // peekNext        下一个字符
    // advance()       获取游标处字符，且游标后移
    // previous        游标前一个字符
    // match(...)      这个检查会判断当前的标记是否属于给定的类型之一
private:
    const String& _str;
    int _cursor;
    int _progress = 0;
    std::function<void(int)> _parseProgressChanged;
};

Json::Json(Type t)
    : type_(t)
{
    switch (t) {
        case Type::Null: break;
        case Type::Bool: {
            *this = Json(t, false);
            // 原地构造
            // this->type_ = t;
            // this->data_ = std::make_unique<Data>(false);
        } break;
        case Type::Int: {
            *this = Json(t, 0ll);
        } break;
        case Type::Double: {
            *this = Json(t, .0);
        } break;
        case Type::String: {
            *this = Json(t, "");
        } break;
        case Type::Array: {
            *this = Json(t, Array());
        } break;
        case Type::Object: {
            *this = Json(t, Object());
        } break;
    }
}

Json::Json(Type t, const Data& d)
    : type_(t)
    , data_(d)
{
    // type != d.type()?
    // data_ = std::make_unique<Data>(d);
}

Json::Json(const std::vector<std::string>& stringList)
{
    *this = Json::array();
    for (const auto& str : stringList) {
        push_back(str);
    }
}

Json::Json(const Json& json)
{
    // 调用右值拷贝构造, swap(*this, Json(json));
    *this = json;
}

Json Json::parseFile(std::string_view filename, int* errorCode, std::function<void(int)> progressCallback)
{
    try {
        Json result;
        std::ifstream file(std::string(filename), std::ios::binary);
        if (!file.is_open()) {
            if (errorCode) {
                *errorCode = -1; // 错误代码，表示文件无法打开
            }
            return result;
        }

        // 读取文件到字符串
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // 检查并移除UTF-8 BOM（EF BB BF）
        if (fileContent.size() >= 3 && fileContent[0] == '\xEF' && fileContent[1] == '\xBB' && fileContent[2] == '\xBF') {
            fileContent.erase(0, 3); // 移除前3个字节
        }
        return Json::parse(fileContent, errorCode, progressCallback);
    }
    catch (...) {
    }
    return {};
}

bool Json::saveToFile(std::string_view filename, int* errorCode) const
{
    try {
        std::ofstream file(filename.data());
        file << dump();
        return true;
    }
    catch (...) {
    }
    return false;
}

void Json::set(std::string_view key, const Json& other)
{
    if (!isObject()) {
        *this = Json(Type::Object);
    }
    this->operator[](key.data()) = other;
}

Json Json::parse(const String& str, int* errorCode, std::function<void(int)> progressCallback)
{
    if (str.empty()) {
        return Json();
    }
#if USE_EXCEPTION
    try {
#endif
        return Parser(str, progressCallback).parse();
#if USE_EXCEPTION
    }
    catch (std::exception& e) {
        std::cout << "json parse error" << e.what() << '\n';
    }
    catch (...) {
    }
#endif
    return {};
}

Json::String Json::dump(bool compact) const
{
#if USE_EXCEPTION
    try {
#endif
        switch (type_) {
            case Type::Null: {
                return "null";
            }
            case Type::Bool: {
                if (toBool()) {
                    return "true";
                }
                return "false";
            }
            case Type::Int: {
                // return String::number(toInt());
                return std::to_string(toInt()).c_str();
            }
            case Type::Double: {
                // return String::number(toDouble());
                return std::to_string(toDouble()).c_str();
            }
            case Type::String: {
                return "\"" + toString() + "\"";
            }
            case Type::Array: {
                const auto d = std::get_if<Array>(&data_);
                if (!d) {
                    return {};
                }
                String str = "[";
                for (int i = 0; i < d->size(); ++i) {
                    str += d->at(i).dump() + ((i == d->size() - 1) ? "]" : ",");
                }
                if (!str.ends_with(']')) {
                    str += ']';
                }
                return str;
            }
            case Type::Object: {
                const auto d = std::get_if<Object>(&data_);
                if (!d) {
                    return {};
                }
                String str = "{";
                for (auto it = d->begin(); it != d->end(); ++it) {
                    const auto& [key, value] = *it;
                    str += (it == d->begin() ? "" : ",") + ("\"" + key + "\":" + value.dump());
                }
                str += "}";
                return str;
            }
        }
#if USE_EXCEPTION
    }
    catch (std::exception& e) {
        std::cout << "json parse error" << e.what();
    }
    catch (...) {
    }
#endif
    return {};
}

bool Json::toBool(const bool defaultValue) const
{
    if (isNull()) {
        return false;
    }
    if (const auto d = std::get_if<bool>(&data_)) {
        return *d;
    }
    if (isArray() || isObject() || isString()) {
        return !empty();
    }
    if (isInt()) {
        return static_cast<bool>(toInt());
    }
    if (isDouble()) {
        return fabs(toDouble() - DBL_EPSILON) != 0;
    }
    return defaultValue;
}

Json::Int Json::toInt(Int defaultValue) const
{
    if (isDouble()) {
        return static_cast<Int>(toDouble());
    }
    if (const auto d = std::get_if<Int>(&data_)) {
        return *d;
    }
    if (isString()) {
        try {
            return std::stoll(toString());
        }
        catch (...) {
        }
    }
    return 0;
}

int32_t Json::toInt32(int32_t defaultValue) const
{
    return static_cast<int32_t>(toInt(defaultValue));
}

double Json::toDouble(const double defaultValue) const
{
    if (isInt()) {
        return static_cast<double>(toInt());
    }
    if (const auto d = std::get_if<double>(&data_)) {
        if (isinf(*d) || isnan(*d)) {
            return 0;
        }
        return *d;
    }
    if (isString()) {
        try {
            return std::stod(toString());
        }
        catch (...) {
        }
    }
    return defaultValue;
}

Json::String Json::toString(const String& defaultValue) const
{
    if (const auto d = std::get_if<String>(&data_)) {
        if (*d == R"("")") {
            return "";
        }

        // 对字符串中的特殊字符进行转义，并移除 tab 符号
        std::string processedString;
        for (char c : *d) {
            switch (c) {
                case '"': processedString += R"(\")"; break;
                case '\\': processedString += R"(\\)"; break;
                // case '\n': processedString += R"(\n)"; break;
                case '\r': processedString += R"(\r)"; break;
                case '\t': break; // 移除 tab 符号
                case '\b': processedString += R"(\b)"; break;
                case '\f': processedString += R"(\f)"; break;
                default: processedString += c; break;
            }
        }

        return processedString;
    }
    if (isBool()) {
        return toBool() ? "true" : "false";
    }
    if (isInt()) {
        return std::to_string(toInt());
    }
    if (isDouble()) {
        return std::to_string(toDouble());
    }
    return defaultValue;
}

Json::Array Json::toArray(const Array& defaultValue) const
{
    if (const auto d = std::get_if<Array>(&data_)) {
        return *d;
    }
    return defaultValue;
}

Json::Object Json::toObject(const Object& defaultValue) const
{
    if (const auto d = std::get_if<Object>(&data_)) {
        return *d;
    }
    return defaultValue;
}

std::vector<Json::String> Json::objectKeys() const
{
    if (isObject()) {
        if (const auto d = std::get_if<Object>(&data_)) {
            return *d | std::views::keys | std::ranges::to<std::vector<String>>();
        }
    }
    return {};
}

bool Json::operator==(const Json& other) const
{
    return (type_ == other.type_ && this->data_ == other.data_);
}

bool Json::operator==(const Int other) const
{
    if (!isInt() or !isDouble()) {
        return false;
    }
    return this->toInt() == other;
}
bool Json::operator==(const int other) const
{
    if (!isInt() and !isDouble()) {
        return false;
    }
    return toInt() == other;
}
bool Json::operator==(const double other) const
{
    if (!isInt() and !isDouble()) {
        return false;
    }
    return fabs(toDouble() - other) <= DBL_EPSILON;
}

bool Json::operator==(const String& other) const
{
    return (isString() && this->toString() == other);
}

bool Json::operator!=(const Json& other) const
{
    return !(*this == other);
}

Json& Json::operator/=(double value)
{
    if (isDouble()) {
        *this = toDouble() / value;
    }
    return *this;
}

bool Json::push_back(const Json& other)
{
    if (Type::Null == type_) {
        *this = Json::array();
    }
    if (type_ == Type::Array) {
        if (const auto d = std::get_if<Array>(&data_)) {
            d->push_back(other);
            return true;
        }
    }
    return false;
}

bool Json::push_back(Json&& other)
{
    if (Type::Null == type_) {
        *this = Json(Type::Array);
    }
    if (type_ == Type::Array) {
        if (const auto d = std::get_if<Array>(&data_)) {
            d->push_back(std::move(other));
            return true;
        }
    }
    return false;
}

bool Json::remove(const String& key)
{
    if (type_ == Type::Object) {
        if (const auto d = std::get_if<Object>(&data_)) {
            d->erase(key);
            // d->remove(key);
            return true;
        }
    }
    return false;
}

bool Json::remove(const int i)
{
    if (type_ == Type::Array) {
        if (const auto d = std::get_if<Array>(&data_)) {
            d->erase(d->begin() + i);
            return true;
        }
    }
    return false;
}

void Json::clear()
{
    if (isArray()) {
        if (const auto d = std::get_if<Array>(&data_)) {
            Array().swap(*d);
        }
    }
    if (isObject()) {
        if (const auto d = std::get_if<Object>(&data_)) {
            Object().swap(*d);
        }
    }
    *this = Json();
}

Json& Json::operator[](const String& key)
{
    if (type_ != Type::Object) {
        *this = Json::object();
    }
    const auto d = std::get_if<Object>(&data_);
    return (*d)[key];
}

Json& Json::operator[](const std::string_view& key)
{
    return this->operator[](key.data());
}

Json& Json::operator[](const char* key)
{
    return this->operator[](String(key));
}

Json& Json::operator[](const int i)
{
    return this->operator[](static_cast<size_t>(i));
}

Json& Json::operator[](const size_t i)
{
    if (type_ == Type::Null) {
        *this = Json::array();
    }
    const auto d = std::get_if<Array>(&data_);
    if (i < 0) {
        throw std::runtime_error("index out of range !");
    }
    if (i >= d->size()) {
        d->resize(i + 1);
    }
    return (*d)[i];
}

Json Json::operator[](const char* key) const
{
    return value(key);
}

Json Json::at(const size_t i) const
{
    const auto d = std::get_if<Array>(&data_);
    if (!d) {
        throwError("not an array !");
    }

    if (i < 0 || i > d->size()) {
        // throw std::runtime_error("index out of range !");
        throwError("index out of range !");
    }
    return d->at(i);
}

Json Json::at(std::string_view key) const
{
    return this->operator[](key.data());
}

Json::operator bool() const
{
    return toBool();
}

Json::operator double() const
{
    return toDouble();
}

Json::operator Int() const
{
    return toInt();
}

Json::operator unsigned long long() const
{
    return toInt();
}

Json::operator int() const
{
    return static_cast<int>(toInt());
}

Json::operator String() const
{
    return toString();
}
#ifdef _RY_USE_QT_
operator QString() const
{
    return QString::fromStdString(toString());
}
operator QByteArray() const
{
    return QString::fromStdString(toString()).toUtf8();
}
#endif

// Json::operator QByteArray() const
//{
//     return toString().toLocal8Bit();
// }

Json::iterator Json::begin() noexcept
{
    if (isArray()) {
        if (const auto b = std::get_if<Array>(&data_)) {
            return b->begin();
        }
    }
    if (isObject()) {
        if (const auto b = std::get_if<Object>(&data_)) {
            return b->begin();
        }
    }
    throwError("The type does not support iterator");
    // return iterator();
    // return {};
}

Json::iterator Json::end() noexcept
{
    if (isArray()) {
        if (const auto b = std::get_if<Array>(&data_)) {
            return b->end();
        }
    }
    if (isObject()) {
        if (const auto b = std::get_if<Object>(&data_)) {
            return b->end();
        }
    }
    // return iterator();
    throwError("The not support iterator");
}

Json::const_iterator Json::const_begin() const noexcept
{
    if (isArray()) {
        if (const auto b = std::get_if<Array>(&data_)) {
            return b->cbegin();
        }
    }
    if (isObject()) {
        if (const auto b = std::get_if<Object>(&data_)) {
            return b->cbegin();
        }
    }
    throwError("The not support iterator");
}

Json::const_iterator Json::const_end() const noexcept
{
    if (isArray()) {
        if (const auto b = std::get_if<Array>(&data_)) {
            return b->cend();
        }
    }
    if (isObject()) {
        if (const auto b = std::get_if<Object>(&data_)) {
            return b->cend();
        }
    }
    throwError("The not support iterator");
}

size_t Json::size() const noexcept
{
    if (isString()) {
        if (const auto d = std::get_if<String>(&data_)) {
            return d->size();
        }
    }
    if (isArray()) {
        if (const auto d = std::get_if<Array>(&data_)) {
            return d->size();
        }
    }
    if (isObject()) {
        if (const auto d = std::get_if<Object>(&data_)) {
            return d->size();
        }
    }
    return 0;
}

void Json::setParseProgressChanged(const std::function<void(int)>& callback)
{
    _parseProgressChanged = callback;
}

Json Json::map(const std::function<Json(const Json& v)>& f) const
{
    if (isArray()) {
        if (const auto d = std::get_if<Array>(&data_)) {
            // return *d | std::ranges::views::transform(f) | std::ranges::to<Array>();
            Array result;
            result.reserve(d->size());
            std::ranges::transform(*d, std::back_inserter(result), f);
            return result;
        }
    }
    return {Type::Null};
}

Json Json::map(const std::function<Json(const Json& v, size_t index)>& f) const
{
    if (isArray()) {
        if (const auto* d = std::get_if<Array>(&data_)) {
            // return *d | std::ranges::views::transform(f) | std::ranges::to<Array>();
            Array result;
            result.reserve(d->size());
            for (size_t i = 0; i < d->size(); ++i) {
                result.push_back(f(d->at(i), i));
            }
            return result;
        }
    }
    return {Type::Null};
}

Json Json::filter(const std::function<bool(const Json& v)>& f) const
{
    if (isArray()) {
        if (const auto d = std::get_if<Array>(&data_)) {
            // return *d | std::ranges::views::transform(f) | std::ranges::to<Array>();
            Array result;
            std::ranges::copy_if(*d, std::back_inserter(result), f);
            return result;
        }
    }
    return {Type::Null};
}

void Json::reserve(size_t s)
{
    if (const auto d = std::get_if<Array>(&data_)) {
        d->reserve(s);
    }
}

Json& Json::insert(const String& key, const Json& value)
{
    if (type_ == Type::Null) {
        *this = Json::object();
    }
    if (type_ == Type::Object) {
        if (const auto d = std::get_if<Object>(&data_)) {
            (*d)[key] = value;
            return *this;
        }
    }
    return *this;
}

void Json::throwError(std::string_view message)
{
    throw std::runtime_error(message.data());
    // #if USE_EXCEPTION
    //     try {
    // #endif
    //         throw std::exception(message.data());
    // #if USE_EXCEPTION
    //     }
    //     catch (std::exception& e) {
    //         std::cout << "json exception: " << e.what();
    //     }
    //     catch (...) {
    //     }
    // #endif
    //     return {};
}
} // namespace ry
