#ifndef STRING_DB_H_
#define STRING_DB_H_

#include "Hop.h"

#include <vector>
#include <unordered_map>

namespace hop
{

class StringDb
{
public:
   StringDb();
   bool empty() const;
   void addStringData( const std::vector<char>& inData );
   void addStringData( const char* inData, size_t count );
   size_t getStringIndex( hop::TStrPtr_t strId ) const;
   inline const char* getString( size_t index ) const
   {
      return &_strData[ index ];
   }

   void formatTraceName(size_t classNameIndex, size_t fctNameIndex, char* nameArray, size_t size) const;

private:
   std::unordered_map< hop::TStrPtr_t, size_t > _stringIndices;
   std::vector< char > _strData;
};

} // namespace hop

#endif // STRING_DB_H_