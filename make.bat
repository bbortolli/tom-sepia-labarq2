if not exist out mkdir out
cl sepia_final.c

for /f %%f in ('dir /b in\') do sepia_final %%f
