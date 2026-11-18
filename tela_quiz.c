/* tela_quiz.c - Desenho do minigame (aula e pergunta). */
#include "jogo.h"

/* escolhe a fonte grande se o texto couber na caixa, senao a media */
void texto_ajustado(const char *txt, float y, ALLEGRO_COLOR cor)
{
    ALLEGRO_FONT *f = (al_get_text_width(f_g, txt) <= 580) ? f_g : f_n;
    float yy = (f == f_g) ? y : y + 7;
    al_draw_text(f, cor, LARG / 2, yy, ALLEGRO_ALIGN_CENTRE, txt);
}

void desenhar_quiz(void)
{
    char buf[96];
    int k;
    float pulso = 0.5f + 0.5f * sinf(tempo * 0.15f);
    ALLEGRO_COLOR borda = rgba(220, 180, 100, 255);
    if (quizEsc >= 0) borda = quizAcertou ? rgba(110, 255, 150, 255) : rgba(255, 90, 80, 255);

    al_draw_filled_rectangle(0, 0, LARG, ALT, rgba(0, 0, 0, 170));
    al_draw_filled_rounded_rectangle(320, 100, 960, 620, 18, 18, rgba(14, 10, 40, 245));
    al_draw_rounded_rectangle(320, 100, 960, 620, 18, 18, borda, 3);
    al_draw_text(f_g, al_map_rgb(255, 225, 140), LARG / 2, 116, ALLEGRO_ALIGN_CENTRE, CARTAS[quizId].nome);
    snprintf(buf, sizeof(buf), "Mat\303\251ria: %s", quizMateria);
    al_draw_text(f_n, al_map_rgb(180, 220, 255), LARG / 2, 168, ALLEGRO_ALIGN_CENTRE, buf);
    if (quizLicao) {
        char aula[700];
        snprintf(aula, sizeof(aula), "%s\n\n%s\n\n%s\n\n%s", quizL[0], quizL[1], quizL[2], quizL[3]);
        al_draw_text(f_n, al_map_rgb(255, 200, 120), LARG / 2, 200, ALLEGRO_ALIGN_CENTRE, "AULA R\303\201PIDA - leia antes de responder");
        al_draw_multiline_text(f_p, al_map_rgb(240, 236, 255), 350, 238, 580, 20, 0, aula);
        desenhar_botao(R_LICAO, "ENTENDI! VER A PERGUNTA", 1);
        return;
    }
    texto_ajustado(quizP0, 225, al_map_rgb(255, 255, 255));
    if (quizP1[0]) texto_ajustado(quizP1, 285, al_map_rgb(255, 255, 255));

    for (k = 0; k < QUIZ_OPC; k++) {
        Ret r = R_QOPC[k];
        int sobre = (quizEsc < 0) && dentro(r, mx, my);
        ALLEGRO_COLOR fundo = sobre ? rgba(235, 170, 60, 255) : rgba(70, 50, 120, 255);
        ALLEGRO_COLOR texto = al_map_rgb(255, 255, 255);
        float dx = 0;
        if (quizEsc >= 0) {
            if (k == quizCerta) fundo = rgba(40, 150, 80, 255);
            else if (k == quizEsc) { fundo = rgba(170, 40, 40, 255); dx = sinf(tempo * 1.6f) * 4 * (quizTimer < 24 ? 1.0f : 0.0f); }
            else { fundo = rgba(45, 40, 65, 255); texto = al_map_rgb(150, 150, 165); }
        }
        al_draw_filled_rounded_rectangle(r.x + dx, r.y, r.x + dx + r.w, r.y + r.h, 12, 12, fundo);
        al_draw_rounded_rectangle(r.x + dx, r.y, r.x + dx + r.w, r.y + r.h, 12, 12,
                                  (quizEsc >= 0 && k == quizCerta) ? rgba(170, 255, 190, (int)(180 + 75 * pulso)) : rgba(255, 225, 140, 255), 3);
        snprintf(buf, sizeof(buf), "%c)  %d", 'A' + k, quizOpc[k]);
        al_draw_text(f_g, texto, r.x + dx + r.w / 2, r.y + r.h / 2 - al_get_font_line_height(f_g) / 2.0f, ALLEGRO_ALIGN_CENTRE, buf);
        if (quizEsc >= 0 && k == quizCerta)
            al_draw_text(f_n, al_map_rgb(200, 255, 215), r.x + dx + r.w - 22, r.y + r.h / 2 - al_get_font_line_height(f_n) / 2.0f, ALLEGRO_ALIGN_CENTRE, "\342\234\223");
        else if (quizEsc >= 0 && k == quizEsc)
            al_draw_text(f_n, al_map_rgb(255, 210, 205), r.x + dx + r.w - 22, r.y + r.h / 2 - al_get_font_line_height(f_n) / 2.0f, ALLEGRO_ALIGN_CENTRE, "\342\234\227");
    }

    if (quizEsc < 0) {
        al_draw_text(f_p, al_map_rgb(200, 190, 230), LARG / 2, 535, ALLEGRO_ALIGN_CENTRE, "Clique na resposta certa (ou aperte 1-4 / A-D). ESC cancela.");
    } else {
        al_draw_text(f_n, quizAcertou ? al_map_rgb(130, 255, 160) : al_map_rgb(255, 120, 110), LARG / 2, 530, ALLEGRO_ALIGN_CENTRE,
                     quizAcertou ? "Acertou! A carta entra em jogo." : "Errou! O feiti\303\247o falhou.");
        al_draw_text(f_p, al_map_rgb(225, 215, 245), LARG / 2, 565, ALLEGRO_ALIGN_CENTRE, quizSol);
        if (quizTimer >= 20)
            al_draw_text(f_p, al_map_rgb(160, 150, 190), LARG / 2, 592, ALLEGRO_ALIGN_CENTRE, "(clique para continuar)");
    }
}
