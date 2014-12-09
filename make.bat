if not exist out mkdir out
cl sepia_filtro.c

for /f %%f in ('dir /b in\') do sepia_filtro %%f
