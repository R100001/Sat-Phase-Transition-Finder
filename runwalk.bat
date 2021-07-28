cd tests
FOR /L %%i IN (1500, 1, 3499) DO (
	cd M%%i
	FOR /L %%j IN (1, 1, 4) DO (
		IF EXIST resultsM%%i_%%j.txt (
			FOR /L %%k IN (1, 1, 5) DO (
				..\..\bcsp_solve walksat testM%%i_%%j.txt resultswalkM%%i_%%j_%%k.txt >> resultswalkM%%i.txt
			)
		)
	)
	cd ..
)