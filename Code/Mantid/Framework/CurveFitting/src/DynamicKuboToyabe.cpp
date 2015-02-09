//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidCurveFitting/DynamicKuboToyabe.h"
#include "MantidAPI/Jacobian.h"
#include "MantidAPI/FunctionFactory.h"
#include <vector>

#define EPS 1e-6
#define JMAX 14
#define JMAXP (JMAX+1)
#define K 5
#define NR_END 1
#define FREE_ARG char*

namespace Mantid
{
namespace CurveFitting
{

using namespace Kernel;
using namespace API;

DECLARE_FUNCTION(DynamicKuboToyabe)

// ** MODIFY THIS **
// Here specify/declare the parameters of your Fit Function
// 
// declareParameter takes three arguments:
//
//   1st: The name of the parameter
//   2nd: The default (initial) value of the parameter
//   3rd: A description of the parameter (optional)
//
void DynamicKuboToyabe::init()
{
  declareParameter("Asym",  0.2, "Amplitude at time 0");
  declareParameter("Delta", 0.2, "Local field");
  declareParameter("Field", 0.0, "External field");
  declareParameter("Nu",    0.0, "Hopping rate");
}


//------------------------------------------------------------------------------------------------
// Routines from Numerical Recipes

double midpnt(double func(const double, const double, const double),
	const double a, const double b, const int n, const double g, const double w0) {
// quote & modified from numerical recipe 2nd edtion (page147)	
	
  static double s;

	if (n==1) {
    s = (b-a)*func(0.5*(a+b),g,w0);
    return (s);
	} else {
		double x, tnm, sum, del, ddel;
		int it, j;
		for (it=1,j=1;j<n-1;j++) it *= 3;
		tnm = it;
		del = (b-a)/(3*tnm);
		ddel=del+del;
		x = a+0.5*del;
		sum =0.0;
		for (j=1;j<=it;j++) {
			sum += func(x,g,w0);
			x += ddel;
			sum += func(x,g,w0);
			x += del;
		}
		s=(s+(b-a)*sum/tnm)/3.0;
		return s;
	}
}

void polint (double xa[], double ya[], int n, double x, double& y, double& dy) {
	int i, m, ns = 1;
  double dif;

  dif = fabs(x-xa[1]);
  std::vector<double> c(n+1);
  std::vector<double> d(n+1);
	for (i=1;i<=n;i++){
    double dift;
		if((dift=fabs(x-xa[i]))<dif) {
			ns=i;
			dif=dift;
		}
    c[i]=ya[i];
		d[i]=ya[i];
	}
	y=ya[ns--];
	for (m=1;m<n;m++) {
		for (i=1;i<=n-m;i++) {
      double den, ho, hp, w;
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if((den=ho-hp)==0.0) //error message!!!
        throw std::runtime_error("Error in routine polint\n");
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		y += (dy=(2*(ns)<(n-m) ? c[ns+1] : d[ns--]));

	}

}


double integrate (double func(const double, const double, const double),
				const double a, const double b, const double g, const double w0) {
	int j;
	double ss,dss;
	double h[JMAXP+1], s[JMAXP];

	h[1] = 1.0;
	for (j=1; j<= JMAX; j++) {
		s[j]=midpnt(func,a,b,j,g,w0);
		if (j >= K) {
			polint(&h[j-K],&s[j-K],K,0.0,ss,dss);
			if (fabs(dss) <= fabs(ss)) return ss;
		}
		h[j+1]=h[j]/9.0;
	}
  throw std::runtime_error("integrate(): Too many steps in routine integrate\n");
	return 0.0;
}

//--------------------------------------------------------------------------------------------------------------------------------------


double f1(const double x, const double G, const double w0) {

  return( exp(-G*G*x*x/2)*sin(w0*x));
}

// Zero Field Kubo Toyabe relaxation function
double ZFKT (const double x, const double G){

  const double q = G*G*x*x;
  return (0.3333333333 + 0.6666666667*exp(-0.5*q)*(1-q));
}

// Non-Zero field Kubo Toyabe relaxation function
double HKT (const double x, const double G, const double F) 
{
  const double w0 = 2.0*3.1415926536*0.01355342*F;
  const double q = G*G*x*x;
  const double p = G*G/(w0*w0);
  double hkt = 1.0-2.0*p*(1-exp(-q/2.0)*cos(w0*x))+2.0*p*p*w0*integrate(f1,0.0,x,G,w0);
  return hkt;
}

// Dynamic Kubo Toyabe function
void DynamicKuboToyabe::function1D(double* out, const double* xValues, const size_t nData)const
{
  const double& A = getParameter("Asym");
  const double& G = fabs(getParameter("Delta"));
  const double& F = fabs(getParameter("Field"));
  const double& v = fabs(getParameter("Nu"));


  // Zero hopping rate
	if (v == 0.0) {

    // Zero external field
    if ( F == 0.0 ){
      for (size_t i = 0; i < nData; i++) {
        out[i] = A*ZFKT(xValues[i],G);
      }
    }
    // Non-zero external field
    else{
      for (size_t i = 0; i < nData; i++) {
        out[i] = A*HKT(xValues[i],G,F);
      }
    }
	} 

  // Non-zero hopping rate
	else {

    // Make sure stepsize is smaller than spacing between xValues
    //int n = 1000;
    //while (n<nData) {
    //   n=n*2; 
    //}
    const double stepsize = (xValues[1]-xValues[0])/4.;
    int n=int(ceil((xValues[nData-1])/stepsize))+1;

    std::vector<double> funcG(n);

    double efac1=exp(-v*stepsize); // survival prob for step
    double efac2=(1.0-efac1);      // hop prob ~ hoprate*step

    // Mark's implementation
    if ( F == 0.0 ){
    // Zero field

      for (int i = 0; i < n; i++) {

        funcG[0]=ZFKT(0,G);
        funcG[1]=ZFKT(stepsize,G);

        for(i=1; i<n; i++) {
          double gdi=ZFKT(i*stepsize,G);
          for(int j=1; j<i; j++) {
            gdi= gdi*efac1 + funcG[j]*ZFKT((i-j)*stepsize,G)*efac2;
          }
          funcG[i]=gdi;
        }
      }
    } else {

      // Non-zero field

      for (int i = 0; i < n; i++) {

        funcG[0]=HKT(0,G,F);
        funcG[1]=HKT(stepsize,G,F);

        for(i=1; i<n; i++) {
          double gdi=HKT(i*stepsize,G,F);
          for(int j=1; j<i; j++) {
            gdi= gdi*efac1 + funcG[j]*HKT((i-j)*stepsize,G,F)*efac2;
          }
          funcG[i]=gdi;
        }
      }
    
    }

  //  //for (int i = 0; i < n; i++) {
  //  //  double Integral=0.0;
  //  //  for (int c = 1; c <= i; c++) {
  //  //    Integral= gz(c*stepsize,G,F)*exp(-v*c*stepsize)*funcG[i-c]*(stepsize) + Integral; 
  //  //  }
  //  //  funcG[i] = (gz(i*stepsize,G,F)*exp(-v*i*stepsize) + v*Integral);
  //  //}

		for (size_t i = 0; i < nData; i++) {

      if (xValues[i]<0){
        out[i] = A;

      } else {

        if(xValues[i]<stepsize*(n-1)) {

          int j=int(floor(xValues[i]/stepsize));
          double fdiv=(xValues[i]-j*stepsize)/stepsize;
          out[i]=A*(funcG[j]*(1-fdiv)+funcG[j+1]*fdiv);
        } else {

          // off end, approximate exponential
          double fdiv=(xValues[i]-(stepsize*(n-1)))/stepsize;
          double efac1=funcG[n-1]/funcG[n-2];
          out[i]=A*funcG[n-1]*pow(efac1,fdiv);
        }
      }
		}


   } // else hopping rate != 0

}



void DynamicKuboToyabe::functionDeriv(const API::FunctionDomain& domain, API::Jacobian& jacobian)
{
  calNumericalDeriv(domain, jacobian);
}

void DynamicKuboToyabe::functionDeriv1D(API::Jacobian* , const double* , const size_t )
{
  throw Mantid::Kernel::Exception::NotImplementedError("functionDerivLocal is not implemented for DynamicKuboToyabe.");
}

void DynamicKuboToyabe::setActiveParameter(size_t i, double value) {

  setParameter( i, fabs(value), false);

}

} // namespace CurveFitting
} // namespace Mantid
