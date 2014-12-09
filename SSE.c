#include <stdio.h>
#include <time.h>

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
	unsigned char * imagem, *ponteiro, soma[8] = { 40, 20, 0, 40, 20, 0, 0, 0 }, subtracao[8] = { 0, 0, 20, 0, 0, 20, 0, 0 };
	int j;
	FILE *file;
	long int tamanhoFile, tamanhoImg;
	char inputtFileDir[64] = "in\\", outputFileDir[64] = "out\\";
	struct pontoflutuante fatores;

	fatores.x = 0.3; fatores.y = 0.59; fatores.z = 0.11; fatores.w = 1.0;



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
	
	

		for (j = 0; j < tamanhoImg - 1; j += 6)
		{
			// media = 0.3*r + 0.59*g + 0.11*b
			// r = media + 40
			// g = media + 20
			// b = media - 20

			
			ponteiro = &imagem[j];
						

			__asm
			{
				// Converte para floats 8 bytes da imagem e e coloca em b e c
				mov edx, ponteiro
				movd xmm1, [edx]
				pxor xmm4, xmm4
				punpcklbw xmm1, xmm4
				punpcklwd xmm1, xmm4
				cvtdq2ps xmm1, xmm1


				movd xmm2, [edx + 3]
				punpcklbw xmm2, xmm4
				punpcklwd xmm2, xmm4
				cvtdq2ps xmm2, xmm2

				
				// Multiplica as cores das imagems pelos fatores
				lea eax, fatores
				movups xmm0, [eax]
				mulps xmm1, xmm0
				mulps xmm2, xmm0


				cvtps2dq xmm1,xmm1
				cvtps2dq xmm2,xmm2

				packusdw xmm1, xmm4
				packuswb xmm1, xmm4
				packusdw xmm2, xmm4
				packuswb xmm2, xmm4


				movd[edx], xmm1
				movd[edx + 3], xmm2

				mov eax, 0
				mov al, [edx]
				add al, [edx + 1]
				add al, [edx + 2]

				mov [edx], al
				mov [edx + 1], al
				mov [edx + 2], al

				mov al, [edx + 3]
				add al, [edx + 4]
				add al, [edx + 5]

				mov [edx + 3], al
				mov [edx + 4], al
				mov [edx + 5], al


				
				movq mm0, [edx]
				movq mm3, soma
				movq mm4, subtracao
				paddusb mm0, mm3
				psubusb mm0, mm4


				movq[edx], mm0

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

