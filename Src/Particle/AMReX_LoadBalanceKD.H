#ifndef AMREX_LOADBALANCEKD_H_
#define AMREX_LOADBALANCEKD_H_

#include <AMReX_Box.H>
#include <AMReX_FArrayBox.H>
#include <AMReX_Particles.H>
#include <AMReX_MultiFab.H>

class KDTree {

    struct KDNode {
        
        amrex::Box box;
        KDNode* left;
        KDNode* right;
        amrex::Real cost;
        int num_procs_left;    
        
        KDNode(const amrex::Box& box_in, 
               amrex::Real cost_in, 
               int num_procs_in)
            : box(box_in),
              cost(cost_in),
              num_procs_left(num_procs_in)
        {
            left = NULL;
            right = NULL;
        }
    };
        
public:
    
    KDTree(const amrex::Box& domain, const amrex::FArrayBox& cost, int num_procs);
    
    ~KDTree();
    
    void GetBoxes(amrex::BoxList& bl, amrex::Array<amrex::Real>& costs);
    
private:
    
    void buildKDTree(KDNode* node, const amrex::FArrayBox& cost);

    void freeKDTree(KDNode* node);

    void walkKDTree(KDNode* node, amrex::BoxList& bl, amrex::Array<amrex::Real>& costs);
    
    void partitionNode(KDNode* node, const amrex::FArrayBox& cost);

    int getLongestDir(const amrex::Box& box);
    
    void splitBox(int split, int dir,
                  const amrex::Box& box, amrex::Box& left, amrex::Box& right);

    KDNode* root;
    
};

namespace loadBalanceKD {

    template <typename T>
    void computeCost(T& myPC, amrex::MultiFab& local_cost, 
                     amrex::MultiFab& global_cost, const amrex::Box& domain) {
        
        const int lev = 0;
        const amrex::BoxArray& ba = myPC.ParticleBoxArray(lev);
        const amrex::DistributionMapping& dm = myPC.ParticleDistributionMap(lev);
        
        amrex::BoxList global_bl;
        amrex::Array<int> procs_map;
        for (int i = 0; i < amrex::ParallelDescriptor::NProcs(); ++i) {
            global_bl.push_back(domain);
            procs_map.push_back(i);
        }
        
        amrex::BoxArray global_ba(global_bl);    
        amrex::DistributionMapping global_dm(procs_map);    
        
        local_cost.define(ba, dm, 1, 0);
        local_cost.setVal(0.0);
        myPC.Increment(local_cost, lev);
        
        global_cost.define(global_ba, global_dm, 1, 0);
        global_cost.copy(local_cost, 0, 0, 1);
    }

    template <typename T>
    void balance(T& myPC, amrex::BoxArray& new_ba, int num_procs) {
        
        const int lev = 0;
        const amrex::Geometry& geom = myPC.Geom(lev);
        const amrex::Box& domain = geom.Domain();
        
        amrex::MultiFab local_cost;
        amrex::MultiFab global_cost;    
        computeCost<T>(myPC, local_cost, global_cost, domain);
        
        amrex::FArrayBox *cost;
        for ( amrex::MFIter mfi(global_cost); mfi.isValid(); ++mfi ) {
            cost = &global_cost[mfi];
        }
        
        KDTree tree = KDTree(domain, *cost, num_procs);
        
        amrex::BoxList new_bl;
        amrex::Array<amrex::Real> box_costs;
        tree.GetBoxes(new_bl, box_costs);
        new_ba.define(new_bl);
    }
}

#endif // AMREX_LOADBALANCEKD_H_
