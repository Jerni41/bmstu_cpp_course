#pragma once
#include <cstdint>
#include <exception>
#include <type_traits>
#include <utility>

namespace bmstu
{
//представление отстутствующего значения
struct nullopt_t
{
    //принимает int, чтобы предотвратить неявные преобразования
    constexpr explicit nullopt_t(int) {}
};

//глобальная константа для создания пустых optional
inline constexpr nullopt_t nullopt{0};


//обработка ошибок
class bad_optional_access : public std::exception
{
   public:
    const char* what() const noexcept override { return "Bad optional access"; }
};

//контейнер для опциональных значений
template <typename T>
class optional
{
   public:
    //создает пустой optional
    optional() noexcept : is_initialized_(false) {}

    //конструктор копирования
    optional(const T& value) : is_initialized_(true)
    {
        //используем placement new для создания объекта в буфере
        new (data_) T(value);
    }
    

    //конструктор перемещения
    optional(T&& value) noexcept : is_initialized_(true)
    {
        new (data_) T(std::move(value));
    }
    
    //копирует состояние и значение другого optional
    optional(const optional& other) : is_initialized_(other.is_initialized_)
    {
        if (other.is_initialized_)
        {
            new (data_) T(*other);
        }
    }
    
    //перемещает состояние и значение другого optional
    optional(optional&& other) noexcept
        : is_initialized_(other.is_initialized_)
    {
        if (other.is_initialized_)
        {
            new (data_) T(std::move(*other));
        }
    }

    
    //присваивание значения типа T
    optional& operator=(const T& value)
    {
        if (is_initialized_)
        {
            //есть значение - присваиваем новое
            **this = value;
        }
        else
        {
            //пуст - создаем новое значение
            new (data_) T(value);
            is_initialized_ = true;
        }
        return *this;
    }
    
    //перемещение
    optional& operator=(T&& value)
    {
        if (is_initialized_)
        {
            **this = std::move(value);
        }
        else
        {
            new (data_) T(std::move(value));
            is_initialized_ = true;
        }
        return *this;
    }
    
    //присваивание копированием из другого optional
    optional& operator=(const optional& other)
    {
        //проверка на самоприсваивание
        if (this != &other)
        {
            if (other.is_initialized_)
            {
                //если другой optional содержит значение
                if (is_initialized_)
                {
					//есть значение - присваиваем
                    **this = *other;
                }
                else
                {
                    //нет значения - создаем копию
                    new (data_) T(*other);
                    is_initialized_ = true;
                }
            }
            else
            {
                //другой optional пуст - сбрасываем наш
                reset();
            }
        }
        return *this;
    }
    
    //присваивание перемещением из другого optional
    optional& operator=(optional&& other) noexcept
    {
        if (this != &other)
        {
            if (other.is_initialized_)
            {
                if (is_initialized_)
                {
                    **this = std::move(*other);
                }
                else
                {
                    new (data_) T(std::move(*other));
                    is_initialized_ = true;
                }
            }
            else
            {
                reset();
            }
        }
        return *this;
    }

    //возвращает ссылку на хранимое значение 
    T& operator*() &
    {
        //преобразует сырой буфер в указатель на T и разыменовывает
        return *reinterpret_cast<T*>(data_);
    }
    
    //разыменования для константных lvalue
    const T& operator*() const&
    {
        return *reinterpret_cast<const T*>(data_);
    }
    
    //возвращает указатель на хранимое значение
    T* operator->()
    {
        return reinterpret_cast<T*>(data_);
    }
    
    //константная версия оператора доступа к членам
    const T* operator->() const
    {
        return reinterpret_cast<const T*>(data_);
    }
    
    //оператор разыменования для rvalue
    T&& operator*() &&
    {
        return std::move(*reinterpret_cast<T*>(data_));
    }

    //бросает исключение, если optional пуст
    T& value() &
    {
        if (!is_initialized_)
        {
            throw bad_optional_access();
        }
        return **this;
    }
    
    //метод value() для константных lvalue
    const T& value() const&
    {
        if (!is_initialized_)
        {
            throw bad_optional_access();
        }
        return **this;
    }

    //уничтожает старое значение и создает новое из аргументов
    template <typename... Args>
    void emplace(Args&&... args)
    {
        reset();
        new (data_) T(std::forward<Args>(args)...);
        is_initialized_ = true;
    }

    //уничтожает хранимое значение
    void reset()
    {
        if (is_initialized_)
        {
            reinterpret_cast<T*>(data_)->~T();
            is_initialized_ = false;
        }
    }
    
    //деструктор корректное уничтожение значения
    ~optional()
    {
        reset();
    }

    //проверяет, содержит ли optional значение
    bool has_value() const noexcept { return is_initialized_; }
    
   private:

    //правильное выравнивание для типа T
    alignas(T) uint8_t data_[sizeof(T)];
    
    //флаг инициализации: true если в буфере создан объект
    bool is_initialized_ = false;
};
}  // namespace bmstu