

#ifndef _GROUP_BY_H_
#define _GROUP_BY_H_

#include <map>
#include <vector>

namespace details
{
    /*
     * return_type
     */

    template< typename T >
    struct return_type
    {
        typedef typename return_type< typename std::decay< decltype( &T::operator() )>::type >::type type;
    };

    template< typename R, typename C, typename... A >
    struct return_type< R ( C::* )( A... ) > 
    {
        typedef typename std::decay< R >::type type;
    };

    template< typename R, typename C, typename... A >
    struct return_type< R ( C::* )( A... ) const > 
    {
        typedef typename std::decay< R >::type type;
    };

    template< typename R, typename... A >
    struct return_type< R ( * )( A... ) > 
    {
        typedef typename std::decay< R >::type type;
    };
}

namespace details
{
    /*
     * GroupBy
     */

    template< typename Arg, typename... Ts >
    struct GroupBy;

    template< typename Arg, typename T, typename... Ts >
    struct GroupBy< Arg, T, Ts... >
    {
        typedef std::map<
            typename details::return_type< typename std::decay< T >::type >::type,
            typename GroupBy< Arg, Ts... >::return_type
        > return_type;
    };

    template< typename Arg, typename T >
    struct GroupBy< Arg, T >
    {
        typedef std::map<
            typename return_type< typename std::decay< T >::type >::type,
            std::vector< Arg >
        > return_type;
    };
}

namespace details
{
    /*
     * groupByImpl
     */

    template< typename Map, typename Iterator, typename F >
    void groupByImpl( Map & map, Iterator && current, F && f )
    {
        map[ f( *current ) ].push_back( *current );
    }

    template< typename Map, typename Iterator, typename F, typename... Fs >
    void groupByImpl( Map & map, Iterator && current, F && f, Fs &&... fs )
    {
        groupByImpl( map[ f( *current ) ], current, std::forward< Fs >( fs )... );
    }
}

/*
 * groupBy
 */

template< typename Iterator, typename F, typename... Fs >
typename details::GroupBy< typename std::iterator_traits< Iterator >::value_type, F, Fs... >::return_type
groupBy( Iterator begin, Iterator const end, F && f, Fs &&... fs )
{
    typename details::GroupBy< typename std::iterator_traits< Iterator >::value_type, F, Fs... >::return_type result;
    for( /* empty */ ; begin != end ; ++ begin )
    {
        details::groupByImpl( result, begin, std::forward< F >( f ), std::forward< Fs >( fs )... );
    }

    return result;
}

#endif

