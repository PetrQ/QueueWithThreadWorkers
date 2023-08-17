#pragma once

#include <initializer_list>
#include <memory>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>

namespace pkus {

/// Кольцевой расширяемый буфер, с динамическим выделением памяти.
/// T обязан иметь пустой конструктор и конструктор копирования или оператор присваивания

template< typename T >
class RingBuffer
{
     T* m_p {}; //указатель на начало динамического массива

     std::size_t m_size {}; //актуальное количество элементов в очереди
     std::size_t m_cap {};  //емкость (сколько выделено памяти)
     std::size_t
          m_begin {}; //индекс первого элемента в очереди (это тот элемент, который можно извлечь из очереди с помощью pop())
     std::size_t
          m_end {}; //индекс первого свободного элемента в очереди (это тот элемент, в который можно присвоить новое значение с помощью push())
     const static std::size_t m_delta
          = 10; //на сколько увеличиваем емкость при перераспределении памяти (TODO лучше по множителю)

     template< typename ContainerType >
     class iterator_tmpl
     {
          ContainerType m_container;
          std::size_t m_index;

     public:
          using iterator_category = std::bidirectional_iterator_tag;
          using difference_type = std::ptrdiff_t;
          using value_type = T;
          using pointer = T*;   // or also value_type*
          using reference = T&; // or also value_type&

          iterator_tmpl( ContainerType c, std::size_t ind )
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
     RingBuffer();
     RingBuffer( size_t size, const T& defaultVal = T() );
     RingBuffer( std::initializer_list< T > init );
     RingBuffer( const RingBuffer& other );
     RingBuffer( RingBuffer&& other );
     RingBuffer< T >& operator=( const RingBuffer& other );
     RingBuffer< T >& operator=( RingBuffer&& other );
     ~RingBuffer();

     template< class ForwardIterator >
     void pushFront( ForwardIterator first, ForwardIterator last )
     {
          while( first != last )
          {
               if( m_size == ( m_cap - 1 ) )
               {
                    size_t newSz = m_cap + m_delta;
                    T* tmpPtr = new T[ newSz ];

                    int ind = m_delta;
                    for( auto& el : *this )
                    {
                         tmpPtr[ ind++ ] = std::move( el );
                    }

                    delete[] m_p;
                    m_p = tmpPtr;
                    m_begin = m_delta;
                    m_end = m_begin + m_size;
                    m_cap = newSz;
               }

               m_begin = ( m_begin - 1 ) % m_cap;
               m_p[ m_begin ] = *( --last );
               ++m_size;
          }
     }

     template< class ForwardIterator >
     void pushBack( ForwardIterator first, ForwardIterator last )
     {
          for( ; first != last; ++first )
          {
               pushBack( *first );
          }
     }

     void pushBack( const T& obj )
     {
          if( m_size == ( m_cap - 1 ) ) //тут должно быть
          {
               //тут должно быть исключение для полного соотвествия заданию
               //throw std::range_error( "container is full" );
               size_t newSz = m_cap + m_delta;
               T* tmpPtr = new T[ newSz ];

               int ind = 0;
               for( auto& el : *this )
               {
                    tmpPtr[ ind++ ] = std::move( el );
               }

               delete[] m_p;
               m_p = tmpPtr;
               m_begin = 0;
               m_end = m_size;
               m_cap = newSz;
          }

          m_p[ m_end ] = obj;
          m_end = ( m_end + 1 ) % m_cap;
          ++m_size;
     }

     T popFront()
     {
          if( !m_size )
               throw std::range_error( "size cannot be zero" );

          T ret = std::move( m_p[ m_begin ] );
          m_begin = ( m_begin + 1 ) % m_cap;
          --m_size;

          return ret;
     }

     void removeFront( size_t count )
     {
          if( m_size < count )
               throw std::range_error( "count to deleting cannot be greater than the size" );

          m_begin = ( m_begin + count ) % m_cap;
          m_size -= count;
     }

     void removeBack( size_t count )
     {
          if( m_size < count )
               throw std::range_error( "count to deleting cannot be greater than the size" );

          int tmp = int( m_end - count );
          m_end = tmp < 0 ? m_cap + tmp : tmp;
          m_size -= count;
     }

     void print() const
     {
          std::ostringstream out;
          out << "begin " << m_begin << " end " << m_end << " size " << m_size << " capacity " << m_cap << std::endl
              << std::endl;

          std::cout << out.str();
     }

     void clear()
     {
          m_size = 0;
          m_begin = 0;
          m_end = 0;
     }

     inline std::size_t size() const
     {
          return m_size;
     }
     inline std::size_t cap() const
     {
          return m_cap;
     }
     inline bool isValid() const
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
               throw std::range_error( "index out of range" );

          std::cout << m_begin << ' ' << index << ' ' << ( m_begin + index ) % m_cap << std::endl;
          return m_p[ ( m_begin + index ) % m_cap ];
     };
     const T& operator[]( size_t index ) const
     {
          if( index >= m_size )
               throw std::range_error( "index out of range" );

          return m_p[ ( m_begin + index ) % m_cap ];
     };

private:
     void Init( size_t size )
     {
          m_size = size;
          m_begin = 0;
          m_end = size;

          if( m_cap && size < ( m_cap - 1 ) ) //для нового обьекта m_cap == 0
               return;

          delete[] m_p;
          m_p = new T[ size + m_delta ];
          m_cap = size + m_delta;
     }
};

template< typename T >
RingBuffer< T >::RingBuffer()
     : m_p( new T[ m_delta ] )
     , m_size { 0 }
     , m_cap { m_delta }
     , m_begin { 0 }
     , m_end { 0 }
{}

template< typename T >
RingBuffer< T >::RingBuffer( size_t size, const T& defaultVal )
     : m_p( new T[ size + 1 ] )
     , m_size { 0 }
     , m_cap { size + 1 }
     , m_begin { 0 }
     , m_end { 0 }
{
     for( auto& el : *this )
          el = defaultVal;
}

template< typename T >
RingBuffer< T >::RingBuffer( std::initializer_list< T > init )
     : m_p( new T[ init.size() + m_delta ] )
     , m_size { init.size() }
     , m_cap { init.size() + m_delta }
     , m_begin { 0 }
     , m_end { init.size() }
{
     auto it = init.begin();

     for( size_t i = 0; i < m_size; ++i )
     {
          m_p[ i ] = std::move( *it );
          ++it;
     }
}

template< typename T >
RingBuffer< T >::~RingBuffer()
{
     delete[] m_p;
     m_p = nullptr;
     m_size = 0;
     m_begin = 0;
     m_end = 0;
     m_cap = 0;
}

template< typename T >
RingBuffer< T >::RingBuffer( const RingBuffer& other )
{
     Init( other.size() );

     int ind = 0;
     for( const auto& el : other )
          m_p[ ind++ ] = el;
}

template< typename T >
RingBuffer< T >& RingBuffer< T >::operator=( const RingBuffer& other )
{
     if( this == &other )
          return *this;

     Init( other.size() );

     int ind = 0;
     for( const auto& el : other )
          m_p[ ind++ ] = el;

     return *this;
}

template< typename T >
RingBuffer< T >::RingBuffer( RingBuffer&& other )
     : m_p { other.m_p }
     , m_size { other.m_size }
     , m_cap { other.m_cap }
     , m_begin { other.m_begin }
     , m_end { other.m_end }
{
     other.m_p = nullptr;
     other.m_size = 0;
     other.m_begin = 0;
     other.m_end = 0;
     other.m_cap = 0;
}

template< typename T >
RingBuffer< T >& RingBuffer< T >::operator=( RingBuffer&& other )
{
     if( this == &other )
          return *this;

     delete[] m_p;

     m_p = other.m_p;
     m_size = other.m_size;
     m_cap = other.m_cap;
     m_begin = other.m_begin;
     m_end = other.m_end;

     other.m_p = nullptr;
     other.m_size = 0;
     other.m_begin = 0;
     other.m_end = 0;
     other.m_cap = 0;

     return *this;
}

} // namespace pkus
