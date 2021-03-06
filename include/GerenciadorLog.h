#ifndef GERENCIADORLOG_H
#define GERENCIADORLOG_H

#include "GerenciadorArquivos.h"
#include "Utilidades.h"

class GerenciadorLog
{
private:
    GerenciadorArquivos *ga;
    String retornaTempo();
public:
    ~GerenciadorLog();
    GerenciadorLog(GerenciadorArquivos *gerenciadorArquivos);
    
    void inserirRegistro(const char *tag, const char *mensagem);
};


#endif