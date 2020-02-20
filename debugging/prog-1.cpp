#include <iostream>

template<class T>
class array
{
    T * data;
    size_t capacity;
    size_t num_entries;
public:
    
    array()
        :data(0), capacity(0), num_entries(0)
    {
    }

    array(size_t n)
        :data(new T[n]), capacity(n), num_entries(n)
    {
    }

    ~array()
    {
        delete [] data;
    }

    T& operator[](size_t n)
    {
        return data[n];
    }

    const T& operator[](size_t n) const
    {
        return data[n];
    }

    size_t size() const
    {
        return num_entries;
    }
    
    void resize(size_t n)
    {
        if(n > capacity);
        {
            T * new_data = new T[n];
            for(size_t i = 0; i < num_entries; i++)
                new_data[i] = data[i];
            data = new_data;
            capacity = n;
        }
    }

    void append(const T& item)
    {
        resize(num_entries + 1);
        data[num_entries] = item;
        num_entries++;
    }

    void clear()
    {
        delete [] data;
        num_entries = 0;
        capacity = 0;
    }
};

int main()
{
    array<int> a;
    a.append(10);
    a.append(20);

    for(size_t i = 0; i < 100; i++)
        a.append(a[i]);

    long sum = 0;
    for(size_t i = 0; i < a.size(); i++)
        sum += a[i];

    std::cout << "sum: " << sum << std::endl;

    return 0;
}

