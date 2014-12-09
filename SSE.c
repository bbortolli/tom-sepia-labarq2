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
	unsigned char * imagem, *temp, soma[8] = { 40, 20, 0, 40, 20, 0, 0, 0 }, subtracao[8] = { 0, 0, 20, 0, 0, 20, 0, 0 };
	int media, redt, red, green, blue, j, *temp2, *temp3, * temp4;
	FILE *file;
	long int tamanhoFile, tamanhoImg;
	char inputtFileDir[64] = "in\\", outputFileDir[64] = "out\\";
	struct pontoflutuante fatores, b, c;

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

			
			temp = &imagem[j];
						

			__asm
			{
				// Converte para floats 8 bytes da imagem e e coloca em b e c
				finit
				mov ebx, temp
				mov al, [ebx]
				mov red, eax
				fild red
				fstp b
				mov al, [ebx + 1]
				mov green, eax
				fild green
				fstp b[4]
				mov al, [ebx + 2]
				mov blue, eax
				fild blue
				fstp b[8]
				mov al, [ebx + 3]
				mov redt, eax
				fild redt
				fstp b[12]

				mov al, [ebx + 3 ] 
				mov red, eax
				fild red
				fstp c
				mov al, [ebx + 4]
				mov green, eax
				fild green
				fstp c[4]
				mov al, [ebx + 5]
				mov blue, eax
				fild blue
				fstp c[8]
				mov al, [ebx + 6]
				mov redt, eax
				fild redt
				fstp c[12]

				
				// Multiplica as cores das imagems pelos fatores
				lea eax, fatores
				lea ebx, b
				lea ecx, c
				movups xmm0, [eax]
				movups xmm1, [ebx]
				movups xmm2, [ecx]
				mulps xmm1, xmm0
				mulps xmm2, xmm0
				movups [ebx], xmm1
				movups [ecx], xmm2

			}



			imagem[j]	    = (unsigned char)b.x;
			imagem[j + 1] = (unsigned char)b.y;
			imagem[j + 2] = (unsigned char)b.z;
			imagem[j + 3] = (unsigned char)b.w;

			imagem[j + 3] = (unsigned char)c.x;
			imagem[j + 4] = (unsigned char)c.y;
			imagem[j + 5] = (unsigned char)c.z;
			imagem[j + 6] = (unsigned char)c.w;

			imagem[j] += imagem[j + 1] + imagem[j + 2];
			imagem[j + 1] = imagem[j];
			imagem[j + 2] = imagem[j];

			imagem[j + 3] += imagem[j + 4] + imagem[j + 5];
			imagem[j + 4] = imagem[j + 3];
			imagem[j + 5] = imagem[j + 3];

			__asm
			{
				mov eax, temp
				movq mm0, [eax]
				movq mm3, soma
				movq mm4, subtracao
				paddusb mm0, mm3
				psubusb mm0, mm4


				mov eax, temp
				movq[eax], mm0

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

