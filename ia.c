/* ia.c - Inteligencia artificial do monstro (jogar cartas, atacar e bloquear). */
#include "jogo.h"

int valor_criatura(const Criatura *c) { return c->atq + c->def; }

/* escolhe e joga UMA carta. Retorna 1 se jogou algo. */
int ia_jogar_uma(void)
{
    Jogador *a = &J[1], *p = &J[0];
    int mana = mana_disp(1);
    int i, k;
    int melhorI = -1, melhorNota = 0, melhorTipo = 0, melhorAlvo = 0;

    if (!a->jogouTerra) {
        for (i = 0; i < a->nMao; i++)
            if (CARTAS[a->mao[i]].tipo == TERRA) {
                jogar_carta(1, i, 0, 0);
                return 1;
            }
    }

    for (i = 0; i < a->nMao; i++) {
        const DefCarta *c = &CARTAS[a->mao[i]];
        int nota = 0, tipo = 0, alvo = 0;
        if (c->tipo == TERRA || c->custo > mana) continue;

        if (c->tipo == CRIATURA) {
            if (a->nCampo < MAX_CAMPO) nota = 20 + c->custo * 4;
        } else if (c->efeito == EF_DANO) {
            int dano = c->valor;
            int melhorV = -1;
            if (p->vida + p->escudo <= dano) {
                nota = 500; tipo = 2;
            } else {
                for (k = 0; k < p->nCampo; k++) {
                    Criatura *q = &p->campo[k];
                    if (q->def - q->dano <= dano && valor_criatura(q) > melhorV) {
                        melhorV = valor_criatura(q);
                        alvo = k;
                    }
                }
                if (melhorV >= 3) { nota = 30 + melhorV * 3; tipo = 1; }
            }
        } else if (c->efeito == EF_CURA) {
            if (a->vida <= 20 - c->valor) nota = 45;
        } else if (c->efeito == EF_TEMPESTADE) {
            int m = 0;
            for (k = 0; k < p->nCampo; k++)
                if (p->campo[k].def - p->campo[k].dano <= c->valor) m++;
            if (m >= 2) nota = 70 + m * 5;
        } else if (c->efeito == EF_COMPRAR) {
            if (a->nMao <= 7) nota = 22;
        } else if (c->efeito == EF_REFORCAR) {
            if (a->nCampo >= 2) nota = 18 + a->nCampo * 3;
        } else if (c->efeito == EF_ESCUDO) {
            int ameaca = 0;                           /* ataque que o jogador pode lancar contra voce */
            for (k = 0; k < p->nCampo; k++) ameaca += p->campo[k].atq;
            if (a->escudo < c->valor && (ameaca >= 4 || a->vida <= 12)) nota = 30 + ameaca;
        } else if (c->efeito == EF_MURALHA) {
            if (a->nCampo >= 2) nota = 16 + a->nCampo * 2;
        }

        if (nota > melhorNota) {
            melhorNota = nota; melhorI = i; melhorTipo = tipo; melhorAlvo = alvo;
        }
    }
    if (melhorI >= 0) return jogar_carta(1, melhorI, melhorTipo, melhorAlvo);
    return 0;
}

/* a IA declara seus ataques */
void ia_atacar(void)
{
    Jogador *a = &J[1], *p = &J[0];
    int i, b, somaAtq = 0, letal, livres = 0;

    nAtac = 0;
    ladoAtac = 1;
    for (i = 0; i < a->nCampo; i++)
        if (!a->campo[i].virada && !a->campo[i].enjoada) somaAtq += a->campo[i].atq;
    letal = (somaAtq >= p->vida + p->escudo);

    for (i = 0; i < a->nCampo; i++) {
        Criatura *c = &a->campo[i];
        int seguro = 1;
        if (c->virada || c->enjoada) continue;
        for (b = 0; b < p->nCampo; b++)
            if (!p->campo[b].virada && p->campo[b].atq >= c->def - c->dano) seguro = 0;
        if (seguro || letal) {
            atacantes[nAtac++] = i;
            c->virada = 1;
        }
    }
    if (nAtac == 0) {
        iaFase = 3;
        iaTimer = 30;
        return;
    }
    for (i = 0; i < nAtac; i++) bloq[i] = -1;
    iaAtacou = 1;
    msg("O oponente ataca com %d criatura(s)!", nAtac);
    for (b = 0; b < p->nCampo; b++)
        if (!p->campo[b].virada) livres++;
    if (livres == 0) {                            /* ninguém pode bloquear */
        resolver_combate();
        if (estado != E_FIM) { iaFase = 3; iaTimer = 40; }
    } else {
        estado = E_BLOQUEIO;
        selBloq = -1;
    }
}

/* a IA escolhe os bloqueios quando VOCÊ ataca */
void ia_bloquear(void)
{
    Jogador *p = &J[0], *a = &J[1];
    int usado[MAX_CAMPO];
    int ordem[MAX_CAMPO];
    int i, k, b, dano;

    for (i = 0; i < MAX_CAMPO; i++) usado[i] = 0;
    for (i = 0; i < nAtac; i++) { bloq[i] = -1; ordem[i] = i; }
    /* atacantes mais fortes primeiro */
    for (i = 0; i < nAtac; i++)
        for (k = i + 1; k < nAtac; k++)
            if (p->campo[atacantes[ordem[k]]].atq > p->campo[atacantes[ordem[i]]].atq) {
                int t = ordem[i]; ordem[i] = ordem[k]; ordem[k] = t;
            }

    for (k = 0; k < nAtac; k++) {
        int ia = ordem[k];
        Criatura *at = &p->campo[atacantes[ia]];
        int melhor = -1, melhorNota = -1000;
        for (b = 0; b < a->nCampo; b++) {
            Criatura *bl = &a->campo[b];
            int mata, morre, nota = -1000;
            if (bl->virada || usado[b]) continue;
            mata = bl->atq >= at->def - at->dano;
            morre = at->atq >= bl->def - bl->dano;
            if (mata && !morre) nota = 200 - valor_criatura(bl);
            else if (mata && morre && valor_criatura(at) >= valor_criatura(bl)) nota = 100 - valor_criatura(bl);
            if (nota > melhorNota) { melhorNota = nota; melhor = b; }
        }
        if (melhor >= 0) { bloq[ia] = melhor; usado[melhor] = 1; }
    }

    /* se o dano restante for letal, bloqueia de qualquer jeito */
    dano = 0;
    for (i = 0; i < nAtac; i++)
        if (bloq[i] < 0) dano += p->campo[atacantes[i]].atq;
    for (k = 0; k < nAtac && dano >= a->vida + a->escudo; k++) {
        int ia = ordem[k];
        int melhor = -1;
        if (bloq[ia] >= 0) continue;
        for (b = 0; b < a->nCampo; b++) {
            if (a->campo[b].virada || usado[b]) continue;
            if (melhor < 0 || valor_criatura(&a->campo[b]) < valor_criatura(&a->campo[melhor])) melhor = b;
        }
        if (melhor >= 0) {
            bloq[ia] = melhor;
            usado[melhor] = 1;
            dano -= p->campo[atacantes[ia]].atq;
        }
    }
}

void ia_passo(void)
{
    if (iaFase == 0) {
        if (dific == 0 && rand() % 100 < 35) { iaFase = 1; iaTimer = 30; return; }
        if (ia_jogar_uma()) iaTimer = 45;
        else { iaFase = 1; iaTimer = 30; }
        return;
    }
    if (iaFase == 1) {
        iaFase = 2;
        ia_atacar();
        return;
    }
    if (iaFase == 3) terminar_turno(1);
}
