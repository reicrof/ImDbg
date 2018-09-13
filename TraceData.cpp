#include "TraceData.h"

#include <algorithm>
#include <cassert>

namespace hop
{

void TraceData::append( const TraceData& newTraces )
{
   const size_t prevSize = deltas.size();

   deltas.insert( deltas.end(), newTraces.deltas.begin(), newTraces.deltas.end() );
   ends.insert( ends.end(), newTraces.ends.begin(), newTraces.ends.end() );
   fileNameIds.insert(
       fileNameIds.end(), newTraces.fileNameIds.begin(), newTraces.fileNameIds.end() );
   fctNameIds.insert( fctNameIds.end(), newTraces.fctNameIds.begin(), newTraces.fctNameIds.end() );
   lineNbs.insert( lineNbs.end(), newTraces.lineNbs.begin(), newTraces.lineNbs.end() );
   zones.insert( zones.end(), newTraces.zones.begin(), newTraces.zones.end() );
   depths.insert( depths.end(), newTraces.depths.begin(), newTraces.depths.end() );
   maxDepth = std::max(maxDepth, newTraces.maxDepth);

   appendLods( lods, computeLods( newTraces, prevSize ) );
}

void TraceData::clear()
{
   ends.clear();
   deltas.clear();
   fileNameIds.clear();
   fctNameIds.clear();
   lineNbs.clear();
   zones.clear();
   depths.clear();
   maxDepth = 0;
   for( auto& dq : lods )
      dq.clear();
}

TraceData TraceData::copy() const
{
   TraceData copy;
   copy.ends = this->ends;
   copy.deltas = this->deltas;
   copy.fileNameIds = this->fileNameIds;
   copy.fctNameIds = this->fctNameIds;
   copy.lineNbs = this->lineNbs;
   copy.zones = this->zones;
   copy.depths = this->depths;
   copy.lods = this->lods;
   copy.maxDepth = this->maxDepth;
   return copy;
}

void LockWaitData::append( const LockWaitData& newLockWaits )
{
   const size_t prevSize = ends.size();

   ends.insert( ends.end(), newLockWaits.ends.begin(), newLockWaits.ends.end() );
   deltas.insert( deltas.end(), newLockWaits.deltas.begin(), newLockWaits.deltas.end() );
   depths.insert( depths.end(), newLockWaits.depths.begin(), newLockWaits.depths.end() );
   mutexAddrs.insert( mutexAddrs.end(), newLockWaits.mutexAddrs.begin(), newLockWaits.mutexAddrs.end() );

   const size_t lockReleaseSize = newLockWaits.lockReleases.size();
   if( lockReleaseSize == 0 )
   {
      // Append 0 for lock releases. They will be filled when the unlock event are received
      lockReleases.resize(
          lockReleases.size() + std::distance( newLockWaits.ends.begin(), newLockWaits.ends.end() ),
          0 );
   }
   else
   {
      // We are probably reading from a file since the lockrealse are already processed.
      assert( lockReleaseSize == newLockWaits.ends.size() );
      lockReleases.insert( lockReleases.end(), newLockWaits.lockReleases.begin(), newLockWaits.lockReleases.end() );
   }

   appendLods( lods, computeLods( newLockWaits, prevSize ) );
}

void LockWaitData::clear()
{
   ends.clear();
   deltas.clear();
   depths.clear();
   mutexAddrs.clear();
   lockReleases.clear();
   for( auto& dq : lods )
      dq.clear();
}



template std::pair<size_t, size_t> visibleIndexSpan<TraceData>(
    const TraceData& traces,
    TimeStamp absoluteStart,
    TimeStamp absoluteEnd );

template std::pair<size_t, size_t> visibleIndexSpan<LockWaitData>(
    const LockWaitData& traces,
    TimeStamp absoluteStart,
    TimeStamp absoluteEnd );

template< typename Ts >
std::pair<size_t, size_t> visibleIndexSpan(
    const Ts& traces,
    TimeStamp absoluteStart,
    TimeStamp absoluteEnd )
{
   auto span = std::make_pair( hop::INVALID_IDX, hop::INVALID_IDX );
   const auto it1 = std::lower_bound( traces.ends.begin(), traces.ends.end(), absoluteStart );
   const auto it2 = std::upper_bound( traces.ends.begin(), traces.ends.end(), absoluteEnd );

   // The last trace does not reach the specified time range
   if ( it1 == traces.ends.end() ) return span;

   span.first = std::distance( traces.ends.begin(), it1 );
   span.second = std::distance( traces.ends.begin(), it2 );

   // Find the the first trace on both size with depths of zero to prevent returning
   // part of the trace stack
   while ( span.first > 0 && traces.depths[span.first] != 0 )
   {
      --span.first;
   }
   while ( span.second < traces.depths.size() && traces.depths[span.second] != 0 )
   {
      ++span.second;
   }
   // We need to go one past the depth 0
   if ( span.second < traces.depths.size() )
   {
      ++span.second;
   }

   return span;
}

std::pair<size_t, size_t> visibleIndexSpan(
    const LodsArray& lodsArr,
    int lodLvl,
    TimeStamp absoluteStart,
    TimeStamp absoluteEnd,
    int baseDepth )
{
   auto span = std::make_pair( hop::INVALID_IDX, hop::INVALID_IDX );

   const auto& lods = lodsArr[lodLvl];
   const LodInfo firstInfo = {absoluteStart, 0, 0, 0, false};
   const LodInfo lastInfo = {absoluteEnd, 0, 0, 0, false};
   auto it1 = std::lower_bound( lods.begin(), lods.end(), firstInfo );
   auto it2 = std::upper_bound( lods.begin(), lods.end(), lastInfo );

   // The last trace of the current thread does not reach the current time
   if ( it1 == lods.end() ) return span;

   // Find the the first trace on right that have a depth of "baseDepth". This can be either 0
   // for traces or 1 for lockwaits. This prevents traces that have a smaller depth than the
   // one foune previously to vanish.
   while ( it2 != lods.end() && it2->depth != baseDepth )
   {
      ++it2;
   }
   if ( it2 != lods.end() )
   {
      ++it2;
   }  // We need to go one past the depth 0

   span.first = std::distance( lods.begin(), it1 );
   span.second = std::distance( lods.begin(), it2 );
   return span;
}
}