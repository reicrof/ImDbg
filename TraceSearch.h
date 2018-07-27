#ifndef TRACE_SEARCH_H_
#define TRACE_SEARCH_H_

#include <vector>
#include <string>

namespace hop
{
   class Timeline;
   struct TimelineTrack;
   class StringDb;

   struct SearchResult
   {
      std::string stringSearched;
      std::vector< std::pair< size_t, uint32_t> > tracesIdxThreadIdx;
      size_t matchCount{0};
   };

   struct SearchSelection
   {
      size_t selectedTraceIdx;
      size_t hoveredTraceIdx;
      uint32_t selectedThreadIdx;
      uint32_t hoveredThreadIdx;
   };

   void findTraces( const char* string, const StringDb& strDb, const std::vector< TimelineTrack >& tracks, SearchResult& result );
   SearchSelection drawSearchResult( SearchResult& searchRes, const Timeline& timeline, const StringDb& strDb, const std::vector< TimelineTrack >& tracks );
   void clearSearchResult( SearchResult& res );
}

#endif //TRACE_SEARCH_H_