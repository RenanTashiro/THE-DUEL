#include <ncurses.h>
#include <set>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>

/* Tela Principal */
#define INIT 0
#define NEW 1
#define SAVED 2
#define GAME 3
#define SAVED_GAME 4

/* Novo */
#define CLASSE 0
#define PLAYER1 1
#define PLAYER2 2
#define TOGAME 3

/* Jogar */
#define SETADO 1007

/* Global */
#define RUNNING 1

/* Misc */
#define limpa() clear()
#define mv(X,Y) move(X,Y)
#define foraddch(x,y) for(int i = 0; i < y; i++) x

using namespace std;

class Lutador {
private:
    void arqueiro() {
        alcance = 5;
        deslocamento = 3;
        vida = 30;
        ataque = 5;
        defesa = 3;
        magica = 15;
    }

    void guerreiro() {
        alcance = 1;
        deslocamento = 4;
        vida = 40;
        ataque = 8;
        defesa = 6;
        magica = 10;
    }

    void mago() {
        alcance = 3;
        deslocamento = 2;
        vida = 20;
        ataque = 3;
        defesa = 3;
        magica = 50;
    }

public:
    int alcance, deslocamento, vida, ataque, defesa, magica;
    int x, y;
    char classe[20], id[2];

    void suaClasse(char *classe) {
        strcpy(this->classe, classe);
        if(strcmp(classe, "Arqueiro") == 0) {
            arqueiro();
        }
        else if(strcmp(classe, "Guerreiro") == 0) {
            guerreiro();
        }
        else if(strcmp(classe, "Mago") == 0) {
            mago();
        }
        x = y = -1;
    }

    bool decrementoValido(int n) {
        if(strcmp(classe, "Arqueiro") == 0) {
            switch(n) {
                case 0: return deslocamento > 3;
                case 1: return vida         > 30;
                case 2: return ataque       > 5;
                case 3: return defesa       > 3;
            }
        }
        else if(strcmp(classe, "Guerreiro") == 0) {
            switch(n) {
                case 0: return deslocamento > 4;
                case 1: return vida         > 40;
                case 2: return ataque       > 8;
                case 3: return defesa       > 6;
            }
        }
        else {
            switch(n) {
                case 0: return deslocamento > 2;
                case 1: return vida         > 20;
                case 2: return ataque       > 3;
                case 3: return defesa       > 3;
            }
        }
    }

    bool semPosicao() {
        if(x == -1 && y == -1) {
            return true;
        }
        else {
            return false;
        }
    }

    void addId(int n) {
        id[0] = classe[0];
        id[1] = (char) n+48;
    }

    int danoBase(int defesaInimiga) {
        return (ataque/2) - (defesaInimiga/4);
    }

    int dano(int defesaInimiga) {
        int dmg = (int)(danoBase(defesaInimiga) * (1.0 + ((rand()%15+5) / 100.00)));
        return dmg > 0 ? dmg : 1;
    }

    void salva(const char *nome) {
        ofstream out(nome, ios::app);
        out << classe << "\n";
        out << id << "\n";
        out << alcance << "\n";
        out << deslocamento << "\n";
        out << vida << "\n";
        out << ataque << "\n";
        out << defesa << "\n";
        out << magica << "\n";
        out << x << ' ' << y << "\n";
        out.close();
    }
};

Lutador *players[2];

/* Tela Principal */
void telaPrincipal(int *stage);
void tela(int *stage);

/* Novo */
void novo(int *nstage);
void escolherClasse(int *stage); // Aqui
char* selecionarClasse();
void adicionarAtributos(char classes[4][2][20], int player);
void adicionarAtributosUtil(char *msg ,int boolean, int x, int y, int valor);

/* Jogo Salvo */
void carregar(int *stage);
void carregarLutadores(char* jogo); // Nao esta carregando as posicoes corretamente
void salvar(char* fileName, int turno, bool atck, bool def, bool mag, int tot1, int tot2);

/* Partida */
void prepararJogo(int *stage);
void escolherPosicao();
int primeiro();
void jogar(int stage);
void ganhador(int p1, int p2);
void mapa();
void mapa2(int x, int y);
void radius(int initX, int initY, int x, int y, int alc, int peso[][500], int X, int Y, set< pair<int,int> > &pos, int player, int desloc);

/* Imagens */
void espadaImg();
void arcoImg();
void cajadoImg();
void casteloImg();

/* Util */
void selecionado(char msg[], int x, int y);
void selecionado(char msg[], int x, int y, bool exp);
void selecionado(char msg[], int value, int x, int y, bool exp);
void selecionado(char msg[], char submsg[], int x, int y, bool exp);
void selecionado(char msg[], bool exp, int value, int x, int y);
void setas(int cmd, int &row, int &col);
void setas(int cmd, int &row, int &col, int rowinc, int colinc);
void setas(int cmd, int &row);
void limits(int &x, int low, int up);
void limitsxy(int &x, int &y, int lowx, int upx, int lowy, int upy);

int scrrow, scrcolumn;

int main()
{
    initscr();
    curs_set(FALSE);
    getmaxyx(stdscr, scrrow, scrcolumn);

    int stage = INIT;

    while(RUNNING) {
        switch(stage) {
            case INIT:          telaPrincipal(&stage); break;
            case NEW:           novo(&stage); break;
            case SAVED:         carregar(&stage); break;
            case GAME:          prepararJogo(&stage); break;
            case SAVED_GAME:    jogar(SAVED_GAME);
        }
    }

    endwin();

    return 0;
}

/* Tela Principal */
void telaPrincipal(int *stage)
{
    tela(stage);
}

void tela(int *stage)
{
    int row = 0;
    char opt[][10] = {"NOVO", "CARREGAR"}, titulo[] = "THE DUEL";

    while(*stage == INIT) {
        limpa();

        selecionado(titulo, scrrow/3, scrcolumn/3);

        for(int i = 0; i < 2; i++) {
            selecionado(opt[i], scrrow-(scrrow/3)+i, scrcolumn/3, i == row);
        }

        int cmd = getch();

        setas(cmd, row);

        if(cmd == 10) {
            if(row == 0)        *stage = NEW;
            else if(row == 1)   *stage = SAVED;
        }

        limits(row, 0, 1);

        getmaxyx(stdscr, scrrow, scrcolumn);
    }
}

/* Novo */
void novo(int *stage)
{
    int nstage = CLASSE;

    while(nstage == CLASSE) {
        switch(nstage) {
            case CLASSE: escolherClasse(&nstage); break;
        }
    }

    *stage = GAME;
}

void escolherClasse(int *nstage)
{
    char jogadores[4][2][20], player1[] = "PLAYER1", player2[] = "PLAYER2";
    int tot = 0;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 2; j++) {
            strcpy(jogadores[i][j], "Nenhuma");
        }
    }

    int row = 0, column = 0;

    while(*nstage == CLASSE) {
        limpa();

        selecionado(player1, scrrow/9, scrcolumn/8);
        selecionado(player2, scrrow/9, scrcolumn-(scrcolumn/8)-8);

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 2; j++) {
                selecionado(jogadores[i][j], 10+i+(i == 3 ? 10 : 0), j == 0 ? scrcolumn/8 : scrcolumn-(scrcolumn/8)-8, i == row && j == column);
            }
        }

        if(tot == 6) {
            mvprintw(scrrow-2, 2, "%s", "Pressione 'SPACE' para continuar");
        }

        int cmd = getch();

        setas(cmd, row, column);

        if(cmd == 10) {
            if(row != 3) {
                if(strcmp(jogadores[row][column], "Nenhuma") == 0) {
                    tot++;
                }
                strcpy( jogadores[row][column], selecionarClasse());
            }
        }
        else if(cmd == 32) {
            adicionarAtributos(jogadores, 0);
            adicionarAtributos(jogadores, 1);
            *nstage = TOGAME;
        }

        limitsxy(row, column, 0, 2, 0, 1);

        getmaxyx(stdscr, scrrow, scrcolumn);
    }
}

char* selecionarClasse()
{
    int row = 0;
    char opt[][20] = {"Arqueiro", "Guerreiro", "Mago"}, escolha[] = "Escolha uma classe:";
    char* retorno;

    while(RUNNING) {
        limpa();

        selecionado(escolha, scrrow/3-2, scrcolumn/8);

        for(int i = 0; i < 3; i++) {
            selecionado(opt[i], scrrow/3+i, scrcolumn/8, i == row);
        }

        switch(row) {
            case 0:             arcoImg();   break;
            case 1:             espadaImg(); break;
            case 2:             cajadoImg(); break;
        }

        switch(getch()) {
            case 65:            row--; break;
            case 66:            row++; break;
            case 10:            retorno = (char*) malloc(sizeof(char)*20);
                                strcpy(retorno, opt[row]); return retorno;
        }

        limits(row, 0, 2);

        getmaxyx(stdscr, scrrow, scrcolumn);
    }
}

void adicionarAtributos(char classes[4][2][20], int player)
{
    int row = 0, totinc = 10;

    players[player] = new Lutador[3];

    players[player][0].suaClasse(classes[0][player]);
    players[player][1].suaClasse(classes[1][player]);
    players[player][2].suaClasse(classes[2][player]);

    char attn[][20] = {"deslocamento", "vida", "ataque", "defesa"};

    while(RUNNING) {
        limpa();

        int x = scrrow/8, y = scrcolumn/8;

        attron(A_STANDOUT);
        mvprintw(x-3, y-3, "%s%d", "PLAYER", player+1);
        attroff(A_STANDOUT);

        for(int i = 0; i < 3; i++) {
            attron(A_STANDOUT);
            mvprintw(x++, y, "%s", players[player][i].classe);
            attroff(A_STANDOUT);
                adicionarAtributosUtil(attn[0], 0+(i*4) == row, ++x, y, players[player][i].deslocamento);
                adicionarAtributosUtil(attn[1], 1+(i*4) == row, ++x, y, players[player][i].vida);
                adicionarAtributosUtil(attn[2], 2+(i*4) == row, ++x, y, players[player][i].ataque);
                adicionarAtributosUtil(attn[3], 3+(i*4) == row, ++x, y, players[player][i].defesa);
            x += 4;
        }

        if(totinc == 0)
            mvprintw(scrrow-2, 2, "%s", "Pressione 'SPACE' para continuar");

        char pontos[] = "PONTOS: ";

        selecionado(pontos, totinc, scrrow/1.1, scrcolumn/1.5, true);

        switch(getch()) {
            case 65:            row--; break;
            case 66:            row++; break;
            case 67:            if(totinc > 0) {
                                    switch(row%4) {
                                        case 0: players[player][row/4].deslocamento++; break;
                                        case 1: players[player][row/4].vida++; break;
                                        case 2: players[player][row/4].ataque++; break;
                                        case 3: players[player][row/4].defesa++; break;
                                    } totinc--; break;
                                }
            case 68:            if(totinc < 10) {
                                    switch(row%4) {
                                        case 0: if(players[player][row/4].decrementoValido(0)) {players[player][row/4].deslocamento--; totinc++;} break;
                                        case 1: if(players[player][row/4].decrementoValido(1)) {players[player][row/4].vida--; totinc++;} break;
                                        case 2: if(players[player][row/4].decrementoValido(2)) {players[player][row/4].ataque--; totinc++;} break;
                                        case 3: if(players[player][row/4].decrementoValido(3)) {players[player][row/4].defesa--; totinc++;} break;
                                    } break;
                                }
            case 32:            if(totinc == 0) return;
        }

        limits(row, 0, 11);

        getmaxyx(stdscr, scrrow, scrcolumn);
    }
}

void adicionarAtributosUtil(char *msg ,int boolean, int x, int y, int valor) {
    if(boolean == 1) attron(A_REVERSE);
    mvprintw(x, y, "%s(%d)", msg, valor);
    mv(x, y+20);
    attron(A_REVERSE);
    for(int i = 0; i < valor; i++) {
        printw("%c", ' ');
    }
    attroff(A_REVERSE);
}

/* Jogo Salvo */
void carregar(int *stage)
{
    std::ifstream in("files");
    char id[10][100];
    int c = 0, row = 0;
    bool SALVO = true;

    if(in != NULL) {
        while(in >> id[c++]); strcpy(id[--c], "Retorne");

        while(SALVO) {
            limpa();
            attron(A_BOLD);
            mvprintw(scrrow-(scrrow/2)-2, scrcolumn/3, "%s (%d)", "ARQUIVOS", c);
            attroff(A_BOLD);

            for(int i = 0; i <= c; i++) {
                if(i == row)        attron(A_REVERSE);
                mvprintw(scrrow-(scrrow/2)+i+(i == c ? 2 : 0), scrcolumn/3, "%s%s", i == c ? "" : "./", id[i]);
                attroff(A_REVERSE);
            }

            switch(getch()) {
                case 65:            row--; break;
                case 66:            row++; break;
                case 10:            if(row == c) *stage = INIT, SALVO = false;
                                    else         *stage = SAVED_GAME, SALVO = false;
            }

            limits(row, 0, c);

            getmaxyx(stdscr, scrrow, scrcolumn);
        }
    }

    if(*stage == SAVED_GAME) {
        carregarLutadores(id[row]);
    }
}

// Temp
int turno, t1, t2;
bool atck, des, mag;

void carregarLutadores(char* jogo)
{
    char dir[100] = "jogos/";
    strcat(dir, jogo);

    std::ifstream in(dir);

    players[0] = new Lutador[3];
    players[1] = new Lutador[3];

    if(in != nullptr) {
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 3; j++) {
                in >> players[i][j].classe >> players[i][j].id >> players[i][j].alcance
                   >> players[i][j].deslocamento >> players[i][j].vida >> players[i][j].ataque
                   >> players[i][j].defesa >> players[i][j].magica >> players[i][j].x >> players[i][j].y;
            }
        }
    }

    in >> turno >> atck >> des >> mag >> t1 >> t2;
}

void salvar(char* fileName, int turno, bool ataque, bool desloc, bool magia, int tot1, int tot2)
{
    string str = "jogos/" + string(fileName);
    const char *jogo = str.c_str();
    char file[] = "files";

    ifstream in("files");
    vector<string> vs;
    vs.push_back(fileName);
    string s;

    while(in >> s) {
        if(find(vs.begin(), vs.end(), s) == vs.end())
        vs.push_back(s);
    }

    sort(vs.begin(), vs.end());

    ofstream out(jogo, ofstream::out | ofstream::trunc);
    ofstream ofile(file);

    for(auto a : vs) {
        ofile << a << "\n";
    }

    out.close();
    ofile.close();

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 3; j++) {
            players[i][j].salva(jogo);
        }
    }

    out.open(jogo, ios::app);

    out << turno << "\n" << ataque << ' ' << desloc << ' ' << magia << "\n";
    out << tot1 << ' ' << tot2 << "\n";

    out.close();
}

/* Partida */
void prepararJogo(int *stage)
{
    escolherPosicao();
    jogar(GAME);
}

void escolherPosicao()
{
    int row = 4, column = 6;
    int selectionRow = 0, i = 0;
    int jaPosicionado[100][100] = {{0}}, setado[2][3] = {{0}};
    bool selecionadoL = false;

    while(i < 6) {
        limpa();
        mapa();
        casteloImg();

        for(int p = 0; p < 2; p++) {
            for(int c = 0; c < 3; c++) {
                if(players[p][c].semPosicao() == false) {
                    mvprintw(players[p][c].x, players[p][c].y, "%s", players[p][c].id);
                }
            }
        }

        char msg[] = "PLAYER", selc[30];

        selecionado(msg, i%2 == 0 ? 1 : 2, 4, 50, true);

        if(!selecionadoL) {
            int c = 0;

            for(int j = 0; j < 3; j++) {
                if(players[i%2][j].semPosicao()) {
                    selecionado(players[i%2][j].classe, 6+(c++), 50, selectionRow == j);
                } else {
                    selecionado(players[i%2][j].classe, SETADO, 6+(c++), 49, selectionRow == j);
                }
            }

            int cmd = getch();

            setas(cmd, selectionRow);

            limits(selectionRow, 0, 2);

            if(cmd == 10 || cmd == 32 && !setado[i%2][selectionRow]) {
                strcpy(selc, players[i%2][selectionRow].classe);
                setado[i%2][selectionRow] = 1;
                selecionadoL = true;
            }
        }
        else {
            char dot[] = "  ";

            selecionado(selc, 6, 50);
            selecionado(dot, row, column);

            int cmd = getch();

            setas(cmd, row, column, 1, 2);

            if(cmd == 10 || cmd == 32 && !jaPosicionado[row][column]) {
                players[i%2][selectionRow].x = row;
                players[i%2][selectionRow].y = column;
                players[i%2][selectionRow].addId(i);
                jaPosicionado[row][column] = 1;
                selecionadoL = false;
                i++;
                //  Seleciona a primeira classe nao posicionada
                for(int j = 0; j < 3; j++) {
                    if(players[i%2][j].semPosicao()) {
                        selectionRow = j;
                        break;
                    }
                }
            }

            limitsxy(row, column, 4, 18, 6, 34);
        }
    }
}

int primeiro()
{
    int soma_player[2];

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 3; j++) {
            soma_player[i] += players[i][j].alcance + players[i][j].deslocamento + players[i][j].vida +
                              players[i][j].ataque + players[i][j].defesa + players[i][j].magica;
        }
    }

    if(soma_player[0] >= soma_player[1]) {
        return 0;
    } else {
        return 1;
    }
}

void jogar(int stage)
{
    int X = 9, Y = 55, tot1 = 3, tot2 = 3;
    int row = X+1, col = Y+1, turn = primeiro(), act = 0, prev_seta = 0;
    int comrow = -1, comcol = -1;
    char filename[] = "default";
    bool flag = false, ataque = false, desloc = false, magia = false;
    bool foiAtck = false, foiDesloc = false, foiMagia = false, salvo = false;

    set< pair<int,int> > valid_pos;
    int nr, nc, cmd;

    /** Converte pos **/
    if(stage == GAME) {
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 3; j++) {
                players[i][j].x = (players[i][j].x-4)*2+10;
                players[i][j].y = ((players[i][j].y-6)/2)*3+56;
            }
        }
    } else {
        turn = turno;
        foiAtck = atck;
        foiDesloc = des;
        foiMagia = mag;
        tot1 = t1;
        tot2 = t2;
    }

    while(RUNNING) {
        limpa();

        // Nao salva tot1 e tot2
        if(tot1 == 0 || tot2 == 0) {
            ganhador(tot1, tot2);
        }

        mapa2(X, Y);

        attron(A_REVERSE);
        mvprintw(5, 10, "Lutadores %d/%d", tot1, tot2);
        attroff(A_REVERSE);

        /** Imprime os Comando **/
        mvprintw(40, 10, "%s", "A - Ataque");
        mvprintw(41, 10, "%s", "D - Deslocamento");
        mvprintw(42, 10, "%s", "M - magia");
        mvprintw(43, 10, "%s", "S - Salva");
        mvprintw(44, 10, "%s", "SPACE - troca de lutador e/ou cancela a acao");

        /** Posiciona os lutadores **/
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 3; j++) {
                if(players[i][j].vida > 0) {
                    mvprintw(players[i][j].x, players[i][j].y, "%s", players[i][j].id);
                }
            }
        }

        /** left side **/
        int px = 10, py = 10;
        char id[] = "PLAYER", vida[] = "Vida\0",  mp[] = "MP";

        selecionado(id, 1, px, py, true);
        px += 2;

        for(int i = 0; i < 3; i++) {
            selecionado(players[0][i].classe, players[0][i].id, ++px, py, true);
            selecionado(vida, false, players[0][i].vida, ++px+1, py+5);
            printw(" ");
            attron(A_REVERSE);
            for(int j = 0; j < players[0][i].vida/2; j++) printw("%c", ' ');
            attroff(A_REVERSE);
            selecionado(mp, false, players[0][i].magica, ++px+1, py+5);
            printw("   ");
            attron(A_REVERSE);
            for(int j = 0; j < players[0][i].magica/2; j++) printw("%c", ' ');
            attroff(A_REVERSE);
            px += 2;
        }

        /** right side **/
        px = 10, py = 115;

        selecionado(id, 2, px, py, true);
        px += 2;

        for(int i = 0; i < 3; i++) {
            selecionado(players[1][i].classe, players[1][i].id, ++px, py, true);
            selecionado(vida, false, players[1][i].vida, ++px+1, py+5);
            printw(" ");
            attron(A_REVERSE);
            for(int j = 0; j < players[1][i].vida/2; j++) printw("%c", ' ');
            attroff(A_REVERSE);
            selecionado(mp, false, players[1][i].magica, ++px+1, py+5);
            printw("   ");
            attron(A_REVERSE);
            for(int j = 0; j < players[1][i].magica/2; j++) printw("%c", ' ');
            attroff(A_REVERSE);
            px += 2;
        }

        /** Jogar **/
        int rod = turn%2;

        selecionado(players[rod][act%3].id, players[rod][act%3].x, players[rod][act%3].y);

        if(flag) {
            char space[] = "  ", target[] = "[]";

            for(set< pair<int,int> >::iterator it = valid_pos.begin(); it != valid_pos.end(); ++it) {
                bool valid = true;
                for(int i = 0; i < 2; i++) {
                    for(int j = 0; j < 3; j++) {
                        if(players[i][j].x == it->first && players[i][j].y == it->second)
                            valid = false;
                    }
                }
                if(valid)
                    selecionado(space, it->first, it->second);
            }

            mvprintw(players[rod][act%3].x, players[rod][act%3].y, "%s", players[rod][act%3].id);
            selecionado(target, nr, nc);

            cmd = getch();

            // Deslocamento na area de movimento
            switch(cmd) {
                case 65: if(valid_pos.find(pair<int,int>(nr-2,nc)) != valid_pos.end()) nr -= 2;
                         else if(prev_seta == 67 && valid_pos.find(pair<int,int>(nr-2,nc+3)) != valid_pos.end()) nr -= 2, nc += 3, prev_seta = 0;
                         else if(prev_seta == 68 && valid_pos.find(pair<int,int>(nr-2,nc-3)) != valid_pos.end()) nr -= 2, nc -= 3, prev_seta = 0;
                         else prev_seta = 65;
                         break;
                case 66: if(valid_pos.find(pair<int,int>(nr+2,nc)) != valid_pos.end()) nr += 2;
                         else if(prev_seta == 67 && valid_pos.find(pair<int,int>(nr+2,nc+3)) != valid_pos.end()) nr += 2, nc += 3, prev_seta = 0;
                         else if(prev_seta == 68 && valid_pos.find(pair<int,int>(nr+2,nc-3)) != valid_pos.end()) nr += 2, nc -= 3, prev_seta = 0;
                         else prev_seta = 66;
                         break;
                case 67: if(valid_pos.find(pair<int,int>(nr,nc+3)) != valid_pos.end()) nc += 3;
                         else if(prev_seta == 66 && valid_pos.find(pair<int,int>(nr+2,nc+3)) != valid_pos.end()) nr += 2, nc += 3, prev_seta = 0;
                         else if(prev_seta == 65 && valid_pos.find(pair<int,int>(nr-2,nc+3)) != valid_pos.end()) nr -= 2, nc += 3, prev_seta = 0;
                         else prev_seta = 67;
                         break;
                case 68: if(valid_pos.find(pair<int,int>(nr,nc-3)) != valid_pos.end()) nc -= 3;
                         else if(prev_seta == 66 && valid_pos.find(pair<int,int>(nr+2,nc-3)) != valid_pos.end()) nr += 2, nc -= 3, prev_seta = 0;
                         else if(prev_seta == 65 && valid_pos.find(pair<int,int>(nr-2,nc-3)) != valid_pos.end()) nr -= 2, nc -= 3, prev_seta = 0;
                         else prev_seta = 68;
                         break;
            }
        }

        if(!flag)
            cmd = getch();

        if(cmd == 109 && !flag && !foiMagia && !foiAtck && players[rod][act%3].magica > 0) { // Magia
            int peso[500][500] = {{0}};
            radius(players[rod][act%3].x, players[rod][act%3].y, players[rod][act%3].x, players[rod][act%3].y, players[rod][act%3].alcance+1, peso, X, Y, valid_pos, rod, 0);
            nr = players[rod][act%3].x, nc = players[rod][act%3].y;
            comrow = rod, comcol = act%3;
            flag = true;
            ataque = false;
            desloc = false;
            magia = true;
        }
        if(cmd == 100 && !flag && !foiDesloc) { // Deslocamento
            int peso[500][500] = {{0}};
            radius(players[rod][act%3].x, players[rod][act%3].y, players[rod][act%3].x, players[rod][act%3].y, players[rod][act%3].deslocamento+1, peso, X, Y, valid_pos, rod, 1);
            nr = players[rod][act%3].x, nc = players[rod][act%3].y;
            comrow = rod, comcol = act%3;
            flag = true;
            ataque = false;
            desloc = true;
            magia = false;
        }
        else if(cmd == 97 && !flag && !foiAtck && !foiMagia) { // Ataque
            int peso[500][500] = {{0}};
            radius(players[rod][act%3].x, players[rod][act%3].y, players[rod][act%3].x, players[rod][act%3].y, players[rod][act%3].alcance+1, peso, X, Y, valid_pos, rod, 2);
            nr = players[rod][act%3].x, nc = players[rod][act%3].y;
            comrow = rod, comcol = act%3;
            flag = true;
            ataque = true;
            desloc = false;
            magia = false;
        }
        else if(cmd == 32){ // Troca de lutador
            flag = false;
            valid_pos.clear();
            act++;
            while(players[rod][act%3].vida <= 0) {
                act++;
            }
        }
        else if(cmd == 10 && flag) {
            if(desloc) {
                players[rod][act%3].x = nr, players[rod][act%3].y = nc;;
                foiDesloc = true;
            } else if(magia) {
                for(int i = 0; i < 2; i++) {
                    for(int j = 0; j < 3; j++) {
                        if(players[i][j].x == nr && players[i][j].y == nc) {
                            if(rod%2 == i) {
                                if(strcmp(players[comrow][comcol].classe, "Mago") == 0 && players[comrow][comcol].magica >= 2 ||
                                   strcmp(players[comrow][comcol].classe, "Mago") != 0 && players[comrow][comcol].magica >= 5   ) {
                                    players[i][j].vida += (strcmp(players[comrow][comcol].classe, "Mago") == 0 ? 4 : 2);
                                    players[i][act%3].magica -= (strcmp(players[comrow][comcol].classe, "Mago") == 0 ? 2 : 5);
                                }
                            } else if(strcmp(players[comrow][comcol].classe, "Mago") == 0 && players[comrow][comcol].magica >= 5){
                                players[i][j].vida -= 5;
                                players[rod%2][act%3].magica -= 5;
                                if(players[i][j].vida <= 0) {
                                    players[i][j].x = -1;
                                    players[i][j].y = -1;
                                    if(i == 0)     tot1--;
                                    else           tot2--;
                                }
                            }
                        }
                    }
                }
                foiMagia = true;
            } else if(ataque) {
                for(int i = 0; i < 3; i++) {
                    if(players[(rod+1)%2][i].x == nr && players[(rod+1)%2][i].y == nc) {
                        players[(rod+1)%2][i].vida -= players[rod][act%3].dano(players[(rod+1)%2][i].defesa); // Rever
                        if(players[(rod+1)%2][i].vida <= 0) {
                            players[(rod+1)%2][i].x = -1;
                            players[(rod+1)%2][i].y = -1;
                            if((rod+1)%2 == 0) tot1--;
                            else           tot2--;
                        }
                    }
                }
                foiAtck = true;
            }
            if((foiDesloc && foiAtck) || (foiAtck && foiMagia) || (foiDesloc && foiMagia)) {
                foiAtck = false;
                foiDesloc = false;
                foiMagia = false;
                turn++;
            }

            valid_pos.clear();
            flag = false;
        }
        else if(cmd == 115) {
             if(!salvo) {
                 mvprintw(46, 10, "%s", "NOME DO ARQUIVO: ");
                 mvscanw(46,27,"%s", filename);
                 salvo = true;
             } else {
                mvprintw(46, 10, "%s", "SALVO");
                getch();
             }
             salvar(filename , rod%2, foiAtck, foiDesloc, foiAtck, tot1, tot2);
        }

        limitsxy(row, col, 1+X, 29+X, 1+Y, 43+Y);
    }
}

// QUando um jogo salvo termina n imprime isso
// tot1 e tot2 nao estao funcionado direito
void ganhador(int p1, int p2)
{
    char msg[] = "PLAYER";
    strcat(msg, p1 == 0 ? "2 GANHOU" : "1 GANHOU");

    while(RUNNING) {
        limpa();
        selecionado(msg, scrrow/2, scrcolumn/3);
        getch();
    }
}

void mapa()
{
    mv(3,5);
    addch(ACS_ULCORNER);
    foraddch(addch(ACS_HLINE), 30);
    addch(ACS_URCORNER);
    foraddch((printw("\n     "),addch(ACS_VLINE), printw("                              "), addch(ACS_VLINE)), 15);
    printw("\n     ");
    addch(ACS_LLCORNER);
    foraddch(addch(ACS_HLINE), 30);
    addch(ACS_LRCORNER);
}

void mapa2(int x, int y)
{
    for(int i = 0; i < 15; i++) {
        mvprintw(x++, y, "%s", " -- -- -- -- -- -- -- -- -- -- -- -- -- -- --");
        mvprintw(x++, y, "%s", "|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |");
    }
    mvprintw(x, y, "%s", " -- -- -- -- -- -- -- -- -- -- -- -- -- -- --");
}

void radius(int initX, int initY, int x, int y, int alc, int peso[][500], int X, int Y, set< pair<int,int> > &pos, int player, int desloc)
{
    if(x < 1+X || x >= 30+X || y < 1+Y || y >= 45+Y) {
        return;
    }
    if(alc < 0) {
        return;
    }

    if(desloc == 1) {
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 3; j++) {
                if((x != initX || y != initY) && x == players[i][j].x && y == players[i][j].y) {
                    return;
                }
            }
        }
    } else if(desloc == 2){
        for(int i = 0; i < 3; i++) {
            if((x != initX || y != initY) && x == players[player][i].x && y == players[player][i].y) {
                return;
            }
        }
    }

    pos.insert(pair<int,int>(x,y));

    int tx[] = {-2, -2, -2, 0, 2, 2, 2, 0}, ty[] = {-3, 0, 3, 3, 3, 0, -3, -3};

    for(int i = 0; i < 8; i++) {
        if(peso[x+tx[i]][y+ty[i]] < alc-1) {
            peso[x+tx[i]][y+ty[i]] = alc-1;
            radius(initX, initY, x+tx[i], y+ty[i], alc-1, peso, X, Y, pos, player, desloc);
        }
    }
}

/* Images */
void espadaImg()
{
    int x = scrrow/15, y = 3.3;

    mvprintw(x++, scrcolumn/y, "%s", "                   _.--.    .--._"                          );
    mvprintw(x++, scrcolumn/y, "%s", "                 .'  .'      '.  '."                        );
    mvprintw(x++, scrcolumn/y, "%s", "                ;  .'    /\\    '.  ;"                      );
    mvprintw(x++, scrcolumn/y, "%s", "                ;  '._,-/  \\-,_.`  ;"                      );
    mvprintw(x++, scrcolumn/y, "%s", "                \\  ,`  / /\\ \\  `,  /"                    );
    mvprintw(x++, scrcolumn/y, "%s", "                 \\/    \\/  \\/    \\/"                    );
    mvprintw(x++, scrcolumn/y, "%s", "                 |  '_   \\/   _'  |"                       );
    mvprintw(x++, scrcolumn/y, "%s", "                 |_   ''-..-''   _|"                        );
    mvprintw(x++, scrcolumn/y, "%s", "                 | '-.        .-' |"                        );
    mvprintw(x++, scrcolumn/y, "%s", "                 |    '\\    /'    |"                       );
    mvprintw(x++, scrcolumn/y, "%s", "                 |      |  |      |"                        );
    mvprintw(x++, scrcolumn/y, "%s", "         ___     |      |  |      |     ___"                );
    mvprintw(x++, scrcolumn/y, "%s", "     _,-',  ',   '_     |  |     _'   ,'  ,'-,_"            );
    mvprintw(x++, scrcolumn/y, "%s", "   _(  \\  \\   \\'=--'-.  |  |  .-'--='/   /  /  )_"       );
    mvprintw(x++, scrcolumn/y, "%s", " ,'  \\  \\  \\   \\      '-'--'-'      /   /  /  /  '."    );
    mvprintw(x++, scrcolumn/y, "%s", "!     \\  \\  \\   \\                  /   /  /  /     !"   );
    mvprintw(x++, scrcolumn/y, "%s", ":      \\  \\  \\   \\                /   /  /  /      TK"  );
}

void arcoImg()
{
    int x = scrcolumn/10, y = scrcolumn/4;

    mvprintw(x++, y, "%s", "                                                         |"                 );
    mvprintw(x++, y, "%s", "                                                        \\."                );
    mvprintw(x++, y, "%s", "                                                        /|."                );
    mvprintw(x++, y, "%s", "                                                      /  `|."               );
    mvprintw(x++, y, "%s", "                                                    /     |."               );
    mvprintw(x++, y, "%s", "                                                  /       |."               );
    mvprintw(x++, y, "%s", "                                                /         `|."              );
    mvprintw(x++, y, "%s", "                                              /            |."              );
    mvprintw(x++, y, "%s", "                                            /              |."              );
    mvprintw(x++, y, "%s", "                                          /                |."              );
    mvprintw(x++, y, "%s", "     __                                 /                  `|."             );
    mvprintw(x++, y, "%s", "      -\\                              /                     |."            );
    mvprintw(x++, y, "%s", "        \\\\                          /                       |."           );
    mvprintw(x++, y, "%s", "          \\\\                      /                         |."           );
    mvprintw(x++, y, "%s", "           \\|                   /                           |\\"           );
    mvprintw(x++, y, "%s", "             \\#####\\          /                             ||"           );
    mvprintw(x++, y, "%s", "         ==###########>     /                               ||"             );
    mvprintw(x++, y, "%s", "          \\##==      \\    /                                 ||"           );
    mvprintw(x++, y, "%s", "     ______ =       =|__/___                                ||"             );
    mvprintw(x++, y, "%s", " ,--' ,----`-,__ ___/'  --,-`-==============================##==========>"  );
    mvprintw(x++, y, "%s", "\\Erorppn Xrzavgm'        ##_______ ______   ______,--,____,=##,_"          );
    mvprintw(x++, y, "%s", " `,    __==    ___,-,__,--'#'  ==='      `-'              | ##,-/"          );
    mvprintw(x++, y, "%s", "   `-,____,---'       \\####\\              |        ____,--\\_##,/*/"      );

}

void cajadoImg()
{
    int x = scrrow/4, y = scrcolumn/3;

    mvprintw(x++, y, "%s", "        '             .           ."   );
    mvprintw(x++, y, "%s", "     o       '   o  .     '   . O"     );
    mvprintw(x++, y, "%s", "  '   .   ' .   _____  '    .      ."  );
    mvprintw(x++, y, "%s", "   .     .   .mMMMMMMMm.  '  o  '   ." );
    mvprintw(x++, y, "%s", " '   .     .MMXXXXXXXXXMM.    .   '"   );
    mvprintw(x++, y, "%s", ".       . /XX77:::::::77XX\\ .   .   .");
    mvprintw(x++, y, "%s", "   o  .  ;X7:::''''''':::7X;   .  '"   );
    mvprintw(x++, y, "%s", "  '    . |::'.:'        '::| .   .  ." );
    mvprintw(x++, y, "%s", "     .   ;:.:.            :;. o   ."   );
    mvprintw(x++, y, "%s", "  '     . \\'.:            /.    '   .");
    mvprintw(x++, y, "%s", "     .     `.':.        .'.  '    ."   );
    mvprintw(x++, y, "%s", "   '   . '  .`-._____.-'   .  . '  ."  );
    mvprintw(x++, y, "%s", "    ' o   '  .   O   .   '  o    '"    );
    mvprintw(x++, y, "%s", "     . ' .  ' . '  ' O   . '  '   '"   );
    mvprintw(x++, y, "%s", "       . .'..' . ' ' . . '.  . '"      );
    mvprintw(x++, y, "%s", "        `.':.'        ':'.'.'"         );
    mvprintw(x++, y, "%s", "          `\\\\_  |     _//'"          );
    mvprintw(x++, y, "%s", "            \\(  |\\    )/"            );
    mvprintw(x++, y, "%s", "            //\\ |_\\  /\\\\"          );
    mvprintw(x++, y, "%s", "           (/ /\\(\" )/\\ \\)"         );
    mvprintw(x++, y, "%s", "            \\/\\ (  ) /\\/"           );
    mvprintw(x++, y, "%s", "               |(  )|"                 );
    mvprintw(x++, y, "%s", "               | \( \\"                );
    mvprintw(x++, y, "%s", "               |  )  \\"               );
    mvprintw(x++, y, "%s", "               |      \\"              );
    mvprintw(x++, y, "%s", "               |       \\"             );
    mvprintw(x++, y, "%s", "               |        `.__,"         );
    mvprintw(x++, y, "%s", "                \\_________.-'Ojo/gnv" );
}

void casteloImg()
{
    int x = 5, y = scrcolumn/3;
    mvprintw(x++, y, "%s", "                                 o");
    mvprintw(x++, y, "%s", "                               .-""|");
    mvprintw(x++, y, "%s", "                               |-""|");
    mvprintw(x++, y, "%s", "                                 |   ._--+.");
    mvprintw(x++, y, "%s", "                                .|-\"\"      '.");
    mvprintw(x++, y, "%s", "                               +:'           '.");
    mvprintw(x++, y, "%s", "                               | '.        _.-'|");
    mvprintw(x++, y, "%s", "                               |   +    .-\"   J");
    mvprintw(x++, y, "%s", "            _.+        .....'.'|    '.-\"      |");
    mvprintw(x++, y, "%s", "       _.-""   '.   ..'88888888|     +       J''..");
    mvprintw(x++, y, "%s", "    +:\"          '.'88888888888;-+.  |    _+.|8888:");
    mvprintw(x++, y, "%s", "    | \\         _.-+888888888_.\" _.F F +:'   '.8888'....");
    mvprintw(x++, y, "%s", "     L \\   _.-\"\"   |8888_.-\"  _.\" J J J  '.    +88888888:");
    mvprintw(x++, y, "%s", "     |  '+\"        |_.-\"  _.-\"    | | |    +    '.888888'._''.");
    mvprintw(x++, y, "%s", "   .'8L  L         J  _.-\"        | | |     '.    '.88_.-\"    '.");
    mvprintw(x++, y, "%s", "  :888|  |         J-\"            F F F       '.  _.-\"          '.");
    mvprintw(x++, y, "%s", " :88888L  L     _+  L            J J J          '|.               ';");
    mvprintw(x++, y, "%s", " :888888J  |  +-\\  \\ L          _.+.|.+.          F '.          _.-\" F");
    mvprintw(x++, y, "%s", ":8888888|  L L\\    \\|      _.-\"    '   '.       J    '.     .-\"    |");
    mvprintw(x++, y, "%s", ":8888888.L | | \\    ', _.-\"              '.     |      \"..-\"      J'.");
    mvprintw(x++, y, "%s", ":888888: |  L L '.    \\     _.-+.          '.   :+-.     |        F88'.");
    mvprintw(x++, y, "%s", ":888888:  L | |   \\    ;.-\"\"     '.          :-\"    \":+ J        |88888:");
    mvprintw(x++, y, "%s", ":888888:  |  L L   +:\"\"            '.    _.-\"     .-\" | |       J:888888:");
    mvprintw(x++, y, "%s", ":888888:   L | |   J \\               '.-'     _.-'   J J        F :888888:");
    mvprintw(x++, y, "%s", " :88888:    \\ L L   L \\             _.-+  _.-'       | |       |   :888888:");
    mvprintw(x++, y, "%s", " :888888:    \\| |   |  '.       _.-\"   |-\"          J J       J     :888888:");
    mvprintw(x++, y, "%s", " :888888'.    +'\\   J    \\  _.-\"       F    ,-T\"\\  | |     .-'      :888888:");
    mvprintw(x++, y, "%s", "  :888888 '.     \\   L    +\"          J    /  | J  J J  .-'        .'888888:");
    mvprintw(x++, y, "%s", "   :8888888 :     \\  |    |           |    F  '.|.-'+|-'         .' 8888888:");
    mvprintw(x++, y, "%s", "    :8888888 :     \\ J    |           F   J     '...           .' 888888888:");
    mvprintw(x++, y, "%s", "     :8888888 :     \\ L   |          J    |      \\88'.''.''''.' 8888888888:");
    mvprintw(x++, y, "%s", "      :8888888 :     \\|   |          |  .-'\\      \\8888888888888888888888:");
    mvprintw(x++, y, "%s", "       :8888888 '.    J   |          F-'  .'\\      \\8888888888888888888.'");
    mvprintw(x++, y, "%s", "         :88888888 :   |  |        .+  ...' 88\\      \\8888888888.''.'");
    mvprintw(x++, y, "%s", "          :88888888 :  J  |     .-'  .'    8888\\      \\'.'''.'.'");
    mvprintw(x++, y, "%s", "           :88888888 :  \\ |  .-'   .' 888888888.\\    _-'");
    mvprintw(x++, y, "%s", "            '.88888888'..         : 8888888.'");
    mvprintw(x++, y, "%s", "              :88888888  ''''.''.' 88888888:  hs");
    mvprintw(x++, y, "%s", "              :8888888888888888888888888888:");
    mvprintw(x++, y, "%s", "               :88888888888888888888888888:");
    mvprintw(x++, y, "%s", "                :888888888888888888888888:");
    mvprintw(x++, y, "%s", "                 ''.8888888888888...'.'''");
    mvprintw(x++, y, "%s", "                    '''''......''");
}

/* Util */
void selecionado(char *msg, int x, int y)
{
    attron(A_REVERSE);
    mvprintw(x, y, "%s", msg);
    attroff(A_REVERSE);
}

void selecionado(char* msg, int x, int y, bool exp)
{
    if(exp == true) {
        selecionado(msg, x, y);
    } else {
        mvprintw(x, y, "%s", msg);
    }
}

void selecionado(char msg[], int value, int x, int y, bool exp)
{
    if(exp == true) {
        attron(A_REVERSE);
        if(value == SETADO) mvprintw(x, y, "%c%s", '>', msg);
        else                mvprintw(x, y, "%s%d", msg, value);
        attroff(A_REVERSE);
    } else {
        if(value == SETADO) mvprintw(x, y, "%c%s", '>', msg);
        else                mvprintw(x, y, "%s%d", msg, value);
    }
}

void selecionado(char msg[], char submsg[], int x, int y, bool exp)
{
    if(exp == true) {
        attron(A_REVERSE);
        mvprintw(x, y, "%s(%s)", msg, submsg);
        attroff(A_REVERSE);
    } else {
        mvprintw(x, y, "(%s)", msg, submsg);
    }
}

void selecionado(char msg[], bool exp, int value, int x, int y)
{
    if(exp == true) {
        attron(A_REVERSE);
        mvprintw(x, y, "%s(%d)", msg, value);
        attroff(A_REVERSE);
    } else {
        mvprintw(x, y, "%s(%d)", msg, value);
    }

}

void setas(int cmd, int &row, int &col)
{
    switch(cmd) {
        case 65: row--; break;
        case 66: row++; break;
        case 67: col++; break;
        case 68: col--; break;
    }
}

void setas(int cmd, int &row, int &col, int rowinc, int colinc)
{
    switch(cmd) {
        case 65: row -= rowinc; break;
        case 66: row += rowinc; break;
        case 67: col += colinc; break;
        case 68: col -= colinc; break;
    }
}

void setas(int cmd, int &row)
{
    switch(cmd) {
        case 65:            row--; break;
        case 66:            row++; break;
    }
}

void limits(int &x, int low, int up)
{
    if(x < low) {
        x = low;
    } else if(x > up) {
        x = up;
    }
}

void limitsxy(int &x, int &y, int lowx, int upx, int lowy, int upy)
{
    if(x < lowx) {
        x = lowx;
    } else if(x > upx) {
        x = upx;
    }
    if(y < lowy) {
        y = lowy;
    } else if(y > upy) {
        y = upy;
    }
}
