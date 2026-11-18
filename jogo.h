/* jogo.h - declaracoes compartilhadas por todos os arquivos do jogo. */
#ifndef JOGO_H
#define JOGO_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#define LARG 1280            /* largura virtual da tela */
#define ALT 720              /* altura virtual da tela */
#define FPS 60.0
#define NUM_CARTAS 31        /* tipos diferentes de carta */
#define TAM_BARALHO 48
#define MAX_MAO 10
#define MAX_CAMPO 7
#define VIDA_INICIAL 20
#define VIDA_MAXIMA 40
#define CARTA_W 220          /* tamanho base de uma carta (px) */
#define CARTA_H 308
#define CW 110               /* carta pequena no campo de batalha */
#define CH 154
#define GAP 6
#define MW 123               /* carta da mão do jogador */
#define MH 172
#define MAO_Y 548
enum { TERRA, CRIATURA, FEITICO };
enum { FOGO, AGUA, FLORESTA, LUZ };
enum { PAPEL_NENHUM, PAPEL_ATAQUE, PAPEL_DEFESA, PAPEL_APOIO };
enum { EF_NENHUM, EF_DANO, EF_CURA, EF_TEMPESTADE, EF_COMPRAR, EF_REFORCAR, EF_ESCUDO, EF_MURALHA };
typedef struct {
    const char *nome;
    int tipo, cor, papel, custo, atq, def, investida, efeito, valor;
    const char *texto;
} DefCarta;
typedef struct {
    int id;                   /* qual carta é */
    int atq, def;             /* atributos atuais (podem ser reforçados) */
    int dano;                 /* dano sofrido neste turno */
    int virada;               /* 1 = já atacou (não pode atacar nem bloquear) */
    int enjoada;              /* 1 = acabou de entrar, não pode atacar */
    int sel;                  /* 1 = selecionada para atacar */
} Criatura;
typedef struct {
    int vida;
    int escudo;                      /* dano que o escudo ainda absorve */
    int mao[MAX_MAO];  int nMao;
    int baralho[TAM_BARALHO];  int nBaralho;
    int terras;                      /* terras em jogo = mana máxima */
    int terraCor[TAM_BARALHO];
    int manaUsada;
    int jogouTerra;                  /* já jogou terra neste turno? */
    Criatura campo[MAX_CAMPO];  int nCampo;
} Jogador;
/* estados da tela / do jogo */
enum { E_MENU, E_INSTR, E_JOGADOR, E_ALVO, E_IA, E_BLOQUEIO, E_FIM, E_DIFIC, E_QUIZ };
typedef struct { float x, y, w, h; } Ret;
typedef struct {
    float x, y;
    char txt[24];
    int t;
    ALLEGRO_COLOR cor;
} Flutuante;
#define QUIZ_OPC 4
/* efeitos visuais: particulas, aneis, tremor e clarao */
typedef struct {
    float x, y, vx, vy, tam;
    int t, tmax, forma;                        /* forma: 0 bolinha, 1 estrela, 2 faisca */
    ALLEGRO_COLOR cor;
} Particula;
typedef struct { float x, y, r, vr; int t, tmax; ALLEGRO_COLOR cor; } Anel;
#define MAX_PART 220
#define MAX_ANEL 10
#define ESCUDO_MAX 12

/* ---- variaveis e tabelas (definidas em globais.c e dados.c) ---- */
extern const DefCarta CARTAS[NUM_CARTAS];
extern const int COPIAS[NUM_CARTAS];
extern const char *NOME_COR[4];
extern const char *NOME_TIPO[3];
extern const char *NOME_PAPEL[4];
extern ALLEGRO_DISPLAY *disp;
extern ALLEGRO_FONT *f_p, *f_n, *f_g, *f_t, *f_cnome, *f_ctexto, *f_cnum;
extern ALLEGRO_BITMAP *img_fundo, *img_menu, *img_titulo, *img_verso;
extern ALLEGRO_BITMAP *img_moldura[4], *img_arte[NUM_CARTAS];
extern ALLEGRO_BITMAP *img_ret[2], *img_monstro[5], *img_mana, *img_vida;
extern ALLEGRO_SAMPLE *snd_clique, *snd_jogar, *snd_golpe, *snd_cura, *snd_feitico;
extern ALLEGRO_SAMPLE *snd_vitoria, *snd_derrota, *snd_musica;
extern ALLEGRO_SAMPLE *snd_acerto, *snd_erro, *snd_escudo;
extern int audio_ok;
extern float escala, offx, offy;
extern float mx, my;
extern int sair;
extern Jogador J[2];
extern int estado;
extern int turnoDe, numTurno, vencedor;
extern int jaAtacou;
extern int maoAlvo;
extern int atacantes[MAX_CAMPO], nAtac;
extern int bloq[MAX_CAMPO];
extern int ladoAtac;
extern int selBloq;
extern int iaFase, iaTimer;
extern int dific;
extern int luta;
extern int vidaJog;
extern int bonusDano;
extern int streak;
extern int iaAtacou;
extern const int VIDA_INI_INIMIGO[3];
extern const char *NOME_MONSTRO[5];
extern int quizIm, quizTipo, quizIdx, quizId;
extern int quizResp;
extern int quizOpc[QUIZ_OPC];
extern int quizCerta;
extern int quizEsc;
extern int quizTimer;
extern char quizMateria[48], quizP0[96], quizP1[96], quizSol[96];
extern Particula part[MAX_PART];
extern Anel aneis[MAX_ANEL];
extern int tremor;
extern float tremorForca;
extern int clarao, claraoMax;
extern ALLEGRO_COLOR claraoCor;
extern int quizLicao;
extern char quizL[4][160];
extern int quizAcertou;
extern char linhasLog[6][100];
extern Flutuante flut[40];
extern int tempo;
extern const Ret R_JOGAR;
extern const Ret R_INSTR;
extern const Ret R_SAIR;
extern const Ret R_VOLTAR;
extern const Ret R_D0;
extern const Ret R_D1;
extern const Ret R_D2;
extern const Ret R_QOPC[QUIZ_OPC];
extern const Ret R_FIM;
extern const Ret R_FIM2;
extern const Ret R_LICAO;
extern const Ret R_PRINC;
extern const Ret R_RET_OPP;
extern const Ret R_RET_JOG;
extern int quizErr[10], quizNErr;

/* ---- funcoes de cada arquivo ---- */

/* util.c */
ALLEGRO_COLOR rgba(int r, int g, int b, int a);
int dentro(Ret r, float px, float py);
const char *caminho(const char *rel);
ALLEGRO_FONT *carregar_fonte(const char *rel, int tam);
ALLEGRO_BITMAP *carregar(const char *rel);
ALLEGRO_SAMPLE *carregar_som(const char *rel);
void tocar(ALLEGRO_SAMPLE *s);
void msg(const char *fmt, ...);
void flutuar(float x, float y, ALLEGRO_COLOR cor, const char *fmt, ...);

/* efeitos.c */
float aleat(float a, float b);
void explodir(float x, float y, int n, ALLEGRO_COLOR cor, int forma, float vel);
void anel(float x, float y, ALLEGRO_COLOR cor, float vr, int dur);
void sacudir(int frames, float forca);
void piscar(ALLEGRO_COLOR cor, int frames);
void atualizar_efeitos(void);
ALLEGRO_COLOR com_alfa(ALLEGRO_COLOR c, float a);
void desenhar_efeitos(void);

/* layout.c */
float pos_campo_x(int n, int i);
float pos_campo_y(int lado);
float pos_mao_x(int n, int i);
void centro_criatura(int lado, int i, float *x, float *y);
void centro_retrato(int lado, float *x, float *y);
int idx_mao_em(float x, float y);
int idx_campo_em(int lado, float x, float y);

/* regras.c */
int mana_disp(int l);
void dano_jogador(int l, int dano);
void embaralhar(int *a, int n);
void montar_baralho(Jogador *j);
void fim_de_jogo(int venc);
void checar_vitoria(void);
void comprar(int l);
void remover_da_mao(Jogador *j, int im);
void limpar_mortos(int l);
int jogar_carta(int l, int im, int alvoTipo, int alvoIdx);
void resolver_combate(void);
void iniciar_turno(int l);
void terminar_turno(int l);
void novo_jogo(void);

/* ia.c */
int valor_criatura(const Criatura *c);
int ia_jogar_uma(void);
void ia_atacar(void);
void ia_bloquear(void);
void ia_passo(void);

/* jogador.c */
int contar_sel(void);
void declarar_ataque_humano(void);
int precisa_alvo(int id);
void clicar_mao(int im);
void acao_principal(void);

/* quiz.c */
void add_err(int v);
void licao(const char *a, const char *b, const char *c, const char *d);
int ipow(int b, int e);
const char *pn(int v);
void fmt_quadratica(char *out, size_t n, int a, int b, int c);
void fmt_log(char *out, size_t n, int base);
void quiz_exponencial(int d);
void quiz_logaritmo(int d);
void quiz_afim(int d);
void quiz_quadratica(int d);
void quiz_porcentagem(int d);
void quiz_financeira(int d);
int tem_valor(const int *v, int n, int x);
void montar_alternativas(void);
void gerar_quiz(int id);
void iniciar_quiz(int im, int alvoTipo, int alvoIdx);
void escolher_alternativa(int k);
void finalizar_quiz(void);

/* entrada.c */
void clique(int bt);
void teclado(int tecla);

/* tela_jogo.c */
void atualizar_escala(void);
void desenhar_botao(Ret r, const char *txt, int ativo);
void desenhar_carta(int id, float x, float y, float esc, int atq, int def, int dano);
void contorno(float x, float y, float w, float h, ALLEGRO_COLOR c, float esp);
int eh_atacante(int i);
void desenhar_criatura(int lado, int i);
ALLEGRO_COLOR cor_mana(int cor);
void desenhar_painel_jogador(int lado);
void desenhar_log(void);
void desenhar_jogo(void);

/* menu.c */
void desenhar_menu(void);
void desenhar_instrucoes(void);
void desenhar_dific(void);

/* tela_quiz.c */
void texto_ajustado(const char *txt, float y, ALLEGRO_COLOR cor);
void desenhar_quiz(void);

/* main.c */
void desenhar(void);
void atualizar(void);
void carregar_tudo(void);

#endif
