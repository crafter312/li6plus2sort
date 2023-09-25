
#include "correl2.h"


correl2::correl2()
{
  proton.init(1,1);
  particle.push_back(&proton);
  H2.init(1,2);
  particle.push_back(&H2);
  H3.init(1,3);
  particle.push_back(&H3);
  He3.init(2,3);
  particle.push_back(&He3);
  alpha.init(2,4);
  particle.push_back(&alpha);
  
  //new slew of particles
  //Silicon isotopes
  Si28.init(14,28);
  particle.push_back(&Si28);
  //Phosphorus isotopes
  P30.init(15,30);
  particle.push_back(&P30);
  P31.init(15,31);
  particle.push_back(&P31);
  P32.init(15,32);
  particle.push_back(&P32);
  //Sulfur isotopes
  S31.init(16,31);
  particle.push_back(&S31);
  S32.init(16,32);
  particle.push_back(&S32);
  S33.init(16,33);
  particle.push_back(&S33);
  //Chlorine isotopes
  Cl31.init(17,31);
  particle.push_back(&Cl31);
  Cl32.init(17,32);
  particle.push_back(&Cl32);
  Cl33.init(17,33);
  particle.push_back(&Cl33);
  Cl34.init(17,34);
  particle.push_back(&Cl34);
  //Argon isotopes
  Ar32.init(18,32);
  particle.push_back(&Ar32);
  Ar33.init(18,33);
  particle.push_back(&Ar33);
  Ar34.init(18,34);
  particle.push_back(&Ar34);
  Ar35.init(18,35);
  particle.push_back(&Ar35);
  //Potassium isotopes
  K35.init(19,35);
  particle.push_back(&K35);
  K36.init(19,36);
  particle.push_back(&K36);
  //Caclium isotopes
  Ca35.init(20,35);
  particle.push_back(&Ca35);
  Ca36.init(20,36);
  particle.push_back(&Ca36);

  Nparticles = particle.size();
}


//*********************************************
void correl2::reset()
{
  for (int i=0;i<Nparticles;i++) particle[i]->mult = 0.;
}
//********************************************
void correl2::zeroMask()
{
  for (int i=0;i<Nparticles;i++) particle[i]->zeroMask();
}
//**************************************************
void correl2::makeArray(bool flagMask)
{
  N = 0;


  for (int i=0;i<Nparticles;i++)
  {
    for (int j=0;j<particle[i]->mult;j++)
    {
      if (!flagMask || particle[i]->mask[j])
      {
        frag[N] = particle[i]->Sol[j];
        N++;
      }
    }
  }
}
//************************************************

void correl2::load(solution * fragment)
{
  for (int i=0;i<Nparticles;i++)
  {
    if (fragment->iZ == particle[i]->Z && fragment->iA == particle[i]->A)
    {
      if (particle[i]->mult < 6)
      {
        particle[i]->Sol[particle[i]->mult] = fragment;
        particle[i]->mult++;
      }
      break;
    }
  }
}


//**************************************************************
/**
 * Finds the total kinetic energy of the fragments
 * in there center-of-mass frame.
 */
float correl2::findErel()
{
  
  //first find total momentum
  for (int i=0;i<3;i++) Mtot[i] = 0.;
  float energyTot = 0.;   // total energy for relativity, total mass for newton

  for (int i=0;i<N;i++) 
  {
    energyTot += frag[i]->energyTot;
    //cout << "frag " << i << " Z = " << frag[i]->iZ;
    //cout <<" energyTot = " << frag[i]->energyTot;
    //cout << " Ekin = " << frag[i]->Ekin;
    //cout << " mass = " << frag[i]->mass << endl;

    if(frag[i]->mass > 1000000) abort();
    
    for (int j=0;j<3;j++)
    {
      Mtot[j] += frag[i]->Mvect[j];
      //  cout << "Frag = " << i << " Momentum " << j << " = " << Mtot[j] << endl;
    }
  }

  momentumCM = 0.;
  for (int j=0;j<3;j++) momentumCM += pow(Mtot[j],2);
  momentumCM = sqrt(momentumCM);


  //float mmc = 0.;
  //for (int i=0;i<3;i++) mmc += pow(momC[i],2);
  //mmc = sqrt(mmc);
  //cosThetaC = momC[2]/mmc;

  //PperpC = sqrt(pow(momC[0],2)+pow(momC[1],2));
  //PparaC = momC[2];
  //PtotC = sqrt(pow(momC[0],2)+pow(momC[1],2)+pow(momC[2],2));

  //velocity of cemter of mass
  velocityCM = momentumCM*Kinematics.c/energyTot;

  //  cout << "energyTot = " << energyTot << endl;
  
  float velCM[3]={0.};
  for (int j=0;j<3;j++) velCM[j] = velocityCM/momentumCM*Mtot[j];
  thetaCM = acos(velCM[2]/velocityCM);
  phiCM = atan2(velCM[1],velCM[0]);

  float totalKE = 0.;
  for (int i=0;i<N;i++)
  {

    float eKinNew = Kinematics.transformMomentum(frag[i]->Mvect,velCM,frag[i]->energyTot,frag[i]->MomCM);
    frag[i]->energyCM = eKinNew - Kinematics.scale*frag[i]->mass;
    totalKE += eKinNew - Kinematics.scale*frag[i]->mass;
    check_ke = eKinNew - Kinematics.scale*frag[i]->mass;
    check_mass = Kinematics.scale*frag[i]->mass;
  }


  //In case of 2p decay find angle between heavy fragment's momentum and CM momentum?
  if (N == 3)
  {
    float dot = 0.;
    float mm = 0.;
    for (int j=0;j<3;j++)
    {
      dot += frag[2]->MomCM[j]*momC[j];
      mm += pow(frag[2]->MomCM[j],2);
    }
    mm = sqrt(mm);
    cosAlphaQ = dot/mm/PtotC;
    // cos(x) = A*B/(|A| |B|) where A is heavy fragment's momentum and B is CM momentum
  }

  //  cout << "Erel = " << totalKE << endl;

  return totalKE;
}
//***********************************************************
void correl2::getJacobi()
{

  for (int i=0;i<3;i++)
    {
      frag[i]->momentumCM = 0.;
      for (int k=0;k<3;k++) frag[i]->momentumCM +=
                  pow(frag[i]->MomCM[k],2);
      frag[i]->momentumCM = sqrt(frag[i]->momentumCM);
    }


  //alpha is the  third fragment
  //first JacobiT
  float dot = 0.;
  float pp[3] = {0.};
  float PP = 0.;
  for (int k=0;k<3;k++)
    {
      pp[k] = frag[0]->MomCM[k] - frag[1]->MomCM[k];
      PP += pow(pp[k],2);
      dot += pp[k]*frag[2]->MomCM[k];
    }
  PP = sqrt(PP);
  cosThetaT = dot/PP/frag[2]->momentumCM;


  dot = 0;
  double PP1 = 0;
  double pp1[3]={0.};
  for (int k=0;k<3;k++)
    {
      pp1[k] = frag[0]->MomCM[k]/frag[0]->mass 
    - frag[2]->MomCM[k]/frag[2]->mass;
      PP1 += pow(pp1[k],2);
      dot += pp1[k]*frag[1]->MomCM[k];
    }  
  PP1 = sqrt(PP1);
  cosThetaY[0] = -dot/PP1/frag[1]->momentumCM;


  dot = 0;
  double PP2 = 0;
  double pp2[3]={0.};
  for (int k=0;k<3;k++)
    {
      pp2[k] = frag[1]->MomCM[k]/frag[1]->mass 
    - frag[2]->MomCM[k]/frag[2]->mass;
      PP2 += pow(pp2[k],2);
      dot += pp2[k]*frag[0]->MomCM[k];
    }  
  PP2 = sqrt(PP2);
  cosThetaY[1] = -dot/PP2/frag[0]->momentumCM;


  cosThetaV = (pp1[0]*pp2[0] + pp1[1]*pp2[1] + pp1[2]*pp2[2])/PP1/PP2;
}
