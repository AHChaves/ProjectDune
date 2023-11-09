#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//DEFINE

#define NOME_TAM    (64) + 1
#define SENHA_TAM   (16) + 1
#define TEXTO_TAM   (256) + 1

#define ARQUIVO_USUARIOS "users.bin"
#define ARQUIVO_POSTS "posts.bin"

// Tamanho do buffer para cada linha da imagem
#define BUFFER_TAMANHO 50000

// Tamanho m�ximo da linha de comando
#define LINHA_COMANDO 10000

// Limiar inferior que considera o download com sucesso
#define LIMIAR_INFERIOR_TAMANHO_IMAGEM 500

// N�mero de colunas da imagem
#define IMAGEM_NUM_COLUNAS_PADRAO 120

// Defini��o de imagem utilizada
#define IMAGEM_TER_COR_PADRAO false

#define IMAGEM_NUM_COLUNAS_PADRAO 120

// Defini��o de imagem colorida
#define IMAGEM_COLORIDA true
// Defini��o de imagem preto/branco
#define IMAGEM_PRETO_BRANCO false
// Defini��o de imagem utilizada
#define IMAGEM_TER_COR_PADRAOIMAGEM_COLORIDA

// Nome do execut�vel da ferramenta de download e convers�o da imagem
#ifdef __unix__
#define FERRAMENTA_IMAGEM "./ascii-image-converter.bin"
#else
#define FERRAMENTA_IMAGEM "ascii-image-converter.exe"
#endif

// Nome do arquivo de sa�da tempor�rio da imagem
#define ARQUIVO_IMAGEM_TMP "ascii_art.txt"

/**
 *  \brief Fun��o que carrega uma imagem informada na URL.
 *
 *  \param [in] colorido Define se a imagem ser� colorida.
 *  \param [in] largura Define a largura da imagem gerada.
 *  \return Endere�o da estrutura com a imagem.
 *          Caso a imagem n�o tenha sido carregada corretamente, a fun��o
 *          retornar� NULL.
 */

//STRUCTS

typedef enum 
{
    Ordena_Id = 0,
    Ordena_Nome = 1,
    Ordena_Email = 2

} E_OrdenarPor;


typedef struct
{
    unsigned int Id;
    unsigned int Perfil_Id;
    unsigned int Postagem_Id;
    char Mensagem[TEXTO_TAM];
} S_Comentario;

typedef struct 
{
    unsigned int Quantidade;
    S_Comentario *Comentarios;
} S_ArrayComentarios;
typedef struct 
{
    unsigned int Quantidade;
    unsigned int *Id;
} Array_Id_t;

typedef struct
{
    unsigned int Id;
    char Nome[NOME_TAM];
    char Email[NOME_TAM];
    char Senha[SENHA_TAM];
    char Bio[NOME_TAM];

    Array_Id_t PostagensCurtidas;
    Array_Id_t PostagensComentadas;
} S_Usuario;

typedef struct
{
    S_Usuario* Usuarios;
    unsigned int Quantidade;
} S_ArrayUsuarios;

struct asciiImg_s
{
    uint8_t *bytes;
    int nBytes;
};

/// Tipo "Imagem ASCII"
typedef struct asciiImg_s asciiImg_t;
typedef struct 
{
    unsigned int Quantidade;
    asciiImg_t** Imagens;
} S_ArrayImagens;

typedef struct
{
    unsigned int Id;
    unsigned int Autor_Id;
    char Descrit[TEXTO_TAM];
    S_ArrayImagens Imagens;
    
    Array_Id_t QuemCurtiu;
    S_ArrayComentarios Comentarios;
} S_Postagem;

typedef struct 
{
    unsigned int Quantidade;
    S_Postagem* Postagens;
} S_ArrayPostagens;

//COISAS FEIAS
/*
extern S_ArrayUsuarios Usuarios;
extern S_ArrayPostagens Postagens;
extern S_Usuario* Usuario_Logado;
extern S_Usuario* Usuario_Ativo;
*/

S_ArrayUsuarios Vetor_Usuarios; //= (S_ArrayUsuarios) {0, NULL};
S_ArrayPostagens Vetor_Postagens;
S_Usuario* Usuario_Logado = NULL;
S_Usuario* Usuario_Ativo = NULL;

//FUNCOES GERAIS

void Liberar_ArrayId(Array_Id_t* array)
{
    if(array->Id != NULL)
        free(array->Id);
    array->Id = NULL;
    array->Quantidade = 0;
    return;
}

void Retira_quebra_de_linha(char* texto)
{
    texto[strlen(texto) - 1] = '\0';
}

bool AcharParte_NaFrase(char* base, char* achar)
{
    unsigned int Tamanho_Base = strlen(base);
    unsigned int Tamanho_Achar = strlen(achar);
    unsigned int Letras_Coincidentes = 0;

    for(int i = 0; i < Tamanho_Base - Tamanho_Achar + 1; i++)
    {
        Letras_Coincidentes = 0;
        for(int j = 0; j < Tamanho_Achar; j++)
        {
            if(base[i+j] != achar[j])
                break;
            else Letras_Coincidentes++;
        }

        if(Letras_Coincidentes == Tamanho_Achar)
            return true;
    }

    return false;
}

bool Compara_Frases(char* frase1, char* frase2)
{
    unsigned int Tamanho1 = strlen(frase1);
    unsigned int Tamanho2 = strlen(frase2);

    for(int i = 0; i < Tamanho1 && i < Tamanho2; i++)
    {
        if(frase1[i] < frase2[i])
            return true;
        else if(frase1[i] > frase2[i])
            return false;
    }
    
    if(Tamanho1 < Tamanho2)
        return true;
    else return false;
}

void Alocar_Comentario(S_ArrayComentarios* array)
{
    (array->Quantidade)++;
    if(array->Quantidade == 1)
        array->Comentarios = (S_Comentario *) malloc(sizeof(S_Comentario));
    else
        array->Comentarios = (S_Comentario *) realloc(array->Comentarios, array->Quantidade * sizeof(S_Comentario));

    if(array->Comentarios == NULL)
        printf("ERRO: Alocagem de comentario");    
    return;
}

void Alocar_Imagem(S_ArrayImagens* array)
{
    (array->Quantidade)++;
    if(array->Quantidade == 1)
        array->Imagens = (asciiImg_t **) malloc(sizeof(asciiImg_t* ));
    else
        array->Imagens = (asciiImg_t **) realloc(array->Imagens, array->Quantidade * sizeof(asciiImg_t *));

    if(array->Imagens == NULL)
        printf("ERRO: Alocagem de imagens");    
    return;
}

void Alocar_ArrayId(Array_Id_t* array)
{
    (array->Quantidade)++;
    if(array->Quantidade == 1)
        array->Id = (unsigned int *) malloc(sizeof(unsigned int));
    else
        array->Id = (unsigned int *) realloc(array->Id, array->Quantidade * sizeof(unsigned int));

    if(array->Id == NULL)
        printf("ERRO: Alocagem de chave-usuario");    
    return;
}


void Alocar_ArrayId_ComValor(Array_Id_t* array, unsigned int novo_valor)
{
    Alocar_ArrayId(array);
    array->Id[array->Quantidade-1] = novo_valor;
    return;
}

void Desalocar_ArrayId(Array_Id_t* array)
{
    if(array->Quantidade == 0)
        return;

    (array->Quantidade)--;
    if(array->Quantidade == 0)
        Liberar_ArrayId(array);
    else
    {
        array->Id = (unsigned int *) realloc(array->Id, array->Quantidade * sizeof(unsigned int));
        if(array->Id == NULL)
            printf("ERRO: De-alocagem de chave-usuario");    
    }
    return;
}

void Remove_Posicao_ArrayId(Array_Id_t* array, int posicao)
{
    array->Id[posicao] = array->Id[array->Quantidade-1];
    Desalocar_ArrayId(array);
}

void Remove_Id_DoArray(Array_Id_t* array, unsigned int valor)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(array->Id[i] == valor)
            Remove_Posicao_ArrayId(array, i);
    }
    return;
}

//CADASTRO

//Funcao que alocadinamicamente um usuario
void Alocar_Usuario_Dinamicamente(S_ArrayUsuarios* usuario)
{
    (usuario->Quantidade)++; //aumenta quantos usuarios tem.

    if(usuario->Quantidade == 1) // se não foi inicializado, aloca dinamicamente com malloc 
        usuario->Usuarios = (S_Usuario *) malloc(sizeof(S_Usuario));
    else
        usuario->Usuarios = (S_Usuario *) realloc(usuario->Usuarios, usuario->Quantidade * sizeof(S_Usuario));
        //caso tenha um (1) usuario realoca o tamanho do vetor de usuario pra um tamanho maior

    //Se a alocacao falhar imprime uma mensagem de erro
    if(usuario->Usuarios == NULL)
        printf("Erro na alocagem de usuario");    
    return;
}

bool Verifica_Email_Existe( S_ArrayUsuarios* array,  char* email)
{
    for(int i = 0; i < array->Quantidade - 1; i++)
    {
        if(strcmp(array->Usuarios[i].Email, email) == 0)
            return false;
    }
    return true;
}

bool Verifica_Email_Valido( char* email)
{
    int Tamanho_Email = strlen(email);
    for(int i = 0; i < Tamanho_Email; i++)
    {
        if(email[i] != '@') continue;

        for(i; i < Tamanho_Email; i++)
        {
            if(email[i] != '.') continue;

            if(i < Tamanho_Email - 2 && email[i + 1] != ' ')
                return true;
        }
    }
    return false;
}

void Cadastrar_NovoUsuario(S_ArrayUsuarios* novo_Usuario)
{
    Alocar_Usuario_Dinamicamente(novo_Usuario);
    S_Usuario* UsuarioNovo = &(novo_Usuario->Usuarios[novo_Usuario->Quantidade - 1]);
    UsuarioNovo->Id = novo_Usuario->Quantidade;

    //inicializa
    UsuarioNovo->PostagensCurtidas.Quantidade = 0;
    UsuarioNovo->PostagensCurtidas.Id = NULL;
    UsuarioNovo->PostagensComentadas.Quantidade = 0;
    UsuarioNovo->PostagensComentadas.Id = NULL;

    printf(">> Digite o nome do usuario: ");
    fgets(UsuarioNovo->Nome, NOME_TAM, stdin);
    Retira_quebra_de_linha(UsuarioNovo->Nome);

    do
    {
        printf(">> Digite o email do usuario: ");
        fgets(UsuarioNovo->Email, NOME_TAM, stdin);
        Retira_quebra_de_linha(UsuarioNovo->Email);

        if(Verifica_Email_Valido(UsuarioNovo->Email) == false)
            printf(">>> Email fornecido e invalido! <<<\n");
        else if (Verifica_Email_Existe(novo_Usuario, UsuarioNovo->Email) == false)
            printf(">>> Este email ja esta em uso! <<<\n");
        else break;
    }
    while (1);
    

    printf(">> Digite a senha do usuario: ");
    fgets(UsuarioNovo->Senha, SENHA_TAM, stdin);
    Retira_quebra_de_linha(UsuarioNovo->Senha);

    printf(">> Digite uma bio para o usuario: ");
    fgets(UsuarioNovo->Bio, NOME_TAM, stdin);
    Retira_quebra_de_linha(UsuarioNovo->Bio);

    printf(">> Cadastro concluido <<\n\n");
}

void Liberar_Usuarios(S_ArrayUsuarios* array)
{
    if(array->Usuarios != NULL)
        free(array->Usuarios);
    array->Usuarios = NULL;
    array->Quantidade = 0;
    return;
}

void Liberar_Usuarios_Profundo(S_ArrayUsuarios* array)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        Liberar_ArrayId(&array->Usuarios[i].PostagensComentadas);
        Liberar_ArrayId(&array->Usuarios[i].PostagensCurtidas);
    }
    if(array->Usuarios != NULL)
        free(array->Usuarios);
    array->Usuarios = NULL;
    array->Quantidade = 0;
    return;
}


void Adicionar_Usuario_NoArray(S_ArrayUsuarios* array, S_Usuario* novo_usuario)
{
    Alocar_Usuario_Dinamicamente(array);
    array->Usuarios[array->Quantidade-1] = *novo_usuario;
}

S_Usuario* Achar_Usuario_PorEmail(S_ArrayUsuarios* array, char* email)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(strcmp(array->Usuarios[i].Email, email) == 0)
            return &array->Usuarios[i];
    }
    return NULL;
}


S_Usuario* Achar_Usuario_PorId( S_ArrayUsuarios* array, unsigned int id)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(array->Usuarios[i].Id == id)
            return &array->Usuarios[i];
    }
    return NULL;
}

S_Usuario* Logar_Usuario( S_ArrayUsuarios* array)
{
    int Escolha = 0;
    char Email_Busca[NOME_TAM];
    char Senha_Busca[SENHA_TAM];
    S_Usuario* Encontrado;

    do
    {
        Encontrado = NULL;
        
        printf("[1] Logar com o ID\n");
        printf("[2] Logar com o Email\n");
        printf("[0] Cancelar\n");

        printf(">> ");
        scanf("%d", &Escolha);
        getchar();

        switch (Escolha)
        {
        case 0:
            return NULL;
        case 1:
            printf(">> Digite o ID: ");
            scanf("%d", &Escolha);
            getchar();
            Encontrado = Achar_Usuario_PorId(array, (unsigned int) Escolha);
            break;
        case 2:
            printf(">> Digite o email (Completo): ");
            fgets(Email_Busca, NOME_TAM, stdin);
            Retira_quebra_de_linha(Email_Busca);
            Encontrado = Achar_Usuario_PorEmail(array, Email_Busca);
            break;
        default:
            continue;
        }

        printf(">> Digite a senha: ");
        fgets(Senha_Busca, SENHA_TAM, stdin);
        Retira_quebra_de_linha(Senha_Busca);


        if(Encontrado == NULL)
        {
            printf("Email/ID invalido!\n");
            continue;
        }
        else if(strcmp(Encontrado->Senha, Senha_Busca) != 0)
        {
            printf("Senha invalida para esse usuario!!\n");
            continue;
        }
        else
        {
            printf("Logado como %s #%04u\n", Encontrado->Nome, Encontrado->Id);
            return Encontrado;
        }
    } while (1);
}




void Deslogar_Usuario(S_Usuario** usuario)
{
    *usuario = NULL;
}


void Alterar_Senha_Usuario(S_Usuario* usuario)
{
    char Senha_Antiga[SENHA_TAM];
    char Senha_Nova[SENHA_TAM];
    
    printf(">> Digite a senha atual: ");
    fgets(Senha_Antiga, SENHA_TAM, stdin);
    Retira_quebra_de_linha(Senha_Antiga);

    if(strcmp(usuario->Senha, Senha_Antiga) != 0)
    {
        printf("A senha digitada esta incorreta!\n");
        return;
    }

    printf(">> Digite a nova senha: ");
    fgets(Senha_Nova, SENHA_TAM, stdin);
    Retira_quebra_de_linha(Senha_Nova);

    strcpy(usuario->Senha, Senha_Nova);
    printf(">> Senha alterada com sucesso! <<\n");
    return;
}


void Alterar_Nome_Usuario(S_Usuario* usuario)
{
    printf(">> Digite o novo nome: ");
    fgets(usuario->Nome, NOME_TAM, stdin);
    Retira_quebra_de_linha(usuario->Nome);

    printf(">> Nome alterado com sucesso! <<\n");
    return;
}


void Alterar_Email_Usuario(S_Usuario* usuario)
{
    char Senha_Confirma[SENHA_TAM];
    
    printf(">> Digite a senha atual: ");
    fgets(Senha_Confirma, SENHA_TAM, stdin);
    Retira_quebra_de_linha(Senha_Confirma);

    if(strcmp(usuario->Senha, Senha_Confirma) != 0)
    {
        printf("A senha digitada esta incorreta!\n");
        return;
    }

    printf(">> Digite o novo email: ");
    fgets(usuario->Email, NOME_TAM, stdin);
    Retira_quebra_de_linha(usuario->Email);

    printf(">> Email alterado com sucesso! <<\n");
    return;
}


void Alterar_Bio_Usuario(S_Usuario* usuario)
{
    printf(">> Escreva uma breve descricao sua: ");
    fgets(usuario->Bio, NOME_TAM, stdin);
    Retira_quebra_de_linha(usuario->Bio);

    printf(">> Sua Bio foi atualizada com sucesso! <<\n");
    return;
}

int Checar_Usuario_CurtiuPost( S_Usuario* usuario, unsigned int post_id)
{
    for(int i = 0; i < usuario->PostagensCurtidas.Quantidade; i++)
    {
        if(usuario->PostagensCurtidas.Id[i] == post_id)
            return i;
    }
    return -1;
}


int Checar_Usuario_Comentou( S_Usuario* usuario, unsigned int post_id)
{
    for(int i = 0; i < usuario->PostagensComentadas.Quantidade; i++)
    {
        if(usuario->PostagensComentadas.Id[i] == post_id)
            return i;
    }
    return -1;
}

void Mostrar_Usuario( S_Usuario* usuario)
{
    printf(">>> Usuario #%06u <<<\n", usuario->Id);
    printf("> Nome: %s \n", usuario->Nome);
    printf("> Email: %s \n", usuario->Email);
    printf("> Bio: %s \n", usuario->Bio);
    printf("> Posts Curtidos: %u \n", usuario->PostagensCurtidas.Quantidade);
    printf("> Comentarios Feitos: %u \n", usuario->PostagensComentadas.Quantidade);
}


void Mostrar_Usuario_EmTabela( S_Usuario* usuario)
{
    printf("| %06u | %-16.16s | %-37.37s | %-38.38s |\n",
        usuario->Id, usuario->Nome, usuario->Email, usuario->Bio);
}


void Mostrar_Usuarios( S_ArrayUsuarios* array)
{
    if(array->Quantidade == 0)
    {
        printf(">>> Nenhum Usuario Encontrado! <<<\n");
        return;
    }
    
    printf("#--------#------------------#---------------------------------------#----------------------------------------#\n");
    printf("|   ID   |       Nome       |                 Email                 |                 Status                 |\n");
    printf("#========#==================#=======================================#========================================#\n");
    for(int i = 0; i < array->Quantidade; i++)
        Mostrar_Usuario_EmTabela(&(array->Usuarios[i]));
    printf("#--------#------------------#---------------------------------------#----------------------------------------#\n");
}

void SalvarArquivo_Usuarios( S_ArrayUsuarios* array, char* arquivo)
{
    FILE* Arquivo = fopen(arquivo, "wb");
    if(Arquivo == NULL)
    {
        printf("ERRO: Abertura de arquivo");
        return;
    }
    
    fwrite(&(array->Quantidade), sizeof(unsigned int), 1, Arquivo);
    fwrite(array->Usuarios, sizeof(S_Usuario), array->Quantidade, Arquivo);
    for(int i = 0; i < array->Quantidade; i++)
    {
        fwrite(array->Usuarios[i].PostagensCurtidas.Id,
                sizeof(unsigned int),
                array->Usuarios[i].PostagensCurtidas.Quantidade,
                Arquivo);
        fwrite(array->Usuarios[i].PostagensComentadas.Id,
                sizeof(unsigned int),
                array->Usuarios[i].PostagensComentadas.Quantidade,
                Arquivo);
    }
    
    fclose(Arquivo);
    return;
}


//Pega os dados do arquivo de usuarios
void CarregarArquivo_Usuarios(S_ArrayUsuarios* vetor_Usuarios, char* arquivo)
{
    FILE* arquivo_usuarios = fopen(arquivo, "rb"); //abre o arquivo na forma de leitura

    if(arquivo_usuarios == NULL)//Sen não conseguir abrir o arquivo, imprime msg de erro e finaliza a função
    {
        printf("O arquivo de usuarios nao foi encontrado!!!!\n");
        return;
    }
    
    fread(&(vetor_Usuarios->Quantidade), sizeof(unsigned int), 1, arquivo_usuarios); //le quantos usuarios existem
    
    vetor_Usuarios->Usuarios = (S_Usuario*) malloc(vetor_Usuarios->Quantidade * sizeof(S_Usuario)); //aloca dinamicamente o numero de usuarios
    
    fread(vetor_Usuarios->Usuarios, sizeof(S_Usuario), vetor_Usuarios->Quantidade, arquivo_usuarios); //Le os dados dos usuarios
    
    for(int i = 0; i < vetor_Usuarios->Quantidade; i++) //for que vai percorrer o vetor de usuarios
    {
        S_Usuario* Usuario = &vetor_Usuarios->Usuarios[i]; //variavel de usuarios que vai receber o endereço de 'array->Usuarios[i]' para n precisar ficar digitando tudo isso a todo momento

        Usuario->PostagensCurtidas.Id = (unsigned int *) malloc(Usuario->PostagensCurtidas.Quantidade * sizeof(unsigned int)); //aloca dinamicamente o vetor de 'Id' de curtidas

        fread(Usuario->PostagensCurtidas.Id, sizeof(unsigned int), Usuario->PostagensCurtidas.Quantidade, arquivo_usuarios); //Le quais postagens foram curtidas

        if(Usuario->PostagensCurtidas.Quantidade == 0){ //Se nenhuma postagem foi curtida por aquele usuario muda o valor da alocacao para 'NULL'
            Usuario->PostagensCurtidas.Id = NULL;
        }
        
        Usuario->PostagensComentadas.Id = (unsigned int *) malloc(Usuario->PostagensComentadas.Quantidade * sizeof(unsigned int)); //aloca dinamicante o vetor de 'Id' de comentarios

        fread(Usuario->PostagensComentadas.Id, sizeof(unsigned int), Usuario->PostagensComentadas.Quantidade, arquivo_usuarios); //le quais postagens foram comentadas

        if(Usuario->PostagensComentadas.Quantidade == 0){ ////Se nenhuma postagem foi comentada por aquele usuario muda o valor da alocacao para 'NULL'
            Usuario->PostagensComentadas.Id = NULL;
        }
    }
    
    fclose(arquivo_usuarios); //fecha o arquivo
    return;
}

S_ArrayUsuarios Buscar_Usuarios_PorEmail( S_ArrayUsuarios* array, char* email)
{
    S_ArrayUsuarios Usuarios_Encontrados;
    Usuarios_Encontrados.Quantidade = 0;
    Usuarios_Encontrados.Usuarios = NULL;
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(AcharParte_NaFrase(array->Usuarios[i].Email, email))
            Adicionar_Usuario_NoArray(&Usuarios_Encontrados, &array->Usuarios[i]);
    }
    return Usuarios_Encontrados;
}


S_ArrayUsuarios Buscar_Usuarios_PorNome( S_ArrayUsuarios* array, char* nome)
{
    S_ArrayUsuarios Usuarios_Encontrados;

    Usuarios_Encontrados.Quantidade = 0;
    Usuarios_Encontrados.Usuarios = NULL;

    for(int i = 0; i < array->Quantidade; i++)
    {
        if(AcharParte_NaFrase(array->Usuarios[i].Nome, nome))
            Adicionar_Usuario_NoArray(&Usuarios_Encontrados, &array->Usuarios[i]);
    }
    return Usuarios_Encontrados;
}

bool Encontra_Usuario_NaLista( S_ArrayUsuarios* array, S_Usuario* usuario)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(array->Usuarios[i].Id == usuario->Id)
            return true;
    }
    return false;
}


void Ordenar_Usuarios(S_ArrayUsuarios* array, E_OrdenarPor tipo_ordenacao)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        S_Usuario Buffer;
        bool Resultado_Cheque;

        for(int j = i; j < array->Quantidade; j++)
        {
            switch (tipo_ordenacao)
            {
            case Ordena_Nome:
                Resultado_Cheque = Compara_Frases(array->Usuarios[j].Nome, array->Usuarios[i].Nome);
                break;
            case Ordena_Email:
                Resultado_Cheque = Compara_Frases(array->Usuarios[j].Email, array->Usuarios[i].Email);
                break;
            case Ordena_Id:
                Resultado_Cheque = array->Usuarios[j].Id < array->Usuarios[i].Id;
            }

            if(Resultado_Cheque == true)
            {
                Buffer = array->Usuarios[i];
                array->Usuarios[i] = array->Usuarios[j];
                array->Usuarios[j] = Buffer;
            }
        } 
    }
    return;
}

//POSTAGEM

unsigned int GerarId_Postagem( S_ArrayPostagens* array)
{
    unsigned int ultimo_id = 0;
    for(int i = 0; i < array->Quantidade - 1; i++)
    {
        if(array->Postagens[i].Id > ultimo_id)
            ultimo_id = array->Postagens[i].Id;
    }
    return ultimo_id + 1;
}


void Alocar_Postagem(S_ArrayPostagens* array)
{
    (array->Quantidade)++;
    if(array->Quantidade == 1)
        array->Postagens = (S_Postagem *) malloc(sizeof(S_Postagem));
    else
        array->Postagens = (S_Postagem *) realloc(array->Postagens, array->Quantidade * sizeof(S_Postagem));

    if(array->Postagens == NULL)
        printf("ERRO: Alocagem de postagem");    
    return;
}

void Liberar_Comentarios(S_ArrayComentarios* array)
{
    if(array->Comentarios != NULL)
        free(array->Comentarios);
    array->Comentarios = NULL;
    array->Quantidade = 0;
    return;
}

void insta_liberaImagem(asciiImg_t *img)
{
    free(img->bytes);
    free(img);
}

void Liberar_Imagens(S_ArrayImagens* array)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        insta_liberaImagem(array->Imagens[i]);
    }

    if(array->Imagens != NULL)
        free(array->Imagens);
    array->Imagens = NULL;
    array->Quantidade = 0;
    return;
}

void Liberar_Postagens_Profundo(S_ArrayPostagens* array)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        Liberar_ArrayId(&array->Postagens[i].QuemCurtiu);
        Liberar_Comentarios(&array->Postagens[i].Comentarios);
        Liberar_Imagens(&array->Postagens[i].Imagens);
    }

    if(array->Postagens == NULL);
        free(array->Postagens);
    array->Postagens = NULL;
    array->Quantidade = 0;
    return;
}

void Desalocar_Postagem(S_ArrayPostagens* array)
{
    if(array->Quantidade == 0)
        return;

    (array->Quantidade)--;
    if(array->Quantidade == 0)
        Liberar_Postagens_Profundo(array);
    else
    {
        array->Postagens = (S_Postagem *) realloc(array->Postagens, array->Quantidade * sizeof(S_Postagem));
        if(array->Postagens == NULL)
            printf("ERRO: De-alocagem de comentario");    
    }
    return;
}


void Liberar_Postagens(S_ArrayPostagens* array)
{
    if(array->Postagens == NULL)
        free(array->Postagens);
    array->Postagens = NULL;
    array->Quantidade = 0;
    return;
}



void Adicionar_Postagem_NoArray(S_ArrayPostagens* array,  S_Postagem* postagem)
{
    Alocar_Postagem(array);
    array->Postagens[array->Quantidade-1] = *postagem;
    return;
}

S_Postagem* Achar_Postagem_PorId( S_ArrayPostagens* array, unsigned int id_postagem)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(array->Postagens[i].Id == id_postagem)
            return &array->Postagens[i];
    }
    return NULL;
}


void Postar(S_ArrayPostagens* array, unsigned int id_autor)
{
    Alocar_Postagem(array);
    S_Postagem* NovaPostagem = &array->Postagens[array->Quantidade - 1];

    NovaPostagem->Id = GerarId_Postagem(array);
    NovaPostagem->Autor_Id = id_autor;
    NovaPostagem->Imagens.Quantidade = 0;
    NovaPostagem->Imagens.Imagens = NULL;
    NovaPostagem->QuemCurtiu = (Array_Id_t) {0, NULL};
    NovaPostagem->Comentarios.Quantidade = 0;
    NovaPostagem->Comentarios.Comentarios = NULL;

    printf(">> Escreva uma descricao para seu postagem: ");
    fgets(NovaPostagem->Descrit, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(NovaPostagem->Descrit);
}

void insta_imprimeImagem(asciiImg_t *img)
{
    printf("%s", img->bytes);
}

void VerPostagem( S_Postagem* postagem, S_ArrayUsuarios* array)
{
    S_Usuario* Autor = Achar_Usuario_PorId(array, postagem->Autor_Id);
    printf(">>> POSTAGEM %06u <<<\n", postagem->Id);
    printf("> Autor: %s [%06u]\n", Autor->Nome, Autor->Id);
    printf("> Imagens: %d\n", postagem->Imagens.Quantidade);
    printf("> Curtidas: %d \n\n", postagem->QuemCurtiu.Quantidade);
    

    for(int i = 0; i < postagem->Imagens.Quantidade; i++)
    {
        printf(">>> INICIO DA IMAGEM %u <<< \n", i);
        insta_imprimeImagem(postagem->Imagens.Imagens[i]);
        printf(">>> FIM DA IMAGEM %u <<< \n", i);
    }
    printf(">> Descricao do Autor <<\n");
    printf("> %s < \n\n", postagem->Descrit);
}


S_ArrayPostagens Achar_Postagens_DoUsuario(S_ArrayPostagens* array, unsigned int id_usuario)
{
    S_ArrayPostagens Saida = {0, NULL};
    for(int i = 0; i < array->Quantidade; i++)
    {
        if(array->Postagens[i].Autor_Id == id_usuario)
            Adicionar_Postagem_NoArray(&Saida, &array->Postagens[i]);
    }
    return Saida;
}

unsigned int GerarId_Comentario( S_ArrayComentarios* array)
{
    unsigned int ultimo_id = 0;
    for(int i = 0; i < array->Quantidade - 1; i++)
    {
        if(array->Comentarios[i].Id > ultimo_id)
            ultimo_id = array->Comentarios[i].Id;
    }
    return ultimo_id + 1;
}

void Criar_Comentario(S_ArrayComentarios* array, unsigned int id_postagem, unsigned int id_autor)
{
    Alocar_Comentario(array);
    S_Comentario* NovoComentario = &array->Comentarios[array->Quantidade - 1];
    NovoComentario->Id = GerarId_Comentario(array);
    NovoComentario->Perfil_Id = id_autor;
    NovoComentario->Postagem_Id = id_postagem;

    printf(">> Escreva o seu comentario (maximo de 256 caracteres): \n");
    fgets(NovoComentario->Mensagem, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(NovoComentario->Mensagem);

    printf(">> Comentario Criado<<\n");
    return;
}

void Comentar(S_ArrayUsuarios* array, S_Postagem* postagem, unsigned int id_autor)
{
    Criar_Comentario(&postagem->Comentarios, postagem->Id, id_autor);
    S_Usuario* Usuario = Achar_Usuario_PorId(array, id_autor);
    Alocar_ArrayId_ComValor(&Usuario->PostagensComentadas, postagem->Id);
}

S_Comentario* Achar_Comentario(S_ArrayComentarios* array, int id_comentario)
{
    for (int i = 0; i < array->Quantidade; i++)
    {
        if(array->Comentarios[i].Id == id_comentario)
            return &array->Comentarios[i];
    }
    return NULL;
}

bool Verifica_Autoria( S_Comentario* comentario, unsigned int id_usuario)
{
    return comentario->Perfil_Id == id_usuario;
}

void Editar_Comentario(S_ArrayComentarios* array, S_Comentario* comentario)
{
    printf(">> Edite o seu comentario (maximo de 256 caracteres): \n");
    fgets(comentario->Mensagem, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(comentario->Mensagem);
    printf(">>> Comentario Editado <<<\n");
    return;
}

void Recomentar(S_Postagem* postagem, unsigned int id_autor)
{
    int id_comentario = -1;

    printf(">> ID do comentario a ser apagado: ");
    scanf("%d", &id_comentario);
    getchar();

    S_Comentario* Comentario = Achar_Comentario(&postagem->Comentarios, id_comentario);
    if(Comentario == NULL)
    {
        printf(">>>> Este Comentario nao existe! <<<<\n");
        return;
    }
    if(Verifica_Autoria(Comentario, id_autor) == false)
    {
        printf(">>>> Este Comentario pertence a outro usuario! <<<<\n");
        return;
    }

    Editar_Comentario(&postagem->Comentarios, Comentario);
    printf(">>> Postagem apagada! <<<\n");
}

int Achar_Posicao_DoComentario(S_ArrayComentarios* array, unsigned int id_comentario)
{
    for (int i = 0; i < array->Quantidade; i++)
    {
        if(array->Comentarios[i].Id == id_comentario)
            return i;
    }
    return -1;
}

void Desalocar_Comentario(S_ArrayComentarios* array)
{
    if(array->Quantidade == 0)
        return;

    (array->Quantidade)--;
    if(array->Quantidade == 0)
        Liberar_Comentarios(array);
    else
    {
        array->Comentarios = (S_Comentario *) realloc(array->Comentarios, array->Quantidade * sizeof(S_Comentario));
        if(array->Comentarios == NULL)
            printf("ERRO: De-alocagem de comentario");    
    }
    return;
}

void Apagar_Comentario(S_ArrayComentarios* array, int posicao_comentario)
{
    array->Comentarios[posicao_comentario] = array->Comentarios[array->Quantidade - 1];
    Desalocar_Comentario(array);
}

void Descomentar(S_ArrayUsuarios* array, S_Postagem* postagem, unsigned int id_autor)
{
    int id_comentario = -1, posicao_comentario = -1;

    printf(">> ID do comentario a ser apagado: ");
    scanf("%d", &id_comentario);
    getchar();

    posicao_comentario = Achar_Posicao_DoComentario(&postagem->Comentarios, id_comentario);
    if(posicao_comentario < 0)
    {
        printf(">>>> Este Comentario nao existe! <<<<\n");
        return;
    }
    if(Verifica_Autoria(&postagem->Comentarios.Comentarios[posicao_comentario], id_autor) == false)
    {
        printf(">>>> Este Comentario pertence a outro usuario! <<<<\n");
        return;
    }
    S_Comentario* Comentario = &postagem->Comentarios.Comentarios[posicao_comentario];

    S_Usuario* Usuario = Achar_Usuario_PorId(array, Comentario->Perfil_Id);
    Remove_Id_DoArray(&Usuario->PostagensComentadas, Comentario->Id);

    Apagar_Comentario(&postagem->Comentarios, posicao_comentario);
    printf(">>> Postagem apagada! <<<\n");
}

bool VerificarCurtida_Postagem( S_Postagem* postagem, unsigned int id_usuario)
{
    for(int i = 0; i < postagem->QuemCurtiu.Quantidade; i++)
    {
        if(postagem->QuemCurtiu.Id[i] == id_usuario)
            return true;
    }
    return false;
}

void Curtir_Postagem(S_ArrayUsuarios* array, S_Postagem* postagem, unsigned int id_usuario)
{
    if(VerificarCurtida_Postagem(postagem, id_usuario) == true)
        return;

    Alocar_ArrayId_ComValor(&postagem->QuemCurtiu, id_usuario);
    
    S_Usuario* Usuario = Achar_Usuario_PorId(array, id_usuario);
    Alocar_ArrayId_ComValor(&Usuario->PostagensCurtidas, postagem->Id);

    printf("Postagem Curtida!\n");
}


void Descurtir_Postagem(S_ArrayUsuarios* array, S_Postagem* postagem, unsigned int id_usuario)
{
    if(VerificarCurtida_Postagem(postagem, id_usuario) != true)
        return;

    for(int i = 0; i < postagem->QuemCurtiu.Quantidade; i++)
    {
        if(postagem->QuemCurtiu.Id[i] == id_usuario)
        {
            postagem->QuemCurtiu.Id[i] = postagem->QuemCurtiu.Id[postagem->QuemCurtiu.Quantidade-1];
            Desalocar_ArrayId(&postagem->QuemCurtiu);
            
            S_Usuario* Usuario = Achar_Usuario_PorId(array, id_usuario);
            Remove_Id_DoArray(&Usuario->PostagensCurtidas, postagem->Id);
            
            printf("Postagem Descurtida!\n");
            return;
        }
    } 
}

void Mostrar_Curtidas( S_ArrayUsuarios* array,  S_Postagem* postagem)
{
    S_ArrayUsuarios Encontrados;
    Encontrados.Quantidade = 0;
    Encontrados.Usuarios = NULL;


    S_Usuario* Buffer;
    for(int i = 0; i < postagem->QuemCurtiu.Quantidade; i++)
    {
        Buffer = Achar_Usuario_PorId(array, postagem->QuemCurtiu.Id[i]);
        Adicionar_Usuario_NoArray(&Encontrados, Buffer);
    }

    Mostrar_Usuarios(&Encontrados);
    Liberar_Usuarios(&Encontrados);
}
asciiImg_t *insta_carregaImagem(char url[], bool colorido, int largura)
{
    FILE *arquivo;
    char buffer[BUFFER_TAMANHO];
    int nBytes, nBytesTotal = 0;
    char linhaComando[LINHA_COMANDO];

    asciiImg_t *img;
    // Aloca espa�o para uma imagem
    img = malloc(sizeof(asciiImg_t));
    if (img == NULL)
        return NULL;
    // Inicializa a estrutura
    img->bytes = NULL;
    img->nBytes = 0;
    // Monta a linha de comando
    (void)sprintf(linhaComando, "%s %s %s -W %d -c > %s", FERRAMENTA_IMAGEM, url, (colorido ? "-C" : ""), largura, ARQUIVO_IMAGEM_TMP);
    // Chama o programa para fazer o download da imagem
    (void)system(linhaComando);

    // Tenta abrir o arquivo recem criado
    arquivo = fopen(ARQUIVO_IMAGEM_TMP, "r");
    if (arquivo != NULL)
    {
        // Ignorar as 2 primeiras linhas do arquivo
        for (int i = 0; i < 2; i++)
        {
            fgets(buffer, BUFFER_TAMANHO, arquivo);
        }
        
        while (!feof(arquivo))
        {

            // Limpa a linha
            (void)memset(buffer, 0, sizeof(buffer));

            // Tenta ler uma linha
            if (fgets(buffer, BUFFER_TAMANHO, arquivo) == NULL)
                continue;

            // Descobre o n�mero de bytes da linha
            for (nBytes = 0; buffer[nBytes] != 0; nBytes++)
                ;

            // Aloca o espa�o
            img->bytes = realloc(img->bytes, sizeof(unsigned char) * (nBytesTotal + nBytes));

            // Copia para o espa�o alocado
            (void)memcpy(&(img->bytes[nBytesTotal]), buffer, nBytes);
            nBytesTotal += nBytes;
        }

        // Finaliza a imagem colocando o \0 final e o tamanho
        img->bytes = realloc(img->bytes, sizeof(unsigned char) * (nBytesTotal + 1));
        img->bytes[nBytesTotal++] = '\0';
        img->nBytes = nBytesTotal;

        // Fecha o arquivo
        fclose(arquivo);
    }
    // Verifica se a imagem � v�lida
    if (img->nBytes < LIMIAR_INFERIOR_TAMANHO_IMAGEM)
    {
        // Libera todo o espa�o alocado
        free(img->bytes);
        free(img);

        return NULL;
    }
    return img;
}

void Upload_Imagem(S_ArrayImagens* array)
{
    char Url[TEXTO_TAM];
    int ImagemColorida = true;
    int Resolucao_Horizontal = IMAGEM_NUM_COLUNAS_PADRAO;
    
    Alocar_Imagem(array);
    
    printf("Insira a URL da Imagem: ");
    fgets(Url, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(Url);

    printf("Modo de cor: \n");
    printf("[0] Preto & Branco \n");
    printf("[1] Colorido \n");
    scanf("%d", &ImagemColorida);
    getchar();

    printf("Resolucao horizontal (numero de colunas) da imagem: ");
    scanf("%d", &Resolucao_Horizontal);
    getchar();

    array->Imagens[array->Quantidade-1] = insta_carregaImagem(Url, (bool) ImagemColorida, Resolucao_Horizontal);
    printf(">>> INICIO DA IMAGEM <<< \n");
    insta_imprimeImagem(array->Imagens[array->Quantidade-1]);
    printf(">>> FIM DA IMAGEM <<< \n");
    
    printf(">>> Imagem carregada com sucesso! <<<\n");
}

void Remover_Imagem_DoArray(S_ArrayImagens* array, unsigned int posicao)
{
    if(array->Quantidade == 0 || posicao < 0 || posicao > array->Quantidade - 1)
        return;
    
    insta_liberaImagem(array->Imagens[posicao]);
    for(int i = posicao; i < array->Quantidade - 1; i++)
        array->Imagens[i] = array->Imagens[i + 1];
    
    (array->Quantidade)--;
    if(array->Quantidade == 0)
        Liberar_Imagens(array);
}

void Editar_Post(S_ArrayPostagens* postagens, S_Postagem* postagem)
{
    int Escolha = 0;
    do
    {
        printf("[1] Alterar Descricao\n");
        printf("[2] Adicionar Imagens\n");
        printf("[3] Remover Imagem\n");
        printf("[0] Cancelar\n");
        
        printf(">> ");
        scanf("%d", &Escolha);
        getchar();

        switch (Escolha)
        {
            case 0:
                return;
            case 1:
                printf(">> Escreva uma descricao para seu postagem: ");
                fgets(postagem->Descrit, TEXTO_TAM, stdin);
                Retira_quebra_de_linha(postagem->Descrit);
                printf(">> Descricao alterada com sucesso! <<\n");
                break;
            case 2:
                do
                {
                    Upload_Imagem(&postagem->Imagens);
                    printf(">> Adicionar outra imagem? <1 para sim, 0 para nao>: \n");
                
                    scanf("%d", &Escolha);
                    getchar();
                } while (Escolha != 0); 
                break;
            case 3:
                printf(">> Posicao da imagem: \n");
                scanf("%d", &Escolha);

                Remover_Imagem_DoArray(&postagem->Imagens, Escolha-1);
                break;
        }
    } while (1);
    
}


void Apagar_Post(S_ArrayPostagens* postagens, unsigned int post_id, S_ArrayUsuarios* usuarios)
{
    for(int i = 0; i < usuarios->Quantidade; i++)
    {
        S_Usuario* Usuario_Atual = &usuarios->Usuarios[i];
        int Posicao_Aux = Checar_Usuario_CurtiuPost(Usuario_Atual, post_id);
        if(Posicao_Aux >= 0)
            Remove_Posicao_ArrayId(&Usuario_Atual->PostagensCurtidas, Posicao_Aux);
        do
        {
            Posicao_Aux = Checar_Usuario_Comentou(Usuario_Atual, post_id);
            if(Posicao_Aux < 0) break;
            
            Remove_Posicao_ArrayId(&Usuario_Atual->PostagensComentadas, Posicao_Aux);
        } while (1);
    }

    for(int i = 0; i < postagens->Quantidade; i++)
    {
        if(postagens->Postagens[i].Id == post_id)
        {
            postagens->Postagens[i] = postagens->Postagens[postagens->Quantidade-1];
            Desalocar_Postagem(postagens);
            return;
        }
    }
}

void Salvar_Imagens_NoArquivo( S_ArrayImagens* array, FILE* arquivo)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        fwrite(&array->Imagens[i]->nBytes, sizeof(int), 1, arquivo);
        fwrite(array->Imagens[i]->bytes, sizeof(uint8_t), array->Imagens[i]->nBytes, arquivo);
    }
}

void SalvarArquivo_Postagens( S_ArrayPostagens* array, char* arquivo)
{
    FILE* Arquivo = fopen(arquivo, "wb");
    if(Arquivo == NULL)
    {
        printf("ERRO: Abertura de arquivo");
        return;
    }
    
    fwrite(&(array->Quantidade), sizeof(unsigned int), 1, Arquivo);
    fwrite(array->Postagens, sizeof(S_Postagem), array->Quantidade, Arquivo);
    for(int i = 0; i < array->Quantidade; i++)
    {

        Salvar_Imagens_NoArquivo(&array->Postagens[i].Imagens, Arquivo);

        fwrite(array->Postagens[i].QuemCurtiu.Id,
                sizeof(unsigned int),
                array->Postagens[i].QuemCurtiu.Quantidade,
                Arquivo);
        fwrite(array->Postagens[i].Comentarios.Comentarios,
                sizeof(S_Comentario),
                array->Postagens[i].Comentarios.Quantidade,
                Arquivo);
    }
    
    fclose(Arquivo);
    return;
}

void Carregar_Imagens_DoArquivo(S_ArrayImagens* array, FILE* arquivo)
{
    array->Imagens = (asciiImg_t **) malloc(array->Quantidade * sizeof(asciiImg_t*));
    for(int i = 0; i < array->Quantidade; i++)
    {
        array->Imagens[i] = (asciiImg_t *) malloc(sizeof(asciiImg_t));
        fread(&array->Imagens[i]->nBytes, sizeof(int), 1, arquivo);
        array->Imagens[i]->bytes = (uint8_t *) malloc(array->Imagens[i]->nBytes * sizeof(uint8_t));
        fread(array->Imagens[i]->bytes, sizeof(uint8_t), array->Imagens[i]->nBytes, arquivo);
    }
    if(array->Quantidade == 0)
        array->Imagens = NULL;
}

void CarregarArquivo_Postagens(S_ArrayPostagens* array, char* arquivo)
{
    FILE* Arquivo = fopen(arquivo, "rb");

    if(Arquivo == NULL)
    {
        printf(">>> Arquivo de postagens (posts.bin) nao encontrado! <<\n");
        printf(">>> Um novo arquivo sera criado ao final do programa <<\n");
        return;
    }
    
    fread(&(array->Quantidade), sizeof(unsigned int), 1, Arquivo);
    array->Postagens = (S_Postagem*) malloc(array->Quantidade * sizeof(S_Postagem));
    fread(array->Postagens, sizeof(S_Postagem), array->Quantidade, Arquivo);
    for(int i = 0; i < array->Quantidade; i++)
    {
        Carregar_Imagens_DoArquivo(&array->Postagens[i].Imagens, Arquivo);
        
        Array_Id_t* QuemCurtiu = &array->Postagens[i].QuemCurtiu;
        QuemCurtiu->Id = (unsigned int *) malloc(QuemCurtiu->Quantidade * sizeof(unsigned int));
        fread(array->Postagens[i].QuemCurtiu.Id,
                sizeof(unsigned int),
                array->Postagens[i].QuemCurtiu.Quantidade,
                Arquivo);
        if(QuemCurtiu->Quantidade == 0)
            QuemCurtiu->Id = NULL;
        
        S_ArrayComentarios* Comentarios = &array->Postagens[i].Comentarios;
        Comentarios->Comentarios = (S_Comentario *) malloc(Comentarios->Quantidade * sizeof(S_Comentario));
        fread(array->Postagens[i].Comentarios.Comentarios,
                sizeof(S_Comentario),
                array->Postagens[i].Comentarios.Quantidade,
                Arquivo);
        if(Comentarios->Quantidade == 0)
            Comentarios->Comentarios = NULL;
        
    }
    
    fclose(Arquivo);
    return;
}

//COMENTARIO

void Adicionar_Comentario_NoArray(S_ArrayComentarios* array,  S_Comentario* comentario)
{
    Alocar_Comentario(array);
    array->Comentarios[array->Quantidade-1] = *comentario;
    return;
}

S_ArrayComentarios Achar_Comentarios_DoUsuario( S_ArrayComentarios* array, unsigned int id_usuario)
{
    S_ArrayComentarios Saida = {0, NULL};

    for(int i = 0; i < array->Quantidade; i++)
    {
        S_Comentario* Comentario = &array->Comentarios[i];
        if(Verifica_Autoria(Comentario, id_usuario) == true)
            Adicionar_Comentario_NoArray(&Saida, Comentario);
    }
    
    return Saida;
}

void Imprime_Comentario( S_Comentario* comentario,  S_ArrayUsuarios* usuarios)
{
    S_Usuario* Autor = Achar_Usuario_PorId(usuarios, comentario->Perfil_Id);
    printf(">>> Comentario #%06u <<<\n", comentario->Id);
    printf("Usuario %s #%06u comentou:\n", Autor->Nome, Autor->Id);
    printf("> %s <\n", comentario->Mensagem);
    return;
}

void Imprime_Comentarios( S_ArrayComentarios* array,  S_ArrayUsuarios* usuarios)
{
    if(array->Quantidade < 1)
        printf(">>>> Nenhum Comentario Foi Feito Ainda<<<<\n");
    else
    {
        for(int i = 0; i < array->Quantidade; i++)
            Imprime_Comentario(&array->Comentarios[i], usuarios);
    }
    return;
}

bool Imprime_Comentarios_DoUsuario( S_ArrayComentarios* array,  S_ArrayUsuarios* usuarios, unsigned int id_usuario)
{
    bool TemComentarios = false;
    if(array->Quantidade < 1)
    {
        printf(">>>> Nenhum Comentario Foi Feito Ainda<<<<\n");
        return false;
    }

    for(int i = 0; i < array->Quantidade; i++)
    {
        S_Comentario* Comentario = &array->Comentarios[i];
        if(Verifica_Autoria(Comentario, id_usuario) == true)
        {
            Imprime_Comentario(Comentario, usuarios);
            TemComentarios = true;
        }
    }
    
    if (TemComentarios == false)
    {
        printf(">>>> Nenhum Comentario Foi Feito Por Este Usuario <<<<\n");
        return false;
    }
    return true;
}

//IMAGEM

void Desaloca_Imagem(S_ArrayImagens* array)
{
    if(array->Quantidade == 0)
        return;

    (array->Quantidade)--;
    if(array->Quantidade == 0)
        Liberar_Imagens(array);
    else
    {
        insta_liberaImagem(array->Imagens[array->Quantidade]);
        array->Imagens = (asciiImg_t **) realloc(array->Imagens, array->Quantidade * sizeof(asciiImg_t *));
        if(array->Imagens == NULL)
            printf("ERRO: De-alocagem de comentario");    
    }
    return;
}

//MENUS

/*Funcao que mostra o menu inicial do programa.
* Ela é do tipo 'int' porque caso o usuario peca para finalizar o programa ela retornara o valor 1, quebrando o loop do 'while'
*/

void Mostrar_Comentarios_EmPost(S_Postagem* postagem)
{
    int Escolha = 0;
    unsigned int Id = 0;
    printf("[1] Todos\n");
    printf("[2] Seus\n");
    printf("[3] Autor\n");
    if(Usuario_Ativo->Id != Usuario_Logado->Id)
    {
        printf("[4] Usuario Ativo <%s #%06u>\n",
                Usuario_Ativo->Nome,
                Usuario_Ativo->Id);
    }
    printf("[5] Usuario Especifico\n");
    printf("[0] Cancelar\n");
    
    printf(">> ");
    scanf("%d", &Escolha);
    getchar();

    switch (Escolha)
    {
    case 0:
        break;
    case 1:
        Imprime_Comentarios(&postagem->Comentarios, &Vetor_Usuarios);
        break;
    case 2:
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Logado->Id);
        break;
    case 3:
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, postagem->Autor_Id);
        break;
    case 4:
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Ativo->Id);
        break;
    case 5:
        printf("Id do usuario: ");
        scanf("%u", &Id);
        getchar();
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Id);
        break;
    }
}

void Mostrar_Tela_Postagem(S_Postagem* postagem)
{
    int Escolha = 0;
    do
    {
        printf("[1] Curtir\n");
        printf("[2] Descurtir\n");
        printf("[3] Ver Quem Curtiu\n");
        printf("------------------------\n");
        printf("[4] Comentar\n");
        printf("[5] Ver Comentarios\n");
        if(postagem->Autor_Id == Usuario_Logado->Id)
        {
            printf("[6] Editar Comentario\n");
            printf("[7] Apagar Comentario\n");
            printf("------------------------\n");
            printf("[8] Editar Postagem\n");
            printf("[9] Apagar Postagem\n");
        }
        printf("------------------------\n");
        printf("[0] Concluir\n");
        
        printf(">> ");
        scanf("%d", &Escolha);
        getchar();

        switch (Escolha)
        {
        case 0:
            return;
        case 1:
            Curtir_Postagem(&Vetor_Usuarios, postagem, Usuario_Logado->Id);
            break;
        case 2:
            Descurtir_Postagem(&Vetor_Usuarios, postagem, Usuario_Logado->Id);
            break;
        case 3:
            Mostrar_Curtidas(&Vetor_Usuarios, postagem);
            break;
        case 4:
            Comentar(&Vetor_Usuarios, postagem, Usuario_Logado->Id);
            break;
        case 5:
            Mostrar_Comentarios_EmPost(postagem);
            break;
        case 8:
            if(postagem->Autor_Id == Usuario_Logado->Id)
                Editar_Post(&Vetor_Postagens, postagem);
            break;
        case 7:
            if(postagem->Autor_Id == Usuario_Logado->Id)
            {
                Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Logado->Id);
                Descomentar(&Vetor_Usuarios, postagem, Usuario_Logado->Id);
            }
            break;
        case 9:
            if(postagem->Autor_Id == Usuario_Logado->Id)
            {
                Apagar_Post(&Vetor_Postagens, postagem->Id, &Vetor_Usuarios);
                printf(">>> Postagem apagada! <<<\n");
            }
            return;
        case 6:
            if(postagem->Autor_Id == Usuario_Logado->Id)
            {
                Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Logado->Id);
                Recomentar(postagem, Usuario_Logado->Id);
            }
            break;
        }
    } while (1);
}

void Mostrar_Postagem(S_Postagem* postagem)
{
    if(postagem == NULL)
    {
        printf(">> Postagem Nao Encontrado <<\n");
        return;
    }
    VerPostagem(postagem, &Vetor_Usuarios);
    Mostrar_Tela_Postagem(postagem);
    return;
}

void Mostrar_Postagens_EmMenu(S_ArrayPostagens* array)
{
    int Escolha = 0, Indice_atual = 0;
    if(array->Quantidade < 1)
    {
        printf(">>>> Este Perfil Nao Possui Nenhum postagem <<<<\n");
        return;
    }
    do
    {
        S_Postagem* Postagem_Atual = &array->Postagens[Indice_atual];
        S_Usuario* Postagem_Autor = Achar_Usuario_PorId(&Vetor_Usuarios, Postagem_Atual->Autor_Id);
        printf("<><><><><><><><><><><><><><><><><><><><><><><><>\n");
        printf("> #%06u\n", Postagem_Atual->Id);
        printf("> Postada por %s #%06u\n", Postagem_Autor->Nome, Postagem_Autor->Id);
        printf("\"%.32s\"\n", Postagem_Atual->Descrit);
        printf("<><><><><><><><><><><><><><><><><><><><><><><><>\n");

        printf("[1] Anterior\n");
        printf("[2] Proxima\n");
        printf("[3] Ver Mais\n");
        printf("[0] Cancelar\n");

        printf(">> ");
        scanf("%d", &Escolha);
        getchar();

        switch (Escolha)
        {
        case 1:
            Indice_atual = Indice_atual == 0 ? array->Quantidade - 1 : Indice_atual - 1;
            break;
        case 2:
            Indice_atual = Indice_atual == array->Quantidade - 1 ? 0 : Indice_atual + 1;
            break;
        case 3:
            Mostrar_Postagem(Postagem_Atual);
            break;
        case 0:
            return;
        }
        
    } while (1);
}

void Mostrar_Postagens_Curtidas()
{
    S_ArrayPostagens Curtidas = {0, NULL};
    for(int i = 0; i < Usuario_Ativo->PostagensCurtidas.Quantidade; i++)
        Adicionar_Postagem_NoArray(&Curtidas, Achar_Postagem_PorId(&Vetor_Postagens, Usuario_Ativo->PostagensCurtidas.Id[i]));

    Mostrar_Postagens_EmMenu(&Curtidas);
    Liberar_Postagens(&Curtidas);
    return;
}

void Mostrar_Tela_UsuarioAtivo()
{
    S_ArrayPostagens Posts_DoUsuario = {0, NULL};
    do
    {
        int Escolha = 0;
        Mostrar_Usuario(Usuario_Ativo);
        printf("[1] Ver Postagens Deste Perfil \n");
        printf("[2] Ver Postagens Curtidas Por Este Perfil\n");
        printf("[3] Estatisticas Do Perfil\n");
        printf("[0] Voltar\n");
        
        printf(">> ");
        scanf("%d", &Escolha);
        getchar();
        
        switch (Escolha)
        {
        case 0:
            Usuario_Ativo = Usuario_Logado;
            return;
        case 1:
            Posts_DoUsuario = Achar_Postagens_DoUsuario(&Vetor_Postagens, Usuario_Ativo->Id);
            Mostrar_Postagens_EmMenu(&Posts_DoUsuario);
            Liberar_Postagens(&Posts_DoUsuario);
            break;
        case 2:
            Mostrar_Postagens_Curtidas();
            return;
        case 3:
            Mostrar_Usuario(Usuario_Ativo);
            break;
        }
    } while (1);
}

void Mostrar_Usuarios_EmMenu(S_ArrayUsuarios* array)
{
    int Escolha = 0, Indice_atual = 0;
    do
    {
        S_Usuario* Usuario_atual = &array->Usuarios[Indice_atual];
        printf("> Id #%06u \n", Usuario_atual->Id);
        printf("> Nome: %s \n", Usuario_atual->Nome);
        printf("> Email: %s \n", Usuario_atual->Email);
        printf("\"%s\"\n", Usuario_atual->Bio);

        printf("[1] Anterior\n");
        printf("[2] Proximo\n");
        printf("[3] Ver Mais\n");
        printf("[0] Cancelar\n");

        printf(">> ");
        scanf("%d", &Escolha);
        getchar();

        switch (Escolha)
        {
        case 1:
            Indice_atual = Indice_atual == 0 ? array->Quantidade - 1 : Indice_atual - 1;
            break;
        case 2:
            Indice_atual = Indice_atual == array->Quantidade - 1 ? 0 : Indice_atual + 1;
            break;
        case 3:
            if(Usuario_atual->Id == Usuario_Logado->Id)
                return;
            Usuario_Ativo = Usuario_atual;
            Mostrar_Tela_UsuarioAtivo();
            break;
        case 0:
            return;
        }
        
    } while (1);
}



void Mostrar_Configs_Usuario()
{
    int Escolha = 0;
    do
    {
        Mostrar_Usuario(Usuario_Logado);
        printf("[1] Alterar Nome\n");
        printf("[2] Alterar Email\n");
        printf("[3] Alterar Senha\n");
        printf("[4] Alterar Bio\n");
        printf("[0] Concluir\n");
        
        printf(">> ");
        scanf("%d", &Escolha);
        getchar();

        switch (Escolha)
        {
        case 0:
            return;
        case 1:
            Alterar_Nome_Usuario(Usuario_Logado);
            break;
        case 2:
            Alterar_Email_Usuario(Usuario_Logado);
            break;
        case 3:
            Alterar_Senha_Usuario(Usuario_Logado);
            break;
        case 4:
            Alterar_Bio_Usuario(Usuario_Logado);
            break;
        }
    } while (1);
}


void Mostrar_Tela_Principal()
{
    S_ArrayPostagens Posts_DoUsuario;
    
    Posts_DoUsuario.Quantidade = 0;
    Posts_DoUsuario.Postagens = NULL;

    S_ArrayUsuarios Busca_Usuarios; 
    Busca_Usuarios.Quantidade = 0;
    Busca_Usuarios.Usuarios = NULL;


    char Busca_Buffer[NOME_TAM];
    do
    {
        int Escolha = 0;
        Mostrar_Usuario(Usuario_Logado);
        printf("[1] Nova Postagem\n");
        printf("[2] Ver Postagem \n");
        printf("[3] Ver Postagens Deste Perfil \n");
        printf("[4] Ver Todas As Postagens\n");
        printf("[5] Ver Postagens Curtidas\n");
        printf("-------------------------------\n");
        printf("[6] Buscar Perfil\n");
        printf("[7] Ver Todos Os Perfis\n");
        printf("[8] Estatisticas Deste Perfil\n");
        printf("[9] Alterar Perfil\n");
        printf("-------------------------------\n");
        printf("[0] Deslogar\n");
        
        printf(">> ");
        scanf("%d", &Escolha);
        getchar();
        
        switch (Escolha)
        {
        case 0:
            Deslogar_Usuario(&Usuario_Ativo);
            Deslogar_Usuario(&Usuario_Logado);
            return;
        case 1:
            Postar(&Vetor_Postagens, Usuario_Ativo->Id);
            do
            {
                Upload_Imagem(&Vetor_Postagens.Postagens[Vetor_Postagens.Quantidade-1].Imagens);
                printf(">> Adicionar outra imagem? <1 para sim, 0 para nao>: \n");
            
                scanf("%d", &Escolha);
                getchar();
            } while (Escolha != 0); 
            break;
        case 2:
            printf(">> ID da postagem: ");
            
            scanf("%d", &Escolha);
            getchar();
            
            Mostrar_Postagem(Achar_Postagem_PorId(&Vetor_Postagens, Escolha));
            break;
        case 3:
            Posts_DoUsuario = Achar_Postagens_DoUsuario(&Vetor_Postagens, Usuario_Logado->Id);
            Mostrar_Postagens_EmMenu(&Posts_DoUsuario);
            Liberar_Postagens(&Posts_DoUsuario);
            break;
        case 4:
            Mostrar_Postagens_EmMenu(&Vetor_Postagens);
            break;
        case 5:
            Mostrar_Postagens_Curtidas();
            break;
        case 6:
            printf(">> Buscar por? <1 para nome, 2 para email, 0 cancelar>: ");
            
            scanf("%d", &Escolha);
            getchar();

            if(Escolha == 0) break;
            printf(">> Escreva parte do %s: ", Escolha == 1 ? "nome" : "email");
            fgets(Busca_Buffer, NOME_TAM, stdin);
            Retira_quebra_de_linha(Busca_Buffer);

            Busca_Usuarios = Escolha == 1 ? Buscar_Usuarios_PorNome(&Vetor_Usuarios, Busca_Buffer) : Buscar_Usuarios_PorEmail(&Vetor_Usuarios, Busca_Buffer);


            if(Busca_Usuarios.Quantidade == 0)
            {
                printf(">>>> Usuario nao encontrado! <<<<\n");
            }
            else Mostrar_Usuarios_EmMenu(&Busca_Usuarios);
            Liberar_Usuarios(&Busca_Usuarios);
            break;
        case 7:
            printf("[1] Por Nome\n");
            printf("[2] Por Email\n");
            printf("[0] Por Id\n");
            
            scanf("%d", &Escolha);
            getchar();
            
            Ordenar_Usuarios(&Vetor_Usuarios, (E_OrdenarPor) Escolha);
            Mostrar_Usuarios_EmMenu(&Vetor_Usuarios);
            break;
        case 8:
            Mostrar_Usuario(Usuario_Logado);
            break;
        case 9:
            Mostrar_Configs_Usuario();
            break;
        }
    } while (1);
}

int Mostrar_Tela_Inicio()
{
    int Escolha = 0;
    
    do
    {
        printf("[1] Logar\n");
        printf("[2] Cadastrar\n");
        printf("[3] Listar Usuarios\n");
        printf("[0] Sair\n");
        
        printf(">> ");
        scanf("%d", &Escolha);
        getchar();
        
        switch (Escolha)
        {
        case 0:
            return 1; //retorna 1 para caso o usuario deseje sair do programa
        case 1:

            //chama a funcao que loga o usuario

            Usuario_Ativo = Usuario_Logado = Logar_Usuario(&Vetor_Usuarios);
            if(Usuario_Logado != NULL)
                Mostrar_Tela_Principal();
            break;
        case 2:
            Cadastrar_NovoUsuario(&Vetor_Usuarios);
            break;
        case 3:
            printf("[1] Por Nome\n");
            printf("[2] Por Email\n");
            printf("[0] Por Id\n");
            
            scanf("%d", &Escolha);
            getchar();
            
            Ordenar_Usuarios(&Vetor_Usuarios, (E_OrdenarPor) Escolha);
            Mostrar_Usuarios(&Vetor_Usuarios);
            break;
        }
    } while(1);
}

int main(int argc, char** argv)
{

    Vetor_Usuarios.Quantidade = 0;
    Vetor_Usuarios.Usuarios = NULL;

    Vetor_Postagens.Quantidade = 0;
    Vetor_Postagens.Postagens = NULL;

    CarregarArquivo_Usuarios(&Vetor_Usuarios, ARQUIVO_USUARIOS);
    CarregarArquivo_Postagens(&Vetor_Postagens, ARQUIVO_POSTS);


    while (1)
    {
        if(Mostrar_Tela_Inicio() == 1)
        {
            Deslogar_Usuario(&Usuario_Logado);
            SalvarArquivo_Postagens(&Vetor_Postagens, ARQUIVO_POSTS);
            SalvarArquivo_Usuarios(&Vetor_Usuarios, ARQUIVO_USUARIOS);
            Liberar_Usuarios_Profundo(&Vetor_Usuarios);
            Liberar_Postagens_Profundo(&Vetor_Postagens);
            break;
        }
    }
    
    return 0;
}