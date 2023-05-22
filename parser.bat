flex -i -Pcfg cfglex.l
mv lex.cfg.c lex.cfg.cpp
bison -d -v -p cfg cfgparse.y
mv cfgparse.tab.c cfgparse.tab.cpp
