#ifndef PRIMITIVA_H_
#define PRIMITIVA_H_

//----------------------------------------------------------------------------------------------

// Os valores são os códigos usados no arquivo.
enum class ETipoPrimitiva : char
{
  BLOCO = 'B',
  ESFERA = 'E',
  CILINDRO = 'C'
};

//----------------------------------------------------------------------------------------------

class TPrimitiva
{
  public:
    virtual ~TPrimitiva() = default;

    virtual ETipoPrimitiva Tipo() const = 0;
};

//----------------------------------------------------------------------------------------------

#endif
