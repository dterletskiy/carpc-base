#pragma once

#include <functional>
#include <memory>

#include "carpc/base/types/ID.hpp"

#include "carpc/trace/trace.h"



namespace carpc::callback {

   class SharedCallback;
   using ID = carpc::TID< SharedCallback >;
   using Name = carpc::TName< SharedCallback >;



   class SharedCallback
   {
      protected:
         SharedCallback( )
         {
            CARPC_TRACE_LOG_TRACE( "created" );
         }

      public:
         virtual ~SharedCallback( )
         {
            CARPC_TRACE_LOG_TRACE( "destroyed" );
         }

         const ID& id( ) const
         {
            return m_id;
         }

         template< typename TWeakPtr >
         [[nodiscard]] static bool is_locked( const TWeakPtr& wp )
         {
            return wp.use_count( ) != 0;
         }

         template< typename TWeakPtr >
         [[nodiscard]] static bool is_unlocked( const TWeakPtr& wp )
         {
            return wp.use_count( ) == 0;
         }

      private:
         ID m_id = ID::generate( );
   };



   template< typename DATA >
   class TSharedCallback : public SharedCallback
   {
      public:
         using tObject = TSharedCallback< DATA >;
         using tSptr = std::shared_ptr< tObject >;
         using tWptr = std::weak_ptr< tObject >;
         using tData = DATA;
         using tFinalizer = std::function< void( const tData& ) >;

      private:
         explicit TSharedCallback( tFinalizer finalizer, tData data )
            : m_finalizer( std::move(finalizer) )
            , m_data( std::move(data) )
         {
         }

         template< typename... Args >
         explicit TSharedCallback( tFinalizer finalizer, Args&&... args )
            : m_finalizer( std::move(finalizer) )
            , m_data( std::forward<Args>(args)... )
         {
         }

      public:
         ~TSharedCallback( ) override
         {
            try
            {
               if( m_finalizer )
                  m_finalizer( m_data );
            }
            catch( const std::exception& e )
            {
               CARPC_TRACE_LOG_TRACE( "exception: %s", e.what( ) );
            }
            catch(...)
            {
               CARPC_TRACE_LOG_TRACE( "unknown exception" );
            }
         }

      public:
         static tSptr create( tFinalizer finalizer, tData data )
         {
            struct MakeSharedEnabler : public TSharedCallback
            {
               explicit MakeSharedEnabler( tFinalizer finalizer, tData data )
                  : TSharedCallback(
                        std::move( finalizer ),
                        std::move( data )
                     )
               {
               }
            };
            return std::make_shared< MakeSharedEnabler >(
                  std::move( finalizer ), std::move( data )
               );
         }

         template< typename... Args >
         static tSptr create( tFinalizer finalizer, Args&&... args )
         {
            struct MakeSharedEnabler : public TSharedCallback
            {
               explicit MakeSharedEnabler( tFinalizer finalizer, Args&&... args )
                  : TSharedCallback(
                        std::move( finalizer ),
                        std::forward< Args >( args )...
                     )
               {
               }
            };

            return std::make_shared< MakeSharedEnabler >(
                  std::move( finalizer ), std::forward< Args >( args )...
               );
         }

         void reset_finalizer( )
         {
            m_finalizer = nullptr;
         }

      private:
         tFinalizer  m_finalizer;
         tData       m_data;
   };



   template< >
   class TSharedCallback< void > : public SharedCallback
   {
      public:
         using tObject = TSharedCallback< void >;
         using tSptr = std::shared_ptr< tObject >;
         using tWptr = std::weak_ptr< tObject >;
         using tFinalizer = std::function< void( ) >;

      private:
         explicit TSharedCallback( tFinalizer finalizer )
            : m_finalizer( std::move(finalizer) )
         {
         }

      public:
         ~TSharedCallback( ) override
         {
            try
            {
               if( m_finalizer )
                  m_finalizer( );
            }
            catch( const std::exception& e )
            {
               CARPC_TRACE_LOG_TRACE( "exception: %s", e.what( ) );
            }
            catch(...)
            {
               CARPC_TRACE_LOG_TRACE( "unknown exception" );
            }
         }

      public:
         static tSptr create( tFinalizer finalizer )
         {
            struct MakeSharedEnabler : public TSharedCallback
            {
               explicit MakeSharedEnabler( tFinalizer finalizer )
                  : TSharedCallback( std::move( finalizer ) )
               {
               }
            };
            return std::make_shared< MakeSharedEnabler >(
                  std::move(finalizer)
               );
         }

         void reset_finalizer( )
         {
            m_finalizer = nullptr;
         }

      private:
         tFinalizer m_finalizer;
   };

   using tBlockerRoot = TSharedCallback< void >;
   using tBlocker = tBlockerRoot::tSptr;
   using tBlockerLink = tBlockerRoot::tWptr;

} // namespace carpc::callback
