// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>


int main()
{
	FILE * img;				//Abre a imagem
	FILE * gs;				//Gera nova imagem em GS
	unsigned char *imagem;		//Recebe todos os bits da imagem
	long posicao;
	long fim;
	long i, soma, iRed, iGreen, iBlue, oRed, oGreen, oBlue;

	//Abre a imagem, e copia o header a nova imagem
	img = fopen("Luna.pnm", "rb");
	gs = fopen("LunaSepia.pnm", "wb");

	fseek(img,0,SEEK_END);
	fim = ftell(img);
	fseek(img,0,SEEK_SET);

	posicao = ftell(img);
	printf("\n%d\n", posicao);

	imagem = (unsigned char*) malloc (fim + 20);

	printf("%d\n", fim);

	fgets(imagem, 16, img);
	fputs(imagem, gs);
	printf("%s",imagem);
	fgets(imagem, 50, img);
	fputs(imagem, gs);
	printf("%s",imagem);
	/*fgets(imagem,10,img);
	fputs(imagem,gs);
	printf("%s",imagem);*/

	posicao = ftell(img);				//guarda posiçaõ do inicio dos bytes de cor
	printf("\n\n%d", posicao);
	//fseek(img, posicao, SEEK_SET);		//posiciona ponteiro do arquivo para o inicio de bytes de cor


	fread(imagem, 1, fim - posicao, img);	//Le todos os bites de cor

    //getchar();

	i = 0;
	while (i != (fim - posicao) -1)
	{
	 //   printf("\n%d", i);

		iRed = imagem[i];
		iGreen = imagem[i + 1];
		iBlue = imagem[i + 2];

		oRed = (iRed * 0.393) + (iBlue * 0.769) + (iGreen * 0.189);
		oGreen = (iRed * 0.349) + (iBlue * 0.686) + (iGreen * 0.168);
		oBlue = (iRed * 0.272) + (iBlue * 0.534) + (iGreen * 0.131);
		if(oRed > 255)
            oRed = 255;
        if(oGreen > 255)
            oGreen = 255;
        if(oBlue >255)
            oBlue = 255;


		imagem[i] = oRed;
		imagem[i + 1] = oGreen;
		imagem[i + 2] = oBlue;

		i += 3;

	}



	fwrite(imagem,1, fim - posicao, gs);


	return 0;
}

