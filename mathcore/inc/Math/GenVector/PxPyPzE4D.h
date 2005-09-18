// @(#)root/mathcore:$Name:  $:$Id: PxPyPzE4D.hv 1.0 2005/06/23 12:00:00 moneta Exp $
// Authors: W. Brown, M. Fischler, L. Moneta    2005  

/**********************************************************************
*                                                                    *
* Copyright (c) 2005 , LCG ROOT MathLib Team                         *
*                                                                    *
*                                                                    *
**********************************************************************/

// Header file for class PxPyPzE4D
// 
// Created by: fischler at Wed Jul 20   2005
//   (starting from PxPyPzE4D by moneta)
// 
// Last update: $Id: PxPyPzE4D.h,v 1.6 2005/07/21 16:29:48 fischler Exp $
// 
#ifndef ROOT_Math_GenVector_PxPyPzE4D 
#define ROOT_Math_GenVector_PxPyPzE4D  1

#include "Math/GenVector/etaMax.h"
#include "Math/GenVector/GenVector_exception.h"


#if defined(__MAKECINT__) || defined(G__DICTIONARY) 
#include "Math/GenVector/PtEtaPhiE4D.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#endif


#include <cmath>

namespace ROOT { 
  
  namespace Math { 
    
/** 
   Class describing a 4D cartesian coordinate system (x, y, z, t coordinates)
   or momentum-energy vectors stored as (Px, Py, Pz, E). 
   The metric used is (-,-,-,+)
*/ 

template <class ScalarType = double> 
class PxPyPzE4D { 

public : 

  typedef ScalarType Scalar;

  // --------- Constructors ---------------

  /**
  Default constructor  with x=y=z=t=0 
   */
  PxPyPzE4D() : fX(0), fY(0), fZ(0), fT(0) { }


  /**
    Constructor  from x, y , z , t values
   */
  PxPyPzE4D(Scalar x, Scalar y, Scalar z, Scalar t) : 
      				    fX(x), fY(y), fZ(z), fT(t) { }

  /**
    construct from any vector or  coordinate system class 
    implementing x(), y() and z() and t()
   */
  template <class CoordSystem> 
    explicit PxPyPzE4D(const CoordSystem & v) : 
    fX( v.x() ), fY( v.y() ), fZ( v.z() ), fT( v.t() )  { }

  // no reason for a custom destructor  ~Cartesian3D() {} and copy constructor

  /**
    Set internal data based on an array of 4 Scalar numbers
   */ 
  void SetCoordinates( const Scalar src[] ) 
  		{ fX=src[0]; fY=src[1]; fZ=src[2]; fT=src[3]; }

  /**
    get internal data into an array of 4 Scalar numbers
   */ 
  void GetCoordinates( Scalar dest[] ) const 
  		{ dest[0] = fX; dest[1] = fY; dest[2] = fZ; dest[3] = fT; }

  /**
    Set internal data based on 4 Scalar numbers
   */ 
  void SetCoordinates(Scalar  x, Scalar  y, Scalar  z, Scalar t) 
  		{ fX=x; fY=y; fZ=z; fT=t;}

  /**
    get internal data into 4 Scalar numbers
   */ 
  void GetCoordinates(Scalar& x, Scalar& y, Scalar& z, Scalar& t) const 
  		{ x=fX; y=fY; z=fZ; t=fT;}  				

  // --------- Coordinates and Coordinate-like Scalar properties -------------

  // cartesian (Minkowski)coordinate accessors 

  Scalar Px() const { return fX;}
  Scalar Py() const { return fY;}
  Scalar Pz() const { return fZ;}
  Scalar E()  const { return fT;}

  Scalar X() const { return fX;}
  Scalar Y() const { return fY;}
  Scalar Z() const { return fZ;}
  Scalar T() const { return fT;}

  // other coordinate representation

  /**
    squared magnitude of spatial components
   */
  Scalar P2() const { return fX*fX + fY*fY + fZ*fZ; } 

  /**
    magnitude of spatial components (magnitude of 3-momentum)
   */
  Scalar P() const { return std::sqrt(P2()); } 
  Scalar R() const { return P(); } 

  /**
    vector magnitude squared (or mass squared)
   */
  Scalar M2() const   { return fT*fT - fX*fX - fY*fY - fZ*fZ;}
  Scalar Mag2() const { return M2(); } 

  /**
    invariant mass 
   */
  Scalar M() const    { 
    Scalar mm = M2();
    if (mm >= 0) {
      return std::sqrt(mm);
    } else {
      GenVector_exception e ("PxPyPzE4D::M() - Tachyonic:\n"
      			     "    P^2 > E^2 so the mass would be imaginary");
      Throw(e);  
      return -std::sqrt(-mm);
    }
  }
  Scalar Mag() const    { return M(); }

  /** 
    transverse spatial component squared  
    */
  Scalar Pt2()   const { return fX*fX + fY*fY;}
  Scalar Perp2() const { return Pt2();}

  /**
    Transverse spatial component (P_perp or rho)
   */
  Scalar Pt()   const { return std::sqrt(Perp2());}
  Scalar Perp() const { return Pt();}
  Scalar Rho()  const { return Pt();}

  /** 
    transverse mass squared
    */
  Scalar Mt2() const { return fT*fT - fZ*fZ; } 

  /**
    transverse mass
   */
  Scalar Mt() const { 
    Scalar mm = Mt2();
    if (mm >= 0) {
      return std::sqrt(mm);
    } else {
      GenVector_exception e ("PxPyPzE4D::Mt() - Tachyonic:\n"
      		"    Pz^2 > E^2 so the transverse mass would be imaginary");
      Throw(e);  
      return -std::sqrt(-mm);
    }
  } 

  /** 
    transverse energy squared
    */
  Scalar Et2() const {  // is (E^2 * pt ^2) / p^2 
  			// but it is faster to form p^2 from pt^2
    Scalar pt2 = Pt2();
    return pt2 == 0 ? 0 : fT*fT * pt2/( pt2 + fZ*fZ );
  }

  /**
    transverse energy
   */
  Scalar Et() const { 
    Scalar etet = Et2();
    return fT < 0.0 ? -std::sqrt(etet) : std::sqrt(etet);
  }

  /**
    azimuthal angle 
   */
  Scalar Phi() const  { 
    return (fX == 0.0 && fY == 0.0) ? 0.0 : std::atan2(fY,fX);
  }

  /**
    polar angle
   */
  Scalar Theta() const {
    return (fX == 0.0 && fY == 0.0 && fZ == 0.0) ? 0.0 : std::atan2(Pt(),fZ);
  }

  /** 
    pseudorapidity
    */
  Scalar Eta() const { 
    Scalar rho = Pt();
    if (rho > 0) {
      Scalar z_scaled(fZ/rho);
      return std::log(z_scaled+std::sqrt(z_scaled*z_scaled+1)); // faster 
    } else if (fZ==0) {
      return 0;
    } else if (fZ>0) {
      return fZ + etaMax<Scalar>( );
    }  else {
      return fZ - etaMax<Scalar>( );
    }
  }

  // --------- Set Coordinates of this system  ---------------


  /**
    set X value 
   */
  void SetPx( Scalar  x) { 
    fX = x; 
  }
  /**
    set Y value 
   */
  void SetPy( Scalar  y) { 
    fY = y; 
  }
  /**
    set Z value 
   */
  void SetPz( Scalar  z) { 
    fZ = z; 
  }
  /**
    set T value 
   */
  void SetE( Scalar  t) { 
    fT = t; 
  }


  // ------ Manipulations -------------
  
  /**
     negate the 4-vector
   */
  void Negate( ) { fX = -fX; fY = -fY;  fZ = -fZ; fT = -fT;}

  /**
    scale coordinate values by a scalar quantity a
   */
  void Scale( const Scalar & a) { 
    fX *= a; 
    fY *= a; 
    fZ *= a; 
    fT *= a; 
  }
 
  /**
    Assignment from a generic coordinate system implementing 
    x(), y(), z() and t()
   */
  template <class AnyCoordSystem> 
    PxPyPzE4D & operator = (const AnyCoordSystem & v) { 
      fX = v.x();  
      fY = v.y();  
      fZ = v.z();  
      fT = v.t();
      return *this;
    }

  /**
    Exact equality
   */  
  bool operator == (const PxPyPzE4D & rhs) const {
    return fX == rhs.fX && fY == rhs.fY && fZ == rhs.fZ && fT == rhs.fT;
  }
  bool operator != (const PxPyPzE4D & rhs) const {return !(operator==(rhs));}


  // ============= Compatibility section ==================

  // The following make this coordinate system look enough like a CLHEP
  // vector that an assignment member template can work with either
  Scalar x() const { return X(); }
  Scalar y() const { return Y(); }
  Scalar z() const { return Z(); } 
  Scalar t() const { return E(); } 



#if defined(__MAKECINT__) || defined(G__DICTIONARY) 

  // ====== Set member functions for coordinates in other systems =======

  void SetPt(Scalar pt) {  
    GenVector_exception e("PxPyPzE4D::SetPt() is not supposed to be called");
    Throw(e);
    PtEtaPhiE4D<Scalar> v(*this); v.SetPt(pt); *this = PxPyPzE4D<Scalar>(v);
  }
  void SetEta(Scalar eta) {  
    GenVector_exception e("PxPyPzE4D::SetEta() is not supposed to be called");
    Throw(e);
    PtEtaPhiE4D<Scalar> v(*this); v.SetEta(eta); *this = PxPyPzE4D<Scalar>(v);
  }
  void SetPhi(Scalar phi) {  
    GenVector_exception e("PxPyPzE4D::SetPhi() is not supposed to be called");
    Throw(e);
    PtEtaPhiE4D<Scalar> v(*this); v.SetPhi(phi); *this = PxPyPzE4D<Scalar>(v);
  }
  void SetM(Scalar m) {  
    GenVector_exception e("PxPyPzE4D::SetM() is not supposed to be called");
    Throw(e);
    PtEtaPhiM4D<Scalar> v(*this); v.SetM(m); 
    *this = PxPyPzE4D<Scalar>(v);
  }

#endif

private:

  /**
    (contigous) data containing the coordinate values x,y,z,t
  */

  Scalar fX;
  Scalar fY;
  Scalar fZ;
  Scalar fT;

}; 
    
} // end namespace Math  
} // end namespace ROOT

#endif // ROOT_Math_GenVector_PxPyPzE4D 
