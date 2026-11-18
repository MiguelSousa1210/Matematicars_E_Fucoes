/* layout.c - Posicoes das cartas na tela e deteccao de qual carta esta sob o mouse. */
#include "jogo.h"

float pos_campo_x(int n, int i)
{
    float total = n * CW + (n - 1) * GAP;
    float x0 = 130 + (830 - total) / 2;
    return x0 + i * (CW + GAP);
}

float pos_campo_y(int lado) { return lado == 0 ? 300.0f : 120.0f; }

float pos_mao_x(int n, int i)
{
    float passo = MW + 8;
    float total;
    if (n > 1 && (n - 1) * passo + MW > 940) passo = (940.0f - MW) / (n - 1);
    total = (n - 1) * passo + MW;
    return 20 + (940 - total) / 2 + i * passo;
}

void centro_criatura(int lado, int i, float *x, float *y)
{
    *x = pos_campo_x(J[lado].nCampo, i) + CW / 2;
    *y = pos_campo_y(lado) + CH / 2;
}

void centro_retrato(int lado, float *x, float *y)
{
    Ret r = (lado == 0) ? R_RET_JOG : R_RET_OPP;
    *x = r.x + r.w / 2;
    *y = r.y + r.h / 2;
}

int idx_mao_em(float x, float y)
{
    int n = J[0].nMao, i;
    for (i = n - 1; i >= 0; i--) {
        float cx = pos_mao_x(n, i);
        if (x >= cx && x <= cx + MW && y >= MAO_Y - 30 && y <= ALT) return i;
    }
    return -1;
}

int idx_campo_em(int lado, float x, float y)
{
    int n = J[lado].nCampo, i;
    float cy = pos_campo_y(lado);
    for (i = 0; i < n; i++) {
        float cx = pos_campo_x(n, i);
        if (x >= cx && x <= cx + CW && y >= cy - 20 && y <= cy + CH + 25) return i;
    }
    return -1;
}
