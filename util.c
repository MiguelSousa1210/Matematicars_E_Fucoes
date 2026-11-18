/* util.c - Utilitarios: cores, carregar imagens/fontes/sons, mensagens do log e textos flutuantes. */
#include "jogo.h"

ALLEGRO_COLOR rgba(int r, int g, int b, int a)
{
    float f = a / 255.0f;
    return al_map_rgba_f(r / 255.0f * f, g / 255.0f * f, b / 255.0f * f, f);
}

int dentro(Ret r, float px, float py)
{
    return px >= r.x && px <= r.x + r.w && py >= r.y && py <= r.y + r.h;
}

/* Procura o arquivo em algumas pastas, assim funciona no Visual Studio
   (que roda a partir da pasta do projeto) e em linha de comando. */
const char *caminho(const char *rel)
{
    static const char *prefixos[] = { "", "../", "../../", "../../../", "ARENA_CARTAS/", "../ARENA_CARTAS/", "../../ARENA_CARTAS/" };
    static char buf[8][512];
    static int k = 0;
    char *b;
    int i;
    const char *base = strrchr(rel, '/');       /* só o nome do arquivo, sem a pasta */
    base = base ? base + 1 : rel;
    k = (k + 1) % 8;
    b = buf[k];
    for (i = 0; i < 7; i++) {                   /* 1) com a pasta (imagens/arte_00.png) */
        snprintf(b, 512, "%s%s", prefixos[i], rel);
        if (al_filename_exists(b)) return b;
    }
    for (i = 0; i < 7; i++) {                   /* 2) arquivos soltos na pasta do projeto */
        snprintf(b, 512, "%s%s", prefixos[i], base);
        if (al_filename_exists(b)) return b;
    }
    snprintf(b, 512, "%s", rel);
    return b;
}

/* carrega uma fonte; se não achar a do jogo, tenta uma fonte do Windows */
ALLEGRO_FONT *carregar_fonte(const char *rel, int tam)
{
    ALLEGRO_FONT *f = al_load_ttf_font(caminho(rel), tam, 0);
    if (!f) f = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", tam, 0);
    if (!f) {
        fprintf(stderr, "Nao achei a fonte: %s\n", rel);
        fprintf(stderr, "Copie a pasta fontes (ou os arquivos .ttf) para a pasta do projeto, ao lado do main.c.\n");
        exit(1);
    }
    return f;
}

ALLEGRO_BITMAP *carregar(const char *rel)
{
    ALLEGRO_BITMAP *b = al_load_bitmap(caminho(rel));
    if (!b) {
        fprintf(stderr, "Nao achei a imagem: %s\n", rel);
        b = al_create_bitmap(64, 64);       /* imagem de reserva para não travar */
        al_set_target_bitmap(b);
        al_clear_to_color(al_map_rgb(200, 0, 200));
        al_set_target_backbuffer(disp);
    }
    return b;
}

ALLEGRO_SAMPLE *carregar_som(const char *rel)
{
    ALLEGRO_SAMPLE *s;
    if (!audio_ok) return NULL;
    s = al_load_sample(caminho(rel));
    if (!s) fprintf(stderr, "Nao achei o som: %s\n", rel);
    return s;
}

void tocar(ALLEGRO_SAMPLE *s)
{
    if (audio_ok && s) al_play_sample(s, 0.8f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
}

/* registra uma mensagem no painel de log */
void msg(const char *fmt, ...)
{
    va_list ap;
    int i;
    for (i = 0; i < 5; i++) strcpy(linhasLog[i], linhasLog[i + 1]);
    va_start(ap, fmt);
    vsnprintf(linhasLog[5], sizeof(linhasLog[5]), fmt, ap);
    va_end(ap);
}

/* texto flutuante (dano, cura, avisos) */
void flutuar(float x, float y, ALLEGRO_COLOR cor, const char *fmt, ...)
{
    va_list ap;
    int i;
    for (i = 0; i < 40; i++) {
        if (flut[i].t <= 0) {
            flut[i].x = x;
            flut[i].y = y;
            flut[i].t = 70;
            flut[i].cor = cor;
            va_start(ap, fmt);
            vsnprintf(flut[i].txt, sizeof(flut[i].txt), fmt, ap);
            va_end(ap);
            return;
        }
    }
}
