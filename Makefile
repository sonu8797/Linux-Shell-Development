run: fp sb shell file_process.cpp squashbug.cpp main.cpp
fp:
	g++ file_process.cpp -o fp
sb:
	g++ squashbug.cpp -o sb
shell:
	g++ main.cpp -o shell -lreadline

clean:
	rm -f sb fp shell