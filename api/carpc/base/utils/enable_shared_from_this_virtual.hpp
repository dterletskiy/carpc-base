#pragma once
#include <memory>
#include <cassert>

namespace util
{

/**
 * @brief Base class for virtual enable_shared_from_this support.
 *
 * This class provides a single weak_ptr to manage shared ownership
 * for objects in complex inheritance hierarchies, including
 * multiple and virtual inheritance.
 *
 * @note All derived objects must be created using std::shared_ptr.
 *       Calling shared_from_this on objects not managed by shared_ptr
 *       results in undefined behavior (assert triggers in debug build).
 */
class enable_shared_from_this_virtual_base
    : public std::enable_shared_from_this<
            enable_shared_from_this_virtual_base
        >
{
    using base_type = std::enable_shared_from_this< enable_shared_from_this_virtual_base >;

    template< typename T >
    friend struct enable_shared_from_this_virtual;

protected:
    /**
     * @brief Returns a shared_ptr to this base object.
     *
     * @return std::shared_ptr<enable_shared_from_this_virtual_base>
     *
     * @note Asserts if the object is not managed by shared_ptr.
     */
    std::shared_ptr< enable_shared_from_this_virtual_base >
        shared_from_this_base( )
    {
        auto sp = base_type::shared_from_this( );
        assert(sp);
        return sp;
    }

    /**
     * @brief Returns a shared_ptr to this base object (const version).
     *
     * @return std::shared_ptr<enable_shared_from_this_virtual_base const>
     *
     * @note Asserts if the object is not managed by shared_ptr.
     */
    std::shared_ptr< enable_shared_from_this_virtual_base const >
        shared_from_this_base( ) const
    {
        auto sp = base_type::shared_from_this( );
        assert(sp);
        return sp;
    }
};

/**
 * @brief Template to enable shared_from_this for virtual/multiple inheritance.
 *
 * Provides safe shared_from_this() methods for derived type T.
 *
 * @tparam T The derived class type.
 *
 * @note Derive virtually from this template if multiple inheritance is used.
 * @note Objects must be created via std::shared_ptr.
 *
 * @example
 * struct A : util::enable_shared_from_this_virtual<A> { };
 * auto a = std::make_shared<A>();
 * auto sp = a->shared_from_this();
 */
template< typename T >
struct enable_shared_from_this_virtual
    : virtual enable_shared_from_this_virtual_base
{
protected:
    using base_type = enable_shared_from_this_virtual_base;

public:
    /**
     * @brief Returns a shared_ptr to this object.
     *
     * @return std::shared_ptr<T> Shared pointer to the derived object.
     *
     * @note Asserts if the object is not managed by shared_ptr.
     */
    std::shared_ptr< T > shared_from_this( ) noexcept
    {
        auto base_sp = base_type::shared_from_this_base( );
        return std::shared_ptr< T >( base_sp, static_cast< T* >( this ) );
    }

    /**
     * @brief Returns a shared_ptr<const T> to this object.
     *
     * @return std::shared_ptr<const T> Shared pointer to const derived object.
     *
     * @note Asserts if the object is not managed by shared_ptr.
     */
    std::shared_ptr< T const > shared_from_this( ) const noexcept
    {
        auto base_sp = base_type::shared_from_this_base( );
        return std::shared_ptr< T const >(
                base_sp, static_cast< T const* >( this )
            );
    }
};

} // namespace util
