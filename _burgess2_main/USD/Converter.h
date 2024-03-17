#pragma once
#include <burgess2_main/Core/base.h>
#include <burgess2_main/Math.h>

#define NOMINMAX
#pragma warning(push)
#pragma warning(disable: 4244 4267 4305 6011 6319 26439 26451 26495 26812)
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/variantSets.h>
#include <pxr/usd/usd/editContext.h>
#include <pxr/usd/usd/modelApi.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/sphere.h>
#include <pxr/usd/usdGeom/xformCommonAPI.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/kind/registry.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdSkel/skeleton.h>

#pragma warning(pop)
#undef NOMINMAX

using namespace PXR_INTERNAL_NS;

template<typename T>
void Convert ( OUT Mat4T<T>& brg, const GfMatrix4d& usd );

template<typename T>
void Convert ( OUT std::vector<Mat4T<T>>& brg, const VtArray<GfMatrix4d>& usd );

template<typename T>
void Convert ( OUT GfMatrix4d& usd, const Mat4T<T>& brg );

template<typename T>
void Convert ( OUT VtArray<GfMatrix4d>& usd, const std::vector<Mat4T<T>>& brg );

#include "Converter.inl"

//******************************************************************************
//! End of File
//******************************************************************************