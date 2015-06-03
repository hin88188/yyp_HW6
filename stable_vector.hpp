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
    
        void update() {
            for (size_t i=0; i<v.size(); ++i) { v[i]->up=v.begin()+i; }
        }
    
        stable_vector():v(0) {
            node* sentinel=new node;
            v.push_back(sentinel);
            v[0]->up=v.end()-1;
        }
/*stage6
        explicit stable_vector(const size_type n, const T& value = T()):stable_vector() {
            for (size_t i=0; i<n; ++i) {
                v.push_back(new node);
                v[i]->datum=value;
                //上面相當v.begin()->datum=value;
            }
            update();
        }

        template<typename InputIterator>
        stable_vector(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr) {
            for (iterator it=first; it<last; ++it) {
                v.push_back(new node);
            }
        }

        stable_vector(const stable_vector& rhs) {
            
        }

        stable_vector& operator=(const stable_vector& rhs) {
            
        }

        ~stable_vector() {
            
        }

        void assign(const size_type n, const T& value) {
            erase(begin(),end());
            insert(begin(),n,value);
        }
        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last, typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = nullptr) {
            erase(begin(),end());
            insert(begin(),first,last);
        }
*/
        reference at(const size_type pos) { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }
        const_reference at(const size_type pos) const { return pos < size() ? (*this)[pos] : throw std::range_error("stable_vector: out of range"); }

        reference operator[](const size_type pos){
            if (pos < v.size()) { return v[pos]->datum; }
            else { throw std::range_error("stable_vector: out of range"); }
            //return static_cast<node>(*this->v[pos]).datum;
        }
        const_reference operator[](const size_type pos) const {
            if (pos < v.size()) { return v[pos]->datum; }
            else { throw std::range_error("stable_vector: out of range"); }
        }

        reference front() {
            if(!v.empty()) { return v[0]->datum; }
            else { throw std::range_error("vector is empty"); }
        }
        const_reference front() const {
            if(!v.empty()) { return v[0]->datum; }
            else { throw std::range_error("vector is empty"); }
        }

        reference back() {
            if(!v.empty()) { return v[v.size()-1]->datum; }
            else { throw std::range_error("vector is empty"); }
        }
        const_reference back() const {
            if(!v.empty()) { return v[v.size()-1]->datum; }
            else { throw std::range_error("vector is empty"); }
        }

        iterator begin() {
            return iterator(node_ptr(v.front()));
            //return (this->v.empty()) ? this->end(): iterator(node_ptr_traits::static_cast_from(this->v.front()));
        }
        const_iterator begin() const {
            return const_iterator(node_ptr(v.front()));
            //return (this->v.empty()) ? this->cend() : const_iterator(node_ptr_traits::static_cast_from(this->v.front()));
        }
        const_iterator cbegin() const {
            return begin();
        }

        iterator end() {
            return iterator(node_ptr(v.back()));
            //return iterator(this->priv_get_end_node());
        }
        const_iterator end() const {
            return const_iterator(node_ptr(v.back()));
            //return const_iterator(this->priv_get_end_node());
        }
        const_iterator cend() const {
            return end();
        }

        bool empty() const { return v.empty() ? true : false; }

        size_type size() const { return v.size(); }

//        void clear() {
//            erase(begin(),end());
//            //this->erase(this->cbegin(),this->cend());
//        }
//
//        iterator insert(const_iterator pos, const T& value) {
//            difference_type d=position-begin();
//            impl_iterator   it;
//            if(impl.capacity()>impl.size()){
//                it=impl.insert(impl.begin()+d,0);
//                try{
//                    *it=new_node(&*it,value);
//                }
//                catch(...){
//                    impl.erase(it);
//                    throw;
//                }
//                align_nodes(it+1,impl.end());
//            }
//            else{
//                it=impl.insert(impl.begin()+d,0);
//                try{
//                    *it=new_node(0,t);
//                }
//                catch(...){
//                    impl.erase(it);
//                    align_nodes(impl.begin(),impl.end());
//                    throw;
//                }
//                align_nodes(impl.begin(),impl.end());
//            }
//            return iterator(node_ptr(*it));
//        }
//        template<typename InputIterator>
//        iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
//            insert_iter(
//                        position,first,last,
//                        boost::mpl::not_<boost::is_integral<InputIterator> >());
//        }
//
//        iterator erase(const_iterator pos) {
//            difference_type d=position-begin();
//            impl_iterator   it=impl.begin()+d;
//            delete_node(*it);
//            impl.erase(it);
//            align_nodes(impl.begin()+d,impl.end());
//            return begin()+d;
//            
////            const size_type d = p - this->cbegin();
////            index_iterator it = this->index.begin() + d;
////            this->priv_delete_node(p.node_pointer());
////            it = this->index.erase(it);
////            index_traits_type::fix_up_pointers_from(this->index, it);
////            return iterator(node_ptr_traits::static_cast_from(*it));
//        }
//        iterator erase(const_iterator first, const_iterator last) {
//            difference_type d1=first-begin(),d2=last-begin();
//            impl_iterator   it1=impl.begin()+d1,it2=impl.begin()+d2;
//            for(impl_iterator it=it1;it!=it2;++it)delete_node(*it);
//            impl.erase(it1,it2);
//            align_nodes(impl.begin()+d1,impl.end());
//            return begin()+d1;
//            
////            const const_iterator cbeg(this->cbegin());
////            const size_type d1 = static_cast<size_type>(first - cbeg),
////            d2 = static_cast<size_type>(last  - cbeg);
////            size_type d_dif = d2 - d1;
////            if(d_dif){
////                multiallocation_chain holder;
////                const index_iterator it1(this->index.begin() + d1);
////                const index_iterator it2(it1 + d_dif);
////                index_iterator it(it1);
////                while(d_dif--){
////                    node_base_ptr &nb = *it;
////                    ++it;
////                    node_type &n = *node_ptr_traits::static_cast_from(nb);
////                    this->priv_destroy_node(n);
////                    holder.push_back(node_ptr_traits::pointer_to(n));
////                }
////                this->priv_put_in_pool(holder);
////                const index_iterator e = this->index.erase(it1, it2);
////                index_traits_type::fix_up_pointers_from(this->index, e);
////            }
////            return iterator(last.node_pointer());
//        }
//
//        void push_back(const T& value) {
//            insert(v.end()-1,value);
//            update();
//        }
//        void pop_back() {
//            erase(end()-1);
////            this->erase(--this->cend());
//        }
//
//        void resize(size_type count, const T& value = T()) {
//            if(count>size())insert(end(),count-size(),value);
//            else if(count<size())erase(begin()+count,end());
//            
////            if(count > this->size())
////                this->insert(this->cend(), count - this->size(), value);
////            else if(n < this->size())
////                this->erase(this->cbegin() + count, this->cend());
//        }
//
//        void swap(stable_vector& other) {
//            swap_impl(*this,other);
//            
////            this->index.swap(other.index);
////            this->priv_swap_members(other);
//        }
//
//        friend bool operator==(const stable_vector& lhs, const stable_vector& rhs) {
//            return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
//        }
//        friend bool operator!=(const stable_vector& lhs, const stable_vector& rhs) { return !(lhs == rhs); }
//        friend bool operator< (const stable_vector& lhs, const stable_vector& rhs) {
//            return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
//        }
//        friend bool operator<=(const stable_vector& lhs, const stable_vector& rhs) { return !(rhs < lhs); }
//        friend bool operator> (const stable_vector& lhs, const stable_vector& rhs) { return rhs < lhs; }
//        friend bool operator>=(const stable_vector& lhs, const stable_vector& rhs) { return !(lhs < rhs); }

        class iterator {
            friend class stable_vector;

            public:
                typedef stable_vector::difference_type difference_type;
                typedef stable_vector::value_type value_type;
                typedef stable_vector::pointer pointer;
                typedef stable_vector::reference reference;
                typedef std::random_access_iterator_tag iterator_category;

                explicit iterator(node* const n_ = nullptr) :n(n_){}
                iterator(const iterator& rhs) {	(*this).n = rhs.n; }
                iterator& operator=(const iterator& rhs) { (*this).n = rhs.n; }
                ~iterator() {}

                reference operator*() const { return ((*this).n)->datum; }
                pointer operator->() const { return std::addressof(operator*()); }
/*stage7
                friend iterator operator+(iterator it, const difference_type i) {
                    stable_vector_iterator tmp(left);
                    tmp += off;
                    return tmp;
                    
//                    return (*((*(it.n)).up)+i);
                }
                friend iterator operator+(const difference_type i, iterator it) {
                    stable_vector_iterator tmp(right);
                    tmp += off;
                    return tmp;
                    
//                    return it+i;
                }
                friend iterator operator-(iterator it, const difference_type i) {
                    stable_vector_iterator tmp(left);
                    tmp -= off;
                    return tmp;
                    
//                    return (*((*(it.n)).up)-i);
                }
                friend difference_type operator-(const iterator lhs, const iterator rhs) {
                    return lhs.m_pn->up - rhs.m_pn->up;
                    
//                    return *((*(lhs.n)).up) - *((*(rhs.n)).up);
                }

                iterator& operator+=(const difference_type i) {
                    if(i) this->m_pn = this->m_pn->up[i];
                    return *this;
                    
//                    *this = *this + i;
//                    return *this;
                }
                iterator& operator-=(const difference_type i) {
                    *this += -i;
                    return *this;
                    
//                    *this = *this + i;
//                    return *this;
                }
                
                iterator& operator++() {
                    node_base_ptr_ptr p(this->m_pn->up);
                    this->m_pn = *(++p);
                    return *this;
                    
//                    *this = *this + 1;
//                    return *this;
                }
                iterator operator++(int) {
                    iterator tmp(*this);
                    ++*this;
                    return iterator(tmp);
                    
//                    return *this + 1;
                }

                iterator& operator--() {
                    node_base_ptr_ptr p(this->m_pn->up);
                    this->m_pn = *(--p);
                    return *this;
                    
//                    *this = *this - 1;
//                    return *this;
                }
                iterator operator--(int) {
                    iterator tmp(*this);
                    --*this;
                    return iterator(tmp);
                    
//                    *this = *this - 1;
                }

                reference operator[](const difference_type i) {
                    return node_ptr_traits::static_cast_from(this->m_pn->up[i])->value;
                    
//                    return *(*this + i);
                }
                const_reference operator[](const difference_type i) const {
                    return node_ptr_traits::static_cast_from(this->m_pn->up[i])->value;
                    
//                    return *(*this + i);

                }
*/
//                operator const_iterator() const {
////                    return (const_iterator)(*this).n;
//                }
/*stage9
                friend bool operator==(const iterator lhs, const iterator rhs) {
                    return lhs.n == rhs.n ? 1 : 0;
                }
                friend bool operator!=(const iterator lhs, const iterator rhs) { return !(lhs==rhs) }
                friend bool operator< (const iterator lhs, const iterator rhs) {}
                friend bool operator<=(const iterator lhs, const iterator rhs) {}
                friend bool operator> (const iterator lhs, const iterator rhs) {}
                friend bool operator>=(const iterator lhs, const iterator rhs) {}
*/
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
                const_iterator(const const_iterator& rhs) { (*this).n = rhs.n; }
                const_iterator& operator=(const const_iterator& rhs) { (*this).n = rhs.n; }
                ~const_iterator() {}

//                friend const_iterator operator+(const_iterator it, const difference_type i) {}
//                friend const_iterator operator+(const difference_type i, const_iterator it) {}
//                friend const_iterator operator-(const_iterator it, const difference_type i) {}
//                friend difference_type operator-(const const_iterator lhs, const const_iterator rhs) {}
//
//                const_iterator& operator+=(const difference_type i) {}
//                const_iterator& operator-=(const difference_type i) {}
            
                const_reference operator*() const {return ((*this).n)->datum;}
                const_pointer operator->() const { return std::addressof(operator*()); }

//                const_iterator& operator++() {}
//                const_iterator operator++(int) {}
//
//                const_iterator& operator--() {}
//                const_iterator operator--(int) {}
//
//                const_reference operator[](const difference_type i) const {}
//
//                friend bool operator==(const const_iterator lhs, const const_iterator rhs) {}
//                friend bool operator!=(const const_iterator lhs, const const_iterator rhs) {}
//                friend bool operator< (const const_iterator lhs, const const_iterator rhs) {}
//                friend bool operator<=(const const_iterator lhs, const const_iterator rhs) {}
//                friend bool operator> (const const_iterator lhs, const const_iterator rhs) {}
//                friend bool operator>=(const const_iterator lhs, const const_iterator rhs) {}

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
