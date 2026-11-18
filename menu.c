/* menu.c - Telas de menu: menu principal, como jogar e escolha de dificuldade. */
#include "jogo.h"

void desenhar_menu(void)
{
    float f = 0.5f + 0.5f * sinf(tempo * 0.04f);
    al_draw_bitmap(img_menu, 0, 0, 0);
    al_draw_bitmap(img_titulo, (LARG - al_get_bitmap_width(img_titulo)) / 2.0f, 50 + 6 * f, 0);
    desenhar_botao(R_JOGAR, "JOGAR", 1);
    desenhar_botao(R_INSTR, "COMO JOGAR", 1);
    desenhar_botao(R_SAIR, "SAIR", 1);
    al_draw_text(f_p, al_map_rgb(200, 190, 230), LARG / 2, 680, ALLEGRO_ALIGN_CENTRE, "F11: tela cheia");
}

void desenhar_instrucoes(void)
{
    static const char *linhas[] = {
        "OBJETIVO: reduza a vida do monstro a 0 (ou deixe o baralho dele acabar).",
        "TERRAS: jogue 1 terra por turno. Cada terra em campo d\303\241 1 mana por turno.",
        "CRIATURAS: custam mana e ficam enjoadas (zZz) no turno em que entram,",
        "     exceto as com Investida. Clique nelas e em ATACAR para atacar.",
        "BLOQUEIO: quando o monstro atacar, clique numa criatura sua e depois no",
        "     atacante (borda vermelha). Depois clique em CONFIRMAR BLOQUEIOS.",
        "CARTAS DE ATAQUE (selo vermelho): criaturas fortes e feiti\303\247os de dano.",
        "CARTAS DE DEFESA (selo azul): criaturas resistentes, cura e ESCUDO.",
        "     O escudo absorve o dano antes da sua vida. CARTAS DE APOIO (selo verde).",
        "CONTA: ao jogar uma carta, escolha a resposta certa entre 4 alternativas",
        "     (clique ou tecle 1-4 / A-D). Errou = o feiti\303\247o falha.",
        "MAT\303\211RIAS: Fogo = exponencial e logaritmo   \303\201gua = fun\303\247\303\243o afim e quadr\303\241tica",
        "     Floresta = porcentagem   Luz = matem\303\241tica financeira (juros).",
        "COMBO: se o monstro n\303\243o atacar, seus feiti\303\247os de dano crescem x1,5 por turno (at\303\251 4).",
        "TECLAS: Espa\303\247o/Enter = bot\303\243o principal   F11 = tela cheia   ESC = menu",
        "Ven\303\247a o monstro para lutar de novo: ele fica mais forte, mas voc\303\252 ganha vida e dano."
    };
    int i, n = (int)(sizeof(linhas) / sizeof(linhas[0]));
    al_draw_bitmap(img_menu, 0, 0, 0);
    al_draw_filled_rounded_rectangle(90, 60, 1190, 610, 16, 16, rgba(6, 4, 20, 215));
    al_draw_rounded_rectangle(90, 60, 1190, 610, 16, 16, rgba(220, 180, 100, 255), 3);
    al_draw_text(f_g, al_map_rgb(255, 225, 140), LARG / 2, 80, ALLEGRO_ALIGN_CENTRE, "COMO JOGAR");
    for (i = 0; i < n; i++)
        al_draw_text(f_n, al_map_rgb(235, 230, 250), 125, 135 + i * 29, 0, linhas[i]);
    desenhar_botao(R_VOLTAR, "VOLTAR", 1);
}

void desenhar_dific(void)
{
    al_draw_bitmap(img_menu, 0, 0, 0);
    al_draw_filled_rounded_rectangle(380, 150, 900, 600, 16, 16, rgba(6, 4, 20, 215));
    al_draw_rounded_rectangle(380, 150, 900, 600, 16, 16, rgba(220, 180, 100, 255), 3);
    al_draw_text(f_g, al_map_rgb(255, 225, 140), LARG / 2, 175, ALLEGRO_ALIGN_CENTRE, "ESCOLHA A DIFICULDADE");
    desenhar_botao(R_D0, "F\303\201CIL", 1);
    desenhar_botao(R_D1, "M\303\211DIO", 1);
    desenhar_botao(R_D2, "DIF\303\215CIL", 1);
    al_draw_text(f_p, al_map_rgb(200, 190, 230), LARG / 2, 500, ALLEGRO_ALIGN_CENTRE, "F\303\241cil: monstro fraco e feiti\303\247o errado volta pra m\303\243o");
    al_draw_text(f_p, al_map_rgb(200, 190, 230), LARG / 2, 525, ALLEGRO_ALIGN_CENTRE, "Dif\303\255cil: monstro com mais vida e contas maiores");
    desenhar_botao(R_VOLTAR, "VOLTAR", 1);
}
