/* globais.c - variaveis globais que mudam durante o jogo (estado da partida, fontes, imagens, sons). */
#include "jogo.h"

ALLEGRO_DISPLAY *disp;
ALLEGRO_FONT *f_p, *f_n, *f_g, *f_t, *f_cnome, *f_ctexto, *f_cnum;
ALLEGRO_BITMAP *img_fundo, *img_menu, *img_titulo, *img_verso;
ALLEGRO_BITMAP *img_moldura[4], *img_arte[NUM_CARTAS];
ALLEGRO_BITMAP *img_ret[2], *img_monstro[5], *img_mana, *img_vida;
ALLEGRO_SAMPLE *snd_clique, *snd_jogar, *snd_golpe, *snd_cura, *snd_feitico;
ALLEGRO_SAMPLE *snd_vitoria, *snd_derrota, *snd_musica;
ALLEGRO_SAMPLE *snd_acerto, *snd_erro, *snd_escudo;
int audio_ok = 0;
float escala = 1, offx = 0, offy = 0;     /* tela virtual -> real */
float mx = 0, my = 0;                     /* mouse em coordenadas virtuais */
int sair = 0;
Jogador J[2];                  /* 0 = você, 1 = oponente */
int estado = E_MENU;
int turnoDe = 0, numTurno = 0, vencedor = -1;
int jaAtacou = 0;
int maoAlvo = -1;              /* carta da mão esperando um alvo */
/* combate */
int atacantes[MAX_CAMPO], nAtac = 0;
int bloq[MAX_CAMPO];           /* bloqueador de cada atacante (-1 = nenhum) */
int ladoAtac = 0;
int selBloq = -1;
/* inteligência artificial */
int iaFase = 0, iaTimer = 0;
/* dificuldade, lutas, combo exponencial e minigame de conta */
int dific = 1;                 /* 0 facil, 1 medio, 2 dificil */
int luta = 0;                  /* numero da luta (0 = primeira) */
int vidaJog = 20;              /* vida do jogador ao comecar a luta */
int bonusDano = 0;             /* dano extra ganho a cada vitoria */
int streak = 0;                /* turnos seguidos em que o inimigo NAO atacou */
int iaAtacou = 0;
int quizIm = -1, quizTipo = 0, quizIdx = 0, quizId = 0;
int quizResp = 0;                       /* resposta certa (numero) */
int quizOpc[QUIZ_OPC];                  /* valores das alternativas */
int quizCerta = 0;                      /* indice da alternativa certa */
int quizEsc = -1;                       /* alternativa escolhida (-1 = ainda nao) */
int quizTimer = 0;                      /* tempo mostrando o resultado */
char quizMateria[48], quizP0[96], quizP1[96], quizSol[96];
Particula part[MAX_PART];
Anel aneis[MAX_ANEL];
int tremor = 0;                         /* frames de tela tremendo */
float tremorForca = 0;
int clarao = 0, claraoMax = 1;          /* frames de clarao na tela */
ALLEGRO_COLOR claraoCor;
int quizLicao = 0;                      /* 1 = mostrando a aula antes da pergunta */
char quizL[4][160];                     /* paragrafos da aula */
int quizAcertou = 0;                    /* resultado mostrado na tela do quiz */
/* mensagens */
char linhasLog[6][100];
Flutuante flut[40];
int tempo = 0;                 /* contador de frames */
int quizErr[10], quizNErr = 0;           /* erros comuns (viram alternativas erradas) */
