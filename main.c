/* main.c - Programa principal: carrega tudo, laco do jogo, atualizar() e desenhar(). */
#include "jogo.h"

void desenhar(void)
{
    ALLEGRO_TRANSFORM t;
    atualizar_escala();
    al_identity_transform(&t);
    al_use_transform(&t);
    al_set_clipping_rectangle(0, 0, al_get_display_width(disp), al_get_display_height(disp));
    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_identity_transform(&t);
    al_scale_transform(&t, escala, escala);
    if (tremor > 0) {                             /* tela tremendo (erro / dano) */
        float f = tremorForca * (tremor / 26.0f > 1 ? 1 : tremor / 26.0f);
        al_translate_transform(&t, sinf(tempo * 2.7f) * f, cosf(tempo * 3.3f) * f);
    }
    al_translate_transform(&t, offx, offy);
    al_use_transform(&t);
    al_set_clipping_rectangle((int)offx, (int)offy, (int)(LARG * escala), (int)(ALT * escala));

    if (estado == E_MENU) desenhar_menu();
    else if (estado == E_INSTR) desenhar_instrucoes();
    else if (estado == E_DIFIC) desenhar_dific();
    else {
        char buf[80];
        desenhar_jogo();
        snprintf(buf, sizeof(buf), "Luta %d  |  Combo x%.1f  |  B\303\264nus dano +%d", luta + 1, pow(1.5, streak), bonusDano);
        al_draw_text(f_p, al_map_rgb(255, 230, 150), 130, 8, 0, buf);
        if (estado == E_QUIZ) desenhar_quiz();
        desenhar_efeitos();
    }
}

void atualizar(void)
{
    int i;
    tempo++;
    for (i = 0; i < 40; i++) if (flut[i].t > 0) flut[i].t--;
    atualizar_efeitos();
    if (estado == E_QUIZ && quizEsc >= 0) {
        quizTimer++;
        if (quizTimer >= (quizAcertou ? 80 : 170)) finalizar_quiz();
    }
    if (estado == E_IA) {
        if (iaTimer > 0) iaTimer--;
        else ia_passo();
    }
}

void carregar_tudo(void)
{
    static const char *molduras[4] = { "fogo", "agua", "floresta", "luz" };
    char nome[128];
    int i;

    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    img_fundo = carregar("imagens/fundo_arena.png");
    img_menu = carregar("imagens/fundo_menu.png");
    img_titulo = carregar("imagens/titulo.png");
    img_verso = carregar("imagens/carta_verso.png");
    img_ret[0] = carregar("imagens/retrato_jogador.png");
    img_ret[1] = carregar("imagens/retrato_oponente.png");
    for (i = 0; i < 5; i++) {
        snprintf(nome, sizeof(nome), "imagens/monstro_%d.png", i);
        img_monstro[i] = carregar(nome);
    }
    img_mana = carregar("imagens/icone_mana.png");
    img_vida = carregar("imagens/icone_vida.png");
    for (i = 0; i < 4; i++) {
        snprintf(nome, sizeof(nome), "imagens/moldura_%s.png", molduras[i]);
        img_moldura[i] = carregar(nome);
    }
    for (i = 0; i < NUM_CARTAS; i++) {
        snprintf(nome, sizeof(nome), "imagens/arte_%02d.png", i);
        img_arte[i] = carregar(nome);
    }

    f_p = carregar_fonte("fontes/fonte_ui.ttf", 15);
    f_n = carregar_fonte("fontes/fonte_ui.ttf", 20);
    f_g = carregar_fonte("fontes/fonte_ui.ttf", 34);
    f_t = carregar_fonte("fontes/fonte_titulo.ttf", 64);
    f_cnome = carregar_fonte("fontes/fonte_titulo.ttf", 16);
    f_ctexto = carregar_fonte("fontes/fonte_texto.ttf", 13);
    f_cnum = carregar_fonte("fontes/fonte_ui.ttf", 20);

    snd_clique = carregar_som("sons/clique.wav");
    snd_jogar = carregar_som("sons/jogar.wav");
    snd_golpe = carregar_som("sons/golpe.wav");
    snd_cura = carregar_som("sons/cura.wav");
    snd_feitico = carregar_som("sons/feitico.wav");
    snd_vitoria = carregar_som("sons/vitoria.wav");
    snd_derrota = carregar_som("sons/derrota.wav");
    snd_musica = carregar_som("sons/musica.wav");
    snd_acerto = carregar_som("sons/acerto.wav");
    snd_erro = carregar_som("sons/erro.wav");
    snd_escudo = carregar_som("sons/escudo.wav");
}

int main(void)
{
    ALLEGRO_EVENT_QUEUE *fila;
    ALLEGRO_TIMER *timer;
    int rodando = 1, redesenhar = 1;

    srand((unsigned)time(NULL));

    if (!al_init()) { fprintf(stderr, "Falha ao iniciar o Allegro.\n"); return 1; }
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    audio_ok = al_install_audio() && al_init_acodec_addon() && al_reserve_samples(12);

    al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_WINDOWED);
    disp = al_create_display(LARG, ALT);
    if (!disp) { fprintf(stderr, "Falha ao criar a janela.\n"); return 1; }
    al_set_window_title(disp, "Math Wars - Duelo de Contas");

    carregar_tudo();

    fila = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);
    al_register_event_source(fila, al_get_display_event_source(disp));
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_start_timer(timer);

    if (audio_ok && snd_musica)                    /* música de fundo em loop */
        al_play_sample(snd_musica, 0.35f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, NULL);

    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);
        atualizar_escala();

        switch (ev.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            rodando = 0;
            break;
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(disp);
            break;
        case ALLEGRO_EVENT_TIMER:
            atualizar();
            redesenhar = 1;
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            mx = (ev.mouse.x - offx) / escala;
            my = (ev.mouse.y - offy) / escala;
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            mx = (ev.mouse.x - offx) / escala;
            my = (ev.mouse.y - offy) / escala;
            clique(ev.mouse.button);
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            teclado(ev.keyboard.keycode);
            break;
        }
        if (sair) rodando = 0;

        if (redesenhar && al_is_event_queue_empty(fila)) {
            desenhar();
            al_flip_display();
            redesenhar = 0;
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(fila);
    al_destroy_display(disp);
    return 0;
}
