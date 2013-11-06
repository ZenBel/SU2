/*!
 * \file numerics_structure.cpp
 * \brief This file contains all the numerical methods.
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 2.0.8
 *
 * Stanford University Unstructured (SU2).
 * Copyright (C) 2012-2013 Aerospace Design Laboratory (ADL).
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/numerics_structure.hpp"

CNumerics::CNumerics(void) { }

CNumerics::CNumerics(unsigned short val_nDim, unsigned short val_nVar, CConfig *config) {
	nDim = val_nDim;
	nVar = val_nVar;
	Gamma = config->GetGamma();
	Gamma_Minus_One = Gamma - 1.0;
	Gas_Constant = config->GetGas_ConstantND();

	//U_id = new double [nVar];
	//U_jd = new double [nVar];

	UnitNormal = new double [nDim];
	UnitNormald = new double [nDim];

	Normal = new double [nDim];
	Flux_Tensor = new double* [nVar];
	for (unsigned short iVar = 0; iVar < (nVar); iVar++)
		Flux_Tensor[iVar] = new double [nDim];

	tau = new double* [nDim];
	delta = new double* [nDim];
	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		tau[iDim] = new double [nDim];
		delta[iDim] = new double [nDim];
	}

	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		for (unsigned short jDim = 0; jDim < nDim; jDim++) {
			if (iDim==jDim) delta[iDim][jDim]=1.0;
			else delta[iDim][jDim]=0.0;
		}
	}

	U_n = new double [nVar];
	U_nM1 = new double [nVar];
	U_nP1 = new double [nVar];

	Proj_Flux_Tensor = new double [nVar];

	turb_ke_i = 0.0;
	turb_ke_j = 0.0;
  
  Diffusion_Coeff_i = NULL;
  Diffusion_Coeff_j = NULL;
  
  if ((config->GetKind_Solver() == TNE2_EULER)        ||
      (config->GetKind_Solver() == TNE2_NAVIER_STOKES)  ) {
    nSpecies = nVar - nDim - 2;
    Diffusion_Coeff_i = new double[nSpecies];
    Diffusion_Coeff_j = new double[nSpecies];
  }
  
  Vector = new double[nDim];
}
/* Class overloaded to include multiple fluid equations for plasma */
CNumerics::CNumerics(unsigned short val_nDim, unsigned short val_nVar, unsigned short val_nSpecies, CConfig *config) {

	nDim		= val_nDim;
	nVar		= val_nVar;
	nSpecies	= val_nSpecies;
	Gamma = config->GetGamma();
	Gamma_Minus_One = Gamma - 1.0;
	Gas_Constant_MultipleSpecies = new double [nSpecies];
	Vector_Gamma = new double [nSpecies];

	for (unsigned short iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		Gas_Constant_MultipleSpecies[iSpecies] =config->GetSpecies_Gas_Constant(iSpecies);
		Vector_Gamma[iSpecies] =   config->GetSpecies_Gamma(iSpecies);
	}

	UnitNormal = new double [nDim];
	Normal  = new double [nDim];
	Flux_Tensor = new double* [nVar];
	for (unsigned short iVar = 0; iVar < (nVar); iVar++)
		Flux_Tensor[iVar] = new double [nDim];

	tau = new double* [nDim];
	delta = new double* [nDim];
	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		tau[iDim] = new double [nDim];
		delta[iDim] = new double [nDim];
	}

	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		for (unsigned short jDim = 0; jDim < nDim; jDim++) {
			if (iDim==jDim) delta[iDim][jDim]=1.0;
			else delta[iDim][jDim]=0.0;
		}
	}

	U_n = new double [nVar];
	U_nM1 = new double [nVar];
	U_nP1 = new double [nVar];

	Proj_Flux_Tensor = new double [nVar];

	Laminar_Viscosity_MultipleSpecies_i = new double [nSpecies];
	Laminar_Viscosity_MultipleSpecies_j = new double [nSpecies];
	Eddy_Viscosity_MultipleSpecies_i = new double [nSpecies];
	Eddy_Viscosity_MultipleSpecies_j = new double [nSpecies];

}

/* Class overloaded to include multiple fluid equations for plasma */
CNumerics::CNumerics(unsigned short val_nDim, unsigned short val_nVar, unsigned short val_nSpecies, unsigned short val_nDiatomics, unsigned short val_nMonatomics, CConfig *config) {

	/*--- Set parameters ---*/
	nDim		        = val_nDim;
	nVar		        = val_nVar;
	nSpecies	      = val_nSpecies;
	nMonatomics     = val_nMonatomics;
	nDiatomics      = val_nDiatomics;
	Gamma           = config->GetGamma();
	Gamma_Minus_One = Gamma - 1.0;

	/*--- Allocate gas property arrays ---*/
	Gas_Constant_MultipleSpecies = new double [nSpecies];
	Vector_Gamma                 = new double [nSpecies];
	Enthalpy_formation           = new double [nSpecies];
	Theta_v                      = new double [nSpecies];

	for (unsigned short iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		Gas_Constant_MultipleSpecies[iSpecies] = config->GetSpecies_Gas_Constant(iSpecies);
		Vector_Gamma[iSpecies]                 = config->GetSpecies_Gamma(iSpecies);
		Enthalpy_formation[iSpecies]           = config->GetEnthalpy_Formation(iSpecies);
		Theta_v[iSpecies]                      = config->GetCharVibTemp(iSpecies);
	}

	UnitNormal = new double [nDim];
	Normal  = new double [nDim];
	Flux_Tensor = new double* [nDim+3];
	for (unsigned short iVar = 0; iVar < nDim+3; iVar++)
		Flux_Tensor[iVar] = new double [nDim];

	tau   = new double* [nDim];
	delta = new double* [nDim];
	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		tau[iDim]   = new double [nDim];
		delta[iDim] = new double [nDim];
	}

	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		for (unsigned short jDim = 0; jDim < nDim; jDim++) {
			if (iDim==jDim) delta[iDim][jDim]=1.0;
			else delta[iDim][jDim]=0.0;
		}
	}

	U_n   = new double[nVar];
	U_nM1 = new double[nVar];
	U_nP1 = new double[nVar];

	Proj_Flux_Tensor = new double [nDim+3];

	Laminar_Viscosity_MultipleSpecies_i        = new double[nSpecies];
	Laminar_Viscosity_MultipleSpecies_j        = new double[nSpecies];
	Eddy_Viscosity_MultipleSpecies_i           = new double[nSpecies];
	Eddy_Viscosity_MultipleSpecies_j           = new double[nSpecies];
	Thermal_Conductivity_MultipleSpecies_i     = new double[nSpecies];
	Thermal_Conductivity_MultipleSpecies_j     = new double[nSpecies];
	Thermal_Conductivity_vib_MultipleSpecies_i = new double[nSpecies];
	Thermal_Conductivity_vib_MultipleSpecies_j = new double[nSpecies];

	dVdU    = new double *[nDim+3];
	dFvdV_j = new double *[nDim+3];
	dFvdV_i = new double *[nDim+3];
	unsigned short iVar;
	for (iVar = 0; iVar < nVar; iVar++) {
		dVdU[iVar] = new double[nDim+3];
		dFvdV_j[iVar] = new double [nDim+3];
		dFvdV_i[iVar] = new double [nDim+3];
	}

}

CNumerics::~CNumerics(void) {

  delete [] Normal;
	delete [] UnitNormal;

	delete [] U_n;
	delete [] U_nM1;
	delete [] U_nP1;

	// visc
	delete [] Proj_Flux_Tensor;

	for (unsigned short iVar = 0; iVar < nDim+3; iVar++) {
		delete [] Flux_Tensor[iVar];
	}
	delete [] Flux_Tensor;

	for (unsigned short iDim = 0; iDim < nDim; iDim++) {
		delete [] tau[iDim];
		delete [] delta[iDim];
	}
	delete [] tau;
	delete [] delta;
	delete [] Normal;
	delete [] Gas_Constant_MultipleSpecies;
	delete [] Vector_Gamma;
	delete [] Enthalpy_formation;
	delete [] Theta_v;
  if (Diffusion_Coeff_i != NULL) delete [] Diffusion_Coeff_i;
  if (Diffusion_Coeff_j != NULL) delete [] Diffusion_Coeff_j;
  if (Vector != NULL) delete [] Vector;

	if (Laminar_Viscosity_MultipleSpecies_i        != NULL) delete [] Laminar_Viscosity_MultipleSpecies_i;
	if (Laminar_Viscosity_MultipleSpecies_j        != NULL) delete [] Laminar_Viscosity_MultipleSpecies_j;
	if (Eddy_Viscosity_MultipleSpecies_i           != NULL) delete [] Eddy_Viscosity_MultipleSpecies_i;
	if (Eddy_Viscosity_MultipleSpecies_j           != NULL) delete [] Eddy_Viscosity_MultipleSpecies_j;
	if (Thermal_Conductivity_MultipleSpecies_i     != NULL) delete [] Thermal_Conductivity_MultipleSpecies_i;
	if (Thermal_Conductivity_MultipleSpecies_j     != NULL) delete [] Thermal_Conductivity_MultipleSpecies_i;
	if (Thermal_Conductivity_vib_MultipleSpecies_i != NULL) delete [] Thermal_Conductivity_vib_MultipleSpecies_i;
	if (Thermal_Conductivity_vib_MultipleSpecies_j != NULL) delete [] Thermal_Conductivity_vib_MultipleSpecies_i;


	unsigned short iVar;
	for (iVar = 0; iVar < nVar; iVar++) {
		delete [] dVdU[iVar];
		delete [] dFvdV_i[iVar];
		delete [] dFvdV_j[iVar];
	}
	delete [] dVdU;
	delete [] dFvdV_i;
	delete [] dFvdV_j;

}

void CNumerics::GetInviscidFlux(double val_density, double *val_velocity,
		double val_pressure, double val_enthalpy) {
	if (nDim == 3) {
		Flux_Tensor[0][0] = val_density*val_velocity[0];
		Flux_Tensor[1][0] = Flux_Tensor[0][0]*val_velocity[0]+val_pressure;
		Flux_Tensor[2][0] = Flux_Tensor[0][0]*val_velocity[1];
		Flux_Tensor[3][0] = Flux_Tensor[0][0]*val_velocity[2];
		Flux_Tensor[4][0] = Flux_Tensor[0][0]*val_enthalpy;

		Flux_Tensor[0][1] = val_density*val_velocity[1];
		Flux_Tensor[1][1] = Flux_Tensor[0][1]*val_velocity[0];
		Flux_Tensor[2][1] = Flux_Tensor[0][1]*val_velocity[1]+val_pressure;
		Flux_Tensor[3][1] = Flux_Tensor[0][1]*val_velocity[2];
		Flux_Tensor[4][1] = Flux_Tensor[0][1]*val_enthalpy;

		Flux_Tensor[0][2] = val_density*val_velocity[2];
		Flux_Tensor[1][2] = Flux_Tensor[0][2]*val_velocity[0];
		Flux_Tensor[2][2] = Flux_Tensor[0][2]*val_velocity[1];
		Flux_Tensor[3][2] = Flux_Tensor[0][2]*val_velocity[2]+val_pressure;
		Flux_Tensor[4][2] = Flux_Tensor[0][2]*val_enthalpy;

	}
	if(nDim == 2) {
		Flux_Tensor[0][0] = val_density*val_velocity[0];
		Flux_Tensor[1][0] = Flux_Tensor[0][0]*val_velocity[0]+val_pressure;
		Flux_Tensor[2][0] = Flux_Tensor[0][0]*val_velocity[1];
		Flux_Tensor[3][0] = Flux_Tensor[0][0]*val_enthalpy;

		Flux_Tensor[0][1] = val_density*val_velocity[1];
		Flux_Tensor[1][1] = Flux_Tensor[0][1]*val_velocity[0];
		Flux_Tensor[2][1] = Flux_Tensor[0][1]*val_velocity[1]+val_pressure;
		Flux_Tensor[3][1] = Flux_Tensor[0][1]*val_enthalpy;
	}
}

void CNumerics::GetInviscidProjFlux(double *val_density,
                                    double *val_velocity,
                                    double *val_pressure,
                                    double *val_enthalpy,
                                    double *val_normal,
                                    double *val_Proj_Flux) {
    double rhou, rhov, rhow;
    

	if (nDim == 2) {
		rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];

		val_Proj_Flux[0] = rhou*val_normal[0];
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0];
		val_Proj_Flux[2] = rhou*val_velocity[1]*val_normal[0];
		val_Proj_Flux[3] = rhou*(*val_enthalpy)*val_normal[0];

		val_Proj_Flux[0] += rhov*val_normal[1];
		val_Proj_Flux[1] += rhov*val_velocity[0]*val_normal[1];
		val_Proj_Flux[2] += (rhov*val_velocity[1]+(*val_pressure))*val_normal[1];
		val_Proj_Flux[3] += rhov*(*val_enthalpy)*val_normal[1];
	} 
	else {
		rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
		rhow = (*val_density)*val_velocity[2];

		val_Proj_Flux[0] = rhou*val_normal[0];
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0];
		val_Proj_Flux[2] = rhou*val_velocity[1]*val_normal[0];
		val_Proj_Flux[3] = rhou*val_velocity[2]*val_normal[0];
		val_Proj_Flux[4] = rhou*(*val_enthalpy)*val_normal[0];

		val_Proj_Flux[0] += rhov*val_normal[1];
		val_Proj_Flux[1] += rhov*val_velocity[0]*val_normal[1];
		val_Proj_Flux[2] += (rhov*val_velocity[1]+(*val_pressure))*val_normal[1];
		val_Proj_Flux[3] += rhov*val_velocity[2]*val_normal[1];
		val_Proj_Flux[4] += rhov*(*val_enthalpy)*val_normal[1];

		val_Proj_Flux[0] += rhow*val_normal[2];
		val_Proj_Flux[1] += rhow*val_velocity[0]*val_normal[2];
		val_Proj_Flux[2] += rhow*val_velocity[1]*val_normal[2];
		val_Proj_Flux[3] += (rhow*val_velocity[2]+(*val_pressure))*val_normal[2];
		val_Proj_Flux[4] += rhow*(*val_enthalpy)*val_normal[2];
	}

}

void CNumerics::GetInviscidProjFlux(double *val_density,
                                    double *val_velocity,
                                    double *val_pressure,
                                    double *val_enthalpy,
                                    double *val_energy_ve,
                                    double *val_normal,
                                    double *val_Proj_Flux) {
  unsigned short iSpecies;
  double rho, rhou, rhov, rhow;
  
  //************************************************//
  // Please do not delete //SU2_CPP2C comment lines //
  //************************************************//
  
  //SU2_CPP2C SUB START GetInviscidProjFlux
  //SU2_CPP2C SUB VARS *val_density val_velocity *val_pressure *val_enthalpy val_Proj_Flux val_normal
  
  rho = 0.0;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    rho += val_density[iSpecies];
  
	if (nDim == 2) {
		rhou = rho*val_velocity[0];
		rhov = rho*val_velocity[1];
    
    /*--- iDim = 0 (x-direction) ---*/
    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
      val_Proj_Flux[iSpecies] = rhou * (val_density[iSpecies]/rho) * val_normal[0];
		val_Proj_Flux[nSpecies]   = (rhou * val_velocity[0] + (*val_pressure)) * val_normal[0];
		val_Proj_Flux[nSpecies+1] = rhou * val_velocity[1] * val_normal[0];
		val_Proj_Flux[nSpecies+2] = rhou * (*val_enthalpy) * val_normal[0];
    val_Proj_Flux[nSpecies+3] = rhou * (*val_energy_ve) * val_normal[0];
    
    /*---- iDim = 1 (y-direction) ---*/
    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
      val_Proj_Flux[iSpecies] += rhov * (val_density[iSpecies]/rho) * val_normal[1];
		val_Proj_Flux[nSpecies]   += rhov * val_velocity[0] * val_normal[1];
		val_Proj_Flux[nSpecies+1] += (rhov * val_velocity[1] + (*val_pressure)) * val_normal[1];
		val_Proj_Flux[nSpecies+2] += rhov * (*val_enthalpy) * val_normal[1];
    val_Proj_Flux[nSpecies+3] += rhov * (*val_energy_ve) * val_normal[1];
	}
	else {
		rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
		rhow = (*val_density)*val_velocity[2];
    
    /*--- iDim = 0 (x-direction) ---*/
    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
      val_Proj_Flux[iSpecies] = rhou * (val_density[iSpecies]/rho) * val_normal[0];
		val_Proj_Flux[nSpecies]   = (rhou * val_velocity[0] + (*val_pressure)) * val_normal[0];
		val_Proj_Flux[nSpecies+1] = rhou * val_velocity[1] * val_normal[0];
		val_Proj_Flux[nSpecies+2] = rhou * val_velocity[2] * val_normal[0];
		val_Proj_Flux[nSpecies+3] = rhou * (*val_enthalpy) * val_normal[0];
    val_Proj_Flux[nSpecies+4] = rhou * (*val_energy_ve) * val_normal[0];
    
    /*--- iDim = 0 (y-direction) ---*/
    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
      val_Proj_Flux[iSpecies] += rhov * (val_density[iSpecies]/rho) * val_normal[1];
		val_Proj_Flux[nSpecies]   += rhov * val_velocity[0] * val_normal[1];
		val_Proj_Flux[nSpecies+1] += (rhov * val_velocity[1] + (*val_pressure)) * val_normal[1];
		val_Proj_Flux[nSpecies+2] += rhov * val_velocity[2] * val_normal[1];
		val_Proj_Flux[nSpecies+3] += rhov * (*val_enthalpy) * val_normal[1];
    val_Proj_Flux[nSpecies+4] += rhov * (*val_energy_ve) * val_normal[1];
    
    /*--- iDim = 0 (z-direction) ---*/
    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
      val_Proj_Flux[iSpecies] += rhow * (val_density[iSpecies]/rho) * val_normal[2];
		val_Proj_Flux[nSpecies]   += rhow * val_velocity[0] * val_normal[2];
		val_Proj_Flux[nSpecies+1] += rhow * val_velocity[1] * val_normal[2];
		val_Proj_Flux[nSpecies+2] += (rhow * val_velocity[2] + (*val_pressure)) * val_normal[2];
		val_Proj_Flux[nSpecies+3] += rhow * (*val_enthalpy) * val_normal[2];
    val_Proj_Flux[nSpecies+4] += rhow * (*val_energy_ve) * val_normal[2];
	}
  
  //SU2_CPP2C SUB END GetInviscidProjFlux
}

void CNumerics::GetInviscidArtCompProjFlux(double *val_density,
                                           double *val_velocity,
                                           double *val_pressure,
                                           double *val_betainc2,
                                           double *val_normal,
                                           double *val_Proj_Flux) {
    double rhou, rhov, rhow;
    
   	if (nDim == 2) {
      rhou = (*val_density)*val_velocity[0];
      rhov = (*val_density)*val_velocity[1];
      
      val_Proj_Flux[0] = (*val_betainc2)*(val_velocity[0]*val_normal[0] + val_velocity[1]*val_normal[1]);
      val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0] + rhou*val_velocity[1]*val_normal[1];
      val_Proj_Flux[2] = rhov*val_velocity[0]*val_normal[0] + (rhov*val_velocity[1]+(*val_pressure))*val_normal[1];
	}
  else {
    rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
		rhow = (*val_density)*val_velocity[2];
    
		val_Proj_Flux[0] = (*val_betainc2)*(val_velocity[0]*val_normal[0] + val_velocity[1]*val_normal[1] + val_velocity[2]*val_normal[2]);
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0] + rhou*val_velocity[1]*val_normal[1] + rhou*val_velocity[2]*val_normal[2];
		val_Proj_Flux[2] = rhov*val_velocity[0]*val_normal[0] + (rhov*val_velocity[1]+(*val_pressure))*val_normal[1] + rhov*val_velocity[2]*val_normal[2];
		val_Proj_Flux[3] = rhow*val_velocity[0]*val_normal[0] + rhow*val_velocity[1]*val_normal[1] + (rhow*val_velocity[2]+(*val_pressure))*val_normal[2];
	}
  
}

void CNumerics::GetInviscidArtComp_FreeSurf_ProjFlux(double *val_density, double *val_velocity, double *val_pressure, double *val_betainc2,
                                                     double *val_levelset, double *val_normal, double *val_Proj_Flux) {
  
  double rhou, rhov, rhow, ProjVel;
  
  if (nDim == 2) {
    rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
    ProjVel = (val_velocity[0]*val_normal[0] + val_velocity[1]*val_normal[1]);
    
		val_Proj_Flux[0] = (*val_betainc2)*ProjVel;
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0] + rhou*val_velocity[1]*val_normal[1];
		val_Proj_Flux[2] = rhov*val_velocity[0]*val_normal[0] + (rhov*val_velocity[1]+(*val_pressure))*val_normal[1];
    val_Proj_Flux[3] = (*val_levelset)*ProjVel;
	}
  else {
    rhou = (*val_density)*val_velocity[0];
		rhov = (*val_density)*val_velocity[1];
		rhow = (*val_density)*val_velocity[2];
    ProjVel = (val_velocity[0]*val_normal[0] + val_velocity[1]*val_normal[1] + val_velocity[2]*val_normal[2]);

		val_Proj_Flux[0] = (*val_betainc2)*ProjVel;
		val_Proj_Flux[1] = (rhou*val_velocity[0]+(*val_pressure))*val_normal[0] + rhou*val_velocity[1]*val_normal[1] + rhou*val_velocity[2]*val_normal[2];
		val_Proj_Flux[2] = rhov*val_velocity[0]*val_normal[0] + (rhov*val_velocity[1]+(*val_pressure))*val_normal[1] + rhov*val_velocity[2]*val_normal[2];
		val_Proj_Flux[3] = rhow*val_velocity[0]*val_normal[0] + rhow*val_velocity[1]*val_normal[1] + (rhow*val_velocity[2]+(*val_pressure))*val_normal[2];
    val_Proj_Flux[4] = (*val_levelset)*ProjVel;
	}

}


void CNumerics::GetInviscidProjFlux(double *val_density, double **val_velocity,
		double *val_pressure, double *val_enthalpy,
		double *val_normal, double *val_Proj_Flux) {
	unsigned short iSpecies, loc = 0;

	for ( iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		loc = iSpecies*(nDim+2);
		Gamma = Vector_Gamma[iSpecies];
		Gamma_Minus_One = Gamma - 1.0;

		if(nDim == 3) {
			double rhou		= val_density[iSpecies]*val_velocity[iSpecies][0];
			double rhov		= val_density[iSpecies]*val_velocity[iSpecies][1];
			double rhow		= val_density[iSpecies]*val_velocity[iSpecies][2];
			double pressure = val_pressure[iSpecies];
			double u		= val_velocity[iSpecies][0];
			double v		= val_velocity[iSpecies][1];
			double w		= val_velocity[iSpecies][2];
			double enthalpy = val_enthalpy[iSpecies];
			val_Proj_Flux[loc + 0] = rhou*val_normal[0] + rhov*val_normal[1] + rhow*val_normal[2];
			val_Proj_Flux[loc + 1] = (rhou*u + pressure)*val_normal[0] + rhov*u*val_normal[1]+ rhow*u*val_normal[2];
			val_Proj_Flux[loc + 2] = rhou*v*val_normal[0] + (rhov*v+pressure)*val_normal[1] + rhow*v*val_normal[2];
			val_Proj_Flux[loc + 3] = rhou*w*val_normal[0] + rhov*w*val_normal[1] + (rhow*w + pressure)*val_normal[2];
			val_Proj_Flux[loc + 4] = rhou*enthalpy*val_normal[0] + rhov*enthalpy*val_normal[1] + rhow*enthalpy*val_normal[2] ;
		}

		if(nDim == 2) {
			double rhou		= val_density[iSpecies]*val_velocity[iSpecies][0];
			double rhov		= val_density[iSpecies]*val_velocity[iSpecies][1];
			double pressure = val_pressure[iSpecies];
			double u		= val_velocity[iSpecies][0];
			double v		= val_velocity[iSpecies][1];
			double enthalpy = val_enthalpy[iSpecies];
			val_Proj_Flux[loc + 0] = rhou*val_normal[0] + rhov*val_normal[1];
			val_Proj_Flux[loc + 1] = (rhou*u + pressure)*val_normal[0] + rhov*u*val_normal[1];
			val_Proj_Flux[loc + 2] = rhou*v*val_normal[0] + (rhov*v+pressure)*val_normal[1];
			val_Proj_Flux[loc + 3] = rhou*enthalpy*val_normal[0] + rhov*enthalpy*val_normal[1];
		}
	}
}

void CNumerics::GetInviscidProjFlux_(double *val_density, double **val_velocity, double *val_pressure, double *val_enthalpy,
		double *val_energy_vib, double *val_normal, double *val_Proj_Flux) {

	unsigned short iSpecies, iVar, iDim, jDim, loc = 0;

	for (iVar = 0; iVar < nVar; iVar++)
		val_Proj_Flux[iVar] = 0.0;

	for ( iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
		else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

		for (iDim = 0; iDim < nDim; iDim++) {
			val_Proj_Flux[loc+0] += val_density[iSpecies]*val_velocity[iSpecies][iDim]*val_normal[iDim];
			for (jDim = 0; jDim < nDim; jDim++) {
				val_Proj_Flux[loc+iDim+1] += val_density[iSpecies]*val_velocity[iSpecies][iDim]*val_velocity[iSpecies][jDim]*val_normal[jDim];
			}
			val_Proj_Flux[loc+iDim+1] += val_pressure[iSpecies]*val_normal[iDim];
			val_Proj_Flux[loc+nDim+1] += val_density[iSpecies]*val_enthalpy[iSpecies]*val_velocity[iSpecies][iDim]*val_normal[iDim];
			if (iSpecies < nDiatomics) 
				val_Proj_Flux[loc+nDim+2] += val_density[iSpecies]*val_velocity[iSpecies][iDim]*val_energy_vib[iSpecies]*val_normal[iDim];
		}
	}
}

void CNumerics::GetInviscidProjJac(double *val_velocity, double *val_energy, double *val_normal,
		double val_scale, double **val_Proj_Jac_Tensor) {
	unsigned short iDim, jDim;
    double sqvel, proj_vel, phi, a1, a2;
    
	sqvel = 0.0, proj_vel = 0.0;
	for (iDim = 0; iDim < nDim; iDim++) {
		sqvel    += val_velocity[iDim]*val_velocity[iDim];
		proj_vel += val_velocity[iDim]*val_normal[iDim];
	}
    
	phi = 0.5*Gamma_Minus_One*sqvel;
	a1 = Gamma*(*val_energy)-phi;
	a2 = Gamma-1.0;

	val_Proj_Jac_Tensor[0][0] = 0.0;
	for (iDim = 0; iDim < nDim; iDim++)
		val_Proj_Jac_Tensor[0][iDim+1] = val_scale*val_normal[iDim];
	val_Proj_Jac_Tensor[0][nDim+1] = 0.0;

	for (iDim = 0; iDim < nDim; iDim++) {
		val_Proj_Jac_Tensor[iDim+1][0] = val_scale*(val_normal[iDim]*phi - val_velocity[iDim]*proj_vel);
		for (jDim = 0; jDim < nDim; jDim++)
			val_Proj_Jac_Tensor[iDim+1][jDim+1] = val_scale*(val_normal[jDim]*val_velocity[iDim]-a2*val_normal[iDim]*val_velocity[jDim]);
		val_Proj_Jac_Tensor[iDim+1][iDim+1] += val_scale*proj_vel;
		val_Proj_Jac_Tensor[iDim+1][nDim+1] = val_scale*a2*val_normal[iDim];
	}

	val_Proj_Jac_Tensor[nDim+1][0] = val_scale*proj_vel*(phi-a1);
	for (iDim = 0; iDim < nDim; iDim++)
		val_Proj_Jac_Tensor[nDim+1][iDim+1] = val_scale*(val_normal[iDim]*a1-a2*val_velocity[iDim]*proj_vel);
	val_Proj_Jac_Tensor[nDim+1][nDim+1] = val_scale*Gamma*proj_vel;
}

void CNumerics::GetInviscidProjJac(double *val_density, double *val_velocity, double *val_enthalpy,
                                   double *val_energy_ve, double *val_dPdrhos, double val_dPdrhoE,
                                   double val_dPdrhoEve, double *val_normal, double val_scale,
                                   double **val_Proj_Jac_Tensor) {
	unsigned short iDim, iVar, jVar, iSpecies, jSpecies;
  double proj_vel, rho;
  
  
  for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
      val_Proj_Jac_Tensor[iVar][jVar] = 0.0;
  
  rho = 0.0;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    rho += val_density[iSpecies];
	proj_vel = 0.0;
	for (iDim = 0; iDim < nDim; iDim++) {
		proj_vel += val_velocity[iDim]*val_normal[iDim];
	}
  
/*  cout << "CNumerics -- InvProjJac: " << endl;
  cout << "nSpecies: " << nSpecies << endl;
  cout << "ProjVel: " << proj_vel << endl;
  cout << "rho: " << rho << endl;
  cout << "Val Velocity: " << val_velocity[0] << endl;
  cout << "Enthalpy: " << *val_enthalpy << endl;
  cout << "dPdrhos: " << val_dPdrhos[0] << endl;
  cout << "dPdrhoE: " << val_dPdrhoE << endl;
  cout << "dPdrhoEve: " << val_dPdrhoEve << endl;
  cin.get();*/
  
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    for (jSpecies = 0; jSpecies < nSpecies; jSpecies++) {
      val_Proj_Jac_Tensor[iSpecies][jSpecies] = -(val_density[iSpecies]/rho) * proj_vel;
    }
    val_Proj_Jac_Tensor[iSpecies][iSpecies]  += proj_vel;
    val_Proj_Jac_Tensor[iSpecies][nSpecies]   = (val_density[iSpecies]/rho) * val_normal[0];
    val_Proj_Jac_Tensor[iSpecies][nSpecies+1] = (val_density[iSpecies]/rho) * val_normal[1];
    val_Proj_Jac_Tensor[iSpecies][nSpecies+2] = (val_density[iSpecies]/rho) * val_normal[2];
    
    val_Proj_Jac_Tensor[nSpecies][iSpecies]   = val_dPdrhos[iSpecies]*val_normal[0] - proj_vel*val_velocity[0];
    val_Proj_Jac_Tensor[nSpecies+1][iSpecies] = val_dPdrhos[iSpecies]*val_normal[1] - proj_vel*val_velocity[1];
    val_Proj_Jac_Tensor[nSpecies+2][iSpecies] = val_dPdrhos[iSpecies]*val_normal[2] - proj_vel*val_velocity[2];
    val_Proj_Jac_Tensor[nSpecies+3][iSpecies] = (val_dPdrhos[iSpecies]-(*val_enthalpy)) * proj_vel;
    val_Proj_Jac_Tensor[nSpecies+4][iSpecies] = -proj_vel * (*val_energy_ve);
  }
  
  val_Proj_Jac_Tensor[nSpecies][nSpecies]     = -val_dPdrhoE*val_velocity[0]*val_normal[0] + val_velocity[0]*val_normal[0] + proj_vel;
  val_Proj_Jac_Tensor[nSpecies][nSpecies+1]   = -val_dPdrhoE*val_velocity[1]*val_normal[0] + val_velocity[0]*val_normal[1];
  val_Proj_Jac_Tensor[nSpecies][nSpecies+2]   = -val_dPdrhoE*val_velocity[2]*val_normal[0] + val_velocity[0]*val_normal[2];
  val_Proj_Jac_Tensor[nSpecies][nSpecies+3]   = val_dPdrhoE * val_normal[0];
  val_Proj_Jac_Tensor[nSpecies][nSpecies+4]   = val_dPdrhoEve * val_normal[0];
  
  val_Proj_Jac_Tensor[nSpecies+1][nSpecies]   = -val_dPdrhoE*val_velocity[0]*val_normal[1] + val_velocity[1]*val_normal[0];
  val_Proj_Jac_Tensor[nSpecies+1][nSpecies+1] = -val_dPdrhoE*val_velocity[1]*val_normal[1] + val_velocity[1]*val_normal[1] + proj_vel;
  val_Proj_Jac_Tensor[nSpecies+1][nSpecies+2] = -val_dPdrhoE*val_velocity[2]*val_normal[1] + val_velocity[1]*val_normal[2];
  val_Proj_Jac_Tensor[nSpecies+1][nSpecies+3] = val_dPdrhoE * val_normal[1];
  val_Proj_Jac_Tensor[nSpecies+1][nSpecies+4] = val_dPdrhoEve * val_normal[1];
  
  val_Proj_Jac_Tensor[nSpecies+2][nSpecies]   = -val_dPdrhoE*val_velocity[0]*val_normal[2] + val_velocity[2]*val_normal[0];
  val_Proj_Jac_Tensor[nSpecies+2][nSpecies+1] = -val_dPdrhoE*val_velocity[1]*val_normal[2] + val_velocity[2]*val_normal[1];
  val_Proj_Jac_Tensor[nSpecies+2][nSpecies+2] = -val_dPdrhoE*val_velocity[2]*val_normal[2] + val_velocity[2]*val_normal[2] + proj_vel;
  val_Proj_Jac_Tensor[nSpecies+2][nSpecies+3] = val_dPdrhoE * val_normal[2];
  val_Proj_Jac_Tensor[nSpecies+2][nSpecies+4] = val_dPdrhoEve * val_normal[2];
  
  val_Proj_Jac_Tensor[nSpecies+3][nSpecies]   = -val_dPdrhoE*val_velocity[0]*proj_vel + (*val_enthalpy)*val_normal[0];
  val_Proj_Jac_Tensor[nSpecies+3][nSpecies+1] = -val_dPdrhoE*val_velocity[1]*proj_vel + (*val_enthalpy)*val_normal[1];
  val_Proj_Jac_Tensor[nSpecies+3][nSpecies+2] = -val_dPdrhoE*val_velocity[2]*proj_vel + (*val_enthalpy)*val_normal[2];
  val_Proj_Jac_Tensor[nSpecies+3][nSpecies+3] = val_dPdrhoE*proj_vel + proj_vel;
  val_Proj_Jac_Tensor[nSpecies+3][nSpecies+4] = val_dPdrhoEve * proj_vel;
  
  val_Proj_Jac_Tensor[nSpecies+4][nSpecies]   = (*val_energy_ve) * val_normal[0];
  val_Proj_Jac_Tensor[nSpecies+4][nSpecies+1] = (*val_energy_ve) * val_normal[1];
  val_Proj_Jac_Tensor[nSpecies+4][nSpecies+2] = (*val_energy_ve) * val_normal[2];
  val_Proj_Jac_Tensor[nSpecies+4][nSpecies+3] = 0.0;
  val_Proj_Jac_Tensor[nSpecies+4][nSpecies+4] = proj_vel;
  
	for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
      val_Proj_Jac_Tensor[iVar][jVar] = val_scale * val_Proj_Jac_Tensor[iVar][jVar];
}


void CNumerics::GetInviscidArtCompProjJac(double *val_density, double *val_velocity, double *val_betainc2, double *val_normal,
		double val_scale, double **val_Proj_Jac_Tensor) {
	unsigned short iDim;
	double proj_vel;

	proj_vel = 0.0;
	for (iDim = 0; iDim < nDim; iDim++)
		proj_vel += val_velocity[iDim]*val_normal[iDim];

    if (nDim == 2) {
		val_Proj_Jac_Tensor[0][0] = 0.0;
		val_Proj_Jac_Tensor[0][1] = val_scale*(*val_betainc2)*val_normal[0]/(*val_density);
		val_Proj_Jac_Tensor[0][2] = val_scale*(*val_betainc2)*val_normal[1]/(*val_density);
        
		val_Proj_Jac_Tensor[1][0] = val_scale*val_normal[0];
		val_Proj_Jac_Tensor[1][1] = val_scale*(val_velocity[0]*val_normal[0] + proj_vel);
		val_Proj_Jac_Tensor[1][2] = val_scale*val_velocity[0]*val_normal[1];
        
		val_Proj_Jac_Tensor[2][0] = val_scale*val_normal[1];
		val_Proj_Jac_Tensor[2][1] = val_scale*val_velocity[1]*val_normal[0];
		val_Proj_Jac_Tensor[2][2] = val_scale*(val_velocity[1]*val_normal[1] + proj_vel);
	}
	else {
		val_Proj_Jac_Tensor[0][0] = 0.0;
		val_Proj_Jac_Tensor[0][1] = val_scale*(*val_betainc2)*val_normal[0]/(*val_density);
		val_Proj_Jac_Tensor[0][2] = val_scale*(*val_betainc2)*val_normal[1]/(*val_density);
		val_Proj_Jac_Tensor[0][3] = val_scale*(*val_betainc2)*val_normal[2]/(*val_density);

		val_Proj_Jac_Tensor[1][0] = val_scale*val_normal[0];
		val_Proj_Jac_Tensor[1][1] = val_scale*(val_velocity[0]*val_normal[0] + proj_vel);
		val_Proj_Jac_Tensor[1][2] = val_scale*val_velocity[0]*val_normal[1];
		val_Proj_Jac_Tensor[1][3] = val_scale*val_velocity[0]*val_normal[2];

		val_Proj_Jac_Tensor[2][0] = val_scale*val_normal[1];
		val_Proj_Jac_Tensor[2][1] = val_scale*val_velocity[1]*val_normal[0];
		val_Proj_Jac_Tensor[2][2] = val_scale*(val_velocity[1]*val_normal[1] + proj_vel);
		val_Proj_Jac_Tensor[2][3] = val_scale*val_velocity[1]*val_normal[2];

		val_Proj_Jac_Tensor[3][0] = val_scale*val_normal[2];
		val_Proj_Jac_Tensor[3][1] = val_scale*val_velocity[2]*val_normal[0];
		val_Proj_Jac_Tensor[3][2] = val_scale*val_velocity[2]*val_normal[1];
		val_Proj_Jac_Tensor[3][3] = val_scale*(val_velocity[2]*val_normal[2] + proj_vel);
	}

}

void CNumerics::GetInviscidArtComp_FreeSurf_ProjJac(double *val_density, double *val_ddensity, double *val_velocity, double *val_betainc2, double *val_levelset, double *val_normal,
                                                    double val_scale, double **val_Proj_Jac_Tensor) {
  
	double a = 0.0, b = 0.0, c = 0.0, d = 0.0, area2 = 0.0, nx = 0.0, ny = 0.0, nz = 0.0, u = 0.0, v = 0.0, w = 0.0;
  
  a = (*val_betainc2)/(*val_density);
  b = (*val_levelset)/(*val_density);
  c = (*val_ddensity);
  
  if (nDim == 2) {
    
    nx = val_normal[0];   ny = val_normal[1];   area2 = nx*nx + ny*ny;
    u = val_velocity[0];  v = val_velocity[1];  d = u*nx + v*ny;
    
    val_Proj_Jac_Tensor[0][0] = 0.0;
    val_Proj_Jac_Tensor[0][1] = val_scale*a*nx;
    val_Proj_Jac_Tensor[0][2] = val_scale*a*ny;
    val_Proj_Jac_Tensor[0][3] = - val_scale*a*c*d;
    
    
    val_Proj_Jac_Tensor[1][0] = val_scale*nx;
    val_Proj_Jac_Tensor[1][1] = val_scale*(d + nx*u);
    val_Proj_Jac_Tensor[1][2] = val_scale*ny*u;
    val_Proj_Jac_Tensor[1][3] = -val_scale*c*d*u;
    
    
    val_Proj_Jac_Tensor[2][0] = val_scale*ny;
    val_Proj_Jac_Tensor[2][1] = val_scale*nx*v;
    val_Proj_Jac_Tensor[2][2] = val_scale*(d + ny*v);
    val_Proj_Jac_Tensor[2][3] = -val_scale*c*d*v;
    
    
    val_Proj_Jac_Tensor[3][0] = 0.0;
    val_Proj_Jac_Tensor[3][1] = val_scale*b*nx;
    val_Proj_Jac_Tensor[3][2] = val_scale*b*ny;
    val_Proj_Jac_Tensor[3][3] = val_scale*(d - b*c*d);
    
  }
	else {
  
    nx = val_normal[0];   ny = val_normal[1];   nz = val_normal[2];   area2 = nx*nx + ny*ny + nz*nz;
    u = val_velocity[0];  v = val_velocity[1];  w = val_velocity[2];  d = u*nx + v*ny + w*nz;
  
    val_Proj_Jac_Tensor[0][0] = 0.0;
    val_Proj_Jac_Tensor[0][1] = val_scale*a*nx;
    val_Proj_Jac_Tensor[0][2] = val_scale*a*ny;
    val_Proj_Jac_Tensor[0][3] = val_scale*a*nz;
    val_Proj_Jac_Tensor[0][4] = - val_scale*a*c*d;
    
    
    val_Proj_Jac_Tensor[1][0] = val_scale*nx;
    val_Proj_Jac_Tensor[1][1] = val_scale*(d + nx*u);
    val_Proj_Jac_Tensor[1][2] = val_scale*ny*u;
    val_Proj_Jac_Tensor[1][3] = val_scale*nz*u;
    val_Proj_Jac_Tensor[1][4] = -val_scale*c*d*u;
    
    
    val_Proj_Jac_Tensor[2][0] = val_scale*ny;
    val_Proj_Jac_Tensor[2][1] = val_scale*nx*v;
    val_Proj_Jac_Tensor[2][2] = val_scale*(d + ny*v);
    val_Proj_Jac_Tensor[2][3] = val_scale*nz*v;
    val_Proj_Jac_Tensor[2][4] = -val_scale*c*d*v;
    
    val_Proj_Jac_Tensor[3][0] = val_scale*nz;
    val_Proj_Jac_Tensor[3][1] = val_scale*nx*w;
    val_Proj_Jac_Tensor[3][2] = val_scale*ny*w;
    val_Proj_Jac_Tensor[3][3] = val_scale*(d + nz*w);
    val_Proj_Jac_Tensor[3][4] = -val_scale*c*d*w;
    
    val_Proj_Jac_Tensor[3][0] = 0.0;
    val_Proj_Jac_Tensor[3][1] = val_scale*b*nx;
    val_Proj_Jac_Tensor[3][2] = val_scale*b*ny;
    val_Proj_Jac_Tensor[3][3] = val_scale*b*nz;
    val_Proj_Jac_Tensor[3][4] = val_scale*(d - b*c*d);
    
  }
  
}

void CNumerics::GetInviscidProjJac(double **val_velocity, double *val_energy, double *val_normal,
		double val_scale, double **val_Proj_Jac_Tensor) {
	unsigned short iDim, jDim;
	unsigned short iVar, jVar, iSpecies, loc = 0;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_Proj_Jac_Tensor[iVar][jVar] = 0.0;
		}
	}
	double sqvel,proj_vel;
	double phi, a1, a2;
	for(iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		loc = iSpecies * (nDim+2);
		Gamma = Vector_Gamma[iSpecies];
		Gamma_Minus_One = Gamma - 1.0;

		sqvel = 0.0;
		proj_vel = 0.0;
		for (iDim = 0; iDim < nDim; iDim++) {
			sqvel    += val_velocity[iSpecies][iDim]*val_velocity[iSpecies][iDim];
			proj_vel += val_velocity[iSpecies][iDim]*val_normal[iDim];
		}
		phi = 0.5*Gamma_Minus_One*sqvel;
		a1  = Gamma*val_energy[iSpecies]-phi;
		a2  = Gamma-1.0;

		val_Proj_Jac_Tensor[loc+0][loc+0] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
			val_Proj_Jac_Tensor[loc+0][loc+iDim+1] = val_scale*val_normal[iDim];
		val_Proj_Jac_Tensor[loc+0][loc+nDim+1] = 0.0;

		for (iDim = 0; iDim < nDim; iDim++) {
			val_Proj_Jac_Tensor[loc+iDim+1][loc+0] = val_scale*(val_normal[iDim]*phi - val_velocity[iSpecies][iDim]*proj_vel);
			for (jDim = 0; jDim < nDim; jDim++)
				val_Proj_Jac_Tensor[loc+iDim+1][loc+jDim+1] = val_scale*(val_normal[jDim]*val_velocity[iSpecies][iDim]-a2*val_normal[iDim]*val_velocity[iSpecies][jDim]);
			val_Proj_Jac_Tensor[loc+iDim+1][loc+iDim+1] += val_scale*proj_vel;
			val_Proj_Jac_Tensor[loc+iDim+1][loc+nDim+1] = val_scale*a2*val_normal[iDim];
		}

		val_Proj_Jac_Tensor[loc+nDim+1][loc+0] = val_scale*proj_vel*(phi-a1);
		for (iDim = 0; iDim < nDim; iDim++)
			val_Proj_Jac_Tensor[loc+nDim+1][loc+iDim+1] = val_scale*(val_normal[iDim]*a1-a2*val_velocity[iSpecies][iDim]*proj_vel);
		val_Proj_Jac_Tensor[loc+nDim+1][loc+nDim+1] = val_scale*Gamma*proj_vel;
	}
}


void CNumerics::GetInviscidProjJac_(double **val_velocity, double *val_energy, double *val_energy_vib, double *val_enthalpy,
		double *val_normal, double val_scale, double **val_Proj_Jac_Tensor, CConfig *config) {
	unsigned short iDim, jDim, nVar_Species;
	unsigned short iVar, jVar, iSpecies, loc = 0;
	double Energy_el;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_Proj_Jac_Tensor[iVar][jVar] = 0.0;			
		}
	}
	double sqvel,proj_vel;
	double dPdrho_s, dPdE, dPdEvib, a2;
	//	double phi, a1;

	for(iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		if ( iSpecies < nDiatomics ) {
			loc = (nDim+3)*iSpecies;
			nVar_Species = nDim+3;
		} else {
			loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);
			nVar_Species = nDim+2;
		}
		sqvel = 0.0;
		proj_vel = 0.0;
		for (iDim = 0; iDim < nDim; iDim++) {
			sqvel    += val_velocity[iSpecies][iDim]*val_velocity[iSpecies][iDim];
			proj_vel += val_velocity[iSpecies][iDim]*val_normal[iDim];
		}
		/*		if (iSpecies < nDiatomics){
			phi = 0.5*(GammaDiatomic-1.0)*sqvel;
			a1  = GammaDiatomic*val_energy[iSpecies]-phi;
			a2  = GammaDiatomic-1.0;
		}
		else {
			phi = 0.5*(GammaMonatomic-1.0)*sqvel;
			a1  = GammaMonatomic*val_energy[iSpecies]-phi;
			a2  = GammaMonatomic-1.0;
		}*/

		//		phi = 0.5*(Vector_Gamma[iSpecies]-1.0)*sqvel;
		//		a1  = Vector_Gamma[iSpecies]*val_energy[iSpecies]-phi;

		Energy_el = 0.0;
		dPdrho_s = (Vector_Gamma[iSpecies]-1.0)*(0.5*sqvel-config->GetEnthalpy_Formation(iSpecies) - Energy_el);
		dPdE = Vector_Gamma[iSpecies]-1.0;
		dPdEvib = -(Vector_Gamma[iSpecies]-1.0);
		a2  = Vector_Gamma[iSpecies]-1.0;

		/*--- New implementation ---*/
		val_Proj_Jac_Tensor[loc+0][loc+0] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
			val_Proj_Jac_Tensor[loc+0][loc+iDim+1] = val_scale*val_normal[iDim];
		val_Proj_Jac_Tensor[loc+0][loc+nDim+1] = 0.0;

		for (iDim = 0; iDim < nDim; iDim++) {
			val_Proj_Jac_Tensor[loc+iDim+1][loc+0] = val_scale*(val_normal[iDim]*dPdrho_s - val_velocity[iSpecies][iDim]*proj_vel);
			for (jDim = 0; jDim < nDim; jDim++)
				val_Proj_Jac_Tensor[loc+iDim+1][loc+jDim+1] = val_scale*(-a2*val_velocity[iSpecies][jDim]*val_normal[iDim] + val_velocity[iSpecies][iDim]*val_normal[jDim]);				
			val_Proj_Jac_Tensor[loc+iDim+1][loc+iDim+1] += val_scale*proj_vel;
			val_Proj_Jac_Tensor[loc+iDim+1][loc+nDim+1] = val_scale*dPdE*val_normal[iDim];			
		}

		val_Proj_Jac_Tensor[loc+nDim+1][loc+0] = val_scale*proj_vel*(dPdrho_s - val_enthalpy[iSpecies]);
		for (iDim = 0; iDim < nDim; iDim++)
			val_Proj_Jac_Tensor[loc+nDim+1][loc+iDim+1] = val_scale*(-a2*val_velocity[iSpecies][iDim]*proj_vel + val_enthalpy[iSpecies]*val_normal[iDim]);
		val_Proj_Jac_Tensor[loc+nDim+1][loc+nDim+1] = val_scale*(1.0+dPdE)*proj_vel;

		/*--- Populate additional row and column to account for diatomic species ---*/
		if (iSpecies < nDiatomics) {
			val_Proj_Jac_Tensor[loc+0][loc+nDim+2] = 0.0;
			val_Proj_Jac_Tensor[loc+nDim+2][loc+0] = -val_scale * val_energy_vib[iSpecies] * proj_vel;
			for (iDim = 0; iDim < nDim; iDim++) { 
				val_Proj_Jac_Tensor[loc+iDim+1][loc+nDim+2] = val_scale * dPdEvib * val_normal[iDim];
				val_Proj_Jac_Tensor[loc+nDim+2][loc+iDim+1] = val_scale * val_energy_vib[iSpecies] * val_normal[iDim];
			}
			val_Proj_Jac_Tensor[loc+nDim+2][loc+nDim+1] = 0.0;
			val_Proj_Jac_Tensor[loc+nDim+1][loc+nDim+2] = val_scale * dPdEvib * proj_vel;
			val_Proj_Jac_Tensor[loc+nDim+2][loc+nDim+2] = val_scale * proj_vel;
		}
	}
}


void CNumerics::SetPastSol (double *val_u_nM1, double *val_u_n, double *val_u_nP1) {
	unsigned short iVar;

	for(iVar = 0; iVar < nVar; iVar++) {
		U_nM1[iVar] = val_u_nM1[iVar];
		U_n[iVar] = val_u_n[iVar];
		U_nP1[iVar] = val_u_nP1[iVar];
	}

}
void CNumerics::SetPastVolume (double val_volume_nM1, double val_volume_n, double val_volume_nP1) {
	Volume_nM1 = val_volume_nM1;
	Volume_n = val_volume_n;
	Volume_nP1 = val_volume_nP1;
}


void CNumerics::GetPMatrix(double *val_density, double *val_velocity,
		double *val_soundspeed, double *val_normal, double **val_p_tensor) {
//************************************************//
// Please do not delete //SU2_CPP2C comment lines //
//************************************************//

//SU2_CPP2C SUB START GetPMatrix
//SU2_CPP2C SUB VARS *val_density val_velocity *val_soundspeed val_p_tensor val_normal

	double sqvel, rhooc, rhoxc, c2;

	rhooc = *val_density / *val_soundspeed,
			rhoxc = *val_density * *val_soundspeed,
			c2 = *val_soundspeed * *val_soundspeed;

	if(nDim == 2) {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];

		val_p_tensor[0][0]=1.0;
		val_p_tensor[0][1]=0.0;
		val_p_tensor[0][2]=0.5*rhooc;
		val_p_tensor[0][3]=0.5*rhooc;

		val_p_tensor[1][0]=val_velocity[0];
		val_p_tensor[1][1]=*val_density*val_normal[1];
		val_p_tensor[1][2]=0.5*(val_velocity[0]*rhooc+val_normal[0]**val_density);
		val_p_tensor[1][3]=0.5*(val_velocity[0]*rhooc-val_normal[0]**val_density);

		val_p_tensor[2][0]=val_velocity[1];
		val_p_tensor[2][1]=-*val_density*val_normal[0];
		val_p_tensor[2][2]=0.5*(val_velocity[1]*rhooc+val_normal[1]**val_density);
		val_p_tensor[2][3]=0.5*(val_velocity[1]*rhooc-val_normal[1]**val_density);

		val_p_tensor[3][0]=0.5*sqvel;
		val_p_tensor[3][1]=*val_density*val_velocity[0]*val_normal[1]-*val_density*val_velocity[1]*val_normal[0];
		val_p_tensor[3][2]=0.5*(0.5*sqvel*rhooc+*val_density*val_velocity[0]*val_normal[0]+*val_density*val_velocity[1]*val_normal[1]+rhoxc/Gamma_Minus_One);
		val_p_tensor[3][3]=0.5*(0.5*sqvel*rhooc-*val_density*val_velocity[0]*val_normal[0]-*val_density*val_velocity[1]*val_normal[1]+rhoxc/Gamma_Minus_One);
	} 
	else {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];

		val_p_tensor[0][0]=val_normal[0];
		val_p_tensor[0][1]=val_normal[1];
		val_p_tensor[0][2]=val_normal[2];
		val_p_tensor[0][3]=0.5*rhooc;
		val_p_tensor[0][4]=0.5*rhooc;

		val_p_tensor[1][0]=val_velocity[0]*val_normal[0];
		val_p_tensor[1][1]=val_velocity[0]*val_normal[1]-*val_density*val_normal[2];
		val_p_tensor[1][2]=val_velocity[0]*val_normal[2]+*val_density*val_normal[1];
		val_p_tensor[1][3]=0.5*(val_velocity[0]*rhooc+*val_density*val_normal[0]);
		val_p_tensor[1][4]=0.5*(val_velocity[0]*rhooc-*val_density*val_normal[0]);

		val_p_tensor[2][0]=val_velocity[1]*val_normal[0]+*val_density*val_normal[2];
		val_p_tensor[2][1]=val_velocity[1]*val_normal[1];
		val_p_tensor[2][2]=val_velocity[1]*val_normal[2]-*val_density*val_normal[0];
		val_p_tensor[2][3]=0.5*(val_velocity[1]*rhooc+*val_density*val_normal[1]);
		val_p_tensor[2][4]=0.5*(val_velocity[1]*rhooc-*val_density*val_normal[1]);

		val_p_tensor[3][0]=val_velocity[2]*val_normal[0]-*val_density*val_normal[1];
		val_p_tensor[3][1]=val_velocity[2]*val_normal[1]+*val_density*val_normal[0];
		val_p_tensor[3][2]=val_velocity[2]*val_normal[2];
		val_p_tensor[3][3]=0.5*(val_velocity[2]*rhooc+*val_density*val_normal[2]);
		val_p_tensor[3][4]=0.5*(val_velocity[2]*rhooc-*val_density*val_normal[2]);

		val_p_tensor[4][0]=0.5*sqvel*val_normal[0]+*val_density*val_velocity[1]*val_normal[2]-*val_density*val_velocity[2]*val_normal[1];
		val_p_tensor[4][1]=0.5*sqvel*val_normal[1]-*val_density*val_velocity[0]*val_normal[2]+*val_density*val_velocity[2]*val_normal[0];
		val_p_tensor[4][2]=0.5*sqvel*val_normal[2]+*val_density*val_velocity[0]*val_normal[1]-*val_density*val_velocity[1]*val_normal[0];
		val_p_tensor[4][3]=0.5*(0.5*sqvel*rhooc+*val_density*(val_velocity[0]*val_normal[0]+val_velocity[1]*val_normal[1]+val_velocity[2]*val_normal[2])+rhoxc/Gamma_Minus_One);
		val_p_tensor[4][4]=0.5*(0.5*sqvel*rhooc-*val_density*(val_velocity[0]*val_normal[0]+val_velocity[1]*val_normal[1]+val_velocity[2]*val_normal[2])+rhoxc/Gamma_Minus_One);
	}

//SU2_CPP2C SUB END GetPMatrix
}


void CNumerics::GetPMatrix(double *val_density, double *val_velocity, double *val_enthalpy,
                           double *val_energy_ve, double *val_soundspeed, double *val_dPdrhos,
                           double val_dPdrhoE, double val_dPdrhoEve, double *val_normal,
                           double *l, double *m, double **val_p_tensor) {
//************************************************//
// Please do not delete //SU2_CPP2C comment lines //
//************************************************//
  
//SU2_CPP2C SUB START GetPMatrix
//SU2_CPP2C SUB VARS *val_density val_velocity *val_soundspeed val_p_tensor val_normal
  
  
  // P matrix is equivalent to the L matrix in Gnoffo
  
  unsigned short iSpecies, iDim, iVar, jVar;
	double sqvel, rho, a2;
  double U, V, W;
  
  for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
      val_p_tensor[iVar][jVar] = 0.0;
  
  /*--- Pre-compute useful quantities ---*/
  sqvel = 0.0;
  rho   = 0.0;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    rho += val_density[iSpecies];
  U = 0.0;  V = 0.0;  W = 0.0;
  for (iDim = 0; iDim < nDim; iDim++) {
    U     += val_velocity[iDim] * val_normal[iDim];
    V     += val_velocity[iDim] * l[iDim];
    W     += val_velocity[iDim] * m[iDim];
    sqvel += val_velocity[iDim] * val_velocity[iDim];
  }
  a2 = (*val_soundspeed) * (*val_soundspeed);
  
	if(nDim == 2) {
		cout << "P matrix not implemented for 2-D Flows!!" << endl;
#ifdef NO_MPI
    exit(1);
#else
    MPI::COMM_WORLD.Abort(1);
    MPI::Finalize();
#endif
	}
	else {
    
    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
      val_p_tensor[iSpecies][iSpecies]   = 1.0/a2;
      val_p_tensor[iSpecies][nSpecies]   = 0.0;
      val_p_tensor[iSpecies][nSpecies+1] = 0.0;
      val_p_tensor[iSpecies][nSpecies+2] = val_density[iSpecies] / (2.0*rho*a2);
      val_p_tensor[iSpecies][nSpecies+3] = val_density[iSpecies] / (2.0*rho*a2);
      val_p_tensor[iSpecies][nSpecies+4] = 0.0;
      
      val_p_tensor[nSpecies][iSpecies]   = val_velocity[0] / a2;
      val_p_tensor[nSpecies+1][iSpecies] = val_velocity[1] / a2;
      val_p_tensor[nSpecies+2][iSpecies] = val_velocity[2] / a2;
      val_p_tensor[nSpecies+3][iSpecies] = (val_dPdrhoE*sqvel-val_dPdrhos[iSpecies]) / (val_dPdrhoE*a2);
      val_p_tensor[nSpecies+4][iSpecies] = 0.0;
    }
    
		val_p_tensor[nSpecies][nSpecies]     = l[0];
    val_p_tensor[nSpecies][nSpecies+1]   = m[0];
    val_p_tensor[nSpecies][nSpecies+2]   = (val_velocity[0]+(*val_soundspeed)*val_normal[0]) / (2.0*a2);
    val_p_tensor[nSpecies][nSpecies+3]   = (val_velocity[0]-(*val_soundspeed)*val_normal[0]) / (2.0*a2);
    val_p_tensor[nSpecies][nSpecies+4]   = 0.0;
    
    val_p_tensor[nSpecies+1][nSpecies]   = l[1];
    val_p_tensor[nSpecies+1][nSpecies+1] = m[1];
    val_p_tensor[nSpecies+1][nSpecies+2] = (val_velocity[1]+(*val_soundspeed)*val_normal[1]) / (2.0*a2);
    val_p_tensor[nSpecies+1][nSpecies+3] = (val_velocity[1]-(*val_soundspeed)*val_normal[1]) / (2.0*a2);
    val_p_tensor[nSpecies+1][nSpecies+4] = 0.0;
    
    val_p_tensor[nSpecies+2][nSpecies]   = l[2];
    val_p_tensor[nSpecies+2][nSpecies+1] = m[2];
    val_p_tensor[nSpecies+2][nSpecies+2] = (val_velocity[2]+(*val_soundspeed)*val_normal[2]) / (2.0*a2);
    val_p_tensor[nSpecies+2][nSpecies+3] = (val_velocity[2]-(*val_soundspeed)*val_normal[2]) / (2.0*a2);
    val_p_tensor[nSpecies+2][nSpecies+4] = 0.0;
    
    val_p_tensor[nSpecies+3][nSpecies]   = V;
    val_p_tensor[nSpecies+3][nSpecies+1] = W;
    val_p_tensor[nSpecies+3][nSpecies+2] = ((*val_enthalpy)+(*val_soundspeed)*U) / (2.0*a2);
    val_p_tensor[nSpecies+3][nSpecies+3] = ((*val_enthalpy)-(*val_soundspeed)*U) / (2.0*a2);
    val_p_tensor[nSpecies+3][nSpecies+4] = -val_dPdrhoEve / (val_dPdrhoE*a2);
    
    val_p_tensor[nSpecies+4][nSpecies]   = 0.0;
    val_p_tensor[nSpecies+4][nSpecies+1] = 0.0;
    val_p_tensor[nSpecies+4][nSpecies+2] = (*val_energy_ve) / (2.0*a2);
    val_p_tensor[nSpecies+4][nSpecies+3] = (*val_energy_ve) / (2.0*a2);
    val_p_tensor[nSpecies+4][nSpecies+4] = 1.0 / a2;
	}
//SU2_CPP2C SUB END GetPMatrix
}


void CNumerics::GetPMatrix(double *val_density, double **val_velocity,
		double *val_soundspeed, double *val_normal, double **val_p_tensor) {

	double sqvel, rhooc, rhoxc, c2, u ,v, w,rho ;
	unsigned short loc, iVar, jVar, iSpecies;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_p_tensor[iVar][jVar]= 0.0;
		}
	}

	for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		rhooc = val_density[iSpecies]    / val_soundspeed[iSpecies];
		rhoxc = val_density[iSpecies]	* val_soundspeed[iSpecies];
		c2    = val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
		rho   = val_density[iSpecies];
		loc   = iSpecies * (nDim+2);
		Gamma = Vector_Gamma[iSpecies];
		Gamma_Minus_One = Gamma - 1.0;
		if (nDim == 3) {
			u	  = val_velocity[iSpecies][0];
			v     = val_velocity[iSpecies][1];
			w     = val_velocity[iSpecies][2];
			sqvel = u*u + v*v + w*w;

			val_p_tensor[loc +0][loc +0]=val_normal[0];
			val_p_tensor[loc +0][loc +1]=val_normal[1];
			val_p_tensor[loc +0][loc +2]=val_normal[2];
			val_p_tensor[loc +0][loc +3]=0.5*rhooc;
			val_p_tensor[loc +0][loc +4]=0.5*rhooc;

			val_p_tensor[loc +1][loc +0]=u*val_normal[0];
			val_p_tensor[loc +1][loc +1]=u*val_normal[1]-rho*val_normal[2];
			val_p_tensor[loc +1][loc +2]=u*val_normal[2]+rho*val_normal[1];
			val_p_tensor[loc +1][loc +3]=0.5*(u*rhooc+rho*val_normal[0]);
			val_p_tensor[loc +1][loc +4]=0.5*(u*rhooc-rho*val_normal[0]);

			val_p_tensor[loc +2][loc +0]=v*val_normal[0]+rho*val_normal[2];
			val_p_tensor[loc +2][loc +1]=v*val_normal[1];
			val_p_tensor[loc +2][loc +2]=v*val_normal[2]-rho*val_normal[0];
			val_p_tensor[loc +2][loc +3]=0.5*(v*rhooc+rho*val_normal[1]);
			val_p_tensor[loc +2][loc +4]=0.5*(v*rhooc-rho*val_normal[1]);

			val_p_tensor[loc +3][loc +0]=w*val_normal[0]-rho*val_normal[1];
			val_p_tensor[loc +3][loc +1]=w*val_normal[1]+rho*val_normal[0];
			val_p_tensor[loc +3][loc +2]=w*val_normal[2];
			val_p_tensor[loc +3][loc +3]=0.5*(w*rhooc+rho*val_normal[2]);
			val_p_tensor[loc +3][loc +4]=0.5*(w*rhooc-rho*val_normal[2]);

			val_p_tensor[loc +4][loc +0]=0.5*sqvel*val_normal[0]+rho*v*val_normal[2]-rho*w*val_normal[1];
			val_p_tensor[loc +4][loc +1]=0.5*sqvel*val_normal[1]-rho*u*val_normal[2]+rho*w*val_normal[0];
			val_p_tensor[loc +4][loc +2]=0.5*sqvel*val_normal[2]+rho*u*val_normal[1]-rho*v*val_normal[0];
			val_p_tensor[loc +4][loc +3]=0.5*(0.5*sqvel*rhooc+rho*(u*val_normal[0]+v*val_normal[1]+w*val_normal[2])+rhoxc/Gamma_Minus_One);
			val_p_tensor[loc +4][loc +4]=0.5*(0.5*sqvel*rhooc-rho*(u*val_normal[0]+v*val_normal[1]+w*val_normal[2])+rhoxc/Gamma_Minus_One);
		}
		if(nDim == 2) {
			u	  = val_velocity[iSpecies][0];
			v     = val_velocity[iSpecies][1];
			sqvel = u*u + v*v;

			val_p_tensor[loc+0][loc+0]=1.0;
			val_p_tensor[loc+0][loc+1]=0.0;
			val_p_tensor[loc+0][loc+2]=0.5*rhooc;
			val_p_tensor[loc+0][loc+3]=0.5*rhooc;

			val_p_tensor[loc+1][loc+0]=u;
			val_p_tensor[loc+1][loc+1]=rho *val_normal[1];
			val_p_tensor[loc+1][loc+2]=0.5*(u*rhooc+val_normal[0]*rho);
			val_p_tensor[loc+1][loc+3]=0.5*(u*rhooc-val_normal[0]*rho);

			val_p_tensor[loc+2][loc+0]=v;
			val_p_tensor[loc+2][loc+1]=-rho*val_normal[0];
			val_p_tensor[loc+2][loc+2]=0.5*(v*rhooc+val_normal[1]*rho);
			val_p_tensor[loc+2][loc+3]=0.5*(v*rhooc-val_normal[1]*rho);

			val_p_tensor[loc+3][loc+0]=0.5*sqvel;
			val_p_tensor[loc+3][loc+1]=rho*u*val_normal[1]-rho*v*val_normal[0];
			val_p_tensor[loc+3][loc+2]=0.5*(0.5*sqvel*rhooc+rho*u*val_normal[0]+rho*v*val_normal[1]+rhoxc/Gamma_Minus_One);
			val_p_tensor[loc+3][loc+3]=0.5*(0.5*sqvel*rhooc-rho*u*val_normal[0]-rho*v*val_normal[1]+rhoxc/Gamma_Minus_One);

		}
	}
}

void CNumerics::GetPMatrix_(double *val_density, double **val_velocity, double *val_enthalpy,
		double *val_soundspeed, double *val_energy_vib, double *val_energy_el,
		CConfig *config, double *val_normal, double **val_p_tensor) {

	double sqvel, rhooc, rhoxc, c2, hf;
	unsigned short loc, iVar, jVar, iSpecies;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_p_tensor[iVar][jVar]= 0.0;
		}
	}

	if (nDim == 3) {
		for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhooc = val_density[iSpecies]	 / val_soundspeed[iSpecies],
					rhoxc = val_density[iSpecies]	 * val_soundspeed[iSpecies],
					c2 =    val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1]+val_velocity[iSpecies][2]*val_velocity[iSpecies][2];
			hf = config->GetEnthalpy_Formation(iSpecies);

			val_p_tensor[loc+0][loc+0]=val_normal[0];
			val_p_tensor[loc+0][loc+1]=val_normal[1];
			val_p_tensor[loc+0][loc+2]=val_normal[2];
			val_p_tensor[loc+0][loc+3]=0.5*rhooc;
			val_p_tensor[loc+0][loc+4]=0.5*rhooc;

			val_p_tensor[loc+1][loc+0]=val_velocity[iSpecies][0]*val_normal[0];
			val_p_tensor[loc+1][loc+1]=val_velocity[iSpecies][0]*val_normal[1]-val_density[iSpecies]*val_normal[2];
			val_p_tensor[loc+1][loc+2]=val_velocity[iSpecies][0]*val_normal[2]+val_density[iSpecies]*val_normal[1];
			val_p_tensor[loc+1][loc+3]=0.5*(val_velocity[iSpecies][0]*rhooc+val_density[iSpecies]*val_normal[0]);
			val_p_tensor[loc+1][loc+4]=0.5*(val_velocity[iSpecies][0]*rhooc-val_density[iSpecies]*val_normal[0]);

			val_p_tensor[loc+2][loc+0]=val_velocity[iSpecies][1]*val_normal[0]+val_density[iSpecies]*val_normal[2];
			val_p_tensor[loc+2][loc+1]=val_velocity[iSpecies][1]*val_normal[1];
			val_p_tensor[loc+2][loc+2]=val_velocity[iSpecies][1]*val_normal[2]-val_density[iSpecies]*val_normal[0];
			val_p_tensor[loc+2][loc+3]=0.5*(val_velocity[iSpecies][1]*rhooc+val_density[iSpecies]*val_normal[1]);
			val_p_tensor[loc+2][loc+4]=0.5*(val_velocity[iSpecies][1]*rhooc-val_density[iSpecies]*val_normal[1]);

			val_p_tensor[loc+3][loc+0]=val_velocity[iSpecies][2]*val_normal[0]-val_density[iSpecies]*val_normal[1];
			val_p_tensor[loc+3][loc+1]=val_velocity[iSpecies][2]*val_normal[1]+val_density[iSpecies]*val_normal[0];
			val_p_tensor[loc+3][loc+2]=val_velocity[iSpecies][2]*val_normal[2];
			val_p_tensor[loc+3][loc+3]=0.5*(val_velocity[iSpecies][2]*rhooc+val_density[iSpecies]*val_normal[2]);
			val_p_tensor[loc+3][loc+4]=0.5*(val_velocity[iSpecies][2]*rhooc-val_density[iSpecies]*val_normal[2]);

			if (iSpecies < nDiatomics) {

				val_p_tensor[loc+4][loc+0]=(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies])*val_normal[0]+val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[2]-val_density[iSpecies]*val_velocity[iSpecies][2]*val_normal[1];
				val_p_tensor[loc+4][loc+1]=(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies])*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[2]+val_density[iSpecies]*val_velocity[iSpecies][2]*val_normal[0];
				val_p_tensor[loc+4][loc+2]=(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies])*val_normal[2]+val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[0];
				val_p_tensor[loc+4][loc+3]=0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies])+val_density[iSpecies]*(val_velocity[iSpecies][0]*val_normal[0]/2.0+val_velocity[iSpecies][1]*val_normal[1]/2.0+val_velocity[iSpecies][2]*val_normal[2]/2.0)+rhoxc/(2.0*(GammaDiatomic-1.0));
				val_p_tensor[loc+4][loc+4]=0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies])-val_density[iSpecies]*(val_velocity[iSpecies][0]*val_normal[0]/2.0+val_velocity[iSpecies][1]*val_normal[1]/2.0+val_velocity[iSpecies][2]*val_normal[2]/2.0)+rhoxc/(2.0*(GammaDiatomic-1.0));

				/*--- Last column ---*/
				val_p_tensor[loc+0][loc+5] = 0.0;
				val_p_tensor[loc+1][loc+5] = 0.0;
				val_p_tensor[loc+2][loc+5] = 0.0;
				val_p_tensor[loc+3][loc+5] = 0.0;
				val_p_tensor[loc+4][loc+5] = val_density[iSpecies];
				val_p_tensor[loc+5][loc+5] = val_density[iSpecies];

				/*--- Last row ---*/
				val_p_tensor[loc+5][loc+0] = val_energy_vib[iSpecies]*val_normal[0];
				val_p_tensor[loc+5][loc+1] = val_energy_vib[iSpecies]*val_normal[1];
				val_p_tensor[loc+5][loc+2] = val_energy_vib[iSpecies]*val_normal[2];
				val_p_tensor[loc+5][loc+3] = val_energy_vib[iSpecies]*val_density[iSpecies]/(2.0*val_soundspeed[iSpecies]);
				val_p_tensor[loc+5][loc+4] = val_energy_vib[iSpecies]*val_density[iSpecies]/(2.0*val_soundspeed[iSpecies]);				

			}
			else {
				val_p_tensor[loc+4][loc+0]=(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies])*val_normal[0]+val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[2]-val_density[iSpecies]*val_velocity[iSpecies][2]*val_normal[1];
				val_p_tensor[loc+4][loc+1]=(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies])*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[2]+val_density[iSpecies]*val_velocity[iSpecies][2]*val_normal[0];
				val_p_tensor[loc+4][loc+2]=(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies])*val_normal[2]+val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[0];
				val_p_tensor[loc+4][loc+3]=0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies])+val_density[iSpecies]*(val_velocity[iSpecies][0]*val_normal[0]/2.0+val_velocity[iSpecies][1]*val_normal[1]/2.0+val_velocity[iSpecies][2]*val_normal[2]/2.0)+rhoxc/(2.0*(GammaMonatomic-1.0));
				val_p_tensor[loc+4][loc+4]=0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies])-val_density[iSpecies]*(val_velocity[iSpecies][0]*val_normal[0]/2.0+val_velocity[iSpecies][1]*val_normal[1]/2.0+val_velocity[iSpecies][2]*val_normal[2]/2.0)+rhoxc/(2.0*(GammaMonatomic-1.0));
			}

		}
	}

	if(nDim == 2) {
		for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhooc = val_density[iSpecies]	 / val_soundspeed[iSpecies],
					rhoxc = val_density[iSpecies]	 * val_soundspeed[iSpecies],
					c2 =    val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1];
			hf = config->GetEnthalpy_Formation(iSpecies);

			val_p_tensor[loc+0][loc+0]=1.0;
			val_p_tensor[loc+0][loc+1]=0.0;
			val_p_tensor[loc+0][loc+2]=0.5*rhooc;
			val_p_tensor[loc+0][loc+3]=0.5*rhooc;

			val_p_tensor[loc+1][loc+0]=val_velocity[iSpecies][0];
			val_p_tensor[loc+1][loc+1]=val_density[iSpecies]*val_normal[1];
			val_p_tensor[loc+1][loc+2]=0.5*(val_velocity[iSpecies][0]*rhooc+val_normal[0]*val_density[iSpecies]);
			val_p_tensor[loc+1][loc+3]=0.5*(val_velocity[iSpecies][0]*rhooc-val_normal[0]*val_density[iSpecies]);

			val_p_tensor[loc+2][loc+0]=val_velocity[iSpecies][1];
			val_p_tensor[loc+2][loc+1]=-val_density[iSpecies]*val_normal[0];
			val_p_tensor[loc+2][loc+2]=0.5*(val_velocity[iSpecies][1]*rhooc+val_normal[1]*val_density[iSpecies]);
			val_p_tensor[loc+2][loc+3]=0.5*(val_velocity[iSpecies][1]*rhooc-val_normal[1]*val_density[iSpecies]);

			if (iSpecies < nDiatomics) {
				val_p_tensor[loc+3][loc+0] = 0.5*sqvel + config->GetEnthalpy_Formation(iSpecies) + val_energy_vib[iSpecies] + val_energy_el[iSpecies];
				val_p_tensor[loc+3][loc+1] = val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[1] - val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[0];
				val_p_tensor[loc+3][loc+2] = 0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies]) + 0.5*val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[0] + 0.5*val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[1] + 0.5*rhoxc/(GammaDiatomic-1.0);
				val_p_tensor[loc+3][loc+3] = 0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_vib[iSpecies]+val_energy_el[iSpecies]) - 0.5*val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[0] - 0.5*val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[1] + 0.5*rhoxc/(GammaDiatomic-1.0);

				/*--- Last column ---*/
				val_p_tensor[loc+0][loc+4] = 0.0;
				val_p_tensor[loc+1][loc+4] = 0.0;
				val_p_tensor[loc+2][loc+4] = 0.0;
				val_p_tensor[loc+3][loc+4] = val_density[iSpecies];
				val_p_tensor[loc+4][loc+4] = val_density[iSpecies];

				/*--- Last row ---*/
				val_p_tensor[loc+4][loc+0] = val_energy_vib[iSpecies];
				val_p_tensor[loc+4][loc+1] = 0.0;
				val_p_tensor[loc+4][loc+2] = val_energy_vib[iSpecies]*val_density[iSpecies]/(2.0*val_soundspeed[iSpecies]);
				val_p_tensor[loc+4][loc+3] = val_energy_vib[iSpecies]*val_density[iSpecies]/(2.0*val_soundspeed[iSpecies]);		
			}
			else {
				val_p_tensor[loc+3][loc+0] = 0.5*sqvel + config->GetEnthalpy_Formation(iSpecies) + val_energy_el[iSpecies];
				val_p_tensor[loc+3][loc+1] = val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[1] - val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[0];
				val_p_tensor[loc+3][loc+2] = 0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies]) + 0.5*val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[0] + 0.5*val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[1] + 0.5*rhoxc/(GammaMonatomic-1.0);
				val_p_tensor[loc+3][loc+3] = 0.5*rhooc*(0.5*sqvel+config->GetEnthalpy_Formation(iSpecies)+val_energy_el[iSpecies]) - 0.5*val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[0] - 0.5*val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[1] + 0.5*rhoxc/(GammaMonatomic-1.0);
			}
		}
	}
}


void CNumerics::GetPMatrix_AM(double *val_density, double **val_velocity,
		double *val_soundspeed, double *val_normal, double **val_p_tensor, double *val_vibrational_energy) {

	double sqvel, rhooc, rhoxc, c2;
	unsigned short loc, iVar, jVar, iSpecies;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_p_tensor[iVar][jVar]= 0.0;
		}
	}

	if (nDim == 3) {
		for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhooc = val_density[iSpecies]	 / val_soundspeed[iSpecies],
					rhoxc = val_density[iSpecies]	 * val_soundspeed[iSpecies],
					c2 =    val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1]+val_velocity[iSpecies][2]*val_velocity[iSpecies][2];

			val_p_tensor[loc+0][loc+0]=val_normal[0];
			val_p_tensor[loc+0][loc+1]=val_normal[1];
			val_p_tensor[loc+0][loc+2]=val_normal[2];
			val_p_tensor[loc+0][loc+3]=0.5*rhooc;
			val_p_tensor[loc+0][loc+4]=0.5*rhooc;

			val_p_tensor[loc+1][loc+0]=val_velocity[iSpecies][0]*val_normal[0];
			val_p_tensor[loc+1][loc+1]=val_velocity[iSpecies][0]*val_normal[1]-val_density[iSpecies]*val_normal[2];
			val_p_tensor[loc+1][loc+2]=val_velocity[iSpecies][0]*val_normal[2]+val_density[iSpecies]*val_normal[1];
			val_p_tensor[loc+1][loc+3]=0.5*(val_velocity[iSpecies][0]*rhooc+val_density[iSpecies]*val_normal[0]);
			val_p_tensor[loc+1][loc+4]=0.5*(val_velocity[iSpecies][0]*rhooc-val_density[iSpecies]*val_normal[0]);

			val_p_tensor[loc+2][loc+0]=val_velocity[iSpecies][1]*val_normal[0]+val_density[iSpecies]*val_normal[2];
			val_p_tensor[loc+2][loc+1]=val_velocity[iSpecies][1]*val_normal[1];
			val_p_tensor[loc+2][loc+2]=val_velocity[iSpecies][1]*val_normal[2]-val_density[iSpecies]*val_normal[0];
			val_p_tensor[loc+2][loc+3]=0.5*(val_velocity[iSpecies][1]*rhooc+val_density[iSpecies]*val_normal[1]);
			val_p_tensor[loc+2][loc+4]=0.5*(val_velocity[iSpecies][1]*rhooc-val_density[iSpecies]*val_normal[1]);

			val_p_tensor[loc+3][loc+0]=val_velocity[iSpecies][2]*val_normal[0]-val_density[iSpecies]*val_normal[1];
			val_p_tensor[loc+3][loc+1]=val_velocity[iSpecies][2]*val_normal[1]+val_density[iSpecies]*val_normal[0];
			val_p_tensor[loc+3][loc+2]=val_velocity[iSpecies][2]*val_normal[2];
			val_p_tensor[loc+3][loc+3]=0.5*(val_velocity[iSpecies][2]*rhooc+val_density[iSpecies]*val_normal[2]);
			val_p_tensor[loc+3][loc+4]=0.5*(val_velocity[iSpecies][2]*rhooc-val_density[iSpecies]*val_normal[2]);

			val_p_tensor[loc+4][loc+0]=0.5*sqvel*val_normal[0]+val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[2]-val_density[iSpecies]*val_velocity[iSpecies][2]*val_normal[1];
			val_p_tensor[loc+4][loc+1]=0.5*sqvel*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[2]+val_density[iSpecies]*val_velocity[iSpecies][2]*val_normal[0];
			val_p_tensor[loc+4][loc+2]=0.5*sqvel*val_normal[2]+val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[0];
			val_p_tensor[loc+4][loc+3]=0.5*(0.5*sqvel*rhooc+val_density[iSpecies]*(val_velocity[iSpecies][0]*val_normal[0]+val_velocity[iSpecies][1]*val_normal[1]+val_velocity[iSpecies][2]*val_normal[2])+rhoxc/(GammaDiatomic-1.0));
			val_p_tensor[loc+4][loc+4]=0.5*(0.5*sqvel*rhooc-val_density[iSpecies]*(val_velocity[iSpecies][0]*val_normal[0]+val_velocity[iSpecies][1]*val_normal[1]+val_velocity[iSpecies][2]*val_normal[2])+rhoxc/(GammaDiatomic-1.0));


			if (iSpecies < nDiatomics) {
				val_p_tensor[loc+0][loc+5] = 1.0;
				val_p_tensor[loc+1][loc+5] = 1.0;
				val_p_tensor[loc+2][loc+5] = 1.0;
				val_p_tensor[loc+3][loc+5] = 1.0;
				val_p_tensor[loc+4][loc+5] = 1.0;
				val_p_tensor[loc+5][loc+5] = 1.0;
			}

		}
	}

	if(nDim == 2) {
		for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhooc = val_density[iSpecies]	 / val_soundspeed[iSpecies],
					rhoxc = val_density[iSpecies]	 * val_soundspeed[iSpecies],
					c2 =    val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1];

			val_p_tensor[loc+0][loc+0]=1.0;
			val_p_tensor[loc+0][loc+1]=0.0;
			val_p_tensor[loc+0][loc+2]=0.5*rhooc;
			val_p_tensor[loc+0][loc+3]=0.5*rhooc;

			val_p_tensor[loc+1][loc+0]=val_velocity[iSpecies][0];
			val_p_tensor[loc+1][loc+1]=val_density[iSpecies]*val_normal[1];
			val_p_tensor[loc+1][loc+2]=0.5*(val_velocity[iSpecies][0]*rhooc+val_normal[0]*val_density[iSpecies]);
			val_p_tensor[loc+1][loc+3]=0.5*(val_velocity[iSpecies][0]*rhooc-val_normal[0]*val_density[iSpecies]);

			val_p_tensor[loc+2][loc+0]=val_velocity[iSpecies][1];
			val_p_tensor[loc+2][loc+1]=-val_density[iSpecies]*val_normal[0];
			val_p_tensor[loc+2][loc+2]=0.5*(val_velocity[iSpecies][1]*rhooc+val_normal[1]*val_density[iSpecies]);
			val_p_tensor[loc+2][loc+3]=0.5*(val_velocity[iSpecies][1]*rhooc-val_normal[1]*val_density[iSpecies]);

			val_p_tensor[loc+3][loc+0]=0.5*sqvel;
			val_p_tensor[loc+3][loc+1]=val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[1]-val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[0];
			val_p_tensor[loc+3][loc+2] = 0.5*(0.5*sqvel*rhooc+val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[0]+val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[1]+rhoxc/(GammaDiatomic-1.0));
			val_p_tensor[loc+3][loc+3] = 0.5*(0.5*sqvel*rhooc-val_density[iSpecies]*val_velocity[iSpecies][0]*val_normal[0]-val_density[iSpecies]*val_velocity[iSpecies][1]*val_normal[1]+rhoxc/(GammaDiatomic-1.0));

			if (iSpecies < nDiatomics) {
				val_p_tensor[loc+0][loc+4] = 1.0;
				val_p_tensor[loc+1][loc+4] = 1.0;
				val_p_tensor[loc+2][loc+4] = 1.0;
				val_p_tensor[loc+3][loc+4] = 1.0;
				val_p_tensor[loc+4][loc+4] = 1.0;

			}
		}
	}
}

void CNumerics::GetPMatrix_inv_AM(double *val_density, double **val_velocity,
		double *val_soundspeed, double *val_normal, double **val_invp_tensor, double *val_vibrational_energy) {

	double rhoxc, c2, gm1, k0orho, k1orho, gm1_o_c2, gm1_o_rhoxc, sqvel;
	unsigned short loc, iSpecies; // location along the matrix
	unsigned short iVar, jVar;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_invp_tensor[iVar][jVar] = 0.0;
		}
	}

	if (nDim == 3) {
		for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhoxc		= val_density[iSpecies] * val_soundspeed[iSpecies];
			c2			= val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			k0orho		= val_normal[0] / val_density[iSpecies];
			k1orho		= val_normal[1] / val_density[iSpecies];
			if (iSpecies < nDiatomics)
				gm1 = GammaDiatomic-1.0;
			else
				gm1 = GammaMonatomic-1.0;

			gm1_o_c2	= gm1/c2;
			gm1_o_rhoxc = gm1/rhoxc;
			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1]+val_velocity[iSpecies][2]*val_velocity[iSpecies][2];

			val_invp_tensor[loc+0][loc+0]=val_normal[0]-val_normal[2]*val_velocity[iSpecies][1] / val_density[iSpecies]+val_normal[1]*val_velocity[iSpecies][2] / val_density[iSpecies]-val_normal[0]*0.5*gm1*sqvel/c2;
			val_invp_tensor[loc+0][loc+1]=val_normal[0]*gm1*val_velocity[iSpecies][0]/c2;
			val_invp_tensor[loc+0][loc+2]=val_normal[2] / val_density[iSpecies]+val_normal[0]*gm1*val_velocity[iSpecies][1]/c2;
			val_invp_tensor[loc+0][loc+3]=-val_normal[1] / val_density[iSpecies]+val_normal[0]*gm1*val_velocity[iSpecies][2]/c2;
			val_invp_tensor[loc+0][loc+4]=-val_normal[0]*gm1/c2;

			val_invp_tensor[loc+1][loc+0]=val_normal[1]+val_normal[2]*val_velocity[iSpecies][0] / val_density[iSpecies]-val_normal[0]*val_velocity[iSpecies][2] / val_density[iSpecies]-val_normal[1]*0.5*gm1*sqvel/c2;
			val_invp_tensor[loc+1][loc+1]=-val_normal[2] / val_density[iSpecies]+val_normal[1]*gm1*val_velocity[iSpecies][0]/c2;
			val_invp_tensor[loc+1][loc+2]=val_normal[1]*gm1*val_velocity[iSpecies][1]/c2;
			val_invp_tensor[loc+1][loc+3]=val_normal[0] / val_density[iSpecies]+val_normal[1]*gm1*val_velocity[iSpecies][2]/c2;
			val_invp_tensor[loc+1][loc+4]=-val_normal[1]*gm1/c2;

			val_invp_tensor[loc+2][loc+0]=val_normal[2]-val_normal[1]*val_velocity[iSpecies][0] / val_density[iSpecies]+val_normal[0]*val_velocity[iSpecies][1] / val_density[iSpecies]-val_normal[2]*0.5*gm1*sqvel/c2;
			val_invp_tensor[loc+2][loc+1]=val_normal[1] / val_density[iSpecies]+val_normal[2]*gm1*val_velocity[iSpecies][0]/c2;
			val_invp_tensor[loc+2][loc+2]=-val_normal[0] / val_density[iSpecies]+val_normal[2]*gm1*val_velocity[iSpecies][1]/c2;
			val_invp_tensor[loc+2][loc+3]=val_normal[2]*gm1*val_velocity[iSpecies][2]/c2;
			val_invp_tensor[loc+2][loc+4]=-val_normal[2]*gm1/c2;

			val_invp_tensor[loc+3][loc+0]=-(val_normal[0]*val_velocity[iSpecies][0]+val_normal[1]*val_velocity[iSpecies][1]+val_normal[2]*val_velocity[iSpecies][2]) / val_density[iSpecies]+0.5*gm1*sqvel/rhoxc;
			val_invp_tensor[loc+3][loc+1]=val_normal[0] / val_density[iSpecies]-gm1*val_velocity[iSpecies][0]/rhoxc;
			val_invp_tensor[loc+3][loc+2]=val_normal[1] / val_density[iSpecies]-gm1*val_velocity[iSpecies][1]/rhoxc;
			val_invp_tensor[loc+3][loc+3]=val_normal[2] / val_density[iSpecies]-gm1*val_velocity[iSpecies][2]/rhoxc;
			val_invp_tensor[loc+3][loc+4]=gm1/rhoxc;

			val_invp_tensor[loc+4][loc+0]=(val_normal[0]*val_velocity[iSpecies][0]+val_normal[1]*val_velocity[iSpecies][1]+val_normal[2]*val_velocity[iSpecies][2]) / val_density[iSpecies]+0.5*gm1*sqvel/rhoxc;
			val_invp_tensor[loc+4][loc+1]=-val_normal[0] / val_density[iSpecies]-gm1*val_velocity[iSpecies][0]/rhoxc;
			val_invp_tensor[loc+4][loc+2]=-val_normal[1] / val_density[iSpecies]-gm1*val_velocity[iSpecies][1]/rhoxc;
			val_invp_tensor[loc+4][loc+3]=-val_normal[2] / val_density[iSpecies]-gm1*val_velocity[iSpecies][2]/rhoxc;
			val_invp_tensor[loc+4][loc+4]=gm1/rhoxc;

		}
	}

	if(nDim == 2) {

		for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhoxc		= val_density[iSpecies] * val_soundspeed[iSpecies];
			c2			= val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			k0orho		= val_normal[0] / val_density[iSpecies];
			k1orho		= val_normal[1] / val_density[iSpecies];
			if (iSpecies < nDiatomics)
				gm1 = GammaDiatomic-1.0;
			else
				gm1 = GammaMonatomic-1.0;

			gm1_o_c2	= gm1/c2;
			gm1_o_rhoxc = gm1/rhoxc;

			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1];

			val_invp_tensor[loc+0][loc+0]=1.0-0.5*gm1_o_c2*sqvel;
			val_invp_tensor[loc+0][loc+1]=gm1_o_c2*val_velocity[iSpecies][0];
			val_invp_tensor[loc+0][loc+2]=gm1_o_c2*val_velocity[iSpecies][1];
			val_invp_tensor[loc+0][loc+3]=-gm1_o_c2;

			val_invp_tensor[loc+1][loc+0]=-k1orho*val_velocity[iSpecies][0]+k0orho*val_velocity[iSpecies][1];
			val_invp_tensor[loc+1][loc+1]=k1orho;
			val_invp_tensor[loc+1][loc+2]=-k0orho;
			val_invp_tensor[loc+1][loc+3]=0.0;

			val_invp_tensor[loc+2][loc+0]=-k0orho*val_velocity[iSpecies][0]-k1orho*val_velocity[iSpecies][1]+0.5*gm1_o_rhoxc*sqvel;
			val_invp_tensor[loc+2][loc+1]=k0orho-gm1_o_rhoxc*val_velocity[iSpecies][0];
			val_invp_tensor[loc+2][loc+2]=k1orho-gm1_o_rhoxc*val_velocity[iSpecies][1];
			val_invp_tensor[loc+2][loc+3]=gm1_o_rhoxc;

			val_invp_tensor[loc+3][loc+0]=k0orho*val_velocity[iSpecies][0]+k1orho*val_velocity[iSpecies][1]+0.5*gm1_o_rhoxc*sqvel;
			val_invp_tensor[loc+3][loc+1]=-k0orho-gm1_o_rhoxc*val_velocity[iSpecies][0];
			val_invp_tensor[loc+3][loc+2]=-k1orho-gm1_o_rhoxc*val_velocity[iSpecies][1];
			val_invp_tensor[loc+3][loc+3]=gm1_o_rhoxc;

		}
	}
}

void CNumerics::GetPMatrix_inv(double *val_density, double *val_velocity,
		double *val_soundspeed, double *val_normal, double **val_invp_tensor) {
	double rhoxc, c2, gm1, k0orho, k1orho, gm1_o_c2, gm1_o_rhoxc, sqvel;

	rhoxc = *val_density * *val_soundspeed;
	c2 = *val_soundspeed * *val_soundspeed;
	gm1 = Gamma_Minus_One;
	k0orho = val_normal[0] / *val_density;
	k1orho = val_normal[1] / *val_density;
	gm1_o_c2 = gm1/c2;
	gm1_o_rhoxc = gm1/rhoxc;

	if (nDim == 3) {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];

		val_invp_tensor[0][0]=val_normal[0]-val_normal[2]*val_velocity[1] / *val_density+val_normal[1]*val_velocity[2] / *val_density-val_normal[0]*0.5*gm1*sqvel/c2;
		val_invp_tensor[0][1]=val_normal[0]*gm1*val_velocity[0]/c2;
		val_invp_tensor[0][2]=val_normal[2] / *val_density+val_normal[0]*gm1*val_velocity[1]/c2;
		val_invp_tensor[0][3]=-val_normal[1] / *val_density+val_normal[0]*gm1*val_velocity[2]/c2;
		val_invp_tensor[0][4]=-val_normal[0]*gm1/c2;

		val_invp_tensor[1][0]=val_normal[1]+val_normal[2]*val_velocity[0] / *val_density-val_normal[0]*val_velocity[2] / *val_density-val_normal[1]*0.5*gm1*sqvel/c2;
		val_invp_tensor[1][1]=-val_normal[2] / *val_density+val_normal[1]*gm1*val_velocity[0]/c2;
		val_invp_tensor[1][2]=val_normal[1]*gm1*val_velocity[1]/c2;
		val_invp_tensor[1][3]=val_normal[0] / *val_density+val_normal[1]*gm1*val_velocity[2]/c2;
		val_invp_tensor[1][4]=-val_normal[1]*gm1/c2;

		val_invp_tensor[2][0]=val_normal[2]-val_normal[1]*val_velocity[0] / *val_density+val_normal[0]*val_velocity[1] / *val_density-val_normal[2]*0.5*gm1*sqvel/c2;
		val_invp_tensor[2][1]=val_normal[1] / *val_density+val_normal[2]*gm1*val_velocity[0]/c2;
		val_invp_tensor[2][2]=-val_normal[0] / *val_density+val_normal[2]*gm1*val_velocity[1]/c2;
		val_invp_tensor[2][3]=val_normal[2]*gm1*val_velocity[2]/c2;
		val_invp_tensor[2][4]=-val_normal[2]*gm1/c2;

		val_invp_tensor[3][0]=-(val_normal[0]*val_velocity[0]+val_normal[1]*val_velocity[1]+val_normal[2]*val_velocity[2]) / *val_density+0.5*gm1*sqvel/rhoxc;
		val_invp_tensor[3][1]=val_normal[0] / *val_density-gm1*val_velocity[0]/rhoxc;
		val_invp_tensor[3][2]=val_normal[1] / *val_density-gm1*val_velocity[1]/rhoxc;
		val_invp_tensor[3][3]=val_normal[2] / *val_density-gm1*val_velocity[2]/rhoxc;
		val_invp_tensor[3][4]=Gamma_Minus_One/rhoxc;

		val_invp_tensor[4][0]=(val_normal[0]*val_velocity[0]+val_normal[1]*val_velocity[1]+val_normal[2]*val_velocity[2]) / *val_density+0.5*gm1*sqvel/rhoxc;
		val_invp_tensor[4][1]=-val_normal[0] / *val_density-gm1*val_velocity[0]/rhoxc;
		val_invp_tensor[4][2]=-val_normal[1] / *val_density-gm1*val_velocity[1]/rhoxc;
		val_invp_tensor[4][3]=-val_normal[2] / *val_density-gm1*val_velocity[2]/rhoxc;
		val_invp_tensor[4][4]=Gamma_Minus_One/rhoxc;
	}
	if(nDim == 2) {
		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];

		val_invp_tensor[0][0]=1.0-0.5*gm1_o_c2*sqvel;
		val_invp_tensor[0][1]=gm1_o_c2*val_velocity[0];
		val_invp_tensor[0][2]=gm1_o_c2*val_velocity[1];
		val_invp_tensor[0][3]=-gm1_o_c2;

		val_invp_tensor[1][0]=-k1orho*val_velocity[0]+k0orho*val_velocity[1];
		val_invp_tensor[1][1]=k1orho;
		val_invp_tensor[1][2]=-k0orho;
		val_invp_tensor[1][3]=0.0;

		val_invp_tensor[2][0]=-k0orho*val_velocity[0]-k1orho*val_velocity[1]+0.5*gm1_o_rhoxc*sqvel;
		val_invp_tensor[2][1]=k0orho-gm1_o_rhoxc*val_velocity[0];
		val_invp_tensor[2][2]=k1orho-gm1_o_rhoxc*val_velocity[1];
		val_invp_tensor[2][3]=gm1_o_rhoxc;

		val_invp_tensor[3][0]=k0orho*val_velocity[0]+k1orho*val_velocity[1]+0.5*gm1_o_rhoxc*sqvel;
		val_invp_tensor[3][1]=-k0orho-gm1_o_rhoxc*val_velocity[0];
		val_invp_tensor[3][2]=-k1orho-gm1_o_rhoxc*val_velocity[1];
		val_invp_tensor[3][3]=gm1_o_rhoxc;
	}
}


void CNumerics::GetPMatrix_inv(double *val_density, double *val_velocity, double *val_energy_ve, double *val_soundspeed, double *val_dPdrhos, double val_dPdrhoE, double val_dPdrhoEve, double *val_normal, double *l, double *m, double **val_invp_tensor) {

  unsigned short iSpecies, jSpecies, iDim, iVar, jVar;
  double rho, a2;
  double U, V, W;
  
  for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
      val_invp_tensor[iVar][jVar] = 0.0;
  
  /*--- Pre-compute useful quantities ---*/
  rho = 0.0;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++)
    rho += val_density[iSpecies];
  U = 0.0;  V = 0.0;  W = 0.0;
  for (iDim = 0; iDim < nDim; iDim++) {
    U += val_velocity[iDim] * val_normal[iDim];
    V += val_velocity[iDim] * l[iDim];
    W += val_velocity[iDim] * m[iDim];
  }
  a2 = (*val_soundspeed) * (*val_soundspeed);
  
	if (nDim == 3) {

    for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
      for (jSpecies = 0; jSpecies < nSpecies; jSpecies++) {
        val_invp_tensor[iSpecies][jSpecies] = -(val_density[iSpecies]/rho) * val_dPdrhos[jSpecies];
      }
      val_invp_tensor[iSpecies][iSpecies]  += a2;
      val_invp_tensor[iSpecies][nSpecies]   = val_dPdrhoE * val_velocity[0] * (val_density[iSpecies]/rho);
      val_invp_tensor[iSpecies][nSpecies+1] = val_dPdrhoE * val_velocity[1] * (val_density[iSpecies]/rho);
      val_invp_tensor[iSpecies][nSpecies+2] = val_dPdrhoE * val_velocity[2] * (val_density[iSpecies]/rho);
      val_invp_tensor[iSpecies][nSpecies+3] = -val_dPdrhoE * (val_density[iSpecies]/rho);
      val_invp_tensor[iSpecies][nSpecies+4] = -val_dPdrhoEve * (val_density[iSpecies]/rho);
      
      val_invp_tensor[nSpecies][iSpecies]   = -V;
      val_invp_tensor[nSpecies+1][iSpecies] = -W;
      val_invp_tensor[nSpecies+2][iSpecies] = val_dPdrhos[iSpecies] - U*(*val_soundspeed);
      val_invp_tensor[nSpecies+3][iSpecies] = val_dPdrhos[iSpecies] + U*(*val_soundspeed);
      val_invp_tensor[nSpecies+4][iSpecies] = -(*val_energy_ve) * val_dPdrhos[iSpecies];
    }
    
    val_invp_tensor[nSpecies][nSpecies]     = l[0];
    val_invp_tensor[nSpecies][nSpecies+1]   = l[1];
    val_invp_tensor[nSpecies][nSpecies+2]   = l[2];
    val_invp_tensor[nSpecies][nSpecies+3]   = 0.0;
    val_invp_tensor[nSpecies][nSpecies+4]   = 0.0;
    
    val_invp_tensor[nSpecies+1][nSpecies]   = m[0];
    val_invp_tensor[nSpecies+1][nSpecies+1] = m[1];
    val_invp_tensor[nSpecies+1][nSpecies+2] = m[2];
    val_invp_tensor[nSpecies+1][nSpecies+3] = 0.0;
    val_invp_tensor[nSpecies+1][nSpecies+4] = 0.0;
    
    val_invp_tensor[nSpecies+2][nSpecies]   = (*val_soundspeed)*val_normal[0] - val_dPdrhoE*val_velocity[0];
    val_invp_tensor[nSpecies+2][nSpecies+1] = (*val_soundspeed)*val_normal[1] - val_dPdrhoE*val_velocity[1];
    val_invp_tensor[nSpecies+2][nSpecies+2] = (*val_soundspeed)*val_normal[2] - val_dPdrhoE*val_velocity[2];
    val_invp_tensor[nSpecies+2][nSpecies+3] = val_dPdrhoE;
    val_invp_tensor[nSpecies+2][nSpecies+4] = val_dPdrhoEve;
    
    val_invp_tensor[nSpecies+3][nSpecies]   = -(*val_soundspeed)*val_normal[0] - val_dPdrhoE*val_velocity[0];
    val_invp_tensor[nSpecies+3][nSpecies+1] = -(*val_soundspeed)*val_normal[1] - val_dPdrhoE*val_velocity[1];
    val_invp_tensor[nSpecies+3][nSpecies+2] = -(*val_soundspeed)*val_normal[2] - val_dPdrhoE*val_velocity[2];
    val_invp_tensor[nSpecies+3][nSpecies+3] = val_dPdrhoE;
    val_invp_tensor[nSpecies+3][nSpecies+4] = val_dPdrhoEve;
    
    val_invp_tensor[nSpecies+4][nSpecies]   = val_dPdrhoE * val_velocity[0] * (*val_energy_ve);
    val_invp_tensor[nSpecies+4][nSpecies+1] = val_dPdrhoE * val_velocity[1] * (*val_energy_ve);
    val_invp_tensor[nSpecies+4][nSpecies+2] = val_dPdrhoE * val_velocity[2] * (*val_energy_ve);
    val_invp_tensor[nSpecies+4][nSpecies+3] = -val_dPdrhoE * (*val_energy_ve);
    val_invp_tensor[nSpecies+4][nSpecies+4] = a2 - val_dPdrhoEve*(*val_energy_ve);
    
  }
	if(nDim == 2) {
		cout << "InvP matrix not implemented for 2D flows!!!!" << endl;
#ifdef NO_MPI
    exit(1);
#else
    MPI::COMM_WORLD.Abort(1);
    MPI::Finalize();
#endif
	}
}


void CNumerics::GetPMatrix_inv(double *val_density, double **val_velocity,
		double *val_soundspeed, double *val_normal, double **val_invp_tensor) {

	double rhoxc, c2, gm1, k0orho, k1orho, gm1_o_c2, gm1_o_rhoxc, sqvel,u ,v,w, rho;
	double k0, k1 ,k2;
	unsigned short loc, iSpecies; // location along the matrix
	unsigned short iVar, jVar;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_invp_tensor[iVar][jVar] = 0.0;
		}
	}

	for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		loc			= (nDim+2)*iSpecies;

		Gamma = Vector_Gamma[iSpecies];
		Gamma_Minus_One = Gamma - 1.0;
		gm1 = Gamma_Minus_One;
		rhoxc		= val_density[iSpecies] * val_soundspeed[iSpecies];
		c2			= val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
		k0orho		= val_normal[0] / val_density[iSpecies];
		k1orho		= val_normal[1] / val_density[iSpecies];
		gm1_o_c2	= gm1/c2;
		gm1_o_rhoxc = gm1/rhoxc;
		rho 		= val_density[iSpecies];

		if (nDim == 3) {
			u = val_velocity[iSpecies][0];
			v = val_velocity[iSpecies][1];
			w = val_velocity[iSpecies][2];
			k0 = val_normal[0];
			k1 = val_normal[1];
			k2 = val_normal[2];
			sqvel 	= u*u + v*v + w*w;

			val_invp_tensor[loc+ 0][loc + 0] =  k0 - k2*v/rho + k1*w/rho - k0*0.5*gm1*sqvel/c2;
			val_invp_tensor[loc + 0][loc + 1] =  k0*gm1*u/c2;
			val_invp_tensor[loc + 0][loc + 2] =  k2/rho + k0*gm1*v/c2;
			val_invp_tensor[loc + 0][loc + 3] = -k1/rho + k0*gm1*w/c2;
			val_invp_tensor[loc + 0][loc + 4] = -k0*gm1/c2;

			val_invp_tensor[loc +1][loc + 0] =  k1+k2*u/rho - k0*w/rho - k1*0.5*gm1*sqvel/c2;
			val_invp_tensor[loc +1][loc + 1] = -k2/rho + k1*gm1*u/c2;
			val_invp_tensor[loc +1][loc + 2] =  k1*gm1*v/c2;
			val_invp_tensor[loc +1][loc + 3] =  k0/rho + k1*gm1*w/c2;
			val_invp_tensor[loc +1][loc + 4] = -k1*gm1/c2;

			val_invp_tensor[loc +2][loc + 0] =  k2 - k1*u/rho + k0*v/rho - k2*0.5*gm1*sqvel/c2;
			val_invp_tensor[loc +2][loc + 1] =  k1/rho + k2*gm1*u/c2;
			val_invp_tensor[loc +2][loc + 2] = -k0/rho + k2*gm1*v/c2;
			val_invp_tensor[loc +2][loc + 3] =  k2*gm1*w/c2;
			val_invp_tensor[loc +2][loc + 4] = -k2*gm1/c2;

			val_invp_tensor[loc + 3][loc + 0] = -(k0*u+k1*v+k2*w)/rho + 0.5*gm1*sqvel/rhoxc;
			val_invp_tensor[loc + 3][loc + 1] = k0/rho - gm1*u/rhoxc;
			val_invp_tensor[loc + 3][loc + 2] = k1/rho - gm1*v/rhoxc;
			val_invp_tensor[loc + 3][loc + 3] = k2/rho - gm1*w/rhoxc;
			val_invp_tensor[loc + 3][loc + 4] = gm1/rhoxc;

			val_invp_tensor[loc + 4][loc + 0] = (k0*u+k1*v+k2*w) /rho + 0.5*gm1*sqvel/rhoxc;
			val_invp_tensor[loc + 4][loc + 1] = -k0/rho - gm1*u/rhoxc;
			val_invp_tensor[loc + 4][loc + 2] = -k1/rho - gm1*v/rhoxc;
			val_invp_tensor[loc + 4][loc + 3] = -k2/rho - gm1*w/rhoxc;
			val_invp_tensor[loc + 4][loc + 4] =  gm1/rhoxc;
		}

		if(nDim == 2) {

			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1];

			val_invp_tensor[loc+0][loc+0]=1.0-0.5*gm1_o_c2*sqvel;
			val_invp_tensor[loc+0][loc+1]=gm1_o_c2*val_velocity[iSpecies][0];
			val_invp_tensor[loc+0][loc+2]=gm1_o_c2*val_velocity[iSpecies][1];
			val_invp_tensor[loc+0][loc+3]=-gm1_o_c2;

			val_invp_tensor[loc+1][loc+0]=-k1orho*val_velocity[iSpecies][0]+k0orho*val_velocity[iSpecies][1];
			val_invp_tensor[loc+1][loc+1]=k1orho;
			val_invp_tensor[loc+1][loc+2]=-k0orho;
			val_invp_tensor[loc+1][loc+3]=0.0;

			val_invp_tensor[loc+2][loc+0]=-k0orho*val_velocity[iSpecies][0]-k1orho*val_velocity[iSpecies][1]+0.5*gm1_o_rhoxc*sqvel;
			val_invp_tensor[loc+2][loc+1]=k0orho-gm1_o_rhoxc*val_velocity[iSpecies][0];
			val_invp_tensor[loc+2][loc+2]=k1orho-gm1_o_rhoxc*val_velocity[iSpecies][1];
			val_invp_tensor[loc+2][loc+3]=gm1_o_rhoxc;

			val_invp_tensor[loc+3][loc+0]=k0orho*val_velocity[iSpecies][0]+k1orho*val_velocity[iSpecies][1]+0.5*gm1_o_rhoxc*sqvel;
			val_invp_tensor[loc+3][loc+1]=-k0orho-gm1_o_rhoxc*val_velocity[iSpecies][0];
			val_invp_tensor[loc+3][loc+2]=-k1orho-gm1_o_rhoxc*val_velocity[iSpecies][1];
			val_invp_tensor[loc+3][loc+3]=gm1_o_rhoxc;
		}
	}
}

void CNumerics::GetPMatrix_inv_(double *val_density, double **val_velocity, double *val_soundspeed,
		double *val_energy_vib, double *val_energy_el, CConfig *config,
		double *val_normal, double **val_invp_tensor) {

	double rhoxc, c2, gm1, k0orho, k1orho, gm1_o_c2, gm1_o_rhoxc, sqvel, hf;
	unsigned short loc, iSpecies; // location along the matrix
	unsigned short iVar, jVar;

	for (iVar = 0; iVar < nVar; iVar ++) {
		for (jVar = 0; jVar < nVar; jVar ++) {
			val_invp_tensor[iVar][jVar] = 0.0;
		}
	}

	/*--- New implementation ---*/
	if (nDim == 3) {
		for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhoxc		= val_density[iSpecies] * val_soundspeed[iSpecies];
			c2			= val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			k0orho		= val_normal[0] / val_density[iSpecies];
			k1orho		= val_normal[1] / val_density[iSpecies];
			Gamma = config->GetSpecies_Gamma(iSpecies);
			gm1 = Gamma-1.0;
			hf = config->GetEnthalpy_Formation(iSpecies);

			gm1_o_c2	= gm1/c2;
			gm1_o_rhoxc = gm1/rhoxc;
			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1]+val_velocity[iSpecies][2]*val_velocity[iSpecies][2];

			val_invp_tensor[loc+0][loc+0]=val_normal[0]-val_normal[2]*val_velocity[iSpecies][1] / val_density[iSpecies]+val_normal[1]*val_velocity[iSpecies][2] / val_density[iSpecies]-val_normal[0]/c2*gm1*(0.5*sqvel-hf-val_energy_el[iSpecies]);
			val_invp_tensor[loc+0][loc+1]=val_normal[0]*gm1*val_velocity[iSpecies][0]/c2;
			val_invp_tensor[loc+0][loc+2]=val_normal[2] / val_density[iSpecies]+val_normal[0]*gm1*val_velocity[iSpecies][1]/c2;
			val_invp_tensor[loc+0][loc+3]=-val_normal[1] / val_density[iSpecies]+val_normal[0]*gm1*val_velocity[iSpecies][2]/c2;
			val_invp_tensor[loc+0][loc+4]=-val_normal[0]*gm1/c2;

			val_invp_tensor[loc+1][loc+0]=val_normal[1]+val_normal[2]*val_velocity[iSpecies][0] / val_density[iSpecies]-val_normal[0]*val_velocity[iSpecies][2] / val_density[iSpecies]-val_normal[1]/c2*gm1*(0.5*sqvel-hf-val_energy_el[iSpecies]);
			val_invp_tensor[loc+1][loc+1]=-val_normal[2] / val_density[iSpecies]+val_normal[1]*gm1*val_velocity[iSpecies][0]/c2;
			val_invp_tensor[loc+1][loc+2]=val_normal[1]*gm1*val_velocity[iSpecies][1]/c2;
			val_invp_tensor[loc+1][loc+3]=val_normal[0] / val_density[iSpecies]+val_normal[1]*gm1*val_velocity[iSpecies][2]/c2;
			val_invp_tensor[loc+1][loc+4]=-val_normal[1]*gm1/c2;

			val_invp_tensor[loc+2][loc+0]=val_normal[2]-val_normal[1]*val_velocity[iSpecies][0] / val_density[iSpecies]+val_normal[0]*val_velocity[iSpecies][1] / val_density[iSpecies]-val_normal[2]/c2*gm1*(0.5*sqvel-hf-val_energy_el[iSpecies]);
			val_invp_tensor[loc+2][loc+1]=val_normal[1] / val_density[iSpecies]+val_normal[2]*gm1*val_velocity[iSpecies][0]/c2;
			val_invp_tensor[loc+2][loc+2]=-val_normal[0] / val_density[iSpecies]+val_normal[2]*gm1*val_velocity[iSpecies][1]/c2;
			val_invp_tensor[loc+2][loc+3]=val_normal[2]*gm1*val_velocity[iSpecies][2]/c2;
			val_invp_tensor[loc+2][loc+4]=-val_normal[2]*gm1/c2;

			val_invp_tensor[loc+3][loc+0]=-(val_normal[0]*val_velocity[iSpecies][0]+val_normal[1]*val_velocity[iSpecies][1]+val_normal[2]*val_velocity[iSpecies][2]) / val_density[iSpecies]+(1.0/rhoxc)*gm1*(0.5*sqvel-hf-val_energy_el[iSpecies]);
			val_invp_tensor[loc+3][loc+1]=val_normal[0] / val_density[iSpecies]-gm1*val_velocity[iSpecies][0]/rhoxc;
			val_invp_tensor[loc+3][loc+2]=val_normal[1] / val_density[iSpecies]-gm1*val_velocity[iSpecies][1]/rhoxc;
			val_invp_tensor[loc+3][loc+3]=val_normal[2] / val_density[iSpecies]-gm1*val_velocity[iSpecies][2]/rhoxc;
			val_invp_tensor[loc+3][loc+4]=(Gamma-1.0)/rhoxc;

			val_invp_tensor[loc+4][loc+0]=(val_normal[0]*val_velocity[iSpecies][0]+val_normal[1]*val_velocity[iSpecies][1]+val_normal[2]*val_velocity[iSpecies][2]) / val_density[iSpecies]+(1.0/rhoxc)*gm1*(0.5*sqvel-hf-val_energy_el[iSpecies]);
			val_invp_tensor[loc+4][loc+1]=-val_normal[0] / val_density[iSpecies]-gm1*val_velocity[iSpecies][0]/rhoxc;
			val_invp_tensor[loc+4][loc+2]=-val_normal[1] / val_density[iSpecies]-gm1*val_velocity[iSpecies][1]/rhoxc;
			val_invp_tensor[loc+4][loc+3]=-val_normal[2] / val_density[iSpecies]-gm1*val_velocity[iSpecies][2]/rhoxc;
			val_invp_tensor[loc+4][loc+4]=(Gamma-1.0)/rhoxc;

			if (iSpecies < nDiatomics) {
				//Last row
				val_invp_tensor[loc+5][loc+0] = -val_energy_vib[iSpecies]/val_density[iSpecies];
				val_invp_tensor[loc+5][loc+1] = 0.0;
				val_invp_tensor[loc+5][loc+2] = 0.0;
				val_invp_tensor[loc+5][loc+3] = 0.0;
				val_invp_tensor[loc+5][loc+4] = 0.0;
				val_invp_tensor[loc+5][loc+5] = 1/val_density[iSpecies];

				//Last column				
				val_invp_tensor[loc+0][loc+5] = val_normal[0]*gm1_o_c2;
				val_invp_tensor[loc+1][loc+5] = val_normal[1]*gm1_o_c2;
				val_invp_tensor[loc+2][loc+5] = val_normal[2]*gm1_o_c2;
				val_invp_tensor[loc+3][loc+5] = -gm1/rhoxc;
				val_invp_tensor[loc+4][loc+5] = -gm1/rhoxc;
			}
		}
	}

	if(nDim == 2) {

		for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
			if ( iSpecies < nDiatomics ) loc = (nDim+3)*iSpecies;
			else loc = (nDim+3)*nDiatomics + (nDim+2)*(iSpecies-nDiatomics);

			rhoxc		= val_density[iSpecies] * val_soundspeed[iSpecies];
			c2			= val_soundspeed[iSpecies] * val_soundspeed[iSpecies];
			k0orho		= val_normal[0] / val_density[iSpecies];
			k1orho		= val_normal[1] / val_density[iSpecies];
			Gamma = config->GetSpecies_Gamma(iSpecies);
			gm1 = Gamma-1.0;			
			gm1_o_c2	= gm1/c2;
			gm1_o_rhoxc = gm1/rhoxc;
			hf = config->GetEnthalpy_Formation(iSpecies);

			sqvel = val_velocity[iSpecies][0]*val_velocity[iSpecies][0]+val_velocity[iSpecies][1]*val_velocity[iSpecies][1];

			val_invp_tensor[loc+0][loc+0]=1.0-gm1_o_c2*(0.5*sqvel-config->GetEnthalpy_Formation(iSpecies)-val_energy_el[iSpecies]);
			val_invp_tensor[loc+0][loc+1]=gm1_o_c2*val_velocity[iSpecies][0];
			val_invp_tensor[loc+0][loc+2]=gm1_o_c2*val_velocity[iSpecies][1];
			val_invp_tensor[loc+0][loc+3]=-gm1_o_c2;

			val_invp_tensor[loc+1][loc+0]=-k1orho*val_velocity[iSpecies][0]+k0orho*val_velocity[iSpecies][1];
			val_invp_tensor[loc+1][loc+1]=k1orho;
			val_invp_tensor[loc+1][loc+2]=-k0orho;
			val_invp_tensor[loc+1][loc+3]=0.0;

			val_invp_tensor[loc+2][loc+0]=-k0orho*val_velocity[iSpecies][0]-k1orho*val_velocity[iSpecies][1]+gm1_o_rhoxc*(0.5*sqvel-config->GetEnthalpy_Formation(iSpecies)-val_energy_el[iSpecies]);
			val_invp_tensor[loc+2][loc+1]=k0orho-gm1_o_rhoxc*val_velocity[iSpecies][0];
			val_invp_tensor[loc+2][loc+2]=k1orho-gm1_o_rhoxc*val_velocity[iSpecies][1];
			val_invp_tensor[loc+2][loc+3]=gm1_o_rhoxc;

			val_invp_tensor[loc+3][loc+0]=k0orho*val_velocity[iSpecies][0]+k1orho*val_velocity[iSpecies][1]+gm1_o_rhoxc*(0.5*sqvel-config->GetEnthalpy_Formation(iSpecies)-val_energy_el[iSpecies]);
			val_invp_tensor[loc+3][loc+1]=-k0orho-gm1_o_rhoxc*val_velocity[iSpecies][0];
			val_invp_tensor[loc+3][loc+2]=-k1orho-gm1_o_rhoxc*val_velocity[iSpecies][1];
			val_invp_tensor[loc+3][loc+3]=gm1_o_rhoxc;

			if (iSpecies < nDiatomics) {
				//Last row
				val_invp_tensor[loc+4][loc+0] = -val_energy_vib[iSpecies]/val_density[iSpecies];
				val_invp_tensor[loc+4][loc+1] = 0.0;
				val_invp_tensor[loc+4][loc+2] = 0.0;
				val_invp_tensor[loc+4][loc+3] = 0.0;
				val_invp_tensor[loc+4][loc+4] = 1/val_density[iSpecies];

				//Last column				
				val_invp_tensor[loc+0][loc+4] = gm1_o_c2;
				val_invp_tensor[loc+1][loc+4] = 0.0;
				val_invp_tensor[loc+2][loc+4] = -gm1/rhoxc;
				val_invp_tensor[loc+3][loc+4] = -gm1/rhoxc;


			}
		}
	}
}

void CNumerics::GetinvRinvPe(double Beta2, double val_enthalpy, double val_soundspeed, double val_density, double* val_velocity, double **invRinvPe) {

	double sqvel;
	double factor = 1.0/(val_soundspeed*val_soundspeed*Beta2);

	if(nDim == 2) {

		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];

		invRinvPe[0][0] = factor;
		invRinvPe[0][1] = 0.0;
		invRinvPe[0][2] = 0.0;
		invRinvPe[0][3] = -val_density/Gamma;

		invRinvPe[1][0] = val_velocity[0]*factor;
		invRinvPe[1][1] = val_density;
		invRinvPe[1][2] = 0.0;
		invRinvPe[1][3] = -val_density*val_velocity[0]/Gamma;

		invRinvPe[2][0] = val_velocity[1]*factor;
		invRinvPe[2][1] = 0;
		invRinvPe[2][2] = val_density;
		invRinvPe[2][3] = -val_density*val_velocity[1]/Gamma;

		invRinvPe[3][0] = val_enthalpy*factor;
		invRinvPe[3][1] = val_density*val_velocity[0];
		invRinvPe[3][2] = val_density*val_velocity[1];
		invRinvPe[3][3] = -val_density*sqvel/(2.0*Gamma);
	}
	else {

		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];

		invRinvPe[0][0] =  factor;
		invRinvPe[0][1] = 0.0;
		invRinvPe[0][2] = 0.0;
		invRinvPe[0][3] = 0.0;
		invRinvPe[0][4] = -val_density/Gamma;

		invRinvPe[1][0] = val_velocity[0]*factor;
		invRinvPe[1][1] = val_density;
		invRinvPe[1][2] = 0.0;
		invRinvPe[1][3] = 0.0;
		invRinvPe[1][4] = -val_density*val_velocity[0]/Gamma;

		invRinvPe[2][0] = val_velocity[1]*factor;
		invRinvPe[2][1] = 0;
		invRinvPe[2][2] = val_density;
		invRinvPe[2][3] = 0.0;
		invRinvPe[2][4] = -val_density*val_velocity[1]/Gamma;


		invRinvPe[3][0] = val_velocity[2]*factor;
		invRinvPe[3][1] = 0;
		invRinvPe[3][2] = 0;
		invRinvPe[3][3] = val_density;
		invRinvPe[3][4] = -val_density*val_velocity[2]/Gamma;

		invRinvPe[4][0] = val_enthalpy*factor;
		invRinvPe[4][1] = val_density*val_velocity[0];
		invRinvPe[4][2] = val_density*val_velocity[1];
		invRinvPe[4][3] = val_density*val_velocity[2];
		invRinvPe[4][4] = -val_density*sqvel/(2.0*Gamma);

	}

}

void CNumerics::GetRMatrix(double val_pressure, double val_soundspeed, double val_density, double* val_velocity, double **R_Matrix) {

	double sqvel;
	//double factor = 1.0/(val_soundspeed*val_soundspeed*1);
	double gm1 = Gamma - 1.0;

	if(nDim == 2) {

		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1];

		R_Matrix[0][0] =  0.5*gm1*sqvel;
		R_Matrix[0][1] = -val_velocity[0]*gm1;
		R_Matrix[0][2] = -val_velocity[1]*gm1;
		R_Matrix[0][3] = gm1;

		R_Matrix[1][0] = -val_velocity[0]/val_density;
		R_Matrix[1][1] = 1.0/val_density;
		R_Matrix[1][2] = 0.0;
		R_Matrix[1][3] = 0.0;

		R_Matrix[2][0] = -val_velocity[1]/val_density;
		R_Matrix[2][1] = 0.0;
		R_Matrix[2][2] = 1.0/val_density;
		R_Matrix[2][3] = 0.0;

		R_Matrix[3][0] = 0.5*gm1*sqvel/val_pressure - Gamma/val_density;
		R_Matrix[3][1] = -gm1*val_velocity[0]/val_pressure;
		R_Matrix[3][2] = -gm1*val_velocity[1]/val_pressure;
		R_Matrix[3][3] = gm1/val_pressure;
	}
	else {

		sqvel = val_velocity[0]*val_velocity[0]+val_velocity[1]*val_velocity[1]+val_velocity[2]*val_velocity[2];

		R_Matrix[0][0] =  0.5*gm1*sqvel;
		R_Matrix[0][1] = -val_velocity[0]*gm1;
		R_Matrix[0][2] = -val_velocity[1]*gm1;
		R_Matrix[0][3] = -val_velocity[2]*gm1;
		R_Matrix[0][4] = gm1;

		R_Matrix[1][0] = -val_velocity[0]/val_density;
		R_Matrix[1][1] = 1.0/val_density;
		R_Matrix[1][2] = 0.0;
		R_Matrix[1][3] = 0.0;
		R_Matrix[1][4] = 0.0;

		R_Matrix[2][0] = -val_velocity[1]/val_density;
		R_Matrix[2][1] = 0.0;
		R_Matrix[2][2] = 1.0/val_density;
		R_Matrix[2][3] = 0.0;
		R_Matrix[2][4] = 0.0;

		R_Matrix[3][0] = -val_velocity[2]/val_density;
		R_Matrix[3][1] = 0.0;
		R_Matrix[3][2] = 0.0;
		R_Matrix[3][3] = 1.0/val_density;
		R_Matrix[3][4] = 0.0;

		R_Matrix[4][0] = 0.5*gm1*sqvel/val_pressure - Gamma/val_density;
		R_Matrix[4][1] = -gm1*val_velocity[0]/val_pressure;
		R_Matrix[4][2] = -gm1*val_velocity[1]/val_pressure;
		R_Matrix[4][3] = -gm1*val_velocity[2]/val_pressure;
		R_Matrix[4][4] = gm1/val_pressure;

	}

}


void CNumerics::GetPrecondJacobian(double Beta2, double r_hat, double s_hat, double t_hat, double rB2a2, double* Lambda, double *val_normal,
		double **val_absPeJac) {

	double lam1, lam2, lam3, lam4;
	lam1 = Lambda[0]; lam2 = Lambda[1]; lam3 = Lambda[2]; lam4 = Lambda[3];

	if(nDim == 2) {

		val_absPeJac[0][0] =  lam3*s_hat/(2.0*t_hat) - lam4*r_hat/(2.0*t_hat);
		val_absPeJac[0][1] = -lam3*rB2a2*val_normal[0]/(2.0*t_hat) + lam4*rB2a2*val_normal[0]/(2.0*t_hat);
		val_absPeJac[0][2] = -lam3*rB2a2*val_normal[1]/(2.0*t_hat) + lam4*rB2a2*val_normal[1]/(2.0*t_hat);
		val_absPeJac[0][3] =  0.0;

		val_absPeJac[1][0] = r_hat*val_normal[0]*lam3*s_hat/(2.0*t_hat*rB2a2) + s_hat*val_normal[0]*lam4*(-r_hat)/(2.0*t_hat*rB2a2);
		val_absPeJac[1][1] = lam2*(val_normal[1]*val_normal[1]) - lam3*r_hat*val_normal[0]*val_normal[0]/(2.0*t_hat) + lam4*s_hat*val_normal[0]*val_normal[0]/(2.0*t_hat);
		val_absPeJac[1][2] = -lam2*val_normal[0]*val_normal[1] - lam3*r_hat*val_normal[0]*val_normal[1]/(2.0*t_hat) + lam4*s_hat*val_normal[0]*val_normal[1]/(2.0*t_hat);
		val_absPeJac[1][3] = 0.0;

		val_absPeJac[2][0] = lam3*r_hat*val_normal[1]*s_hat/(2.0*t_hat*rB2a2) - s_hat*val_normal[1]*lam4*r_hat/(2.0*t_hat*rB2a2);
		val_absPeJac[2][1] = -val_normal[0]*val_normal[1]*lam2 - r_hat*val_normal[1]*val_normal[0]*lam3/(2.0*t_hat) + s_hat*val_normal[0]*val_normal[1]*lam4/(2.0*t_hat);
		val_absPeJac[2][2] = val_normal[0]*val_normal[0]*lam2 -r_hat*val_normal[1]*val_normal[1]*lam3/(2.0*t_hat) + s_hat*val_normal[1]*val_normal[1]*lam4/(2.0*t_hat);
		val_absPeJac[2][3] = 0.0;

		val_absPeJac[3][0] = 0.0;
		val_absPeJac[3][1] = 0.0;
		val_absPeJac[3][2] = 0.0;
		val_absPeJac[3][3] = lam1;

	}
	else {

		double lam5 = Lambda[4];

		val_absPeJac[0][0] =  lam4*s_hat/(2.0*t_hat) - lam5*r_hat/(2.0*t_hat);
		val_absPeJac[0][1] = -lam4*rB2a2*val_normal[0]/(2.0*t_hat) + lam5*rB2a2*val_normal[0]/(2.0*t_hat);
		val_absPeJac[0][2] = -lam4*rB2a2*val_normal[1]/(2.0*t_hat) + lam5*rB2a2*val_normal[1]/(2.0*t_hat);
		val_absPeJac[0][3] = -lam4*rB2a2*val_normal[2]/(2.0*t_hat) + lam5*rB2a2*val_normal[2]/(2.0*t_hat);
		val_absPeJac[0][4] =  0.0;

		val_absPeJac[1][0] = r_hat*val_normal[0]*lam4*s_hat/(2.0*t_hat*rB2a2) + s_hat*val_normal[0]*lam5*(-r_hat)/(2.0*t_hat*rB2a2);
		val_absPeJac[1][1] = lam2*(val_normal[2]*val_normal[2] + val_normal[1]*val_normal[1]) - lam4*r_hat*val_normal[0]*val_normal[0]/(2.0*t_hat) + lam5*s_hat*val_normal[0]*val_normal[0]/(2.0*t_hat);
		val_absPeJac[1][2] = -lam2*val_normal[0]*val_normal[1] - lam4*r_hat*val_normal[0]*val_normal[1]/(2.0*t_hat) + lam5*s_hat*val_normal[0]*val_normal[1]/(2.0*t_hat);
		val_absPeJac[1][3] = -lam2*val_normal[0]*val_normal[2] - lam4*r_hat*val_normal[0]*val_normal[2]/(2.0*t_hat) + lam5*s_hat*val_normal[0]*val_normal[2]/(2.0*t_hat);
		val_absPeJac[1][4] = 0.0;

		val_absPeJac[2][0] = lam4*r_hat*val_normal[1]*s_hat/(2.0*t_hat*rB2a2) - s_hat*val_normal[1]*lam5*r_hat/(2.0*t_hat*rB2a2);
		val_absPeJac[2][1] = -val_normal[0]*val_normal[1]*lam2 - r_hat*val_normal[1]*val_normal[0]*lam4/(2.0*t_hat) + s_hat*val_normal[0]*val_normal[1]*lam5/(2.0*t_hat);
		val_absPeJac[2][2] = val_normal[0]*val_normal[0]*lam2 + val_normal[2]*val_normal[2]*lam3 -r_hat*val_normal[1]*val_normal[1]*lam4/(2.0*t_hat) + s_hat*val_normal[1]*val_normal[1]*lam5/(2.0*t_hat);
		val_absPeJac[2][3] = -val_normal[2]*val_normal[1]*lam2 - r_hat*val_normal[2]*val_normal[1]*lam4/(2.0*t_hat) + s_hat*lam5*val_normal[1]*val_normal[2]/(2.0*t_hat);
		val_absPeJac[2][4] = 0.0;

		val_absPeJac[3][0] = r_hat*s_hat*val_normal[2]*lam4/(2.0*t_hat*rB2a2) - r_hat*s_hat*val_normal[2]*lam5/(2.0*t_hat*rB2a2);
		val_absPeJac[3][1] = -val_normal[0]*val_normal[2]*lam3 - lam4*val_normal[0]*val_normal[2]*r_hat/(2.0*t_hat) + lam5*val_normal[0]*val_normal[2]*s_hat/(2.0*t_hat);
		val_absPeJac[3][2] = -val_normal[1]*val_normal[2]*lam3 - lam4*val_normal[1]*val_normal[2]*r_hat/(2.0*t_hat) + lam5*val_normal[1]*val_normal[2]*s_hat/(2.0*t_hat);
		val_absPeJac[3][3] = (val_normal[1]*val_normal[1] + val_normal[0]*val_normal[0])*lam3 - lam4*val_normal[2]*val_normal[2]*r_hat/(2.0*t_hat) + lam5*val_normal[2]*val_normal[2]*s_hat/(2.0*t_hat);
		val_absPeJac[3][4] = 0.0;

		val_absPeJac[4][0] = 0.0;
		val_absPeJac[4][1] = 0.0;
		val_absPeJac[4][2] = 0.0;
		val_absPeJac[4][3] = 0.0;
		val_absPeJac[4][4] = lam1;

	}

}

void CNumerics::GetPArtCompMatrix(double *val_density, double *val_velocity, double *val_betainc2,
		double *val_normal, double **val_p_tensor) {
	double a, a2, Projvel, area2, sx, sy, sz = 0.0, u, v, w = 0.0, factor = 0.0;

	sx = val_normal[0]; sy = val_normal[1]; u = val_velocity[0]; v = val_velocity[1];
    if (nDim == 3) { sz = val_normal[2]; w = val_velocity[2]; }
	Projvel = u*sx + v*sy; area2 = sx*sx + sy*sy;
    if (nDim == 3) { Projvel += w*sz; area2 += sz*sz; }
	a2 = Projvel*Projvel + ((*val_betainc2)/(*val_density))*area2; a = sqrt(a2);
	factor = 1/(2.0*((*val_betainc2)/(*val_density))*a2);

    if(nDim == 2) {
		val_p_tensor[0][0] = 0.0;
		val_p_tensor[0][1] = factor*((*val_betainc2)/(*val_density))*a;
		val_p_tensor[0][2] = -factor*((*val_betainc2)/(*val_density))*a;
        
		val_p_tensor[1][0] = -factor*2.0*sy*((*val_betainc2)/(*val_density));
		val_p_tensor[1][1] = factor*(u*(a+Projvel) + sx*((*val_betainc2)/(*val_density)));
		val_p_tensor[1][2] = factor*(u*(Projvel-a) + sx*((*val_betainc2)/(*val_density)));
        
		val_p_tensor[2][0] = factor*2.0*sx*((*val_betainc2)/(*val_density));
		val_p_tensor[2][1] = factor*(v*(a+Projvel) + sy*((*val_betainc2)/(*val_density)));
		val_p_tensor[2][2] = factor*(v*(Projvel-a) + sy*((*val_betainc2)/(*val_density)));
	}
	else {
		val_p_tensor[0][0] = 0.0;
		val_p_tensor[0][1] = 0.0;
		val_p_tensor[0][2] = ((*val_betainc2)/(*val_density))*a;
		val_p_tensor[0][3] = -((*val_betainc2)/(*val_density))*a;

		val_p_tensor[1][0] = -sz;
		val_p_tensor[1][1] = -sy;
		val_p_tensor[1][2] = u*(Projvel+a) + sx*((*val_betainc2)/(*val_density));
		val_p_tensor[1][3] = u*(Projvel-a) + sx*((*val_betainc2)/(*val_density));

		val_p_tensor[2][0] = 0.0;
		val_p_tensor[2][1] = sx;
		val_p_tensor[2][2] = v*(Projvel+a) + sy*((*val_betainc2)/(*val_density));
		val_p_tensor[2][3] = v*(Projvel-a) + sy*((*val_betainc2)/(*val_density));

		val_p_tensor[3][0] = sx;
		val_p_tensor[3][1] = 0.0;
		val_p_tensor[3][2] = w*(Projvel+a) + sz*((*val_betainc2)/(*val_density));
		val_p_tensor[3][3] = w*(Projvel-a) + sz*((*val_betainc2)/(*val_density));
	}

}

void CNumerics::GetPArtCompMatrix_inv(double *val_density, double *val_velocity, double *val_betainc2,
		double *val_normal, double **val_invp_tensor) {
	double a, a2, Projvel, area2, sx, sy, sz = 0.0, u, v, w = 0.0;

	sx = val_normal[0]; sy = val_normal[1]; u = val_velocity[0]; v = val_velocity[1];
    if (nDim == 3) { sz = val_normal[2]; w = val_velocity[2];}
	Projvel = u*sx + v*sy; area2 = sx*sx + sy*sy;
    if (nDim == 3) { Projvel += w*sz; area2 += sz*sz; }
	a2 = Projvel*Projvel + ((*val_betainc2)/(*val_density))*area2; a = sqrt(a2);

    if (nDim == 2) {
		val_invp_tensor[0][0] = (sy*u-sx*v);
		val_invp_tensor[0][1] = -v*Projvel-sy*((*val_betainc2)/(*val_density));
		val_invp_tensor[0][2] = u*Projvel+sx*((*val_betainc2)/(*val_density));
        
		val_invp_tensor[1][0] = (a-Projvel);
		val_invp_tensor[1][1] = ((*val_betainc2)/(*val_density))*sx;
		val_invp_tensor[1][2] = ((*val_betainc2)/(*val_density))*sy;
        
		val_invp_tensor[2][0] = (-a-Projvel);
		val_invp_tensor[2][1] = ((*val_betainc2)/(*val_density))*sx;
		val_invp_tensor[2][2] = ((*val_betainc2)/(*val_density))*sy;
	}
	else {
		val_invp_tensor[0][0] = (sz*Projvel-area2*w)/(sx*a2);
		val_invp_tensor[0][1] = -(w*Projvel+sz*((*val_betainc2)/(*val_density)))/a2;
		val_invp_tensor[0][2] = -sy*(w*Projvel+sz*((*val_betainc2)/(*val_density)))/(sx*a2);
		val_invp_tensor[0][3] = ((sx*u+sy*v)*Projvel+(sx*sx+sy*sy)*((*val_betainc2)/(*val_density)))/(sx*a2);

		val_invp_tensor[1][0] = (sy*Projvel-area2*v)/(sx*a2);
		val_invp_tensor[1][1] = -(v*Projvel+sy*((*val_betainc2)/(*val_density)))/a2;
		val_invp_tensor[1][2] = ((sx*u+sz*w)*Projvel+(sx*sx+sz*sz)*((*val_betainc2)/(*val_density)))/(sx*a2);
		val_invp_tensor[1][3] = -sz*(v*Projvel+sy*((*val_betainc2)/(*val_density)))/(sx*a2);

		val_invp_tensor[2][0] = -(Projvel-a)/(2.0*a2*((*val_betainc2)/(*val_density)));
		val_invp_tensor[2][1] = sx/(2.0*a2);
		val_invp_tensor[2][2] = sy/(2.0*a2);
		val_invp_tensor[2][3] = sz/(2.0*a2);

		val_invp_tensor[3][0] = -(Projvel+a)/(2.0*a2*((*val_betainc2)/(*val_density)));
		val_invp_tensor[3][1] = sx/(2.0*a2);
		val_invp_tensor[3][2] = sy/(2.0*a2);
		val_invp_tensor[3][3] = sz/(2.0*a2);
	}

}

void CNumerics::GetPArtComp_FreeSurf_Matrix(double *val_density, double *val_ddensity, double *val_velocity, double *val_betainc2, double *val_levelset, double *val_normal, double **val_p_tensor) {
  
	double a = 0.0, b = 0.0, c = 0.0, d = 0.0, area2 = 0.0, e2 = 0.0, f = 0.0, nx = 0.0, ny = 0.0, nz = 0.0, u = 0.0, v = 0.0, w = 0.0;
  
  a = (*val_betainc2)/(*val_density);
  b = (*val_levelset)/(*val_density);
  c = (*val_ddensity);
  
  if (nDim == 2) {
    
    nx = val_normal[0];   ny = val_normal[1];   area2 = nx*nx + ny*ny;
    u = val_velocity[0];  v = val_velocity[1];  d = u*nx + v*ny;
    e2 = (2.0*d - b*c*d)*(2.0*d - b*c*d);
    f = sqrt(4.0*a*area2 + e2);
    
    val_p_tensor[0][0] = 0;
    val_p_tensor[0][1] = 0;
    val_p_tensor[0][2] = (d*d*(1.0 - b*c) + 2.0*a*area2 + d*d + d*f)/(2.0*b*area2);
    val_p_tensor[0][3] = (d*d*(1.0 - b*c) + 2.0*a*area2 + d*d - d*f)/(2.0*b*area2);
    
    val_p_tensor[1][0] = (c*d)/nx;
    val_p_tensor[1][1] = -(ny/nx);
    val_p_tensor[1][2] = (d*nx*(b*c - 1.0) + nx*nx*u + 2.0*ny*ny*u - nx*ny*v - nx*f)/(2*b*area2);
    val_p_tensor[1][3] = (d*nx*(b*c - 1.0) + nx*nx*u + 2.0*ny*ny*u - nx*ny*v + nx*f)/(2*b*area2);
    
    val_p_tensor[2][0] = 0.0;
    val_p_tensor[2][1] = 1.0;
    val_p_tensor[2][2] = (d*ny*(b*c - 1.0) - nx*ny*u + 2.0*nx*nx*v + ny*ny*v - ny*f)/(2*b*area2);
    val_p_tensor[2][3] = (d*ny*(b*c - 1.0) - nx*ny*u + 2.0*nx*nx*v + ny*ny*v + ny*f)/(2*b*area2);
    
    val_p_tensor[3][0] = 1.0;
    val_p_tensor[3][1] = 0.0;
    val_p_tensor[3][2] = 1.0;
    val_p_tensor[3][3] = 1.0;
    
	}
	else {
  
    nx = val_normal[0];   ny = val_normal[1];   nz = val_normal[2];   area2 = nx*nx + ny*ny + nz*nz;
    u = val_velocity[0];  v = val_velocity[1];  w = val_velocity[2];  d = u*nx + v*ny + w*nz;
    e2 = (2.0*d - b*c*d)*(2.0*d - b*c*d);
    f = sqrt(4.0*a*area2 + e2);
  
    val_p_tensor[0][0] = 0.0;
    val_p_tensor[0][1] = 0.0;
    val_p_tensor[0][2] = 0.0;
    val_p_tensor[0][3] = -((a*(b*c*d + f))/(b*(2.0*d - b*c*d - f)));
    val_p_tensor[0][4] = (a*(- b*c*d + f))/(b*(2.0*d - b*c*d + f));
      
    val_p_tensor[1][0] = (c*d)/nx;
    val_p_tensor[1][1] = -(nz/nx);
    val_p_tensor[1][2] = -(ny/nx);
    val_p_tensor[1][3] = -((-2.0*a*nx + b*c*d*u - 2.0*u*d + u*f)/(b*(2.0*d - b*c*d - f)));
    val_p_tensor[1][4] = -((-2.0*a*nx + b*c*d*u - 2.0*u*d - u*f)/(b*(2.0*d - b*c*d + f)));
      
    val_p_tensor[2][0] = 0.0;
    val_p_tensor[2][1] = 0.0;
    val_p_tensor[2][2] = 1.0;
    val_p_tensor[2][3] = -((-2.0*a*ny + b*c*d*v - 2.0*v*d + v*f)/(b*(2.0*d - b*c*d - f)));
    val_p_tensor[2][4] = -((-2.0*a*ny + b*c*d*v - 2.0*v*d - v*f)/(b*(2.0*d - b*c*d + f)));
      
    val_p_tensor[3][0] = 0.0;
    val_p_tensor[3][1] = 1.0;
    val_p_tensor[3][2] = 0.0;
    val_p_tensor[3][3] = -((-2.0*a*nz + b*c*d*w - 2.0*w*d + w*f)/(b*(2.0*d - b*c*d - f)));
    val_p_tensor[3][4] = -((-2.0*a*nz + b*c*d*w - 2.0*w*d - w*f)/(b*(2.0*d - b*c*d + f)));

    val_p_tensor[4][0] = 1.0;
    val_p_tensor[4][1] = 0.0;
    val_p_tensor[4][2] = 0.0;
    val_p_tensor[4][3] = 1.0;
    val_p_tensor[4][4] = 1.0;
    
  }
  
}

void CNumerics::GetPArtComp_FreeSurf_Matrix_inv(double *val_density, double *val_ddensity, double *val_velocity, double *val_betainc2, double *val_levelset,
                                      double *val_normal, double **val_invp_tensor) {
  
	double a = 0.0, b = 0.0, c = 0.0, d = 0.0, area2 = 0.0, e2 = 0.0, f = 0.0, nx = 0.0, ny = 0.0, nz = 0.0, u = 0.0, v = 0.0, w = 0.0;
  
  a = (*val_betainc2)/(*val_density);
  b = (*val_levelset)/(*val_density);
  c = (*val_ddensity);
  
  if (nDim == 2) {
    
    nx = val_normal[0];   ny = val_normal[1];   area2 = nx*nx + ny*ny;
    u = val_velocity[0];  v = val_velocity[1];  d = u*nx + v*ny;
    e2 = (2.0*d - b*c*d)*(2.0*d - b*c*d);
    f = sqrt(4.0*a*area2 + e2);

    val_invp_tensor[0][0] = -((b*area2)/(a*area2 + d*d*(1.0 - b*c)));
    val_invp_tensor[0][1] = -((b*d*nx)/(a*area2 + d*d*(1.0 - b*c)));
    val_invp_tensor[0][2] = -((b*d*ny)/(a*area2 + d*d*(1.0 - b*c)));
    val_invp_tensor[0][3] = ( a*area2 + d*d)/(a*area2 + d*d*(1.0 - b*c));
    
    val_invp_tensor[1][0] = (-b*c*d*ny + nx*(ny*u - nx*v))/(a*area2 + d*d*(1.0-b*c));
    val_invp_tensor[1][1] = -((nx*(a*ny + d*v))/(a*area2 + d*d*(1.0-b*c)));
    val_invp_tensor[1][2] = (-b*c*d*d + nx*(a*nx + d*u))/(a*area2 + d*d*(1.0-b*c));
    val_invp_tensor[1][3] = (c*d*(a*ny + d*v))/(a*area2 + d*d*(1.0-b*c));
    
    val_invp_tensor[2][0] = (b*area2*(-b*c*d + f))/(2.0*(-b*c*d*d + a*area2 + d*d)*f);
    val_invp_tensor[2][1] = -((b*nx*(-(2.0 - b*c)*d*d - 2.0*a*area2 + d*f))/(2.0*(- a*area2 - d*d*(1.0-b*c))*f));
    val_invp_tensor[2][2] = -((b*ny*(-(2.0 - b*c)*d*d - 2.0*a*area2 + d*f))/(2.0*(- a*area2 - d*d*(1.0-b*c))*f));
    val_invp_tensor[2][3] = (b*c*d*(-(2.0 - b*c)*d*d - 2.0*a*area2 + d*f))/(2.0*(- a*area2 - d*d*(1.0-b*c))*f);
    
    val_invp_tensor[3][0] = (b*area2*(b*c*d + f))/(2.0*(-b*c*d*d + a*area2 + d*d)*f);
    val_invp_tensor[3][1] = -((b*nx*((2.0 - b*c)*d*d + 2.0*a*area2 + d*f))/(2.0*(- a*area2 - d*d*(1.0-b*c))*f));
    val_invp_tensor[3][2] = -((b*ny*((2.0 - b*c)*d*d + 2.0*a*area2 + d*f))/(2.0*(- a*area2 - d*d*(1.0-b*c))*f));
    val_invp_tensor[3][3] = (b*c*d*((2.0 - b*c)*d*d + 2.0*a*area2 + d*f))/(2.0*(- a*area2 - d*d*(1.0-b*c))*f);
    
	}
	else {
  
    nx = val_normal[0];   ny = val_normal[1];   nz = val_normal[2];   area2 = nx*nx + ny*ny + nz*nz;
    u = val_velocity[0];  v = val_velocity[1];  w = val_velocity[2];  d = u*nx + v*ny + w*nz;
    e2 = (2.0*d - b*c*d)*(2.0*d - b*c*d);
    f = sqrt(4.0*a*area2 + e2);
    
    val_invp_tensor[0][0] = (b*area2)/(b*c*d*d - a*area2 - d*d);
    val_invp_tensor[0][1] = -((b*d*nx)/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[0][2] = -((b*d*ny)/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[0][3] = -((b*d*nz)/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[0][4] = (a*area2 + d*d)/(-b*c*d*d +a*area2 + d*d);
    
    val_invp_tensor[1][0] = (-b*c*d*nz +nx*nz*u - nx*nx*w + ny*(nz*v - ny*w))/(-b*c*d*d +a*area2 + d*d);
    val_invp_tensor[1][1] = -((nx*(a*nz + d*w))/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[1][2] = -((ny*(a*nz + d*w))/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[1][3] = (-b*c*d*d + a*(nx*nx + ny*ny) + d*(nx*u + ny*v))/(-b*c*d*d + a*area2 + d*d);
    val_invp_tensor[1][4] = (c*d*(a*nz + d*w))/(-b*c*d*d + a*area2 + d*d);
        
    val_invp_tensor[2][0] = (-b*c*d*ny + nx*ny*u - nx*nx*v + nz*(-nz*v + ny*w))/(-b*c*d*d + a*area2 + d*d);
    val_invp_tensor[2][1] =  -((nx*(a*ny + d*v))/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[2][2] = (-b*c*d*d + a*(nx*nx + nz*nz) + d*(nx*u + nz*w))/(-b*c*d*d + a*area2 + d*d);
    val_invp_tensor[2][3] = -((nz*(a*ny + d*v))/(-b*c*d*d + a*area2 + d*d));
    val_invp_tensor[2][4] = (c*d*(a*ny + d*v))/(-b*c*d*d + a*area2 + d*(nx*u + ny*v + nz*w));
    
    val_invp_tensor[3][0] = -(b*(-d + b*c*d - d + f)*(b*b*c*c*d*d + 2.0*a*area2 + d*d - 3.0*b*c*d*d + 2.0*nx*ny*u*v + 2.0*nx*nz*u*w + 2.0*ny*nz*v*w + nx*nx*u*u + ny*ny*v*v + nz*nz*w*w - b*c*d*f + d*f))/(4*a*(b*c*d*d - a*area2 - d*d)*f);
    val_invp_tensor[3][1] = (b*nx*(-d + b*c*d - d + f)*(- b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f);
    val_invp_tensor[3][2] = (b*ny*(-d + b*c*d - d + f)*(- b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f);
    val_invp_tensor[3][3] = (b*nz*(-d + b*c*d - d + f)*(- b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f);
    val_invp_tensor[3][4] = -((b*c*d*(-d + b*c*d - d + f)*(- b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f));
        
    val_invp_tensor[4][0] = -(b*(2.0*d - b*c*d + f)*(b*b*c*c*d*d + 2.0*a*area2 + d*d - 3.0*b*c*d*d + 2.0*nx*ny*u*v + 2.0*nx*nz*u*w + 2.0*ny*nz*v*w + nx*nx*u*u + ny*ny*v*v + nz*nz*w*w + b*c*d*f - d*f))/(4*a*(b*c*d*d - a*area2 - d*d)*f);
    val_invp_tensor[4][1] = -((b*nx*(b*c*d + f)*(2.0*d - b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f));
    val_invp_tensor[4][2] = -((b*ny*(b*c*d + f)*(2.0*d - b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f));
    val_invp_tensor[4][3] = -((b*nz*(b*c*d + f)*(2.0*d - b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f));
    val_invp_tensor[4][4] = (b*c*d*(b*c*d + f)*(2.0*d - b*c*d + f))/(4.0*(b*c*d*d - a*area2 - d*d)*f);

  }
  
}

void CNumerics::GetJacInviscidLambda_fabs(double *val_velocity, double val_soundspeed,
		double *val_normal, double *val_Lambda_Vector) {
	double ProjVelocity = 0;

	for (unsigned short iDim = 0; iDim < nDim; iDim++)
		ProjVelocity += val_velocity[iDim]*val_normal[iDim];

	if (nDim == 3) {
		val_Lambda_Vector[0] = fabs(ProjVelocity);
		val_Lambda_Vector[1] = fabs(ProjVelocity);
		val_Lambda_Vector[2] = fabs(ProjVelocity);
		val_Lambda_Vector[3] = fabs(ProjVelocity + val_soundspeed);
		val_Lambda_Vector[4] = fabs(ProjVelocity - val_soundspeed);
	}

	if(nDim == 2) {
		val_Lambda_Vector[0] = fabs(ProjVelocity);
		val_Lambda_Vector[1] = fabs(ProjVelocity);
		val_Lambda_Vector[2] = fabs(ProjVelocity + val_soundspeed);
		val_Lambda_Vector[3] = fabs(ProjVelocity - val_soundspeed);
	}
}

void CNumerics::ConsVar2PrimVar_MultiSpecies(double *val_consvar, double *val_primvar) {
	double Density, sq_vel = 0;
	unsigned short iDim, loc, iSpecies;

	for (iSpecies = 0; iSpecies < nSpecies; iSpecies ++) {
		loc = (nDim+2)*iSpecies;
		Density = val_consvar[loc + 0];
		sq_vel = 0.0;
		Gamma = Vector_Gamma[iSpecies];
		Gamma_Minus_One = Gamma - 1.0;
		for (iDim = 0; iDim < nDim; iDim++) {
			val_primvar[loc + iDim+1] = val_consvar[loc + iDim+1]/Density;
			sq_vel += val_primvar[loc + iDim+1]*val_primvar[loc + iDim+1];
		}
		val_primvar[loc + nDim + 1] = Gamma_Minus_One*(val_consvar[loc + nDim+1]-0.5*sq_vel*val_consvar[loc + 0]);
		val_primvar[loc + 0] = val_primvar[loc + nDim + 1] / (Gas_Constant_MultipleSpecies[iSpecies]*Density);
	}
}

void CNumerics::GetViscousFlux(double *val_primvar, double **val_gradprimvar,
		double val_laminar_viscosity, double val_eddy_viscosity, double val_mach_inf) {

	double total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	double cp = (Gamma / Gamma_Minus_One) * Gas_Constant;
	double heat_flux_factor = cp * (val_laminar_viscosity/PRANDTL + val_eddy_viscosity/PRANDTL_TURB);

	double div_vel = 0.0;
	for (unsigned short iDim = 0 ; iDim < nDim; iDim++)
		div_vel += val_gradprimvar[iDim+1][iDim];

	for (unsigned short iDim = 0 ; iDim < nDim; iDim++) {
		for (unsigned short jDim = 0 ; jDim < nDim; jDim++) {
			tau[iDim][jDim] = total_viscosity*( val_gradprimvar[jDim+1][iDim] +
                                          val_gradprimvar[iDim+1][jDim] )
                       -TWO3*total_viscosity*div_vel*delta[iDim][jDim];
		}
	}

	// Gradient of primitive variables -> [Temp vel_x vel_y vel_z Pressure]
	if (nDim == 3) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][2];
		Flux_Tensor[4][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2] + tau[0][2]*val_primvar[3] +
				heat_flux_factor*val_gradprimvar[0][0];

		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][2];
		Flux_Tensor[4][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2] + tau[1][2]*val_primvar[3] +
				heat_flux_factor*val_gradprimvar[0][1];

		Flux_Tensor[0][2] = 0.0;
		Flux_Tensor[1][2] = tau[2][0];
		Flux_Tensor[2][2] = tau[2][1];
		Flux_Tensor[3][2] = tau[2][2];
		Flux_Tensor[4][2] = tau[2][0]*val_primvar[1] + tau[2][1]*val_primvar[2] + tau[2][2]*val_primvar[3] +
				heat_flux_factor*val_gradprimvar[0][2];
	}
	if (nDim == 2) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2]+
				heat_flux_factor*val_gradprimvar[0][0];

		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2]+
				heat_flux_factor*val_gradprimvar[0][1];
	}
}

void CNumerics::GetViscousProjFlux(double *val_primvar,
                                   double **val_gradprimvar, double val_turb_ke,
                                   double *val_normal,
                                   double val_laminar_viscosity,
                                   double val_eddy_viscosity) {

	unsigned short iVar, iDim, jDim;
	double total_viscosity, heat_flux_factor, div_vel, cp, Density;
	Density = val_primvar[nDim+2];

	total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	cp = (Gamma / Gamma_Minus_One) * Gas_Constant;
	heat_flux_factor = cp * (val_laminar_viscosity/PRANDTL + val_eddy_viscosity/PRANDTL_TURB);

	div_vel = 0.0;
	for (iDim = 0 ; iDim < nDim; iDim++)
		div_vel += val_gradprimvar[iDim+1][iDim];

	for (iDim = 0 ; iDim < nDim; iDim++)
		for (jDim = 0 ; jDim < nDim; jDim++)
			tau[iDim][jDim] = total_viscosity*( val_gradprimvar[jDim+1][iDim] + val_gradprimvar[iDim+1][jDim] )
			- TWO3*total_viscosity*div_vel*delta[iDim][jDim]
			                                           - TWO3*Density*val_turb_ke*delta[iDim][jDim];


	/*--- Gradient of primitive variables -> [Temp vel_x vel_y vel_z Pressure] ---*/
	if (nDim == 2) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2]+
				heat_flux_factor*val_gradprimvar[0][0];

		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2]+
				heat_flux_factor*val_gradprimvar[0][1];
	} else {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = tau[0][0];
		Flux_Tensor[2][0] = tau[0][1];
		Flux_Tensor[3][0] = tau[0][2];
		Flux_Tensor[4][0] = tau[0][0]*val_primvar[1] + tau[0][1]*val_primvar[2] + tau[0][2]*val_primvar[3] +
				heat_flux_factor*val_gradprimvar[0][0];

		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = tau[1][0];
		Flux_Tensor[2][1] = tau[1][1];
		Flux_Tensor[3][1] = tau[1][2];
		Flux_Tensor[4][1] = tau[1][0]*val_primvar[1] + tau[1][1]*val_primvar[2] + tau[1][2]*val_primvar[3] +
				heat_flux_factor*val_gradprimvar[0][1];

		Flux_Tensor[0][2] = 0.0;
		Flux_Tensor[1][2] = tau[2][0];
		Flux_Tensor[2][2] = tau[2][1];
		Flux_Tensor[3][2] = tau[2][2];
		Flux_Tensor[4][2] = tau[2][0]*val_primvar[1] + tau[2][1]*val_primvar[2] + tau[2][2]*val_primvar[3] +
				heat_flux_factor*val_gradprimvar[0][2];
	}

	for (iVar = 0; iVar < nVar; iVar++) {
		Proj_Flux_Tensor[iVar] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
			Proj_Flux_Tensor[iVar] += Flux_Tensor[iVar][iDim] * val_normal[iDim];
	}

}

void CNumerics::GetViscousProjFlux(double *val_primvar,
                                   double **val_gradprimvar, double *val_normal,
                                   double* val_laminar_viscosity,
                                   double* val_eddy_viscosity,
                                   unsigned short val_iSpecies) {
	unsigned short iVar, iDim, jDim, nVar_species;
	double total_viscosity, heat_flux_factor, heat_flux_factor_vib, div_vel, cp;

	Gamma = Vector_Gamma[val_iSpecies];
	Gamma_Minus_One = Gamma - 1.0;
	total_viscosity = val_laminar_viscosity[val_iSpecies] + val_eddy_viscosity[val_iSpecies];
	cp = (Gamma / Gamma_Minus_One) * Gas_Constant_MultipleSpecies[val_iSpecies];
	heat_flux_factor = cp * (val_laminar_viscosity[val_iSpecies]/PRANDTL + val_eddy_viscosity[val_iSpecies]/PRANDTL_TURB);
	div_vel = 0.0;
	for (iDim = 0 ; iDim < nDim; iDim++)
		div_vel += val_gradprimvar[iDim+1][iDim];

	for (iDim = 0 ; iDim < nDim; iDim++)
		for (jDim = 0 ; jDim < nDim; jDim++)
			tau[iDim][jDim] = total_viscosity*( val_gradprimvar[jDim+1][iDim] + val_gradprimvar[iDim+1][jDim]) -
			TWO3*total_viscosity*div_vel*delta[iDim][jDim];

	/*--- Populate entries in the viscous flux vector ---*/
	for (iDim = 0; iDim < nDim; iDim++) {
		Flux_Tensor[0][iDim] = 0.0;
		Flux_Tensor[nDim+1][iDim] =0.0;
		for (jDim = 0; jDim < nDim; jDim++) {
			Flux_Tensor[jDim+1][iDim] = tau[iDim][jDim];
			Flux_Tensor[nDim+1][iDim] += tau[iDim][jDim]*val_primvar[jDim+1];
		}
		Flux_Tensor[nDim+1][iDim] += heat_flux_factor*val_gradprimvar[0][iDim];

		//NOTE: THIS IS NOT PROPERLY IMPLEMENTED.  MUST REVISIT TO MAKE GENERAL FOR MOLECULES W/ INTERNAL STRUCTURE!!
		if (val_iSpecies < nDiatomics) {
			heat_flux_factor_vib = heat_flux_factor;
			Flux_Tensor[nDim+1][iDim] += heat_flux_factor_vib*val_gradprimvar[nDim+1][iDim];
			Flux_Tensor[nDim+2][iDim]  = heat_flux_factor_vib*val_gradprimvar[nDim+1][iDim];
		}
	}

	/*--- Project the viscous flux vector in the normal direction ---*/
	if (val_iSpecies < nDiatomics) nVar_species = nDim+3;
	else nVar_species = nDim+2;
	for (iVar = 0; iVar < nVar_species; iVar++) {
		Proj_Flux_Tensor[iVar] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
			Proj_Flux_Tensor[iVar] += Flux_Tensor[iVar][iDim]*val_normal[iDim];
	}
}

void CNumerics::GetViscousProjFlux(double *val_primvar,
                                   double **val_gradprimvar,
                                   double *val_normal,
                                   double *val_laminar_viscosity,
                                   double *val_eddy_viscosity,
                                   double *val_therm_conductivity,
                                   double *val_therm_conductivity_vib,
                                   unsigned short val_iSpecies) {
  
	unsigned short iVar, iDim, jDim, nVar_species;
	double total_viscosity, div_vel;

	total_viscosity = val_laminar_viscosity[val_iSpecies] + val_eddy_viscosity[val_iSpecies];

	div_vel = 0.0;
	for (iDim = 0 ; iDim < nDim; iDim++)
		div_vel += val_gradprimvar[iDim+1][iDim];

	for (iDim = 0 ; iDim < nDim; iDim++)
		for (jDim = 0 ; jDim < nDim; jDim++)
			tau[iDim][jDim] = total_viscosity*( val_gradprimvar[jDim+1][iDim] + val_gradprimvar[iDim+1][jDim]) -
		                  	TWO3*total_viscosity*div_vel*delta[iDim][jDim];

	/*--- Populate entries in the viscous flux vector ---*/
	for (iDim = 0; iDim < nDim; iDim++) {
		Flux_Tensor[0][iDim] = 0.0;
		Flux_Tensor[nDim+1][iDim] = 0.0;
		for (jDim = 0; jDim < nDim; jDim++) {
			Flux_Tensor[jDim+1][iDim]  = tau[iDim][jDim];
			Flux_Tensor[nDim+1][iDim] += tau[iDim][jDim]*val_primvar[jDim+1];
		}
		Flux_Tensor[nDim+1][iDim] += val_therm_conductivity[val_iSpecies]*val_gradprimvar[0][iDim];

		if (val_iSpecies < nDiatomics) {
			Flux_Tensor[nDim+1][iDim] += val_therm_conductivity_vib[val_iSpecies]*val_gradprimvar[nDim+1][iDim];
			Flux_Tensor[nDim+2][iDim]  = val_therm_conductivity_vib[val_iSpecies]*val_gradprimvar[nDim+1][iDim];
		}
	}

	/*--- Project the viscous flux vector in the normal direction ---*/
	if (val_iSpecies < nDiatomics) nVar_species = nDim+3;
	else nVar_species = nDim+2;
	for (iVar = 0; iVar < nVar_species; iVar++) {
		Proj_Flux_Tensor[iVar] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
			Proj_Flux_Tensor[iVar] += Flux_Tensor[iVar][iDim]*val_normal[iDim];
	}
}

void CNumerics::GetViscousProjFlux(double *val_primvar,
                                   double **val_gradprimvar,
                                   double *val_normal,
                                   double *val_diffusioncoeff,
                                   double val_viscosity,
                                   double val_therm_conductivity,
                                   double val_therm_conductivity_ve,
                                   CConfig *config) {
  
  bool ionization;
	unsigned short iSpecies, iVar, iDim, jDim, nHeavy, iEl, nEl;
  unsigned short *nElStates;
	double *Ds, mu, ktr, kve, div_vel;
  double *xi, *Ms, *thetav, **thetae, **g, *Tref, *hf, Ru;
  double Ys, rho, T, Tve, Ev, Ee, eve, hs, num, denom;
  
  /*--- Initialize ---*/
  for (iVar = 0; iVar < nVar; iVar++) {
    Proj_Flux_Tensor[iVar] = 0.0;
    for (iDim = 0; iDim < nDim; iDim++)
      Flux_Tensor[iVar][iDim] = 0.0;
  }
  
  /*--- Read from CConfig ---*/
  ionization = config->GetIonization();
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }
  
  /*--- Rename for convenience ---*/
  Ds  = val_diffusioncoeff;
  mu  = val_viscosity;
  ktr = val_therm_conductivity;
  kve = val_therm_conductivity_ve;
  rho = val_primvar[RHO_INDEX];
  T   = val_primvar[T_INDEX];
  Tve = val_primvar[TVE_INDEX];
  Ru  = UNIVERSAL_GAS_CONSTANT;
  
  /*--- Load variables from the config class --*/
  xi        = config->GetRotationModes();      // Rotational modes of energy storage
  Ms        = config->GetMolar_Mass();         // Species molar mass
  thetav    = config->GetCharVibTemp();        // Species characteristic vib. temperature [K]
  thetae    = config->GetCharElTemp();         // Characteristic electron temperature [K]
  g         = config->GetElDegeneracy();       // Degeneracy of electron states
  nElStates = config->GetnElStates();          // Number of electron states
  Tref      = config->GetRefTemperature();     // Thermodynamic reference temperature [K]
  hf        = config->GetEnthalpy_Formation(); // Formation enthalpy [J/kg]
  
  /*--- Calculate the velocity divergence ---*/
	div_vel = 0.0;
	for (iDim = 0 ; iDim < nDim; iDim++)
		div_vel += val_gradprimvar[VEL_INDEX+iDim][iDim];
  
  /*--- Pre-compute mixture quantities ---*/
  for (iDim = 0; iDim < nDim; iDim++) {
    Vector[iDim] = 0.0;
    for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
      Vector[iDim] += rho*Ds[iSpecies]*val_gradprimvar[RHOS_INDEX+iSpecies][iDim];
    }
  }
  
  /*--- Compute the viscous stress tensor ---*/
	for (iDim = 0 ; iDim < nDim; iDim++)
		for (jDim = 0 ; jDim < nDim; jDim++)
			tau[iDim][jDim] = mu * (val_gradprimvar[VEL_INDEX+jDim][iDim] +
                              val_gradprimvar[VEL_INDEX+iDim][jDim])
                       -TWO3*mu*div_vel*delta[iDim][jDim];
  
	/*--- Populate entries in the viscous flux vector ---*/
	for (iDim = 0; iDim < nDim; iDim++) {
    /*--- Species diffusion velocity ---*/
    for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
      Ys = val_primvar[RHOS_INDEX+iSpecies]/rho;
      Flux_Tensor[iSpecies][iDim] = -rho*Ds[iSpecies]*val_gradprimvar[RHOS_INDEX+iSpecies][iDim]
                                  + Ys*Vector[iDim];
    }
    if (ionization) {
      cout << "GetViscProjFlux -- NEED TO IMPLEMENT IONIZED FUNCTIONALITY!!!" << endl;
      exit(1);
    }
    /*--- Shear stress related terms ---*/
    Flux_Tensor[nSpecies+nDim][iDim] = 0.0;
		for (jDim = 0; jDim < nDim; jDim++) {
			Flux_Tensor[nSpecies+jDim][iDim]  = tau[iDim][jDim];
			Flux_Tensor[nSpecies+nDim][iDim] += tau[iDim][jDim]*val_primvar[VEL_INDEX+jDim];
		}
    
    /*--- Diffusion terms ---*/
    for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
      if (thetav[iSpecies] != 0.0)
        Ev = Ru/Ms[iSpecies] * thetav[iSpecies] / (exp(thetav[iSpecies]/Tve)-1.0);
      else
        Ev = 0.0;
      num = 0.0;
      denom = g[iSpecies][0] * exp(thetae[iSpecies][0]/Tve);
      for (iEl = 1; iEl < nElStates[iSpecies]; iEl++) {
        num   += g[iSpecies][iEl] * thetae[iSpecies][iEl] * exp(-thetae[iSpecies][iEl]/Tve);
        denom += g[iSpecies][iEl] * exp(-thetae[iSpecies][iEl]/Tve);
      }
      Ee = Ru/Ms[iSpecies] * (num/denom);
      eve = Ev + Ee;
      
      hs = Ru/Ms[iSpecies]*T + (3.0/2.0+xi[iSpecies]/2.0)*Ru/Ms[iSpecies] + hf[iSpecies] + eve;
      
      Flux_Tensor[nSpecies+nDim][iDim]   += Flux_Tensor[iSpecies][iDim] * hs;
      Flux_Tensor[nSpecies+nDim+1][iDim] += Flux_Tensor[iSpecies][iDim] * eve;
    }
    
    /*--- Heat transfer terms ---*/
		Flux_Tensor[nSpecies+nDim][iDim]   += ktr*val_gradprimvar[T_INDEX][iDim] +
                                          kve*val_gradprimvar[TVE_INDEX][iDim];
    Flux_Tensor[nSpecies+nDim+1][iDim] += kve*val_gradprimvar[TVE_INDEX][iDim];
	}
  
  for (iVar = 0; iVar < nVar; iVar++) {
    for (iDim = 0; iDim < nDim; iDim++) {
      Proj_Flux_Tensor[iVar] += Flux_Tensor[iVar][iDim]*val_normal[iDim];
    }
  }
}


void CNumerics::GetViscousArtCompProjFlux(double *val_primvar, double **val_gradprimvar, double *val_normal, double val_laminar_viscosity,
		double val_eddy_viscosity) {
	unsigned short iVar, iDim;
	double total_viscosity;
	
	total_viscosity = (val_laminar_viscosity + val_eddy_viscosity);

	if (nDim == 3) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = total_viscosity * val_gradprimvar[1][0];
		Flux_Tensor[2][0] = total_viscosity * val_gradprimvar[2][0];
		Flux_Tensor[3][0] = total_viscosity * val_gradprimvar[3][0];

		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = total_viscosity * val_gradprimvar[1][1];
		Flux_Tensor[2][1] = total_viscosity * val_gradprimvar[2][1];
		Flux_Tensor[3][1] = total_viscosity * val_gradprimvar[3][1];

		Flux_Tensor[0][2] = 0.0;
		Flux_Tensor[1][2] = total_viscosity * val_gradprimvar[1][2];
		Flux_Tensor[2][2] = total_viscosity * val_gradprimvar[2][2];
		Flux_Tensor[3][2] = total_viscosity * val_gradprimvar[3][2];
	}

	if (nDim == 2) {
		Flux_Tensor[0][0] = 0.0;
		Flux_Tensor[1][0] = total_viscosity * val_gradprimvar[1][0];
		Flux_Tensor[2][0] = total_viscosity * val_gradprimvar[2][0];

		Flux_Tensor[0][1] = 0.0;
		Flux_Tensor[1][1] = total_viscosity * val_gradprimvar[1][1];
		Flux_Tensor[2][1] = total_viscosity * val_gradprimvar[2][1];
	}

	for (iVar = 0; iVar < nVar; iVar++) {
		Proj_Flux_Tensor[iVar] = 0.0;
		for (iDim = 0; iDim < nDim; iDim++)
			Proj_Flux_Tensor[iVar] += Flux_Tensor[iVar][iDim] * val_normal[iDim];
	}
}

void CNumerics::GetViscousProjJacs(double *val_Mean_PrimVar, double val_laminar_viscosity,
		double val_eddy_viscosity, double val_dist_ij, double *val_normal, double val_dS,
		double *val_Proj_Visc_Flux, double **val_Proj_Jac_Tensor_i, double **val_Proj_Jac_Tensor_j) {
	unsigned short iDim, iVar, jVar;

	double theta = 0.0, sqvel = 0.0, proj_viscousflux_vel = 0.0;
  
	for (iDim = 0; iDim < nDim; iDim++) {
		theta += val_normal[iDim]*val_normal[iDim];
		sqvel += val_Mean_PrimVar[iDim+1]*val_Mean_PrimVar[iDim+1];
		proj_viscousflux_vel += val_Proj_Visc_Flux[iDim+1]*val_Mean_PrimVar[iDim+1];
	}
  
	double phi = 0.5*(Gamma-1.0)*sqvel;
	double Density = val_Mean_PrimVar[nDim+2];
	double Pressure = val_Mean_PrimVar[nDim+1];
	double total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	double heat_flux_factor = val_laminar_viscosity / PRANDTL + val_eddy_viscosity / PRANDTL_TURB;
	double cpoR = Gamma/(Gamma-1.0); // cp over R
	double factor = total_viscosity*val_dS/(Density*val_dist_ij);
	double phi_rho = -cpoR*heat_flux_factor*Pressure/(Density*Density);
	double phi_p = cpoR*heat_flux_factor/(Density);
	double rhoovisc = Density/(total_viscosity); // rho over viscosity

  for (unsigned short iVar = 0; iVar < nVar; iVar++) {
    for (unsigned short jVar = 0; jVar < nVar; jVar++) {
      val_Proj_Jac_Tensor_i[iVar][jVar] = 0.0;
      val_Proj_Jac_Tensor_j[iVar][jVar] = 0.0;
    }
  }
  
	if (nDim == 2) {
    
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;

		double etaz = val_normal[0]*val_normal[1]/3.0;

		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay;

		val_Proj_Jac_Tensor_i[0][0] = 0.0;
		val_Proj_Jac_Tensor_i[0][1] = 0.0;
		val_Proj_Jac_Tensor_i[0][2] = 0.0;
		val_Proj_Jac_Tensor_i[0][3] = 0.0;
		val_Proj_Jac_Tensor_i[1][0] = factor*pix;
		val_Proj_Jac_Tensor_i[1][1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[1][2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[1][3] = 0.0;
		val_Proj_Jac_Tensor_i[2][0] = factor*piy;
		val_Proj_Jac_Tensor_i[2][1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[2][3] = 0.0;

		val_Proj_Jac_Tensor_i[3][0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) - (pix*val_Mean_PrimVar[1]+piy*val_Mean_PrimVar[2]));
		val_Proj_Jac_Tensor_i[3][1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1.0)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[3][2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1.0)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[3][3] = -factor*((Gamma-1.0)*rhoovisc*theta*phi_p);
    
		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];

		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[3][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[3][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[3][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[3][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[3][2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[3][2] += factor*val_Proj_Visc_Flux[2];
    
    
	} 
	else {

		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
		double thetaz = theta + val_normal[2]*val_normal[2]/3.0;

		double etax = val_normal[1]*val_normal[2]/3.0;
		double etay = val_normal[0]*val_normal[2]/3.0;
		double etaz = val_normal[0]*val_normal[1]/3.0;

		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz   + val_Mean_PrimVar[3]*etay;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay + val_Mean_PrimVar[3]*etax;
		double piz = val_Mean_PrimVar[1]*etay   + val_Mean_PrimVar[2]*etax   + val_Mean_PrimVar[3]*thetaz;

		val_Proj_Jac_Tensor_i[0][0] = 0.0;
		val_Proj_Jac_Tensor_i[0][1] = 0.0;
		val_Proj_Jac_Tensor_i[0][2] = 0.0;
		val_Proj_Jac_Tensor_i[0][3] = 0.0;
		val_Proj_Jac_Tensor_i[0][4] = 0.0;
		val_Proj_Jac_Tensor_i[1][0] = factor*pix;
		val_Proj_Jac_Tensor_i[1][1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[1][2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[1][3] = -factor*etay;
		val_Proj_Jac_Tensor_i[1][4] = 0.0;
		val_Proj_Jac_Tensor_i[2][0] = factor*piy;
		val_Proj_Jac_Tensor_i[2][1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[2][3] = -factor*etax;
		val_Proj_Jac_Tensor_i[2][4] = 0.0;
		val_Proj_Jac_Tensor_i[3][0] = factor*piz;
		val_Proj_Jac_Tensor_i[3][1] = -factor*etay;
		val_Proj_Jac_Tensor_i[3][2] = -factor*etax;
		val_Proj_Jac_Tensor_i[3][3] = -factor*thetaz;
		val_Proj_Jac_Tensor_i[3][4] = 0.0;
		val_Proj_Jac_Tensor_i[4][0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) - (pix*val_Mean_PrimVar[1] + piy*val_Mean_PrimVar[2] + piz*val_Mean_PrimVar[3]));
		val_Proj_Jac_Tensor_i[4][1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[4][2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[4][3] = -factor*(piz-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[3]);
		val_Proj_Jac_Tensor_i[4][4] = -factor*((Gamma-1)*rhoovisc*theta*phi_p);

		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];
    
		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[4][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[4][0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[4][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[4][1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[4][2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[4][2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_i[4][3] += factor*val_Proj_Visc_Flux[3];
		val_Proj_Jac_Tensor_j[4][3] += factor*val_Proj_Visc_Flux[3];

	}

}

void CNumerics::GetViscousProjJacs(double *val_Mean_PrimVar,
                                   double *val_diffusion_coeff,
                                   double val_laminar_viscosity,
                                   double val_thermal_conductivity,
                                   double val_thermal_conductivity_ve,
                                   double val_dist_ij,
                                   double *val_normal, double val_dS,
                                   double *val_Fv,
                                   double **val_Jac_i,
                                   double **val_Jac_j, CConfig *config) {

  // NOTE: Diffusion terms not implemented yet!!!
  // NOTE: Jacobian i & j should have the dVdU terms wrt i & j, but instead calculated using the mean
  
  bool ionization;
  unsigned short iDim, iEl, iSpecies, jSpecies, iVar, jVar, nHeavy, nEl, kVar;
  unsigned short *nElStates;
  double rho, u, v, w, T, Tve, rhoCvtr, rhoCvve, v2, ef, Cvtrs;
  double evibs, eels, num, denom;
  double mu, ktr, kve, *Ds, dij, Ru;
  double theta, thetax, thetay, thetaz;
  double etax, etay, etaz;
  double pix, piy, piz;
  double *dTdrs, *dTvedrs;
  double *Ys, *Tref, *Ms, *hf, *xi, *thv, **the, **g, *eve, *hs, sumY;
  double **dFdYj, **dFdYi, *sumdFdYih, *sumdFdYjh, *sumdFdYieve, *sumdFdYjeve;

  if (nDim == 2) {
    cout << "Viscous Proj Jacobian not available in 2D!!!" << endl;
    exit(1);
	}
  
  /*--- Initialize the Jacobian matrices ---*/
  for (iVar = 0; iVar < nVar; iVar++) {
    for (jVar = 0; jVar < nVar; jVar++) {
      val_Jac_i[iVar][jVar] = 0.0;
      val_Jac_j[iVar][jVar] = 0.0;
    }
  }
  
  /*--- Allocate mass-fraction array --*/
  Ys      = new double[nSpecies];
  dTdrs   = new double[nSpecies];
  dTvedrs = new double[nSpecies];
  hs      = new double[nSpecies];
  eve     = new double[nSpecies];
  dFdYi   = new double*[nSpecies];
  dFdYj   = new double*[nSpecies];
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    dFdYi[iSpecies] = new double[nSpecies];
    dFdYj[iSpecies] = new double[nSpecies];
  }
  sumdFdYih   = new double[nSpecies];
  sumdFdYjh   = new double[nSpecies];
  sumdFdYieve = new double[nSpecies];
  sumdFdYjeve = new double[nSpecies];
  
  /*--- Assign booleans from CConfig ---*/
  ionization = config->GetIonization();
  if (ionization) { nHeavy = nSpecies-1; nEl = 1; }
  else            { nHeavy = nSpecies;   nEl = 0; }
  
  /*--- Load from CConfig ---*/
  Ms   = config->GetMolar_Mass();
  Tref = config->GetRefTemperature();
  hf   = config->GetEnthalpy_Formation();
  xi   = config->GetRotationModes();
  thv  = config->GetCharVibTemp();
  the  = config->GetCharElTemp();
  g    = config->GetElDegeneracy();
  nElStates = config->GetnElStates();
  
  /*--- Rename for convenience ---*/
  rho = val_Mean_PrimVar[RHO_INDEX];
  u   = val_Mean_PrimVar[VEL_INDEX];
  v   = val_Mean_PrimVar[VEL_INDEX+1];
  w   = val_Mean_PrimVar[VEL_INDEX+2];
  T   = val_Mean_PrimVar[T_INDEX];
  Tve = val_Mean_PrimVar[TVE_INDEX];
  Ds  = val_diffusion_coeff;
  mu  = val_laminar_viscosity;
  ktr = val_thermal_conductivity;
  kve = val_thermal_conductivity_ve;
  dij = val_dist_ij;
  Ru  = UNIVERSAL_GAS_CONSTANT;
  rhoCvtr = val_Mean_PrimVar[RHOCVTR_INDEX];
  rhoCvve = val_Mean_PrimVar[RHOCVVE_INDEX];
  
  /*--- Calculate supporting quantities ---*/
  v2 = 0.0;
  for (iDim = 0; iDim <  nDim; iDim++)
    v2 += val_Mean_PrimVar[VEL_INDEX+iDim]*val_Mean_PrimVar[VEL_INDEX+iDim];
  
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    ef    = hf[iSpecies] - Ru/Ms[iSpecies]*Tref[iSpecies];
    Cvtrs = (3.0/2.0 + xi[iSpecies]/2.0)*Ru/Ms[iSpecies];
    if (thv[iSpecies]!= 0.0)
      evibs = Ru/Ms[iSpecies] * thv[iSpecies] / (exp(thv[iSpecies]/Tve) - 1.0);
    else
      evibs = 0.0;
    num   = 0.0;
    denom = g[iSpecies][0] * exp(the[iSpecies][0]/Tve);
    for (iEl = 1; iEl < nElStates[iSpecies]; iEl++) {
      num   += g[iSpecies][iEl] * the[iSpecies][iEl] * exp(-the[iSpecies][iEl]/Tve);
      denom += g[iSpecies][iEl] * exp(-the[iSpecies][iEl]/Tve);
    }
    eels = Ru/Ms[iSpecies] * (num/denom);
    eve[iSpecies]     = evibs + eels;
    hs[iSpecies]      = Ru/Ms[iSpecies]*T + (3.0/2.0+xi[iSpecies]/2.0)*Ru/Ms[iSpecies]
                      + hf[iSpecies] + eve[iSpecies];
    dTdrs[iSpecies]   = (-ef + 0.5*v2 + Cvtrs*(Tref[iSpecies]-T)) / rhoCvtr;
    dTvedrs[iSpecies] = -(evibs+eels)/rhoCvve;
    Ys[iSpecies]      = val_Mean_PrimVar[RHOS_INDEX+iSpecies]/rho;
  }
  if (ionization) {
    cout <<  "GetViscousProjJacs: Need to implement dT & dTve for electrons!!" << endl;
    exit(1);
  }
  
  /*--- Calculate geometrical parameters ---*/
  theta = 0.0;
  for (iDim = 0; iDim < nDim; iDim++) {
    theta += val_normal[iDim]*val_normal[iDim];
  }
  thetax = theta + (val_normal[0]*val_normal[0])/3.0;
  thetay = theta + (val_normal[1]*val_normal[1])/3.0;
  thetaz = theta + (val_normal[2]*val_normal[2])/3.0;
  etax   = val_normal[1]*val_normal[2]/3.0;
  etay   = val_normal[0]*val_normal[2]/3.0;
  etaz   = val_normal[0]*val_normal[1]/3.0;
  pix    = mu/dij * (thetax*u + etaz*v   + etay*w);
  piy    = mu/dij * (etaz*u   + thetay*v + etax*w);
  piz    = mu/dij * (etay*u   + etax*v   + thetaz*w);
  
  /*--- Calculate derivatives of Fv w.r.t. mass fractions ---*/
  for (iSpecies = 0; iSpecies < nHeavy; iSpecies++) {
    sumY = 0.0;
    for (jSpecies = 0; jSpecies < nHeavy; jSpecies++) {
      dFdYi[iSpecies][jSpecies] = -Ys[iSpecies]*rho*Ds[jSpecies]*theta/dij;
      dFdYj[iSpecies][jSpecies] = Ys[iSpecies]*rho*Ds[jSpecies]*theta/dij;
      sumY += rho*Ds[jSpecies]*theta/dij*(V_j[RHOS_INDEX+jSpecies] -
                                          V_i[RHOS_INDEX+jSpecies])/rho;
    }
    dFdYj[iSpecies][iSpecies] += -rho*Ds[iSpecies]*theta/dij + 0.5*sumY;
    dFdYi[iSpecies][iSpecies] += rho*Ds[iSpecies]*theta/dij + 0.5*sumY;

    sumdFdYih[iSpecies]   = 0.0;
    sumdFdYjh[iSpecies]   = 0.0;
    sumdFdYieve[iSpecies] = 0.0;
    sumdFdYjeve[iSpecies] = 0.0;
    for (jSpecies = 0; jSpecies < nSpecies; jSpecies++) {
      sumdFdYih[iSpecies]   += dFdYi[jSpecies][iSpecies]*hs[jSpecies];
      sumdFdYjh[iSpecies]   += dFdYj[jSpecies][iSpecies]*hs[jSpecies];
      sumdFdYieve[iSpecies] += dFdYi[jSpecies][iSpecies]*eve[jSpecies];
      sumdFdYjeve[iSpecies] += dFdYj[jSpecies][iSpecies]*eve[jSpecies];
    }
  }
  
  /*--- Populate the viscous Jacobian matrix (more terms follow) ---*/
  // x-momentum
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    val_Jac_j[nSpecies][iSpecies] = -pix/rho * val_dS;
  }
  val_Jac_j[nSpecies][nSpecies]     = mu*thetax/ (dij*rho) * val_dS;
  val_Jac_j[nSpecies][nSpecies+1]   = mu*etaz  / (dij*rho) * val_dS;
  val_Jac_j[nSpecies][nSpecies+2]   = mu*etay  / (dij*rho) * val_dS;
  // y-momentum
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    val_Jac_j[nSpecies+1][iSpecies] = -piy/rho * val_dS;
  }
  val_Jac_j[nSpecies+1][nSpecies]   = mu*etaz  / (dij*rho) * val_dS;
  val_Jac_j[nSpecies+1][nSpecies+1] = mu*thetay/ (dij*rho) * val_dS;
  val_Jac_j[nSpecies+1][nSpecies+2] = mu*etax  / (dij*rho) * val_dS;
  // z-momentum
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    val_Jac_j[nSpecies+2][iSpecies] = -piz/rho * val_dS;
  }
  val_Jac_j[nSpecies+2][nSpecies]   = mu*etay  / (dij*rho) * val_dS;
  val_Jac_j[nSpecies+2][nSpecies+1] = mu*etax  / (dij*rho) * val_dS;
  val_Jac_j[nSpecies+2][nSpecies+2] = mu*thetaz/ (dij*rho) * val_dS;
  // total energy
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    val_Jac_j[nSpecies+3][iSpecies] = (-pix*u/rho - piy*v/rho - piz*w/rho
                                    + ktr*theta/dij*dTdrs[iSpecies]
                                    + kve*theta/dij*dTvedrs[iSpecies]) * val_dS;
  }
  val_Jac_j[nSpecies+3][nSpecies]   = (pix/rho - ktr*theta*u/(dij*rhoCvtr)) * val_dS;
  val_Jac_j[nSpecies+3][nSpecies+1] = (piy/rho - ktr*theta*v/(dij*rhoCvtr)) * val_dS;
  val_Jac_j[nSpecies+3][nSpecies+2] = (piz/rho - ktr*theta*w/(dij*rhoCvtr)) * val_dS;
  val_Jac_j[nSpecies+3][nSpecies+3] = ktr*theta/(dij*rhoCvtr) * val_dS;
  val_Jac_j[nSpecies+3][nSpecies+4] = (-ktr*theta/(dij*rhoCvtr) +
                                       kve*theta/(dij*rhoCvve)) * val_dS;
  // vib-el. energy
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    val_Jac_j[nSpecies+4][iSpecies] = kve*theta/dij * dTvedrs[iSpecies] * val_dS;
  }
  val_Jac_j[nSpecies+4][nSpecies+4] = kve*theta/(dij*rhoCvve) * val_dS;
  
  /*--- Calculate i Jacobian before adding common terms ---*/
  for (iVar = 0; iVar < nVar; iVar++)
    for (jVar = 0; jVar < nVar; jVar++)
      val_Jac_i[iVar][jVar] = -val_Jac_j[iVar][jVar];
  
  /*--- Add common terms ---*/
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    // species continuity
    for (jSpecies = 0; jSpecies < nSpecies; jSpecies++) {
      for (kVar = 0; kVar < nSpecies; kVar++) {
//        val_Jac_i[iSpecies][jSpecies] += dFdYi[iSpecies][kVar]*(-Ys[kVar]/rho) * val_dS;
//        val_Jac_j[iSpecies][jSpecies] += dFdYj[iSpecies][kVar]*(-Ys[kVar]/rho) * val_dS;
      }
//      val_Jac_i[iSpecies][jSpecies] += dFdYi[iSpecies][jSpecies]/rho * val_dS;
//      val_Jac_j[iSpecies][jSpecies] += dFdYj[iSpecies][jSpecies]/rho * val_dS;
      
//      val_Jac_i[nSpecies+3][iSpecies] += sumdFdYih[jSpecies]*(-Ys[jSpecies]/rho) * val_dS;
//      val_Jac_j[nSpecies+3][iSpecies] += sumdFdYjh[jSpecies]*(-Ys[jSpecies]/rho) * val_dS;
//      val_Jac_i[nSpecies+4][iSpecies] += sumdFdYieve[jSpecies]*(-Ys[jSpecies]/rho) * val_dS;
//      val_Jac_j[nSpecies+4][iSpecies] += sumdFdYjeve[jSpecies]*(-Ys[jSpecies]/rho) * val_dS;
    }
//    val_Jac_i[nSpecies+3][iSpecies] += sumdFdYih[jSpecies]/rho * val_dS;
//    val_Jac_j[nSpecies+3][iSpecies] += sumdFdYjh[jSpecies]/rho * val_dS;
//    val_Jac_i[nSpecies+4][iSpecies] += sumdFdYieve[jSpecies]/rho * val_dS;
//    val_Jac_j[nSpecies+4][iSpecies] += sumdFdYjeve[jSpecies]/rho * val_dS;
    
    // total energy
    val_Jac_i[nSpecies+3][iSpecies] += -0.5*u*val_Fv[nSpecies]/rho
                                       -0.5*v*val_Fv[nSpecies+1]/rho
                                       -0.5*w*val_Fv[nSpecies+2]/rho;
    val_Jac_j[nSpecies+3][iSpecies] += -0.5*u*val_Fv[nSpecies]/rho
                                       -0.5*v*val_Fv[nSpecies+1]/rho
                                       -0.5*w*val_Fv[nSpecies+2]/rho;
  }
  for (iDim = 0; iDim < nDim; iDim++) {
    val_Jac_i[nSpecies+3][nSpecies+iDim] += 0.5*val_Fv[nSpecies+iDim]/rho;
    val_Jac_j[nSpecies+3][nSpecies+iDim] += 0.5*val_Fv[nSpecies+iDim]/rho;
  }
  
  delete [] Ys;
  delete [] dTdrs;
  delete [] dTvedrs;
  delete [] hs;
  delete [] eve;
  for (iSpecies = 0; iSpecies < nSpecies; iSpecies++) {
    delete [] dFdYi[iSpecies];
    delete [] dFdYj[iSpecies];
  }
  delete [] dFdYi;
  delete [] dFdYj;
  delete [] sumdFdYih;
  delete [] sumdFdYjh;
  delete [] sumdFdYieve;
  delete [] sumdFdYjeve;
}




void CNumerics::GetViscousProjJacsDiatomics(double *val_Mean_PrimVar,   double *val_laminar_viscosity, double *val_eddy_viscosity, double *val_thermal_conductivity, double *val_thermal_conductivity_vib, double val_dist_ij, double *val_normal, double val_dS, double *val_Proj_Visc_Flux, double **val_Proj_Jac_Tensor_i, double **val_Proj_Jac_Tensor_j, unsigned short val_iSpecies) {
  
  unsigned short iDim, iVar, jVar, kVar, loc, nVar_species;
  double tmp_i, tmp_j;
  double Theta, Theta_x, Theta_y, Theta_z, Eta_x, Eta_y, Eta_z, Pi_x, Pi_y, Pi_z, factor;
  double Viscosity_ij, ThermConductivity_ij, ThermConductivity_vib_ij, Gamma, GasConstant, Density, Vel2, Ttr, Tvib, h_f, Energy_el, CharVibTemp;
  
  /*--- Determine location in the Jacobian based on species ---*/
  if ( val_iSpecies < nDiatomics ) {
    loc = (nDim+3)*val_iSpecies;
    nVar_species = (nDim+3);
  }
  else {
    loc = (nDim+3)*nDiatomics + (nDim+2)*(val_iSpecies-nDiatomics);
    nVar_species = (nDim+2);
  }
  
	/*--- Calculate useful quantities ---*/
	Theta = 0.0;
	Vel2 = 0.0;
	for (iDim = 0; iDim < nDim; iDim++) {
		Theta += val_normal[iDim]*val_normal[iDim];
		Vel2  += val_Mean_PrimVar[iDim+1]*val_Mean_PrimVar[iDim+1];
	}
  
  Viscosity_ij             = val_laminar_viscosity[val_iSpecies];
  ThermConductivity_ij     = val_thermal_conductivity[val_iSpecies];
  ThermConductivity_vib_ij = val_thermal_conductivity_vib[val_iSpecies];
  GasConstant              = Gas_Constant_MultipleSpecies[val_iSpecies];
  CharVibTemp              = Theta_v[val_iSpecies];
  Gamma                    = Vector_Gamma[val_iSpecies];
  factor                   = Viscosity_ij / val_dist_ij;
  Density                  = val_Mean_PrimVar[nDim+3];
  Ttr                      = val_Mean_PrimVar[0];
  Tvib                     = val_Mean_PrimVar[nDim+1];
  h_f                      = Enthalpy_formation[val_iSpecies];
  Energy_el                = 0.0;
  
  if (nDim == 3) {
    /*--- Populate the transformation matrix ---*/
    //Comment: Technically, we should calculate this matrix using i and j values separately, then apply them to the appropriate Jacobian matrices at i & j.  We can try this with PrimVar_i and PrimVar_j as members of the CNumerics class, but try this implementation with the mean quantities as a first attempt
    dVdU[0][0] = 1.0;
    dVdU[0][1] = 0.0;
    dVdU[0][2] = 0.0;
    dVdU[0][3] = 0.0;
    dVdU[0][4] = 0.0;
    
    dVdU[1][0] = -val_Mean_PrimVar[1]/Density;
    dVdU[1][1] = 1.0/Density;
    dVdU[1][2] = 0.0;
    dVdU[1][3] = 0.0;
    dVdU[1][4] = 0.0;
    
    dVdU[2][0] = -val_Mean_PrimVar[2]/Density;
    dVdU[2][1] = 0.0;
    dVdU[2][2] = 1.0/Density;
    dVdU[2][3] = 0.0;
    dVdU[2][4] = 0.0;
    
    dVdU[3][0] = -val_Mean_PrimVar[3]/Density;
    dVdU[3][1] = 0.0;
    dVdU[3][2] = 0.0;
    dVdU[3][3] = 1.0/Density;
    dVdU[3][4] = 0.0;
    
    dVdU[4][0] = 1.0/Density * ( -Ttr + (Gamma-1.0)/GasConstant*(Vel2/2.0 - h_f - Energy_el) );
    dVdU[4][1] = -(Gamma-1.0)/GasConstant * val_Mean_PrimVar[1]/Density;
    dVdU[4][2] = -(Gamma-1.0)/GasConstant * val_Mean_PrimVar[2]/Density;
    dVdU[4][3] = -(Gamma-1.0)/GasConstant * val_Mean_PrimVar[3]/Density;
    dVdU[4][4] = (Gamma-1.0)/(GasConstant*Density);
    
    if (val_iSpecies < nDiatomics) {
      dVdU[5][0] = -Tvib*Tvib/(CharVibTemp*Density) * ( 1.0 - 1.0/(exp(CharVibTemp/Tvib)) );
      dVdU[5][1] = 0.0;
      dVdU[5][2] = 0.0;
      dVdU[5][3] = 0.0;
      dVdU[5][4] = 0.0;
      dVdU[5][5] = Tvib*Tvib/(CharVibTemp*CharVibTemp*Density*GasConstant) * (exp(CharVibTemp/Tvib)-1.0)*(exp(CharVibTemp/Tvib)-1.0)/exp(CharVibTemp/Tvib);
      
      dVdU[0][5] = 0.0;
      dVdU[1][5] = 0.0;
      dVdU[2][5] = 0.0;
      dVdU[3][5] = 0.0;
      dVdU[4][5] = -(Gamma-1.0)/(GasConstant*Density); 
    }    
    
    /*--- Calculate useful quantities ---*/
    Theta_x = Theta + 1.0/3.0 * val_normal[0]*val_normal[0];
    Theta_y = Theta + 1.0/3.0 * val_normal[1]*val_normal[1];
    Theta_z = Theta + 1.0/3.0 * val_normal[2]*val_normal[2];
    Eta_x   = 1.0/3.0 * val_normal[1]*val_normal[2];
    Eta_y   = 1.0/3.0 * val_normal[0]*val_normal[2];
    Eta_z   = 1.0/3.0 * val_normal[0]*val_normal[1];
    Pi_x    =   val_Mean_PrimVar[1] * factor * Theta_x
              + val_Mean_PrimVar[2] * factor * Eta_z
              + val_Mean_PrimVar[3] * factor * Eta_y;
    Pi_y    =   val_Mean_PrimVar[1] * factor * Eta_z
              + val_Mean_PrimVar[2] * factor * Theta_y
              + val_Mean_PrimVar[3] * factor * Eta_x;
    Pi_z    =   val_Mean_PrimVar[1] * factor * Eta_y
              + val_Mean_PrimVar[2] * factor * Eta_x
              + val_Mean_PrimVar[3] * factor * Theta_z;
        
    /*--- First row: dFv(1)/dV' ---*/
    dFvdV_j[0][0] = 0.0;
    dFvdV_j[0][1] = 0.0;
    dFvdV_j[0][2] = 0.0;
    dFvdV_j[0][3] = 0.0;
    dFvdV_j[0][4] = 0.0;
    
    /*--- Second row: dFv(2)/dV' ---*/
    dFvdV_j[1][0] = 0.0;
    dFvdV_j[1][1] = factor * Theta_x;
    dFvdV_j[1][2] = factor * Eta_z;
    dFvdV_j[1][3] = factor * Eta_y;
    dFvdV_j[1][4] = 0.0;
    
    /*--- Third row: dFv(3)/dV' ---*/
    dFvdV_j[2][0] = 0.0;
    dFvdV_j[2][1] = factor * Eta_z;
    dFvdV_j[2][2] = factor * Theta_y;
    dFvdV_j[2][3] = factor * Eta_x;
    dFvdV_j[2][4] = 0.0;
    
    /*--- Fourth row: dFv(4)/dV' ---*/
    dFvdV_j[3][0] = 0.0;
    dFvdV_j[3][1] = factor * Eta_y;
    dFvdV_j[3][2] = factor * Eta_x;
    dFvdV_j[3][3] = factor * Theta_z;
    dFvdV_j[3][4] = 0.0;
    
    /*--- Fifth row: dFv(5)/dV' ---*/
    dFvdV_j[4][0] = 0.0;
    dFvdV_j[4][1] = Pi_x; // Additional term follows
    dFvdV_j[4][2] = Pi_y; // Additional term follows
    dFvdV_j[4][3] = Pi_z; // Additional term follows
    dFvdV_j[4][4] = ThermConductivity_ij*Theta/val_dist_ij;

    
    /*--- Add additional column and row to accommodate vibrational energy convection ---*/
    if (val_iSpecies < nDiatomics) {
      dFvdV_j[0][5] = 0.0;
      dFvdV_j[1][5] = 0.0;
      dFvdV_j[2][5] = 0.0;
      dFvdV_j[3][5] = 0.0;
      dFvdV_j[4][5] = ThermConductivity_vib_ij*Theta/val_dist_ij;
      dFvdV_j[5][0] = 0.0;
      dFvdV_j[5][1] = 0.0;
      dFvdV_j[5][2] = 0.0;
      dFvdV_j[5][3] = 0.0;
      dFvdV_j[5][4] = 0.0;
      dFvdV_j[5][5] = ThermConductivity_vib_ij*Theta/val_dist_ij;
    }
    
  } else {
    /*--- Populate the transformation matrix ---*/
    //Comment: Technically, we should calculate this matrix using i and j values separately, then apply them to the appropriate Jacobian matrices at i & j.  We can try this with PrimVar_i and PrimVar_j as members of the CNumerics class, but try this implementation with the mean quantities as a first attempt
    dVdU[0][0] = 1.0;
    dVdU[0][1] = 0.0;
    dVdU[0][2] = 0.0;
    dVdU[0][3] = 0.0;
    
    dVdU[1][0] = -val_Mean_PrimVar[1]/Density;
    dVdU[1][1] = 1.0/Density;
    dVdU[1][2] = 0.0;
    dVdU[1][3] = 0.0;
    
    dVdU[2][0] = -val_Mean_PrimVar[2]/Density;
    dVdU[2][1] = 0.0;
    dVdU[2][2] = 1.0/Density;
    dVdU[2][3] = 0.0;
    
    dVdU[3][0] = 1.0/Density * ( -Ttr + (Gamma-1.0)/GasConstant*(Vel2/2.0 - h_f - Energy_el) );
    dVdU[3][1] = -(Gamma-1.0)/GasConstant * val_Mean_PrimVar[1]/Density;
    dVdU[3][2] = -(Gamma-1.0)/GasConstant * val_Mean_PrimVar[2]/Density;
    dVdU[3][3] = (Gamma-1.0)/(GasConstant*Density);
    
    if (val_iSpecies < nDiatomics) {
      dVdU[4][0] = -Tvib*Tvib/(CharVibTemp*Density) * ( 1.0 - 1.0/(exp(CharVibTemp/Tvib)) );
      dVdU[4][1] = 0.0;
      dVdU[4][2] = 0.0;
      dVdU[4][3] = 0.0;
      dVdU[4][4] = Tvib*Tvib/(CharVibTemp*CharVibTemp*Density*GasConstant) * (exp(CharVibTemp/Tvib)-1.0)*(exp(CharVibTemp/Tvib)-1.0)/exp(CharVibTemp/Tvib);
      
      dVdU[0][4] = 0.0;
      dVdU[1][4] = 0.0;
      dVdU[2][4] = 0.0;
      dVdU[3][4] = -(Gamma-1.0)/(GasConstant*Density);
    }
    
    /*--- Calculate useful quantities ---*/
    Theta_x = Theta + 1.0/3.0 * val_normal[0]*val_normal[0];
    Theta_y = Theta + 1.0/3.0 * val_normal[1]*val_normal[1];
    Eta_z   = 1.0/3.0 * val_normal[0] * val_normal[1];
    Pi_x    =   val_Mean_PrimVar[1] * factor * Theta_x
              + val_Mean_PrimVar[2] * factor * Eta_z;
    Pi_y    =   val_Mean_PrimVar[1] * factor * Eta_z
              + val_Mean_PrimVar[2] * factor * Theta_y;
    
    /*--- First row: dFv(1)/dV' ---*/
    dFvdV_j[0][0] = 0.0;
    dFvdV_j[0][1] = 0.0;
    dFvdV_j[0][2] = 0.0;
    dFvdV_j[0][3] = 0.0;
    
    /*--- Second row: dFv(2)/dV' ---*/
    dFvdV_j[1][0] = 0.0;
    dFvdV_j[1][1] = factor * Theta_x;
    dFvdV_j[1][2] = factor * Eta_z;
    dFvdV_j[1][3] = 0.0;
    
    /*--- Third row: dFv(3)/dV' ---*/
    dFvdV_j[2][0] = 0.0;
    dFvdV_j[2][1] = factor * Eta_z;
    dFvdV_j[2][2] = factor * Theta_x;
    dFvdV_j[2][3] = 0.0;
    
    /*--- Fourth row: dFv(4)/dV' ---*/
    dFvdV_j[3][0] = 0.0;
    dFvdV_j[3][1] = Pi_x; // Additional term follows
    dFvdV_j[3][2] = Pi_y; // Additional term follows
    dFvdV_j[3][3] = ThermConductivity_ij*Theta/val_dist_ij;
 
    /*--- Add additional column and row to accommodate vibrational energy convection ---*/
    if (val_iSpecies < nDiatomics) {
      dFvdV_j[0][4] = 0.0;
      dFvdV_j[1][4] = 0.0;
      dFvdV_j[2][4] = 0.0;
      dFvdV_j[3][4] = ThermConductivity_vib_ij*Theta/val_dist_ij;
      dFvdV_j[4][0] = 0.0;
      dFvdV_j[4][1] = 0.0;
      dFvdV_j[4][2] = 0.0;
      dFvdV_j[4][3] = 0.0;
      dFvdV_j[4][4] = ThermConductivity_vib_ij*Theta/val_dist_ij;
    }
  }
  
  /*--- Set the Jacobian at point i ---*/
  for (iVar = 0; iVar < nVar_species; iVar++)
    for (jVar = 0; jVar < nVar_species; jVar++)
      dFvdV_i[iVar][jVar] = -dFvdV_j[iVar][jVar];
  
  /*--- Add the final terms to the Jacobian that are the same for both points i and j ---*/
  // Note: Projected viscous fluxes are already integrated over the interface, divide by dS
  for (iDim = 0; iDim < nDim; iDim++) {
    dFvdV_i[nDim+1][iDim+1] += 0.5 * val_Proj_Visc_Flux[iDim+1]/val_dS;
    dFvdV_j[nDim+1][iDim+1] += 0.5 * val_Proj_Visc_Flux[iDim+1]/val_dS;
  }
  
  /*--- Multiply Tensor by the transformation matrix and area dS ---*/
  for (iVar = 0; iVar < nVar_species; iVar++) {
    for (jVar = 0; jVar < nVar_species; jVar++) {
      tmp_i = 0.0;
      tmp_j = 0.0;
      for (kVar = 0; kVar < nVar_species; kVar++) {
        tmp_i += dFvdV_i[iVar][kVar]*dVdU[kVar][jVar];
        tmp_j += dFvdV_j[iVar][kVar]*dVdU[kVar][jVar];
      }
      val_Proj_Jac_Tensor_i[loc+iVar][loc+jVar] = tmp_i*val_dS;
      val_Proj_Jac_Tensor_j[loc+iVar][loc+jVar] = tmp_j*val_dS;
    }
  }
}


void CNumerics::GetViscousProjJacs(double *val_Mean_PrimVar,   double *val_laminar_viscosity,
                                   double *val_eddy_viscosity, double *val_thermal_conductivity, double *val_thermal_conductivity_vib, double val_dist_ij, double *val_normal, double val_dS,
                                   double *val_Proj_Visc_Flux, double **val_Proj_Jac_Tensor_i, double **val_Proj_Jac_Tensor_j,
                                   unsigned short val_iSpecies) {
  
	unsigned short iDim, iVar, jVar, loc;
  
	if ( val_iSpecies < nDiatomics ) loc = (nDim+3)*val_iSpecies;
	else loc = (nDim+3)*nDiatomics + (nDim+2)*(val_iSpecies-nDiatomics);
  
	double theta = 0.0, sqvel = 0.0, proj_viscousflux_vel = 0.0;
	for (iDim = 0; iDim < nDim; iDim++) {
		theta += val_normal[iDim]*val_normal[iDim];
		sqvel += val_Mean_PrimVar[iDim+1]*val_Mean_PrimVar[iDim+1];
		proj_viscousflux_vel += val_Proj_Visc_Flux[iDim+1]*val_Mean_PrimVar[iDim+1];
	}
	double phi = 0.5*(Gamma-1.)*sqvel;
  
	double Gamma = Vector_Gamma[val_iSpecies];
	double Density = val_Mean_PrimVar[nDim+3];
	double Pressure = val_Mean_PrimVar[nDim+2];
	double total_viscosity = val_laminar_viscosity[val_iSpecies] + val_eddy_viscosity[val_iSpecies];
  
  //double heat_flux_factor = val_laminar_viscosity[val_iSpecies] / PRANDTL + val_eddy_viscosity[val_iSpecies] / PRANDTL_TURB;
  double heat_flux_factor = val_thermal_conductivity[val_iSpecies] / (Gamma/(Gamma-1.0) * Gas_Constant_MultipleSpecies[val_iSpecies]);  
  
	double cpoR = Gamma/(Gamma-1.); // cp over R
	double factor = total_viscosity/(Density*val_dist_ij)*val_dS;
	double phi_rho = -cpoR*heat_flux_factor*Pressure/(Density*Density);
	double phi_p = cpoR*heat_flux_factor/Density;
	double rhoovisc = Density/total_viscosity; // rho over viscosity
  
  
	if (nDim == 2) {
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
    
		double etaz = val_normal[0]*val_normal[1]/3.0;
    
		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay;
    
		val_Proj_Jac_Tensor_i[loc+0][loc+0] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+1] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+2] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+3] = 0.0;
		val_Proj_Jac_Tensor_i[loc+1][loc+0] = factor*pix;
		val_Proj_Jac_Tensor_i[loc+1][loc+1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[loc+1][loc+2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+1][loc+3] = 0.0;
		val_Proj_Jac_Tensor_i[loc+2][loc+0] = factor*piy;
		val_Proj_Jac_Tensor_i[loc+2][loc+1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+2][loc+2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[loc+2][loc+3] = 0.0;
    
		val_Proj_Jac_Tensor_i[loc+3][loc+0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) -
                                                   pix*val_Mean_PrimVar[1]+piy*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[loc+3][loc+1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[loc+3][loc+2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[loc+3][loc+3] = -factor*((Gamma-1)*rhoovisc*theta*phi_p);
    
		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];
    
		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[loc+3][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[loc+3][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[loc+3][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[loc+3][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[loc+3][loc+2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[loc+3][loc+2] += factor*val_Proj_Visc_Flux[2];
	}
	else {
    
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
		double thetaz = theta + val_normal[2]*val_normal[2]/3.0;
    
		double etax = val_normal[1]*val_normal[2]/3.0;
		double etay = val_normal[0]*val_normal[2]/3.0;
		double etaz = val_normal[0]*val_normal[1]/3.0;
    
		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz   + val_Mean_PrimVar[3]*etay;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay + val_Mean_PrimVar[3]*etax;
		double piz = val_Mean_PrimVar[1]*etay   + val_Mean_PrimVar[2]*etax   + val_Mean_PrimVar[3]*thetaz;
    
		val_Proj_Jac_Tensor_i[loc+0][loc+0] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+1] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+2] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+3] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+1][loc+0] = factor*pix;
		val_Proj_Jac_Tensor_i[loc+1][loc+1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[loc+1][loc+2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+1][loc+3] = -factor*etay;
		val_Proj_Jac_Tensor_i[loc+1][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+2][loc+0] = factor*piy;
		val_Proj_Jac_Tensor_i[loc+2][loc+1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+2][loc+2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[loc+2][loc+3] = -factor*etax;
		val_Proj_Jac_Tensor_i[loc+2][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+3][loc+0] = factor*piz;
		val_Proj_Jac_Tensor_i[loc+3][loc+1] = -factor*etay;
		val_Proj_Jac_Tensor_i[loc+3][loc+2] = -factor*etax;
		val_Proj_Jac_Tensor_i[loc+3][loc+3] = -factor*thetaz;
		val_Proj_Jac_Tensor_i[loc+3][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+4][loc+0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) -
                                                   pix*val_Mean_PrimVar[1] + piy*val_Mean_PrimVar[2] + piz*val_Mean_PrimVar[3]);
		val_Proj_Jac_Tensor_i[loc+4][loc+1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[loc+4][loc+2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[loc+4][loc+3] = -factor*(piz-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[3]);
		val_Proj_Jac_Tensor_i[loc+4][loc+4] = -factor*((Gamma-1)*rhoovisc*theta*phi_p);
    
/*    cout << "K term (old): " << -factor*(rhoovisc*theta*(phi_rho+phi*phi_p)) << endl;
    cout << "pi terms (old): " << -factor*(-pix*val_Mean_PrimVar[1] + piy*val_Mean_PrimVar[2] + piz*val_Mean_PrimVar[3]) << endl;
    cout << "****" << endl;
    cin.get();*/
    
		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];
    
		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[loc+4][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[loc+4][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[loc+4][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[loc+4][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[loc+4][loc+2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[loc+4][loc+2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_i[loc+4][loc+3] += factor*val_Proj_Visc_Flux[3];
		val_Proj_Jac_Tensor_j[loc+4][loc+3] += factor*val_Proj_Visc_Flux[3];
	}
}


//NOTE: THIS ROUTINE NOT GENERAL FOR MOLECULES W/ INTERNAL STRUCTURE!!!
void CNumerics::GetViscousProjJacs(double *val_Mean_PrimVar,   double *val_laminar_viscosity,
		double *val_eddy_viscosity, double val_dist_ij, double *val_normal, double val_dS,
		double *val_Proj_Visc_Flux, double **val_Proj_Jac_Tensor_i, double **val_Proj_Jac_Tensor_j,
		unsigned short val_iSpecies) {

	unsigned short iDim, iVar, jVar, loc;

	if ( val_iSpecies < nDiatomics ) loc = (nDim+3)*val_iSpecies;
	else loc = (nDim+3)*nDiatomics + (nDim+2)*(val_iSpecies-nDiatomics);

	double theta = 0.0, sqvel = 0.0, proj_viscousflux_vel = 0.0;
	for (iDim = 0; iDim < nDim; iDim++) {
		theta += val_normal[iDim]*val_normal[iDim];
		sqvel += val_Mean_PrimVar[iDim+1]*val_Mean_PrimVar[iDim+1];
		proj_viscousflux_vel += val_Proj_Visc_Flux[iDim+1]*val_Mean_PrimVar[iDim+1];
	}
	double phi = 0.5*(Gamma-1.)*sqvel;

	double Gamma = Vector_Gamma[val_iSpecies];
	double Density = val_Mean_PrimVar[nDim+3];
	double Pressure = val_Mean_PrimVar[nDim+2];
	double total_viscosity = val_laminar_viscosity[val_iSpecies] + val_eddy_viscosity[val_iSpecies];

  double heat_flux_factor = val_laminar_viscosity[val_iSpecies] / PRANDTL + val_eddy_viscosity[val_iSpecies] / PRANDTL_TURB;
  
  
	double cpoR = Gamma/(Gamma-1.); // cp over R
	double factor = total_viscosity/(Density*val_dist_ij)*val_dS;
	double phi_rho = -cpoR*heat_flux_factor*Pressure/(Density*Density);
	double phi_p = cpoR*heat_flux_factor/Density;
	double rhoovisc = Density/total_viscosity; // rho over viscosity

  
	if (nDim == 2) {
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;

		double etaz = val_normal[0]*val_normal[1]/3.0;

		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay;

		val_Proj_Jac_Tensor_i[loc+0][loc+0] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+1] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+2] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+3] = 0.0;
		val_Proj_Jac_Tensor_i[loc+1][loc+0] = factor*pix;
		val_Proj_Jac_Tensor_i[loc+1][loc+1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[loc+1][loc+2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+1][loc+3] = 0.0;
		val_Proj_Jac_Tensor_i[loc+2][loc+0] = factor*piy;
		val_Proj_Jac_Tensor_i[loc+2][loc+1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+2][loc+2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[loc+2][loc+3] = 0.0;

		val_Proj_Jac_Tensor_i[loc+3][loc+0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) -
				pix*val_Mean_PrimVar[1]+piy*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[loc+3][loc+1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[loc+3][loc+2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[loc+3][loc+3] = -factor*((Gamma-1)*rhoovisc*theta*phi_p);

		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];

		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[loc+3][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[loc+3][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[loc+3][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[loc+3][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[loc+3][loc+2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[loc+3][loc+2] += factor*val_Proj_Visc_Flux[2];
	}
	else {

		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
		double thetaz = theta + val_normal[2]*val_normal[2]/3.0;

		double etax = val_normal[1]*val_normal[2]/3.0;
		double etay = val_normal[0]*val_normal[2]/3.0;
		double etaz = val_normal[0]*val_normal[1]/3.0;

		double pix = val_Mean_PrimVar[1]*thetax + val_Mean_PrimVar[2]*etaz   + val_Mean_PrimVar[3]*etay;
		double piy = val_Mean_PrimVar[1]*etaz   + val_Mean_PrimVar[2]*thetay + val_Mean_PrimVar[3]*etax;
		double piz = val_Mean_PrimVar[1]*etay   + val_Mean_PrimVar[2]*etax   + val_Mean_PrimVar[3]*thetaz;

		val_Proj_Jac_Tensor_i[loc+0][loc+0] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+1] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+2] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+3] = 0.0;
		val_Proj_Jac_Tensor_i[loc+0][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+1][loc+0] = factor*pix;
		val_Proj_Jac_Tensor_i[loc+1][loc+1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[loc+1][loc+2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+1][loc+3] = -factor*etay;
		val_Proj_Jac_Tensor_i[loc+1][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+2][loc+0] = factor*piy;
		val_Proj_Jac_Tensor_i[loc+2][loc+1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[loc+2][loc+2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[loc+2][loc+3] = -factor*etax;
		val_Proj_Jac_Tensor_i[loc+2][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+3][loc+0] = factor*piz;
		val_Proj_Jac_Tensor_i[loc+3][loc+1] = -factor*etay;
		val_Proj_Jac_Tensor_i[loc+3][loc+2] = -factor*etax;
		val_Proj_Jac_Tensor_i[loc+3][loc+3] = -factor*thetaz;
		val_Proj_Jac_Tensor_i[loc+3][loc+4] = 0.0;
		val_Proj_Jac_Tensor_i[loc+4][loc+0] = -factor*(rhoovisc*theta*(phi_rho+phi*phi_p) -
				pix*val_Mean_PrimVar[1] + piy*val_Mean_PrimVar[2] + piz*val_Mean_PrimVar[3]);
		val_Proj_Jac_Tensor_i[loc+4][loc+1] = -factor*(pix-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[1]);
		val_Proj_Jac_Tensor_i[loc+4][loc+2] = -factor*(piy-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[2]);
		val_Proj_Jac_Tensor_i[loc+4][loc+3] = -factor*(piz-rhoovisc*theta*phi_p*(Gamma-1)*val_Mean_PrimVar[3]);
		val_Proj_Jac_Tensor_i[loc+4][loc+4] = -factor*((Gamma-1)*rhoovisc*theta*phi_p);
    
		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];

		factor = 0.5/Density;
		val_Proj_Jac_Tensor_i[loc+4][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_j[loc+4][loc+0] += factor*proj_viscousflux_vel;
		val_Proj_Jac_Tensor_i[loc+4][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_j[loc+4][loc+1] += factor*val_Proj_Visc_Flux[1];
		val_Proj_Jac_Tensor_i[loc+4][loc+2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_j[loc+4][loc+2] += factor*val_Proj_Visc_Flux[2];
		val_Proj_Jac_Tensor_i[loc+4][loc+3] += factor*val_Proj_Visc_Flux[3];
		val_Proj_Jac_Tensor_j[loc+4][loc+3] += factor*val_Proj_Visc_Flux[3];
	}
}

void CNumerics::GetViscousArtCompProjJacs(double val_laminar_viscosity,
		double val_eddy_viscosity, double val_dist_ij, double *val_normal, double val_dS,
		double **val_Proj_Jac_Tensor_i, double **val_Proj_Jac_Tensor_j) {
	unsigned short iDim, iVar, jVar;

	double theta = 0.0;
	for (iDim = 0; iDim < nDim; iDim++)
		theta += val_normal[iDim]*val_normal[iDim];

	double total_viscosity = val_laminar_viscosity + val_eddy_viscosity;
	double factor = total_viscosity/(val_dist_ij)*val_dS;

	if (nDim == 3) {
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
		double thetaz = theta + val_normal[2]*val_normal[2]/3.0;

		double etax = val_normal[1]*val_normal[2]/3.0;
		double etay = val_normal[0]*val_normal[2]/3.0;
		double etaz = val_normal[0]*val_normal[1]/3.0;

		val_Proj_Jac_Tensor_i[0][0] = 0.0;
		val_Proj_Jac_Tensor_i[0][1] = 0.0;
		val_Proj_Jac_Tensor_i[0][2] = 0.0;
		val_Proj_Jac_Tensor_i[0][3] = 0.0;
		val_Proj_Jac_Tensor_i[1][0] = 0.0;
		val_Proj_Jac_Tensor_i[1][1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[1][2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[1][3] = -factor*etay;
		val_Proj_Jac_Tensor_i[2][0] = 0.0;
		val_Proj_Jac_Tensor_i[2][1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][2] = -factor*thetay;
		val_Proj_Jac_Tensor_i[2][3] = -factor*etax;
		val_Proj_Jac_Tensor_i[3][0] = 0.0;
		val_Proj_Jac_Tensor_i[3][1] = -factor*etay;
		val_Proj_Jac_Tensor_i[3][2] = -factor*etax;
		val_Proj_Jac_Tensor_i[3][3] = -factor*thetaz;

		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];

	}

	if (nDim == 2) {
		double thetax = theta + val_normal[0]*val_normal[0]/3.0;
		double thetay = theta + val_normal[1]*val_normal[1]/3.0;
		double etaz = val_normal[0]*val_normal[1]/3.0;

		val_Proj_Jac_Tensor_i[0][0] = 0.0;
		val_Proj_Jac_Tensor_i[0][1] = 0.0;
		val_Proj_Jac_Tensor_i[0][2] = 0.0;
		val_Proj_Jac_Tensor_i[1][0] = 0.0;
		val_Proj_Jac_Tensor_i[1][1] = -factor*thetax;
		val_Proj_Jac_Tensor_i[1][2] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][0] = 0.0;
		val_Proj_Jac_Tensor_i[2][1] = -factor*etaz;
		val_Proj_Jac_Tensor_i[2][2] = -factor*thetay;

		for (iVar = 0; iVar < nVar; iVar++)
			for (jVar = 0; jVar < nVar; jVar++)
				val_Proj_Jac_Tensor_j[iVar][jVar] = -val_Proj_Jac_Tensor_i[iVar][jVar];
	}
}

CSourceNothing::CSourceNothing(unsigned short val_nDim, unsigned short val_nVar, CConfig *config) : CNumerics(val_nDim, val_nVar, config) { }

CSourceNothing::~CSourceNothing(void) { }