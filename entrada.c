/* entrada.c - Entrada: cliques do mouse e teclado em cada tela. */
#include "jogo.h"

void clique(int bt)
{
    if (estado == E_MENU) {
        if (bt != 1) return;
        if (dentro(R_JOGAR, mx, my)) { tocar(snd_clique); estado = E_DIFIC; }
        else if (dentro(R_INSTR, mx, my)) { tocar(snd_clique); estado = E_INSTR; }
        else if (dentro(R_SAIR, mx, my)) sair = 1;
        return;
    }
    if (estado == E_DIFIC) {
        int d = -1;
        if (bt != 1) return;
        if (dentro(R_D0, mx, my)) d = 0;
        else if (dentro(R_D1, mx, my)) d = 1;
        else if (dentro(R_D2, mx, my)) d = 2;
        else if (dentro(R_VOLTAR, mx, my)) { tocar(snd_clique); estado = E_MENU; return; }
        if (d >= 0) { dific = d; luta = 0; vidaJog = VIDA_INICIAL; bonusDano = 0; tocar(snd_clique); novo_jogo(); }
        return;
    }
    if (estado == E_QUIZ) {
        if (bt != 1) return;
        if (quizLicao) {                          /* aula: so avanca no botao */
            if (dentro(R_LICAO, mx, my)) { quizLicao = 0; tocar(snd_clique); }
            return;
        }
        if (quizEsc >= 0) {                       /* ja respondeu: clique pula a espera */
            if (quizTimer >= 20) finalizar_quiz();
        } else {
            int k;
            for (k = 0; k < QUIZ_OPC; k++)
                if (dentro(R_QOPC[k], mx, my)) { escolher_alternativa(k); break; }
        }
        return;
    }
    if (estado == E_INSTR) {
        if (bt == 1 && dentro(R_VOLTAR, mx, my)) { tocar(snd_clique); estado = E_MENU; }
        return;
    }
    if (estado == E_FIM) {
        if (bt != 1) return;
        if (dentro(R_FIM, mx, my)) {
            tocar(snd_clique);
            if (vencedor == 0) {                  /* venceu: proxima luta, mais forte */
                luta++;
                vidaJog = J[0].vida + 4;
                if (vidaJog > VIDA_MAXIMA) vidaJog = VIDA_MAXIMA;
                bonusDano++;
            }
            novo_jogo();                          /* perdeu: tenta a mesma luta de novo */
        } else if (dentro(R_FIM2, mx, my)) {
            tocar(snd_clique);
            estado = E_MENU;
        }
        return;
    }
    if (estado == E_IA) return;

    if (bt == 2) {                                /* botão direito cancela */
        if (estado == E_ALVO) estado = E_JOGADOR;
        else if (estado == E_BLOQUEIO) selBloq = -1;
        return;
    }
    if (bt != 1) return;
    if (dentro(R_PRINC, mx, my)) { acao_principal(); return; }

    if (estado == E_JOGADOR) {
        int im = idx_mao_em(mx, my), ic;
        if (im >= 0) { clicar_mao(im); return; }
        ic = idx_campo_em(0, mx, my);
        if (ic >= 0) {
            Criatura *c = &J[0].campo[ic];
            float x, y;
            centro_criatura(0, ic, &x, &y);
            if (!jaAtacou && !c->virada && !c->enjoada) { c->sel = !c->sel; tocar(snd_clique); }
            else if (c->enjoada) flutuar(x, y, rgba(255, 220, 120, 255), "Enjoada");
            else flutuar(x, y, rgba(255, 220, 120, 255), "N\303\243o pode atacar");
        }
    } else if (estado == E_ALVO) {
        int ic = idx_campo_em(1, mx, my);
        if (ic >= 0) {
            iniciar_quiz(maoAlvo, 1, ic);
            if (estado == E_ALVO) estado = E_JOGADOR;
        } else if (dentro(R_RET_OPP, mx, my)) {
            iniciar_quiz(maoAlvo, 2, 0);
            if (estado == E_ALVO) estado = E_JOGADOR;
        }
    } else if (estado == E_BLOQUEIO) {
        int ib = idx_campo_em(0, mx, my), ia, k, q;
        if (ib >= 0) {
            if (J[0].campo[ib].virada) {
                float x, y;
                centro_criatura(0, ib, &x, &y);
                flutuar(x, y, rgba(255, 220, 120, 255), "Est\303\241 virada");
                return;
            }
            for (k = 0; k < nAtac; k++) if (bloq[k] == ib) bloq[k] = -1;
            selBloq = (selBloq == ib) ? -1 : ib;
            tocar(snd_clique);
            return;
        }
        ia = idx_campo_em(1, mx, my);
        if (ia >= 0) {
            k = -1;
            for (q = 0; q < nAtac; q++) if (atacantes[q] == ia) k = q;
            if (k < 0) return;
            if (selBloq >= 0) { bloq[k] = selBloq; selBloq = -1; tocar(snd_clique); }
            else bloq[k] = -1;
        }
    }
}

void teclado(int tecla)
{
    if (estado == E_QUIZ) {
        int k = -1;
        if (quizLicao) {                          /* aula: Enter/Espaco avancam, ESC cancela */
            if (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER || tecla == ALLEGRO_KEY_SPACE) {
                quizLicao = 0; tocar(snd_clique);
                return;
            }
            if (tecla != ALLEGRO_KEY_ESCAPE) return;
        }
        if (quizEsc >= 0) {                       /* mostrando o resultado: qualquer tecla segue */
            if (quizTimer >= 20 && (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER ||
                                    tecla == ALLEGRO_KEY_SPACE || tecla == ALLEGRO_KEY_ESCAPE)) finalizar_quiz();
            return;
        }
        if (tecla == ALLEGRO_KEY_ESCAPE) { estado = E_JOGADOR; return; }
        if (tecla >= ALLEGRO_KEY_1 && tecla <= ALLEGRO_KEY_4) k = tecla - ALLEGRO_KEY_1;
        else if (tecla >= ALLEGRO_KEY_PAD_1 && tecla <= ALLEGRO_KEY_PAD_4) k = tecla - ALLEGRO_KEY_PAD_1;
        else if (tecla >= ALLEGRO_KEY_A && tecla <= ALLEGRO_KEY_D) k = tecla - ALLEGRO_KEY_A;
        if (k >= 0) escolher_alternativa(k);
        return;
    }
    if (tecla == ALLEGRO_KEY_F11) {
        int cheia = al_get_display_flags(disp) & ALLEGRO_FULLSCREEN_WINDOW;
        al_set_display_flag(disp, ALLEGRO_FULLSCREEN_WINDOW, !cheia);
    } else if (tecla == ALLEGRO_KEY_ESCAPE) {
        if (estado == E_MENU) sair = 1;
        else estado = E_MENU;
    } else if (tecla == ALLEGRO_KEY_SPACE || tecla == ALLEGRO_KEY_ENTER) {
        if (estado == E_JOGADOR || estado == E_BLOQUEIO || estado == E_ALVO) acao_principal();
    }
}
