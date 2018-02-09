#ifndef TIMELINE_H_
#define TIMELINE_H_

#include "Hop.h"
#include "Lod.h"

#include <vector>

namespace hop
{
struct ThreadInfo;
class Timeline
{
  public:
   void update( float deltaTimeMs ) noexcept;
   void draw(
       const std::vector<ThreadInfo>& _tracesPerThread,
       const std::vector<uint32_t>& threadIds );
   TimeStamp absoluteStartTime() const noexcept;
   TimeStamp absolutePresentTime() const noexcept;
   void setAbsoluteStartTime( TimeStamp time ) noexcept;
   void setAbsolutePresentTime( TimeStamp time ) noexcept;
   int64_t microsToDisplay() const noexcept;
   float windowWidthPxl() const noexcept;

   // Move to first trace
   void moveToStart( bool animate = true ) noexcept;
   // Move to latest time
   void moveToPresentTime( bool animate = true ) noexcept;
   // Move timeline so the specified time is in the middle
   void moveToTime( int64_t timeInMicro, bool animate = true ) noexcept;
   // Update timeline to always display last race
   void setRealtime( bool isRealtime ) noexcept;
   bool realtime() const noexcept;

  private:
   void drawTimeline( const float posX, const float posY );
   void drawTraces( const ThreadInfo& traces, int threadIndex, const float posX, const float posY );
   void drawLockWaits( const ThreadInfo& traces, const float posX, const float posY );
   void handleMouseDrag( float mousePosX, float mousePosY );
   void handleMouseWheel( float mousePosX, float mousePosY );
   void zoomOn( int64_t microToZoomOn, float zoomFactor );
   void selectTrace( const ThreadInfo& data, uint32_t threadIndex, size_t traceIndex );
   void setStartMicro( int64_t timeInMicro, bool withAnimation = true ) noexcept;
   void setZoom( uint64_t microsToDisplay, bool withAnimation = true );

   int64_t _startMicros{0};
   uint64_t _microsToDisplay{50000};
   int64_t _stepSizeInMicros{1000};
   TimeStamp _absoluteStartTime{};
   TimeStamp _absolutePresentTime{};
   float _rightClickStartPosInCanvas[2] = {};
   TDepth_t _maxTraceDepthPerThread[MAX_THREAD_NB] = {};
   float _windowWidthPxl{0};
   bool _realtime{true};

   struct Selection
   {
      static constexpr size_t NONE = -1;
      uint32_t threadIndex;
      size_t id{NONE};
      size_t lodIds[ LOD_COUNT ];
   } _selection;

   struct AnimationState
   {
      int64_t targetStartMicros{0};
      uint64_t targetMicrosToDisplay{50000};
   } _animationState;
};
}

#endif  // TIMELINE_H_