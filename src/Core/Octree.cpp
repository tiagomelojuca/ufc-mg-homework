#include "Octree.h"
#include "ClassificadorCSG.h"
#include <cmath>

TOctree::TOctree(int profundidadeMaxima)
    : profundidadeMaxima_(profundidadeMaxima) {}

void TOctree::ConstruirDeCSG(const TArvoreCSG &csg, const TVetor3D &min, const TVetor3D &max) {
    raiz_ = std::make_shared<TOctreeNo>(min, max);
    ConverteRecursivo(raiz_, csg, 0);
}

void TOctree::ConverteRecursivo
(
    std::shared_ptr<TOctreeNo> no,
    const TArvoreCSG &csg,
    int profundidadeAtual
) {
    TVetor3D centro = no->Centro();

    EClassificacao classeCentro = ClassificaPonto(csg, centro);

    if (profundidadeAtual >= profundidadeMaxima_) {
        if (classeCentro == EClassificacao::IN || classeCentro == EClassificacao::ON) {
        no->tipo = ETipoNoOctree::CHEIO;
        } else {
        no->tipo = ETipoNoOctree::VAZIO;
        }
        return;
    }

    TVetor3D min = no->min;
    TVetor3D max = no->max;

    TVetor3D cantos[9] = {
        centro,
        TVetor3D{min.x, min.y, min.z}, TVetor3D{max.x, min.y, min.z},
        TVetor3D{min.x, max.y, min.z}, TVetor3D{max.x, max.y, min.z},
        TVetor3D{min.x, min.y, max.z}, TVetor3D{max.x, min.y, max.z},
        TVetor3D{min.x, max.y, max.z}, TVetor3D{max.x, max.y, max.z}
    };

    bool temIn = false;
    bool temOut = false;

    // 0 => centro da celula
    // 1 a 8 => 8 vértices do cubo
    for (int i = 0; i < 9; ++i) {
        EClassificacao c = ClassificaPonto(csg, cantos[i]);
        if (c == EClassificacao::IN || c == EClassificacao::ON) temIn = true;
        if (c == EClassificacao::OUT) temOut = true;
    }

    if (temIn && !temOut) {
        no->tipo = ETipoNoOctree::CHEIO;
        return;
    }
    if (temOut && !temIn) {
        no->tipo = ETipoNoOctree::VAZIO;
        return;
    }

    no->tipo = ETipoNoOctree::MISTO;
    no->filhos.resize(8);

    TVetor3D mid = centro;

    for (int i = 0; i < 8; ++i) {
        TVetor3D subMin{(i & 1) ? mid.x : min.x,
                        (i & 2) ? mid.y : min.y,
                        (i & 4) ? mid.z : min.z};

        TVetor3D subMax{(i & 1) ? max.x : mid.x,
                        (i & 2) ? max.y : mid.y,
                        (i & 4) ? max.z : mid.z};

        no->filhos[i] = std::make_shared<TOctreeNo>(subMin, subMax);
        ConverteRecursivo(no->filhos[i], csg, profundidadeAtual + 1);
    }
    }

    double TOctree::CalcularVolume() const {
    if (!raiz_) return 0.0;
    return CalculaVolumeRecursivo(raiz_);
    }

    double TOctree::CalculaVolumeRecursivo(const std::shared_ptr<TOctreeNo> &no) const {
    if (no->tipo == ETipoNoOctree::CHEIO) {
        return no->Volume();
    }
    if (no->tipo == ETipoNoOctree::VAZIO) {
        return 0.0;
    }

    double volumeTotal = 0.0;
    for (const auto &filho : no->filhos) {
        if (filho) {
        volumeTotal += CalculaVolumeRecursivo(filho);
        }
    }
    return volumeTotal;
}