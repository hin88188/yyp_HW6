#ifndef STABLE_VECTOR_HPP
#define STABLE_VECTOR_HPP

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<typename T>
class stable_vector {
    private:
        struct node;

    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef std::size_t size_type; 
        typedef std::ptrdiff_t difference_type; 

        class iterator;
        class const_iterator;
        
        stable_vector();

        explicit stable_vector(const size_type n, const T& value = T());

        template<typename InputIterator>
        stable_vector(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr);

        stable_vector(const stable_vector& rhs);

        stable_vector& operator=(const stable_vector& rhs);

        ~stable_vector();

        void assign(const size_type n, const T& value);
        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr);

        reference at(const size_type pos) { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }
        const_reference at(const size_type pos) const { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }

        reference operator[](const size_type pos);
        const_reference operator[](const size_type pos) const;

        reference front();
        const_reference front() const;

        reference back();
        const_reference back() const;

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

        bool empty() const;

        size_type size() const;

        void clear();

        iterator insert(const_iterator pos, const T& value);
        template<typename InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last);

        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        void push_back(const T& value);
        void pop_back();

        void resize(size_type count, const T& value = T());

        void swap(stable_vector& other);

        friend bool operator==(const stable_vector& lhs, const stable_vector& rhs);
        friend bool operator!=(const stable_vector& lhs, const stable_vector& rhs);
        friend bool operator< (const stable_vector& lhs, const stable_vector& rhs);
        friend bool operator<=(const stable_vector& lhs, const stable_vector& rhs);
        friend bool operator> (const stable_vector& lhs, const stable_vector& rhs);
        friend bool operator>=(const stable_vector& lhs, const stable_vector& rhs);

        class iterator {
            friend class stable_vector;

            public:
                typedef stable_vector::difference_type difference_type;
                typedef stable_vector::value_type value_type;
                typedef stable_vector::pointer pointer;
                typedef stable_vector::reference reference;
                typedef std::random_access_iterator_tag iterator_category;

                explicit iterator(node* const n_ = nullptr);
                iterator(const iterator& rhs);
                iterator& operator=(const iterator& rhs);
                ~iterator();

                reference operator*() const;
                pointer operator->() const { return std::addressof(operator*()); }

                friend iterator operator+(iterator it, const difference_type i);
                friend iterator operator+(const difference_type i, iterator it);
                friend iterator operator-(iterator it, const difference_type i);
                friend difference_type operator-(const iterator lhs, const iterator rhs);

                iterator& operator+=(const difference_type i);
                iterator& operator-=(const difference_type i);
                
                iterator& operator++();
                iterator operator++(int);

                iterator& operator--();
                iterator operator--(int);

                reference operator[](const difference_type i);
                const_reference operator[](const difference_type i) const;

                operator const_iterator() const;

                friend bool operator==(const iterator lhs, const iterator rhs);
                friend bool operator!=(const iterator lhs, const iterator rhs);
                friend bool operator< (const iterator lhs, const iterator rhs);
                friend bool operator<=(const iterator lhs, const iterator rhs);
                friend bool operator> (const iterator lhs, const iterator rhs);
                friend bool operator>=(const iterator lhs, const iterator rhs);

            private:
                node* n;
        };

        class const_iterator {
            friend class stable_vector;

            public:
                typedef stable_vector::difference_type difference_type;
                typedef stable_vector::value_type value_type;
                typedef stable_vector::pointer pointer;
                typedef stable_vector::reference reference;
                typedef std::random_access_iterator_tag iterator_category;

                explicit const_iterator(const node* const n_);
                const_iterator(const const_iterator& rhs);
                const_iterator& operator=(const const_iterator& rhs);
                ~const_iterator();

                friend const_iterator operator+(const_iterator it, const difference_type i);
                friend const_iterator operator+(const difference_type i, const_iterator it);
                friend const_iterator operator-(const_iterator it, const difference_type i);
                friend difference_type operator-(const const_iterator lhs, const const_iterator rhs);

                const_iterator& operator+=(const difference_type i);
                const_iterator& operator-=(const difference_type i);
                
                const_reference operator*() const;
                const_pointer operator->() const { return std::addressof(operator*()); }

                const_iterator& operator++();
                const_iterator operator++(int);

                const_iterator& operator--();
                const_iterator operator--(int);

                const_reference operator[](const difference_type i) const;

                friend bool operator==(const const_iterator lhs, const const_iterator rhs);
                friend bool operator!=(const const_iterator lhs, const const_iterator rhs);
                friend bool operator< (const const_iterator lhs, const const_iterator rhs);
                friend bool operator<=(const const_iterator lhs, const const_iterator rhs);
                friend bool operator> (const const_iterator lhs, const const_iterator rhs);
                friend bool operator>=(const const_iterator lhs, const const_iterator rhs);

            private:
                const node* n;
        };

    private:
        typedef std::vector<node*> vector_type;
        vector_type v;

        struct node {
            T datum;
            typename vector_type::iterator up;
        };
};

#endif
