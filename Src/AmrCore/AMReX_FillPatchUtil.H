#ifndef _FillPatchUtil_H_
#define _FillPatchUtil_H_

#include <AMReX_MultiFab.H>
#include <AMReX_Geometry.H>
#include <AMReX_PhysBCFunct.H>
#include <AMReX_Interpolater.H>
#include <array>

namespace amrex
{
    bool ProperlyNested (const IntVect& ratio, const IntVect& blockint_factor, int ngrow, 
			 const IndexType& boxType, Interpolater* mapper);

    void FillPatchSingleLevel (MultiFab& mf, Real time, 
			       const Array<MultiFab*>& smf, const Array<Real>& stime, 
			       int scomp, int dcomp, int ncomp,
			       const Geometry& geom, PhysBCFunctBase& physbcf);

    void FillPatchTwoLevels (MultiFab& mf, Real time,
			     const Array<MultiFab*>& cmf, const Array<Real>& ct,
			     const Array<MultiFab*>& fmf, const Array<Real>& ft,
			     int scomp, int dcomp, int ncomp,
			     const Geometry& cgeom, const Geometry& fgeom, 
			     PhysBCFunctBase& cbc, PhysBCFunctBase& fbc,
			     const IntVect& ratio, 
			     Interpolater* mapper, const Array<BCRec>& bcs);

    void InterpFromCoarseLevel (MultiFab& mf, Real time,
				const MultiFab& cmf, int scomp, int dcomp, int ncomp,
				const Geometry& cgeom, const Geometry& fgeom, 
				PhysBCFunctBase& cbc, PhysBCFunctBase& fbc, const IntVect& ratio, 
				Interpolater* mapper, const Array<BCRec>& bcs);

    void InterpCrseFineBndryBfield (const std::array<MultiFab,BL_SPACEDIM>& crse,
                                    std::array<MultiFab,BL_SPACEDIM>& fine,
                                    const Geometry& cgeom, const Geometry& fgeom,
                                    int ref_ratio);

    enum InterpEM_t { InterpE, InterpB};
    
    void InterpCrseFineBndryEMfield (InterpEM_t interp_type,
                                     const std::array<MultiFab,BL_SPACEDIM>& crse,
                                     std::array<MultiFab,BL_SPACEDIM>& fine,
                                     const Geometry& cgeom, const Geometry& fgeom,
                                     int ref_ratio);
}

#endif
