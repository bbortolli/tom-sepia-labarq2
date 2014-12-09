#include <stdio.h>
#include <time.h>
#include <xmmintrin.h>

// referencia: [1]
struct pontoflutuante
{
	float x, y, z, w;


};

int main(int argc, char** argv)
{
	clock_t start, end;
	double cpu_time_used;
	char formato[5], info[24];
	unsigned char * imagem, *temp, soma[4] = { 40, 20, 0, 0 }, subtracao[4] = { 0, 0, 20, 0 };
	int media, redt, red, green, blue, j, *temp2, *temp3, * temp4;
	FILE *file;
	long int tamanhoFile, tamanhoImg;
	char inputtFileDir[64] = "in\\", outputFileDir[64] = "out\\";
	struct pontoflutuante a, b;

	a.x = 0.3; a.y = 0.59; a.z = 0.11; a.w = 1.0; 



	strcat(inputtFileDir, argv[1]);
	strcat(outputFileDir, argv[1]);


	if ((file = fopen(inputtFileDir, "rb")) != NULL)
	{
		// Pegar tamanho do arquivo
		fseek(file, 0, SEEK_END);
		tamanhoFile = ftell(file);
		rewind(file);

		// Alocar memoria suficiente
		imagem = (unsigned char*)malloc(sizeof(unsigned char)*tamanhoFile);
		if (imagem == NULL)
			exit(2);

		// Leitura dos dados
		fgets(formato, 10, file);
		fgets(info, 30, file);
		tamanhoImg = fread(imagem, sizeof(unsigned char), tamanhoFile, file);
		fclose(file);
	}
	else
		exit(1);

	start = clock();


	//Calculos para o tom sepia paralelo (SSE2)
	
	

		for (j = 0; j < tamanhoImg - 1; j += 3)
		{
			// media = 0.3*r + 0.59*g + 0.11*b
			// r = media + 40
			// g = media + 20
			// b = media - 20

			
			temp = &imagem[j];
			//red = (int)imagem[j];
			///green = (int)imagem[j + 1];
			//blue = (int)imagem[j + 2];
			//redt = (int)imagem[j + 3];


			b.x = (float)imagem[j];
			b.y = (float)imagem[j + 1];
			b.z = (float)imagem[j + 2];
			b.w = (float)imagem[j + 3];


			__asm
			{
				lea eax, b
				lea ebx, a
				movdqu xmm0, [eax]
				movdqu xmm1, [ebx]
				mulps xmm0, xmm1
				movdqu [eax], xmm0
			}

			imagem[j] = (unsigned char)b.x;
			imagem[j + 1] = (unsigned char)b.y;
			imagem[j + 2] = (unsigned char)b.z;
			imagem[j + 3] = (unsigned char)b.w;


			imagem[j] += imagem[j + 1] + imagem[j + 2];
			imagem[j + 1] = imagem[j];
			imagem[j + 2] = imagem[j];

			__asm
			{
				mov eax, temp
				movd mm0, [eax]
				movd mm3, soma
				movd mm4, subtracao
				paddusb mm0, mm3
				psubusb mm0, mm4


				mov eax, temp
				movd[eax], mm0

				emms
			}


			
		}

		end = clock();


	// Escrever novo arquivo
	if ((file = fopen(outputFileDir, "wb")) != NULL)
	{
		fputs(formato, file);
		fputs(info, file);
		fwrite(imagem, sizeof(unsigned char), tamanhoImg, file);
	}

	fclose(file);
	free(imagem);

	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	fprintf(stderr, "tempo SSE = %f segundos\n", cpu_time_used);
	

	return 0;
}


/*
Referencias:
[1]: http://neilkemp.us/src/sse_tutorial/sse_tutorial.html

*/

