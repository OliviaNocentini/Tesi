#include <vector>
#include <type_traits>

template < typename T > using matrix = std::vector< std::vector<T> > ;

template < typename T >
typename std::enable_if< std::is_arithmetic<T>::value, matrix<T> >::type
make_vandermonde_matrix( const std::vector<T>& xvals )
{
    const std::size_t n = xvals.size() ;
    matrix<T> result ;
    for( const T& x : xvals )
    {
        result.push_back( std::vector<T>( 1U, 1 ) ) ;
        T prev = 1 ;
        for( std::size_t i = 1 ; i < n ; ++i ) result.back().push_back( prev *= x ) ;
    }
    return result ;
}

int main()
{
    std::vector<double> xvals = { 1.2, 2.3, 3.4, 4.5 } ;
    auto vandermonde = make_vandermonde_matrix(xvals) ;
}