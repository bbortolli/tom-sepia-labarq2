#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <time.h>

using namespace std;

int main()
{
    char formato[5], info[24];
    unsigned char * imagem;
    int media, newColor, red, green, blue;
    FILE *file;
    long int tamanhoFile, tamanhoImg;

    if((file = fopen("UFSCar.pnm","rb")) != NULL)
    {
        // Pegar tamanho do arquivo
        fseek (file , 0 , SEEK_END);
        tamanhoFile = ftell (file);
        rewind (file);
        cout << "- ARQUIVO LIDO COM SUCESSO." << endl << "- TAMANHO FILE: " << tamanhoFile << " bytes" << endl;

        // Alocar memoria suficiente
        imagem = (unsigned char*) malloc (sizeof(unsigned char)*tamanhoFile);
        if(imagem == NULL)
        {
            cout << "Erro de memoria! ";
            exit(1);
        }
        else
            cout << "- MEMORIA ALOCADA!" << endl;

        // Leitura dos dados
        fgets(formato,10, file);
        fgets(info,30, file);
        tamanhoImg = fread(imagem,sizeof(unsigned char),tamanhoFile,file);
        fclose(file);
        cout << "- TAMANHO IMAGEM(sem cabecalho): " << tamanhoImg << " bytes" << endl;
    }
    else
    {
        cout << "FALHA NA LEITURA!!!";
        exit(2);
    }

    //Calculos para o tom de sepia
    for(int j = 0; j < tamanhoImg-1; j+=3)
    {
        // Calculo do grayscale
        red = int(imagem[j]);
        green = int(imagem[j+1]);
        blue = int(imagem[j+2]);
        media = int((red + (2*green) + blue)/4);
        // Aplicação do efeito sépia
        newColor = int(0.3*media + 0.59*media + 0.11*media);
        imagem[j] = min(255, newColor+50);
        imagem[j+1] = min(255, newColor+25);
        imagem[j+2] = max(0, newColor-20);
    }
    // Escrever novo arquivo
    if((file = fopen("teste.pnm","wb")) != NULL)
    {
        fputs(formato,file);
        fputs(info,file);
        fwrite(imagem , sizeof(unsigned char), tamanhoImg, file);
        cout << "- ARQUIVO ESCRITO COM SUCESSO." << endl;
    }
    fclose(file);
    free(imagem);

    return 0;
}
