#pragma once

#include <initializer_list>
#include <memory>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

namespace pkus {

const int s_resize_factor = 2;

/// Кольцевой расширяемый буфер, с динамическим выделением памяти.
/// T обязан иметь пустой конструктор и конструктор копирования или оператор присваивания
/// Предоставляет сильную горантию исключений для всех методов
template< typename T >
class RingBuffer
{
     std::unique_ptr< T[] > m_p; //указатель на начало динамического массива

     size_t m_size {}; //актуальное количество элементов в очереди
     size_t m_cap {};  //емкость (сколько выделено памяти). Всегда не меньше 1
     size_t m_begin {}; //индекс первого элемента в очереди
     size_t m_end {};   //индекс первого свободного элемента в очереди

     template< typename ContainerType >
     class iterator_tmpl
     {
          ContainerType m_container;
          size_t m_index;

     public:
          using iterator_category = std::bidirectional_iterator_tag;
          using difference_type = std::ptrdiff_t;
          using value_type = T;
          using pointer = T*;   // or also value_type*
          using reference = T&; // or also value_type&

          iterator_tmpl( ContainerType c, size_t ind )
               : m_container( c )
               , m_index( ind )
          {}

          reference operator*()
          {
               return m_container->m_p[ m_index ];
          }
          pointer operator->()
          {
               return &m_container->m_p[ m_index ];
          }
          iterator_tmpl& operator++()
          {
               m_index = ( m_index + 1 ) % ( m_container->m_cap );
               return *this;
          }
          iterator_tmpl& operator--()
          {
               m_index = ( m_index - 1 ) % ( m_container->m_cap );
               return *this;
          }

          //https://stackoverflow.com/questions/10070020/c-operator-overloading
          friend bool operator==( const iterator_tmpl& lf, const iterator_tmpl& rt )
          {
               return lf.m_index == rt.m_index;
          }
          friend bool operator!=( const iterator_tmpl& lf, const iterator_tmpl& rt )
          {
               return !( lf == rt );
          }
     };

     typedef iterator_tmpl< RingBuffer< T >* > iterator;
     typedef iterator_tmpl< const RingBuffer< T >* > const_iterator;

public:
     RingBuffer( size_t size = 15 );
     RingBuffer( size_t size, const T& defaultVal );
     RingBuffer( std::initializer_list< T > init );
     RingBuffer( const RingBuffer& other );
     RingBuffer( RingBuffer&& other ) noexcept;
     RingBuffer< T >& operator=( RingBuffer other ) noexcept;
     ~RingBuffer() noexcept( noexcept( std::is_nothrow_destructible< T >() ) ) {};

     template< class ForwardIterator >
     void pushFront( ForwardIterator first, ForwardIterator last )
     {
          size_t new_cap = m_cap;
          while( ( last - first ) + m_size >= new_cap )
          {
               new_cap *= s_resize_factor;
          }
          std::unique_ptr< T[] > storage;
          size_t tmp_begin = m_begin;
          size_t tmp_end = m_end;
          size_t tmp_size = m_size;

          if( new_cap != m_cap )
          {
               storage.reset( new T[ new_cap ] );

               int ind = tmp_begin = m_cap;
               dataMoving( storage, ind );
               tmp_end = tmp_begin + tmp_size;
          }

          T* ptr = storage ? storage.get() : m_p.get();
          while( first != last )
          {
               tmp_begin = ( tmp_begin ? tmp_begin : m_cap ) - 1;
               ptr[ tmp_begin ] = *( --last );
               ++tmp_size;
          }
          m_begin = tmp_begin;
          m_size = tmp_size;

          if( storage )
          {
               m_p.swap( storage );
               m_cap = new_cap;
               m_end = tmp_end;
          }
     }

     void pushFront( const T& obj )
     {
          if( m_size == ( m_cap - 1 ) )
          {
               size_t new_cap = m_cap * s_resize_factor;
               std::unique_ptr< T[] > tmp( new T[ new_cap ] );

               int ind, new_begin;
               new_begin = ind = m_cap - 1;
               tmp[ ind++ ] = obj;
               dataMoving( tmp, ind );

               std::swap( m_p, tmp );
               m_begin = new_begin;
               ++m_size;
               m_end = m_begin + m_size;
               m_cap = new_cap;

               return;
          }

          int new_begin = ( m_begin ? m_begin : m_cap ) - 1; //for exception safety
          m_p[ new_begin ] = obj;
          m_begin = new_begin;
          ++m_size;
     }

     template< class ForwardIterator >
     void pushBack( ForwardIterator first, ForwardIterator last )
     {
          size_t new_cap = m_cap;
          while( ( last - first ) + m_size >= new_cap )
          {
               new_cap *= s_resize_factor;
          }
          std::unique_ptr< T[] > storage;
          size_t tmp_end = m_end;
          size_t tmp_size = m_size;

          if( new_cap != m_cap )
          {
               storage.reset( new T[ new_cap ] );

               int ind = 0;
               dataMoving( storage, ind );
               tmp_end = tmp_size = ind;
          }

          T* ptr = storage ? storage.get() : m_p.get();
          for( ; first != last; ++first )
          {
               ptr[ tmp_end ] = *first;
               ++tmp_end;
               if( tmp_end == new_cap )
                    tmp_end = 0;
               ++tmp_size;
          }
          m_end = tmp_end;
          m_size = tmp_size;

          if( storage )
          {
               m_p.swap( storage );
               m_cap = new_cap;
               m_begin = 0;
          }
     }

     void pushBack( const T& obj )
     {
          if( m_size == ( m_cap - 1 ) )
          {
               // тут должно быть исключение для полного соотвествия заданию
               // throw std::out_of_range("container is full");
               size_t new_cap = m_cap * 2;
               std::unique_ptr< T[] > tmp( new T[ new_cap ] );

               int ind = 0;
               dataMoving( tmp, ind );
               tmp[ ind++ ] = obj;

               m_p.swap( tmp );
               m_begin = 0;
               m_end = m_size = ind;
               m_cap = new_cap;

               return;
          }

          m_p[ m_end ] = obj;
          ++m_end;
          if( m_end == m_cap )
               m_end = 0;
          ++m_size;
     }

     T popFront()
     {
          if( !m_size )
               throw std::out_of_range( "size cannot be zero" );

          T ret = std::move( m_p[ m_begin ] );
          m_begin = ( m_begin + 1 ) % m_cap;
          --m_size;

          return ret;
     }

     void removeFront( size_t count )
     {
          if( m_size < count )
               throw std::out_of_range( "count to deleting cannot be greater than the size" );

          m_begin = ( m_begin + count ) % m_cap;
          m_size -= count;
     }

     void removeBack( size_t count )
     {
          if( m_size < count )
               throw std::out_of_range( "count to deleting cannot be greater than the size" );

          int tmp = int( m_end - count );
          m_end = tmp < 0 ? m_cap + tmp : tmp;
          m_size -= count;
     }

     void print() const noexcept
     {
          std::ostringstream out;
          out << "begin " << m_begin << " end " << m_end << " size " << m_size << " capacity " << m_cap << std::endl
              << std::endl;

          std::cout << out.str();
     }

     void clear() noexcept
     {
          m_size = 0;
          m_begin = 0;
          m_end = 0;
     }

     inline size_t size() const noexcept
     {
          return m_size;
     }
     inline size_t cap() const noexcept
     {
          return m_cap;
     }
     inline bool isValid() const noexcept
     {
          return m_begin != m_end;
     }

     typename RingBuffer< T >::iterator begin()
     {
          return RingBuffer< T >::iterator( this, m_begin );
     }
     typename RingBuffer< T >::iterator end()
     {
          return RingBuffer< T >::iterator( this, m_end );
     }
     typename RingBuffer< T >::const_iterator begin() const
     {
          return RingBuffer< T >::const_iterator( this, m_begin );
     }
     typename RingBuffer< T >::const_iterator end() const
     {
          return RingBuffer< T >::const_iterator( this, m_end );
     }
     T& operator[]( size_t index )
     {
          if( index >= m_size )
               throw std::out_of_range( "index out of range" );

          std::cout << m_begin << ' ' << index << ' ' << ( m_begin + index ) % m_cap << std::endl;
          return m_p[ ( m_begin + index ) % m_cap ];
     };
     const T& operator[]( size_t index ) const
     {
          if( index >= m_size )
               throw std::out_of_range( "index out of range" );

          return m_p[ ( m_begin + index ) % m_cap ];
     };

private:
     void dataMoving( std::unique_ptr< T[] >& newMemo, int& ind )
     {
          //Cильная гарантия:
          //Перемещаем - если уверены в процессе перемещения не может возникнуть исключение
          //которое потребует возврата к начальному состоянию. Иначе копируем.
          if( std::is_nothrow_move_assignable< T >() )
          {
               for( auto& el : *this )
               {
                    newMemo[ ind++ ] = std::move( el );
               }
          }
          else
          {
               for( const auto& el : *this )
               {
                    newMemo[ ind++ ] = el;
               }
          }
     }
};

template< typename T >
RingBuffer< T >::RingBuffer( size_t size )
     : m_p( new T[ size + 1 ] )
     , m_cap { size + 1 }
{}

template< typename T >
RingBuffer< T >::RingBuffer( size_t size, const T& defaultVal )
     : m_p( new T[ size + 1 ] )
     , m_cap { size + 1 }
     , m_size { size }
     , m_begin { 0 }
     , m_end { size }
{
     for( auto& el : *this )
          el = defaultVal;
     //not optimal, think about allocate() and unintialized_copy()
}

template< typename T >
RingBuffer< T >::RingBuffer( std::initializer_list< T > init )
     : m_p( new T[ init.size() + 1 ] )
     , m_cap { init.size() + 1 }
     , m_size { init.size() }
     , m_begin { 0 }
     , m_end { init.size() }
{
     std::move( init.begin(), init.end(), m_p.get() );
     //in fact make copy because: https://stackoverflow.com/questions/8193102/initializer-list-and-move-semantics
     //not optimal, think about allocate() and unintialized_copy()
}

template< typename T >
RingBuffer< T >::RingBuffer( const RingBuffer& other )
     : m_p( new T[ other.m_size + 1 ] )
     , m_cap { other.m_size + 1 }
     , m_size { other.m_size }
     , m_begin { 0 }
     , m_end { other.m_size }
{
     std::copy( other.begin(), other.end(), m_p.get() );
}

template< typename T >
RingBuffer< T >::RingBuffer( RingBuffer&& other ) noexcept
     : m_size { other.m_size }
     , m_cap { other.m_cap }
     , m_begin { other.m_begin }
     , m_end { other.m_end }
{
     m_p.swap( other.m_p );
}

template< typename T >
RingBuffer< T >& RingBuffer< T >::operator=( RingBuffer other ) noexcept
{
     m_p.swap( other.m_p );
     std::swap( m_size, other.m_size );
     std::swap( m_cap, other.m_cap );
     std::swap( m_begin, other.m_begin );
     std::swap( m_end, other.m_end );

     return *this;
}

} // namespace pkus
