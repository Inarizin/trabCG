// **********************************************************************
// PUCRS/Escola Politecnica
// COMPUTACAO GRAFICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;


#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif
#include "Ponto.h"
#include "Instancia.h"
#include "ModeloMatricial.h"


#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

Temporizador T;
double AccumDeltaT=0;
Temporizador T2;

Instancia Personagens[500];//vai guardar as informações bases do personagem, inimigos e o spawn dos tiros
ModeloMatricial Modelos[15];
int AREA_DE_BACKUP = 250;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

bool desenha = false;

Poligono Mapa, MeiaSeta, Mastro;
int nInstancias=0;
int nModelos=0;
int ModeloCorrente;
int PersonagemAtual;
int tamanho = 50;
float angulo=0.0;
int vidas = 3;
void CriaInstancias();

// **********************************************************************
//
// **********************************************************************
void RotacionaAoRedorDeUmPonto(float alfa, Ponto P)
{
    glTranslatef(P.x, P.y, P.z);
    glRotatef(alfa, 0,0,1);
    glTranslatef(-P.x, -P.y, -P.z);
}

// **********************************************************************
//
// **********************************************************************
void CarregaModelos()
{
    //Mapa.LePoligono("EstadoRS.txt");
    //MeiaSeta.LePoligono("MeiaSeta.txt");
    //Mastro.LePoligono("Mastro.txt");
 
    Modelos[0].leModelo("MatrizNave.txt");
    //Modelos[0].Imprime();
    Modelos[1].leModelo("MatrizProjetilInimigo.txt");
    
    Modelos[2].leModelo("MatrizCoracao.txt");
    Modelos[3].leModelo("MatrizInimigo1.txt");
    Modelos[4].leModelo("MatrizInimigo2.txt");
    Modelos[5].leModelo("MatrizInimigo3.txt");
    Modelos[6].leModelo("MatrizDisparador.txt");
    Modelos[7].leModelo("MatrizProjetil.txt");
    /*
    Modelos[2].leModelo("NaveCaca.txt");
    Modelos[3].leModelo("NavePassageiros.txt");
    */
    nModelos = 8;
}

// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

    CarregaModelos();
    CriaInstancias();
 
    Min = Ponto(-tamanho,-tamanho);
    Max = Ponto(tamanho,tamanho);
}

double nFrames=0;
double TempoTotal=0;
double timer=0; //Utilizado para a atualização do tempo em segundos

// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=2;
        glutPostRedisplay();
    }
    //a cada segundo imprime o tempo total decorrido
    
    if (TempoTotal > 1.0)//A cada segundo atualiza esse dado
    {
        //Informações adicionais do programa
        //cout << "Tempo Acumulado: "  << ult_temp << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames;
        cout << " | FPS(sem desenho): " << nFrames/TempoTotal << endl;
        timer++;
        cout << endl <<"Tempo decorrido:" << timer <<endl<< endl;
        TempoTotal = 0;
        nFrames = 0;
    }
    
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x, Min.y,Max.y, -10,+10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// **********************************************************************
void SetaCor(int cor)
{
    defineCor(cor);
}
// **********************************************************************
void DesenhaLinha(Ponto P1, Ponto P2)
{
    glBegin(GL_LINES);
        glVertex3f(P1.x,P1.y,P1.z);
        glVertex3f(P2.x,P2.y,P2.z);
    glEnd();
}
// **********************************************************************
void DesenhaCelula()
{
    glBegin(GL_QUADS);
        glVertex2f(0,0);
        glVertex2f(0,1);
        glVertex2f(1,1);
        glVertex2f(1,0);
    glEnd();

}
// **********************************************************************
void DesenhaBorda()
{
    glBegin(GL_LINE_LOOP);
        glVertex2f(0,0);
        glVertex2f(0,1);
        glVertex2f(1,1);
        glVertex2f(1,0);
    glEnd();

}

// **********************************************************************
void DesenhaPersonagemMatricial()
{
    ModeloMatricial MM;
    
    int ModeloDoPersonagem = Personagens[PersonagemAtual].IdDoModelo;
    MM = Modelos[ModeloDoPersonagem];
      
    glPushMatrix();
    int larg = MM.nColunas;
    int alt = MM.nLinhas;
    //cout << alt << " LINHAS e " << larg << " COLUNAS" << endl;
    for (int i=0;i<alt;i++)
    {
       glPushMatrix();
       for (int j=0;j<larg;j++)
       {
           int cor = MM.getColor(alt-1-i,j);
           if (cor != -1) // nao desenha celulas com -1 (transparentes)
           {
               SetaCor(cor);
               DesenhaCelula();
               //defineCor(Wheat);//Monstra cada pixel
               //DesenhaBorda();//bordas da matriz interna do desenho (diferencia pixel por pixel)
           }
           glTranslatef(1, 0, 0);
       }
       glPopMatrix();
       glTranslatef(0, 1, 0);
    }
    glPopMatrix();

}

// **********************************************************************
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}

// **********************************************************************
void DesenhaSeta()
{
    glPushMatrix();
        MeiaSeta.desenhaPoligono();
        glScaled(1,-1, 1);
        MeiaSeta.desenhaPoligono();
    glPopMatrix();
}
// **********************************************************************
void DesenhaApontador()
{
    glPushMatrix();
        glTranslated(-4, 0, 0);
        DesenhaSeta();
    glPopMatrix();
}
// **********************************************************************
void DesenhaHelice()
{
    glPushMatrix();
    for(int i=0;i < 4; i++)
    {
        glRotatef(90, 0, 0, 1);
        DesenhaApontador();
    }
    glPopMatrix();
}
// **********************************************************************
void DesenhaHelicesGirando()
{
    glPushMatrix();
        glRotatef(angulo, 0, 0, 1);
        DesenhaHelice();
   glPopMatrix();
}
// **********************************************************************
void DesenhaMastro()
{
    Mastro.desenhaPoligono();
}
// **********************************************************************
void DesenhaCatavento()
{
    glLineWidth(3);
    glPushMatrix();
        defineCor(BrightGold);
        DesenhaMastro();
        glPushMatrix();
            glColor3f(1,0,0); // R, G, B  [0..1]
            glTranslated(0,3,0);
            glScaled(0.2, 0.2, 1);
            defineCor(YellowGreen);
            DesenhaHelicesGirando();
        glPopMatrix();
    glPopMatrix();
}
// **********************************************************************
// **********************************************************************
void DesenhaRS()
{
    Mapa.desenhaPoligono();
}

// **********************************************************************
// Esta funcao deve instanciar todos os personagens do cenario
// **********************************************************************
void CriaInstancias()
{

    //Informações do player == 0
    int i=0;
    float ang;
    ang = -45;
    Personagens[i].Posicao = Ponto (0,0);//Onde ele irá nascer
    Personagens[i].Escala = Ponto (1,1);
    Personagens[i].Rotacao = ang;
    Personagens[i].IdDoModelo = 0;
    Personagens[i].Tipo = 0;//Personagem principal
    Personagens[i].Pivot = Ponto(2.5,0);//Deslocamento do Envelope
    Personagens[i].Direcao = Ponto(0,1); // direcao do movimento para a cima
    Personagens[i].Direcao.rotacionaZ(ang); // direcao alterada para a direita
    Personagens[i].Velocidade = 4; // move-se a 10 m/s
    Personagens[i].modelo = DesenhaPersonagemMatricial;
    Personagens[i].t = 0.0;
    i++; 
    //----------------------------------------------------
    //Corações == 1,2,3
 
    for(; i < 4;i++){
        if (i==1)Personagens[i].Posicao = Ponto (38,46);
        else if (i==2)Personagens[i].Posicao = Ponto (42,46);
        else Personagens[i].Posicao = Ponto (46,46);
        Personagens[i].Escala = Ponto (0.6,0.6);
        Personagens[i].Tipo = 1;//Vidas
        Personagens[i].IdDoModelo = 2;
        Personagens[i].modelo = DesenhaPersonagemMatricial;
    }
    //----------------------------------------------------
    //Inimigos 4,5
    
     int j = 0; 
    for(; i< 14;i++){//ATÉ 10 INIMIGOS
        //variaveis que alteram a cada geração
        double x = (rand() % 2 == 0) ? -40.0 : 40.0;
        double y = (rand() % 2 == 0) ? -40.0 : 40.0;
        Personagens[i].Posicao = Ponto (x,y);// fazer val aleatório dentro do mapa
        Personagens[i].IdDoModelo = rand() % 4 + 3; //Fazer um valor random entre 3 a 6 (DEVEM SER 4 MODELOS)
        ang = (rand() % 361) - 180;
        //Declara o pivot de cada nave diferente
        if (Personagens[i].IdDoModelo==3) Personagens[i].Pivot = Ponto(6,0);
        else if(Personagens[i].IdDoModelo==4) Personagens[i].Pivot = Ponto(4,0);
        else if(Personagens[i].IdDoModelo==5) Personagens[i].Pivot = Ponto(5.5,0);
        else Personagens[i].Pivot = Ponto(8,0);
        //dados que são iguais para todos
        Personagens[i].Rotacao = ang;
        Personagens[i].Direcao = Ponto(0,1); // direcao do movimento para a cima 
        Personagens[i].Direcao.rotacionaZ(ang);
        Personagens[i].dead = true;//começa morto, para não aparecer+
        Personagens[i].Escala = Ponto (1,1);
        Personagens[i].Tipo = 2;//Inimigo
        Personagens[i].Velocidade = 4;
        Personagens[i].t = 0.0;//tempo do ultimo tiro realizado
        Personagens[i].modelo = DesenhaPersonagemMatricial;
        cout << "inimigo: "<< i << " criado"<< endl;
    }
    
    nInstancias = i; // esta variavel deve conter a quantidade total de personagens
    
}

// **********************************************************************
// Esta função testa a colisao entre os envelopes
// de dois personagens matriciais
// **********************************************************************
bool TestaColisao(int Objeto1, int Objeto2)
{

    //cout << "\n-----\n" << endl;
    //Personagens[Objeto1].ImprimeEnvelope("Envelope 1: ", "\n");
    //Personagens[Objeto2].ImprimeEnvelope("\nEnvelope 2: ", "\n");
    //cout << endl;
    // Testa todas as arestas do envelope de
    // um objeto contra as arestas do outro
   
    for(int i=0;i<4;i++)
    {
        Ponto A = Personagens[Objeto1].Envelope[i];
        Ponto B = Personagens[Objeto1].Envelope[(i+1)%4];
        for(int j=0;j<4;j++)
        {
            
//            cout << "Testando " << i << " contra " << j << endl;
//            Personagens[Objeto1].ImprimeEnvelope("\nEnvelope 1: ", "\n");
//            Personagens[Objeto2].ImprimeEnvelope("Envelope 2: ", "\n");

            Ponto C = Personagens[Objeto2].Envelope[j];
            Ponto D = Personagens[Objeto2].Envelope[(j+1)%4];

            
//            A.imprime("A:","\n");
//            B.imprime("B:","\n");
//            C.imprime("C:","\n");
//            D.imprime("D:","\n\n");
            
            if (HaInterseccao(A, B, C, D))
                return true;
        }
    }
    return false;
}

// **********************************************************************
// Esta função calcula o envelope do personagem matricial
// **********************************************************************
Ponto linha_cabeca[500];//pega a linha da cabeça de cada personagem
void AtualizaEnvelope(int personagem)
{
    Instancia I;
    I = Personagens[personagem];
    
    ModeloMatricial MM = Modelos[I.IdDoModelo];
    
    Ponto A;
    Ponto V;
    V = I.Direcao * (MM.nColunas/2.0) * I.Escala.x; // Considera a escala em x
    V.rotacionaZ(90);
    A = I.PosicaoDoPersonagem + V;
    
    Ponto B = A + I.Direcao*(MM.nLinhas) * I.Escala.y; // Considera a escala em y
    
    V = I.Direcao * (MM.nColunas) * I.Escala.x; // Considera a escala em x
    V.rotacionaZ(-90);
    Ponto C = B + V;
    
    V = -I.Direcao * (MM.nLinhas) * I.Escala.y; // Considera a escala em y
    Ponto D = C + V;
    
    linha_cabeca[personagem].x = (B.x+C.x)/2.0;
    linha_cabeca[personagem].y = (B.y+C.y)/2.0;

    // Desenha o envelope
    //defineCor(Red);
    //glBegin(GL_LINE_LOOP);
    //    glVertex2f(A.x, A.y);
    //    glVertex2f(B.x, B.y);
    //    glVertex2f(C.x, C.y);
    //    glVertex2f(D.x, D.y);
    //glEnd();
    
    // armazerna as coordenadas do envelope na instância
    Personagens[personagem].Envelope[0] = A;
    Personagens[personagem].Envelope[1] = B;
    Personagens[personagem].Envelope[2] = C;
    Personagens[personagem].Envelope[3] = D;
}

// **********************************************************************
//
// **********************************************************************
int kills =0;
int ult_spawn=0;
int ult_inimigo_spawn=4;//informa o inimigo anterior (no caso do 3 é um coração shhhhhh)
void AtualizaJogo()
{
    // Esta funcao deverá atualizar todos os elementos do jogo
    // em funcao das novas posicoes dos personagens
    // Entre outras coisas, deve-se:
    
    //  - calcular colisões
    // Para calcular as colisoes eh preciso fazer o calculo do envelopes de
    // todos os personagens

    for(int i=0; i<nInstancias;i++)
    {
        if(Personagens[i].dead)
            continue;
        AtualizaEnvelope(i);
    }
    // Feito o calculo, eh preciso testar todos os tiros e
    // demais personagens contra o jogador
    
    for(int i=4; i<nInstancias;i++) // comeca em 4 pois o 0 eh o personagem e 1->3
    {
        if(Personagens[i].dead || Personagens[i].Tipo == 3)//Se o personagem estiver morto ou for um tiro
            continue;
        if (TestaColisao(0,i)){
            //cout << Personagens[i].dead << endl;
            Personagens[i].dead = true;
            Personagens[vidas].dead = true;
            vidas--;
            if (Personagens[i].Tipo==2) kills++; // se realizar "ataque suicida, conta como kill"
        }
        
        for(int j=i; j<nInstancias;j++){
            if(j==i)
                continue;
            if(Personagens[i].Tipo != Personagens[j].Tipo)
            {
                if(TestaColisao(i,j)){
                    if((Personagens[i].Tipo==2 && Personagens[j].Tipo==4 )|| (Personagens[i].Tipo==4 && Personagens[j].Tipo==2))//se a colisão for do tiro, feito por um inimigo com o tiro de um outro inimigo, conitue
                        continue;
                    Personagens[i].dead = true;
                    Personagens[j].dead = true;
                    if(Personagens[j].Tipo==2 ||Personagens[i].Tipo==2 ){//Se for um inimigo que morreu, de alguma forma
                        kills++;
                    }
                }
            }
        }
    }

    if(ult_inimigo_spawn <15){
        boolean vivos = false;
        for (int i = 4; i < 14;i++){//Verifica se todos os inimigos estão mortos
            if(Personagens[i].dead==false){//se estiver vivo
                vivos = true;
            }
        }
        if(vivos == true){//Se tiver inimigos vivos
            if(timer>=ult_spawn+3){//Passou 3 segundos sem matar um inimigo,spawna o próximo
                Personagens[ult_inimigo_spawn].dead = false;
                ult_inimigo_spawn++;
                ult_spawn = timer;
            }
        }else{//Caso não tenha nenhum inimigo vivo, spawna o próximo
            Personagens[ult_inimigo_spawn].dead = false;
            ult_inimigo_spawn++;
            ult_spawn = timer;  
        }
    }

    if( vidas == 0 || kills == 10)//Jogo acaba, se vida zerar, matou todos os inimigos
    {
        //cout<< vidas << "  " << kills;
        exit ( 0 );
    }
    //  - remover/inserir personagens
    //  - atualizar áreas de mensagens e de icones
}
// **********************************************************************
void AtualizaPersonagens(float tempoDecorrido)
{
    for(int i=0; i<nInstancias;i++)
    {
        if(Personagens[i].dead)// se um personagem morrer, continua o jogo
            continue;
         
        if((Personagens[i].Tipo == 2 || Personagens[i].Tipo == 0) &&
         (linha_cabeca[i].x >= 50 || linha_cabeca[i].x <= -50 
         || linha_cabeca[i].y >= 50|| linha_cabeca[i].y <= -50))
        {
            float AnguloAtual = Personagens[i].Rotacao;
            Personagens[i].Direcao = Ponto(0,1); //aleatoriamente muda o angulo de movimento 
            Personagens[i].Direcao.rotacionaZ(90+AnguloAtual); // das naves inimigas
            Personagens[i].Rotacao = 90+AnguloAtual;
        }
        if(Personagens[i].Tipo == 2 && rand()%1000 <5){//Aleatoriza o movimento dos inimigos
            Personagens[i].Direcao = Ponto(0,1); //aleatoriamente muda o angulo de movimento 
            int ang = rand()%360;
            Personagens[i].Direcao.rotacionaZ(ang); // das naves inimigas
            Personagens[i].Rotacao = ang;
        }
        if(Personagens[i].Tipo ==2 && rand()%500 <10 && timer>Personagens[i].t+1 && !Personagens[i].dead){
            Personagens[nInstancias].Posicao = Ponto(linha_cabeca[i].x,linha_cabeca[i].y);
            Personagens[nInstancias].Rotacao = Personagens[i].Rotacao;
            Personagens[nInstancias].Direcao = Personagens[i].Direcao;
            Personagens[nInstancias].Escala = Ponto (1,1);
            Personagens[nInstancias].Pivot = Ponto (0.5,0);
            Personagens[nInstancias].Tipo = 4;//Tiro inimigo
            Personagens[nInstancias].IdDoModelo = 7;
            Personagens[nInstancias].Velocidade = 5 + Personagens[i].Velocidade;
            Personagens[nInstancias].modelo = DesenhaPersonagemMatricial;
            Personagens[i].t = timer;
            nInstancias++;
        }
        Personagens[i].AtualizaPosicao(tempoDecorrido);
    }
    AtualizaJogo();

}
// **********************************************************************
void DesenhaPersonagens()
{
    for(int i=0; i<nInstancias;i++)
    {
        if(Personagens[i].dead)
            continue;
        PersonagemAtual = i; 
        Personagens[i].desenha();
    }
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites logicos da area OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    
    //DesenhaEixos();
    
    DesenhaPersonagens();
    AtualizaPersonagens(T2.getDeltaT());
    
	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo numero de segundos e informa quanto frames
// se passaram neste periodo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
int limit_tiro = 10;//variavel que limita a quantidade de tiros em cadencia
double recarga_temp = 0.0;// tempo para a proxima recarga, recarregue depois de 2 segundo sem atirar
void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
            if(limit_tiro>0){//limita 10 tiros até recarregar
                if(timer>=recarga_temp+2){// Tempo de carregamento = 2 segundos
                    if(timer>=Personagens[0].t+1){//Cadencia de tiro a cada 1 segundo
                        //Personagens[nInstancias].Posicao = Personagens[0].PosicaoDoPersonagem;
                        Personagens[nInstancias].Posicao = Ponto(linha_cabeca[0].x,linha_cabeca[0].y);
                        Personagens[nInstancias].Rotacao = Personagens[0].Rotacao;
                        Personagens[nInstancias].Direcao = Personagens[0].Direcao;
                        Personagens[nInstancias].Escala = Ponto (1,1);
                        Personagens[nInstancias].Pivot = Ponto (0.5,0);
                        Personagens[nInstancias].Tipo = 3;//Tiro próprio
                        Personagens[nInstancias].IdDoModelo = 1;//"projetilInimigo.txt"
                        Personagens[nInstancias].Velocidade = 5 + Personagens[0].Velocidade;
                        Personagens[nInstancias].modelo = DesenhaPersonagemMatricial;
                        nInstancias++;
                        limit_tiro--;
                        Personagens[0].t=timer;
                        cout << "Tiros restantes "<< limit_tiro+1 << endl;
                    }
                }else{
                    cout<<"EM COULDOWN!"<<endl;
                }
            }else{//Caso a munição acabe, força recarregar
                cout <<endl<< "Recarregando" <<endl;
                recarga_temp=timer;
                limit_tiro=10;//reseta o cartucho
                cout<< endl<< "10 tiros disponiveis" << endl;
            }
            break;
        case 'r': //reloading sistem
            cout <<endl<< "Recarregando" <<endl;
            recarga_temp=timer;
            limit_tiro=10;//reseta o cartucho
            cout<< endl<< "10 tiros disponiveis" << endl;
            break;
        case 'a':
            
        break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
        case GLUT_KEY_LEFT:
            Personagens[0].Rotacao +=5;
            Personagens[0].Direcao.rotacionaZ(5);
            break;
        case GLUT_KEY_RIGHT:
            Personagens[0].Rotacao -=5;
            Personagens[0].Direcao.rotacionaZ(-5);
            break;
		case GLUT_KEY_UP:       // Se pressionar UP
            if(Personagens[0].Velocidade < 15)Personagens[0].Velocidade++;
            break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
            if( Personagens[0].Velocidade>0)Personagens[0].Velocidade--;
			break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 700, 700);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow    ( "Transformacoes Geometricas em OpenGL" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // sera chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

}

