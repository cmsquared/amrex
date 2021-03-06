#include "AMReX_LO_BCTYPES.H"

module abl_module

  use amrex_fort_module
  use amrex_error_module
  implicit none

contains

  subroutine fort_init_rhs (lo, hi, rhs, rlo, rhi, dx, a, b, sigma, w, ibnd) &
       bind(c,name="fort_init_rhs")
    integer, intent(in) :: lo(3), hi(3), rlo(3), rhi(3), ibnd
    real(amrex_real), intent(inout) :: rhs(rlo(1):rhi(1),rlo(2):rhi(2),rlo(3):rhi(3))
    real(amrex_real), intent(in) ::  dx(3), a, b, sigma, w

    integer :: i,j,k
    real(amrex_real) :: x,y,z,r, theta, beta, dbdrfac
    real(amrex_real) :: pi, fpi, tpi, fac

    pi = 4.d0 * atan(1.d0)
    tpi = 2.0d0 * pi
    fpi = 4.0d0 * pi
    fac = 3.0d0 * tpi**2
  
    theta = 0.5d0*log(3.0d0) / (w + 1.d-50)

    do k = lo(3), hi(3)
       z = (dble(k)+0.5d0)*dx(3)
       
       do j = lo(2), hi(2)
          y = (dble(j)+0.5d0)*dx(2)
          
          do i = lo(1), hi(1)
             x = (dble(i)+0.5d0)*dx(1)
             
             r = sqrt((x-0.5d0)**2 + (y-0.5d0)**2 + (z-0.5d0)**2)
             
             beta = (sigma-1.d0)/2.d0*tanh(theta*(r-0.25d0)) + (sigma+1.d0)/2.d0
             beta = beta * b
             dbdrfac = (sigma-1.d0)/2.d0/(cosh(theta*(r-0.25d0)))**2 * theta/r
             dbdrfac = dbdrfac * b
             
             if (ibnd .eq. 0 .or. ibnd.eq. LO_NEUMANN) then
                rhs(i,j,k) =         beta*fac*(cos(tpi*x) * cos(tpi*y) * cos(tpi*z)  &
                                             + cos(fpi*x) * cos(fpi*y) * cos(fpi*z)) &
                     + dbdrfac*((x-0.5d0)*(tpi*sin(tpi*x) * cos(tpi*y) * cos(tpi*z) &  
                                          + pi*sin(fpi*x) * cos(fpi*y) * cos(fpi*z)) &
                              + (y-0.5d0)*(tpi*cos(tpi*x) * sin(tpi*y) * cos(tpi*z) & 
                                          + pi*cos(fpi*x) * sin(fpi*y) * cos(fpi*z)) &
                              + (z-0.5d0)*(tpi*cos(tpi*x) * cos(tpi*y) * sin(tpi*z) & 
                                          + pi*cos(fpi*x) * cos(fpi*y) * sin(fpi*z))) &
                     +      a * (              cos(tpi*x) * cos(tpi*y) * cos(tpi*z) &  
                                    + 0.25d0 * cos(fpi*x) * cos(fpi*y) * cos(fpi*z))
             else if (ibnd .eq. LO_DIRICHLET) then
                rhs(i,j,k) =          beta*fac*(sin(tpi*x) * sin(tpi*y) * sin(tpi*z) & 
                                              + sin(fpi*x) * sin(fpi*y) * sin(fpi*z)) &
                     + dbdrfac*((x-0.5d0)*(-tpi*cos(tpi*x) * sin(tpi*y) * sin(tpi*z) & 
                                           - pi*cos(fpi*x) * sin(fpi*y) * sin(fpi*z)) &
                              + (y-0.5d0)*(-tpi*sin(tpi*x) * cos(tpi*y) * sin(tpi*z) & 
                                           - pi*sin(fpi*x) * cos(fpi*y) * sin(fpi*z)) &
                              + (z-0.5d0)*(-tpi*sin(tpi*x) * sin(tpi*y) * cos(tpi*z) & 
                                           - pi*sin(fpi*x) * sin(fpi*y) * cos(fpi*z))) &
                     +      a * (               sin(tpi*x) * sin(tpi*y) * sin(tpi*z) & 
                                     + 0.25d0 * sin(fpi*x) * sin(fpi*y) * sin(fpi*z))
             else
                call amrex_error('fort_init_rhs:: unknown boundary type')
             endif
          end do
       end do
    end do

  end subroutine fort_init_rhs


  subroutine fort_init_cc_coef (lo, hi, coef, clo, chi, dx, sigma, w) &
       bind(c, name='fort_init_cc_coef')
    integer, intent(in) :: lo(3), hi(3), clo(3), chi(3)
    real(amrex_real), intent(inout) :: coef(clo(1):chi(1),clo(2):chi(2),clo(3):chi(3))
    real(amrex_real), intent(in) :: dx(3), sigma, w
    
    integer :: i,j,k
    real(amrex_real) :: theta, x, y, z, r
    
    theta = 0.5d0*log(3.0d0) / (w + 1.d-50)

    do k = lo(3), hi(3)
       z = (dble(k)+0.5d0)*dx(3)
       do j = lo(2), hi(2)
          y = (dble(j)+0.5d0)*dx(2)
          do i = lo(1), hi(1)
             x = (dble(i)+0.5d0)*dx(1)
             
             r = sqrt((x-0.5d0)**2 + (y-0.5d0)**2 + (z-0.5d0)**2)
             
             coef(i,j,k) = (sigma-1.d0)/2.d0*tanh(theta*(r-0.25d0)) &
                  + (sigma+1.d0)/2.d0
          end do
       end do
    end do
  end subroutine fort_init_cc_coef


  subroutine fort_comp_asol (soln, slo, shi, dx, ibnd) bind(c,name='fort_comp_asol')
    integer, intent(in) :: slo(3), shi(3), ibnd
    real(amrex_real), intent(inout) :: soln(slo(1):shi(1),slo(2):shi(2),slo(3):shi(3))
    real(amrex_real), intent(in) :: dx(3)

    integer :: i,j,k
    real(amrex_real) :: x,y,z
    real(amrex_real) :: pi, fpi, tpi

    pi = 4.d0 * atan(1.d0)
    tpi = 2.0d0 * pi
    fpi = 4.0d0 * pi

    do k = slo(3), shi(3)
       z = (dble(k)+0.5d0)*dx(3)
       do j = slo(2), shi(2)
          y = (dble(j)+0.5d0)*dx(2)
          do i = slo(1), shi(1)
             x = (dble(i)+0.5d0)*dx(1)
             
             if (ibnd .eq. 0 .or. ibnd.eq. LO_NEUMANN) then
                soln(i,j,k) = 1.d0 * cos(tpi*x) * cos(tpi*y) * cos(tpi*z) & 
                           + .25d0 * cos(fpi*x) * cos(fpi*y) * cos(fpi*z)
             else if (ibnd .eq. LO_DIRICHLET) then
                soln(i,j,k) = 1.d0 * sin(tpi*x) * sin(tpi*y) * sin(tpi*z) & 
                           + .25d0 * sin(fpi*x) * sin(fpi*y) * sin(fpi*z)
             else
                call amrex_error('fort_comp_asol: unknown boundary type')
             endif
          end do
       end do
    end do
    
  end subroutine fort_comp_asol

end module abl_module
