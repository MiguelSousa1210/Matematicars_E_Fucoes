/* dados.c - tabelas fixas: cartas (nomes, custos, textos), copias no baralho, nomes e botoes. */
#include "jogo.h"

const DefCarta CARTAS[NUM_CARTAS] = {
    /* ---- criaturas (0-9) ---- */
    { "Potência Veloz",        CRIATURA, FOGO,     PAPEL_ATAQUE, 1, 2, 1, 1, EF_NENHUM, 0, "Ataque Exponencial: 2ˣ cresce rápido e a carta já entra atacando (Investida)." },
    { "Cento por Cento",      CRIATURA, FLORESTA, PAPEL_ATAQUE, 2, 3, 2, 0, EF_NENHUM, 0, "Ataque de Porcentagem: 100% de força, poucos aguentam um golpe inteiro." },
    { "Reta Guardiã",   CRIATURA, AGUA,     PAPEL_DEFESA, 2, 1, 4, 0, EF_NENHUM, 0, "Defesa de Função Afim: uma reta firme e constante, ótima para bloquear." },
    { "Parábola Serpente",    CRIATURA, AGUA,     PAPEL_ATAQUE, 3, 3, 3, 0, EF_NENHUM, 0, "Ataque de Função Quadrática: a curva sobe, vira e esmaga os inimigos." },
    { "Bloco de 100%",    CRIATURA, FLORESTA, PAPEL_DEFESA, 4, 3, 5, 0, EF_NENHUM, 0, "Defesa de Porcentagem: um bloco inteiro de 100%, resiste a quase tudo." },
    { "Dragão Exponencial",      CRIATURA, FOGO,     PAPEL_ATAQUE, 5, 5, 4, 0, EF_NENHUM, 0, "Ataque Exponencial: cada turno vale o dobro do anterior. Cuidado com ele." },
    { "Anjo dos Juros",    CRIATURA, LUZ,      PAPEL_ATAQUE, 4, 4, 3, 0, EF_NENHUM, 0, "Ataque Financeiro: os juros compostos descem como uma espada." },
    { "Raiz Fantasma",          CRIATURA, AGUA,     PAPEL_ATAQUE, 3, 2, 2, 1, EF_NENHUM, 0, "Ataque de Função Quadrática: a raiz que surge do nada. Investida." },
    { "Vértice Abissal",    CRIATURA, AGUA,     PAPEL_ATAQUE, 6, 6, 6, 0, EF_NENHUM, 0, "Ataque de Função Quadrática: o ponto mais fundo da parábola. Ninguém escapa." },
    { "Fênix do Capital",     CRIATURA, LUZ,      PAPEL_ATAQUE, 5, 4, 4, 1, EF_NENHUM, 0, "Ataque Financeiro: o capital renasce com juros. Investida." },
    /* ---- feiticos (10-15) ---- */
    { "Explosão 2ˣ",      FEITICO,  FOGO,     PAPEL_ATAQUE, 2, 0, 0, 0, EF_DANO,       3, "Ataque Exponencial: a potência explode e causa 3 de dano a uma criatura ou ao oponente." },
    { "Raio Linear",       FEITICO,  AGUA,     PAPEL_ATAQUE, 1, 0, 0, 0, EF_DANO,       2, "Ataque de Função Afim: uma reta veloz causa 2 de dano a uma criatura ou ao oponente." },
    { "Rendimento",       FEITICO,  LUZ,      PAPEL_DEFESA, 2, 0, 0, 0, EF_CURA,       4, "Defesa Financeira: o dinheiro rende e fecha as feridas. Você recupera 4 de vida." },
    { "Chuva de Parábolas",        FEITICO,  AGUA,     PAPEL_ATAQUE, 4, 0, 0, 0, EF_TEMPESTADE, 2, "Ataque de Função Quadrática: causa 2 de dano a todas as criaturas do oponente." },
    { "Tabela de Valores",  FEITICO,  LUZ,      PAPEL_APOIO,  2, 0, 0, 0, EF_COMPRAR,    2, "Apoio Financeiro: a tabela revela os valores certos. Compre 2 cartas do seu baralho." },
    { "Aumento de 10%",        FEITICO,  FLORESTA, PAPEL_APOIO,  3, 0, 0, 0, EF_REFORCAR,   1, "Apoio de Porcentagem: suas criaturas ganham +1/+1 permanentemente." },
    /* ---- terras (16-19) ---- */
    { "Monte Expoente",          TERRA,    FOGO,     PAPEL_NENHUM, 0, 0, 0, 0, EF_NENHUM, 0, "Terreno: gera 1 mana por turno. Você pode jogar 1 terreno por turno." },
    { "Lago dos Gráficos",              TERRA,    AGUA,     PAPEL_NENHUM, 0, 0, 0, 0, EF_NENHUM, 0, "Terreno: gera 1 mana por turno. Você pode jogar 1 terreno por turno." },
    { "Bosque dos Percentuais",          TERRA,    FLORESTA, PAPEL_NENHUM, 0, 0, 0, 0, EF_NENHUM, 0, "Terreno: gera 1 mana por turno. Você pode jogar 1 terreno por turno." },
    { "Praça dos Juros",          TERRA,    LUZ,      PAPEL_NENHUM, 0, 0, 0, 0, EF_NENHUM, 0, "Terreno: gera 1 mana por turno. Você pode jogar 1 terreno por turno." },
    /* ---- cartas novas: defesa e ataque de cada materia (20-30) ---- */
    { "Muro de Potências", CRIATURA, FOGO,     PAPEL_DEFESA, 2, 1, 4, 0, EF_NENHUM, 0, "Defesa Exponencial: um muro de potências segura o avanço inimigo." },
    { "Escudo 2ˣ", FEITICO,  FOGO,     PAPEL_DEFESA, 2, 0, 0, 0, EF_ESCUDO,  4, "Defesa Exponencial: o escudo absorve os próximos 4 de dano que você sofrer." },
    { "Meteoro Exponencial",           FEITICO,  FOGO,     PAPEL_ATAQUE, 4, 0, 0, 0, EF_DANO,    5, "Ataque Exponencial: curva em chamas que causa 5 de dano a uma criatura ou ao oponente." },
    { "Coeficiente Ancião", CRIATURA, AGUA,   PAPEL_DEFESA, 3, 1, 6, 0, EF_NENHUM, 0, "Defesa de Função Afim: coeficiente imóvel, quase impossível de quebrar." },
    { "Barreira Linear",  FEITICO,  AGUA,     PAPEL_DEFESA, 3, 0, 0, 0, EF_ESCUDO,  6, "Defesa de Função Afim: a reta y = k absorve os próximos 6 de dano que você sofrer." },
    { "Desconto Relâmpago",   CRIATURA, FLORESTA, PAPEL_ATAQUE, 3, 4, 2, 1, EF_NENHUM, 0, "Ataque de Porcentagem: −50% de defesa para o inimigo. Investida." },
    { "Metade Guardiã",     CRIATURA, FLORESTA, PAPEL_DEFESA, 3, 2, 5, 0, EF_NENHUM, 0, "Defesa de Porcentagem: 50% atacante, 50% muralha. Bloqueia com firmeza." },
    { "Margem de Segurança",   FEITICO,  FLORESTA, PAPEL_DEFESA, 2, 0, 0, 0, EF_MURALHA, 2, "Defesa de Porcentagem: suas criaturas ganham +0/+2 permanentemente." },
    { "Juros Compostos",      FEITICO,  LUZ,      PAPEL_ATAQUE, 3, 0, 0, 0, EF_DANO,    4, "Ataque Financeiro: os juros sobre juros causam 4 de dano a uma criatura ou ao oponente." },
    { "Cofre Guardião", CRIATURA, LUZ,     PAPEL_DEFESA, 4, 2, 6, 0, EF_NENHUM, 0, "Defesa Financeira: um cofre blindado que protege os aliados." },
    { "Reserva de Emergência",        FEITICO,  LUZ,      PAPEL_DEFESA, 1, 0, 0, 0, EF_ESCUDO,  3, "Defesa Financeira: a reserva absorve os próximos 3 de dano que você sofrer." }
};

/* quantas copias de cada carta entram no baralho (total = 48) */
const int COPIAS[NUM_CARTAS] = {
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1,      /*  0-9  criaturas                       */
    2, 1, 1, 1, 1, 1,                  /* 10-15 feiticos                        */
    4, 4, 4, 4,                        /* 16-19 terras: 16                      */
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1    /* 20-30 novas (defesa/ataque por materia) */
};

const char *NOME_COR[4] = { "Exp/Log", "Fun\303\247\303\265es", "Porcent.", "Finan\303\247as" };

const char *NOME_TIPO[3] = { "Terreno", "N\303\272mero", "F\303\263rmula" };

const char *NOME_PAPEL[4] = { "", "ATAQUE", "DEFESA", "APOIO" };

const int VIDA_INI_INIMIGO[3] = { 14, 20, 26 };

const char *NOME_MONSTRO[5] = { "Slime Matem\303\241tico", "Goblin dos Expoentes", "Golem da Fun\303\247\303\243o",
                                       "Drag\303\243o do Infinito", "Hidra das Ra\303\255zes" };

/* botões e áreas */
const Ret R_JOGAR   = { 490, 330, 300, 62 };

const Ret R_INSTR   = { 490, 408, 300, 62 };

const Ret R_SAIR    = { 490, 486, 300, 62 };

const Ret R_VOLTAR  = { 490, 636, 300, 56 };

const Ret R_D0 = { 490, 250, 300, 62 };

const Ret R_D1 = { 490, 330, 300, 62 };

const Ret R_D2 = { 490, 410, 300, 62 };

const Ret R_QOPC[QUIZ_OPC] = {         /* alternativas do quiz (2 x 2) */
    { 340, 372, 280, 62 }, { 660, 372, 280, 62 },
    { 340, 447, 280, 62 }, { 660, 447, 280, 62 }
};

const Ret R_FIM     = { 490, 430, 300, 62 };

const Ret R_FIM2    = { 490, 504, 300, 62 };

const Ret R_LICAO   = { 380, 548, 520, 58 };

const Ret R_PRINC   = { 1005, 630, 240, 64 };

const Ret R_RET_OPP = { 20, 115, 90, 90 };

const Ret R_RET_JOG = { 20, 335, 90, 90 };
