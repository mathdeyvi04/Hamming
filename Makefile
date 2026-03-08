all:
	g++ -O3 -march=native src/main.cpp -o main

without_codif:
	g++ -O3 -march=native -DWITHOUT_CODIF src/main.cpp -o main_without_codif

debug:
	g++ -g3 -O0 -Wall -fno-inline src/main.cpp -o main

docs:
	@echo "\e[1;36m[INFO] Gerando HTML e LATEX com Doxygen\e[0m"
	@doxygen Doxyfile
	@echo "\e[1;36m[INFO] Compilando PDF na pasta docs/latex\e[0m"
	@$(MAKE) -C docs/latex
	@echo "\e[1;36m[INFO] Trazendo PDF para diretório padrão\e[0m"
	@mv docs/latex/refman.pdf Documentation.pdf

.PHONY: docs