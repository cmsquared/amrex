#include "ShortRangeParticleContainer.H"

#include "short_range_F.H"

using namespace amrex;

ShortRangeParticleContainer::
ShortRangeParticleContainer(const Geometry            & geom,
                            const DistributionMapping & dmap,
                            const BoxArray            & ba,
                            int                         ncells)
    : NeighborParticleContainer<2*BL_SPACEDIM, 0, 2*BL_SPACEDIM>
      (geom, dmap, ba, ncells)
{}

void ShortRangeParticleContainer::InitParticles() {
    
    BL_PROFILE("ShortRangeParticleContainer::InitParticles");
    
    const int lev = 0;
    const Geometry& geom = Geom(lev);
    const Real* dx  = geom.CellSize();
    
    std::mt19937 mt(0451);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    
    for (MFIter mfi = MakeMFIter(lev); mfi.isValid(); ++mfi) {
        const Box& tile_box = mfi.tilebox();
        const RealBox tile_real_box { tile_box, dx, geom.ProbLo() };
        
        const int grid_id = mfi.index();
        const int tile_id = mfi.LocalTileIndex();
        auto& particle_tile = GetParticles(lev)[std::make_pair(grid_id, tile_id)];
        
        const auto& boxlo = tile_box.smallEnd();
        ParticleType p;
        for (IntVect iv = tile_box.smallEnd(); iv <= tile_box.bigEnd(); tile_box.next(iv)) {
            p.id() = ParticleType::NextID();
            p.cpu() = ParallelDescriptor::MyProc();
            
            p.pos(0) = tile_real_box.lo(0) + (iv[0]- boxlo[0] + 0.5)*dx[0];
            p.pos(1) = tile_real_box.lo(1) + (iv[1]- boxlo[1] + 0.5)*dx[1];
#if (BL_SPACEDIM == 3)
            p.pos(2) = tile_real_box.lo(2) + (iv[2]- boxlo[2] + 0.5)*dx[2];
#endif
            p.rdata(0) = dist(mt);
            p.rdata(1) = dist(mt);
#if (BL_SPACEDIM == 3)
            p.rdata(2) = dist(mt);
#endif            
            p.rdata(BL_SPACEDIM)   = 0;
            p.rdata(BL_SPACEDIM+1) = 0;
#if (BL_SPACEDIM == 3)
            p.rdata(BL_SPACEDIM+2) = 0;
#endif            
            particle_tile.push_back(p);
        }
    }
}

void ShortRangeParticleContainer::computeForces() {

    BL_PROFILE("ShortRangeParticleContainer::computeForces");
    const int lev = 0;

#ifdef _OPENMP
#pragma omp parallel
#endif
    for (MyParIter pti(*this, lev); pti.isValid(); ++pti) {
        AoS& particles = pti.GetArrayOfStructs();
        int Np = pti.numParticles();
        PairIndex index(pti.index(), pti.LocalTileIndex());
        int Nn = neighbors[index].size() / pdata_size;
        amrex_compute_forces(particles.data(), &Np, 
                             neighbors[index].dataPtr(), &Nn);
    }        
}

void ShortRangeParticleContainer::moveParticles(const Real dt) {

    BL_PROFILE("ShortRangeParticleContainer::moveParticles");

    const int lev = 0;
    const RealBox& prob_domain = Geom(lev).ProbDomain();

#ifdef _OPENMP
#pragma omp parallel
#endif
    for (MyParIter pti(*this, lev); pti.isValid(); ++pti) {
        AoS& particles = pti.GetArrayOfStructs();
        int Np = pti.numParticles();
        amrex_move_particles(particles.data(), &Np, &dt,
                             prob_domain.lo(), prob_domain.hi());
    }
}

void ShortRangeParticleContainer::writeParticles(int n) {

    BL_PROFILE("ShortRangeParticleContainer::writeParticles");

    const std::string& pltfile = amrex::Concatenate("particles", n, 5);
    WriteAsciiFile(pltfile);
}
