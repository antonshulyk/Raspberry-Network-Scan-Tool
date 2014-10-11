
RPI_OBJS = rpicore_main.o rpicore_debug.o rpicore_util.o rpicore_curl.o rpicore_mutex.o rpicore_cmdlist.o rpicore_register.o rpicore_report.o rpicore_uptlist.o

LIBS = -ljson-c -luci -lcurl -lpthread -lm `pkg-config --cflags --libs glib-2.0`

override CFLAGS +=--std=gnu99 -Wall -pedantic-errors -Wextra -Wformat-nonliteral -Wformat-security -Winit-self \
		 -Wswitch-default -Wunused-parameter -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn \
		 -Wundef -Wunsafe-loop-optimizations -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings  -Wconversion \
		 -Wjump-misses-init -Wlogical-op -Wdouble-promotion

rpi-new: $(RPI_OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $(CFLAGS) -o $@ $(RPI_OBJS)

clean:
	rm -rf $(RPI_OBJS)
