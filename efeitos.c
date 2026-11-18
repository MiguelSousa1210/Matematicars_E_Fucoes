/* efeitos.c - Efeitos visuais: particulas, aneis, tela tremendo e clarao. */
#include "jogo.h"

float aleat(float a, float b) { return a + (b - a) * (rand() / (float)RAND_MAX); }

/* explosao de particulas. forma: 0 bolinha, 1 estrela, 2 faisca */
void explodir(float x, float y, int n, ALLEGRO_COLOR cor, int forma, float vel)
{
    int i, k;
    for (i = 0; i < n; i++) {
        for (k = 0; k < MAX_PART && part[k].t > 0; k++) ;
        if (k >= MAX_PART) return;
        {
            float ang = aleat(0, 6.2832f), v = aleat(0.35f, 1.0f) * vel;
            part[k].x = x; part[k].y = y;
            part[k].vx = cosf(ang) * v; part[k].vy = sinf(ang) * v - 0.6f;
            part[k].tmax = part[k].t = 28 + rand() % 26;
            part[k].tam = aleat(2.0f, 5.5f);
            part[k].forma = forma;
            part[k].cor = cor;
        }
    }
}

void anel(float x, float y, ALLEGRO_COLOR cor, float vr, int dur)
{
    int i;
    for (i = 0; i < MAX_ANEL; i++)
        if (aneis[i].t <= 0) {
            aneis[i].x = x; aneis[i].y = y; aneis[i].r = 6; aneis[i].vr = vr;
            aneis[i].tmax = aneis[i].t = dur; aneis[i].cor = cor;
            return;
        }
}

void sacudir(int frames, float forca) { tremor = frames; tremorForca = forca; }

void piscar(ALLEGRO_COLOR cor, int frames) { clarao = claraoMax = frames; claraoCor = cor; }

void atualizar_efeitos(void)
{
    int i;
    for (i = 0; i < MAX_PART; i++)
        if (part[i].t > 0) {
            part[i].t--;
            part[i].x += part[i].vx;
            part[i].y += part[i].vy;
            part[i].vy += 0.09f;
            part[i].vx *= 0.985f;
        }
    for (i = 0; i < MAX_ANEL; i++)
        if (aneis[i].t > 0) { aneis[i].t--; aneis[i].r += aneis[i].vr; }
    if (tremor > 0) tremor--;
    if (clarao > 0) clarao--;
}

/* a cor ja e pre-multiplicada (ver rgba), entao basta escalar tudo por "a" */
ALLEGRO_COLOR com_alfa(ALLEGRO_COLOR c, float a)
{
    c.r *= a; c.g *= a; c.b *= a; c.a *= a;
    return c;
}

void desenhar_efeitos(void)
{
    int i;
    for (i = 0; i < MAX_ANEL; i++)
        if (aneis[i].t > 0) {
            float a = aneis[i].t / (float)aneis[i].tmax;
            al_draw_circle(aneis[i].x, aneis[i].y, aneis[i].r, com_alfa(aneis[i].cor, a), 2.0f + 4.0f * a);
        }
    for (i = 0; i < MAX_PART; i++)
        if (part[i].t > 0) {
            float a = part[i].t / (float)part[i].tmax, s = part[i].tam * (0.5f + 0.5f * a);
            ALLEGRO_COLOR c = com_alfa(part[i].cor, a);
            if (part[i].forma == 1) {                       /* estrelinha de 4 pontas */
                al_draw_filled_triangle(part[i].x, part[i].y - s * 2, part[i].x + s * 0.6f, part[i].y, part[i].x - s * 0.6f, part[i].y, c);
                al_draw_filled_triangle(part[i].x, part[i].y + s * 2, part[i].x + s * 0.6f, part[i].y, part[i].x - s * 0.6f, part[i].y, c);
                al_draw_filled_triangle(part[i].x - s * 2, part[i].y, part[i].x, part[i].y + s * 0.6f, part[i].x, part[i].y - s * 0.6f, c);
                al_draw_filled_triangle(part[i].x + s * 2, part[i].y, part[i].x, part[i].y + s * 0.6f, part[i].x, part[i].y - s * 0.6f, c);
            } else if (part[i].forma == 2) {                /* faisca: risco na direcao do movimento */
                al_draw_line(part[i].x, part[i].y, part[i].x - part[i].vx * 3, part[i].y - part[i].vy * 3, c, 2.0f);
            } else {
                al_draw_filled_circle(part[i].x, part[i].y, s, c);
            }
        }
    if (clarao > 0) {
        float a = clarao / (float)claraoMax;
        al_draw_filled_rectangle(0, 0, LARG, ALT, com_alfa(claraoCor, a * 0.55f));
    }
}
