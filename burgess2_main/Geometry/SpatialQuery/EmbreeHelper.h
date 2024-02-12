#pragma once
#include <burgess2_main/Math.h>

// embree
#include <embree/embree_vc14/include/embree3/rtcore.h>
#include <embree/embree_vc14/include/embree3/rtcore_ray.h>

template<typename T> class PolygonMeshSpatialQueryT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***************************************************************************************************** ドキュメントには載らない*/
/// @cond
namespace BurgessEmbree{
	#define MAX_TOTAL_HITS 100
	#define g_max_next_hits u32(100)
	#define g_max_total_hits u32(100)
	
	template<typename T>
	struct HitList {
		// Hit structure that defines complete order over hits
		template<typename T>
		struct HitT {
			Bool opaque;
			T t, u, v;
			u32 primID, geomID, instID;

			HitT () {}
			HitT ( Bool opaque, T t, T u, T v, u32 primID, u32 geomID, u32 instID ) : opaque ( opaque ), t ( t ), primID ( primID ), geomID ( geomID ), instID ( instID ), u ( u ), v ( v ) {}

			friend std::ostream& operator<<( std::ostream& cout, const HitT<T>& hit ) { return cout << "Hit { opaque = " << hit.opaque << ", t = " << hit.t << ", instID = " << hit.instID << ", geomID = " << hit.geomID << ", primID = " << hit.primID << " }"; }
			__forceinline friend bool operator == ( const HitT<T>& a, const HitT<T>& b ) { return a.t == b.t && a.primID == b.primID && a.geomID == b.geomID && a.instID == b.instID; }
			__forceinline friend bool operator <= ( const HitT<T>& a, const HitT<T>& b ) { if (a == b) { return true; } else { return a < b; } }
			__forceinline friend bool operator != ( const HitT<T>& a, const HitT<T>& b ) { return !(a == b); }
			__forceinline friend bool operator <  ( const HitT<T>& a, const HitT<T>& b ) {
				// lexicographical order (t,instID,geomID,primID)
				if (a.t == b.t) {
					if (a.instID == b.instID) {
						if (a.geomID == b.geomID) return a.primID < b.primID;
						else                      return a.geomID < b.geomID;
					}
					else return a.instID < b.instID;
				}
				return a.t < b.t;
			}
		};

		u32 begin;   // begin of hit list
		u32 end;     // end of hit list
		u32 maxHitCount;
		u8 side; // 0=bothFaces, 1=frontFace, 2=backFace
		HitT<T> hits[MAX_TOTAL_HITS];   // array to store all found hits to

		HitList () : begin ( 0 ), end ( 0 ) {}
		HitList ( u32 maxHitCount ) : begin ( 0 ), end ( 0 ), maxHitCount ( maxHitCount ) {}

		u32 size () const { return this->end - this->begin; } // return number of gathered hits
		const HitT<T>& last () const { return this->hits[end - 1]; } // returns the last hit
		Bool last_is_opaque () const { return this->size () && this->last ().opaque; } // checks if the last hit is opaque

		using Hit = HitT<T>;
	};

	template<typename T>
	struct IntersectContext {
		RTCIntersectContext context;
		HitList<T>& hits;
		u32 max_next_hits; // maximal number of hits to collect in a single pass

		IntersectContext ( HitList<T>& hits ) : hits ( hits ), max_next_hits ( g_max_next_hits ) {}
	};

	template<typename T>
	struct PointQuery {
		std::vector<Vec3T<T>>* pointPositionsPtr;
		std::vector<u32>* packedPointIDsPtr;
		Vec3T<T> restClosestPosition;
		u32  restClosestTriangleID;
		u8 closestFaceSide;

		PointQuery (){}
	};

	template<typename T>
	struct TrianglePolygonInfo {
		u32 origPolygonID;
		s8 localTriID;
	};

	/////////////////////////////
	// Query functions
	template<typename T>
	void gather_all_hits ( const struct RTCFilterFunctionNArguments* args );

	template<typename T>
	HitList<T> getRayIntersect_task ( const RTCScene& scene, RTCRayHit& rayhit, u8 faceSide = 0, u32 maxHitCount = MAX_TOTAL_HITS );

	template<typename T>
	Bool closestPointFunc ( RTCPointQueryFunctionArguments* args );

	template<typename T>
	RTCRayHit toEmbreeRay(const Ray3T<T>& ray3, T maxDistance = T(BURGESS2::F32_INFINITE));



	template<typename T>
	HitList<T> getRayIntersect_task_test( const RTCScene& scene, RTCRayHit& rayhit, u8 faceSide = 0, u32 maxHitCount = MAX_TOTAL_HITS );
}

#include "EmbreeHelper.inl"
/// @endcond
/*****************************************************************************************************ドキュメントには載らない*/

//******************************************************************************
//! End of File
//******************************************************************************