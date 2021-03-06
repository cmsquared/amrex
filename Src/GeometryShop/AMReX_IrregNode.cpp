

/*
 *      .o.       ooo        ooooo ooooooooo.             ooooooo  ooooo 
 *     .888.      `88.       .888' `888   `Y88.            `8888    d8'  
 *    .8"888.      888b     d'888   888   .d88'  .ooooo.     Y888..8P    
 *   .8' `888.     8 Y88. .P  888   888ooo88P'  d88' `88b     `8888'     
 *  .88ooo8888.    8  `888'   888   888`88b.    888ooo888    .8PY888.    
 * .8'     `888.   8    Y     888   888  `88b.  888    .o   d8'  `888b   
 *o88o     o8888o o8o        o888o o888o  o888o `Y8bod8P' o888o  o88888o 
 *
 */

#include "AMReX_IrregNode.H"

namespace amrex
{

  /*******************************/
  int IrregNode::
  index(int a_idir, Side::LoHiSide a_sd)
  {
    assert(a_idir >= 0 && a_idir < SpaceDim);
    int retval;
    if (a_sd == Side::Lo)
      {
        retval = a_idir;
      }
    else
      {
        retval = a_idir + SpaceDim;
      }
    return retval;
  }
  /*******************************/

  void IrregNode::makeRegular(const IntVect& iv, const Real& a_dx)
  {
    m_cell = iv;
    m_volFrac = 1.0;
    m_cellIndex = 0;
    m_volCentroid   = RealVect::Zero;
    m_bndryCentroid = RealVect::Zero;
    //low sides
    for (int i=0; i<SpaceDim; i++)
      {
        m_arc[i].resize(1,0);
        m_areaFrac[i].resize(1,1.0);
        RealVect faceCenter = RealVect::Zero;
        faceCenter[i] = -0.5;
        m_faceCentroid[i].resize(1,faceCenter);
      }
    //hi sides
    for (int i=0; i<SpaceDim; i++)
      {
        m_arc[i+SpaceDim].resize(1,0);
        m_areaFrac[i+SpaceDim].resize(1,1.0);
        RealVect faceCenter = RealVect::Zero;
        faceCenter[i] = 0.5;
        m_faceCentroid[i+SpaceDim].resize(1,faceCenter);
      }
  }

}
