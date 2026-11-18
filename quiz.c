/* quiz.c - Minigame: geracao das contas, aulas, alternativas e resultado. */
#include "jogo.h"

void add_err(int v) { if (quizNErr < 10) quizErr[quizNErr++] = v; }

void licao(const char *a, const char *b, const char *c, const char *d)
{
    snprintf(quizL[0], sizeof(quizL[0]), "%s", a);
    snprintf(quizL[1], sizeof(quizL[1]), "%s", b);
    snprintf(quizL[2], sizeof(quizL[2]), "%s", c);
    snprintf(quizL[3], sizeof(quizL[3]), "%s", d);
}

int ipow(int b, int e) { int r = 1; while (e-- > 0) r *= b; return r; }

/* numero entre parenteses se for negativo (para mostrar a solucao) */
const char *pn(int v)
{
    static char buf[6][16];
    static int k = 0;
    k = (k + 1) % 6;
    if (v < 0) snprintf(buf[k], sizeof(buf[k]), "(%d)", v);
    else snprintf(buf[k], sizeof(buf[k]), "%d", v);
    return buf[k];
}

/* monta "ax^2 + bx + c" sem termos zerados */
void fmt_quadratica(char *out, size_t n, int a, int b, int c)
{
    char t[40];
    out[0] = 0;
    if (a == 1) snprintf(t, sizeof(t), "x\302\262");
    else snprintf(t, sizeof(t), "%dx\302\262", a);
    strncat(out, t, n - strlen(out) - 1);
    if (b != 0) {
        if (abs(b) == 1) snprintf(t, sizeof(t), " %c x", b < 0 ? '-' : '+');
        else snprintf(t, sizeof(t), " %c %dx", b < 0 ? '-' : '+', abs(b));
        strncat(out, t, n - strlen(out) - 1);
    }
    if (c != 0) {
        snprintf(t, sizeof(t), " %c %d", c < 0 ? '-' : '+', abs(c));
        strncat(out, t, n - strlen(out) - 1);
    }
}

/* escreve "log" com a base em subscrito: log2, log3, log5 ou log (base 10) */
void fmt_log(char *out, size_t n, int base)
{
    if (base == 2) snprintf(out, n, "log\342\202\202");
    else if (base == 3) snprintf(out, n, "log\342\202\203");
    else if (base == 5) snprintf(out, n, "log\342\202\205");
    else snprintf(out, n, "log");
}

void quiz_exponencial(int d)
{
    int A = 1 + rand() % (3 + d * 2), B = 2 + rand() % 2, X = 1 + rand() % (2 + d), P;
    P = ipow(B, X);
    quizResp = A * P;
    strcpy(quizMateria, "Fun\303\247\303\243o Exponencial");
    licao("O QUE É: na função exponencial a letra x fica no EXPOENTE. f(x) = A · B^x.",
          "B^x quer dizer: multiplicar B por ele mesmo x vezes. Ex.: 2^3 = 2 · 2 · 2 = 8.",
          "EXEMPLO: f(x) = 3 · 2^x, com x = 2. Primeiro 2^2 = 4, depois 3 · 4 = 12.",
          "PASSOS: 1) calcule a potência B^x   2) multiplique o resultado por A.");
    snprintf(quizP0, sizeof(quizP0), "f(x) = %d \302\267 %d^x", A, B);
    snprintf(quizP1, sizeof(quizP1), "Quanto vale f(%d)?", X);
    snprintf(quizSol, sizeof(quizSol), "f(%d) = %d \302\267 %d^%d = %d \302\267 %d = %d", X, A, B, X, A, P, quizResp);
    add_err(A * B * X);                         /* confundiu pot\303\252ncia com multiplica\303\247\303\243o */
    add_err(ipow(A * B, X));                    /* multiplicou A por B antes de elevar */
    add_err(A + P);
    add_err(A * ipow(B, X + 1));
    add_err(A * ipow(B, X - 1));
}

void quiz_logaritmo(int d)
{
    static const int BASES[3][4] = { { 2, 3, 2, 3 }, { 2, 3, 5, 10 }, { 2, 3, 5, 10 } };
    int b = BASES[d][rand() % 4], e, xv, a = 1, c = 0;
    char lg[16], f[40];
    if (b == 10) e = 1 + rand() % 3;
    else if (b == 2) e = 1 + rand() % (4 + d);
    else e = 1 + rand() % (3 + (d > 0));
    xv = ipow(b, e);
    if (d >= 2) { a = 2 + rand() % 2; c = -3 + rand() % 9; }
    quizResp = a * e + c;
    fmt_log(lg, sizeof(lg), b);
    strcpy(quizMateria, "Fun\303\247\303\243o Logar\303\255tmica");
    if (d >= 2) licao("O QUE É: log_b(x) pergunta \"a que expoente elevo b para chegar em x?\". Ex.: log₂(8) = 3, pois 2³ = 8.",
          "Sem base escrita, a base é 10: log(1000) = 3, pois 10³ = 1000.",
          "EXEMPLO: f(x) = 2 · log₂(x) + 1, x = 8. Como log₂(8) = 3, f = 2 · 3 + 1 = 7.",
          "PASSOS: 1) ache o logaritmo (o expoente)   2) multiplique por A   3) some ou subtraia C.");
    else licao("O QUE É: log_b(x) pergunta \"a que expoente elevo b para chegar em x?\".",
          "Ex.: log₂(8) = 3, porque 2³ = 8. Sem base escrita, a base é 10: log(100) = 2.",
          "EXEMPLO: log₃(9) = 2, porque 3 · 3 = 9, ou seja, 3² = 9.",
          "PASSOS: 1) escreva x como potência da base   2) o expoente é a resposta.");
    if (d >= 2) {
        snprintf(f, sizeof(f), "f(x) = %d \302\267 %s(x) %c %d", a, lg, c < 0 ? '-' : '+', abs(c));
        snprintf(quizSol, sizeof(quizSol), "%s(%d) = %d, pois %d^%d = %d. f = %d \302\267 %d %c %d = %d", lg, xv, e, b, e, xv, a, e, c < 0 ? '-' : '+', abs(c), quizResp);
        add_err(e + c);
        add_err(a * e - c);
        add_err(a * xv + c);
    } else {
        snprintf(f, sizeof(f), "f(x) = %s(x)", lg);
        snprintf(quizSol, sizeof(quizSol), "%s(%d) = %d, pois %d^%d = %d", lg, xv, e, b, e, xv);
        add_err(b * e);                         /* multiplicou base por expoente */
        add_err(xv / b);
        add_err(xv - b);
    }
    strcpy(quizP0, f);
    snprintf(quizP1, sizeof(quizP1), "Quanto vale f(%d)?", xv);
    add_err(quizResp + 1);
    add_err(quizResp - 1);
}

void quiz_afim(int d)
{
    int a = 2 + rand() % (5 + d * 3), b = -9 + rand() % 19, X = 1 + rand() % (6 + d * 4);
    quizResp = a * X + b;
    strcpy(quizMateria, "Fun\303\247\303\243o Afim");
    licao("O QUE É: função afim é f(x) = a·x + b. O gráfico é uma reta.",
          "Para achar f(x), troque a letra x pelo número que a pergunta dá.",
          "EXEMPLO: f(x) = 2x + 3, com x = 4. Fica 2 · 4 + 3 = 8 + 3 = 11.",
          "PASSOS: 1) multiplique a · x   2) some o b. Cuidado com o sinal do b!");
    snprintf(quizP0, sizeof(quizP0), "f(x) = %dx %c %d", a, b < 0 ? '-' : '+', abs(b));
    snprintf(quizP1, sizeof(quizP1), "Quanto vale f(%d)?", X);
    snprintf(quizSol, sizeof(quizSol), "f(%d) = %d \302\267 %d %c %d = %d", X, a, X, b < 0 ? '-' : '+', abs(b), quizResp);
    add_err(a * X - b);                         /* sinal do termo b trocado */
    add_err(a * X);                             /* esqueceu o b */
    add_err(a * (X + b));
    add_err(a + X + b);
}

void quiz_quadratica(int d)
{
    int tipo = rand() % (d == 0 ? 1 : (d == 1 ? 2 : 3));    /* 0: f(x), 1: vertice, 2: raiz */
    char q[64];
    strcpy(quizMateria, "Fun\303\247\303\243o Quadr\303\241tica");
    if (tipo == 0) licao("O QUE É: função quadrática é f(x) = ax² + bx + c. O gráfico é uma parábola.",
          "Troque x pelo número dado. Calcule o x² ANTES de multiplicar pelo a.",
          "EXEMPLO: f(x) = x² + 2x + 1, com x = 3. Fica 9 + 6 + 1 = 16.",
          "PASSOS: 1) calcule x²   2) multiplique por a e por b   3) some tudo com o c.");
    else if (tipo == 1) licao("O QUE É: o vértice é o ponto onde a parábola muda de direção (o ponto mais baixo ou mais alto).",
          "O x do vértice é xv = −b / (2a).",
          "EXEMPLO: f(x) = x² − 6x + 5. Aqui a = 1 e b = −6, então xv = −(−6) / (2 · 1) = 3.",
          "PASSOS: 1) pegue a e b   2) troque o sinal do b   3) divida por 2a.");
    else licao("O QUE É: as raízes são os valores de x em que f(x) = 0 (onde a parábola corta o eixo x).",
          "Quando a = 1: a SOMA das raízes é −b e o PRODUTO das raízes é c.",
          "EXEMPLO: x² − 5x + 6. Soma = 5 e produto = 6, então as raízes são 2 e 3.",
          "PASSOS: 1) ache a soma e o produto   2) procure dois números que fecham as duas contas.");
    if (tipo == 0) {
        int a = 1 + rand() % (d + 1 + (d > 1)), b = -3 - d * 2 + rand() % (7 + d * 4), c = -5 - d * 2 + rand() % (11 + d * 4), X;
        do { X = (d == 0) ? 1 + rand() % 3 : -3 + rand() % (7 + d); } while (X == 0);
        quizResp = a * X * X + b * X + c;
        fmt_quadratica(q, sizeof(q), a, b, c);
        snprintf(quizP0, sizeof(quizP0), "f(x) = %s", q);
        snprintf(quizP1, sizeof(quizP1), "Quanto vale f(%d)?", X);
        if (b == 0) snprintf(quizSol, sizeof(quizSol), "f(%d) = %d \302\267 %s\302\262 + %s = %d", X, a, pn(X), pn(c), quizResp);
        else snprintf(quizSol, sizeof(quizSol), "f(%d) = %d \302\267 %s\302\262 + %s \302\267 %s + %s = %d", X, a, pn(X), pn(b), pn(X), pn(c), quizResp);
        add_err(a * 2 * X + b * X + c);        /* x\302\262 virou 2x */
        add_err(a * X * X - b * X + c);
        add_err(a * X * X + b * X - c);
        add_err((a * X + b) * X + c + a);
    } else if (tipo == 1) {
        int a = 1 + rand() % 2, h = -4 + rand() % 11, b, c = -6 + rand() % 13;
        if (h == 0) h = 3;
        b = -2 * a * h;
        quizResp = h;
        fmt_quadratica(q, sizeof(q), a, b, c);
        snprintf(quizP0, sizeof(quizP0), "f(x) = %s", q);
        snprintf(quizP1, sizeof(quizP1), "Qual \303\251 o x do v\303\251rtice?");
        snprintf(quizSol, sizeof(quizSol), "xv = -b / (2a) = -%s / (2 \302\267 %d) = %d", pn(b), a, h);
        add_err(-h);
        add_err(b);
        add_err(2 * h);
        add_err(h + 1);
        add_err(h - 1);
    } else {
        int r1 = -5 + rand() % 13, r2 = -5 + rand() % 13, mx_, mn_;
        if (r1 == r2) r2 = r1 + 2;
        mx_ = r1 > r2 ? r1 : r2;
        mn_ = r1 > r2 ? r2 : r1;
        quizResp = mx_;
        fmt_quadratica(q, sizeof(q), 1, -(r1 + r2), r1 * r2);
        snprintf(quizP0, sizeof(quizP0), "f(x) = %s", q);
        snprintf(quizP1, sizeof(quizP1), "Qual \303\251 a maior raiz de f?");
        snprintf(quizSol, sizeof(quizSol), "Soma = %d e produto = %d \342\206\222 ra\303\255zes %d e %d", r1 + r2, r1 * r2, mn_, mx_);
        add_err(mn_);
        add_err(-mx_);
        add_err(r1 + r2);
        add_err(r1 * r2);
    }
}

void quiz_porcentagem(int d)
{
    static const int P[4] = { 10, 20, 25, 50 };
    int p = P[rand() % 4], V = 20 * (1 + rand() % (5 + d * 5)), tipo = (d == 0) ? 0 : rand() % 3, r = V * p / 100;
    strcpy(quizMateria, "Porcentagem");
    if (tipo == 0) licao("O QUE É: porcentagem é uma fração de 100. p% de V = (p / 100) · V.",
          "Ex.: 10% é 10/100 = 0,1, então 10% de 50 é 0,1 · 50 = 5.",
          "EXEMPLO: 20% de 50 = 20 / 100 · 50 = 10.",
          "PASSOS: 1) divida a porcentagem por 100   2) multiplique pelo valor.");
    else if (tipo == 1) licao("O QUE É: aumento de p% significa somar ao valor p% dele mesmo.",
          "novo valor = V + (p% de V).",
          "EXEMPLO: R$ 100 com aumento de 10%. 10% de 100 = 10, então fica 100 + 10 = 110.",
          "PASSOS: 1) ache p% de V   2) SOME ao valor original.");
    else licao("O QUE É: desconto de p% significa tirar do valor p% dele mesmo.",
          "novo valor = V − (p% de V).",
          "EXEMPLO: R$ 100 com desconto de 10%. 10% de 100 = 10, então fica 100 − 10 = 90.",
          "PASSOS: 1) ache p% de V   2) SUBTRAIA do valor original.");
    quizP1[0] = 0;
    if (tipo == 0) {                            /* parte de um todo */
        quizResp = r;
        snprintf(quizP0, sizeof(quizP0), "Quanto \303\251 %d%% de %d?", p, V);
        snprintf(quizSol, sizeof(quizSol), "%d%% de %d = %d / 100 \302\267 %d = %d", p, V, p, V, quizResp);
    } else if (tipo == 1) {                     /* aumento */
        quizResp = V + r;
        snprintf(quizP0, sizeof(quizP0), "Um item de R$ %d sobe %d%%.", V, p);
        snprintf(quizP1, sizeof(quizP1), "Qual o novo pre\303\247o, em reais?");
        snprintf(quizSol, sizeof(quizSol), "%d + %d%% de %d = %d + %d = %d", V, p, V, V, r, quizResp);
        add_err(V - r);
    } else {                                    /* desconto */
        quizResp = V - r;
        snprintf(quizP0, sizeof(quizP0), "Um item de R$ %d tem %d%% de desconto.", V, p);
        snprintf(quizP1, sizeof(quizP1), "Qual o novo pre\303\247o, em reais?");
        snprintf(quizSol, sizeof(quizSol), "%d - %d%% de %d = %d - %d = %d", V, p, V, V, r, quizResp);
        add_err(V + r);
    }
    add_err(r);
    add_err(V * p / 10);
    add_err(V / p);
    add_err(V - r);
}

void quiz_financeira(int d)
{
    int composto = (d == 0) || (rand() % 2 == 0);
    strcpy(quizMateria, "Matem\303\241tica Financeira");
    if (composto) licao("O QUE É: nos juros compostos, os juros de cada mês rendem juros também. M = C · (1 + i)^t.",
          "M é o montante (o total no fim), C é o capital inicial, i é a taxa e t é o tempo.",
          "EXEMPLO: C = 100, i = 10% e t = 2 meses. M = 100 · 1,1² = 100 · 1,21 = 121.",
          "PASSOS: 1) some 1 + i (10% vira 1,1)   2) eleve ao tempo t   3) multiplique por C.");
    else licao("O QUE É: nos juros simples, os juros são sempre calculados só sobre o capital inicial. M = C · (1 + i · t).",
          "M é o montante (o total no fim), C é o capital inicial, i é a taxa e t é o tempo.",
          "EXEMPLO: C = 100, i = 5% ao mês e t = 3. Juros = 100 · 5% · 3 = 15, então M = 100 + 15 = 115.",
          "PASSOS: 1) calcule os juros C · i · t   2) some ao capital C.");
    if (composto) {                             /* juros compostos a 10% */
        int t = 1 + rand() % (d == 2 ? 3 : 2), C;
        C = (t == 3) ? 1000 * (1 + rand() % 2) : 100 * (1 + rand() % (3 + d));
        quizResp = (t == 1) ? C * 11 / 10 : (t == 2 ? C * 121 / 100 : C * 1331 / 1000);
        snprintf(quizP0, sizeof(quizP0), "Juros compostos: M = C \302\267 (1 + i)^t, i = 10%%");
        snprintf(quizP1, sizeof(quizP1), "C = %d e t = %d m\303\252s(es). Qual o montante M?", C, t);
        snprintf(quizSol, sizeof(quizSol), "M = %d \302\267 1,1^%d = %d", C, t, quizResp);
        add_err(C + C * t / 10);                /* usou juros simples */
        add_err(C * 11 / 10);
        add_err(quizResp - C);                  /* so os juros */
    } else {                                    /* juros simples */
        static const int I[3] = { 2, 5, 10 };
        int i = I[rand() % 3], t = 2 + rand() % 5, C = 100 * (1 + rand() % (3 + d)), J = C * i * t / 100;
        quizResp = C + J;
        snprintf(quizP0, sizeof(quizP0), "Juros simples: M = C \302\267 (1 + i \302\267 t), i = %d%% ao m\303\252s", i);
        snprintf(quizP1, sizeof(quizP1), "C = %d e t = %d meses. Qual o montante M?", C, t);
        snprintf(quizSol, sizeof(quizSol), "J = %d \302\267 %d%% \302\267 %d = %d, ent\303\243o M = %d + %d = %d", C, i, t, J, C, J, quizResp);
        add_err(J);                             /* respondeu so os juros */
        add_err(C + C * i / 100);               /* ignorou o tempo */
        {   /* usou juros compostos */
            double m = C; int k;
            for (k = 0; k < t; k++) m *= 1.0 + i / 100.0;
            add_err((int)(m + 0.5));
        }
    }
    add_err(quizResp + 100);
    add_err(quizResp - 100);
}

int tem_valor(const int *v, int n, int x)
{
    int i;
    for (i = 0; i < n; i++) if (v[i] == x) return 1;
    return 0;
}

/* sorteia 3 alternativas erradas (erros comuns primeiro) e embaralha com a certa */
void montar_alternativas(void)
{
    int v[QUIZ_OPC], n = 0, i, tent = 0, ordem[10];
    for (i = 0; i < quizNErr; i++) ordem[i] = quizErr[i];
    embaralhar(ordem, quizNErr);
    for (i = 0; i < quizNErr && n < QUIZ_OPC - 1; i++)
        if (ordem[i] != quizResp && !tem_valor(v, n, ordem[i]) && abs(ordem[i]) < 100000) v[n++] = ordem[i];
    while (n < QUIZ_OPC - 1 && tent < 300) {    /* completa com valores proximos da resposta */
        int amp = abs(quizResp) / 4 + 3, delta = 1 + rand() % amp, x;
        x = (rand() % 2) ? quizResp + delta : quizResp - delta;
        tent++;
        if (x != quizResp && !tem_valor(v, n, x) && (quizResp < 0 || x >= 0 || tent > 150)) v[n++] = x;
    }
    for (i = 1; n < QUIZ_OPC - 1; i++)          /* garantia final */
        if (!tem_valor(v, n, quizResp + i)) v[n++] = quizResp + i;
    v[n++] = quizResp;
    embaralhar(v, n);
    for (i = 0; i < QUIZ_OPC; i++) {
        quizOpc[i] = v[i];
        if (v[i] == quizResp) quizCerta = i;
    }
}

void gerar_quiz(int id)
{
    int cor = CARTAS[id].cor, d = dific;
    quizNErr = 0;
    quizSol[0] = 0;
    if (cor == FOGO) { if (rand() % 2) quiz_exponencial(d); else quiz_logaritmo(d); }
    else if (cor == AGUA) { if (rand() % 2) quiz_afim(d); else quiz_quadratica(d); }
    else if (cor == FLORESTA) quiz_porcentagem(d);
    else quiz_financeira(d);
    montar_alternativas();
}

void iniciar_quiz(int im, int alvoTipo, int alvoIdx)
{
    quizIm = im; quizTipo = alvoTipo; quizIdx = alvoIdx;
    quizId = J[0].mao[im];
    quizEsc = -1; quizTimer = 0; quizAcertou = 0;
    gerar_quiz(quizId);
    quizLicao = 1;
    estado = E_QUIZ;
    tocar(snd_clique);
}

/* o jogador clicou numa alternativa: mostra acerto/erro com som e efeitos */
void escolher_alternativa(int k)
{
    float x, y;
    if (quizEsc >= 0 || k < 0 || k >= QUIZ_OPC) return;
    quizEsc = k;
    quizTimer = 0;
    quizAcertou = (k == quizCerta);
    x = R_QOPC[k].x + R_QOPC[k].w / 2;
    y = R_QOPC[k].y + R_QOPC[k].h / 2;
    if (quizAcertou) {
        tocar(snd_acerto);
        explodir(x, y, 26, rgba(255, 225, 110, 255), 1, 5.5f);
        explodir(x, y, 22, rgba(120, 255, 150, 255), 0, 4.5f);
        anel(x, y, rgba(120, 255, 150, 255), 4.0f, 30);
        anel(x, y, rgba(255, 225, 110, 255), 2.6f, 38);
        piscar(rgba(90, 255, 140, 255), 14);
    } else {
        tocar(snd_erro);
        explodir(x, y, 22, rgba(255, 80, 70, 255), 2, 6.0f);
        explodir(x, y, 12, rgba(70, 30, 30, 255), 0, 3.0f);
        anel(x, y, rgba(255, 80, 70, 255), 3.4f, 26);
        sacudir(26, 8.0f);
        piscar(rgba(255, 50, 40, 255), 20);
    }
}

/* depois de mostrar o resultado, aplica o efeito na partida */
void finalizar_quiz(void)
{
    float x = LARG / 2.0f;
    estado = E_JOGADOR;
    quizEsc = -1;
    if (quizAcertou) {
        flutuar(x, 300, rgba(120, 255, 150, 255), "Acertou!");
        if (!jogar_carta(0, quizIm, quizTipo, quizIdx)) msg("A carta nao pode mais ser jogada.");
    } else {
        flutuar(x, 300, rgba(255, 110, 100, 255), "Errou!");
        msg("Errou! A resposta era %d. O feiti\303\247o falhou.", quizResp);
        J[0].manaUsada += CARTAS[quizId].custo;
        if (dific > 0) remover_da_mao(&J[0], quizIm);   /* no facil a carta volta pra mao */
    }
}
