mkdir tests
FOR /L %%i IN (1500, 1, 3499) DO (
	gcc.exe -c bcsp_generate.c -o bcsp_generate.o
	gcc.exe -o bcsp_generate.exe bcsp_generate.o
	cd tests
	mkdir M%%i
	cd M%%i
	..\..\bcsp_generate testM%%i 1 4
	FOR /L %%j IN (1, 1, 4) DO ..\..\bcsp_solve dpll testM%%i_%%j.txt resultsM%%i_%%j.txt >> resultsM%%i.txt
	cd ..\..
	increaseM.py
)
cd ..\..
runwalk