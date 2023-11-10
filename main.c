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
    S_Comentario *dados_Comentario;
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
    S_ArrayImagens imagens_do_post;
    
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

void Liberar_Vetor_Id(Array_Id_t* vetor)
{
    if(vetor->Id != NULL) { //se o vetor de Id for diferente de nulo, libera ele da memoria
        free(vetor->Id);
    }
    vetor->Id = NULL; //atribui o valor de nulo ao vetor de Id
    vetor->Quantidade = 0; //define a quantidade/tamanho como 0
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
        if(frase1[i] < frase2[i]){
            return true;
        }
        else if(frase1[i] > frase2[i]){
            return false;
        }
    }
    
    if(Tamanho1 < Tamanho2)
        return true;
    else return false;
}

void Alocar_Dinamicamente_Comentario(S_ArrayComentarios* vetor_comentarios)
{
    (vetor_comentarios->Quantidade)++; //aumenta a quantidade/tamnho do vetor

    if(vetor_comentarios->Quantidade == 1){ //se quantidade for 1, incializa o vetor com 1 de tamnho
        vetor_comentarios->dados_Comentario = (S_Comentario *) malloc(sizeof(S_Comentario));
    }
    else{ //se for maior que 1, aumenta o tamnho do vetor com base em quantidade
        vetor_comentarios->dados_Comentario = (S_Comentario *) realloc(vetor_comentarios->dados_Comentario, vetor_comentarios->Quantidade * sizeof(S_Comentario));
    }

    //se o vetor for nulo, houve um erro na alocacao
    if(vetor_comentarios->dados_Comentario == NULL){
        printf("Erro na alocagem de comentario!!");    
    }
    return;
}

void Aloca_Imagem_Dinamicamente(S_ArrayImagens* vetor_imagem)
{
    (vetor_imagem->Quantidade)++; //aumenta a quantidade de imagens no vetor

    if(vetor_imagem->Quantidade == 1){ //se nunca foi inicializada o vetor de imagens da postagem alocamos 1 de tamanho
        vetor_imagem->Imagens = (asciiImg_t **) malloc(sizeof(asciiImg_t* ));
    }
    else{ //se ja foi inicializado, realocamos o tamanho para um maior
        vetor_imagem->Imagens = (asciiImg_t **) realloc(vetor_imagem->Imagens, vetor_imagem->Quantidade * sizeof(asciiImg_t *));
    }

    //se 'Imagens' for nulo, houve um erro na alocacao
    if(vetor_imagem->Imagens == NULL){
        printf("erro na alocagem de imagens!!!");    
    }
    return;
}

void Alocar_ArrayId(Array_Id_t* vetor)
{
    (vetor->Quantidade)++; //aumenta a quantidade do vetor

    if(vetor->Quantidade == 1){ //se nunca foi inicializado, inicializa ele com 1 de tamanho
        vetor->Id = (unsigned int *) malloc(sizeof(unsigned int));
    }
    else{ // se ja foi inicializado, aumenta o tamnho do vetor
        vetor->Id = (unsigned int *) realloc(vetor->Id, vetor->Quantidade * sizeof(unsigned int));
    }

    //se o valor do vetor for nulo, houve um erro na alocacao
    if(vetor->Id == NULL){
        printf("Erro na alocagem de vetor!!");    
    }
    return;
}

void Alocar_Vetor_Id_Com_Valor(Array_Id_t* vetor, unsigned int novo_valor)
{
    Alocar_ArrayId(vetor); //aloca o vetor recebido
    vetor->Id[vetor->Quantidade-1] = novo_valor; //atribui um valor ao Id
    return;
}

void Desalocar_Vetor_Id(Array_Id_t* vetor_Id)
{
    if(vetor_Id->Quantidade == 0){ //se o tamanho do vetor for 0, sai da funcao
        return;
    }
    (vetor_Id->Quantidade)--; //diminui a quantidade/tamnho do vetor

    if(vetor_Id->Quantidade == 0) { //se a quantidade for 0, depois de subtraida, libera o vetor
        Liberar_Vetor_Id(vetor_Id);
    }
    else //se a quantidade for maior que 0 realloca o tamanho do vetor para a nova quantidade, ou seja, um tamanho menor
    {
        vetor_Id->Id = (unsigned int *) realloc(vetor_Id->Id, vetor_Id->Quantidade * sizeof(unsigned int));

        //se o vetor for nulo, houve um erro na alocacao
        if(vetor_Id->Id == NULL){
            printf("Erro na alocagem");    
        }
    }
    return;
}

void Remove_Posicao_Vetor_Id(Array_Id_t* vetor_Id, int posicao)
{
    vetor_Id->Id[posicao] = vetor_Id->Id[vetor_Id->Quantidade-1]; //atribui o valor da posicao anterior na posicao atual
    Desalocar_Vetor_Id(vetor_Id); //retira da memoria o valor
}

void Remove_Id_Do_Vetor(Array_Id_t* vetor_Id, unsigned int id_informado)
{
    for(int i = 0; i < vetor_Id->Quantidade; i++) //percorre o vetor
    {
        if(vetor_Id->Id[i] == id_informado){ //se o id do vetor for igual ao informado remove ele do vetor
            Remove_Posicao_Vetor_Id(vetor_Id, i);
        }
    }
    return;
}

//CADASTRO

//Funcao que alocadinamicamente um usuario
void Alocar_Usuario_Dinamicamente(S_ArrayUsuarios* usuario)
{
    (usuario->Quantidade)++; //aumenta quantos usuarios tem.

    if(usuario->Quantidade == 1){// se não foi inicializado, aloca dinamicamente com malloc 
        usuario->Usuarios = (S_Usuario *) malloc(sizeof(S_Usuario));
    } 
    else{
        usuario->Usuarios = (S_Usuario *) realloc(usuario->Usuarios, usuario->Quantidade * sizeof(S_Usuario));
    }//caso tenha um (1) usuario realoca o tamanho do vetor de usuario pra um tamanho maior

    //Se a alocacao falhar imprime uma mensagem de erro
    if(usuario->Usuarios == NULL){
        printf("Erro na alocagem de usuario");    
    }
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

void Free_Usuarios(S_ArrayUsuarios* vetor_usuarios)
{
    if(vetor_usuarios->Usuarios != NULL){ //se o vetor de usuario for diferente de nulo, libera ele da memoria
        free(vetor_usuarios->Usuarios);
    }
    vetor_usuarios->Usuarios = NULL; //define os usuarios do vetor como nulo
    vetor_usuarios->Quantidade = 0; //define 0 para a quantidade de usuarios
    return;
}

void Liberar_Usuarios_Profundo(S_ArrayUsuarios* array)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        Liberar_Vetor_Id(&array->Usuarios[i].PostagensComentadas);
        Liberar_Vetor_Id(&array->Usuarios[i].PostagensCurtidas);
    }
    if(array->Usuarios != NULL)
        free(array->Usuarios);
    array->Usuarios = NULL;
    array->Quantidade = 0;
    return;
}


void Adicionar_Usuario_No_Vetor(S_ArrayUsuarios* vetor_usuarios, S_Usuario* novo_usuario)
{
    Alocar_Usuario_Dinamicamente(vetor_usuarios); //aloca um novo tamanho no vetor de usuarios
    vetor_usuarios->Usuarios[vetor_usuarios->Quantidade-1] = *novo_usuario; //grava a informacao recebida do novo usuario na ultima posicao do vetor
}

S_Usuario* Achar_Usuario_PorEmail(S_ArrayUsuarios* array, char* email)
{
    for(int i = 0; i < array->Quantidade; i++) //percorre o vetor de usuarios
    {
        if(strcmp(array->Usuarios[i].Email, email) == 0) //se o email de algum dos usuarios for igual ao informado retorna o usuario
            return &array->Usuarios[i];
    }
    return NULL; //se nao achar nenhum usuario com id igual ao informado retorna o valor nulo
}


S_Usuario* Achar_Usuario_Pelo_Id( S_ArrayUsuarios* array, unsigned int id)
{
    for(int i = 0; i < array->Quantidade; i++) //percorre o vetor de usuarios
    {
        if(array->Usuarios[i].Id == id) //se o id de algum dos usuarios for igual ao informado retorna o usuario
            return &array->Usuarios[i];
    }
    return NULL; //se nao achar nenhum usuario com id igual ao informado retorna o valor nulo
}


//Funcao que loga o usuario
S_Usuario* Logar_Usuario( S_ArrayUsuarios* array)
{
    int escolha = 0;
    char email[NOME_TAM];
    char senha[SENHA_TAM];
    S_Usuario* usuario_encontrado;

    do
    {
        usuario_encontrado = NULL; //atribui o valor nulo a struct 'usuario_encontrado'
        
        printf("1 - Logar com o ID\n");
        printf("2 - Logar com o Email\n");
        printf("0 - Cancelar\n");

        printf("opcao:  ");
        scanf("%d", &escolha);
        getchar();

        switch (escolha)
        {
        case 0: //se o usuario escolher 0 finaliza a operacao, retornando o valor nulo
            return NULL; 
        
        case 1: //Se o usuario quiser logar pelo id 
            printf("Digite o ID: ");
            scanf("%d", &escolha); //pega o id do usuario
            getchar();

            usuario_encontrado = Achar_Usuario_Pelo_Id(array, (unsigned int) escolha); //atribui o valor de retornado a 'usuario_encontrado'
            break;

        case 2: //se o usuario quiser logar pelo email de cadastro
            printf("Digite o email: "); //pede o email da conta ao usuario, deve ser o email completo
            fgets(email, NOME_TAM, stdin);
            Retira_quebra_de_linha(email); //retira a quebra de linha no final da string para evitar problemas


            usuario_encontrado = Achar_Usuario_PorEmail(array, email); //atribui o valor de retornado a 'usuario_encontrado'
            break;
        default:
            continue;
        }

        printf("Digite a senha: "); //pede a senha da conta ao usuario
        fgets(senha, SENHA_TAM, stdin);
        Retira_quebra_de_linha(senha); 


        if(usuario_encontrado == NULL) //se o usuario for nulo o email ou id informados estavam errados
        {
            printf("Email/ID invalido!\n");
            continue;
        }
        else if(strcmp(usuario_encontrado->Senha, senha) != 0) //compara a senha informada com a do usuario encontrado, se for diferente informa ao usuario
        {
            printf("Senha invalida para esse usuario!!\n");
            continue;
        }
        else //se a senha encpontrada for valida imprime o usuario que foi logado e retorna ele
        {
            printf("Logado como %s #%04u\n", usuario_encontrado->Nome, usuario_encontrado->Id);
            return usuario_encontrado;
        }
    } while (1);
}

void Deslogar_Usuario(S_Usuario** usuario)
{
    *usuario = NULL; //usuario recebido na chamada da funcao se torna nulo
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
    printf("Digite o novo nome: ");
    fgets(usuario->Nome, NOME_TAM, stdin);
    Retira_quebra_de_linha(usuario->Nome);

    return;
}

void Alterar_Email_Usuario(S_Usuario* usuario)
{
    char Senha_Confirma[SENHA_TAM];
    
    printf("Digite a senha atual: ");
    fgets(Senha_Confirma, SENHA_TAM, stdin);
    Retira_quebra_de_linha(Senha_Confirma);

    if(strcmp(usuario->Senha, Senha_Confirma) != 0)
    {
        printf("A senha digitada esta incorreta!\n");
        return;
    }

    printf("Digite o novo email: ");
    fgets(usuario->Email, NOME_TAM, stdin);
    Retira_quebra_de_linha(usuario->Email);

    return;
}

void Alterar_Bio_Usuario(S_Usuario* usuario)
{
    printf("Escreva uma breve descricao sua: ");
    fgets(usuario->Bio, NOME_TAM, stdin);
    Retira_quebra_de_linha(usuario->Bio);

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

void Mostrar_Usuario( S_Usuario* usuario) //Mostra os dados do usuario
{
    printf("Usuario #%06u <<<\n", usuario->Id);
    printf("Nome: %s \n", usuario->Nome);
    printf("Email: %s \n", usuario->Email);
    printf("Bio: %s \n", usuario->Bio);
}

void Imprime_Usuario_Em_Formato_Tabela(S_Usuario* usuario)
{
    printf("| %-50u | %-50s |%-50s | %-50s |\n", usuario->Id, usuario->Nome, usuario->Email, usuario->Bio);
}

void Imprime_Usuarios(S_ArrayUsuarios* vetor_usuarios)
{
    if(vetor_usuarios->Quantidade == 0) //se a quantidade for 0, sai da funcao, pois na ha nenhum usuario no vetor
    {
        printf("Nenhum Usuario Encontrado!!!\n");
        return;
    }
    
    printf("| %-50s | %-50s | %-50s | %-50s |\n", "ID", "Nome", "Email", "Bio"); //titulo

    for(int i = 0; i < vetor_usuarios->Quantidade; i++){ //percorre o vetor de usuarios recebido pela funcao
        Imprime_Usuario_Em_Formato_Tabela(&(vetor_usuarios->Usuarios[i]));
    }
}

void SalvarArquivo_Usuarios( S_ArrayUsuarios* vetor_usuarios, char* arquivo)
{
    FILE* Arquivo = fopen(arquivo, "wb");
    if(Arquivo == NULL)
    {
        printf("ERRO: Abertura de arquivo");
        return;
    }
    
    fwrite(&(vetor_usuarios->Quantidade), sizeof(unsigned int), 1, Arquivo);
    fwrite(vetor_usuarios->Usuarios, sizeof(S_Usuario), vetor_usuarios->Quantidade, Arquivo);
    for(int i = 0; i < vetor_usuarios->Quantidade; i++)
    {
        fwrite(vetor_usuarios->Usuarios[i].PostagensCurtidas.Id, sizeof(unsigned int), vetor_usuarios->Usuarios[i].PostagensCurtidas.Quantidade, Arquivo);

        fwrite(vetor_usuarios->Usuarios[i].PostagensComentadas.Id, sizeof(unsigned int), vetor_usuarios->Usuarios[i].PostagensComentadas.Quantidade, Arquivo);
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
        if(AcharParte_NaFrase(array->Usuarios[i].Email, email)){
            Adicionar_Usuario_No_Vetor(&Usuarios_Encontrados, &array->Usuarios[i]);
        }
    }
    return Usuarios_Encontrados;
}

S_ArrayUsuarios Buscar_Usuarios_PorNome( S_ArrayUsuarios* vetor_usuarios, char* nome)
{
    S_ArrayUsuarios Usuarios_Encontrados;

    Usuarios_Encontrados.Quantidade = 0;
    Usuarios_Encontrados.Usuarios = NULL;

    for(int i = 0; i < vetor_usuarios->Quantidade; i++)
    {
        if(AcharParte_NaFrase(vetor_usuarios->Usuarios[i].Nome, nome)){
            Adicionar_Usuario_No_Vetor(&Usuarios_Encontrados, &vetor_usuarios->Usuarios[i]);
        }
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

void Ordenar_Usuarios(S_ArrayUsuarios* vetor_usuarios, int tipo_ordenacao)
{
    for(int i = 0; i < vetor_usuarios->Quantidade; i++)
    {
        S_Usuario usuario_temporario;

        bool Resultado_Cheque;

        for(int j = i; j < vetor_usuarios->Quantidade; j++)
        {
            switch (tipo_ordenacao)
            {
            case 1:
                Resultado_Cheque = Compara_Frases(vetor_usuarios->Usuarios[j].Nome, vetor_usuarios->Usuarios[i].Nome);
                break;
            case 2:
                Resultado_Cheque = Compara_Frases(vetor_usuarios->Usuarios[j].Email, vetor_usuarios->Usuarios[i].Email);
                break;
            case 0:
                Resultado_Cheque = vetor_usuarios->Usuarios[j].Id < vetor_usuarios->Usuarios[i].Id;
            }

            if(Resultado_Cheque == true)
            {
                usuario_temporario = vetor_usuarios->Usuarios[i];
                vetor_usuarios->Usuarios[i] = vetor_usuarios->Usuarios[j];
                vetor_usuarios->Usuarios[j] = usuario_temporario;
            }
        } 
    }
    return;
}

//POSTAGEM

unsigned int GerarId_Postagem( S_ArrayPostagens* vetor_postagens)
{
    unsigned int ultimo_id = 0;

    for(int i = 0; i < vetor_postagens->Quantidade - 1; i++) //percorre o vetor de postagens
    {
        if(vetor_postagens->Postagens[i].Id > ultimo_id){ //se uma postagem tiver um id com valor maior que a variavel 'ultimo_id', a variavel passa a ter o valor do id da postagem
            ultimo_id = vetor_postagens->Postagens[i].Id;
        }
    }

    return ultimo_id + 1; //retorna o valor do maior id do vetor de postagens +1 
}

void Aloca_Dinamicamente_Postagem(S_ArrayPostagens* vetor_postagem)
{
    (vetor_postagem->Quantidade)++; //soma quantos postagens tem no vetor

    if(vetor_postagem->Quantidade == 1) // se a quantidade for igual a 1, inicializa o vetor com 1 de tamnho
        vetor_postagem->Postagens = (S_Postagem *) malloc(sizeof(S_Postagem));
    else 
        vetor_postagem->Postagens = (S_Postagem *) realloc(vetor_postagem->Postagens, vetor_postagem->Quantidade * sizeof(S_Postagem));
        //se tiver mais de 1 postagem realoca o numero delas para um maior

    if(vetor_postagem->Postagens == NULL){ //se postagens tiver valor nulo, significa que houve erro na alocacao
        printf("erro na alocagem de postagem!!!");    
    }
    return;
}

void Liberar_Comentarios(S_ArrayComentarios* array)
{
    if(array->dados_Comentario != NULL)
        free(array->dados_Comentario);
    array->dados_Comentario = NULL;
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

    if(array->Imagens != NULL){
        free(array->Imagens);
    }
    array->Imagens = NULL;
    array->Quantidade = 0;
    return;
}

void Liberar_Postagens_Profundo(S_ArrayPostagens* array)
{
    for(int i = 0; i < array->Quantidade; i++)
    {
        Liberar_Vetor_Id(&array->Postagens[i].QuemCurtiu);
        Liberar_Comentarios(&array->Postagens[i].Comentarios);
        Liberar_Imagens(&array->Postagens[i].imagens_do_post);
    }

    if(array->Postagens != NULL){
        free(array->Postagens);
    }
    array->Postagens = NULL;
    array->Quantidade = 0;
    return;
}

void Desalocar_Postagem(S_ArrayPostagens* array)
{
    if(array->Quantidade == 0)
        return;

    (array->Quantidade)--;

    if(array->Quantidade == 0){
        Liberar_Postagens_Profundo(array);
    }
    else
    {
        array->Postagens = (S_Postagem *) realloc(array->Postagens, array->Quantidade * sizeof(S_Postagem));
        if(array->Postagens == NULL){
            printf("ERRO: De-alocagem de comentario");    
        }
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
    Aloca_Dinamicamente_Postagem(array);
    array->Postagens[array->Quantidade-1] = *postagem;
    return;
}

S_Postagem* Achar_Postagem_Pelo_id( S_ArrayPostagens* vetor_postagens, unsigned int postagem_id)
{
    for(int i = 0; i < vetor_postagens->Quantidade; i++) //percorre o vetor de postagens
    {
        if(vetor_postagens->Postagens[i].Id == postagem_id){ //se o id informado for igual ao de alguma postagem retorna ela
            return &vetor_postagens->Postagens[i];
        }
    }
    return NULL; //se não retorna nulo
}

void Postar(S_ArrayPostagens* vetor_postagens, unsigned int id_usuario_ativo)
{
    Aloca_Dinamicamente_Postagem(vetor_postagens); //aloca uma nova postagem no vetor
    S_Postagem* postagem_nova = &vetor_postagens->Postagens[vetor_postagens->Quantidade - 1]; //passa a referencia da memoria a nova variavel para nao termos que escrever o nome original toda vez

    //inicializa os valores da nova postagem
    postagem_nova->Id = GerarId_Postagem(vetor_postagens); //recebe um id valido da funcao que gera
    postagem_nova->Autor_Id = id_usuario_ativo;
    postagem_nova->imagens_do_post.Quantidade = 0;
    postagem_nova->imagens_do_post.Imagens = NULL;
    postagem_nova->QuemCurtiu = (Array_Id_t) {0, NULL};
    postagem_nova->Comentarios.Quantidade = 0;
    postagem_nova->Comentarios.dados_Comentario = NULL;

    //pede a descricao da postagem
    printf("informe a descricao da postagem: ");
    fgets(postagem_nova->Descrit, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(postagem_nova->Descrit);
}

void insta_imprimeImagem(asciiImg_t *img)
{
    printf("%s", img->bytes);
}

void VerPostagem( S_Postagem* postagem, S_ArrayUsuarios* vetor_usuarios)
{
    S_Usuario* autor = Achar_Usuario_Pelo_Id(vetor_usuarios, postagem->Autor_Id); //atribui os dados do autor da postagem a variavel
   
    printf("POSTAGEM %06u\n", postagem->Id); //mostra o id da p
    printf("Autor: %s #%06u\n\n", autor->Nome, autor->Id);
    printf("Imagens: %d\n", postagem->imagens_do_post.Quantidade); //informa quantas imagens tem no post

    for(int i = 0; i < postagem->imagens_do_post.Quantidade; i++) //percorre o vetor de imagens do post, para imprimir todos
    {
        insta_imprimeImagem(postagem->imagens_do_post.Imagens[i]);
    }
    printf("Curtidas: %d \n", postagem->QuemCurtiu.Quantidade); //informa quantas curtidas teve o post
    
    printf("Descricao do post:\n"); //mostra a descricao da postagem
    printf("%s\n\n", postagem->Descrit);
}

S_ArrayPostagens Achar_Postagens_DoUsuario(S_ArrayPostagens* array, unsigned int id_usuario)
{
    S_ArrayPostagens Saida = {0, NULL};

    for(int i = 0; i < array->Quantidade; i++)
    {
        if(array->Postagens[i].Autor_Id == id_usuario){
            Adicionar_Postagem_NoArray(&Saida, &array->Postagens[i]);
        }
    }
    return Saida;
}

unsigned int Gerar_Id_Comentario( S_ArrayComentarios* vetor_comentarios)
{
    unsigned int ultimo_id = 0; //inicializa um valor pra variavel

    for(int i = 0; i < vetor_comentarios->Quantidade - 1; i++) //percorre o vetor de comentarios
    {
        if(vetor_comentarios->dados_Comentario[i].Id > ultimo_id){ //se o id de um comentario for maior que 'ultimo_id', ele ganha esse valor
            ultimo_id = vetor_comentarios->dados_Comentario[i].Id;
        }
    }
    return ultimo_id + 1; //devolve o valor de 'ultimo-id' + 1
}

void Cria_Comentario(S_ArrayComentarios* vetor_comentarios, unsigned int id_postagem, unsigned int id_usuario)
{
    Alocar_Dinamicamente_Comentario(vetor_comentarios); //aloca o tamanho do vetor de comentarios do post

    //atribui uma referencia da memoria na nova variavel de comentario para n precisar escrever a antiga, que é maior toda vez.
    S_Comentario* NovoComentario = &vetor_comentarios->dados_Comentario[vetor_comentarios->Quantidade - 1]; 

    //inicializa os valores do novo comentario
    NovoComentario->Id = Gerar_Id_Comentario(vetor_comentarios); //recebe o id do comentario pelo retorno da funcao 'Gerar_Id_Comentario'
    NovoComentario->Perfil_Id = id_usuario;
    NovoComentario->Postagem_Id = id_postagem;

    //pede o comentario ao usuario
    printf("Escreva o seu comentario aqui: \n");
    fgets(NovoComentario->Mensagem, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(NovoComentario->Mensagem);

    return;
}

void Comentar(S_ArrayUsuarios* vetor_usuarios, S_Postagem* postagem, unsigned int id_usuario)
{
    //chamada da funcao que cria um comentario
    Cria_Comentario(&postagem->Comentarios, postagem->Id, id_usuario); //funcao que cria o comentario de fato

    S_Usuario* Usuario = Achar_Usuario_Pelo_Id(vetor_usuarios, id_usuario); //acha o usuario que comentou pelo id dele
    Alocar_Vetor_Id_Com_Valor(&Usuario->PostagensComentadas, postagem->Id); //adiciona as suas informações em que post ele comentou e aumenta o numero de posts
}

S_Comentario* Achar_Comentario(S_ArrayComentarios* vetor_comentarios, int comentario_id)
{
    for (int i = 0; i < vetor_comentarios->Quantidade; i++) //percorre o vetor de comentarios
    {
        if(vetor_comentarios->dados_Comentario[i].Id == comentario_id){//se id do comentario no vetor for igual ao id informado. retorna os dados do comentario na posicao 'i'
            return &vetor_comentarios->dados_Comentario[i];
        }
    }
    return NULL; //se nao achar um comentario com o id informado, retorna nulo
}

bool Verifica_Autoria( S_Comentario* comentario, unsigned int id_usuario)
{
    return comentario->Perfil_Id == id_usuario; // verifica se o usaurio recebido foi o que realizou o comentario
}

void Editar_Comentario(S_Comentario* comentario)
{
    printf("Edite o seu comentario: \n");
    fgets(comentario->Mensagem, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(comentario->Mensagem);

    printf("Comentario Editado\n");
    return;
}

void Recomentar(S_Postagem* postagem, unsigned int id_autor)
{
    int comentario_id = -1; //inicializa o valor da variavel

    printf("ID do comentario a ser apagado: "); //pede ao usaurio para informar qual comentario deve ser apagado
    scanf("%d", &comentario_id);
    getchar();

    S_Comentario* Comentario = Achar_Comentario(&postagem->Comentarios, comentario_id); //funcao que acha o comentario que deve ser apagado e o retorna

    if(Comentario == NULL) //se comentario for nulo signiica que ele nao existe
    {
        printf("Este comentario nao existe!!!\n");
        return;
    }
    if(Verifica_Autoria(Comentario, id_autor) == false) // verifica se foi o usuario logado que realizou o comentario informado
    {
        printf("Este comentario pertence a outro usuario!!! Voce nao tem autorizacao para modifica-lo\n");
        return;
    }

    Editar_Comentario(Comentario); //chamada da fucao que edita um comentario
}

int Achar_Posicao_DoComentario(S_ArrayComentarios* array, unsigned int id_comentario)
{
    for (int i = 0; i < array->Quantidade; i++)
    {
        if(array->dados_Comentario[i].Id == id_comentario)
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
        array->dados_Comentario = (S_Comentario *) realloc(array->dados_Comentario, array->Quantidade * sizeof(S_Comentario));

        if(array->dados_Comentario == NULL)
            printf("ERRO: De-alocagem de comentario");    
    }
    return;
}

void Apagar_Comentario(S_ArrayComentarios* array, int posicao_comentario)
{
    array->dados_Comentario[posicao_comentario] = array->dados_Comentario[array->Quantidade - 1];
    Desalocar_Comentario(array);
}

void Descomentar(S_ArrayUsuarios* array, S_Postagem* postagem, unsigned int id_autor)
{
    int id_comentario = -1, posicao_comentario = -1;

    printf("ID do comentario a ser apagado: ");
    scanf("%d", &id_comentario);
    getchar();

    posicao_comentario = Achar_Posicao_DoComentario(&postagem->Comentarios, id_comentario);
    if(posicao_comentario < 0)
    {
        printf("Este comentario nao existe!\n");
        return;
    }
    if(Verifica_Autoria(&postagem->Comentarios.dados_Comentario[posicao_comentario], id_autor) == false)
    {
        printf("Este Comentario pertence a outro usuario!\n");
        return;
    }
    S_Comentario* Comentario = &postagem->Comentarios.dados_Comentario[posicao_comentario];

    S_Usuario* Usuario = Achar_Usuario_Pelo_Id(array, Comentario->Perfil_Id);
    Remove_Id_Do_Vetor(&Usuario->PostagensComentadas, Comentario->Id);

    Apagar_Comentario(&postagem->Comentarios, posicao_comentario);

    printf(">>> Postagem apagada! <<<\n");
}

bool Verifica_Situacao_Curtida( S_Postagem* postagem, unsigned int usuario_id)
{
    for(int i = 0; i < postagem->QuemCurtiu.Quantidade; i++) //percorre o vetor de curtidas
    {
        if(postagem->QuemCurtiu.Id[i] == usuario_id) //se no vetor de curtidas tiver o id do usuario, significa que o post ja havia sido curtido por ele
            return true;
    }
    return false; //se nao achar a cutida do usuario no vetor, significa que o post nao havia sido curtido
}

void Curtir(S_ArrayUsuarios* vetor_usuarios, S_Postagem* postagem, unsigned int usuario_id)
{
    if(Verifica_Situacao_Curtida(postagem, usuario_id) == true){ //se o usuario já curtiu a postagem sai da funcao
        return;
    }

    Alocar_Vetor_Id_Com_Valor(&postagem->QuemCurtiu, usuario_id); //guarda o nome do usuario que curtiu o post
    
    S_Usuario* Usuario = Achar_Usuario_Pelo_Id(vetor_usuarios, usuario_id); //acha o usuario que curtiu
    Alocar_Vetor_Id_Com_Valor(&Usuario->PostagensCurtidas, postagem->Id); //aumenta o numero de curtidas realizadas por aquele usuario e o post em que ele curtiu

    printf("Postagem Curtida!\n");
}

void Descurtir(S_ArrayUsuarios* vetor_usuarios, S_Postagem* postagem, unsigned int usuario_id)
{
    if(Verifica_Situacao_Curtida(postagem, usuario_id) != true){ //verifica a situacao da curtida, se nao foi curtida pelo usuario sai da funcao
        return;
    }

    for(int i = 0; i < postagem->QuemCurtiu.Quantidade; i++) // percorre o vetor de curtidas
    {
        if(postagem->QuemCurtiu.Id[i] == usuario_id) //se o id de uma curtida for igual a do usuario
        {
            postagem->QuemCurtiu.Id[i] = postagem->QuemCurtiu.Id[postagem->QuemCurtiu.Quantidade-1]; //atribui o id da ultima
            Desalocar_Vetor_Id(&postagem->QuemCurtiu);
            
            S_Usuario* Usuario = Achar_Usuario_Pelo_Id(vetor_usuarios, usuario_id); //procura o perfil do usuario que descurtiu
            Remove_Id_Do_Vetor(&Usuario->PostagensCurtidas, postagem->Id); //remove o id da postagem do vetor de postagens curtidas pelo usuario
            
            printf("Postagem Descurtida!\n");
            return;
        }
    } 
}

void Imprimir_Curtidas( S_ArrayUsuarios* vetor_usuarios,  S_Postagem* postagem)
{
    S_ArrayUsuarios vetor_usuarios_curtiram; //cria um vetor de usuarios que vai guardar quem curtiu o post

    //incializa os valores desse vetor de usuarios
    vetor_usuarios_curtiram.Quantidade = 0;
    vetor_usuarios_curtiram.Usuarios = NULL;

    S_Usuario* usuario_temporario; //cria um ponteiro de um usuario temporario, ele vai gardar temporariamente a informacoes de quem curtiu

    for(int i = 0; i < postagem->QuemCurtiu.Quantidade; i++) //percorre o vetor de curtidas 
    {
        usuario_temporario = Achar_Usuario_Pelo_Id(vetor_usuarios, postagem->QuemCurtiu.Id[i]); //manda os dados de quem curtiu para a variavel 'usuario_temporario'
        Adicionar_Usuario_No_Vetor(&vetor_usuarios_curtiram, usuario_temporario); //adiciona os dados que estão em 'usuario_temporario' no vetor de usuarios que curtiram
    }

    Imprime_Usuarios(&vetor_usuarios_curtiram); //imprime na tela todos os usuarios que curtiram o post
    Free_Usuarios(&vetor_usuarios_curtiram); //libera da memoria o vetor de usuarios que curtiram, para n ficar ocupando espaco
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

void Upload_Imagem(S_ArrayImagens* vetor_imagens)
{
    char Url[TEXTO_TAM];
    int ImagemColorida = true;
    
    Aloca_Imagem_Dinamicamente(vetor_imagens); //aloca a imagem dinamicamente
    
    printf("informe a URL da Imagem: ");
    fgets(Url, TEXTO_TAM, stdin);
    Retira_quebra_de_linha(Url); //remove '\n' da url para evitar problemas

    //pede o usuario para informar se quer a imagem colorida ou nao
    printf("Modo de cor: \n");
    printf("0 -  Preto e Branco \n");
    printf("1 - Colorido \n");
    scanf("%d", &ImagemColorida); //se for 1 entao passa o valor true para a funcao, e se for 0 passa false
    getchar();

    vetor_imagens->Imagens[vetor_imagens->Quantidade-1] = insta_carregaImagem(Url, ImagemColorida, IMAGEM_NUM_COLUNAS_PADRAO); //carrega a imagem da url e atribui na struct
    insta_imprimeImagem(vetor_imagens->Imagens[vetor_imagens->Quantidade-1]); //imprime a imagem
}

void Remover_Imagem_DoArray(S_ArrayImagens* vetor_imagens, unsigned int posicao)
{
    if(vetor_imagens->Quantidade == 0 || posicao < 0 || posicao > vetor_imagens->Quantidade - 1){
        return;
    }
    
    insta_liberaImagem(vetor_imagens->Imagens[posicao]);
    for(int i = posicao; i < vetor_imagens->Quantidade - 1; i++){
        vetor_imagens->Imagens[i] = vetor_imagens->Imagens[i + 1];
    }
    
    (vetor_imagens->Quantidade)--;
    if(vetor_imagens->Quantidade == 0){
        Liberar_Imagens(vetor_imagens);
    }
}

void Editar_Post(S_ArrayPostagens* postagens, S_Postagem* postagem)
{
    int opcao = 0;
    do
    {
        printf("[1] Alterar Descricao\n");
        printf("[2] Adicionar Imagens\n");
        printf("[3] Remover Imagem\n");
        printf("[0] Cancelar\n");
        
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
        {
            case 0:
                return;
            case 1:
                printf("Escreva uma descricao para seu postagem: ");
                fgets(postagem->Descrit, TEXTO_TAM, stdin);
                Retira_quebra_de_linha(postagem->Descrit);
                break;

            case 2:
                do
                {
                    Upload_Imagem(&postagem->imagens_do_post);
                    printf("Adicionar outra imagem? (1 - sim, 0 - nao): \n");
                
                    scanf("%d", &opcao);
                    getchar();
                } while (opcao != 0); 
                break;
            case 3:
                printf("Posicao da imagem: \n");
                scanf("%d", &opcao);

                Remover_Imagem_DoArray(&postagem->imagens_do_post, opcao-1);
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

        if(Posicao_Aux >= 0){
            Remove_Posicao_Vetor_Id(&Usuario_Atual->PostagensCurtidas, Posicao_Aux);
        }
        
        do
        {
            Posicao_Aux = Checar_Usuario_Comentou(Usuario_Atual, post_id);
            if(Posicao_Aux < 0) break;
            
            Remove_Posicao_Vetor_Id(&Usuario_Atual->PostagensComentadas, Posicao_Aux);
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

void Salvar_Imagens_NoArquivo( S_ArrayImagens* vetor_imagens, FILE* arquivo)
{
    for(int i = 0; i < vetor_imagens->Quantidade; i++)
    {
        fwrite(&vetor_imagens->Imagens[i]->nBytes, sizeof(int), 1, arquivo);
        fwrite(vetor_imagens->Imagens[i]->bytes, sizeof(uint8_t), vetor_imagens->Imagens[i]->nBytes, arquivo);
    }
}

void SalvarArquivo_Postagens( S_ArrayPostagens* vetor_postagens, char* arquivo)
{
    FILE* Arquivo = fopen(arquivo, "wb");

    if(Arquivo == NULL)
    {
        printf("ERRO: Abertura de arquivo");
        return;
    }
    
    fwrite(&(vetor_postagens->Quantidade), sizeof(unsigned int), 1, Arquivo);
    fwrite(vetor_postagens->Postagens, sizeof(S_Postagem), vetor_postagens->Quantidade, Arquivo);
    for(int i = 0; i < vetor_postagens->Quantidade; i++)
    {

        Salvar_Imagens_NoArquivo(&vetor_postagens->Postagens[i].imagens_do_post, Arquivo);

        fwrite(vetor_postagens->Postagens[i].QuemCurtiu.Id, sizeof(unsigned int), vetor_postagens->Postagens[i].QuemCurtiu.Quantidade, Arquivo);

        fwrite(vetor_postagens->Postagens[i].Comentarios.dados_Comentario, sizeof(S_Comentario), vetor_postagens->Postagens[i].Comentarios.Quantidade, Arquivo);
    }
    
    fclose(Arquivo);
    return;
}

void Carregar_Imagens_DoArquivo(S_ArrayImagens* vetor_imagens, FILE* arquivo)
{
    vetor_imagens->Imagens = (asciiImg_t **) malloc(vetor_imagens->Quantidade * sizeof(asciiImg_t*)); //aloca dinamicamente quantas imagens sao

    for(int i = 0; i < vetor_imagens->Quantidade; i++) //percorre o vetor de imagens
    {
        vetor_imagens->Imagens[i] = (asciiImg_t *) malloc(sizeof(asciiImg_t)); //aloca um valor em  'Imagens[i]' 
        fread(&vetor_imagens->Imagens[i]->nBytes, sizeof(int), 1, arquivo); // le quantos bytes tem aquela imagem

        vetor_imagens->Imagens[i]->bytes = (uint8_t *) malloc(vetor_imagens->Imagens[i]->nBytes * sizeof(uint8_t)); //aloca dinamicamente a imagem em 'Imagens[i]'
        fread(vetor_imagens->Imagens[i]->bytes, sizeof(uint8_t), vetor_imagens->Imagens[i]->nBytes, arquivo); //le a imagem de acordo com o numero de bytes dela
    }

    if(vetor_imagens->Quantidade == 0){ //se a quantidade de imagens for 0 muda a alocacao para um valor nulo
        vetor_imagens->Imagens = NULL; 
    }
}

void CarregarArquivo_Postagens(S_ArrayPostagens* vetor_postagens, char* arquivo)
{
    FILE* arquivo_postagens = fopen(arquivo, "rb"); //abre o arquivo de postagens

    if(arquivo_postagens == NULL) //se o arquivo não for aberto, imprime mensagem de erro e sai da funcao
    {
        printf("O arquivo de postagens nao foi encontrado!!!!\n");
        return;
    }
    
    fread(&(vetor_postagens->Quantidade), sizeof(unsigned int), 1, arquivo_postagens); //Le quantos posts existem
    vetor_postagens->Postagens = (S_Postagem*) malloc(vetor_postagens->Quantidade * sizeof(S_Postagem)); //aloca dinamicamente o vetor de postagens
    
    fread(vetor_postagens->Postagens, sizeof(S_Postagem), vetor_postagens->Quantidade, arquivo_postagens);//le os dados das postagens
    
    for(int i = 0; i < vetor_postagens->Quantidade; i++) 
    {
        Carregar_Imagens_DoArquivo(&vetor_postagens->Postagens[i].imagens_do_post, arquivo_postagens); //chamada da funcao que carrega as imagens
        
        Array_Id_t* id_Curtidas = &vetor_postagens->Postagens[i].QuemCurtiu; //atribui a referencia da memoria a 'id_Curtidas' para n precisar ficar escrevendo o nome de variavel gigantesco
        id_Curtidas->Id = (unsigned int *) malloc(id_Curtidas->Quantidade * sizeof(unsigned int)); // aloca dinamicamente o vetor de 'Id'

        fread(vetor_postagens->Postagens[i].QuemCurtiu.Id, sizeof(unsigned int), vetor_postagens->Postagens[i].QuemCurtiu.Quantidade, arquivo_postagens); //Le do arquivo os ids de quem curtiu o post

        if(id_Curtidas->Quantidade == 0){ //se ninguem curtiu o post '[i]' muda o valor alocado para nulo
            id_Curtidas->Id = NULL;
        }
        
        S_ArrayComentarios* vetor_comentarios = &vetor_postagens->Postagens[i].Comentarios; //manda a referencia da memoria para a variavel de mesmo tipo, para n precisar ficar digitando um nome de variavel gigantesco
        vetor_comentarios->dados_Comentario = (S_Comentario *) malloc(vetor_comentarios->Quantidade * sizeof(S_Comentario)); // aloca dinamicamente o vetor de comentarios
  
        fread(vetor_postagens->Postagens[i].Comentarios.dados_Comentario, sizeof(S_Comentario), vetor_postagens->Postagens[i].Comentarios.Quantidade, arquivo_postagens); //le os dao

        if(vetor_comentarios->Quantidade == 0){ //Se não houver comentarios, muda o valor alocado para nulo
            vetor_comentarios->dados_Comentario = NULL;
        }
    }
    
    fclose(arquivo_postagens); //fecha o arquivo de postagens
    return;
}

//COMENTARIO

void Adicionar_Comentario_NoArray(S_ArrayComentarios* array,  S_Comentario* comentario)
{
    Alocar_Dinamicamente_Comentario(array);
    array->dados_Comentario[array->Quantidade-1] = *comentario;
    return;
}

S_ArrayComentarios Achar_Comentarios_DoUsuario( S_ArrayComentarios* array, unsigned int id_usuario)
{
    S_ArrayComentarios Saida = {0, NULL};

    for(int i = 0; i < array->Quantidade; i++)
    {
        S_Comentario* Comentario = &array->dados_Comentario[i];
        if(Verifica_Autoria(Comentario, id_usuario) == true)
            Adicionar_Comentario_NoArray(&Saida, Comentario);
    }
    
    return Saida;
}

void Imprime_Comentario( S_Comentario* comentario,  S_ArrayUsuarios* usuarios)
{
    S_Usuario* Autor = Achar_Usuario_Pelo_Id(usuarios, comentario->Perfil_Id); //recebe os dados do usuario que comentou

    printf("Comentario #%06u\n", comentario->Id); //imprime o id da postagem

    printf("Usuario %s #%06u comentou:\n", Autor->Nome, Autor->Id); //imprime quem comentou
 
    printf("%s\n", comentario->Mensagem);//imprime o comentario propriamente dito
    return;
}

void Imprime_Comentarios( S_ArrayComentarios* vetor_comentarios,  S_ArrayUsuarios* vetor_usuarios)
{
    if(vetor_comentarios->Quantidade < 1){ //se a quantidade de comentarios do vetor passado for menor que 1, significa que não ha nenhum comentario
        printf("A postagem nao tem comentarios\n");
    }
    else
    {
        for(int i = 0; i < vetor_comentarios->Quantidade; i++){ //percorre os comentarios imprimindo um por um
            Imprime_Comentario(&vetor_comentarios->dados_Comentario[i], vetor_usuarios);
        }
    }
    return;
}

bool Imprime_Comentarios_DoUsuario( S_ArrayComentarios* vetor_comentarios,  S_ArrayUsuarios* vetor_usuarios, unsigned int id_usuario)
{
    bool TemComentarios = false; //booleano que checa se existe comentario do usario logado no post

    if(vetor_comentarios->Quantidade < 1) // se a quantidade de comentario for menor que 1 entao nao ha nenhum comentario na postagem
    {
        printf("A postagem nao tem comentarios\n");
        return false;
    }

    for(int i = 0; i < vetor_comentarios->Quantidade; i++) //percorre o vetor de comentarios
    {
        S_Comentario* Comentario = &vetor_comentarios->dados_Comentario[i]; //cria uma referencia ao comentario para nao ficar tao verbalizado a variavel

        if(Verifica_Autoria(Comentario, id_usuario) == true) //verifica se foi o usuario que realizou o comentario
        {
            Imprime_Comentario(Comentario, vetor_usuarios);
            TemComentarios = true; // se a autoria for verdadeira significa que existe psotagens desse user
        }
    }
    
    if (TemComentarios == false) //se a variavel for false, siginifica que o usuario nao fez nenhuma postagem
    {
        printf("Nenhum comentario foi feito nessa postagem\n");
        return false;
    }
    return true;
}

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

void Imprimir_Comentarios(S_Postagem* postagem) //menu que tem as opcoes de impressao dos comentarios
{
    int opcao = 0;
    unsigned int Id = 0;

    printf("1 - Todos\n");
    printf("2 - Seus\n");
    printf("3 - Autor\n");
    if(Usuario_Ativo->Id != Usuario_Logado->Id) //ver somente os comentarios do usuario ativo
    {
        printf("4 - Usuario Ativo: %s #%06u\n", Usuario_Ativo->Nome, Usuario_Ativo->Id);
    }
    printf("5 - Usuario Especifico\n");
    printf("0 - Cancelar\n");
    
    printf("Escolha: ");
    scanf("%d", &opcao);
    getchar();

    switch (opcao)
    {
    case 0:
        break;
    case 1:
        Imprime_Comentarios(&postagem->Comentarios, &Vetor_Usuarios); //imprime todos os comentarios da postagem
        break;
    case 2:
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Logado->Id); //imprime os comentarios do usuario logado
        break;
    case 3:
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, postagem->Autor_Id); //imprime os comentario do autor do post
        break;
    case 4:
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Ativo->Id); //imprime os comentarios do usaurio ativo
        break;
    case 5:
        printf("Id do usuario: "); //imprime os comentarios de um usuario especifico
        scanf("%u", &Id);
        getchar();
        Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Id);
        break;
    }
}

void Menu_Postagem(S_Postagem* postagem)
{
    int opcao = 0;
    do
    {
        printf("1 - Curtir\n");
        printf("2 - Descurtir\n");
        printf("3 - Ver curtidas\n");
        printf("4 - Comentar no post\n");
        printf("5 - Ver comentarios\n");
        
        if(postagem->Autor_Id == Usuario_Logado->Id) //verifica se o post é do usuario logado
        {
            printf("6 - Editar comentario\n");
            printf("7 - Apagar comentario\n");
            printf("8 - Editar postagem\n");
            printf("9 - Apagar postagem\n");
        }
        printf("0 - Sair do post\n");
        
        printf("escolha: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
        {
        case 0: 
            return; //sai do menu
        case 1:
            Curtir(&Vetor_Usuarios, postagem, Usuario_Logado->Id); //chamada da funcao que curte o post
            break;
        case 2:
            Descurtir(&Vetor_Usuarios, postagem, Usuario_Logado->Id); //fucao que descurte um post
            break;
        case 3:
            Imprimir_Curtidas(&Vetor_Usuarios, postagem); //mostra quem curtiu o post
            break;
        case 4:
            Comentar(&Vetor_Usuarios, postagem, Usuario_Logado->Id); //usuario logado comenta no post
            break;
        case 5:
            Imprimir_Comentarios(postagem); //mostra os comentarios
            break;
        case 6:  //opcao que peremite editar um comentario feito
            if(postagem->Autor_Id == Usuario_Logado->Id) //verifica se o usuario logado e o mesmo que realizou a postagem
            {
                Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Logado->Id);
                Recomentar(postagem, Usuario_Logado->Id); //funcao que muda os comentarios feitos 
            }
            break;
        case 7:
            if(postagem->Autor_Id == Usuario_Logado->Id) //verifica se o usuario logado e o mesmo que realizou a postagem
            {
                Imprime_Comentarios_DoUsuario(&postagem->Comentarios, &Vetor_Usuarios, Usuario_Logado->Id);
                Descomentar(&Vetor_Usuarios, postagem, Usuario_Logado->Id); 
            }
            break;
        case 8:
            if(postagem->Autor_Id == Usuario_Logado->Id)
                Editar_Post(&Vetor_Postagens, postagem);
            break;
        case 9:
            if(postagem->Autor_Id == Usuario_Logado->Id)
            {
                Apagar_Post(&Vetor_Postagens, postagem->Id, &Vetor_Usuarios);
            }
            return;
        }
    } while (1);
}

void Mostrar_Postagem(S_Postagem* postagem)
{
    //se o valor de postagem for nulo imprime mensagem de erro e sai da funcao
    if(postagem == NULL)
    {
        printf("Postagem nao foi encontrado!!\n");
        return;
    }

    VerPostagem(postagem, &Vetor_Usuarios);
    Menu_Postagem(postagem);
    return;
}

void Mostrar_Postagens_EmMenu(S_ArrayPostagens* array)
{
    int opcao = 0, Indice_atual = 0;
    if(array->Quantidade < 1)
    {
        printf("Este perfil nao possui nenhum postagem\n");
        return;
    }

    do
    {
        S_Postagem* Postagem_Atual = &array->Postagens[Indice_atual];
        S_Usuario* Postagem_Autor = Achar_Usuario_Pelo_Id(&Vetor_Usuarios, Postagem_Atual->Autor_Id);

        printf("-------------------------------------------------\n");

        printf("#%06u\n", Postagem_Atual->Id);

        printf("Postada por %s #%06u\n", Postagem_Autor->Nome, Postagem_Autor->Id);

        printf("\"%.32s\"\n", Postagem_Atual->Descrit);

        printf("-------------------------------------------------\n");

        printf("[1] Anterior\n");
        printf("[2] Proxima\n");
        printf("[3] Ver Mais\n");
        printf("[0] Cancelar\n");

        printf("Escolha ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
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
    S_ArrayPostagens Curtidas;

    Curtidas.Quantidade = 0;
    Curtidas.Postagens =  NULL;

    for(int i = 0; i < Usuario_Ativo->PostagensCurtidas.Quantidade; i++){
        Adicionar_Postagem_NoArray(&Curtidas, Achar_Postagem_Pelo_id(&Vetor_Postagens, Usuario_Ativo->PostagensCurtidas.Id[i]));
    }

    Mostrar_Postagens_EmMenu(&Curtidas);
    Liberar_Postagens(&Curtidas);
    return;
}

void Mostrar_Tela_UsuarioAtivo()
{
    S_ArrayPostagens Posts_DoUsuario = {0, NULL};

    do
    {
        int opcao = 0;

        Mostrar_Usuario(Usuario_Ativo);

        printf("[1] Ver Postagens Deste Perfil \n");
        printf("[2] Ver Postagens Curtidas Por Este Perfil\n");
        printf("[3] Estatisticas Do Perfil\n");
        printf("[0] Voltar\n");
        
        printf(">> ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao)
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
    int opcao = 0, Indice_atual = 0;
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

        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
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
    int opcao = 0;
    do
    {
        Mostrar_Usuario(Usuario_Logado);

        printf("[1] Alterar Nome\n");
        printf("[2] Alterar Email\n");
        printf("[3] Alterar Senha\n");
        printf("[4] Alterar Bio\n");
        printf("[0] Concluir\n");
        
        printf(">> ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
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

void Menu_Principal()
{
    S_ArrayPostagens posts_Usuario; //array dos posts do usuario logado
    
    //inicializando esse valores
    posts_Usuario.Quantidade = 0;
    posts_Usuario.Postagens = NULL;

    S_ArrayUsuarios resultado_usuarios_busca; //array de usuarios retornados da funcao de busca

    //inicializando esses valores 
    resultado_usuarios_busca.Quantidade = 0;
    resultado_usuarios_busca.Usuarios = NULL;

    char usuario_buscado[NOME_TAM];

    do
    {
        int opcao = 0;
        Mostrar_Usuario(Usuario_Logado);
        printf("1 - Nova Postagem\n");
        printf("2 - Ver Postagem \n");
        printf("3 - Ver Postagens Deste Perfil \n");
        printf("4 - Ver Todas As Postagens\n");
        printf("5 - Ver Postagens Curtidas\n");
        printf("6 - Buscar Perfil\n");
        printf("7 - Ver Todos Os Perfis\n");
        printf("8 - Estatisticas Deste Perfil\n");
        printf("9 - Alterar Perfil\n");
        printf("0 - Deslogar\n");
        
        printf("opcao: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao)
        {
        case 0: //Desloga o usuario e sai dp menu principal
            Deslogar_Usuario(&Usuario_Ativo);
            Deslogar_Usuario(&Usuario_Logado);
            return;

        case 1: //realiza nova postagem

            Postar(&Vetor_Postagens, Usuario_Ativo->Id);

            do
            {
                Upload_Imagem(&Vetor_Postagens.Postagens[Vetor_Postagens.Quantidade-1].imagens_do_post); //chamada da funcao que realiza o upload da imagem 
                printf("Adicionar outra imagem?\n(1 - sim, 0 -nao)\nescolha: ");
                scanf("%d", &opcao);
                getchar();
            } while (opcao != 0); //se a escolha for 0, sai do loop
            break;

        case 2: //funcao que visualiza uma postagem

            printf("informe o ID da postagem: ");
            scanf("%d", &opcao);
            getchar();
            
            Mostrar_Postagem(Achar_Postagem_Pelo_id(&Vetor_Postagens, opcao)); //recebe a strutc da postagem retornada da funcao 'Achar_Postagem_PorId'
            break;
        case 3:
            posts_Usuario = Achar_Postagens_DoUsuario(&Vetor_Postagens, Usuario_Logado->Id);
            Mostrar_Postagens_EmMenu(&posts_Usuario);
            Liberar_Postagens(&posts_Usuario);
            break;
        case 4:
            Mostrar_Postagens_EmMenu(&Vetor_Postagens);
            break;
        case 5:
            Mostrar_Postagens_Curtidas();
            break;
        case 6:
            printf("Buscar por? (1 - nome, 2 - email, 0 - cancelar): ");
            
            scanf("%d", &opcao);
            getchar();

            if(opcao == 0) break;

            printf("Escreva parte do %s: ", opcao == 1 ? "nome" : "email");
            fgets(usuario_buscado, NOME_TAM, stdin);
            Retira_quebra_de_linha(usuario_buscado);

            resultado_usuarios_busca = opcao == 1 ? Buscar_Usuarios_PorNome(&Vetor_Usuarios, usuario_buscado) : Buscar_Usuarios_PorEmail(&Vetor_Usuarios, usuario_buscado);


            if(resultado_usuarios_busca.Quantidade == 0)
            {
                printf("Usuario nao encontrado! \n");
            }
            else Mostrar_Usuarios_EmMenu(&resultado_usuarios_busca);
            Free_Usuarios(&resultado_usuarios_busca);
            break;
        case 7:
            printf("[1] Por Nome\n");
            printf("[2] Por Email\n");
            printf("[0] Por Id\n");
            
            scanf("%d", &opcao);
            getchar();
            
            Ordenar_Usuarios(&Vetor_Usuarios, opcao);
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

int Menu_Inicial()
{
    int opcao = 0;
    
    do
    {
        printf("1 - Logar\n");
        printf("2 - Cadastrar\n");
        printf("3 - Listar Usuarios\n");
        printf("0 - Sair\n");
        
        printf("escolha: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao)
        {
        case 0:
            return 1; //retorna 1 para caso o usuario deseje sair do programa
        case 1:
            //chama a funcao que loga o usuario
            Usuario_Ativo = Usuario_Logado = Logar_Usuario(&Vetor_Usuarios); //iguala o valor de usuario ativo e logado
            if(Usuario_Logado != NULL) //se o valor retonado da fucao 'Logar_Usuario' for diferente de nulo mostra o menu principal
                Menu_Principal();
            break;
        case 2:
            //Chama a funcao de cadastrar novo usuario e passa o vetor de usuarios como parametro
            Cadastrar_NovoUsuario(&Vetor_Usuarios);
            break;
        case 3:
            //pede para informar de que modo a listagem ficara organizada
            printf("1 - Por Nome\n");
            printf("2 - Por Email\n");
            printf("0 - Por Id\n");
            
            scanf("%d", &opcao);
            getchar();
            
            Ordenar_Usuarios(&Vetor_Usuarios, opcao); //chamada da funcao que vai ordenar o vetor de usuarios com base na opcao escolhida
            Imprime_Usuarios(&Vetor_Usuarios); //chamada da funcao que imprime
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
        if(Menu_Inicial() == 1)
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