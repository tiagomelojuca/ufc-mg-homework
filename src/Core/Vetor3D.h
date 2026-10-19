#ifndef VETOR3D_H_
#define VETOR3D_H_

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

#endif
