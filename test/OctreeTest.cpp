#include <gtest/gtest.h>
#include <memory>
#include <cmath>

#include "Core/ArvoreCSG.h"
#include "Core/Esfera.h"
#include "Core/Bloco.h"
#include "Core/Octree.h"

namespace {
  constexpr double PI = 3.14159265358979323846;

  TArvoreCSG CriaEsfera(double raio) {
    return CriaPrimitiva(std::make_shared<TEsfera>(raio));
  }

  TArvoreCSG CriaBloco(double lado) {
    return CriaPrimitiva(std::make_shared<TBloco>(lado, lado, lado));
  }
}

TEST(OctreeTest, deve_calcular_volume_aproximado_da_esfera) {
  const double raio = 1.0;
  const TArvoreCSG esfera = CriaEsfera(raio);

  TOctree octree(5);
  octree.ConstruirDeCSG(esfera, {-1.5, -1.5, -1.5}, {1.5, 1.5, 1.5});

  const double volCalculado = octree.CalcularVolume();
  const double volExato = (4.0 / 3.0) * PI * std::pow(raio, 3);

  EXPECT_NEAR(volCalculado, volExato, 0.5);
}

TEST(OctreeTest, deve_calcular_volume_exato_do_bloco) {
  const double lado = 2.0;
  const TArvoreCSG bloco = CriaBloco(lado);

  TOctree octree(4);
  octree.ConstruirDeCSG(bloco, {-2.0, -2.0, -2.0}, {2.0, 2.0, 2.0});

  const double volCalculado = octree.CalcularVolume();
  const double volExato = std::pow(lado, 3);

  EXPECT_NEAR(volCalculado, volExato, 1e-5);
}