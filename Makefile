CC65 = cc65
CA65 = ca65
LD65 = ld65
NAME = wow
CFG = nes.cfg

all: $(NAME).nes 

$(NAME).nes: $(NAME).o crt0.o runtime.lib $(CFG)
	$(LD65) -C $(CFG) -o $(NAME).nes crt0.o $(NAME).o runtime.lib
	@echo $(NAME).nes created

crt0.o: crt0.s
	$(CA65) crt0.s

$(NAME).o: $(NAME).s
	$(CA65) $(NAME).s

$(NAME).s: $(NAME).c
	$(CC65) -Oi $(NAME).c --add-source

clean:
	rm -f $(NAME).nes *.o $(NAME).s 

