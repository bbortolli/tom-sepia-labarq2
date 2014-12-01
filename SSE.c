#include <stdio.h>
#include <time.h>
#include <xmmintrin.h>

// referencia: [1]
struct fatores
{
	float x, y, z, w;


};

int main(int argc, char** argv)
{
	clock_t start, end;
	double cpu_time_used;
	char formato[5], info[24];
	unsigned char * imagem, *temp;
	//float fatores[4] = { 0.3, 0.59, 0.11, 1.0 };
	int media, red, green, blue, j, *temp2, *temp3, soma[4] = { 40, 20, 0, 0 }, subtracao[4] = { 0, 0, 20, 0 },* temp4;
	FILE *file;
	long int tamanhoFile, tamanhoImg;
	char inputtFileDir[64] = "in\\", outputFileDir[64] = "out\\";
	struct fatores a;

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


	//Calculos para o tom de sepia sequencial
	/*
	for (j = 0; j < tamanhoImg - 1; j += 3)
	{
		// media = 0.3*r + 0.59*g + 0.11*b
		// r = media + 40
		// g = media + 20
		// b = media - 20

		red = (int)imagem[j];
		green = (int)imagem[j + 1];
		blue = (int)imagem[j + 2];
		__asm
		{
			// obtendo 0.3*red
			mov eax, red
				mov ebx, 3
				mul ebx
				mov ebx, 10
				mov edx, 0
				div ebx
				mov red, eax

				// obtendo 0.59*g
				mov eax, green
				mov ebx, 59
				mul ebx
				mov ebx, 100
				mov edx, 0
				div ebx
				mov green, eax

				// obtendo 0.11*b
				mov eax, blue
				mov ebx, 11
				mul ebx
				mov ebx, 100
				mov edx, 0
				div ebx

				// ja com 0.11*b em eax, adicionamos 0.59*g e 0.3*red obtendo a media
				add eax, green
				add eax, red
				mov media, eax

				// Calculos nos canais de cores para o tom sepia
				mov eax, media
				add eax, 40
				cmp eax, 255
				ja SET_R_FF
				mov red, eax
				jmp GREEN
			SET_R_FF :
			mov eax, 255
				mov red, eax
			GREEN :
			mov eax, media
				add eax, 20
				cmp eax, 255
				ja SET_G_FF
				mov green, eax
				jmp BLUE
			SET_G_FF :
			mov eax, 255
				mov green, eax
			BLUE :
			mov eax, media
				sub eax, 20
				cmp eax, 255
				ja SET_0_B
				mov blue, eax
				jmp FIM
			SET_0_B :
			mov eax, 0
				mov blue, eax
			FIM :
		}
		imagem[j] = red;
		imagem[j + 1] = green;
		imagem[j + 2] = blue;
	}
	*/
	//Calculos para o tom sepia paralelo (SSE2)
	
	for (j = 0; j < tamanhoImg - 1; j += 3)
	{
		// media = 0.3*r + 0.59*g + 0.11*b
		// r = media + 40
		// g = media + 20
		// b = media - 20

		//printf("%d\n", j);
		temp = &imagem[j];
		temp2 = soma;
		temp3 = subtracao;
		//temp4 = fatores;
		red = (int)imagem[j + 3];
		/*printf("%x\n", temp);
		printf("Antes:\n");
		printf("%d\n", imagem[j]);
		printf("%d\n", imagem[j + 1]);
		printf("%d\n", imagem[j + 2]);
		printf("%d\n", imagem[j + 3]);
		printf("%d\n", imagem[j + 4]);
		printf("%d\n", imagem[j + 5]);
		printf("%d\n", imagem[j + 6]);
		printf("%d\n", imagem[j + 7]);
		printf("%d\n", imagem[j + 8]);
		*/

		__asm
		{
			mov eax, temp
			lea ebx, a
			movd xmm0, [eax]
			movups xmm1, [a]
			pxor xmm2, xmm2 //zera mm2
			//punpcklbw xmm0, xmm2
			pmullw xmm0, xmm1
			lea eax, soma
			lea ebx, subtracao
			movd xmm3, eax
			movd xmm4, ebx
			paddw xmm0, xmm2
			psubw xmm0, xmm2

			//packuswb xmm0, xmm2


			mov eax, temp
			movd [eax],xmm0
			




		}
		imagem[j + 3] = red;
		/*printf("Depois:\n");
		printf("%d\n", imagem[j]);
		printf("%d\n", imagem[j + 1]);
		printf("%d\n", imagem[j + 2]);
		printf("%d\n", imagem[j + 3]);
		printf("%d\n", imagem[j + 4]);
		printf("%d\n", imagem[j + 5]);
		printf("%d\n", imagem[j + 6]);
		printf("%d\n", imagem[j + 7]);
		printf("%d\n\n", imagem[j + 8]);
		*/

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
	fprintf(stderr, "tempo = %f segundos\n", cpu_time_used);

	return 0;
}


/*
Referencias:
[1]: http://neilkemp.us/src/sse_tutorial/sse_tutorial.html

*/
