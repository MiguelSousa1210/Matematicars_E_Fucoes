/* tela_jogo.c - Desenho da tela de batalha: cartas, paineis, mana, log, fim de luta. */
#include "jogo.h"

void atualizar_escala(void)
{
    int w = al_get_display_width(disp), h = al_get_display_height(disp);
    float ex = w / (float)LARG, ey = h / (float)ALT;
    escala = (ex < ey) ? ex : ey;
    offx = (w - LARG * escala) / 2;
    offy = (h - ALT * escala) / 2;
}

void desenhar_botao(Ret r, const char *txt, int ativo)
{
    int sobre = ativo && dentro(r, mx, my);
    ALLEGRO_COLOR fundo = !ativo ? rgba(60, 55, 70, 230) : (sobre ? rgba(235, 170, 60, 255) : rgba(150, 90, 30, 255));
    al_draw_filled_rounded_rectangle(r.x, r.y, r.x + r.w, r.y + r.h, 12, 12, fundo);
    al_draw_rounded_rectangle(r.x, r.y, r.x + r.w, r.y + r.h, 12, 12, rgba(255, 225, 140, 255), 3);
    al_draw_text(f_n, ativo ? al_map_rgb(255, 255, 255) : al_map_rgb(170, 170, 180),
                 r.x + r.w / 2, r.y + r.h / 2 - al_get_font_line_height(f_n) / 2.0f, ALLEGRO_ALIGN_CENTRE, txt);
}

/* desenha uma carta completa (moldura + arte + textos) em qualquer tamanho */
void desenhar_carta(int id, float x, float y, float esc, int atq, int def, int dano)
{
    const DefCarta *c = &CARTAS[id];
    ALLEGRO_TRANSFORM t, antiga;
    char buf[64];

    al_copy_transform(&antiga, al_get_current_transform());
    al_identity_transform(&t);
    al_scale_transform(&t, esc, esc);
    al_translate_transform(&t, x, y);
    al_compose_transform(&t, &antiga);
    al_use_transform(&t);

    al_draw_scaled_bitmap(img_arte[id], 0, 0, al_get_bitmap_width(img_arte[id]), al_get_bitmap_height(img_arte[id]),
                          14, 40, 192, 128, 0);
    al_draw_bitmap(img_moldura[c->cor], 0, 0, 0);
    al_draw_text(f_cnome, al_map_rgb(255, 250, 235), 15, 11, 0, c->nome);

    if (c->tipo != TERRA) {                       /* círculo de custo de mana */
        al_draw_filled_circle(194, 22, 15, al_map_rgb(30, 70, 170));
        al_draw_circle(194, 22, 15, al_map_rgb(240, 210, 120), 2);
        snprintf(buf, sizeof(buf), "%d", c->custo);
        al_draw_text(f_cnum, al_map_rgb(255, 255, 255), 194, 22 - al_get_font_line_height(f_cnum) / 2.0f, ALLEGRO_ALIGN_CENTRE, buf);
    }

    snprintf(buf, sizeof(buf), "%s - %s", NOME_TIPO[c->tipo], NOME_COR[c->cor]);
    if (c->papel != PAPEL_NENHUM) {               /* selo ATAQUE / DEFESA / APOIO */
        float tw = al_get_text_width(f_ctexto, NOME_PAPEL[c->papel]) + 10;
        float x1 = 207, x0 = x1 - tw;
        ALLEGRO_COLOR cp = (c->papel == PAPEL_ATAQUE) ? al_map_rgb(190, 50, 40)
                         : (c->papel == PAPEL_DEFESA) ? al_map_rgb(40, 100, 190) : al_map_rgb(60, 150, 70);
        if (15 + al_get_text_width(f_ctexto, buf) > x0 - 4)       /* sem espaco: so o tipo */
            snprintf(buf, sizeof(buf), "%s", NOME_TIPO[c->tipo]);
        al_draw_filled_rounded_rectangle(x0, 174, x1, 190, 6, 6, cp);
        al_draw_rounded_rectangle(x0, 174, x1, 190, 6, 6, al_map_rgb(255, 235, 170), 1.5f);
        al_draw_text(f_ctexto, al_map_rgb(255, 255, 255), (x0 + x1) / 2, 173, ALLEGRO_ALIGN_CENTRE, NOME_PAPEL[c->papel]);
    }
    al_draw_text(f_ctexto, al_map_rgb(255, 245, 215), 15, 173, 0, buf);
    al_draw_multiline_text(f_ctexto, al_map_rgb(45, 30, 20), 15, 199, 188, 16, 0, c->texto);

    if (c->tipo == CRIATURA) {
        ALLEGRO_COLOR cs = al_map_rgb(255, 255, 255);
        al_draw_filled_rounded_rectangle(146, 272, 212, 302, 8, 8, al_map_rgb(22, 18, 30));
        al_draw_rounded_rectangle(146, 272, 212, 302, 8, 8, al_map_rgb(240, 210, 120), 2);
        if (dano > 0) cs = al_map_rgb(255, 100, 90);
        else if (atq > c->atq || def > c->def) cs = al_map_rgb(140, 255, 140);
        snprintf(buf, sizeof(buf), "%d/%d", atq, def - dano);
        al_draw_text(f_cnum, cs, 179, 287 - al_get_font_line_height(f_cnum) / 2.0f, ALLEGRO_ALIGN_CENTRE, buf);
    }
    al_use_transform(&antiga);
}

void contorno(float x, float y, float w, float h, ALLEGRO_COLOR c, float esp)
{
    al_draw_rounded_rectangle(x, y, x + w, y + h, 8, 8, c, esp);
}

int eh_atacante(int i)
{
    int k;
    for (k = 0; k < nAtac; k++) if (atacantes[k] == i) return 1;
    return 0;
}

void desenhar_criatura(int lado, int i)
{
    Jogador *j = &J[lado];
    Criatura *c = &j->campo[i];
    float x = pos_campo_x(j->nCampo, i), y = pos_campo_y(lado);
    float pulso = 0.5f + 0.5f * sinf(tempo * 0.12f);

    if (lado == 0 && c->sel) y -= 18;
    if (estado == E_BLOQUEIO && lado == 1 && eh_atacante(i)) y += 22;
    if (estado == E_BLOQUEIO && lado == 0 && selBloq == i) y -= 14;

    desenhar_carta(c->id, x, y, CW / (float)CARTA_W, c->atq, c->def, c->dano);

    if (c->virada)
        al_draw_filled_rounded_rectangle(x, y, x + CW, y + CH, 8, 8, rgba(0, 0, 0, 120));
    if (c->enjoada && lado == 0)
        al_draw_text(f_p, al_map_rgb(255, 255, 255), x + CW / 2, y + 62, ALLEGRO_ALIGN_CENTRE, "zZz");

    if (lado == 0 && c->sel) contorno(x - 1, y - 1, CW + 2, CH + 2, al_map_rgb(255, 215, 60), 4);
    if (estado == E_BLOQUEIO && lado == 1 && eh_atacante(i)) contorno(x - 1, y - 1, CW + 2, CH + 2, al_map_rgb(255, 70, 60), 4);
    if (estado == E_BLOQUEIO && lado == 0 && selBloq == i) contorno(x - 1, y - 1, CW + 2, CH + 2, al_map_rgb(90, 180, 255), 4);
    if (estado == E_ALVO && lado == 1)
        contorno(x - 1, y - 1, CW + 2, CH + 2, rgba(120, 255, 140, (int)(120 + 135 * pulso)), 3);
    if (estado == E_JOGADOR && lado == 0 && !jaAtacou && !c->virada && !c->enjoada && !c->sel && turnoDe == 0)
        contorno(x, y, CW, CH, rgba(255, 255, 255, 50), 2);
}

/* cor de cada tipo de terra */
ALLEGRO_COLOR cor_mana(int cor)
{
    if (cor == FOGO) return al_map_rgb(235, 90, 40);
    if (cor == AGUA) return al_map_rgb(60, 150, 235);
    if (cor == FLORESTA) return al_map_rgb(70, 190, 80);
    return al_map_rgb(240, 220, 120);
}

void desenhar_painel_jogador(int lado)
{
    Jogador *j = &J[lado];
    Ret r = (lado == 0) ? R_RET_JOG : R_RET_OPP;
    float pulso = 0.5f + 0.5f * sinf(tempo * 0.12f);
    char buf[64];
    int i;
    float ly = (lado == 0) ? 488.0f : 66.0f;   /* mana do jogador fica abaixo do painel azul */

    /* retrato */
    {
        ALLEGRO_BITMAP *rb = (lado == 1 && img_monstro[luta % 5]) ? img_monstro[luta % 5] : img_ret[lado];
        al_draw_scaled_bitmap(rb, 0, 0, al_get_bitmap_width(rb), al_get_bitmap_height(rb),
                              r.x, r.y, r.w, r.h, 0);
    }
    if (turnoDe == lado && estado != E_FIM)
        al_draw_circle(r.x + r.w / 2, r.y + r.h / 2, r.w / 2 + 3, rgba(255, 215, 80, 255), 3);
    if (estado == E_ALVO && lado == 1)
        al_draw_circle(r.x + r.w / 2, r.y + r.h / 2, r.w / 2 + 6, rgba(120, 255, 140, (int)(120 + 135 * pulso)), 4);

    al_draw_text(f_p, al_map_rgb(235, 225, 255), r.x, r.y - 22, 0, lado == 0 ? "Voc\303\252" : NOME_MONSTRO[luta % 5]);
    al_draw_bitmap(img_vida, r.x, r.y + r.h + 8, 0);
    snprintf(buf, sizeof(buf), "%d", j->vida);
    al_draw_text(f_g, al_map_rgb(255, 255, 255), r.x + 40, r.y + r.h + 4, 0, buf);
    snprintf(buf, sizeof(buf), "M\303\243o %d  Baralho %d", j->nMao, j->nBaralho);
    al_draw_text(f_p, al_map_rgb(200, 195, 220), r.x, r.y + r.h + 46, 0, buf);

    if (j->escudo > 0) {                          /* escudo ativo: anel azul + contador */
        float cx = r.x + r.w / 2, cy = r.y + r.h / 2, sx = r.x + 10, sy = r.y + r.h + 72;
        al_draw_circle(cx, cy, r.w / 2 + 8, rgba(120, 190, 255, (int)(150 + 90 * pulso)), 4);
        al_draw_filled_triangle(sx - 9, sy - 9, sx + 9, sy - 9, sx, sy + 11, rgba(90, 160, 235, 255));
        al_draw_filled_rectangle(sx - 9, sy - 9, sx + 9, sy, rgba(90, 160, 235, 255));
        al_draw_rectangle(sx - 9, sy - 9, sx + 9, sy, al_map_rgb(220, 240, 255), 1.5f);
        snprintf(buf, sizeof(buf), "Escudo %d", j->escudo);
        al_draw_text(f_p, al_map_rgb(170, 215, 255), sx + 16, sy - 9, 0, buf);
    }

    /* fileira de mana */
    al_draw_bitmap(img_mana, 250, ly - 4, 0);
    snprintf(buf, sizeof(buf), "Mana %d/%d", mana_disp(lado), j->terras);
    al_draw_text(f_p, al_map_rgb(255, 255, 255), 286, ly, 0, buf);
    for (i = 0; i < j->terras; i++) {
        ALLEGRO_COLOR cc = cor_mana(j->terraCor[i]);
        float cx = 410 + i * 24.0f;
        if (i < j->manaUsada) cc = al_map_rgb(70, 70, 80);
        al_draw_filled_circle(cx, ly + 9, 9, cc);
        al_draw_circle(cx, ly + 9, 9, al_map_rgb(255, 235, 170), 1.5f);
    }
}

/* painel de mensagens: quebra o texto por palavras (nenhuma palavra fica cortada) */
void desenhar_log(void)
{
    char linhas[8][260];
    int idm[8], nl = 0, i, k;
    const int MAXL = 6;
    for (i = 5; i >= 0 && nl < MAXL; i--) {          /* da mensagem mais nova para a mais antiga */
        char tmp[6][260], atual[260];
        int nt = 0;
        const char *p = linhasLog[i];
        atual[0] = 0;
        if (!p[0]) continue;
        while (*p) {
            char pal[100], teste[260];
            int n = 0;
            while (*p == ' ') p++;
            while (*p && *p != ' ' && n < 98) pal[n++] = *p++;
            pal[n] = 0;
            if (n == 0) break;
            snprintf(teste, sizeof(teste), "%s%s%s", atual, atual[0] ? " " : "", pal);
            if (atual[0] && al_get_text_width(f_p, teste) > 262) {
                if (nt < 6) snprintf(tmp[nt++], sizeof(tmp[0]), "%s", atual);
                snprintf(atual, sizeof(atual), "%s", pal);
            } else {
                snprintf(atual, sizeof(atual), "%s", teste);
            }
        }
        if (atual[0] && nt < 6) snprintf(tmp[nt++], sizeof(tmp[0]), "%s", atual);
        if (nl + nt > MAXL) break;                   /* nao cabe inteira: nao mostra pela metade */
        for (k = nt - 1; k >= 0; k--) { strcpy(linhas[nl], tmp[k]); idm[nl] = i; nl++; }
    }
    for (k = 0; k < nl; k++) {                       /* linhas[0] e a de baixo */
        int a = 90 + idm[k] * 33;
        al_draw_text(f_p, rgba(240, 235, 255, a > 255 ? 255 : a), 995, 20 + (nl - 1 - k) * 21, 0, linhas[k]);
    }
}

void desenhar_jogo(void)
{
    int i, k, hoverId = -1, hAtq = 0, hDef = 0, hDano = 0, hm;
    char buf[80];
    const char *dica = "";
    const char *txtBotao = "FIM DO TURNO";
    int botaoAtivo = 1;
    float pulso = 0.5f + 0.5f * sinf(tempo * 0.12f);

    al_draw_bitmap(img_fundo, 0, 0, 0);

    /* mão do oponente (costas das cartas, saindo pelo topo) */
    for (i = 0; i < J[1].nMao; i++) {
        float cx = 545 - (J[1].nMao * 40) / 2.0f + i * 40.0f;
        al_draw_scaled_bitmap(img_verso, 0, 0, CARTA_W, CARTA_H, cx, -58, 77, 108, 0);
    }

    desenhar_painel_jogador(1);
    desenhar_painel_jogador(0);

    /* campos de batalha */
    for (i = 0; i < J[1].nCampo; i++) desenhar_criatura(1, i);
    for (i = 0; i < J[0].nCampo; i++) desenhar_criatura(0, i);

    /* linhas de bloqueio */
    if (estado == E_BLOQUEIO) {
        for (k = 0; k < nAtac; k++) {
            if (bloq[k] >= 0) {
                float x1, y1, x2, y2;
                centro_criatura(1, atacantes[k], &x1, &y1);
                centro_criatura(0, bloq[k], &x2, &y2);
                al_draw_line(x1, y1 + 22, x2, y2, rgba(90, 180, 255, 230), 4);
                al_draw_filled_circle(x2, y2, 7, rgba(90, 180, 255, 255));
            }
        }
    }

    /* mão do jogador */
    hm = (estado == E_JOGADOR || estado == E_ALVO) ? idx_mao_em(mx, my) : -1;
    if (estado == E_BLOQUEIO || estado == E_IA) hm = idx_mao_em(mx, my);
    for (i = 0; i < J[0].nMao; i++) {
        int id = J[0].mao[i];
        const DefCarta *c = &CARTAS[id];
        float x = pos_mao_x(J[0].nMao, i), y = MAO_Y;
        int jogavel = (turnoDe == 0 && (estado == E_JOGADOR)) &&
                      ((c->tipo == TERRA && !J[0].jogouTerra) || (c->tipo != TERRA && c->custo <= mana_disp(0)));
        if (i == hm) y -= 30;
        if (estado == E_ALVO && i == maoAlvo) y -= 30;
        desenhar_carta(id, x, y, MW / (float)CARTA_W, c->atq, c->def, 0);
        if (!jogavel && !(estado == E_ALVO && i == maoAlvo))
            al_draw_filled_rounded_rectangle(x, y, x + MW, y + MH, 8, 8, rgba(0, 0, 0, 90));
        if (jogavel) contorno(x, y, MW, MH, rgba(120, 255, 140, (int)(110 + 100 * pulso)), 2);
        if (estado == E_ALVO && i == maoAlvo) contorno(x, y, MW, MH, al_map_rgb(255, 215, 60), 4);
    }

    /* carta sob o mouse (mão ou campo) -> prévia grande */
    if (hm >= 0) {
        hoverId = J[0].mao[hm];
        hAtq = CARTAS[hoverId].atq; hDef = CARTAS[hoverId].def;
    } else {
        int lado;
        for (lado = 0; lado < 2 && hoverId < 0; lado++) {
            int ic = idx_campo_em(lado, mx, my);
            if (ic >= 0) {
                Criatura *c = &J[lado].campo[ic];
                hoverId = c->id; hAtq = c->atq; hDef = c->def; hDano = c->dano;
            }
        }
    }

    /* painel direito */
    al_draw_filled_rounded_rectangle(985, 12, 1268, 152, 10, 10, rgba(8, 6, 20, 190));
    al_draw_rounded_rectangle(985, 12, 1268, 152, 10, 10, rgba(200, 160, 90, 200), 2);
    desenhar_log();
    if (hoverId >= 0) desenhar_carta(hoverId, 1026, 165, 0.9f, hAtq, hDef, hDano);
    else {
        al_draw_filled_rounded_rectangle(1026, 165, 1224, 442, 10, 10, rgba(8, 6, 20, 120));
        al_draw_multiline_text(f_p, rgba(200, 190, 230, 200), 1125, 270, 170, 20, ALLEGRO_ALIGN_CENTRE,
                               "Passe o mouse sobre uma carta para ver os detalhes.");
    }

    snprintf(buf, sizeof(buf), "Turno %d - %s", numTurno, turnoDe == 0 ? "Sua vez" : "Vez do oponente");
    al_draw_text(f_n, al_map_rgb(255, 225, 140), 1126, 452, ALLEGRO_ALIGN_CENTRE, buf);

    switch (estado) {
    case E_JOGADOR:
        if (!jaAtacou && contar_sel() > 0) { snprintf(buf, sizeof(buf), "ATACAR (%d)", contar_sel()); txtBotao = buf; }
        else txtBotao = "FIM DO TURNO";
        dica = jaAtacou ? "Voc\303\252 pode jogar mais cartas ou encerrar o turno."
                        : "Clique numa carta da m\303\243o para jogar. Clique nas suas criaturas para atacar.";
        break;
    case E_ALVO:
        txtBotao = "CANCELAR";
        dica = "Escolha o alvo: uma criatura inimiga ou o oponente. (Bot\303\243o direito cancela)";
        break;
    case E_BLOQUEIO:
        txtBotao = "CONFIRMAR BLOQUEIOS";
        dica = "Clique numa criatura sua e depois no atacante (vermelho) que ela vai bloquear.";
        break;
    case E_IA:
        txtBotao = "Oponente jogando...";
        botaoAtivo = 0;
        dica = "Aguarde o turno do oponente.";
        break;
    default: break;
    }
    al_draw_multiline_text(f_p, al_map_rgb(215, 205, 240), 1000, 486, 255, 19, 0, dica);
    desenhar_botao(R_PRINC, txtBotao, botaoAtivo && estado != E_FIM);

    /* alvo: cursor de mira */
    if (estado == E_ALVO) al_draw_circle(mx, my, 14 + 3 * pulso, al_map_rgb(120, 255, 140), 2);

    /* textos flutuantes */
    for (i = 0; i < 40; i++) {
        if (flut[i].t > 0) {
            float a = flut[i].t / 70.0f;
            ALLEGRO_COLOR c = flut[i].cor;
            c.r *= a; c.g *= a; c.b *= a; c.a *= a;
            al_draw_text(f_g, al_map_rgba_f(0, 0, 0, 0.7f * a), flut[i].x + 2, flut[i].y + 2 - (70 - flut[i].t) * 0.8f, ALLEGRO_ALIGN_CENTRE, flut[i].txt);
            al_draw_text(f_g, c, flut[i].x, flut[i].y - (70 - flut[i].t) * 0.8f, ALLEGRO_ALIGN_CENTRE, flut[i].txt);
        }
    }

    /* fim de jogo */
    if (estado == E_FIM) {
        al_draw_filled_rectangle(0, 0, LARG, ALT, rgba(0, 0, 0, 170));
        al_draw_text(f_t, vencedor == 0 ? al_map_rgb(255, 225, 100) : al_map_rgb(255, 100, 90),
                     LARG / 2, 270, ALLEGRO_ALIGN_CENTRE, vencedor == 0 ? "VIT\303\223RIA!" : "DERROTA...");
        al_draw_text(f_n, al_map_rgb(235, 230, 250), LARG / 2, 365, ALLEGRO_ALIGN_CENTRE,
                     vencedor == 0 ? "Voc\303\252 derrotou o oponente!" : "O oponente venceu esta batalha.");
        desenhar_botao(R_FIM, vencedor == 0 ? "PR\303\223XIMA LUTA" : "TENTAR NOVAMENTE", 1);
        desenhar_botao(R_FIM2, "VOLTAR AO MENU", 1);
    }
}
