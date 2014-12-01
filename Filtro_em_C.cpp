#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <time.h>

int main()
{
    clock_t start, end;
    double cpu_time_used;
    char formato[5], info[24];
    unsigned char * imagem;
    int media, newColor, red, green, blue;
    FILE *file;
    long int tamanhoFile, tamanhoImg;

    if((file = fopen("Luna.pnm","rb")) != NULL)
    {
        // Pegar tamanho do arquivo
        fseek (file , 0 , SEEK_END);
        tamanhoFile = ftell (file);
        rewind (file);

        // Alocar memoria suficiente
        imagem = (unsigned char*) malloc (sizeof(unsigned char)*tamanhoFile);
        if(imagem == NULL)
            exit(2);

        // Leitura dos dados
        fgets(formato,10, file);
        fgets(info,30, file);
        tamanhoImg = fread(imagem,sizeof(unsigned char),tamanhoFile,file);
        fclose(file);
    }
    else
        exit(1);

    start = clock();
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
        imagem[j] = std::min(255, newColor+50);
        imagem[j+1] = std::min(255, newColor+25);
        imagem[j+2] = std::max(0, newColor-20);
    }
    end = clock();

    // Escrever novo arquivo
    if((file = fopen("teste.pnm","wb")) != NULL)
    {
        fputs(formato,file);
        fputs(info,file);
        fwrite(imagem , sizeof(unsigned char), tamanhoImg, file);
    }

    fclose(file);
    free(imagem);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "Tempo do processamento em segundos: " << cpu_time_used << std::endl;

    return 0;
}
