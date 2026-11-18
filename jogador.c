/* jogador.c - Acoes do jogador: clicar numa carta da mao, atacar, botao principal. */
#include "jogo.h"

int contar_sel(void)
{
    int i, n = 0;
    for (i = 0; i < J[0].nCampo; i++) if (J[0].campo[i].sel) n++;
    return n;
}

void declarar_ataque_humano(void)
{
    int i;
    nAtac = 0;
    ladoAtac = 0;
    for (i = 0; i < J[0].nCampo; i++) {
        Criatura *c = &J[0].campo[i];
        if (c->sel) {
            atacantes[nAtac++] = i;
            c->virada = 1;
            c->sel = 0;
        }
    }
    jaAtacou = 1;
    if (nAtac == 0) return;
    msg("Voc\303\252 ataca com %d criatura(s)!", nAtac);
    ia_bloquear();
    resolver_combate();
    if (estado != E_FIM) estado = E_JOGADOR;
}

int precisa_alvo(int id)
{
    return CARTAS[id].efeito == EF_DANO;
}

void clicar_mao(int im)
{
    int id = J[0].mao[im];
    const DefCarta *c = &CARTAS[id];
    float x = pos_mao_x(J[0].nMao, im) + MW / 2.0f;

    if (c->tipo == TERRA) {
        if (!jogar_carta(0, im, 0, 0)) flutuar(x, MAO_Y - 20, rgba(255, 220, 120, 255), "S\303\263 1 terra por turno");
        return;
    }
    if (c->custo > mana_disp(0)) {
        flutuar(x, MAO_Y - 20, rgba(255, 120, 120, 255), "Mana insuficiente");
        return;
    }
    if (c->tipo == CRIATURA && J[0].nCampo >= MAX_CAMPO) {
        flutuar(x, MAO_Y - 20, rgba(255, 120, 120, 255), "Campo cheio");
        return;
    }
    if (precisa_alvo(id)) {
        maoAlvo = im;
        estado = E_ALVO;
        tocar(snd_clique);
        return;
    }
    iniciar_quiz(im, 0, 0);
}

void acao_principal(void)
{
    if (estado == E_JOGADOR) {
        if (!jaAtacou && contar_sel() > 0) declarar_ataque_humano();
        else terminar_turno(0);
    } else if (estado == E_ALVO) {
        estado = E_JOGADOR;
    } else if (estado == E_BLOQUEIO) {
        resolver_combate();
        if (estado != E_FIM) { estado = E_IA; iaFase = 3; iaTimer = 40; }
    }
    tocar(snd_clique);
}
