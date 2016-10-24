platform_supervisor: platform_supervisor.c platform_supervisor.h socket.c socket.h control_keys.c control_keys.h
	gcc -o platform_supervisor platform_supervisor.c socket.c control_keys.c -lncurses

	sudo chown root platform_supervisor
	sudo chmod ug+s platform_supervisor