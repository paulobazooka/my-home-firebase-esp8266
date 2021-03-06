#include "GerenciadorLog.h"

GerenciadorLog::~GerenciadorLog(){}

GerenciadorLog::GerenciadorLog(GerenciadorArquivos *gerenciadorArquivos){
    ga = gerenciadorArquivos;
}

void GerenciadorLog::inserirRegistro(const char *tag, const char *mensagem){
    String tempo = retornaTempoCorrente();
    String conteudo = tempo + String(";") + tag + String(";") + mensagem + "\n";
    ga->adicionarConteudoArquivo(ARQUIVO_CONFIG_REGISTROS, conteudo.c_str());
}
