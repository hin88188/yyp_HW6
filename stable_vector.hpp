#ifndef STABLE_VECTOR_HPP
#define STABLE_VECTOR_HPP

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <iostream>

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
    
        void update(typename std::vector<node*>::iterator a) {
            if (v.front()->up!=v.begin()) a=v.begin();    //之前已resize
            for (; a!=v.end(); ++a) { (*a)->up=a; }
        }
    
        stable_vector():v({new node}) { v[0]->up=v.begin(); }

        explicit stable_vector(const size_type n, const T& value = T()) {
            v.resize(n+1);
            for (typename std::vector<node*>::iterator a=v.begin(); a!=v.end(); ++a) {
                *a=new node({value,a});     //已update
            }
        }

        template<typename InputIterator>
        stable_vector(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr) {
            v.resize(static_cast<size_type>(last-first)+1);
            for (typename std::vector<node*>::iterator a=v.begin(); a!=v.end(); ++first,++a) {
                if (a!=v.end()-1) *a=new node({*first,a});    //已update
                else *a=new node({0,a});    //已update
            }
        }

        stable_vector(const stable_vector& rhs) {
            v.resize(rhs.v.size());
            const_iterator it=rhs.begin();
            for (typename std::vector<node*>::iterator a=v.begin(); a!=v.end(); ++it,++a) {
                *a=new node({*it,a});    //已update
            }
        }

        stable_vector& operator=(const stable_vector& rhs) {
            stable_vector<T>(rhs).swap(*this);
            return *this;
        }

        ~stable_vector() { clear(); delete v[0]; }

        void assign(const size_type n, const T& value) {
            *this=stable_vector<T>(n,value);
        }
        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr) {
            *this=stable_vector<T>(first, last);
        }

        reference at(const size_type pos) { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }
        const_reference at(const size_type pos) const { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }

        reference operator[](const size_type pos){ return v[pos]->datum; }
        const_reference operator[](const size_type pos) const { return v[pos]->datum; }

        reference front() { return ( *(v.begin()) )->datum; }
        const_reference front() const { return ( *(v.begin()) )->datum; }

        reference back() { return ( *(v.end()-1) )->datum; }
        const_reference back() const { return ( *(v.end()-1) )->datum; }

        iterator begin() { return iterator(*v.begin()); }
        const_iterator begin() const { return const_iterator(*v.begin()); }
        const_iterator cbegin() const { return const_iterator(*v.begin()); }

        iterator end() { return iterator(*(v.end()-1)); }
        const_iterator end() const { return const_iterator(*(v.end()-1)); }
        const_iterator cend() const { return const_iterator(*(v.end()-1)); }

        bool empty() const { return v.size()==1; }

        size_type size() const { return v.size()-1; }

        void clear() { erase(cbegin(), cend()); }

        iterator insert(const_iterator pos, const T& value) {
            typename vector_type::iterator it=v.begin()+(pos-cbegin());
            v.insert(it, new node({value, it}));
            update(it+1);
            return begin()+(pos-cbegin());
        }
        template<typename InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
            stable_vector vv(first, last);
            iterator it1=vv.begin(), it2=vv.end();
            size_type count=0;
            difference_type d=pos-cbegin();
            for (iterator a=it1; a!=it2; ++count,++a) {
                insert(cbegin()+d+count, *a);
            }
            return begin()+d;
        }

        iterator erase(const_iterator pos) { return erase(pos,pos+1); }
        iterator erase(const_iterator first, const_iterator last) {
            difference_type d1=first-cbegin(), d2=last-first;
            typename vector_type::iterator it1=v.begin()+d1, it2=it1+d2, a=it1;
            for (size_type f=a-v.begin();a!=it2;++a,++f) { delete v[f]; }
            v.erase(it1,it2);
            update(v.begin());
            return begin()+d1;
        }

        void push_back(const T& value) { insert(cend(),value); }
        void pop_back() { if (!empty()) erase(cend()-1); }

        void resize(size_type count, const T& value = T()) {
            if (count > size()) {
                for (size_type f=count-size();f>0;--f) {
                    insert(cbegin()+size(), value);
                }
            }
            else if(count < size()) {
                erase(cbegin()+count, cend());
            }
        }

        void swap(stable_vector& other) {
            v.swap(other.v);
            update(v.begin());
            other.update(other.v.begin());
        }

        friend bool operator==(const stable_vector& lhs, const stable_vector& rhs) {
            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }
        friend bool operator!=(const stable_vector& lhs, const stable_vector& rhs) { return !(lhs == rhs); }
        friend bool operator< (const stable_vector& lhs, const stable_vector& rhs) {
            return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }
        friend bool operator<=(const stable_vector& lhs, const stable_vector& rhs) { return !(rhs < lhs); }
        friend bool operator> (const stable_vector& lhs, const stable_vector& rhs) { return rhs < lhs; }
        friend bool operator>=(const stable_vector& lhs, const stable_vector& rhs) { return !(lhs < rhs); }

        class iterator {
            friend class stable_vector;

            public:
                typedef stable_vector::difference_type difference_type;
                typedef stable_vector::value_type value_type;
                typedef stable_vector::pointer pointer;
                typedef stable_vector::reference reference;
                typedef std::random_access_iterator_tag iterator_category;

                explicit iterator(node* const n_ = nullptr) :n(n_){}
                iterator(const iterator& rhs) {	*this = rhs; }
                iterator& operator=(const iterator& rhs) { n = rhs.n; return *this; }
                ~iterator() {}

                reference operator*() const { return n->datum; }
                pointer operator->() const { return std::addressof(operator*()); }

                friend iterator operator+(iterator it, const difference_type i) {
                    return iterator(*(it.n->up + i));
                }
                friend iterator operator+(const difference_type i, iterator it) {
                    return it+i;
                }
                friend iterator operator-(iterator it, const difference_type i) {
                    return iterator(*(it.n->up - i));
                }
                friend difference_type operator-(const iterator lhs, const iterator rhs) {
                    return lhs.n->up - rhs.n->up;
                }

                iterator& operator+=(const difference_type i) {
                    *this = *this + i;
                    return *this;
                }
                iterator& operator-=(const difference_type i) {
                    *this += -i;
                    return *this;
                }
                
                iterator& operator++() { return *this=*this+1; }
                iterator operator++(int) {
                    iterator it(*this);
                    ++*this;
                    return it;
                }

                iterator& operator--() { return *this=*this-1; }
                iterator operator--(int) {
                    iterator it(*this);
                    --*this;
                    return iterator(it);
                }

                reference operator[](const difference_type i) { return n->up[i]->datum; }
                const_reference operator[](const difference_type i) const { return n->up[i]->datum; }

                operator const_iterator() const { return const_iterator(n); }

                friend bool operator==(const iterator lhs, const iterator rhs) { return lhs.n==rhs.n; }
                friend bool operator!=(const iterator lhs, const iterator rhs) { return !(lhs==rhs); }
                friend bool operator< (const iterator lhs, const iterator rhs) { return (lhs-rhs)<0; }
                friend bool operator<=(const iterator lhs, const iterator rhs) { return !(rhs<lhs); }
                friend bool operator> (const iterator lhs, const iterator rhs) { return rhs<lhs; }
                friend bool operator>=(const iterator lhs, const iterator rhs) { return !(lhs<rhs); }

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

                explicit const_iterator(const node* const n_) :n(n_){}
                const_iterator(const const_iterator& rhs) { n = rhs.n; }
                const_iterator& operator=(const const_iterator& rhs) { n = rhs.n; return *this; }
                ~const_iterator() {}

                friend const_iterator operator+(const_iterator it, const difference_type i) {
                    return const_iterator(*(it.n->up + i));
                }
                friend const_iterator operator+(const difference_type i, const_iterator it) {
                    return it+i;
                }
                friend const_iterator operator-(const_iterator it, const difference_type i) {
                    return const_iterator(*(it.n->up - i));
                }
                friend difference_type operator-(const const_iterator lhs, const const_iterator rhs) {
                    return lhs.n->up-rhs.n->up;
                }

                const_iterator& operator+=(const difference_type i) {
                    *this = *this + i;
                    return *this;
                }
                const_iterator& operator-=(const difference_type i) {
                    *this += -i;
                    return *this;
                }
            
                const_reference operator*() const {return ((*this).n)->datum;}
                const_pointer operator->() const { return std::addressof(operator*()); }

                const_iterator& operator++() { return *this=*this+1; }
                const_iterator operator++(int) {
                    iterator it(*this);
                    ++*this;
                    return it;
                }

                const_iterator& operator--() { return *this=*this-1; }
                const_iterator operator--(int) {
                    iterator it(*this);
                    --*this;
                    return it;
                }

                const_reference operator[](const difference_type i) const { return n->up[i]->datum; }

                friend bool operator==(const const_iterator lhs, const const_iterator rhs) { return lhs.n==rhs.n; }
                friend bool operator!=(const const_iterator lhs, const const_iterator rhs) { return !(lhs==rhs); }
                friend bool operator< (const const_iterator lhs, const const_iterator rhs) { return (lhs-rhs)<0; }
                friend bool operator<=(const const_iterator lhs, const const_iterator rhs) { return !(rhs-lhs); }
                friend bool operator> (const const_iterator lhs, const const_iterator rhs) { return rhs-lhs; }
                friend bool operator>=(const const_iterator lhs, const const_iterator rhs) { return !(lhs<rhs); }

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
