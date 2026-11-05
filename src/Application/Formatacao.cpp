#include "Formatacao.h"

#include <locale>
#include <sstream>

//----------------------------------------------------------------------------------------------

std::string FormataNumero(
  double valor
)
{
  std::ostringstream fluxo;
  fluxo.imbue(std::locale::classic());
  fluxo << valor;
  return fluxo.str();
}

//----------------------------------------------------------------------------------------------

std::string FormataVetor(
  const TVetor3D& valores
)
{
  return
    "(" + FormataNumero(valores.x) + ", " + FormataNumero(valores.y) + ", " +
    FormataNumero(valores.z) + ")";
}

//----------------------------------------------------------------------------------------------
