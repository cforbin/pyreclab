#ifndef __RECSYS_ALGORITHM_H__
#define __RECSYS_ALGORITHM_H__

#include "RatingMatrix.h"
#include "DataFrame.h"
#include "MaxHeap.h"

#include <string>
#include <utility>


template <class smatrix_t>
class RecSysAlgorithm
{
public:

   enum ETrainingEndCauses
   {
      FINISHED,
      STOPPED
   };

   typedef boost::numeric::ublas::mapped_matrix<double, boost::numeric::ublas::row_major> sparse_matrix_ro_t;

   typedef boost::numeric::ublas::mapped_matrix<double, boost::numeric::ublas::column_major> sparse_matrix_co_t;

   RecSysAlgorithm( DataReader& dreader,
                    int userpos = 0,
                    int itempos = 1,
                    int ratingpos = 2 )
   : m_ratingMatrix( dreader, userpos, itempos, ratingpos ),
     m_running( true )
   {
      m_globalMean = m_ratingMatrix.sumRatings()/m_ratingMatrix.numRatings();
   }

   virtual
   ~RecSysAlgorithm()
   {
   }

   virtual
   void reset();

   virtual
   double predict( std::string& userId, std::string& itemId );

   virtual
   double predict( size_t row, size_t col );

   virtual
   bool recommend( const std::string& userId,
                   size_t n,
                   std::vector<std::string>& ranking,
                   bool includeRated = false );

   virtual
   double loss();

   void stop()
   {
      m_running = false;
   }

protected:

   RatingMatrix<smatrix_t> m_ratingMatrix;

   double m_globalMean;

   bool m_running;

};


template <class smatrix_t>
void RecSysAlgorithm<smatrix_t>::reset()
{
}

template <class smatrix_t>
double RecSysAlgorithm<smatrix_t>::predict( std::string& userId, std::string& itemId )
{
   int userrow = m_ratingMatrix.row( userId );
   int itemcol = m_ratingMatrix.column( itemId );
   return predict( userrow, itemcol );
}

template <class smatrix_t>
double RecSysAlgorithm<smatrix_t>::predict( size_t row, size_t col )
{
   std::cerr << "Warning: predict method not implemented" << std::endl;
   return 0;
}

template <class smatrix_t>
bool RecSysAlgorithm<smatrix_t>::recommend( const std::string& userId,
                                            size_t n,
                                            std::vector<std::string>& ranking,
                                            bool includeRated )
{
   int row = m_ratingMatrix.row( userId );
   MaxHeap maxheap;
   for( size_t col = 0 ; col < m_ratingMatrix.items() ; ++col )
   {
      if( includeRated || ( 0 == m_ratingMatrix.get( row, col ) ) )
      {
         double rating = predict( row, col );
         std::pair<double, size_t> e = std::pair<double, size_t>( rating, col );
         maxheap.push( e );
      }
   }

   if( maxheap.empty() )
   {
      return false;
   }

   for( size_t i = 0 ; i < n ; ++i )
   {
      std::pair<double, size_t> e = maxheap.pop();
      ranking.push_back( m_ratingMatrix.itemId( e.second ) );
      if( maxheap.empty() )
      {
         break;
      }
      //std::cout << "(" << e.first << ", " << e.second << ") ";
   }
   //std::cout << std::endl;

   return true;
}

template <class smatrix_t>
double RecSysAlgorithm<smatrix_t>::loss()
{
   return 0;
}

#endif // __RECSYS_ALGORITHM_H__

