#ifndef VETOR3D_H_
#define VETOR3D_H_

#include <cmath>

//----------------------------------------------------------------------------------------------

struct TVetor3D
{
  double x;
  double y;
  double z;
};

//----------------------------------------------------------------------------------------------

inline TVetor3D operator+(
  const TVetor3D& a,
  const TVetor3D& b
)
{
  return { a.x + b.x, a.y + b.y, a.z + b.z };
}

//----------------------------------------------------------------------------------------------

inline TVetor3D operator-(
  const TVetor3D& a,
  const TVetor3D& b
)
{
  return { a.x - b.x, a.y - b.y, a.z - b.z };
}

//----------------------------------------------------------------------------------------------

inline TVetor3D operator*(
  double escalar,
  const TVetor3D& v
)
{
  return { escalar * v.x, escalar * v.y, escalar * v.z };
}

//----------------------------------------------------------------------------------------------

inline double ProdutoEscalar(
  const TVetor3D& a,
  const TVetor3D& b
)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

//----------------------------------------------------------------------------------------------

inline TVetor3D ProdutoVetorial(
  const TVetor3D& a,
  const TVetor3D& b
)
{
  return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

//----------------------------------------------------------------------------------------------

inline double Norma(
  const TVetor3D& v
)
{
  return std::sqrt(ProdutoEscalar(v, v));
}

//----------------------------------------------------------------------------------------------

inline TVetor3D Normaliza(
  const TVetor3D& v
)
{
  return (1.0 / Norma(v)) * v;
}

//----------------------------------------------------------------------------------------------

#endif
