/* regras.c - Regras e mecanicas do jogo: baralho, comprar, jogar carta, dano, escudo, combate, turnos, novo jogo. */
#include "jogo.h"

int mana_disp(int l) { return J[l].terras - J[l].manaUsada; }

/* dano em um jogador: o escudo absorve primeiro, o resto tira vida */
void dano_jogador(int l, int dano)
{
    Jogador *j = &J[l];
    float x, y;
    centro_retrato(l, &x, &y);
    if (j->escudo > 0 && dano > 0) {
        int absorvido = dano < j->escudo ? dano : j->escudo;
        j->escudo -= absorvido;
        dano -= absorvido;
        tocar(snd_escudo);
        anel(x, y, rgba(120, 190, 255, 255), 3.2f, 26);
        explodir(x, y, 12, rgba(150, 210, 255, 255), 2, 4.0f);
        flutuar(x, y - 34, rgba(140, 200, 255, 255), "Escudo -%d", absorvido);
        if (j->escudo == 0) msg("O escudo %s quebrou!", l == 0 ? "seu" : "do oponente");
    }
    if (dano > 0) {
        j->vida -= dano;
        flutuar(x, y, rgba(255, 90, 70, 255), "-%d", dano);
        explodir(x, y, 14, rgba(255, 90, 70, 255), 0, 3.2f);
        if (l == 0) sacudir(12, 4.0f);
    }
}

void embaralhar(int *a, int n)
{
    int i;
    for (i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

void montar_baralho(Jogador *j)
{
    int id, c;
    j->nBaralho = 0;
    for (id = 0; id < NUM_CARTAS; id++)
        for (c = 0; c < COPIAS[id]; c++)
            j->baralho[j->nBaralho++] = id;
    embaralhar(j->baralho, j->nBaralho);
}

void fim_de_jogo(int venc)
{
    if (estado == E_FIM) return;
    vencedor = venc;
    estado = E_FIM;
    tocar(venc == 0 ? snd_vitoria : snd_derrota);
}

void checar_vitoria(void)
{
    if (J[1].vida <= 0) fim_de_jogo(0);
    else if (J[0].vida <= 0) fim_de_jogo(1);
}

void comprar(int l)
{
    Jogador *j = &J[l];
    if (j->nBaralho <= 0) {                       /* baralho vazio = derrota */
        msg("%s ficou sem cartas!", l == 0 ? "Voc\303\252" : "O oponente");
        fim_de_jogo(1 - l);
        return;
    }
    if (j->nMao >= MAX_MAO) {
        j->nBaralho--;
        msg("%s descartou uma carta (m\303\243o cheia).", l == 0 ? "Voc\303\252" : "O oponente");
        return;
    }
    j->mao[j->nMao++] = j->baralho[--j->nBaralho];
}

void remover_da_mao(Jogador *j, int im)
{
    int i;
    for (i = im; i < j->nMao - 1; i++) j->mao[i] = j->mao[i + 1];
    j->nMao--;
}

/* remove criaturas cujo dano >= defesa */
void limpar_mortos(int l)
{
    Jogador *j = &J[l];
    int n = j->nCampo, i, k = 0;
    for (i = 0; i < n; i++) {
        Criatura *c = &j->campo[i];
        if (c->dano >= c->def) {
            float x, y;
            centro_criatura(l, i, &x, &y);
            flutuar(x, y, rgba(255, 255, 255, 255), "Destru\303\255da!");
            explodir(x, y, 18, rgba(255, 235, 190, 255), 1, 4.5f);
            msg("%s foi destru\303\255da.", CARTAS[c->id].nome);
        } else {
            j->campo[k++] = *c;
        }
    }
    j->nCampo = k;
}

/* joga uma carta da mão. alvoTipo: 0 nenhum, 1 criatura inimiga, 2 oponente.
   Retorna 1 se deu certo. Usada pelo jogador e pela IA. */
int jogar_carta(int l, int im, int alvoTipo, int alvoIdx)
{
    Jogador *j = &J[l], *o = &J[1 - l];
    int id, k;
    const DefCarta *c;
    float x, y;

    if (im < 0 || im >= j->nMao) return 0;
    id = j->mao[im];
    c = &CARTAS[id];

    if (c->tipo == TERRA) {
        if (j->jogouTerra) return 0;
        j->terraCor[j->terras++] = c->cor;
        j->jogouTerra = 1;
        remover_da_mao(j, im);
        tocar(snd_jogar);
        msg("%s jogou %s.", l == 0 ? "Voc\303\252" : "Oponente", c->nome);
        return 1;
    }

    if (c->custo > mana_disp(l)) return 0;

    if (c->tipo == CRIATURA) {
        Criatura *nc;
        if (j->nCampo >= MAX_CAMPO) return 0;
        j->manaUsada += c->custo;
        remover_da_mao(j, im);
        nc = &j->campo[j->nCampo++];
        nc->id = id; nc->atq = c->atq; nc->def = c->def;
        nc->dano = 0; nc->virada = 0; nc->sel = 0;
        nc->enjoada = c->investida ? 0 : 1;
        tocar(snd_jogar);
        msg("%s invocou %s.", l == 0 ? "Voc\303\252" : "Oponente", c->nome);
        return 1;
    }

    /* feitiços */
    if (c->efeito == EF_DANO) {
        int dano = c->valor;
        if (l == 0) dano = (int)floor(dano * pow(1.5, streak) + 0.5) + bonusDano;
        if (alvoTipo == 1) {
            if (alvoIdx < 0 || alvoIdx >= o->nCampo) return 0;
        } else if (alvoTipo != 2) {
            return 0;
        }
        j->manaUsada += c->custo;
        remover_da_mao(j, im);
        tocar(snd_feitico);
        if (alvoTipo == 1) {
            o->campo[alvoIdx].dano += dano;
            centro_criatura(1 - l, alvoIdx, &x, &y);
            flutuar(x, y, rgba(255, 90, 70, 255), "-%d", dano);
            explodir(x, y, 14, rgba(255, 140, 80, 255), 2, 4.0f);
            msg("%s usou %s em %s.", l == 0 ? "Voc\303\252" : "Oponente", c->nome, CARTAS[o->campo[alvoIdx].id].nome);
            limpar_mortos(1 - l);
        } else {
            dano_jogador(1 - l, dano);
            msg("%s usou %s: %d de dano!", l == 0 ? "Voc\303\252" : "Oponente", c->nome, dano);
        }
        checar_vitoria();
        return 1;
    }

    j->manaUsada += c->custo;
    remover_da_mao(j, im);
    if (c->efeito == EF_CURA) {
        tocar(snd_cura);
        j->vida += c->valor;
        if (j->vida > VIDA_MAXIMA) j->vida = VIDA_MAXIMA;
        centro_retrato(l, &x, &y);
        flutuar(x, y, rgba(110, 255, 140, 255), "+%d", c->valor);
        anel(x, y, rgba(110, 255, 140, 255), 2.6f, 30);
        explodir(x, y, 16, rgba(150, 255, 170, 255), 1, 3.0f);
        msg("%s recuperou %d de vida.", l == 0 ? "Voc\303\252" : "Oponente", c->valor);
    } else if (c->efeito == EF_TEMPESTADE) {
        tocar(snd_feitico);
        for (k = 0; k < o->nCampo; k++) {
            o->campo[k].dano += c->valor;
            centro_criatura(1 - l, k, &x, &y);
            flutuar(x, y, rgba(255, 90, 70, 255), "-%d", c->valor);
            explodir(x, y, 10, rgba(160, 200, 255, 255), 2, 3.5f);
        }
        msg("%s lan\303\247ou Tempestade!", l == 0 ? "Voc\303\252" : "Oponente");
        limpar_mortos(1 - l);
    } else if (c->efeito == EF_COMPRAR) {
        tocar(snd_cura);
        for (k = 0; k < c->valor; k++) comprar(l);
        msg("%s comprou %d cartas.", l == 0 ? "Voc\303\252" : "Oponente", c->valor);
    } else if (c->efeito == EF_REFORCAR) {
        tocar(snd_cura);
        for (k = 0; k < j->nCampo; k++) {
            j->campo[k].atq += c->valor;
            j->campo[k].def += c->valor;
            centro_criatura(l, k, &x, &y);
            flutuar(x, y, rgba(150, 255, 110, 255), "+%d/+%d", c->valor, c->valor);
            explodir(x, y, 8, rgba(170, 255, 130, 255), 1, 2.8f);
        }
        msg("%s refor\303\247ou suas criaturas.", l == 0 ? "Voc\303\252" : "Oponente");
    } else if (c->efeito == EF_ESCUDO) {
        tocar(snd_escudo);
        j->escudo += c->valor;
        if (j->escudo > ESCUDO_MAX) j->escudo = ESCUDO_MAX;
        centro_retrato(l, &x, &y);
        flutuar(x, y - 34, rgba(140, 200, 255, 255), "Escudo +%d", c->valor);
        anel(x, y, rgba(120, 190, 255, 255), 2.8f, 34);
        explodir(x, y, 16, rgba(160, 215, 255, 255), 1, 3.2f);
        msg("%s ergueu um escudo (%d).", l == 0 ? "Voc\303\252" : "Oponente", j->escudo);
    } else if (c->efeito == EF_MURALHA) {
        tocar(snd_escudo);
        for (k = 0; k < j->nCampo; k++) {
            j->campo[k].def += c->valor;
            centro_criatura(l, k, &x, &y);
            flutuar(x, y, rgba(140, 200, 255, 255), "+0/+%d", c->valor);
            explodir(x, y, 8, rgba(160, 215, 255, 255), 1, 2.8f);
        }
        msg("%s refor\303\247ou a defesa das criaturas.", l == 0 ? "Voc\303\252" : "Oponente");
    }
    return 1;
}

/* resolve o combate entre os atacantes declarados e seus bloqueadores */
void resolver_combate(void)
{
    Jogador *a = &J[ladoAtac], *d = &J[1 - ladoAtac];
    int i;
    float x, y;

    for (i = 0; i < nAtac; i++) {
        Criatura *at = &a->campo[atacantes[i]];
        if (bloq[i] >= 0) {
            Criatura *bl = &d->campo[bloq[i]];
            at->dano += bl->atq;
            bl->dano += at->atq;
            centro_criatura(1 - ladoAtac, bloq[i], &x, &y);
            flutuar(x, y, rgba(255, 90, 70, 255), "-%d", at->atq);
            explodir(x, y, 10, rgba(255, 140, 80, 255), 2, 3.5f);
            centro_criatura(ladoAtac, atacantes[i], &x, &y);
            flutuar(x, y, rgba(255, 90, 70, 255), "-%d", bl->atq);
            explodir(x, y, 10, rgba(255, 140, 80, 255), 2, 3.5f);
            msg("%s e %s lutam!", CARTAS[at->id].nome, CARTAS[bl->id].nome);
        } else {
            dano_jogador(1 - ladoAtac, at->atq);
            msg("%s causa %d de dano!", CARTAS[at->id].nome, at->atq);
        }
        tocar(snd_golpe);
    }
    limpar_mortos(0);
    limpar_mortos(1);
    nAtac = 0;
    checar_vitoria();
}

void iniciar_turno(int l)
{
    Jogador *j = &J[l];
    int i;
    turnoDe = l;
    numTurno++;
    for (i = 0; i < j->nCampo; i++) {
        j->campo[i].virada = 0;
        j->campo[i].enjoada = 0;
        j->campo[i].sel = 0;
    }
    j->manaUsada = 0;
    j->jogouTerra = 0;
    jaAtacou = 0;
    if (l == 1) iaAtacou = 0;
    if (l == 0 && numTurno > 1) {                 /* combo: inimigo passivo = dano cresce */
        streak = iaAtacou ? 0 : streak + 1;
        if (streak > 4) streak = 4;
    }
    if (!(l == 0 && numTurno == 1)) comprar(l);   /* quem começa não compra no 1º turno */
    if (estado == E_FIM) return;
    if (l == 0) {
        estado = E_JOGADOR;
        msg("--- Seu turno ---");
    } else {
        estado = E_IA;
        iaFase = 0;
        iaTimer = 50;
        msg("--- Turno do oponente ---");
    }
}

void terminar_turno(int l)
{
    int i, k;
    for (k = 0; k < 2; k++)                       /* o dano some no fim do turno */
        for (i = 0; i < J[k].nCampo; i++) {
            J[k].campo[i].dano = 0;
            J[k].campo[i].sel = 0;
        }
    iniciar_turno(1 - l);
}

void novo_jogo(void)
{
    int l, i, terras;
    estado = E_MENU;                              /* garante que a partida anterior acabou */
    memset(J, 0, sizeof(J));
    memset(linhasLog, 0, sizeof(linhasLog));
    memset(flut, 0, sizeof(flut));
    for (l = 0; l < 2; l++) {
        do {                                      /* mão inicial com 2 a 5 terras */
            J[l].nMao = 0;
            montar_baralho(&J[l]);
            terras = 0;
            for (i = 0; i < 7; i++) {
                int id = J[l].baralho[--J[l].nBaralho];
                J[l].mao[J[l].nMao++] = id;
                if (CARTAS[id].tipo == TERRA) terras++;
            }
        } while (terras < 2 || terras > 5);
        J[l].vida = VIDA_INICIAL;
    }
    J[0].vida = vidaJog;
    J[1].vida = VIDA_INI_INIMIGO[dific] + 3 * luta;
    streak = 0;
    iaAtacou = 0;
    numTurno = 0;
    vencedor = -1;
    nAtac = 0;
    selBloq = -1;
    maoAlvo = -1;
    iniciar_turno(0);
}
