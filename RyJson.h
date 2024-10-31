#pragma once
/**
 * @简介：封装json，使之更像js对象,
 * @author：rayzhang
 * @协作：
 * @其他
 * @性能优化：考虑减少内存拷贝和递归次数。
 * @todo:
 * 1. 使用解析器方案
 * 2. 支持更多类型
 * 3. 支持自定义类型
 */

#ifdef _RY_USE_QT_
#include <QDebug>
#include <QMap>
#include <QString>
#include <QVariant>
#endif
#include <algorithm>
#include <ranges>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#include <iostream>
#include <map>

namespace ry
{
// JsonValue
//[[deprecated("flow::Json is now deprecated, please use CIQTEK::Json instead.")]];
class Json final
{
    class Parser;
    class Token;

public:
    // token type
    enum class Type
    {
        Null,
        Bool,
        Int,
        Double,
        String,
        Array,
        Object
    };
    // 数据类型可以在这里改。
    using Null   = std::nullptr_t;
    using Bool   = bool;
    using Int    = int64_t;
    using Double = double;
    using String = std::string;
    using Array  = std::vector<Json>;
    // using Object = std::unordered_map<String, Json>; // QMap<QString,
    using Object = std::map<String, Json>; // QMap<QString,
    // using Data = QVariant;
    using Data = std::variant<Null, Bool, Double, Int, String, Array, Object>;
    // 兼容 nlohuman::json
    using parse_error = std::exception;

    class iterator
    {
    public:
        using ArrayIterator    = Array::iterator;
        using ObjectIterator   = Object::iterator;
        using iterator_concept = std::bidirectional_iterator_tag;
        iterator(const ArrayIterator& it)
            : it(std::in_place_type<ArrayIterator>, it)
            , _type(Type::Array)
        { }

        iterator(const ObjectIterator& it)
            : it(std::in_place_type<ObjectIterator>, it)
            , _type(Type::Object)
        { }

        Json& operator*() const
        {
            if (_type == Type::Object) {
                return (*std::get_if<ObjectIterator>(&it))->second;
            }
            return **std::get_if<ArrayIterator>(&it);
        }

        iterator& operator++()
        {
            if (_type == Type::Object) {
                ++(std::get<ObjectIterator>(it));
            }
            else {
                ++(std::get<ArrayIterator>(it));
            }
            return *this;
        }
        iterator& operator--()
        {
            if (_type == Type::Object) {
                --(std::get<ObjectIterator>(it));
            }
            else {
                --(std::get<ArrayIterator>(it));
            }
            return *this;
        }
        bool operator!=(const iterator& other) const
        {
            if (_type != other._type) {
                throw std::logic_error("Cannot compare iterators of different types");
            }
            if (_type == Type::Object) {
                return std::get<ObjectIterator>(it) != std::get<ObjectIterator>(other.it);
            }
            return std::get<ArrayIterator>(it) != std::get<ArrayIterator>(other.it);
        }

        size_t operator-(const iterator& other) const
        {
            if (_type != other._type) {
                throw std::logic_error("Cannot subtract iterators of different types");
            }
            if (_type == Type::Array) {
                return std::get<ArrayIterator>(it) - std::get<ArrayIterator>(other.it);
            }
            throw std::logic_error("Cannot subtract Object::iterator");
        }

        iterator operator++(int)
        {
            iterator old = *this;
            operator++();
            return old;
        }
        iterator operator--(int)
        {
            iterator old = *this;
            operator--();
            return old;
        }

    private:
        std::variant<ArrayIterator, ObjectIterator> it;
        Type _type{Type::Null};
    };
    // todo：实现const_iterator
    class const_iterator
    {
    public:
        using ArrayIterator  = Array::const_iterator;
        using ObjectIterator = Object::const_iterator;

        const_iterator(const ArrayIterator& it)
            : it(std::in_place_type<ArrayIterator>, it)
            , _type(Type::Array)
        { }

        const_iterator(const ObjectIterator& it)
            : it(std::in_place_type<ObjectIterator>, it)
            , _type(Type::Object)
        { }

        const Json& operator*() const
        {
            if (_type == Type::Object) {
                return (*std::get_if<ObjectIterator>(&it))->second;
            }
            return **std::get_if<ArrayIterator>(&it);
        }

        bool operator!=(const const_iterator& other) const
        {
            if (_type != other._type) {
                throw std::logic_error("Cannot compare iterators of different types");
            }
            if (_type == Type::Object) {
                return std::get<ObjectIterator>(it) != std::get<ObjectIterator>(other.it);
            }
            return std::get<ArrayIterator>(it) != std::get<ArrayIterator>(other.it);
        }

        size_t operator-(const const_iterator& other) const
        {
            if (_type != other._type) {
                throw std::logic_error("Cannot subtract iterators of different types");
            }
            if (_type == Type::Array) {
                return std::get<ArrayIterator>(it) - std::get<ArrayIterator>(other.it);
            }
            throw std::logic_error("Cannot subtract Object::iterator");
        }

        const_iterator& operator++()
        {
            if (_type == Type::Object) {
                ++(std::get<ObjectIterator>(it));
            }
            else {
                ++(std::get<ArrayIterator>(it));
            }
            return *this;
        }
        const_iterator& operator--()
        {
            if (_type == Type::Object) {
                --(std::get<ObjectIterator>(it));
            }
            else {
                --(std::get<ArrayIterator>(it));
            }
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator old = *this;
            operator++();
            return old;
        }
        const_iterator operator--(int)
        {
            const_iterator old = *this;
            operator--();
            return old;
        }

    private:
        std::variant<ArrayIterator, ObjectIterator> it;
        Type _type{Type::Null};
    };

    // 构造
    Json(Type t = Type::Null);
    Json(Type t, const Data& d);

    Json(Bool d)
        : Json(Type::Bool, d)
    { }

    Json(Int d)
        : Json(Type::Int, d)
    { }
    // todo:
    Json(size_t d)
        : Json(Type::Int, static_cast<int64_t>(d))
    { }

    Json(int d)
        : Json(Type::Int, static_cast<Int>(d))
    { }

    Json(Double d)
        : Json(Type::Double, d)
    { }

    Json(const Array& d)
        : Json(Type::Array, d)
    { }

    Json(const Object& d)
        : Json(Type::Object, d)
    { }

    Json(const std::string_view str)
        : Json(Type::String, str.data())
    { }

    Json(const String& str)
        : Json(Type::String, str)
    { }

    Json(const char* str)
        : Json(Type::String, str)
    { }
    // 扩展
    Json(const std::vector<std::string>& stringList);
    // 拷贝构造
    Json(const Json& d);
    ~Json() = default;
    // 移动构造
    Json(Json&& other) noexcept
        : type_(other.type_)
        , data_(std::move(other.data_))
    {
        other.type_ = Type::Null;
    }

    // 移动赋值
    Json& operator=(Json&& other) noexcept
    {
        swap(other);
        return *this;
    }

    Json& operator=(const size_t& value) { return *this = Json(static_cast<int64_t>(value)); }
    Json& operator=(const uint32_t& value) { return *this = Json(static_cast<int64_t>(value)); }
    // Json& operator=(const size_t& value) { return *this = Json(static_cast<Double>(value)); }
    Json& operator=(const int& value) { return *this = Json(value); }
    Json& operator=(const bool& value) { return *this = Json(value); }
    Json& operator=(const Int& value) { return *this = Json(value); }
    Json& operator=(const double& value) { return *this = Json(value); }
    // Json& operator=(const Bool& value) { return *this = Json(value); }
    Json& operator=(const char* str) { return *this = Json(str); }
    Json& operator=(const String& str) { return *this = Json(str); }
    Json& operator=(const Array& value) { return *this = Json(value); }
    Json& operator=(const Object& value) { return *this = Json(value); }
    Json& operator=(const Json& other) { return *this = Json(other.type(), other.data_); }
    void set(const Json& other) { *this = other; }
    void set(std::string_view key, const Json& other);
    // 助手函数
    static Json array() { return {Type::Array}; }
    static Json object() { return {Type::Object}; }
    // 解析
    static Json parse(const String& str, int* errorCode = nullptr, std::function<void(int)> progressCallback = {});
    static Json parseFile(std::string_view filename, int* errorCode = nullptr, std::function<void(int)> progressCallback = {});
    bool saveToFile(std::string_view filename, int* errorCode = nullptr) const;
    std::string stringify() const { return dump(); }
    String dump(bool compact = true) const; // 未实现格式化

    // 类型
    [[nodiscard]] Type type() const { return type_; }
    // 获取值
    [[nodiscard]] bool isNull() const { return type() == Type::Null; }
    [[nodiscard]] bool isBool() const { return type() == Type::Bool; }
    [[nodiscard]] bool isNumber() const { return type() == Type::Int || type() == Type::Double; }
    [[nodiscard]] bool isDouble() const { return type() == Type::Double; }
    [[nodiscard]] bool isInt() const { return type() == Type::Int; }
    [[nodiscard]] bool isString() const { return type() == Type::String; }
    [[nodiscard]] bool isArray() const { return type() == Type::Array; }
    [[nodiscard]] bool isObject() const { return type() == Type::Object; }
    // template<typename T>
    // requires is_any_of<T, Null, Bool, Int, Double, String, Array, Object>
    // bool isType() const {
    //     // 判断当前的variant是否存储了指定的类型
    //     return std::holds_alternative<T>(data_);
    // }
    //[[nodiscard]] bool isUndefined() const { return type() == Type::Undefined; }

    bool toBool(bool defaultValue = false) const;
    Int toInt(Int defaultValue = {}) const;
    int32_t toInt32(int32_t defaultValue = {}) const;
    double toDouble(double defaultValue = 0) const;
    String toString(const String& defaultValue = "") const;
#ifdef _RY_USE_QT_
    auto toQString() const { return QString::fromStdString(toString()); }
#endif
    Array toArray(const Array& defaultValue = {}) const;
    Object toObject(const Object& defaultValue = {}) const;
    //
    std::vector<String> objectKeys() const;
    // 比较
    bool operator==(const Json& other) const;
    bool operator==(const Int other) const;
    bool operator==(const int other) const;
    bool operator==(const double other) const;
    bool operator==(const String& other) const;
    bool operator!=(const Json& other) const;
    Json& operator/=(double value);
    // 获取数据
    template <typename... Args>
    [[nodiscard]] bool contains(const std::string& key, const Args&... args) const;

    template <typename T>
    [[nodiscard]] Int indexOf(const T& arg) const;
    // 获取或设置数据
    Json& operator[](const String& key);
    Json& operator[](const std::string_view& key);
    Json& operator[](const char* key); // object key
    Json& operator[](const size_t i);  // array index
    Json& operator[](const int i);     // array index
    // Json operator[](const QString& key) const; // array index
    Json operator[](const char* key) const; // object key
    Json at(const size_t i) const;          // array index
    template <typename T>
    Json at(const size_t i, T t) const; // array index
    Json at(std::string_view) const;    // array index

    template <typename T>
    Json valueOr(std::string_view, T t = {}) const;
#ifdef _RY_USE_QT_
    template <typename T, typename... Args>
    Json valueOr(const T& key, Args... args) const
        requires(std::is_same_v<T, QString>)
    {
        return valueOr(key.toStdString(), std::forward<Args>(args)...);
    }
#endif
    // 返回存在的第一个值
    template <typename... Args>
    Json value(std::string_view, Args... args) const;
#ifdef _RY_USE_QT_
    template <typename T, typename... Args>
    Json value(const T& key, Args... args) const
        requires(std::is_same_v<T, QString>)
    {
        return value(key.toStdString(), std::forward<Args>(args)...);
    }
#endif
    // template <typename... Args>
    // Json value(const QString& key, Args... args) const;

    bool push_back(const Json& other);
    bool push_back(Json&& other);
    bool remove(const String& key);
    bool remove(const int i);
    void clear();

    // 保留用作反射相关
    // template<typename T> T get() { return to }
    // 类型重载，获取数据
    operator Bool() const;
    // operator Null();
    operator Double() const;
    operator Int() const;
    operator size_t() const;
    operator int() const;
    operator String() const;

#ifdef _RY_USE_QT_
    operator QString() const;
    operator QByteArray() const;
#endif

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator const_begin() const noexcept;
    const_iterator const_end() const noexcept;
    const_iterator begin() const noexcept { return const_begin(); }
    const_iterator end() const noexcept { return const_end(); }

    size_t size() const noexcept;
    size_t length() const noexcept { return size(); }
    bool empty() const noexcept { return size() == 0; }

    void setParseProgressChanged(const std::function<void(int)>& callback);
    // 创建一个新的值
    Json map(const std::function<Json(const Json& v)>& f) const;
    Json map(const std::function<Json(const Json& v, size_t index)>& f) const;
    Json filter(const std::function<bool(const Json& v)>& f) const;
    void reserve(size_t s);
    Json& insert(const String& key, const Json& value);

public:
    // 兼容性处理
    [[nodiscard]] bool is_array() const { return type() == Type::Array; }
    [[nodiscard]] bool is_object() const { return type() == Type::Object; }
    [[nodiscard]] bool is_null() const { return type() == Type::Null; }

    // 将Json转为指定类型, 和ORM一样。
private:
    void swap(Json& other) noexcept
    {
        std::swap(this->data_, other.data_);
        std::swap(this->type_, other.type_);
    }

    static void throwError(std::string_view message);

    template <typename T>
    static void assign(T&& value, const Json& data)
    {
        if (data.isNull()) {
            return;
        }
        using U                   = std::remove_const_t<std::remove_reference_t<T>>;
        constexpr bool is_int64_v = (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>);
        if constexpr (std::is_integral_v<U> && !is_int64_v) {
            value = data.toInt();
        }
        else if constexpr (is_int64_v) {
            value = data.toInt();
        }
        else if constexpr (std::is_floating_point_v<U>) {
            value = data.toDouble();
        }
        else if constexpr (std::is_same_v<std::string, U>) {
            value = data.toString();
        }
        else if constexpr (std::is_same_v<String, U>) {
            value = data.toString();
        }
        // else if constexpr (std::is_same_v<QByteArray, U>) {
        //     value = data.toString().toUtf8();
        // }
        else {
            std::cout << "this type has not supported yet" << std::endl;
        }
    }

private:
    Type type_;
    Data data_;
    std::function<void(int)> _parseProgressChanged;
    // std::unique_ptr<Data> data_;
};

template <typename... Args>
bool Json::contains(const String& key, const Args&... args) const
{
    if (!isArray() && !isObject() && !isString()) {
        return false;
    }
    if (const Object* objd = std::get_if<Object>(&data_)) {
        if (objd->contains(key)) {
            return true;
        }
        if constexpr (sizeof...(args) > 0) {
            return contains(std::forward<String>(args)...);
        }
    }
    // String
    else if (const String* strd = std::get_if<String>(&data_)) {
        if (strd->contains(key)) {
            return true;
        }
        if constexpr (sizeof...(args) > 0) {
            return contains(std::forward<Args>(args)...);
        }
    }
    // Array
    else if (const Array* arrd = std::get_if<Array>(&data_)) {
        if (std::ranges::any_of(arrd->begin(), arrd->end(), [&key](const Json& value) {
                return value == key;
            })) {
            return true;
        }
        if constexpr (sizeof...(args) > 0) {
            return contains(std::forward<Args>(args)...);
        }
    }
    return false;
    // return ((hasValue(args)) || ...);
}

template <typename T>
Json::Int Json::indexOf(const T& arg) const
{
    if (!isArray()) {
        return -1;
    }
    if (const Array* arrd = std::get_if<Array>(&data_)) {
        for (Int i = 0; i < std::ssize(*arrd); ++i) {
            if (arrd->at(i) == arg) {
                return i;
            }
        }
    }
    return -1;
}
template <typename T>
Json Json::valueOr(std::string_view key, T t) const
{
    try {
        if (contains(key.data())) {
            return value(key);
        }
    }
    catch (...) {
    }
    if constexpr (std::is_floating_point_v<T>) {
        return Json(static_cast<double>(t));
    }
    return t;
}

template <typename... Args>
Json Json::value(std::string_view key, Args... args) const
{
    if (contains(key.data())) {
        const auto d = std::get_if<Object>(&data_);
        if (!d) {
            return {};
        }
        return d->at(key.data());
    }
    if constexpr (sizeof...(args) == 0) {
        return {};
    }
    else {
        return value(std::forward<Args>(args)...);
    }
}

template <typename T>
Json Json::at(const size_t i, T t) const
{
    const auto d = std::get_if<Array>(&data_);
    if (!d) {
        return t;
    }

    if (i < 0 || i > d->size()) {
        return t;
    }
    return d->at(i);
}

} // namespace ry

// 建议直接全用 RyJson 使用type区分更为方便。
using RyJson       = ry::Json;
using RyJsonValue  = ry::Json;
using RyJsonObject = ry::Json::Object;
using RyJsonArray  = ry::Json::Array;
/*  template <class T>
      requires std::is_same_v<Object, std::decay_t<T>>
  static auto begin(T& j) -> decltype(j.begin())
  {
      return j.begin();
  }
  template <class T>
      requires std::is_same_v<Array, std::decay_t<T>>
  static auto begin(T& j) -> decltype(j.begin())
  {
      return j.begin();
  }*/