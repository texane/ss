//  public domain function by Darel Rex Finley, 2006


#include <limits>
#include <algorithm>
#include <math.h>


// segment segment intersection

static bool intersect_segment
(
 double Ax, double Ay,
 double Bx, double By,
 double Cx, double Cy,
 double Dx, double Dy,
 double *X, double *Y
)
{
  double  distAB, theCos, theSin, newX, ABpos ;

  // fail if zero length
  if ((Ax==Bx && Ay==By) || (Cx==Dx && Cy==Dy))
    return false;

  // fail on endpoint sharing
  if ((Ax==Cx && Ay==Cy) ||
      (Bx==Cx && By==Cy) ||
      (Ax==Dx && Ay==Dy) ||
      (Bx==Dx && By==Dy))
    return false;

  // (1) Translate the system so that point A is on the origin.
  Bx-=Ax; By-=Ay;
  Cx-=Ax; Cy-=Ay;
  Dx-=Ax; Dy-=Ay;

  // Discover the length of segment A-B.
  distAB = ::sqrt(Bx*Bx+By*By);

  // (2) Rotate the system so that point B is on the positive X axis.
  theCos=Bx/distAB;
  theSin=By/distAB;
  newX=Cx*theCos+Cy*theSin;
  Cy  =Cy*theCos-Cx*theSin; Cx=newX;
  newX=Dx*theCos+Dy*theSin;
  Dy  =Dy*theCos-Dx*theSin; Dx=newX;

  //  Fail if segment C-D doesn't cross line A-B.
  if ((Cy<0. && Dy<0.) || (Cy>=0. && Dy>=0.)) return false;

  //  (3) Discover the position of the intersection point along line A-B.
  ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

  //  Fail if segment C-D crosses line A-B outside of segment A-B.
  if (ABpos<0. || ABpos>distAB) return false;

  //  (4) Apply the discovered position to line A-B in the original coordinate system.
  *X=Ax+ABpos*theCos;
  *Y=Ay+ABpos*theSin;

  //  Success.
  return true;
}

double intersect_segment
(double x0, double y0, double x1, double y1,
 double x2, double y2, double x3, double y3)
{
  // return the intersection distance from x0,y0

  double x, y;

  const bool is_inter =
    intersect_segment(x0, y0, x1, y1, x2, y2, x3, y3, &x, &y);
  if (is_inter == false)
    return std::numeric_limits<double>::max();

  const double dx = x - x0;
  const double dy = y - y0;

  return ::sqrt(dx * dx + dy * dy);
}


// exported

#if 0
double intersect_circle
(double px, double py, double qx, double qy, double cx, double cy, double r)
{
  double dx = qx - px;
  double dy = qy - py;

  double t = -((px - cx)*dx + (py - cy)*dy) / ((dx*dx) + (dy*dy));
  if (t < 0.0)
    t = 0.0;
  else if (t > 1.0)
    t = 1.0;

  dx = (px + t*(qx - px)) - cx;
  dy = (py + t*(qy - py)) - cy;

  const double rt = (dx*dx) + (dy*dy);
  if (rt < (r * r))
    return ::sqrt(rt);

  return std::numeric_limits<double>::max();
}
#else
double intersect_circle
(double p1x, double p1y, double p2x, double p2y, double cx, double cy, double r)
{
  double a,b,c;
  
  const double dpx = p2x - p1x;
  const double dpy = p2y - p1y;

  a = dpx * dpx + dpy * dpy;
  b = 2 * (dpx * (p1x - cx) + dpy * (p1y - cy));
  c = cx * cx + cy * cy;
  c += p1x * p1x + p1y * p1y;
  c -= 2 * (cx * p1x + cy * p1y);
  c -= r * r;

  const double bb4ac = b * b - 4 * a * c;

  if ((::fabs(a) < 0.0001) || (bb4ac < 0.f))
    return std::numeric_limits<double>::max();

  const double sqrt_bb4ac = ::sqrt(bb4ac);
  const double aa = 2.f * a;

  const double mu1 = (-b + sqrt_bb4ac) / aa;
  const double mu2 = (-b - sqrt_bb4ac) / aa;
  const double mu = std::min(mu1, mu2);

  const double dx = mu * (p2x - p1x);
  const double dy = mu * (p2y - p1y);

  return ::sqrt(dx * dx + dy * dy);
}
#endif
