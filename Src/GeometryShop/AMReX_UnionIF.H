
/*
 *       {_       {__       {__{_______              {__      {__
 *      {_ __     {_ {__   {___{__    {__             {__   {__  
 *     {_  {__    {__ {__ { {__{__    {__     {__      {__ {__   
 *    {__   {__   {__  {__  {__{_ {__       {_   {__     {__     
 *   {______ {__  {__   {_  {__{__  {__    {_____ {__  {__ {__   
 *  {__       {__ {__       {__{__    {__  {_         {__   {__  
 * {__         {__{__       {__{__      {__  {____   {__      {__
 *
 */

#ifndef _UNIONIF_H_
#define _UNIONIF_H_

#include "AMReX_RealVect.H"
#include <vector>
#include "AMReX_BaseIF.H"

using std::vector;
namespace amrex
{

  ///
  /**
     This implicit function is the minimum of two (of more) implicit
     functions with which it is constructed.  This results in an implicit
     function whose interior is the union of the interiors of the given
     implicit functions.
  */
  class UnionIF: public BaseIF
  {
  public:

    ///
    UnionIF(const vector<BaseIF*>& a_impFuncs);
   
   
    /// Destructor
    virtual ~UnionIF();
   
    ///
    /**
       Return the value of the function at a_point.
    */
    virtual Real value(const RealVect& a_point) const;
   
   
    virtual BaseIF* newImplicitFunction() const;

   
   
  protected:
    int             m_numFuncs; // number of implicit functions
    vector<BaseIF*> m_impFuncs; // implicit functions
   
  private:
    UnionIF();
    /// Copy constructor
    UnionIF(const UnionIF& a_inputIF);
   
   
    void operator=(const UnionIF& a_inputIF);
   
  };
}
#endif
