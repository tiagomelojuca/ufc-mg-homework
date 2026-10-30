#ifndef OCTREE_H_
#define OCTREE_H_

#include <memory>
#include <vector>
#include "ArvoreCSG.h"
#include "Classificacao.h"
#include "Vetor3D.h"

enum class ETipoNoOctree {
  VAZIO,  // Célula fora do sólido (OUT)
  CHEIO,  // Célula dentro do sólido (IN)
  MISTO   // Célula com filhos (Subdividida)
};

class TOctreeNo {
public:
  TVetor3D min;
  TVetor3D max;
  ETipoNoOctree tipo;
  std::vector<std::shared_ptr<TOctreeNo>> filhos; // 8 octantes se MISTO

  TOctreeNo(const TVetor3D &min, const TVetor3D &max)
      : min(min), max(max), tipo(ETipoNoOctree::VAZIO) {}

  TVetor3D Centro() const {
    return TVetor3D{(min.x + max.x) * 0.5,
                    (min.y + max.y) * 0.5,
                    (min.z + max.z) * 0.5};
  }

  double Volume() const {
    double dx = max.x - min.x;
    double dy = max.y - min.y;
    double dz = max.z - min.z;
    return dx * dy * dz;
  }
};

class TOctree {
private:
  std::shared_ptr<TOctreeNo> raiz_;
  int profundidadeMaxima_;

  void ConverteRecursivo(std::shared_ptr<TOctreeNo> no,
                         const TArvoreCSG &csg,
                         int profundidadeAtual);

  double CalculaVolumeRecursivo(const std::shared_ptr<TOctreeNo> &no) const;

public:
  explicit TOctree(int profundidadeMaxima = 4);

  void ConstruirDeCSG(const TArvoreCSG &csg, const TVetor3D &min, const TVetor3D &max);
  double CalcularVolume() const;

  std::shared_ptr<TOctreeNo> Raiz() const { return raiz_; }
  int ProfundidadeMaxima() const { return profundidadeMaxima_; }
};

#endif // OCTREE_H_