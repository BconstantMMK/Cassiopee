//initialisation Newton SA  + vitesse cible

#ifdef _OPENM4
#pragma omp simd
#endif

E_Float tol = 1.e-12;
E_Float Cv1 = 7.1;

for (E_Int noind = 0; noind < ifin-ideb; noind++)
{
  
  utau_vec[noind] = std::max(utau_vec[noind], 1.e-12);

  if (gradP_vec[noind] == 0.) {
      gradP_vec[noind] = 0.;
      utau_vec[noind] = utauOri_vec[noind];

      yplus            = utau_vec[noind]*yplus_vec[noind];
      yplus_vec[noind] = yplus;

      denoml10 = yplus*yplus-8.15*yplus+86.;
      denoml10 = denoml10*denoml10;

      px  = gradP_vec[noind]/pow(utau_vec[noind],3);
      
      l11 = pow(yplus+10.6,9.6);
      l12 = yplus*(yplus-8.15) + 86.;
      l13 = (2.*yplus-8.15)/16.7;

      l1 = 5.424*atan(l13) + log10(l11/(l12*l12)) - 3.52;
      l2 = -2.*kappainv*log((sqrt(1.+px*yplus)+1.)/2.);
      l3 = 2.*kappainv*(sqrt(1.+px*yplus)-1.);

      umod = utau_vec[noind]*(l1 + l2 + l3);
  }

  else { 
    //Mafzal s'applique - FULL VERSION
    yplus = utau_vec[noind]*yplus_vec[noind];
    yplus_vec[noind] = yplus;

    denoml10 = yplus*yplus-8.15*yplus+86.;
    denoml10 = denoml10*denoml10;

    px  = gradP_vec[noind]/pow(utau_vec[noind],3);
    
    l11 = pow(yplus+10.6,9.6);
    l12 = yplus*(yplus-8.15) + 86.;
    l13 = (2.*yplus-8.15)/16.7;

    l1 = 5.424*atan(l13) + log10(l11/(l12*l12)) - 3.52;
    l2 = 0.;
    l3 = 0.;

    if (px > 0.){
      if (MafzalMode == 1){
        l2 = -2.*kappainv*log((sqrt(1.+px*yplus)+1.)/2.); //PRESSURE
        l3 = 2.*kappainv*(sqrt(1.+px*yplus)-1.); //PRESSURE
      }
      else{
        l2 = 2.*kappainv*log((sqrt(1.+px*yplus)+1.)/2.); //PRESSURE
        l3 = 0.; //PRESSURE
      }
    }
    else{
      if (MafzalMode == 3){
        px = -px;
        l2 = -2.*kappainv*log((sqrt(1.+px*yplus)+1.)/2.); //PRESSURE
        l3 = 0.; //PRESSURE
      }
      else{
        l2 = 0.; //PRESSURE
        l3 = 0.; //PRESSURE
      }
    }
    
    umod = utau_vec[noind]*(l1 + l2 + l3);
  }
 
  umod = K_FUNC::E_abs(umod);

  ucible0 = sign_vec[noind] * umod;
  ucible_vec[noind] += ucible0 * ut_vec[noind]; // vitesse tangentielle pour le pt IBC
  vcible_vec[noind] += ucible0 * vt_vec[noind];
  wcible_vec[noind] += ucible0 * wt_vec[noind];

  // uext: norme de la composante tangentielle de la vitesse externe
  uext = sqrt(ut_vec[noind]*ut_vec[noind]+vt_vec[noind]*vt_vec[noind]+wt_vec[noind]*wt_vec[noind]);
  uext = std::max(uext, 1.e-12);


  tcible_vec[noind] = tcible_vec[noind] + 0.5*pow(Pr,one_third)/(cv*gamma)*(uext*uext - umod*umod); // Crocco-Busemann

  //twall = tcible_vec[noind] + 0.5*pow(Pr,one_third)/(cv*gamma)*(uext*uext);
  //tcible_vec[noind] =  twall + (tcible_vec[noind] + 0.5*(uext*uext)/(cv*gamma) - twall)*(umod/uext) - 0.5*(umod*umod)/(cv*gamma); // Equations de Crocco, plus precises pour ecoulements compressibles

  // van dryst pour nut
  expy                = 1.-exp(-yplus/19.);// ranges 17 a 26
  nutcible_vec[noind] = (kappa * alpha_vec[noind])*utau_vec[noind] * expy*expy;//negatif si pt ibc interieur aux corps

  E_Float nutcible = K_FUNC::E_abs( nutcible_vec[noind] );

  // equation 4eme degre
  E_Float a = nutcible;
  E_Float b = nutcible*pow( (mu_vec[noind]/ro_vec[noind])*Cv1, 3.);
  //printf("x^4 + %g x^3 +%g = 0\n", -a,-b);

  // debug
  //a = 1.;
  //b = -12.;

  // changement de variable 4eme degre
  E_Float p = -3*a*a/8.;
  E_Float q = -a*a*a/8.;
  E_Float r = -3*pow(a/4.,4.)-b;

  // equation 3eme degre
  E_Float ap = 8.;
  E_Float bp = -4*p;
  E_Float cp = -8*r;
  //E_Float dp = (a*a)/2.*(pow(a/4.,4.)+3*b);
  E_Float dp = 4*p*r-q*q;
  //printf("cubique: %g x^3 + %g x^2 + %g x + %g\n",ap,bp,cp,dp);

  // racines du 3eme degre
  E_Float delta = 18*ap*bp*cp*dp-4*bp*bp*bp*dp+bp*bp*cp*cp-4*ap*cp*cp*cp-27*ap*ap*dp*dp;
  E_Float delta0 = bp*bp-3*ap*cp;
  E_Float delta1 = 2*bp*bp*bp-9*ap*bp*cp+27*ap*ap*dp;

  E_Float superdelta = -27.*ap*ap*delta;

  //printf("delta %g superdelta>0 = %g\n", delta, superdelta);


  E_Float y1 = -1.;
  E_Float y2 = -1.;
  if (fabs(delta) < tol && fabs(delta0) < tol)
  {
    y1 = -bp/(3*ap);  //printf("racine y1=%g\n", y1);
  }
  else if (fabs(delta) < tol)
  {
    y1 = (9*ap*dp - bp*cp)/(2*delta0);
    y2 = (4*ap*bp*cp-9*ap*ap*dp-bp*bp*bp)/(ap*delta0);
    //printf("racine y1=%g y2=%g\n", y1, y2);
  }
  else
  {
    /* version super delta */
    E_Float C1 = -1.; E_Float C2 = -1.;
    if (superdelta >= 0.)
    {
      E_Float root = sqrt(superdelta);
      if (delta1-root >= 0.)
        C1 = pow( (delta1 -root) /2., 1./3. );
      else
        C1 = -pow( (root -delta1) /2., 1./3. );
      if (delta1 +root >= 0)
        C2 = pow( (delta1 +root) /2., 1./3. );
      else
        C2 = -pow( -(delta1 +root) /2., 1./3. );
    }

    //printf("C1=%g, C2=%g\n", C1, C2);
    y1 = -1./(3*ap)*(bp + C1 + delta0/C1 );
    y2 = -1./(3*ap)*(bp + C2 + delta0/C2 );
    //printf("racine y1=%g y2=%g\n", y1, y2);
  }


  // racine de l'equation du 4eme degre
  E_Float c1 = 2*y1-p;
  E_Float c2 = 2*y2-p;
  //printf("c1 > 0 = %g, c2 > 0 = %g\n",c1,c2);

  E_Float z1 = -123456.;
  if (c1 >= tol)
  {
    E_Float p1 = -2*y1-p+2*q/(sqrt(c1));
    E_Float p2 = -2*y1-p-2*q/(sqrt(c1));
    //printf("1. p1=%g p2=%g\n", p1,p2);
    if (p1 >= tol) { z1 = 0.5*(sqrt(c1)+sqrt(p1));}
    else if (p2 >= tol) { z1 = 0.5*(sqrt(c1)+sqrt(p2));}
  }
  if (c2 >= tol && z1 == -123456)
  {
    E_Float p1 = -2*y2-p+2*q/(sqrt(c2));
    E_Float p2 = -2*y2-p-2*q/(sqrt(c2));
    //printf("2. p1=%g p2=%g\n", p1,p2);
    if (p1 >= tol) { z1 = 0.5*(sqrt(c2)+sqrt(fabs(p1)));}
    else if (p2 >= tol) { z1 = 0.5*(sqrt(c2)+sqrt(fabs(p2)));}
  }
  if (c1 <= tol && z1 == -123456)
  {
    E_Float b0 = y1*y1-r;
    if (b0 >= tol)
    {
      E_Float p1 = -2*y1-p+4.*sqrt(b0);
      E_Float p2 = -2*y1-p-4.*sqrt(b0);
      //printf("3. p1=%g p2=%g\n", p1,p2);
      if (p1 >= tol) { z1 = 0.5*(sqrt(fabs(c1))+sqrt(fabs(p1))); }
      else if (p2 >= tol) { z1 = 0.5*(sqrt(fabs(c1))+sqrt(fabs(p2))); }
    }
  }
  if (c2 <= tol && z1 == -123456)
  {
    E_Float b0 = y2*y2-r;
    if (b0 >= tol)
    {
      E_Float p1 = -2*y2-p+4.*sqrt(b0);
      E_Float p2 = -2*y2-p-4.*sqrt(b0);
      //printf("4. p1=%g p2=%g\n", p1,p2);
      if (p1 >= tol) { z1 = 0.5*(sqrt(fabs(c2))+sqrt(fabs(p1))); }
      else if (p2 >= tol) { z1 = 0.5*(sqrt(fabs(c2))+sqrt(fabs(p2))); }
    }
  }

  // nutile final
  E_Float nutilde1 = z1 + a/4.;
  aa_vec[noind] = nutilde1;
  //printf("nutilde final = %g\n", nutilde1);

}
